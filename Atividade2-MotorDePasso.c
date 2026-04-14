#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

bool timer_cb(repeating_timer_t *t){
    adc_run(true);
    return true;
}

int main()
{
    stdio_init_all();

    // Inicilaiza os pinos
    gpio_init(IN1);
    gpio_set_dir(IN1, GPIO_OUT);
    gpio_init(IN2);
    gpio_set_dir(IN2, GPIO_OUT);
    gpio_init(IN3);
    gpio_set_dir(IN3, GPIO_OUT);
    gpio_init(IN4);
    gpio_set_dir(IN4, GPIO_OUT);

    // Inicializa o ADC
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    // Cria e define o timer
    repeating_timer_t timer;
    add_repeating_timer_ms(5,timer_cb, NULL, &timer);

    while (true) {
        // Lê sinal analógico
        uint16_t raw = adc_read();
        // Converte sinal analógico em volts
        float volts = 3.3f/4095.0f*raw; // Tensão lida varia de 0 a 3.3 volts


    }
}
