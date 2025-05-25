/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

;wa
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

static uint16_t message  = 1;
position_info   position = { 0 };

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

void TIMER1_IRQHandler() {
    LPC_TIM1->IR     = 1;
    position.phase   = LPC_GPIO1->FIOPIN;
    position.com_val = (LPC_ADC->ADDR2 >> 4) & 0xFFF;
    position.b1_val  = (LPC_ADC->ADDR4 >> 4) & 0xFFF;
    position.b2_val  = (LPC_ADC->ADDR5 >> 4) & 0xFFF;
    position.phase   = (position.phase >> 13) & 0x3;
}

void init_bobines() {
    // setting up GPIO for digital read
    LPC_PINCON->PINSEL3  &= ~(0xF << 26);
    LPC_GPIO1->FIODIR    &= ~(0x3 << 13);
    // setting up ADC pins and pincon
    LPC_SC->PCONP        |= 1 << 12; // enabling ADC
    LPC_PINCON->PINSEL1  &= ~(3 << 18);
    LPC_PINCON->PINSEL1  |= (1 << 18);   // P0.25
    LPC_PINCON->PINSEL3  |= (0xF << 28); // P1.30 et P1.31
    LPC_PINCON->PINMODE1 &= ~(3 << 18);  // P0.25 sans pull du tout
    LPC_PINCON->PINMODE1 |= (2 << 18);
    LPC_PINCON->PINMODE3 &= ~(3 << 28 | 3 << 30); // P1.30 et P1.31 sans pull du tout
    LPC_PINCON->PINMODE3 |= (2 << 28) | (2 << 30);
    // ADC config
    LPC_ADC->ADCR         = (1 << 2) | (1 << 4) | (1 << 5) // SEL: AD0.2, AD0.4, AD0.5
                  | (22 << 8)                              // CLKDIV: ajusté pour < 13 MHz
                  | (1 << 16)                              // BURST: mode burst activé
                  | (1 << 21);                             // PDN: ADC allumé
    // setting up EINT2
    LPC_PINCON->PINSEL4 &= ~(3 << 24); // EINT2 on P2.12
    LPC_PINCON->PINSEL4 |= 1 << 24;
    LPC_SC->EXTMODE     |= 1 << 2; // on edge
    LPC_SC->EXTPOLAR    |= 1 << 2; // on rising edge
    LPC_SC->EXTINT       = 1 << 2; // clearing EXTINT

    // setting up TIMER1
    LPC_TIM1->PR  = 24; // 10MHz
    LPC_TIM1->TCR = 1 << 1;
    LPC_TIM1->TCR = 1;
    LPC_TIM1->MR0 = 39; // 4us
    LPC_TIM1->MCR = 7;  // reset, stop and interrupt on MR0
    NVIC_EnableIRQ(EINT2_IRQn);
    NVIC_SetPriority(EINT2_IRQn, 3); // un peu moins prioritaire que l'IR
    NVIC_EnableIRQ(TIMER1_IRQn);     // faible priorité car en vrai osef
}
