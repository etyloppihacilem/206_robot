/* #####################################################################################################################

               """          moteurs.c
        -\-    _|__
         |\___/  . \        Created on 21 May. 2025 at 10:26
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "LPC17xx.h"
#include <stdint.h>

#define PWM_PERIOD 1250
#define PWM_STOP PWM_PERIOD + 1
#define PWM_MIN 550

void init_moteurs() {
    LPC_SC->PCONP |= 1 << 17; // activation du périphérique

    LPC_PINCON->PINSEL3 &= 3 << 6 | 3 << 18; // P1.19 et P1.25
    LPC_PINCON->PINSEL3 |= 1 << 6 | 1 << 18;
    LPC_MCPWM->MCPER0    = PWM_PERIOD; // période canal 0
    LPC_MCPWM->MCPER1    = PWM_PERIOD; // période canal 1
    LPC_MCPWM->MCPW0     = PWM_STOP;    // largeur d'impulsion canal 0 (50%)
    LPC_MCPWM->MCPW1     = PWM_STOP;    // largeur d'impulsion canal 1 (50%)

    // Mettre en route les sorties (MCCON_SET)
    LPC_MCPWM->MCCON_SET = (1 << 0) | (1 << 8); // Ch 0A et 1A enable

    // Démarrer les compteurs (MCCNTCON_SET)
    LPC_MCPWM->MCCNTCON_SET = (1 << 0) | (1 << 1); // Counter enable pour canal 0 et 1
}

/*
 * vitesse min : 0
 * vitesse max : 512
 * */
void deplacement(uint16_t droite, uint16_t gauche) {
    if (droite == 0)
        droite = PWM_STOP;
    else if (droite >= 512)
        droite = 0;
    else {
        droite = 512 - droite;
    }
    if (gauche == 0)
        gauche = PWM_STOP;
    else if (gauche >= 512)
        gauche = 0;
    else {
        gauche = 512 - gauche;
    }
    LPC_MCPWM->MCPW0 = droite; // largeur d'impulsion canal 0 (50%)
    LPC_MCPWM->MCPW1 = gauche; // largeur d'impulsion canal 1 (50%)
}
