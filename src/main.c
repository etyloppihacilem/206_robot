/* #####################################################################################################################

               """          main.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 09:16
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "bobines.h"
#include "com_debug.h"
#include "ir.h"
#include "livraisons.h"
#include "moteurs.h"
#include "params.h"
#include "ultrasonic.h"
#include <stdint.h>

uint8_t next_stop     = 0;
char    ns_side       = droite;
char    ns_letter     = 0;
uint8_t vitesse_cible = 50; // en %
char    state         = dispo;

static const uint16_t coef_correction = 50;
static const uint16_t inv_amplitude   = 125;

/*
 * La procédure est la suivante :
 *  - on calcule à quel point le robot n'est pas sur le fil
 *  - on cherche à savoir le décalage de la bobine avant (b2) pour déterminer l'accélération de la roue pour tourner
 *  - on cherche à savoir le décalage de la bobine arrière (b1) pour déterminer la décélération de la roue opposée
 *    au virage.
 *  - on en conclue une vitesse de roue, chaque roue par défaut à 512, allant de 0 à 1024.
 *  - on fait un produit en croix pour remettre à la vitesse maximum.
 *
 * Tableau de position en fonction de la phase :
 *
 * bobine        | bobine à droite | bobine à gauche
 * --------------|-----------------|-----------------
 * avant (b2)    |   opposition    |    en phase
 * arrière (b1)  |    en phase     |   opposition
 *
 * */
void conducteur() {
    static uint16_t max_ampl   = 0;
    uint16_t        mot_droit  = 512;
    uint16_t        mot_gauche = 512;

    // position du robot sur le fil
    if (max_ampl < position.com_val)
        max_ampl = position.com_val;
    uint16_t div_amplitude = (max_ampl - position.com_val) / inv_amplitude;

    // angle du robot bobine avant
    if (position.phase & b2) // 1 si en phase, 0 sinon
        mot_droit += coef_correction * div_amplitude;
    else
        mot_gauche += coef_correction * div_amplitude;
    if (position.phase & b1) // 1 si en phase, 0 sinon
        mot_droit -= coef_correction * div_amplitude;
    else
        mot_gauche -= coef_correction * div_amplitude;

    deplacement(mot_droit, mot_gauche);
}

int main(void) {
    init_params();
    read_params();
    init_com_debug(115200);
    init_ultrasonic();
    init_moteurs();
    init_bobines();
    init_ir();

    while (1) {}
}
