/* #####################################################################################################################

               """          moteurs.c
        -\-    _|__
         |\___/  . \        Created on 21 May. 2025 at 10:26
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "LPC17xx.h"
#include <stdint.h>

void init_moteurs() {
    LPC_SC->PCONP |= 1 << 17; // activation du périphérique

    LPC_PINCON->PINSEL3 &= 3 << 6 | 3 << 18; // P1.19 et P1.25
    LPC_PINCON->PINSEL3 |= 1 << 6 | 1 << 18;
    LPC_MCPWM->MCPER0    = 1250; // période canal 0
    LPC_MCPWM->MCPER1    = 1250; // période canal 1
    LPC_MCPWM->MCPW0     = 0;    // largeur d'impulsion canal 0 (50%)
    LPC_MCPWM->MCPW1     = 0;    // largeur d'impulsion canal 1 (50%)

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
    // 550 est la valeur minimum
    static const uint16_t min = 550;
    if (droite == 0)
        droite = 1250;
    else if (droite > 512)
        droite = 512;
    else
        droite = droite * (1250 - min) / 512;
    if (gauche == 0)
        gauche = 1250;
    else if (gauche > 512)
        gauche = 512;
    else
        gauche = gauche * (1250 - min) / 512;
    LPC_MCPWM->MCPW0 = 1250 - droite; // largeur d'impulsion canal 0 (50%)
    LPC_MCPWM->MCPW1 = 1250 - gauche; // largeur d'impulsion canal 1 (50%)
}
