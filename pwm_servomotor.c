#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pico/stdio.h"

#define SERVO_GPIO 22   // Pino de controle do servomotor
#define LED_GPIO 12     // Pino do LED RGB
#define PWM_FREQ 50.0f  // Frequência de PWM (50Hz)
#define PWM_PERIOD_US 20000.0f  // Período de 20ms

// Configuração do PWM para servomotor
void setup_servo_pwm(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    float div = clock_get_hz(clk_sys) / (PWM_FREQ * 65535.0f);
    
    pwm_set_clkdiv(slice_num, div);
    pwm_set_wrap(slice_num, 65535);
    pwm_set_enabled(slice_num, true);
}

// Definir ângulo do servomotor com precisão
void set_servo_angle(uint gpio, float duty_us) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);
    
    // Cálculo preciso do nível de PWM
    uint16_t level = (duty_us * 65535.0f) / PWM_PERIOD_US;
    pwm_set_chan_level(slice_num, channel, level);
    
    // Log do movimento
    printf("Servo PWM: %.2f us (Level: %d)\n", duty_us, level);
}

int main() {
    // Inicialização dos periféricos
    stdio_init_all();
    gpio_init(LED_GPIO);
    gpio_set_dir(LED_GPIO, GPIO_OUT);
    
    // Configuração do PWM
    setup_servo_pwm(SERVO_GPIO);
    
    printf("Iniciando controle de servomotor...\n");
    
    while(1) {
        // Posição 180 graus
        gpio_put(LED_GPIO, 1);
        set_servo_angle(SERVO_GPIO, 2400);
        printf("Posição: 180 graus\n");
        sleep_ms(5000);
        
        // Posição 90 graus
        gpio_put(LED_GPIO, 0);
        set_servo_angle(SERVO_GPIO, 1470);
        printf("Posição: 90 graus\n");
        sleep_ms(5000);
        
        // Posição 0 graus
        gpio_put(LED_GPIO, 1);
        set_servo_angle(SERVO_GPIO, 500);
        printf("Posição: 0 graus\n");
        sleep_ms(5000);
        
        // Movimento suave 0 para 180 graus
        printf("Movimento suave: 0 -> 180 graus\n");
        for (int us = 500; us <= 2400; us += 5) {
            set_servo_angle(SERVO_GPIO, us);
            sleep_ms(10);
        }
        
        // Movimento suave 180 para 0 graus
        printf("Movimento suave: 180 -> 0 graus\n");
        for (int us = 2400; us >= 500; us -= 5) {
            set_servo_angle(SERVO_GPIO, us);
            sleep_ms(10);
        }
    }
    
    return 0;
}