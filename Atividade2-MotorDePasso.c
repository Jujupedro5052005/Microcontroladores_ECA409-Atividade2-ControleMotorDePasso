// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//                  Erich Abreu Serafim                  23.10022-2
//                  João Pedro de Jesus Cândido Silva    23.01416-4
// https://github.com/Jujupedro5052005/Microcontroladores_ECA409-Atividade2-ControleMotorDePasso
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Importacao de módulos
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/adc.h"

// Definicao de pinos
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define POT_PIN 26

// Definicao de variaveis globais
const int TEMPO_CALLBACK = 3;

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

// Inicializa variaveis globais do step
int passo_atual = 0;
int passo_setpoint = 0;
bool first_time = true;

// Ajuste alpha do filtro EMA (menor alpha, mais suave e mais lento)
const float alpha = 0.1f;

// Funcao chamada pelo repeating timer
bool step_callback(struct repeating_timer *t){
    // Variavel static retem o valor dela entre chamadas da funcao!!! (bem util)
    // Como se fosse uma variavel global que so esta funcao pode acessar 
    // Esse igual a false so funciona na criacao
    // Em outras chamadas, ele apenas pega o valor anterior que ja estava
    static int step_state = 0;
    static int passos = 0;

    // Implementacao da sequencia em half step
    gpio_put(IN1, mat_seq[step_state][0]);
    gpio_put(IN2, mat_seq[step_state][1]);
    gpio_put(IN3, mat_seq[step_state][2]);
    gpio_put(IN4, mat_seq[step_state][3]);

    passos = passo_setpoint - passo_atual;
    // Incrementar
    if (passos > 0){
        if (step_state >= 7){
            step_state = 0;
        } 
        else {
            step_state++;
            passo_atual++;
        }
    }

    // Decrementar
    if (passos < 0){
        if (step_state <= 0){
            step_state = 7;
        } 
        else {
            step_state--;
            passo_atual--;
        }
    }
    adc_run(true); // Inicia uma conversão single-shot
    return true;
}

void __isr adc_isr(void){
    uint16_t raw = adc_fifo_get();
    irq_clear(ADC_IRQ_FIFO);
    static float adc_filtrado = 0.0f;

    // Implementando filtro exponencial EMA para suavizar a leitura
    adc_filtrado = alpha * raw + (1.0f - alpha) * adc_filtrado;

    // Analise dimensional:
    // 3.3f eh o fundo de escala analogico do ADC da pico (ate quanto a porta le)
    // 4095 eh o fundo de escala digital do ADC (12 bits)
    //      float voltage_setpoint = raw * (3.3f / 4095);
    // 300° eh o angulo de abertura maximo de um tipico potenciometro B10K
    //      float angulo_setpoint = voltage_setpoint * (300 / 3.3f);
    // motor de passo tem 64 passos correspondendo a 5,625°
    //      float passo_setpoint = angulo_setpoint * (64 / 5.625);
    // Juntando todas estas contas, tem-se:
    passo_setpoint = truncf((adc_filtrado * 300.0f * 64.0f) / (4095.0f * 5.625f));
    printf("raw_adc = %u, pass_setpoint = %i\n", adc_filtrado, passo_setpoint);

    if (first_time){
        passo_atual = passo_setpoint;
        first_time = false;
    }

    adc_run(false); // Para a amostragem até próximo timer
}

int main(){
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
    adc_init();                     // Habilita bloco ADC
    adc_gpio_init(POT_PIN);         // Habilita o pino 26 (GPIO26 do pinout)
    adc_select_input(0);            // Configura porta do ADC no canal 0 (ADC0 do pinout)

    // FIFO: habilita + gera IRQ quando >=1 amostra
    adc_fifo_setup(true, false, 1, false, false);
    adc_irq_set_enabled(true);  // Permite interrupção
    irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_isr);
    irq_set_enabled(ADC_IRQ_FIFO, true);

    // Cria um timer de repeticao
    struct repeating_timer timer;

    // Cria uma funcao repetitiva em cima do timer criado
    // Passo o delay em ms, funcao a ser rodada no callback, contexto de 
    // Identificador (usamos NULL) e o endereco do timer
    add_repeating_timer_ms(TEMPO_CALLBACK, step_callback, NULL, &timer);

    while (true) {
        tight_loop_contents();
    }
    return 0;
}
