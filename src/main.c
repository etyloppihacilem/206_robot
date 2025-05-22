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
    // measure_ultrasonic();
    init_moteurs();
    while (1) {
        // tout droit
        deplacement(1, 1);
        delay_ms(4000); // 2s
        // plus vite
        deplacement(256, 256);
        delay_ms(2000); // 2s
        // encore plus vite
        deplacement(512, 512);
        delay_ms(2000); // 2s
        // à gauche
        deplacement(1, 512);
        delay_ms(2000); // 2s
        // à droite
        deplacement(512, 1);
        delay_ms(2000); // 2s
        // à l'arrêt
        deplacement(0, 0);
        delay_ms(2000); // 2s
    }
}
