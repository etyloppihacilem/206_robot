/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          bobines.c
        -\-    _|__
         |\___/  . \        Created on 23 May. 2025 at 11:36
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "bobines.h"
#include "LPC17xx.h"
#include "com_debug.h"
#include "livraisons.h"
#include "params.h"
#include <stdint.h>

#define PORTEUSE_INBOX_SIZE 16
#define MESURE_AFTER 699 // us

static uint16_t         message    = 1;
volatile static uint8_t cpt_mesure = 0;
position_info           position   = { 0 };

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

static uint32_t last_time = 0;

void EINT2_IRQHandler() {
    LPC_SC->EXTINT |= 1 << 2;
    uint32_t time   = LPC_TIM1->TC;
    LPC_TIM1->TCR   = 1;
    if (time - last_time > pause) {
        LPC_TIM1->TCR  = 1 << 1;
        LPC_TIM1->MR0  = MESURE_AFTER;
        if (cpt_mesure++ >= 2)
            cpt_mesure = 0;
        LPC_TIM1->TCR = 1;
        last_time     = 0;
    }
    if (time > entete) {
        if (message != 1 && !(message & (1 << 15)))
            parsing_porteuse();
        message = 1;
    } else if (time > porteuse_1) {
        message = (message << 1) | 1;
    } else if (time > porteuse_0) {
        message = (message << 1);
    }
}

void TIMER1_IRQHandler() {
    // if (cpt_mesure < 3) {
    //     LPC_TIM1->MR0 += 10;
    //     LPC_TIM1->EMR &= ~1;
    // }
    LPC_TIM1->IR   = 1;
    position.phase = LPC_GPIO1->FIOPIN;
    position.phase = (position.phase >> 10) & 0x3;
}

static uint8_t next_ch = 2;

void ADC_IRQHandler() {
    uint32_t gdr = LPC_ADC->ADGDR; // <-- lit & efface DONE global

    uint8_t  ch  = (gdr >> 24) & 0x7;  /* canal terminé (CHN)        */
    uint16_t val = (gdr >> 4) & 0xFFF; /* résultat 12 bits           */

    switch (ch) {
        case 2:
            position.com_val = val;
            next_ch          = 4;
            break;
        case 4:
            position.b1_val = val;
            next_ch         = 5;
            break;
        case 5:
            position.b2_val = val;
            next_ch         = 2;
            break;
        default:
            break; /* sécurité */
    }

    /* prépare la prochaine conversion : on change *uniquement* SEL        */
    LPC_ADC->ADCR = (LPC_ADC->ADCR & ~0xFF) | (1 << next_ch);
    LPC_TIM1->EMR &= ~1;
    // debug_put_hex(LPC_ADC->ADCR);
    // debug_write("\r\n");
}

void init_bobines() {
    // setting up GPIO for digital read
    LPC_PINCON->PINSEL3  &= ~(0xF << 20);
    LPC_GPIO1->FIODIR    &= ~(0x3 << 10); // P1.26 et P1.27
    LPC_PINCON->PINMODE3 &= ~(0xF << 20);
    LPC_PINCON->PINMODE3 |= 0xA << 20;
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

    LPC_ADC->ADCR = (1 << 2)    // | (1 << 4) | (1 << 5) // SEL: AD0.2, AD0.4, AD0.5
                  | (1 << 8)    // CLKDIV: ajusté pour < 13 MHz
                  | (0x6 << 24) // start on MAT1.0
                  | (1 << 21);  // PDN: ADC allumé
    LPC_ADC->ADINTEN     = 1 << 8;
    // setting up EINT2
    LPC_PINCON->PINSEL4 &= ~(3 << 24); // EINT2 on P2.12
    LPC_PINCON->PINSEL4 |= 1 << 24;
    LPC_SC->EXTMODE     |= 1 << 2; // on edge
    LPC_SC->EXTPOLAR    |= 1 << 2; // on rising edge
    LPC_SC->EXTINT       = 1 << 2; // clearing EXTINT

    // setting up TIMER1
    LPC_PINCON->PINSEL3 |= 3 << 12; // P1.22 as MAT1.0
    LPC_SC->PCONP       |= 1 << 2;  // enabling timer 1
    LPC_TIM1->PR         = 24;      // 1MHz
    LPC_TIM1->TCR        = 1 << 1;
    LPC_TIM1->MR0        = MESURE_AFTER; // 500us
    LPC_TIM1->MCR        = 1;            // interrupt on MR0
    LPC_TIM1->EMR       &= ~(3 << 4);
    LPC_TIM1->EMR       |= 2 << 4; // MAT1.0 HIGH at MR0

    // setting up IRQ
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);     // faible priorité car en vrai osef
    NVIC_SetPriority(EINT2_IRQn, 3); // un peu moins prioritaire que l'IR
    NVIC_EnableIRQ(EINT2_IRQn);
}
