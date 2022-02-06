// Library for AM2302
// Ver_2.3

/*========== LIBRARY DESCRIPTION ==========
- Library use STM32F3xx_HAL_Driver 
*/

#ifndef am2302_H_
#define am2302_H_
#include "stm32f1xx_hal.h"

/*========== TYPEDEFS ==========*/

/**
  * @brief Struct for AM2302 data
  * @ingroup am2302
  */
typedef struct {
  int16_t hum;
  int16_t tmpr;
  uint8_t paritet;
  uint8_t error;
  uint8_t lost_con_cnt;
  uint32_t recieved;
  uint32_t lost;
} am2302_data_t;

/**
  * @brief Struct for am2302 pins
  * @ingroup am2302
  */
typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
}am2302_pin_t;

//========== VARIABLES ==========

volatile extern uint8_t am2302_timeout;
extern const am2302_pin_t am2302_pin[];

//========== FUNCTIONS PROTOTYPES ==========

int am2302_init (void);
void am2303_deinit(void);
int am2302_get (GPIO_TypeDef* port, uint16_t pin, am2302_data_t* data);
int am2302_send(GPIO_TypeDef* port, uint16_t pin, am2302_data_t data);
int am2302_get_rtc(GPIO_TypeDef* port, uint16_t pin, am2302_data_t* data);

#endif /* am2302_H_ */
