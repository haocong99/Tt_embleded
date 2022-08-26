#ifndef __TEST_H
#define __TEST_H

#ifdef __cplusplus
 extern "C" {
#endif
	 /*----------------------------------------------------------------------------------------------*/
#include <stdint.h>
	 
#include "main.h"
#include "stm32f0xx_hal.h"
	 
int8_t 	test_init(void);
int8_t test_read_write_flash(void);
int8_t test_function(void);
#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */
