#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"


int main()
{
    stdio_init_all();

    // Configuracoes do ADC
    adc_init();     // Habilita bloco ADC
    adc_gpio_init(26);      // Habilita o pino 26 (GPIO26 do pinout)
    adc_select_input(0);        // Configura porta do ADC no canal 0 (ADC0 do pinout)

    while (true) {
        uint16_t raw = adc_read();
        float v = raw * 3.3f /4095;
        // 3.3f é o fundo de escala analogico do ADC da pico (ate quanto a porta le)
        // 4095 eh o fundo de escala digital do ADC (12 bits)
        printf("%u,%.3f\n", raw, v);
        sleep_ms(1000);
    }
    return 0;
}
