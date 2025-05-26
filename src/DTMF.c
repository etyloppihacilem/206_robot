/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          DTMF.c
        -\-    _|__
         |\___/  . \        Created on 26 May. 2025 at 21:55
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "LPC17xx.h"
#include "livraisons.h"
#include <stdint.h>

volatile uint8_t stop_there = 0;

void EINT1_IRQHandler() {
    LPC_SC->EXTINT |= 1 << 1;
    if (!next_stop) // si pas de prochaine livraison, pas de DTMF
        return;
    uint32_t dtmf_read = (LPC_GPIO0->FIODIR & 0xF) >> 4;
    if (next_stop == dtmf_read)
        stop_there = 1;
}

void init_dtmf() {
    LPC_PINCON->PINSEL1 &= ~(0xFF << 8); // P0.4 P0.5 P0.6 P0.7
    LPC_GPIO0->FIODIR &= ~(0xF << 4);
    LPC_PINCON->PINSEL4 &= ~(3 << 22); // P2.11 for EINT1
    LPC_PINCON->PINSEL4 &= 1 << 22;

    LPC_SC->EXTMODE |= 1 << 1;
    LPC_SC->EXTPOLAR |= 1 << 1;

    NVIC_EnableIRQ(EINT1_IRQn);
}
