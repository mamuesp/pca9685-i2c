#include "mgos_pca9685_i2c.h"

mgos_pca9685* mgos_pca9685_create(uint8_t addr)
{
    mgos_pca9685* newPwm;

    newPwm = calloc(1, sizeof(mgos_pca9685));
    newPwm->addr = addr;
    newPwm->i2c = mgos_i2c_get_global();

    return newPwm;
}

bool mgos_pca9685_free(mgos_pca9685* pwm)
{
    memset(pwm, 0, sizeof(mgos_pca9685));
    free(pwm);
    pwm = NULL;
    return true;
}

mgos_pca9685* mgos_pca9685_get_global()
{
    return globalPwm;
}

bool mgos_pca9685_write8(mgos_pca9685* pwm, uint8_t address, uint8_t data)
{
    char cmd[2];
    cmd[0] = address;
    cmd[1] = data;
    return mgos_i2c_write(pwm->i2c, pwm->addr, cmd, 2, true);
}

uint8_t mgos_pca9685_read8(mgos_pca9685* pwm, char address)
{
    uint8_t rtn;
    mgos_i2c_write(pwm->i2c, pwm->addr, &address, 1, true);
    mgos_i2c_read(pwm->i2c, pwm->addr, &rtn, 1, true);
    return rtn;
}

bool mgos_pca9685_reset(mgos_pca9685* pwm)
{
    bool result;
    result = mgos_pca9685_write8(pwm, MODE1, 0x0);
    LOG(LL_DEBUG, ("mgos_pca9685_reset: result <%s>", result ? "TRUE" : "FALSE"));
    return result;
}

int mgos_pca9685_set_freq(mgos_pca9685* pwm, float freq)
{

    freq *= 0.9; // Correct for some overflow in the frequency setting.
    float prescale = floor((((CLOCK_FREQ / 4096) / freq)) + 0.5) - 1;
    LOG(LL_DEBUG, ("mgos_pca9685 - Final pre-scale: %f", prescale));

    uint8_t oldMode = mgos_pca9685_read8(pwm, MODE1);
    uint8_t newMode = (oldMode & 0x7F) | 0x10; // sleep
    mgos_pca9685_write8(pwm, MODE1, newMode);
    mgos_msleep(5);
    mgos_pca9685_write8(pwm, PRE_SCALE, prescale);
    mgos_pca9685_write8(pwm, MODE1, oldMode);
    mgos_msleep(5);
    mgos_pca9685_write8(pwm, MODE1, oldMode | 0xa1);

    return prescale;
}

void mgos_pca9685_set_pwm(mgos_pca9685* pwm, uint8_t num, uint16_t on, uint16_t off)
{
    uint8_t cmd[5];

    cmd[0] = LED0_ON_L + 4 * num;
    cmd[1] = on;
    cmd[2] = on >> 8;
    cmd[3] = off;
    cmd[4] = off >> 8;

    mgos_i2c_write(pwm->i2c, pwm->addr, cmd, 5, true);
    LOG(LL_DEBUG, ("mgos_pca9685 - Setting PWM %d: 'on' - %d, 'off' - %d", num, on, off));
}

uint32_t mgos_pca9685_get_pwm(mgos_pca9685* pwm, uint8_t num)
{
    uint16_t on, off;

    on = mgos_pca9685_read8(pwm, LED0_ON_H + (num << 2));
    on = (on & 0xf) << 8;
    on += mgos_pca9685_read8(pwm, LED0_ON_L + (num << 2));

    off = mgos_pca9685_read8(pwm, LED0_OFF_H + (num << 2));
    off = (off & 0xf) << 8;
    off += mgos_pca9685_read8(pwm, LED0_OFF_L + (num << 2));

    return (on << 16) + off;
}

void mgos_pca9685_set_pin(mgos_pca9685* pwm, uint8_t pin, uint16_t val, bool invert)
{
    uint16_t on, off;
    val = MIN(val, 0x1000);
    switch (val) {
    case 0:
        on = invert ? 0x1000 : 0;
        off = invert ? 0 : 0x1000;
        break;
    case 4096:
        on = invert ? 0 : 0x1000;
        off = invert ? 0x1000 : 0;
        break;
    default:
        on = 0;
        off = invert ? 0x0FFF - val : val;
        break;
    }
    mgos_pca9685_set_pwm(pwm, pin, on, off);
}

bool mgos_pca9685_i2c_init(void)
{
    if (mgos_sys_config_get_pca9685_enable()) {
        int addr = mgos_sys_config_get_pca9685_addr();
        if (addr != 0) {
          globalPwm = mgos_pca9685_create(addr);
          mgos_pca9685_reset(globalPwm);
        } else {
            LOG(LL_ERROR, ("mgos_pca9685 - I2C address is not configured correctly!"));
        }
    }
    return true;
}
