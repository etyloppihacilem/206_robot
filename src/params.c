/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          params.c
        -\-    _|__
         |\___/  . \        Created on 23 May. 2025 at 15:34
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "params.h"
#include "LPC17xx.h"
#include <stdint.h>

#define MAT_HEIGHT 2
#define MAT_WIDTH 4

typedef enum {
    ligne1  = 8,
    ligne2  = 9,
    ligne3  = 19,
    ligne4  = 20,
    colone1 = 21,
    colone2 = 23,
    colone3 = 24,
    colone4 = 27,
} matrice;

uint8_t id_robot = 0;

uint8_t read_value(matrice y) { // OPTI: peut être passer ça en static
    uint32_t ret = 1 << (y);

    LPC_GPIO0->FIOPIN &= ~ret;      // on met la ligne où lire au niveau bas
    while (LPC_GPIO0->FIOPIN & ret) // il faut attendre que le GPIO soit niveau bas
        ;
    // lecture de la valeur
    ret = (((LPC_GPIO0->FIOPIN & (3 << colone2)) >> (colone2 - 1)) | (LPC_GPIO0->FIOPIN & (1 << colone1))) >> (colone1)
        | (LPC_GPIO0->FIOPIN & (1 << colone4)) >> (colone4 - 3);
    LPC_GPIO0->FIOPIN |= (3 << ligne1) | (3 << ligne3); // on remet la ligne niveau haut
    return (~ret) & 0xF; // on renvoie la valeur NOT parce que niveau bas veut dire selectionné.
}

void init_params() {
    LPC_PINCON->PINSEL1 &= ~(
        (3 << ligne1) | (3 << ligne2) | (3 << ligne3) | (3 << ligne4) | (3 << colone1) | (3 << ligne2) | (3 << ligne3)
        | (3 << ligne4)
    );
    LPC_GPIO0->FIODIR |= (3 << ligne1) | (3 << ligne3);                                // les lignes en sortie
    LPC_GPIO0->FIODIR &= ~(1 << colone1 | 1 << colone2 | 1 << colone3 | 1 << colone4); // les colones en entrée
    LPC_GPIO0->FIOPIN |= (3 << ligne1) | (3 << ligne3);                                // on remet la ligne niveau haut
}

void read_params() {}
