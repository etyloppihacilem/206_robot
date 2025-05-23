/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          livraisons.h
        -\-    _|__
         |\___/  . \        Created on 23 May. 2025 at 14:55
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#pragma once

#include <stdint.h>

typedef enum {
    droite = 0,
    gauche = 1,
} cote;

typedef enum {
    dispo       = 'D',
    enlevecolis = 'E',
    colispris   = 'C',
    livraison   = 'L',
} etat;

extern uint8_t next_stop;
extern char    ns_side;
extern char    ns_letter;
extern uint8_t vitesse_cible; // en %
extern char    state;
extern uint8_t id_robot;
