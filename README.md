# Projeto Pegasus

**Projeto Pegasus** é um projeto para restaurar carros de controle remoto antigos da Estrela (modelos Pegasus, Colossus, Maximus), equipando-os com Arduino, comunicação RF (nRF24L01+), e, futuramente, integração BLE para controle via dispositivos Android.

## Funcionalidades
- **Controle via Arduino**: Código desenvolvido para transmissor e receptor, possibilitando direção, aceleração e reversão.
- **Comunicação RF com nRF24L01+**: Comunicação bidirecional entre transmissor e receptor.
- **Automação de Sinais Luminosos**: Configuração de LEDs para faróis, lanternas e setas.
- **Modos de Motor**: Suporte para motores DC e brushless, com implementações de frenagem e reversão.
- **Bluetooth BLE**: Controle opcional via BLE com dispositivos Android (futuramente, fase 2)
- 
## Requisitos de Hardware
- Arduino Pro Mini (ou AtMega328 compatível)
- Módulo RF nRF24L01+
- Controlador de motor (L298N para DC, ESC para brushless)
- Potenciômetros (para controle de direção e aceleração)
- Servos e motores
- Módulo Bluetooth HM-10 (futuramente, fase 2)
- 
## Primeiros Passos

1. **Clonar o Repositório**
   ```bash
   git clone https://github.com/museu-oldbits/Projeto-Pegasus.git
