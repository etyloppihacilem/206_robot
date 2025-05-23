/* #####################################################################################################################

               """          ultrasonic.h
        -\-    _|__
         |\___/  . \        Created on 18 May. 2025 at 22:03
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#pragma once

#include <stdint.h>

void init_ultrasonic(void);

void     measure_ultrasonic(void);
uint32_t get_distance_USS(void);
