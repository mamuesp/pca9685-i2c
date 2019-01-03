load('api_pwm.js');
load('api_config.js');

let PCA9685 = {
   getGlobal: ffi('void *mgos_pca9685_get_global(void)'),
   reset: ffi('int mgos_pca9685_reset(void*)'),
   setFreq: ffi('int mgos_pca9685_set_freq(void*, double)'),
   setPWM: ffi('void mgos_pca9685_set_pwm(void*, int, int, int)'),
   getPWM: ffi('int mgos_pca9685_get_pwm(void*, int)'),
   setPin: ffi('void mgos_pca9685_set_pin(void*, int, int, int)')
};