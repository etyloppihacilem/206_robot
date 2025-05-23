/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          bobines.h
        -\-    _|__
         |\___/  . \        Created on 23 May. 2025 at 11:36
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#pragma once

#include <stdint.h>

typedef enum {
    porteuse_0 = 31250, // 1,25ms
    porteuse_1 = 50000, // 2ms
    entete     = 68750, // 2,75ms
} t_porteuse_seq;

typedef enum e_message_type {
    ordre_vitesse       = 0b000,
    chargement_gauche   = 0b001,
    chargement_droite   = 0b010,
    dechargement_gauche = 0b011,
    dechargement_droite = 0b100,
} message_type;

typedef struct {
        message_type type;
        uint8_t      argument; // poste ou vitesse
        char         livraison;
} fifo_bobine;

void init_bobines();
