#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/adc.h"

// TODO: filtro ADC (tirar ruidos)
// TODO: ADC com interrupcao

#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

// Sequencia de half steps
int mat_seq[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
};

// Incializa variaveis
int passo_atual = 0;
int passo_setpoint = 0;


// Funcao chamada pelo repeating timer
bool step_callback(struct repeating_timer *t){
    // variavel static retem o valor dela entre chamadas da funcao!!! (bem util)
    // como se fosse uma variavel global que so esta funcao pode acessar 
    // esse igual a false so funciona na criacao
    // em outras chamadas, ele apenas pega o valor anterior que ja estava
    static int step_state = 1;
    static int passos = 0;

    // Implementacao da sequencia em half step
    gpio_put(IN1, mat_seq[step_state][0]);
    gpio_put(IN2, mat_seq[step_state][1]);
    gpio_put(IN3, mat_seq[step_state][2]);
    gpio_put(IN4, mat_seq[step_state][3]);

    passos = passo_setpoint - passo_atual;
    // Incrementar
    if (passos > 0){
        if (step_state >= 8){
            step_state = 1;
        } 
        else {
            step_state++;
            passo_atual++;
        }
    }

    // Decrementar
    if (passos < 0){
        if (step_state <= 1){
            step_state = 8;
        } 
        else {
            step_state--;
            passo_atual--;
        }
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
    add_repeating_timer_ms(5, step_callback, NULL, &timer);

    bool first_time = true;
    while (true) {
        uint16_t raw = adc_read();
        // analise dimensional:
        // 3.3f eh o fundo de escala analogico do ADC da pico (ate quanto a porta le)
        // 4095 eh o fundo de escala digital do ADC (12 bits)
        //      float voltage_setpoint = raw * (3.3f / 4095);
        // 300° eh o angulo de abertura maximo de um tipico potenciometro B10K
        //      float angulo_setpoint = voltage_setpoint * (300 / 3.3f);
        // motor de passo tem 64 passos correspondendo a 5,625°
        //      float passo_setpoint = angulo_setpoint * (64 / 5.625);
        // juntando todas estas contas, tem-se:
        passo_setpoint = truncf((raw * 300.0f * 64.0f) / (4095.0f * 5.625f));
        printf("raw_adc = %u, pass_setpoint = %i\n", raw, passo_setpoint);

        if (first_time){
            passo_atual = passo_setpoint;
            first_time = false;
        }
        sleep_ms(1);
    }
    return 0;
}
