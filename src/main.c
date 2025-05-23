/* #####################################################################################################################

               """          main.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 09:16
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "LPC17xx.h"
#include "moteurs.h"
#include "system_LPC17xx.h"
#include "ultrasonic.h"
#include <stdint.h>

static void delay_ms(uint32_t ms)
{
    volatile uint32_t cycles = ms * (SystemCoreClock / 4000U);
    while (cycles--) __NOP();
}

int main(void) {
    SystemCoreClockUpdate();
    // init_ultrasonic();
    init_moteurs();
    // measure_ultrasonic();
    // deplacement(0, 1);
    while (1) {
    }
}
