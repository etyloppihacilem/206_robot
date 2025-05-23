#include "ultrasonic.h"
#include "LPC17xx.h"
#include "core_cmFunc.h"
#include "params.h"

#define IMPULSES_COUNT 20

uint32_t reset_cpt      = 4000; // 5Hz
int32_t  ultrasonic_cpt = 0;

static int8_t   nbpulse    = 1; // for init impulse to set 0 out
static uint32_t time_count = 0;
static uint32_t distance   = 0; // UINT32_MAX to tell incoherent result.

void PWM1_IRQHandler() {
    if (LPC_PWM1->IR & 1 << 9) {
        LPC_PWM1->IR = 1 << 9; // Acquittement de l'IRQ de MR5
        if (--nbpulse <= 0) {
            LPC_PWM1->PCR &= ~(1 << 13); // Disable PWM1.5 output
            // LPC_PWM1->TCR &= ~(1 | 1 << 3); // Disable Counter
        }
    }
    if (LPC_PWM1->IR & 1) {
        LPC_PWM1->IR = 1;
        time_count++;
        if (ultrasonic_cpt-- <= 0) {
            measure_ultrasonic();
            ultrasonic_cpt = reset_cpt;
        }
        // ici gérer l'émission de BIP si nécessaire.
    }
}

void EINT3_IRQHandler() {
    LPC_SC->EXTINT = 1 << 3; // Acquittement de l'IRQ
    distance       = (time_count * 344ul * 100ul) / (2ul * 40000ul) + 2;
    // vitesse du son, puis division par la fréquence PWM et 2x distance et enfin conversion en CM puis offset de la
    // distance entre le mic et le haut-parleur
}

void init_ultrasonic(void) { // UltraSonic Sensor
    switch (mesure_tele) {
        default:
        case Hz10:
            reset_cpt = 2000;
            break;
        case Hz15:
            reset_cpt = 1334;
            break;
        case Hz20:
            reset_cpt = 1000;
            break;
        case Hz25:
            reset_cpt = 800;
    }
    ultrasonic_cpt       = reset_cpt;
    // PINSEL
    LPC_PINCON->PINSEL3 |= 1 << 17; // P1.24 en mode PWM 1.5
    LPC_PINCON->PINSEL4 |= 1 << 26; // P2.13 en mode EINT3
    // init PWM5
    LPC_PWM1->MCR       |= (1 << 1) | (1 << 15); // Reset on MR0 and interrupt on MR5
    LPC_PWM1->MR0        = 625;                  // Période = 40kHz
    LPC_PWM1->MR5        = 625 / 2;              // 50% rapport cyclique
    NVIC_EnableIRQ(PWM1_IRQn);
    NVIC_SetPriority(PWM1_IRQn, 5);
    // Putting PWM out at 0 level
    LPC_PWM1->PCR |= 1 << 13;      // Enable PWM5 output
    LPC_PWM1->TCR |= 1 | (1 << 3); // Enable PWM Counter and pwm
    while (LPC_PWM1->PCR & (1 << 13))
        ; // waiting for pwm to be disabled again
    // init EINT3
    LPC_SC->EXTMODE  |= (1 << 3);  // Mise en detection par front du EINT3
    LPC_SC->EXTPOLAR &= ~(1 << 3); // Mise en detection sur rising edge
    NVIC_EnableIRQ(EINT3_IRQn);    // Enable the EINT3
    NVIC_SetPriority(EINT3_IRQn, 4);
}

void measure_ultrasonic(void) {
    // Lancer une mesure de distance par ultrason
    if (nbpulse != 0 || LPC_TIM0->TC != 0)
        return; // une mesure est déjà en cours

    nbpulse = IMPULSES_COUNT;
    // distance       = 0; // to tell a measure is ongoing
    __disable_irq();
    time_count = 0;
    __enable_irq();
    LPC_PWM1->PCR |= 1 << 13;  // Enable PWM5 output
    LPC_PWM1->TCR |= (1 << 3); // Enable PWM Counter and pwm
}

uint32_t get_distance_ultrasonic(void) {
    return distance;
}
