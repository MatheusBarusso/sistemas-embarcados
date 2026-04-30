#include <Arduino.h>

// Data line pin
#define DAT_PIN 4

uint32_t cpuFreqMHz;

static inline bool btnPressed(uint8_t byte, uint8_t bit) {
    return (((byte >> bit) & 0x01) == 1);
}

void delayCycles(uint32_t cycles) {
    uint32_t start = xthal_get_ccount();
    while (xthal_get_ccount() - start < cycles);
}

void sendBit(bool bit) {
    if (bit) {
        GPIO.out_w1tc = (1 << DAT_PIN);
        delayCycles(1 * cpuFreqMHz);
        GPIO.out_w1ts = (1 << DAT_PIN);
        delayCycles(3 * cpuFreqMHz);
    } else {
        GPIO.out_w1tc = (1 << DAT_PIN);
        delayCycles(3 * cpuFreqMHz);
        GPIO.out_w1ts = (1 << DAT_PIN);
        delayCycles(1 * cpuFreqMHz);
    }
}

void sendByte(uint8_t b) {
    for (int i = 7; i >= 0; i--) {
        sendBit((b >> i) & 1);
    }
}

int readBit() {
    uint32_t t_timeout = cpuFreqMHz * 8;
    uint32_t start_wait = xthal_get_ccount();
    
    while (((GPIO.in >> DAT_PIN) & 1) == 1) {
        if (xthal_get_ccount() - start_wait > t_timeout) return -1;
    }
    
    uint32_t start_low = xthal_get_ccount();
    
    while (((GPIO.in >> DAT_PIN) & 1) == 0) {
        if (xthal_get_ccount() - start_low > t_timeout) return -1;
    }
    
    uint32_t tempo_low = xthal_get_ccount() - start_low;
    
    if (tempo_low < (cpuFreqMHz * 2)) {
        return 1;
    } else {
        return 0;
    }
}

void setup() {
    Serial.begin(115200);
    cpuFreqMHz = ESP.getCpuFreqMHz();
    
    // Open-drain data line
    pinMode(DAT_PIN, OUTPUT_OPEN_DRAIN);
    GPIO.out_w1ts = (1 << DAT_PIN);
    
    delay(2000);
    Serial.println("Iniciando varredura com auto-sincronia...");
}

void loop() {
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);

    sendByte(0x40);
    sendByte(0x03);
    sendByte(0x00);
    
    sendBit(1);
    
    GPIO.out_w1ts = (1 << DAT_PIN);

    uint8_t resposta[8] = {0};
    bool ok = true;
    
    for (int byte_i = 0; byte_i < 8 && ok; byte_i++) {
        for (int bit_i = 7; bit_i >= 0; bit_i--) {
            int b = readBit();
            if (b == -1) { 
                ok = false; 
                break; 
            }
            resposta[byte_i] |= (b << bit_i); 
        }
    }

    portEXIT_CRITICAL(&mux);

    if (!ok) {
        Serial.println("TIMEOUT: Sinal perdido durante a leitura.");
    } else {
        Serial.print("HEX: ");
        for (int i = 0; i < 8; i++) {
            Serial.printf("%02X ", resposta[i]);
        }
        
        bool all_ff = true;
        for (int i = 0; i < 8; i++) {
            if (resposta[i] != 0xFF) {
                all_ff = false;
                break;
            }
        }

        if (all_ff) {
            Serial.println(" | PACOTE INVALIDO (0xFF em todos bytes)");
            delay(16);
            return;
        }

        bool start_btn = btnPressed(resposta[0], 4);
        bool y_btn     = btnPressed(resposta[0], 3);
        bool x_btn     = btnPressed(resposta[0], 2);
        bool b_btn     = btnPressed(resposta[0], 1);
        bool a_btn     = btnPressed(resposta[0], 0);

        bool l_btn      = btnPressed(resposta[1], 6);
        bool r_btn      = btnPressed(resposta[1], 5);
        bool z_btn      = btnPressed(resposta[1], 4);
        bool dpad_up    = btnPressed(resposta[1], 3);
        bool dpad_down  = btnPressed(resposta[1], 2);
        bool dpad_right = btnPressed(resposta[1], 1);
        bool dpad_left  = btnPressed(resposta[1], 0);

        uint8_t stick_x  = resposta[2];
        uint8_t stick_y  = resposta[3];
        uint8_t cstick_x = resposta[4];
        uint8_t cstick_y = resposta[5];
        
        Serial.printf(
            " | BTN A:%d B:%d X:%d Y:%d Start:%d L:%d R:%d Z:%d D(U:%d D:%d R:%d L:%d)"
            " | ANA STK(%3u,%3u) CSTK(%3u,%3u)\n",
            a_btn, b_btn, x_btn, y_btn, start_btn,
            l_btn, r_btn, z_btn,
            dpad_up, dpad_down, dpad_right, dpad_left,
            stick_x, stick_y, cstick_x, cstick_y
        );
    }
    
    delay(16);
}
