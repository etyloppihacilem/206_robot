/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          ir.c
        -\-    _|__
         |\___/  . \        Created on 24 May. 2025 at 13:51
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "ir.h"
#include "LPC17xx.h"
#include "livraisons.h"
#include "params.h"

#define TRAME_SIZE 53

uint8_t  impulsion          = 0;
uint8_t  current_level      = 0;
uint8_t  trame0[TRAME_SIZE] = { 0 };
uint8_t  trame1[TRAME_SIZE] = { 0 };
uint8_t *trame[2]           = { trame0, trame1 };
uint8_t  current_trame      = 1;
uint8_t  c                  = 0;

void TIMER3_IRQHandler() {
    LPC_TIM3->IR = 1 << 1;
    if (current_level)
        LPC_TIM3->EMR |= 1; // on met à 1 le front
    if (++impulsion == 8) {
        impulsion     = 0;
        current_level = trame[current_trame][c++];
        if (c >= TRAME_SIZE) {
            c                 = 0;
            LPC_GPIO1->FIOSET = 1 << 28;
        } else if (c == 3) {
            LPC_GPIO1->FIOCLR = 1 << 28;
        }
    }
}

void write_to_trame(uint8_t data, uint8_t pos, uint8_t t) {
    // data start at bit 2
    if (pos > 3 || t > 1)
        return;
    for (int8_t i = 3; i >= 0; i--) {
        if (data & 1 << i) {
            trame[t][(pos * 12) + ((3 - i) * 3) + 2] = 1;
            trame[t][(pos * 12) + ((3 - i) * 3) + 3] = 1;
            trame[t][(pos * 12) + ((3 - i) * 3) + 4] = 0;

        } else {
            trame[t][(pos * 12) + ((3 - i) * 3) + 2] = 1;
            trame[t][(pos * 12) + ((3 - i) * 3) + 3] = 0;
            trame[t][(pos * 12) + ((3 - i) * 3) + 4] = 0;
        }
    }
}

uint8_t update_etat(uint8_t t) {
    switch (state) {
        case dispo:
            write_to_trame(ir_dispo, 2, t);
            return ir_dispo;
        case enlevecolis:
            write_to_trame(ir_enlevecolis, 2, t);
            return ir_enlevecolis;
        case colispris:
            write_to_trame(ir_colispris, 2, t);
            return ir_colispris;
        case livraison:
            write_to_trame(ir_livraison, 2, t);
            return ir_livraison;
        default:
            write_to_trame(0, 2, t);
    }
    return 0;
}

uint8_t update_vitesse(uint8_t t) {
    uint8_t vitesse = 0;
    if (vitesse_cible > 20)
        vitesse = (vitesse_cible - 20) / 5;
    write_to_trame(vitesse, 1, t);
    return vitesse;
}

void update_ir() {
    uint8_t sum        = id_robot;
    uint8_t next_trame = 0;
    if (current_trame == 0)
        next_trame = 1;
    sum += update_vitesse(next_trame);
    sum += update_etat(next_trame);
    write_to_trame((~sum) + 1, 3, next_trame);
    current_trame = next_trame;
}

void init_ir() {
    // trame init
    trame0[0] = 1; // debut de la data à partir du bit 2
    write_to_trame(id_robot, 0, 0);
    update_ir();
    for (uint8_t i = 0; i < TRAME_SIZE; i++)
        trame1[i] = trame0[i]; // on copie tout ce qui est pareil et ne changera pas
    // IR SYNC 2
    LPC_PINCON->PINSEL3 &= 3 << 4; // P1.18 pour IRSync 2
    LPC_GPIO1->FIODIR   |= 1 << 18;
    // module init
    LPC_SC->PCLKSEL1    |= 1 << 15; // timer3 at 50MHz
    LPC_SC->PCONP       |= 1 << 23; // enable timer3
    LPC_PINCON->PINSEL0 |= 3 << 20; // P0.10 as MAT3.0

    LPC_TIM3->PR  = 49;     // to make timer3 at 1MHz, period of 1us
    LPC_TIM3->MR0 = 1;      // rapoprt cyclique de l'impulsion
    LPC_TIM3->MR1 = 30;     // periode de l'impulsion pas haute
    LPC_TIM3->EMR = 1 << 4; // EMR MR0 down
    LPC_TIM3->MCR = 1 << 3 | 1 << 4;
    NVIC_EnableIRQ(TIMER3_IRQn);
    NVIC_SetPriority(TIMER3_IRQn, 2);
    LPC_TIM3->TCR = 1 << 1;
    LPC_TIM3->TCR = 1;
}
