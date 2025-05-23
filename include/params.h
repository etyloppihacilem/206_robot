/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          params.h
        -\-    _|__
         |\___/  . \        Created on 23 May. 2025 at 13:20
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#pragma once

#include <stdint.h>

typedef enum {
    Hz10 = 0b11,
    Hz15 = 0b10,
    Hz20 = 0b01,
    Hz25 = 0b00,
} t_mesure_tele;

typedef enum {
    rien_tele   = 0b11,
    nb_mesure   = 0b10,
    cpt_brut    = 0b01,
    dist_mesure = 0b00,
} t_debug_tele;

typedef enum {
    rien_pos     = 0b111,
    analog_A     = 0b110,
    analog_B     = 0b101,
    analog_C     = 0b100,
    dist_bob1    = 0b011,
    dist_bob2    = 0b010,
    angle_rob    = 0b001,
    duree_impuls = 0b000,
} t_debug_pos;

typedef enum {
    rien_moteur = 0b11,
    pwm_raw     = 0b10,
    vitesse     = 0b01,
    vitesse_lue = 0b00, // non implémentée
} t_debug_moteur;

extern uint8_t        id_robot;
extern t_mesure_tele  mesure_tele;
extern t_debug_tele   debug_tele;
extern t_debug_pos    debug_pos;
extern t_debug_moteur debug_moteur;

void init_params();
void read_params();
