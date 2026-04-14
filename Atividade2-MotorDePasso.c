#include <stdio.h>
#include "pico/stdlib.h"

//teste 1

int main()
{
    stdio_init_all();

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
