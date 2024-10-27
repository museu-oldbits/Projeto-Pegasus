#include <EEPROM.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

// Pinagem e definições
#define SERVO_PIN 9
#define MOTOR_PIN_FWD 5
#define MOTOR_PIN_REV 6
#define LED_PIN 7
#define TURN_SIGNAL_LEFT 3
#define TURN_SIGNAL_RIGHT 4
#define WATCHDOG_PIN 2
#define BRAKE_PIN 8

// Configurações do nRF24L01
RF24 radio(9, 10); // CE, CSN
uint8_t address[6] = "00001"; // Endereço como array de bytes
bool connected = false;
int throttle = 0;
int steering = 0;
int brake = 0;
Servo steeringServo;

// Funções de controle do motor e servo
void applySteering(int direction) {
  steeringServo.write(direction);
}

void applyThrottle(int speed) {
  if (speed > 0) {
    digitalWrite(MOTOR_PIN_FWD, HIGH);
    digitalWrite(MOTOR_PIN_REV, LOW);
  } else if (speed < 0) {
    digitalWrite(MOTOR_PIN_FWD, LOW);
    digitalWrite(MOTOR_PIN_REV, HIGH);
  } else {
    // Freio motor
    digitalWrite(MOTOR_PIN_FWD, LOW);
    digitalWrite(MOTOR_PIN_REV, LOW);
    digitalWrite(BRAKE_PIN, HIGH); // Aplica o freio
  }
}

// Comunicação serial para configuração do endereço nRF24
void setAddress(const char* newAddress) {
  for (int i = 0; i < 5; i++) {
    address[i] = newAddress[i]; // Atualiza o array de bytes
  }
  radio.openReadingPipe(0, address); // Atualiza o pipe de leitura
  saveAddressToEEPROM(); // Salva na EEPROM
}

// Função para salvar o endereço na EEPROM
void saveAddressToEEPROM() {
  for (int i = 0; i < 5; i++) {
    EEPROM.write(i, address[i]);
  }
}

// Função para carregar o endereço da EEPROM
void loadAddressFromEEPROM() {
  for (int i = 0; i < 5; i++) {
    address[i] = EEPROM.read(i); // Carrega cada byte do endereço
  }
  radio.openReadingPipe(0, address); // Abre o pipe de leitura
}

// Configuração inicial
void setup() {
  Serial.begin(9600);
  
  // Inicia o nRF24L01+
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
  
  // Configura os pinos de saída
  pinMode(LED_PIN, OUTPUT);
  pinMode(TURN_SIGNAL_LEFT, OUTPUT);
  pinMode(TURN_SIGNAL_RIGHT, OUTPUT);
  pinMode(MOTOR_PIN_FWD, OUTPUT);
  pinMode(MOTOR_PIN_REV, OUTPUT);
  pinMode(BRAKE_PIN, OUTPUT);
  pinMode(WATCHDOG_PIN, INPUT);

  // Inicia o servo de direção
  steeringServo.attach(SERVO_PIN);
  steeringServo.write(90); // Posição neutra

  // Carrega o endereço da EEPROM
  loadAddressFromEEPROM();

  // Verifica se o watchdog está conectado
  if (digitalRead(WATCHDOG_PIN) == LOW) {
    connected = false;
  } else {
    connected = true;
    digitalWrite(LED_PIN, HIGH); // Mantém faróis acesos
  }
}

// Loop principal
void loop() {
  if (Serial.available() > 0) {
    // Configuração do endereço via comunicação serial
    Serial.println("Digite o novo endereço (5 caracteres): ");
    char newAddress[6];
    Serial.readBytes(newAddress, 5);
    setAddress(newAddress);
    Serial.print("Endereço configurado para: ");
    Serial.println(newAddress);
  }
  
  if (radio.available()) {
    int data[2]; // Array para armazenar dados recebidos
    radio.read(&data, sizeof(data)); // Lê dados do transmissor
    
    // Atualiza valores de throttle e steering
    throttle = data[0];
    steering = data[1];
    
    applyThrottle(throttle);
    applySteering(steering);

    // Lógica para piscar as setas
    if (steering < -10) {
      digitalWrite(TURN_SIGNAL_LEFT, HIGH);
      digitalWrite(TURN_SIGNAL_RIGHT, LOW);
    } else if (steering > 10) {
      digitalWrite(TURN_SIGNAL_LEFT, LOW);
      digitalWrite(TURN_SIGNAL_RIGHT, HIGH);
    } else {
      digitalWrite(TURN_SIGNAL_LEFT, LOW);
      digitalWrite(TURN_SIGNAL_RIGHT, LOW);
    }
  } else {
    // Se não houver comunicação, ativar o modo "faróis piscando"
    connected = false;
    digitalWrite(LED_PIN, millis() % 1000 < 500 ? HIGH : LOW); // Pisca os faróis a cada 0.5Hz
  }

  // Verifica o status do watchdog
  if (digitalRead(WATCHDOG_PIN) == LOW) {
    connected = false;
  } else {
    connected = true;
    digitalWrite(LED_PIN, HIGH); // Faróis acesos
  }
}
