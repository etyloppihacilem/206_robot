/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          it_urg.c
        -\-    _|__
         |\___/  . \        Created on 27 May. 2025 at 14:50
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "LPC17xx.h"
#include <stdint.h>
#include "com_debug.h"
#include "moteurs.h"

void EINT0_IRQHandler() {
    LPC_SC->EXTINT |= 1;
    debug_write("URGENCE\r\n");
    deplacement(0, 0);
    while (LPC_GPIO0->FIODIR & 1 << 11); // attendre le bouton
}

void init_it_urg() {
    LPC_PINCON->PINSEL4 &= ~(3ul << 20); // 2.10
    LPC_PINCON->PINSEL4 |= 1ul << 20;
    LPC_PINCON->PINSEL0 &= ~(3ul << 22);

    LPC_SC->EXTMODE &= ~1ul;
    LPC_SC->EXTPOLAR |= 1;

    NVIC_SetPriority(EINT0_IRQn, 0);
    NVIC_EnableIRQ(EINT0_IRQn);
}
