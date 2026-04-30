#include <Arduino.h>

// MUDE AQUI SE VOCÊ TROCOU O PINO NAS TENTATIVAS ANTERIORES
#define DAT_PIN 10 

uint32_t cpuFreqMHz;

// Funções auxiliares pra rodar soltas no main
void delayCycles(uint32_t cycles) {
    uint32_t start = xthal_get_ccount();
    while (xthal_get_ccount() - start < cycles);
}

void sendBit(bool bit) {
    pinMode(DAT_PIN, OUTPUT);
    digitalWrite(DAT_PIN, LOW);
    if (bit) {
        delayCycles(1 * cpuFreqMHz);
        digitalWrite(DAT_PIN, HIGH);
        delayCycles(3 * cpuFreqMHz);
    } else {
        delayCycles(3 * cpuFreqMHz);
        digitalWrite(DAT_PIN, HIGH);
        delayCycles(1 * cpuFreqMHz);
    }
}

void sendByte(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        sendBit((byte >> i) & 1);
    }
}

void setup() {
    Serial.begin(115200);
    cpuFreqMHz = ESP.getCpuFreqMHz();
    pinMode(DAT_PIN, INPUT_PULLUP);
    delay(2000);
    Serial.println("\nINICIANDO SCANNER DE HARDWARE...");
}

void loop() {
    Serial.println("\n=========================================");
    
    // 1. CHECAGEM DE CURTO-CIRCUITO E PULL-UP
    int estadoRepouso = digitalRead(DAT_PIN);
    Serial.print("1. Estado do pino em repouso: ");
    Serial.print(estadoRepouso);
    
    if (estadoRepouso == 0) {
        Serial.println(" -> ERRO CRITICO!");
        Serial.println("O fio DAT esta em 0V. Ele deveria estar em 3.3V (1).");
        Serial.println("Causas: Pino errado no ESP32, fio em curto com o GND, ou controle nao esta recebendo 3.3V.");
    } else {
        Serial.println(" -> OK! (Pull-up funcionando)");
    }

    // 2. CHECAGEM DE COMUNICACAO
    // Desliga interrupcoes para o ESP32 nao se distrair
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);
    
    // Grita para o controle: "Me de seus dados!" (0x40 0x03 0x00)
    sendByte(0x40);
    sendByte(0x03);
    sendByte(0x00);
    sendBit(true); // Stop bit
    
    // Prepara para escutar
    pinMode(DAT_PIN, INPUT_PULLUP);
    delayCycles(5 * cpuFreqMHz);

    // Grava os proximos 1000 micro-estados do fio na velocidade da luz
    uint8_t leituras[1000];
    for(int i = 0; i < 1000; i++) {
        leituras[i] = digitalRead(DAT_PIN);
        delayCycles(5); 
    }
    
    portEXIT_CRITICAL(&mux);

    // 3. ANALISA O QUE CHEGOU
    int mudancasDeEstado = 0;
    for(int i = 1; i < 1000; i++) {
        if (leituras[i] != leituras[i-1]) {
            mudancasDeEstado++;
        }
    }

    Serial.print("2. Pulsos de resposta detectados: ");
    Serial.println(mudancasDeEstado);

    if (mudancasDeEstado == 0) {
        Serial.println("-> DIAGNOSTICO: O controle esta morto ou mudo.");
        Serial.println("   Ele escutou, mas nao mandou um unico pulso de volta.");
        Serial.println("   Solucoes: Troque o GPIO no codigo (tente o 4 ou 5) ou coloque o resistor.");
    } else if (mudancasDeEstado > 10) {
        Serial.println("-> DIAGNOSTICO: SUCESSO DE HARDWARE!");
        Serial.println("   O controle ESTA falando com o ESP32 perfeitamente.");
        Serial.println("   Se a biblioteca antiga falhou, o problema é apenas um ajuste fino nos micro-segundos.");
    } else {
        Serial.println("-> DIAGNOSTICO: RUIDO MAGNÉTICO.");
        Serial.println("   Captou alguma sujeira, mas nao eh um pacote de dados real.");
    }

    delay(3000);
}