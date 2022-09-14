
#ifndef __INA219_H__
#define __INA219_H__
#ifdef __cplusplus
 extern "C" {
#endif

#include "hw.h"

void BSP_INA219_Init(void);
void INA219_Init (void);
void INA219_DeInit (void);
uint16_t INA219_read_bus_voltage(void);
uint16_t INA219_read_bus_current(void);

#ifdef __cplusplus
}
#endif
#endif /* __INA219_H__ */
