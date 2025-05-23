#include "LPC17xx.h"

#define IMPULSES_COUNT 20

static int8_t   nbpulse  = 1; // for init impulse to set 0 out
static uint32_t distance = 0; // UINT32_MAX to tell incoherent result.

void PWM1_IRQHandler() {
    LPC_PWM1->IR = 1 << 9; // Acquittement de l'IRQ de MR5
    if (--nbpulse <= 0) {
        LPC_PWM1->PCR &= ~(1 << 13);    // Disable PWM1.5 output
        LPC_PWM1->TCR &= ~(1 | 1 << 3); // Disable Counter
    }
}

void EINT3_IRQHandler() {
    LPC_TIM0->TCR &= ~(1 << 0); // Stop Timer
    LPC_SC->EXTINT = 1 << 3;    // Acquittement de l'IRQ
    distance       = (LPC_TIM0->TC * 344ul * 100ul) / (2ul * 25000000ul)
             + 2;     // vitesse du son, puis division par l'horloge et 2x
                      // distance et enfin conversion en CM
                      // puis offset de la distance entre le mic et le haut-parleur
    LPC_TIM0->TC = 0; // Reset compteur
}

void init_ultrasonic(void) { // UltraSonic Sensor
    // PINSEL
    LPC_PINCON->PINSEL3 |= 1 << 17; // P1.24 en mode PWM 1.5
    LPC_PINCON->PINSEL4 |= 1 << 26; // P2.13 en mode EINT3
    // init PWM5
    LPC_PWM1->MCR       |= (1 << 1) | (1 << 15); // Reset on MR0 and interrupt on MR5
    LPC_PWM1->MR0        = 625;                  // P¨¦riode = 40kHz
    LPC_PWM1->MR5        = 625 / 2;              // 50% rapport cyclique
    NVIC_EnableIRQ(PWM1_IRQn);
    // Putting PWM out at 0 level
    LPC_PWM1->PCR |= 1 << 13;      // Enable PWM5 output
    LPC_PWM1->TCR |= 1 | (1 << 3); // Enable PWM Counter and pwm
    while (LPC_PWM1->PCR & (1 << 13))
        ; // waiting for pwm to be disabled again
    // init EINT3
    LPC_SC->EXTMODE  |= (1 << 3);  // Mise en detection par front du EINT3
    LPC_SC->EXTPOLAR &= ~(1 << 3); // Mise en detection sur rising edge
    NVIC_EnableIRQ(EINT3_IRQn);    // Enable the EINT3
    LPC_TIM0->TC = 0;
}

void measure_ultrasonic(void) {
    // Lancer une mesure de distance par ultrason
    if (nbpulse != 0 || LPC_TIM0->TC != 0)
        return; // une mesure est d¨¦j¨¤ en cours

    nbpulse        = IMPULSES_COUNT;
    distance       = 0;            // to tell a measure is ongoing
    LPC_PWM1->PCR |= 1 << 13;      // Enable PWM5 output
    LPC_PWM1->TCR |= 1 | (1 << 3); // Enable PWM Counter and pwm
    LPC_TIM0->TCR |= 1;            // Enable Timer
}

uint32_t get_distance_ultrasonic(void) {
    return distance;
}
