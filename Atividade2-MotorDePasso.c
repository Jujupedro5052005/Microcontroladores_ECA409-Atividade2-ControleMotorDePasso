#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main()
{
    stdio_init_all();

    // Inicializa o ADC
    adc_init();
    adc_gpio_init(26);

    while (true) {
        // Lê sinal analógico
        uint16_t raw = adc_read();
        // Converte sinal analógico em volts
        float volts = 3.3f/4095.0f*raw; // Tensão lida varia de 0 a 3.3 volts


    }
}
