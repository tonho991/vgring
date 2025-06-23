<p align="center">
   <img src="/docs/vgring_logo.png" width="200" height="200">
</p>

# 🔔 VGRING - Campainha Inteligente com ESP8266

VGRING é um projeto de campainha inteligente baseado em ESP8266 com painel de configuração via Wi-Fi, suporte a OTA (atualização remota) e integração com um servidor web seguro.


<img src="/docs/esp8266_project.jpg" width="400" height="250">

## 🚀 Funcionalidades

- Configuração de rede via Access Point
- Atualização de firmware OTA via URL segura
- LEDs de status (vermelho e verde)
- Botão de campainha e botão de reset
- Armazenamento de dados com LittleFS


---

## ⚡ Esquema de Pinos (NodeMCU ESP8266)

| Função           | Pino NodeMCU | GPIO Real | Observação                     |
|------------------|--------------|-----------|--------------------------------|
| LED Vermelho     | D6           | GPIO12    | Indica erro ou status negativo |
| LED Verde        | D7           | GPIO13    | Indica sucesso ou ok           |
| Botão Ring       | D5           | GPIO14    | Quando pressionado, envia toque |
| Botão Reset      | D1           | GPIO5     | Segurar por 8s para resetar     |

- Todos os pinos de botão devem estar conectados ao **GND** no acionamento (configurados com `INPUT_PULLUP`)
- LEDs são ativos em nível **HIGH** (acendem com `digitalWrite(pin, HIGH)`)

## Como usar

### 1. Pré-requisitos

- NodeMCU ESP8266 (ESP-12E)
- VS Code + PlatformIO
- Fonte de alimentação 5V
- Biblioteca instaladas via `platformio.ini`

### 2. Compilação e Upload

Conecte o ESP8266 ao computador e execute:

```sh
pio run --target upload
```

### 3. Upload do Sistema de Arquivos (LittleFS)

Para enviar os arquivos da pasta `data/` (ex: index.html):

```sh
pio run --target uploadfs
```

### 4. Configuração Inicial

1. Ao ligar o dispositivo pela primeira vez, ele cria uma rede Wi-Fi chamada **VGRing Connect**.
2. Conecte-se a essa rede e acesse o endereço IP exibido no monitor serial.
3. Siga as instruções da interface web para configurar o Wi-Fi.
4. Após a configuração, o dispositivo irá conectar à rede Wi-Fi selecionada.

### 5. Testes

Para rodar testes unitários (se houver):

```sh
pio test
```

## 🌐 API disponível

| Método | Endpoint           | Descrição                            |
|--------|--------------------|---------------------------------------|
| GET    | `/api/scan_wifi`   | Lista redes Wi-Fi disponíveis         |
| POST   | `/api/connect`     | Envia SSID e senha para conectar      |
| POST   | `/api/sign`        | Salva token do FCM para envio de notificações           |
| GET    | `/api/get_config`  | Retorna SSID, token e versão firmware |
| POST   | `/api/update_firmware`      | Inicia atualização OTA                |

---
