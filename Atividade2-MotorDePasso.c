#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/adc.h"

#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5


// Funcao chamada pelo repeating timer
bool step_callback(struct repeating_timer *t){
    // variavel static retem o valor dela entre chamadas da funcao!!! (bem util)
    // como se fosse uma variavel global que so esta funcao pode acessar 
    // esse igual a false so funciona na criacao
    // em outras chamadas, ele apenas pega o valor anterior que ja estava
    static int step_state = 1;

    printf("%i\n", step_state);

    // Implementacao da sequencia em half step
    switch (step_state){
        case 1:
            gpio_put(IN1, true);
            gpio_put(IN2, false);
            gpio_put(IN3, false);
            gpio_put(IN4, false);
        case 2:
            gpio_put(IN1, true);
            gpio_put(IN2, true);
            gpio_put(IN3, false);
            gpio_put(IN4, false);
        case 3:
            gpio_put(IN1, false);
            gpio_put(IN2, true);
            gpio_put(IN3, false);
            gpio_put(IN4, false);
        case 4:
            gpio_put(IN1, false);
            gpio_put(IN2, true);
            gpio_put(IN3, true);
            gpio_put(IN4, false);
        case 5:
            gpio_put(IN1, false);
            gpio_put(IN2, false);
            gpio_put(IN3, true);
            gpio_put(IN4, false);
        case 6:
            gpio_put(IN1, false);
            gpio_put(IN2, false);
            gpio_put(IN3, true);
            gpio_put(IN4, true);
        case 7:
            gpio_put(IN1, false);
            gpio_put(IN2, false);
            gpio_put(IN3, false);
            gpio_put(IN4, true);
        case 8:
            gpio_put(IN1, true);
            gpio_put(IN2, false);
            gpio_put(IN3, false);
            gpio_put(IN4, true);
    }

    if (step_state == 8){
        step_state = 1;
    } 
    else {
        step_state++;
    }

    return true;
}


int main()
{
    stdio_init_all();

    // Inicializa GPIOs
    gpio_init(IN1);
    gpio_set_dir(IN1, GPIO_OUT);
    gpio_init(IN2);
    gpio_set_dir(IN2, GPIO_OUT);
    gpio_init(IN3);
    gpio_set_dir(IN3, GPIO_OUT);
    gpio_init(IN4);
    gpio_set_dir(IN4, GPIO_OUT);

    // Configuracoes do ADC
    adc_init();     // Habilita bloco ADC
    adc_gpio_init(26);      // Habilita o pino 26 (GPIO26 do pinout)
    adc_select_input(0);        // Configura porta do ADC no canal 0 (ADC0 do pinout)

    // Cria um timer de repeticao
    struct repeating_timer timer;

    // Cria uma funcao repetitiva em cima do timer criado
    // Passo o delay em ms, funcao a ser rodada no callback, contexto de 
    // Identificador (usamos NULL) e o endereco do timer
    add_repeating_timer_ms(100, step_callback, NULL, &timer);

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
