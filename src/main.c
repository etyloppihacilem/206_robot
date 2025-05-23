/* #####################################################################################################################

               """          main.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 09:16
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "bobines.h"
#include "livraisons.h"
#include "moteurs.h"
#include "ultrasonic.h"
#include <stdint.h>

uint8_t next_stop     = 0;
char    ns_side       = droite;
char    ns_letter     = 0;
uint8_t vitesse_cible = 50; // en %
char    state         = dispo;
uint8_t id_robot      = 1;

int main(void) {
    init_ultrasonic();
    init_moteurs();
    init_bobines();
    while (1) {}
}
