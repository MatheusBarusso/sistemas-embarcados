#include "GamecubeReader.h"

GamecubeReader::GamecubeReader(uint8_t pin) {
    dataPin = pin;
    // Pega a velocidade do ESP32-S3 (geralmente 240MHz) para calcular os microssegundos
    cpuFreqMHz = ESP.getCpuFreqMHz(); 
}

void GamecubeReader::begin() {
    pinMode(dataPin, INPUT_PULLUP);
}

// Uma função nativa do ESP32 para contar ciclos do processador com precisão absoluta
void IRAM_ATTR GamecubeReader::delayCycles(uint32_t cycles) {
    uint32_t start = xthal_get_ccount();
    while (xthal_get_ccount() - start < cycles);
}

void IRAM_ATTR GamecubeReader::sendBit(bool bit) {
    pinMode(dataPin, OUTPUT);
    digitalWrite(dataPin, LOW);
    
    // O protocolo do Gamecube usa 4 microssegundos por bit.
    if (bit) {
        delayCycles(1 * cpuFreqMHz); // 1us LOW
        digitalWrite(dataPin, HIGH);
        delayCycles(3 * cpuFreqMHz); // 3us HIGH
    } else {
        delayCycles(3 * cpuFreqMHz); // 3us LOW
        digitalWrite(dataPin, HIGH);
        delayCycles(1 * cpuFreqMHz); // 1us HIGH
    }
}

void IRAM_ATTR GamecubeReader::sendByte(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        sendBit((byte >> i) & 1);
    }
}

bool IRAM_ATTR GamecubeReader::readBit() {
    uint32_t timeout = xthal_get_ccount() + (10 * cpuFreqMHz); // 10us timeout
    
    // Espera o fio descer (LOW)
    while (digitalRead(dataPin) == HIGH) {
        if (xthal_get_ccount() > timeout) return false;
    }
    
    // Quando desce, espera 2 microssegundos e lê o valor
    delayCycles(2 * cpuFreqMHz);
    bool bit = digitalRead(dataPin);
    
    // Espera o fio subir de volta (HIGH)
    timeout = xthal_get_ccount() + (10 * cpuFreqMHz);
    while (digitalRead(dataPin) == LOW) {
        if (xthal_get_ccount() > timeout) break;
    }
    
    return bit;
}

bool IRAM_ATTR GamecubeReader::update() {
    uint8_t response[8] = {0};
    
    // Pausa outras tarefas do ESP32 (como Wi-Fi) para não interromper o tempo crítico do controle
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);
    
    // 1. Envia o comando "Me dê os dados" (0x40, 0x03, 0x00)
    sendByte(0x40);
    sendByte(0x03);
    sendByte(0x00);
    sendBit(true); // Stop bit
    
    // 2. Prepara para escutar a resposta
    pinMode(dataPin, INPUT_PULLUP);
    
    // Espera um tempinho para o controle processar
    delayCycles(5 * cpuFreqMHz);
    
    // 3. Lê os 64 bits (8 bytes) de resposta do controle
    for (int i = 0; i < 8; i++) {
        for (int b = 7; b >= 0; b--) {
            if (readBit()) {
                response[i] |= (1 << b);
            }
        }
    }
    
    portEXIT_CRITICAL(&mux);

    // Se o primeiro byte de resposta for todo 0 (ou todo 1), o controle não está conectado
    if (response[0] == 0x00 || response[0] == 0xFF) return false;

    // 4. Mapeia a resposta bruta para nossa estrutura amigável
    report.start  = response[0] & 0x10;
    report.y      = response[0] & 0x08;
    report.x      = response[0] & 0x04;
    report.b      = response[0] & 0x02;
    report.a      = response[0] & 0x01;
    
    report.l      = response[1] & 0x40;
    report.r      = response[1] & 0x20;
    report.z      = response[1] & 0x10;
    report.dUp    = response[1] & 0x08;
    report.dDown  = response[1] & 0x04;
    report.dRight = response[1] & 0x02;
    report.dLeft  = response[1] & 0x01;
    
    report.xAxis       = response[2];
    report.yAxis       = response[3];
    report.cxAxis      = response[4];
    report.cyAxis      = response[5];
    report.leftAnalog  = response[6];
    report.rightAnalog = response[7];

    return true;
}

void GamecubeReader::debugPrint() {
    Serial.printf("A:%d B:%d X:%d Y:%d START:%d | Stick X:%3d Y:%3d | L:%3d R:%3d\n",
        report.a, report.b, report.x, report.y, report.start,
        report.xAxis, report.yAxis, report.leftAnalog, report.rightAnalog);
}