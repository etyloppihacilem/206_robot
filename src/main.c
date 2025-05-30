/* #####################################################################################################################

               """          main.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 09:16
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "DTMF.h"
#include "LPC17xx.h"
#include "bobines.h"
#include "com_debug.h"
#include "ir.h"
#include "livraisons.h"
#include "moteurs.h"
#include "params.h"
#include "ultrasonic.h"
#include <stdint.h>
#include "it_urg.h"

uint8_t next_stop     = 0;
char    ns_side       = droite;
char    ns_letter     = 0;
uint8_t vitesse_cible = 50; // en %
char    state         = dispo;

static const uint16_t coef_correction = 4;
static const uint16_t inv_amplitude   = 1;

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
    // static uint16_t max_ampl   = 0;
    uint16_t mot_droit  = 512;
    uint16_t mot_gauche = 512;

    // position du robot sur le fil
    // if (max_ampl < position.com_val)
    //     max_ampl = position.com_val;
    // uint16_t div_amplitude = (max_ampl - position.com_val) / inv_amplitude;

    // angle du robot bobine avant
    if (position.phase & b2) // 1 si en phase, 0 sinon
        mot_droit += 256;
    else
        mot_gauche += 256;
    // if (position.phase & b1) // 1 si en phase, 0 sinon
    //     mot_gauche -= coef_correction * div_amplitude;
    // else
    //     mot_droit -= coef_correction * div_amplitude;

    // debug_write("dom_val ");
    // debug_put_uint(position.com_val);
    // debug_write("\r\nb1_val ");
    // debug_put_uint(position.b1_val);
    // debug_write("\r\nb2_val ");
    // debug_put_uint(position.b2_val);
    // debug_write("\r\nphase ");
    // debug_put_uint(position.phase);
    // debug_write("\r\nDROIT ");
    // debug_put_uint(mot_droit);
    // debug_write(" GAUCHE ");
    // debug_put_uint(mot_gauche);
    // debug_write("\r\n");
    // debug_write("coef ");
    // debug_put_uint(coef_correction);
    // debug_write("\r\n");
    // debug_write("max ampl ");
    // debug_put_uint(max_ampl);
    // debug_write("\r\n");

    // deplacement(0, 0);
    deplacement(mot_droit / 8, mot_gauche / 8);
}

static void delay_ms(uint32_t ms) {
    /* Petit délai bloquant : ~1 ms par incrément lorsque l’horloge cœur = 100 MHz.
       Ajuste le facteur si tu changes SystemCoreClock.                    */
    volatile uint32_t cycles = ms * (SystemCoreClock / 4000U);
    while (cycles--)
        __NOP();
}

int main(void) {
    // init_params();
    // read_params();
    init_com_debug(115200);
    // init_ultrasonic();
    // init_moteurs();
    // init_bobines();
    // init_dtmf();
    debug_write("hi\r\n");
    // init_ir();
    // init_it_urg();
    debug_write("coucou\r\n");

    // uint32_t test = 300;
    // static const uint32_t test = 1251;
    // delay_ms(5000);
    while (1) {
        conducteur();
        // LPC_MCPWM->MCPW0 = test; // largeur d'impulsion canal 0 (50%)
        // LPC_MCPWM->MCPW1 = test * 1.1; // largeur d'impulsion canal 1 (50%)
        // debug_write("test: ");
        // debug_put_uint(test);
        // debug_write("\r\n");
        // test += 10;
        // if (test > 1250)
        //     test = 0;
        // delay_ms(250);
    }
}
