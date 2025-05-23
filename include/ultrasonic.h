/* #####################################################################################################################

               """          ultrasonic.h
        -\-    _|__
         |\___/  . \        Created on 18 May. 2025 at 22:03
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#ifndef INCLUDE_INCLUDE_ULTRASONIC_H_
#define INCLUDE_INCLUDE_ULTRASONIC_H_

#include <stdint.h>

void init_ultrasonic(void);

void     measure_ultrasonic(void);
uint32_t get_distance_USS(void);

#endif // INCLUDE_INCLUDE_ULTRASONIC_H_
