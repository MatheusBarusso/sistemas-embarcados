#include "NGCController.h"

#if defined(ARDUINO_ARCH_ESP32)
#include "soc/gpio_struct.h"
#include "xtensa/core-macros.h"
#include <esp32-hal-cpu.h>
#endif

NGCController::NGCController(uint8_t dataPin)
    : _dataPin(dataPin), _cpuFreqMHz(240) {}

void NGCController::begin() {
#if defined(ARDUINO_ARCH_ESP32)
    _cpuFreqMHz = ESP.getCpuFreqMHz();
    pinMode(_dataPin, OUTPUT_OPEN_DRAIN);
    GPIO.out_w1ts = (1UL << _dataPin);
#else
    pinMode(_dataPin, INPUT_PULLUP);
#endif
}

inline void NGCController::delayCycles(uint32_t cycles) {
#if defined(ARDUINO_ARCH_ESP32)
    uint32_t start = xthal_get_ccount();
    while (xthal_get_ccount() - start < cycles) {}
#else
    (void)cycles;
    delayMicroseconds(1);
#endif
}

inline void NGCController::sendBit(bool bit) {
#if defined(ARDUINO_ARCH_ESP32)
    if (bit) {
        GPIO.out_w1tc = (1UL << _dataPin);
        delayCycles(1 * _cpuFreqMHz);
        GPIO.out_w1ts = (1UL << _dataPin);
        delayCycles(3 * _cpuFreqMHz);
    } else {
        GPIO.out_w1tc = (1UL << _dataPin);
        delayCycles(3 * _cpuFreqMHz);
        GPIO.out_w1ts = (1UL << _dataPin);
        delayCycles(1 * _cpuFreqMHz);
    }
#else
    (void)bit;
#endif
}

void NGCController::sendByte(uint8_t b) {
    for (int i = 7; i >= 0; i--) {
        sendBit((b >> i) & 0x01);
    }
}

int NGCController::readBit() {
#if defined(ARDUINO_ARCH_ESP32)
    uint32_t t_timeout = _cpuFreqMHz * 8;
    uint32_t start_wait = xthal_get_ccount();

    while (((GPIO.in >> _dataPin) & 0x01) == 1) {
        if (xthal_get_ccount() - start_wait > t_timeout) return -1;
    }

    uint32_t start_low = xthal_get_ccount();

    while (((GPIO.in >> _dataPin) & 0x01) == 0) {
        if (xthal_get_ccount() - start_low > t_timeout) return -1;
    }

    uint32_t lowTime = xthal_get_ccount() - start_low;
    return (lowTime < (_cpuFreqMHz * 2)) ? 1 : 0;
#else
    return -1;
#endif
}

ControllerState NGCController::read() {
    ControllerState state = {0, 0, 0, 0, 0, 0, 0, 0};

    static int16_t fx = 0, fy = 0, fz = 0, frz = 0, frx = 0, fry = 0;
    static int16_t last_fx = 0, last_fy = 0, last_frx = 0, last_fry = 0;
    static int16_t last_fz = 0, last_frz = 0;
    const int16_t HYST = 3;

#if defined(ARDUINO_ARCH_ESP32)
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);

    sendByte(0x40);
    sendByte(0x03);
    sendByte(0x00);
    sendBit(1);
    GPIO.out_w1ts = (1UL << _dataPin);

    uint8_t response[8] = {0};
    bool ok = true;

    for (int byte_i = 0; byte_i < 8 && ok; byte_i++) {
        for (int bit_i = 7; bit_i >= 0; bit_i--) {
            int b = readBit();
            if (b == -1) {
                ok = false;
                break;
            }
            response[byte_i] |= (b << bit_i);
        }
    }

    portEXIT_CRITICAL(&mux);

    if (!ok) {
        state.x  = last_fx;
        state.y  = last_fy;
        state.rx = last_frx;
        state.ry = last_fry;
        state.z  = last_fz;
        state.rz = last_frz;
        return state;
    }

    bool all_ff = true;
    for (int i = 0; i < 8; i++) {
        if (response[i] != 0xFF) { all_ff = false; break; }
    }
    if (all_ff) return state;

    if (btnPressed(response[0], 0)) state.buttons |= (1UL << 0); // A
    if (btnPressed(response[0], 1)) state.buttons |= (1UL << 1); // B
    if (btnPressed(response[0], 2)) state.buttons |= (1UL << 2); // X
    if (btnPressed(response[0], 3)) state.buttons |= (1UL << 3); // Y
    if (btnPressed(response[0], 4)) state.buttons |= (1UL << 7); // Start
    if (btnPressed(response[1], 4)) state.buttons |= (1UL << 4); // Z
    if (btnPressed(response[1], 6)) state.buttons |= (1UL << 5); // L digital
    if (btnPressed(response[1], 5)) state.buttons |= (1UL << 6); // R digital

    bool dpad_up    = btnPressed(response[1], 3);
    bool dpad_down  = btnPressed(response[1], 2);
    bool dpad_right = btnPressed(response[1], 1);
    bool dpad_left  = btnPressed(response[1], 0);

    if (dpad_up)                    state.hat = 1;
    if (dpad_down)                  state.hat = 5;
    if (dpad_left)                  state.hat = 7;
    if (dpad_right)                 state.hat = 3;
    if (dpad_up   && dpad_right)    state.hat = 2;
    if (dpad_down && dpad_right)    state.hat = 4;
    if (dpad_down && dpad_left)     state.hat = 6;
    if (dpad_up   && dpad_left)     state.hat = 8;

    int16_t lx = (int16_t)response[2] - 128;
    int16_t ly = (int16_t)response[3] - 128;
    int16_t cx = (int16_t)response[4] - 128;
    int16_t cy = (int16_t)response[5] - 128;
    int16_t lt = (int16_t)response[6] - 128;
    int16_t rt = (int16_t)response[7] - 128;

    auto applyDeadzone = [](int16_t v) -> int16_t {
        const int16_t dz = 12;
        if (v > -dz && v < dz) return 0;
        return (v > 0) ? (int16_t)((v - dz) * 128 / (128 - dz))
                       : (int16_t)((v + dz) * 128 / (128 - dz));
    };


    lx = applyDeadzone(lx);
    ly = applyDeadzone(ly);
    cx = applyDeadzone(cx);
    cy = applyDeadzone(cy);
    lt = applyDeadzone(lt);
    rt = applyDeadzone(rt);

    fx  = (fx  * 7 + lx) / 8;
    fy  = (fy  * 7 + ly) / 8;
    frx = (frx * 3 + cx) / 4;
    fry = (fry * 3 + cy) / 4;
    fz  = (fz  * 7 + lt) / 8;
    frz = (frz * 7 + rt) / 8;

    if (abs(fx  - last_fx)  > HYST) last_fx  = fx;
    if (abs(fy  - last_fy)  > HYST) last_fy  = fy;
    if (abs(frx - last_frx) > HYST) last_frx = frx;
    if (abs(fry - last_fry) > HYST) last_fry = fry;
    if (abs(fz  - last_fz)  > HYST) last_fz  = fz;
    if (abs(frz - last_frz) > HYST) last_frz = frz;

    state.x  = last_fx;
    state.y  = last_fy;

    state.rx = last_frx;
    state.ry = last_fry;

    state.z  = last_fz;
    state.rz = last_frz;

#endif

    return state;
}
