/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          com_debug.c
        -\-    _|__
         |\___/  . \        Created on 23 May. 2025 at 15:16
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "com_debug.h"
#include "LPC17xx.h"
#include <stdint.h>

static uint32_t get_uart0_pclk(void) {
    uint32_t             sel        = (LPC_SC->PCLKSEL0 >> 8) & 0x03;
    static const uint8_t div_lut[4] = { 4, 1, 2, 8 };
    return SystemCoreClock / div_lut[sel];
}

/*
 * Il faut une précision de l'horloge de UART0 pour pouvoir monter à 115200 bauds.
 * Pour 9600, pas besoin d'un calcul si compliqué.
 * */
void set_uart0_baud(uint32_t baud) {
    uint32_t pclk     = get_uart0_pclk();
    uint32_t best_dll = 0, best_mul = 1, best_divadd = 0;
    uint32_t best_err = 0xFFFFFFFF;

    for (uint32_t mul = 1; mul <= 15; ++mul) {
        for (uint32_t divadd = 0; divadd < mul; ++divadd) {
            uint32_t dll = (pclk + 8 * baud * mul) / (16 * baud * mul) - divadd; // arrondi
            if (dll == 0 || dll > 0xFFFF)
                continue;

            uint32_t real = pclk / (16 * (dll + ((float) divadd / mul)));
            uint32_t err  = (real > baud) ? real - baud : baud - real;

            if (err < best_err) {
                best_err    = err;
                best_dll    = dll;
                best_mul    = mul;
                best_divadd = divadd;
            }
        }
    }

    LPC_UART0->LCR |= 0x80; // DLAB = 1
    LPC_UART0->DLM  = best_dll >> 8;
    LPC_UART0->DLL  = best_dll & 0xFF;
    LPC_UART0->FDR  = (best_mul << 4) | best_divadd;
    LPC_UART0->LCR &= ~0x80; // DLAB = 0
}

void init_com_debug(uint32_t baudrate) {
    LPC_SC->PCONP |= (1 << 3); // on active l'horloge et le power de UART 0

    LPC_PINCON->PINSEL0 &= ~((3 << 4) | (3 << 6));
    LPC_PINCON->PINSEL0 |= (1 << 4) | (1 << 6); // P0.2 = TXD0, P0.3 = RXD0

    // Désactiver interruptions pendant init
    LPC_UART0->IER = 0;

    // 8 bits, 1 stop bit, pas de parité
    set_uart0_baud(baudrate);

    LPC_UART0->LCR = 0x03; // 8 bits transmit
    LPC_UART0->FCR = 0x07; // Activer FIFO, reset RX/TX

    // Attente que tout soit prêt
    while (!(LPC_UART0->LSR & 0x20))
        ;
}

int uart0_putchar(int c) { // peut être bufferiser ça si les prints prennent trop de temps.
    while (!(LPC_UART0->LSR & 0x20))
        ; // attente THRE
    LPC_UART0->THR = c;
    return c;
}

void debug_write(const char *str) {
    while (*str != '\0')
        uart0_putchar(*(str++));
}

void debug_put_int(int32_t n) {
    if (n < 0) {
        uart0_putchar('-');
        n *= -1;
    }
    if (n >= 10)
        debug_put_int(n / 10);
    uart0_putchar((n % 10) + '0');
}

void debug_put_uint(uint32_t n) {
    if (n >= 10)
        debug_put_uint(n / 10);
    uart0_putchar((n % 10) + '0');
}

void debug_put_hex(uint32_t n) {
    if (n >= 16)
        debug_put_uint(n / 10);
    char c = n % 16;
    if (c < 10)
        uart0_putchar(c + '0');
    else
        uart0_putchar(c - 10 + 'A');
}
