/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          ir.c
        -\-    _|__
         |\___/  . \        Created on 24 May. 2025 at 13:51
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "LPC17xx.h"
#include "com_debug.h"

uint8_t impulsion     = 0;
uint8_t current_level = 0;

void TIMER3_IRQHandler() {
    LPC_TIM3->IR = 1 << 1;
    if (current_level)
        LPC_TIM3->EMR |= 1; // on met à 1 le front
    if (++impulsion == 8) {
        impulsion = 0;
        current_level = !current_level;
    }
}

void init_ir() {
    LPC_SC->PCLKSEL1    |= 1 << 15; // timer3 at 50MHz
    LPC_SC->PCONP       |= 1 << 23; // enable timer3
    LPC_PINCON->PINSEL0 |= 3 << 20;

    LPC_TIM3->PR  = 49;         // to make timer3 at 1MHz, period of 1us
    LPC_TIM3->MR0 = 1;          // rapoprt cyclique de l'impulsion
    LPC_TIM3->MR1 = 30;         // periode de l'impulsion pas haute
    LPC_TIM3->EMR = 1 << 4; // EMR MR0 down
    LPC_TIM3->MCR = 1 << 3 | 1 << 4;
    NVIC_EnableIRQ(TIMER3_IRQn);
    NVIC_SetPriority(TIMER3_IRQn, 2);
    LPC_TIM3->TCR = 1 << 1;
    LPC_TIM3->TCR = 1;
}
