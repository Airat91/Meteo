#ifndef PIN_MAP_H
#define PIN_MAP_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

#define CH_0_PORT       GPIOA
#define CH_0_PIN        GPIO_PIN_0
#define CH_1_PORT       GPIOA
#define CH_1_PIN        GPIO_PIN_1
#define CH_2_PORT       GPIOA
#define CH_2_PIN        GPIO_PIN_2
#define CH_3_PORT       GPIOA
#define CH_3_PIN        GPIO_PIN_3
#define PWR_PORT        GPIOA
#define PWR_PIN         GPIO_PIN_4

#define LED_CON_R_PORT  GPIOA
#define LED_CON_R_PIN   GPIO_PIN_6
#define LED_SYS_R_PORT  GPIOA
#define LED_SYS_R_PIN   GPIO_PIN_7

#define RS_485_TX_PORT  GPIOA
#define RS_485_TX_PIN   GPIO_PIN_9
#define RS_485_RX_PORT  GPIOA
#define RS_485_RX_PIN   GPIO_PIN_10
#define RS_485_DE_PORT  GPIOA
#define RS_485_DE_PIN   GPIO_PIN_11
#define SW1_PORT        GPIOA
#define SW1_PIN          GPIO_PIN_12
#define DEBUG_TMS_PORT  GPIOA
#define DEBUG_TMS_PIN   GPIO_PIN_13
#define DEBUG_TCK_PORT  GPIOA
#define DEBUG_TCK_PIN   GPIO_PIN_14
#define SW2_PORT        GPIOA
#define SW2_PIN         GPIO_PIN_15

#define LED_SYS_G_PORT  GPIOB
#define LED_SYS_G_PIN   GPIO_PIN_0
#define LED_CON_G_PORT  GPIOB
#define LED_CON_G_PIN   GPIO_PIN_1

#define UP_PORT         GPIOB
#define UP_PIN          GPIO_PIN_3
#define RIGHT_PORT      GPIOB
#define RIGHT_PIN       GPIO_PIN_4
#define LEFT_PORT       GPIOB
#define LEFT_PIN        GPIO_PIN_5
#define OK_PORT         GPIOB
#define OK_PIN          GPIO_PIN_6
#define DOWN_PORT       GPIOB
#define DOWN_PIN        GPIO_PIN_7
#define CAN_TX_PORT     GPIOB
#define CAN_TX_PIN      GPIO_PIN_8
#define CAN_RX_PORT     GPIOB
#define CAN_RX_PIN      GPIO_PIN_9
#define DISP_SDA_PORT        GPIOB
#define DISP_SDA_PIN         GPIO_PIN_10
#define DISP_SCL_PORT        GPIOB
#define DISP_SCL_PIN         GPIO_PIN_11
#define AM2302_PORT     GPIOB
#define AM2302_PIN      GPIO_PIN_12

#define LED_PORT        GPIOC
#define LED_PIN         GPIO_PIN_13

#endif // PIN_MAP_H
