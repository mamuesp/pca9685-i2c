/*
 * Copyright 2018-2019 <Manfred Müller-Späth> fms1961@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MGOS_PCA9685_I2C_H
#define __MGOS_PCA9685_I2C_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mgos.h"
#include "mgos_config.h"
#include "mgos_gpio.h"
#include "mgos_i2c.h"

#define MODE1           0x00    // Mode register 1
#define MODE2           0x01    // Mode register 2
#define SUBADR1         0x02    // I2C-bus subaddress 1
#define SUBADR2         0x03    // I2C-bus subaddress 2
#define SUBADR3         0x04    // I2C-bus subaddress 3
#define ALLCALLADR      0x05    // LED All Call I2C-bus address
#define LED0            0x06    // LED0 start register
#define LED0_ON_L       0x06    // LED0 output and brightness control byte 0
#define LED0_ON_H       0x07    // LED0 output and brightness control byte 1
#define LED0_OFF_L      0x08    // LED0 output and brightness control byte 2
#define LED0_OFF_H      0x09    // LED0 output and brightness control byte 3
#define LED_MULTIPLIER  4       // For the other 15 channels
#define ALLLED_ON_L     0xFA    // load all the LEDn_ON registers, byte 0 (turn 0-7 channels on)
#define ALLLED_ON_H     0xFB    // load all the LEDn_ON registers, byte 1 (turn 8-15 channels on)
#define ALLLED_OFF_L    0xFC    // load all the LEDn_OFF registers, byte 0 (turn 0-7 channels off)
#define ALLLED_OFF_H    0xFD    // load all the LEDn_OFF registers, byte 1 (turn 8-15 channels off)
#define PRE_SCALE       0xFE    // prescaler for output frequency
#define CLOCK_FREQ      25000000.0 // 25MHz default osc clock

#define SWRST           0x06  /*!< software reset */

typedef struct {
  uint8_t addr;
  struct mgos_i2c *i2c;
} mgos_pca9685;

mgos_pca9685 *globalPwm;

mgos_pca9685 *mgos_pca9685_create(uint8_t addr);
bool mgos_pca9685_free(mgos_pca9685 *pwm);
mgos_pca9685 *mgos_pca9685_get_global();

bool mgos_pca9685_write8(mgos_pca9685 *pwm, uint8_t address, uint8_t data);
uint8_t mgos_pca9685_read8(mgos_pca9685 *pwm, char address);

bool mgos_pca9685_reset(mgos_pca9685 *pwm);
int mgos_pca9685_set_freq(mgos_pca9685 *pwm, float freq);
void mgos_pca9685_set_pwm(mgos_pca9685 *pwm, uint8_t num, uint16_t on, uint16_t off);
uint32_t mgos_pca9685_get_pwm(mgos_pca9685 *pwm, uint8_t num);
void mgos_pca9685_set_pin(mgos_pca9685 *pwm, uint8_t pin, uint16_t val, bool invert);

bool mgos_pca9685_i2c_init(void);

#endif // __MGOS_PCA9685_I2C_H
