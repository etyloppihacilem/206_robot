/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          com_debug.h
        -\-    _|__
         |\___/  . \        Created on 23 May. 2025 at 15:16
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#pragma once

#include <stdint.h>

void init_com_debug(uint32_t baudrate);
void debug_write(const char *str);
void debug_put_int(int32_t n);
void debug_put_uint(uint32_t n);
int  uart0_putchar(int c); // peut être bufferiser ça si les prints prennent trop de temps.
void debug_put_hex(uint32_t n);
