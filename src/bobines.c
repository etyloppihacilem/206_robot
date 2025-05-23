/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          bobines.c
        -\-    _|__
         |\___/  . \        Created on 23 May. 2025 at 11:36
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "bobines.h"
#include "LPC17xx.h"
#include "livraisons.h"
#include "params.h"
#include <stdint.h>

#define PORTEUSE_INBOX_SIZE 16

static uint16_t message = 1;

static void parsing_porteuse() {
    uint8_t robot = (message & 0xF << 7) >> 7;
    if (robot != id_robot)
        return;
    uint8_t type = (message & 0x7 << 11) >> 11;
    switch (type) {
        case ordre_vitesse:
            vitesse_cible = message & 0x7F;
            break;
        case chargement_droite:
        case chargement_gauche:
            state = enlevecolis;
        case dechargement_droite:
        case dechargement_gauche:
            if (type & 1)
                ns_side = gauche;
            else
                ns_side = droite;
            next_stop = message & 0x1F;
            ns_letter = ((message & 0x3 << 5) >> 5) + 'A';
        default:;
    }
}

void EINT2_IRQHandler() {
    LPC_TIM1->TCR = 0;
    uint32_t time = LPC_TIM1->TC;
    LPC_TIM1->TCR = 1 << 1;
    LPC_TIM1->TCR = 1;
    if (time > entete) {
        if (message != 1 && !(message & (1 << 15)))
            ;
        parsing_porteuse();
        message = 1;
    } else if (time > porteuse_1) {
        message = (message << 1) | 1;
    } else if (time > porteuse_0) {
        message = (message << 1);
    }
}

void init_bobines() {
    // setting up EINT2
    LPC_PINCON->PINSEL4 &= ~(3 << 24); // EINT2 on P2.12
    LPC_PINCON->PINSEL4 |= 1 << 24;
    LPC_SC->EXTMODE     |= 1 << 2; // on edge
    LPC_SC->EXTPOLAR    |= 1 << 2; // on rising edge
    LPC_SC->EXTINT       = 1 << 2; // clearing EXTINT

    // setting up TIMER1
    LPC_TIM1->MCR = 0;
    LPC_TIM1->TCR = 1 << 1;
    LPC_TIM1->TCR = 1;
    // setting up ADC
}
