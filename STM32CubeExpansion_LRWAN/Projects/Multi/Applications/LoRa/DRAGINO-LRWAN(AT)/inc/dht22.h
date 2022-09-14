#ifndef __DHT22_H__
#define __DHT22_H__
#ifdef __cplusplus
 extern "C" {
#endif

#include "hw.h"

void DHT22_Start (void);
uint8_t DHT22_Check_Response (void);
uint8_t DHT22_Read (void);

#ifdef __cplusplus
}
#endif
#endif /* __DHT22_H__ */
