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
    porteuse_0 = 900, // 0.9ms
    porteuse_1 = 1900, // 1.9ms
    entete     = 2400, // 2.4ms
    pause      = 400, // 0.4ms
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

typedef enum {
    b1 = 0x1,
    b2 = 0x3,
} bobine;

typedef struct {
        uint8_t  phase; // first three bis are level
        uint16_t com_val;
        uint16_t b1_val;
        uint16_t b2_val;
} position_info;

extern volatile position_info position;

void init_bobines();
