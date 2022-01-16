
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "stdlib.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal_gpio.h"
#include "dcts.h"
#include "dcts_config.h"
#include "pin_map.h"
#include "buttons.h"
//#include "LCD.h"
#include "adc.h"
#include "portable.h"
#include "am2302.h"
#include "menu.h"
#include "flash.h"
#include "uart.h"
#include "modbus.h"
#include "time.h"
#include "display.h"
//#include "ds18.h"

/**
  * @defgroup MAIN
  */

#define FEEDER 0
#define DEFAULT_TASK_PERIOD 100
#define RELEASE 0
#define DO_NUM 6
#define IN_CHANNEL_NUM 8
#define RTC_KEY 0xABCD

#define RTC_HAL     1
#define RTC_UNIX    2
#define RTC_TIME    RTC_UNIX //(RTC_HAL or RTC_UNIX)

#define LCD_DISP 1
#define ST7735_DISP 2
#define DISP LCD_DISP // LCD_DISP or ST7735_DISP
#define DISP_MAX_LINES  8

#if (DISP == ST7735_DISP)
    #include "st7735.h"
#endif

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef hpwmtim;
IWDG_HandleTypeDef hiwdg;
osThreadId rtcTaskHandle;
osThreadId buttonsTaskHandle;
osThreadId displayTaskHandle;
osThreadId menuTaskHandle;
osThreadId controlTaskHandle;
osThreadId adcTaskHandle;
osThreadId am2302TaskHandle;
osThreadId ds18TaskHandle;
osThreadId navigationtTaskHandle;
osThreadId uartTaskHandle;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
//static void MX_GPIO_Init(void);
void dcts_init (void);
static void channels_init(void);
static void MX_IWDG_Init(void);
static void RTC_Init(void);
static int RTC_write_cnt(time_t cnt_value);
//static void MX_ADC1_Init(void);
//static void MX_USART1_UART_Init(void);
static void print_header(void);
static void main_page_print(u8 tick);
static void menu_page_print(u8 tick);
static void value_print(u8 tick);
static void error_page_print(menu_page_t page);
static void save_page_print (u8 tick);
static void info_print (void);
/*static void meas_channels_print(void);
static void calib_print(uint8_t start_channel);
static void mdb_print(void);
static void display_print(void);*/
static int get_param_value(char* string, menu_page_t page);
static void set_edit_value(menu_page_t page);
static void print_back(void);
static void print_enter_right(void);
static void print_enter_ok(void);
static void print_change(void);
static void save_params(void);
//static void restore_params(void);
static void save_to_bkp(u8 bkp_num, uint16_t var);
//static void save_float_to_bkp(u8 bkp_num, float var);
static uint16_t read_bkp(u8 bkp_num);
//static float read_float_bkp(u8 bkp_num, u8 sign);
static int channel_PWM_timer_init(u8 channel);
static int channel_PWM_duty_set(u8 channel, float duty);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

uint32_t us_cnt_H = 0;
navigation_t navigation_style = MENU_NAVIGATION;
edit_val_t edit_val = {0};
saved_to_flash_t config;
//bkp_data_t * bkp_data_p;
static const char off_on_descr[2][10] = {
    "Выкл.",
    "Вкл.",
};
static const char manual_auto_descr[2][10] = {
    "Ручной",
    "Авто",
};

ch_config_t ch_config[CH_NUM] = {
    {.mode = &config.params.ch_mode[0], .port = CH_0_PORT, .pin = CH_0_PIN, .adc_num = ADC1, .adc_channel = ADC_CHANNEL_0, .pwm_tim = TIM2, .pwm_channel = TIM_CHANNEL_1},
    {.mode = &config.params.ch_mode[1], .port = CH_1_PORT, .pin = CH_1_PIN, .adc_num = ADC1, .adc_channel = ADC_CHANNEL_1, .pwm_tim = TIM2, .pwm_channel = TIM_CHANNEL_2},
    {.mode = &config.params.ch_mode[2], .port = CH_2_PORT, .pin = CH_2_PIN, .adc_num = ADC1, .adc_channel = ADC_CHANNEL_2, .pwm_tim = TIM2, .pwm_channel = TIM_CHANNEL_3},
    {.mode = &config.params.ch_mode[3], .port = CH_3_PORT, .pin = CH_3_PIN, .adc_num = ADC1, .adc_channel = ADC_CHANNEL_3, .pwm_tim = TIM2, .pwm_channel = TIM_CHANNEL_4},
};

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void){

    HAL_Init();
    SystemClock_Config();
    //channels_init();
    tim2_init();
    dcts_init();
    restore_params();
    //led_lin_init();
    refresh_watchdog();

    osThreadDef(rtc_task, rtc_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    rtcTaskHandle = osThreadCreate(osThread(rtc_task), NULL);

    osThreadDef(display_task, display_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
    displayTaskHandle = osThreadCreate(osThread(display_task), NULL);

    /*osThreadDef(adc_task, adc_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*3);
    adcTaskHandle = osThreadCreate(osThread(adc_task), NULL);*/

    osThreadDef(buttons_task, buttons_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    buttonsTaskHandle = osThreadCreate(osThread(buttons_task), NULL);

    osThreadDef(am2302_task, am2302_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    am2302TaskHandle = osThreadCreate(osThread(am2302_task), NULL);

    osThreadDef(navigation_task, navigation_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    navigationtTaskHandle = osThreadCreate(osThread(navigation_task), NULL);

    /*osThreadDef(uart_task, uart_task, osPriorityHigh, 0, configMINIMAL_STACK_SIZE*4);
    uartTaskHandle = osThreadCreate(osThread(uart_task), NULL);*/

    /*osThreadDef(control_task, control_task, osPriorityHigh, 0, configMINIMAL_STACK_SIZE*2);
    controlTaskHandle = osThreadCreate(osThread(control_task), NULL);*/

    /*osThreadDef(ds18_task, ds18_task, osPriorityHigh, 0, configMINIMAL_STACK_SIZE*2);
    ds18TaskHandle = osThreadCreate(osThread(ds18_task), NULL);*/

    /* Start scheduler */
    osKernelStart();

    while (1)  {

    }

}

void dcts_init (void) {

    dcts.dcts_id = DCTS_ID_COMBINED;
    strcpy (dcts.dcts_ver, "0.1.0");
    strcpy (dcts.dcts_name, "Quadro");
    strcpy (dcts.dcts_name_cyr, "Квадро");
    dcts.dcts_address = 0x10;
    dcts.dcts_rtc.day = 1;
    dcts.dcts_rtc.month = 1;
    dcts.dcts_rtc.year = 2020;
    dcts.dcts_rtc.weekday = 6;
    dcts.dcts_rtc.hour = 12;
    dcts.dcts_rtc.minute = 0;
    dcts.dcts_rtc.second = 0;
    dcts.dcts_pwr = 0.0f;
    dcts.dcts_meas_num = MEAS_NUM;
    dcts.dcts_rele_num = RELE_NUM;
    dcts.dcts_act_num  = ACT_NUM;
    dcts.dcts_alrm_num = ALRM_NUM;

    //meas_channels

    dcts_meas_channel_init(TMPR_SELF,   "Tmpr",         "Температура",  "°C",   "°C");
    dcts_meas_channel_init(HUM_SELF,    "Hum",          "Влажнсть",     "%",    "%");
    dcts_meas_channel_init(VREF_VLT,    "Vref V",       "Опорное напр.","V",    "В");
    dcts_meas_channel_init(VBAT_VLT,    "RTC battery V","Батарейка",    "V",    "В");

    dcts_meas_channel_init(CH_0_TMPR,   "CH_0 Tmpr",    "Кан_0 Темп.",  "°C",   "°C");
    dcts_meas_channel_init(CH_0_HUM,    "CH_0 Hum",     "Кан_0 Влажн.", "%",    "%");
    dcts_meas_channel_init(CH_0_ADC,    "CH_0 ADC",     "Кан_0 АЦП",    "adc",  "ацп");
    dcts_meas_channel_init(CH_0_VLT,    "CH_0 VLT",     "Кан_0 Напр.",  "V",    "В");
    dcts_meas_channel_init(CH_0_CNT,    "CH_0 CNT",     "Кан_0 Имп.",   "pls",  "имп");
    dcts_meas_channel_init(CH_0_DI,     "CH_0 DI",      "Кан_0 Лог.",   "log",  "лог");

    dcts_meas_channel_init(CH_1_TMPR,   "CH_1 Tmpr",    "Кан_1 Темп.",  "°C",   "°C");
    dcts_meas_channel_init(CH_1_HUM,    "CH_1 Hum",     "Кан_1 Влажн.", "%",    "%");
    dcts_meas_channel_init(CH_1_ADC,    "CH_1 ADC",     "Кан_1 АЦП",    "adc",  "ацп");
    dcts_meas_channel_init(CH_1_VLT,    "CH_1 VLT",     "Кан_1 Напр.",  "V",    "В");
    dcts_meas_channel_init(CH_1_CNT,    "CH_1 CNT",     "Кан_1 Имп.",   "pls",  "имп");
    dcts_meas_channel_init(CH_1_DI,     "CH_1 DI",      "Кан_1 Лог.",   "log",  "лог");

    dcts_meas_channel_init(CH_2_TMPR,   "CH_2 Tmpr",    "Кан_2 Темп.",  "°C",   "°C");
    dcts_meas_channel_init(CH_2_HUM,    "CH_2 Hum",     "Кан_2 Влажн.", "%",    "%");
    dcts_meas_channel_init(CH_2_ADC,    "CH_2 ADC",     "Кан_2 АЦП",    "adc",  "ацп");
    dcts_meas_channel_init(CH_2_VLT,    "CH_2 VLT",     "Кан_2 Напр.",  "V",    "В");
    dcts_meas_channel_init(CH_2_CNT,    "CH_2 CNT",     "Кан_2 Имп.",   "pls",  "имп");
    dcts_meas_channel_init(CH_2_DI,     "CH_2 DI",      "Кан_2 Лог.",   "log",  "лог");

    dcts_meas_channel_init(CH_3_TMPR,   "CH_3 Tmpr",    "Кан_3 Темп.",  "°C",   "°C");
    dcts_meas_channel_init(CH_3_HUM,    "CH_3 Hum",     "Кан_3 Влажн.", "%",    "%");
    dcts_meas_channel_init(CH_3_ADC,    "CH_3 ADC",     "Кан_3 АЦП",    "adc",  "ацп");
    dcts_meas_channel_init(CH_3_VLT,    "CH_3 VLT",     "Кан_3 Напр.",  "V",    "В");
    dcts_meas_channel_init(CH_3_CNT,    "CH_3 CNT",     "Кан_3 Имп.",   "pls",  "имп");
    dcts_meas_channel_init(CH_3_DI,     "CH_3 DI",      "Кан_3 Лог.",   "log",  "лог");


    //act_channels

    dcts_act_channel_init(CH_0_DSCR_CTRL,   "CH_0 Rele out",    "Кан_0 Дискр. упр.","",     "");
    dcts_act_channel_init(CH_0_PWM,    "CH_0 PWM out",     "Кан_0 ШИМ упр.",   "%",    "%");

    dcts_act_channel_init(CH_1_DSCR_CTRL,   "CH_1 Rele out",    "Кан_1 Дискр. упр.","",     "");
    dcts_act_channel_init(CH_1_PWM,    "CH_1 PWM out",     "Кан_1 ШИМ упр.",   "%",    "%");

    dcts_act_channel_init(CH_2_DSCR_CTRL,   "CH_2 Rele out",    "Кан_2 Дискр. упр.","",     "");
    dcts_act_channel_init(CH_2_PWM,    "CH_2 PWM out",     "Кан_2 ШИМ упр.",   "%",    "%");

    dcts_act_channel_init(CH_3_DSCR_CTRL,   "CH_3 Rele out",    "Кан_3 Дискр. упр.","",     "");
    dcts_act_channel_init(CH_3_PWM,    "CH_3 PWM out",     "Кан_3 ШИМ упр.",   "%",    "%");

    //rele_channels

    dcts_rele_channel_init(CH_0_OUT, "CH_0 Out", "Кан_0 Выход");
    dcts_rele_channel_init(CH_1_OUT, "CH_1 Out", "Кан_0 Выход");
    dcts_rele_channel_init(CH_2_OUT, "CH_2 Out", "Кан_0 Выход");
    dcts_rele_channel_init(CH_3_OUT, "CH_3 Out", "Кан_0 Выход");
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
            |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    /**Configure the Systick interrupt time
    */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* RTC init function */
static void RTC_Init(void){
#if (RTC_TIME == RTC_HAL)
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
#elif (RTC_TIME == RTC_UNIX)
    time_t unix_time = 0;
    struct tm system_time = {0};
#endif // RTC_TIME
    __HAL_RCC_BKP_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_RTC_ENABLE();
    hrtc.Instance = RTC;
    hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        _Error_Handler(__FILE__, __LINE__);
    }

    uint16_t read = read_bkp(0);
    if(read == RTC_KEY){
        dcts.dcts_rtc.state = RTC_STATE_READY;
    }else{
        save_to_bkp(0, RTC_KEY);
        dcts.dcts_rtc.state = RTC_STATE_SET;
    }
    if(dcts.dcts_rtc.state == RTC_STATE_SET){
#if (RTC_TIME == RTC_HAL)
        sTime.Hours = dcts.dcts_rtc.hour;
        sTime.Minutes = dcts.dcts_rtc.minute;
        sTime.Seconds = dcts.dcts_rtc.second;

        sDate.Date = dcts.dcts_rtc.day;
        sDate.Month = dcts.dcts_rtc.month;
        sDate.Year = (uint8_t)(dcts.dcts_rtc.year - 2000);

        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
#elif (RTC_TIME == RTC_UNIX)

        system_time.tm_hour = dcts.dcts_rtc.hour;
        system_time.tm_min = dcts.dcts_rtc.minute;
        system_time.tm_sec = dcts.dcts_rtc.second;

        system_time.tm_mday = dcts.dcts_rtc.day;
        system_time.tm_mon = dcts.dcts_rtc.month - 1;
        system_time.tm_year = dcts.dcts_rtc.year - 1900;

        unix_time = mktime(&system_time);

        RTC_write_cnt(unix_time);
#endif //RTC_TIME
        dcts.dcts_rtc.state = RTC_STATE_READY;
    }
}
/**
 * @brief RTC_write_cnt
 * @param cnt_value - time in unix format
 * @return  0 - OK,\n
 *          -1 - timeout error,\n
 *          -2 - timeout error
 */
static int RTC_write_cnt(time_t cnt_value){
    int result = 0;
    u32 start = HAL_GetTick();
    u32 timeout = 0;
    PWR->CR |= PWR_CR_DBP;                                          //разрешить доступ к Backup области
    while ((!(RTC->CRL & RTC_CRL_RTOFF))&&(timeout <= start + 500)){//проверить закончены ли изменения регистров RTC
        osDelay(1);
        timeout++;
    }
    if(timeout > start + 500){
        result = -1;
    }
    RTC->CRL |= RTC_CRL_CNF;                                        //Разрешить Запись в регистры RTC
    RTC->CNTH = (u32)cnt_value>>16;                                 //записать новое значение счетного регистра
    RTC->CNTL = (u32)cnt_value;
    RTC->CRL &= ~RTC_CRL_CNF;                                       //Запретить запись в регистры RTC
    start = HAL_GetTick();
    while ((!(RTC->CRL & RTC_CRL_RTOFF))&&(timeout <= start + 500)){//Дождаться окончания записи
        osDelay(1);
        timeout++;
    }
    if(timeout > start + 500){
        result = -2;
    }
    PWR->CR &= ~PWR_CR_DBP;                                         //запретить доступ к Backup области
    return result;
}
/**
 * @brief RTC task
 * @param argument - None
 * @todo add group
 */
#define RTC_TASK_PERIOD 500
void rtc_task(void const * argument){
    (void)argument;
#if (RTC_TIME == RTC_HAL)
    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};
#elif (RTC_TIME == RTC_UNIX)
    time_t unix_time = 0;
    struct tm system_time = {0};
#endif // RTC_TIME
    RTC_Init();
    uint32_t last_wake_time = osKernelSysTick();
    while(1){
        switch (dcts.dcts_rtc.state) {
        case RTC_STATE_READY:   //update dcts_rtc from rtc
#if (RTC_TIME == RTC_HAL)
            HAL_RTC_GetDate(&hrtc,&date,RTC_FORMAT_BIN);
            HAL_RTC_GetTime(&hrtc,&time,RTC_FORMAT_BIN);
            taskENTER_CRITICAL();
            dcts.dcts_rtc.hour = time.Hours;
            dcts.dcts_rtc.minute = time.Minutes;
            dcts.dcts_rtc.second = time.Seconds;

            dcts.dcts_rtc.day = date.Date;
            dcts.dcts_rtc.month = date.Month;
            dcts.dcts_rtc.year = date.Year + 2000;
            dcts.dcts_rtc.weekday = date.WeekDay;
            taskEXIT_CRITICAL();
#elif (RTC_TIME == RTC_UNIX)
            unix_time = (time_t)(RTC->CNTL);
            unix_time |= (time_t)(RTC->CNTH<<16);
            system_time = *localtime(&unix_time);

            taskENTER_CRITICAL();
            dcts.dcts_rtc.hour      = (u8)system_time.tm_hour;
            dcts.dcts_rtc.minute    = (u8)system_time.tm_min;
            dcts.dcts_rtc.second    = (u8)system_time.tm_sec;

            dcts.dcts_rtc.day       = (u8)system_time.tm_mday;
            dcts.dcts_rtc.month     = (u8)system_time.tm_mon + 1;
            dcts.dcts_rtc.year      = (u8)system_time.tm_year + 1900;
            if(system_time.tm_wday == 0){
                dcts.dcts_rtc.weekday = 7;
            }else{
                dcts.dcts_rtc.weekday   = (u8)system_time.tm_wday;
            }

            taskEXIT_CRITICAL();
#endif // RTC_TIME
            break;
        case RTC_STATE_SET:     //set new values from dcts_rtc
#if (RTC_TIME == RTC_HAL)
            time.Hours = dcts.dcts_rtc.hour;
            time.Minutes = dcts.dcts_rtc.minute;
            time.Seconds = dcts.dcts_rtc.second;

            date.Date = dcts.dcts_rtc.day;
            date.Month = dcts.dcts_rtc.month;
            date.Year = (uint8_t)(dcts.dcts_rtc.year - 2000);

            HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
            HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
#elif (RTC_TIME == RTC_UNIX)
            system_time.tm_hour = dcts.dcts_rtc.hour;
            system_time.tm_min  = dcts.dcts_rtc.minute;
            system_time.tm_sec  = dcts.dcts_rtc.second;

            system_time.tm_mday = dcts.dcts_rtc.day;
            system_time.tm_mon  = dcts.dcts_rtc.month;
            system_time.tm_year = dcts.dcts_rtc.year - 1900;

            unix_time = mktime(&system_time);

            RTC_write_cnt(unix_time);
#endif // RTC_TIME
            dcts.dcts_rtc.state = RTC_STATE_READY;
            break;
        default:
            break;
        }
        refresh_watchdog();
        osDelayUntil(&last_wake_time, RTC_TASK_PERIOD);
    }
}


/* RTC init function */
/*static void MX_RTC_Init(void){
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    __HAL_RCC_BKP_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_RTC_ENABLE();
    hrtc.Instance = RTC;
    hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        _Error_Handler(__FILE__, __LINE__);
    }

    u32 data;
    const  u32 data_c = 0x1234;
    data = BKP->DR1;
    if(data!=data_c){   // set default values
        HAL_PWR_EnableBkUpAccess();
        BKP->DR1 = data_c;
        HAL_PWR_DisableBkUpAccess();

        sTime.Hours = dcts.dcts_rtc.hour;
        sTime.Minutes = dcts.dcts_rtc.minute;
        sTime.Seconds = dcts.dcts_rtc.second;

        sDate.Date = dcts.dcts_rtc.day;
        sDate.Month = dcts.dcts_rtc.month;
        sDate.Year = (uint8_t)(dcts.dcts_rtc.year - 2000);

        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
        HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    }else{  // read data from bkpram
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        dcts.dcts_rtc.hour = sTime.Hours;
        dcts.dcts_rtc.minute = sTime.Minutes;
        dcts.dcts_rtc.second = sTime.Seconds;

        dcts.dcts_rtc.day = sDate.Date;
        dcts.dcts_rtc.month = sDate.Month;
        dcts.dcts_rtc.year = sDate.Year + 2000;
        dcts.dcts_rtc.weekday = sDate.WeekDay;
    }
}*/


/**
 * @brief default_task
 * @param argument - None
 * @todo add group
 */
/*void default_task(void const * argument){

    (void)argument;
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    uint32_t last_wake_time = osKernelSysTick();

    //HAL_IWDG_Refresh(&hiwdg);
    while(1){
        HAL_RTC_GetDate(&hrtc,&date,RTC_FORMAT_BIN);
        HAL_RTC_GetTime(&hrtc,&time,RTC_FORMAT_BIN);

        dcts.dcts_rtc.hour = time.Hours;
        dcts.dcts_rtc.minute = time.Minutes;
        dcts.dcts_rtc.second = time.Seconds;

        dcts.dcts_rtc.day = date.Date;
        dcts.dcts_rtc.month = date.Month;
        dcts.dcts_rtc.year = date.Year + 2000;
        dcts.dcts_rtc.weekday = date.WeekDay;

        //HAL_IWDG_Refresh(&hiwdg);
        osDelayUntil(&last_wake_time, DEFAULT_TASK_PERIOD);
    }
}*/

/**
 * @brief display_task
 * @param argument
 */
#if(DISP == LCD_DISP)
#define display_task_period 100
#elif(DISP == ST7735_DISP)
#define display_task_period 250
#endif // DISP
/*void display_task(void const * argument){
    (void)argument;
    menu_init();
#if(DISP == LCD_DISP)
    LCD_init();
#elif(DISP == ST7735_DISP)
    LCD.auto_off = config.params.lcd_backlight_time;
    LCD.backlight_lvl = config.params.lcd_backlight_lvl;
    LCD_backlight_timer_init();
    LCD_backlight_on();
    st7735_init();
    st7735_fill_rect(0,0,160,128,ST7735_WHITE);
#endif
    u8 tick = 0;
    u8 tick_2 = 0;
    menu_page_t last_page = selectedMenuItem->Page;
    uint32_t last_wake_time = osKernelSysTick();
    while(1){
        refresh_watchdog();
#if(DISP == LCD_DISP)
        LCD_clr();
#elif(DISP == ST7735_DISP)
        if(last_page != selectedMenuItem->Page){
            st7735_fill_rect(0,0,160,128,ST7735_WHITE);
            last_page = selectedMenuItem->Page;
        }
#endif
        if(last_page != selectedMenuItem->Page){
            tick = 0;
            last_page = selectedMenuItem->Page;
        }
        switch (selectedMenuItem->Page) {
        case MAIN_PAGE:
            main_page_print(tick);
            break;
        case INFO:
            info_print();
            break;
        case SAVE_CHANGES:
            save_page_print(tick);
            break;
        default:
            if(selectedMenuItem->Child_num > 0){
                menu_page_print(tick);
            }else if(selectedMenuItem->Child_num == 0){
                value_print(tick);
            }
        }

#if(DISP == LCD_DISP)
        LCD_update();
#elif(DISP == ST7735_DISP)
        //st7735_update();
#endif
        if((LCD.auto_off != 0)&&(LCD.backlight == LCD_BACKLIGHT_ON)){
            LCD.auto_off_timeout += display_task_period;
            if(LCD.auto_off_timeout > (uint32_t)LCD.auto_off * 10000){
                LCD.auto_off_timeout = 0;
                LCD_backlight_shutdown();
            }
        }
        if(tick_2 == 500/display_task_period){
            tick_2 = 0;
            tick++;
        }
        tick_2++;
        osDelayUntil(&last_wake_time, display_task_period);
    }
}*/
/*
#define BUTTON_PRESS_TIME 1000
#define BUTTON_PRESS_TIMEOUT 10000
#define BUTTON_CLICK_TIME 10
#define navigation_task_period 20
void navigation_task (void const * argument){
    (void)argument;
    u16 timeout = 0;
    uint32_t last_wake_time = osKernelSysTick();
    while(1){
        switch (navigation_style){
        case MENU_NAVIGATION:
            if(button_click(BUTTON_UP, BUTTON_CLICK_TIME)){
                menuChange(selectedMenuItem->Previous);
            }
            if(button_click(BUTTON_DOWN, BUTTON_CLICK_TIME)){
                menuChange(selectedMenuItem->Next);
            }
            if(button_click(BUTTON_LEFT, BUTTON_CLICK_TIME)){
                menuChange(selectedMenuItem->Parent);
            }
            if(button_click(BUTTON_RIGHT, BUTTON_CLICK_TIME)){
                menuChange(selectedMenuItem->Child);
            }
            if(button_click(BUTTON_OK, BUTTON_CLICK_TIME)){
                menuChange(selectedMenuItem->Child);
            }
            break;
        case DIGIT_EDIT:
            switch (selectedMenuItem->Page){
            case TIME_HOUR:
            case TIME_MIN:
            case TIME_SEC:
            case DATE_DAY:
            case DATE_MONTH:
            case DATE_YEAR:
                dcts.dcts_rtc.state = RTC_STATE_EDIT;
                break;
            }
            if(button_click(BUTTON_UP,BUTTON_CLICK_TIME)){
                // increment value
                switch(edit_val.type){
                case VAL_INT8:
                    if(*edit_val.p_val.p_int8 < edit_val.val_max.int8){
                        *edit_val.p_val.p_int8 += (int8_t)uint32_pow(10, (uint8_t)edit_val.digit);
                    }
                    if((*edit_val.p_val.p_int8 > edit_val.val_max.int8)||(*edit_val.p_val.p_int8 < edit_val.val_min.int8)){ //if out of range
                        *edit_val.p_val.p_int8 = edit_val.val_max.int8;
                    }
                    break;
                case VAL_UINT8:
                    if(*edit_val.p_val.p_uint8 < edit_val.val_max.uint8){
                        *edit_val.p_val.p_uint8 += (uint8_t)uint32_pow(10, (uint8_t)edit_val.digit);
                    }
                    if((*edit_val.p_val.p_uint8 > edit_val.val_max.uint8)||(*edit_val.p_val.p_uint8 < edit_val.val_min.uint8)){ //if out of range
                        *edit_val.p_val.p_uint8 = edit_val.val_max.uint8;
                    }
                    break;
                case VAL_INT16:
                    if(*edit_val.p_val.p_int16 < edit_val.val_max.int16){
                        *edit_val.p_val.p_int16 += (int16_t)uint32_pow(10, (uint8_t)edit_val.digit);
                    }
                    if((*edit_val.p_val.p_int16 > edit_val.val_max.int16)||(*edit_val.p_val.p_int16 < edit_val.val_min.int16)){ //if out of range
                        *edit_val.p_val.p_int16 = edit_val.val_max.int16;
                    }
                    break;
                case VAL_UINT16:
                    if(*edit_val.p_val.p_uint16 < edit_val.val_max.uint16){
                        *edit_val.p_val.p_uint16 += (uint16_t)uint32_pow(10, (uint8_t)edit_val.digit);
                    }
                    if((*edit_val.p_val.p_uint16 > edit_val.val_max.uint16)||(*edit_val.p_val.p_uint16 < edit_val.val_min.uint16)){ //if out of range
                        *edit_val.p_val.p_uint16 = edit_val.val_max.uint16;
                    }
                    break;
                case VAL_INT32:
                    if(*edit_val.p_val.p_int32 < edit_val.val_max.int32){
                        *edit_val.p_val.p_int32 += (int32_t)uint32_pow(10, (uint8_t)edit_val.digit);
                    }
                    if((*edit_val.p_val.p_int32 > edit_val.val_max.int32)||(*edit_val.p_val.p_int32 < edit_val.val_min.int32)){ //if out of range
                        *edit_val.p_val.p_int32 = edit_val.val_max.int32;
                    }
                    break;
                case VAL_UINT32:
                    if(*edit_val.p_val.p_uint32 < edit_val.val_max.uint32){
                        *edit_val.p_val.p_uint32 += (uint32_t)uint32_pow(10, (uint8_t)edit_val.digit);
                    }
                    if((*edit_val.p_val.p_uint32 > edit_val.val_max.uint32)||(*edit_val.p_val.p_uint32 < edit_val.val_min.uint32)){ //if out of range
                        *edit_val.p_val.p_uint32 = edit_val.val_max.uint32;
                    }
                    break;
                case VAL_FLOAT:
                    if(*edit_val.p_val.p_float < edit_val.val_max.vfloat){
                        *edit_val.p_val.p_float += float_pow(10.0, edit_val.digit);
                    }
                    if((*edit_val.p_val.p_float > edit_val.val_max.vfloat)||(*edit_val.p_val.p_float < edit_val.val_min.vfloat)){ //if out of range
                        *edit_val.p_val.p_float = edit_val.val_max.vfloat;
                    }
                    break;
                default:
                    break;
                }
            }
            if(button_click(BUTTON_DOWN,BUTTON_CLICK_TIME)){
                // decrement value
                switch(edit_val.type){
                case VAL_INT8:
                    if(*edit_val.p_val.p_int8 > edit_val.val_min.int8){
                        *edit_val.p_val.p_int8 -= (int8_t)uint32_pow(10, (uint8_t)edit_val.digit);
                    }
                    if((*edit_val.p_val.p_int8 > edit_val.val_max.int8)||(*edit_val.p_val.p_int8 < edit_val.val_min.int8)){ //if out of range
                        *edit_val.p_val.p_int8 = edit_val.val_min.int8;
                    }
                    break;
                case VAL_UINT8:
                    if(*edit_val.p_val.p_uint8 > edit_val.val_min.uint8){
                        *edit_val.p_val.p_uint8 -= (uint8_t)uint32_pow(10, (uint8_t)edit_val.digit);
                    }
                    if((*edit_val.p_val.p_uint8 > edit_val.val_max.uint8)||(*edit_val.p_val.p_uint8 < edit_val.val_min.uint8)){ //if out of range
                        *edit_val.p_val.p_uint8 = edit_val.val_min.uint8;
                    }
                    break;
                case VAL_INT16:
                    if(*edit_val.p_val.p_int16 > edit_val.val_min.int16){
                        *edit_val.p_val.p_int16 -= (int16_t)uint32_pow(10, (uint8_t)edit_val.digit);
                    }
                    if((*edit_val.p_val.p_int16 > edit_val.val_max.int16)||(*edit_val.p_val.p_int16 < edit_val.val_min.int16)){ //if out of range
                        *edit_val.p_val.p_int16 = edit_val.val_min.int16;
                    }
                    break;
                case VAL_UINT16:
                    if(*edit_val.p_val.p_uint16 > edit_val.val_min.uint16){
                        *edit_val.p_val.p_uint16 -= (uint16_t)uint32_pow(10, (uint8_t)edit_val.digit);
                    }
                    if((*edit_val.p_val.p_uint16 > edit_val.val_max.uint16)||(*edit_val.p_val.p_uint16 < edit_val.val_min.uint16)){ //if out of range
                        *edit_val.p_val.p_uint16 = edit_val.val_min.uint16;
                    }
                    break;
                case VAL_INT32:
                    if(*edit_val.p_val.p_int32 > edit_val.val_min.int32){
                        *edit_val.p_val.p_int32 -= (int32_t)uint32_pow(10, (uint8_t)edit_val.digit);
                    }
                    if((*edit_val.p_val.p_int32 > edit_val.val_max.int32)||(*edit_val.p_val.p_int32 < edit_val.val_min.int32)){ //if out of range
                        *edit_val.p_val.p_int32 = edit_val.val_min.int32;
                    }
                    break;
                case VAL_UINT32:
                    if(*edit_val.p_val.p_uint32 > edit_val.val_min.uint32){
                        *edit_val.p_val.p_uint32 -= (uint32_t)uint32_pow(10, (uint8_t)edit_val.digit);
                    }
                    if((*edit_val.p_val.p_uint32 > edit_val.val_max.uint32)||(*edit_val.p_val.p_uint32 < edit_val.val_min.uint32)){ //if out of range
                        *edit_val.p_val.p_uint32 = edit_val.val_min.uint32;
                    }
                    break;
                case VAL_FLOAT:
                    if(*edit_val.p_val.p_float > edit_val.val_min.vfloat){
                        *edit_val.p_val.p_float -= float_pow(10.0, edit_val.digit);
                    }
                    if((*edit_val.p_val.p_float > edit_val.val_max.vfloat)||(*edit_val.p_val.p_float < edit_val.val_min.vfloat)){ //if out of range
                        *edit_val.p_val.p_float = edit_val.val_min.vfloat;
                    }
                    break;
                default:
                    break;
                }
            }
            if(button_click(BUTTON_LEFT,BUTTON_CLICK_TIME)){
                //shift position left
                if(edit_val.digit < edit_val.digit_max){
                    edit_val.digit++;
                }
            }
            if(button_click(BUTTON_RIGHT,BUTTON_CLICK_TIME)){
                //shift position right
                if(edit_val.digit > edit_val.digit_min){
                    edit_val.digit--;
                }
            }
            if(button_click(BUTTON_OK,BUTTON_CLICK_TIME)){
                //out from digit_edit mode
                switch (selectedMenuItem->Page){
                case TIME_HOUR:
                case TIME_MIN:
                case TIME_SEC:
                case DATE_DAY:
                case DATE_MONTH:
                case DATE_YEAR:
                    dcts.dcts_rtc.state = RTC_STATE_SET;
                    break;
                }
                navigation_style = MENU_NAVIGATION;
            }
            break;
        }
        if(button_click(BUTTON_BREAK,BUTTON_CLICK_TIME)){
            if(LCD.auto_off == 0){
                LCD_backlight_toggle();
            }
        }
        if(button_click(BUTTON_SET,BUTTON_CLICK_TIME)){
            save_params();
        }

        if((pressed_time[BUTTON_BREAK].pressed > 0)&&(pressed_time[BUTTON_BREAK].pressed < navigation_task_period)){
            if(LCD.auto_off == 0){
                LCD_backlight_toggle();
            }
        }
        if((pressed_time[BUTTON_SET].pressed > 0)&&(pressed_time[BUTTON_SET].pressed < navigation_task_period)){
            save_params();
        }
        osDelayUntil(&last_wake_time, navigation_task_period);
    }
}*/

static void error_page_print(menu_page_t page){
    char string[50];
#if(DISP == LCD_DISP)
    /*LCD_set_xy(25,45);
    sprintf(string, "СТРАНИЦА НЕ");
    LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);
    LCD_set_xy(25,35);
    sprintf(string, "НАЙДЕНА: %d", page);
    LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);

    sprintf(string, "<назад");
    LCD_set_xy(0,0);
    LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);
    LCD_invert_area(0,0,42,11);*/
#elif(DISP == ST7735_DISP)
    st7735_xy(25,45);
    sprintf(string, "СТРАНИЦА НЕ");
    st7735_print(string,&Font_7x10,ST7735_BLACK);
    st7735_xy(25,35);
    sprintf(string, "НАЙДЕНА: %d", page);
    st7735_print(string,&Font_7x10,ST7735_BLACK);

    sprintf(string, "<назад");
    st7735_xy(0,0);
    st7735_fill_rect(0,0,42,11,ST7735_BLACK);
    st7735_print(string,&Font_7x10,ST7735_WHITE);
#endif // DISP
}

static void main_page_print(u8 tick){
    char string[50];

    // temperature and hummidity indoor
#if(DISP == LCD_DISP)
    sprintf(string,"Кабинет");
    /*LCD_set_xy(align_text_center(string,Font_7x10),40);
    LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);
    LCD_invert_area(0,29,127,51);
    LCD_invert_area(1,30,126,41);
    if(dcts_meas[TMPR_OUT].valid == 1){
        sprintf(string,"%.1f%s/%.1f%s",(double)dcts_meas[TMPR_OUT].value,dcts_meas[TMPR_OUT].unit_cyr,(double)dcts_meas[HUM_OUT].value,dcts_meas[HUM_OUT].unit_cyr);
    }else{
        sprintf(string,"Обрыв датчика");
    }
    LCD_set_xy(align_text_center(string,Font_7x10),30);
    LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);*/
#elif(DISP == ST7735_DISP)
    st7735_fill_rect(0,111,36,17,ST7735_WHITE);
    st7735_xy(0,119);
    if(dcts_meas[TMPR_OUT].valid == 1){
        sprintf(string,"%.1f%s",(double)dcts_meas[TMPR_OUT].value,dcts_meas[TMPR_OUT].unit_cyr);
        st7735_print(string,&Font_5x7,ST7735_BLACK);
    }else{
        sprintf(string,"Обрыв");
        st7735_print(string,&Font_5x7,ST7735_RED);
    }
    st7735_xy(0,111);
    if(dcts_meas[HUM_OUT].valid == 1){
        sprintf(string,"%.1f%s",(double)dcts_meas[HUM_OUT].value,dcts_meas[HUM_OUT].unit_cyr);
        st7735_print(string,&Font_5x7,ST7735_BLACK);
    }else{
        sprintf(string,"датчика");
        st7735_print(string,&Font_5x7,ST7735_RED);
    }
#endif // DISP

    //temperature and hummidity outdoor
#if(DISP == LCD_DISP)
    sprintf(string,"Улица");
    /*LCD_set_xy(align_text_center(string,Font_7x10),12);
    LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);
    LCD_invert_area(0,1,127,23);
    LCD_invert_area(1,2,126,13);
    if(dcts_meas[TMPR_IN_1].valid == 1){
        sprintf(string,"%.1f%s/%.1f%s",(double)dcts_meas[TMPR_IN_1].value,dcts_meas[TMPR_IN_1].unit_cyr,(double)dcts_meas[HUM_IN_1].value,dcts_meas[HUM_IN_1].unit_cyr);
    }else{
        sprintf(string,"Обрыв датчика");
    }
    LCD_set_xy(align_text_center(string,Font_7x10),2);
    LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);*/
    /*
    if(dcts_meas[TMPR_IN_AVG].valid == 1){
        if((dcts_act[TMPR_IN_HEATING].state.control == 1)||(dcts_act[TMPR_IN_COOLING].state.control == 1)){
            if((dcts_act[TMPR_IN_HEATING].state.control == 1)&&(dcts_act[TMPR_IN_COOLING].state.control == 1)){
                        sprintf(string,"T %.1f%s (ошибка)",(double)dcts_meas[TMPR_IN_AVG].value,dcts_meas[TMPR_IN_AVG].unit_cyr);
            }else if(dcts_act[TMPR_IN_HEATING].state.control == 1){
                sprintf(string,"T %.1f%s (%.1f%s)",(double)dcts_meas[TMPR_IN_AVG].value,dcts_meas[TMPR_IN_AVG].unit_cyr,(double)dcts_act[TMPR_IN_HEATING].set_value,dcts_meas[TMPR_IN_AVG].unit_cyr);
            }else if(dcts_act[TMPR_IN_COOLING].state.control == 1){
                sprintf(string,"T %.1f%s (%.1f%s)",(double)dcts_meas[TMPR_IN_AVG].value,dcts_meas[TMPR_IN_AVG].unit_cyr,(double)dcts_act[TMPR_IN_COOLING].set_value,dcts_meas[TMPR_IN_AVG].unit_cyr);
            }
        }else{
            sprintf(string,"T %.1f%s",(double)dcts_meas[TMPR_IN_AVG].value,dcts_meas[TMPR_IN_AVG].unit_cyr);
        }
    }else{
        sprintf(string,"Обрыв  обоих  ");
    }
    LCD_set_xy(align_text_right(string,Font_7x10)-36,7);
    LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);

    if(dcts_meas[HUM_IN_AVG].valid == 1){
        if(dcts_act[HUM_IN].state.control == 1){
            sprintf(string,"Rh %.1f%s (%.1f%s)",(double)dcts_act[HUM_IN].meas_value,dcts_meas[HUM_IN_AVG].unit_cyr,(double)dcts_act[HUM_IN].set_value,dcts_meas[HUM_IN_AVG].unit_cyr);
        }else{
            sprintf(string,"Rh %.1f%s",(double)dcts_meas[HUM_IN_AVG].value,dcts_meas[HUM_IN_AVG].unit_cyr);
        }
    }else{
        sprintf(string,"датчиков    ");
    }
    LCD_set_xy(align_text_right(string,Font_7x10)-36,0);
    LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);*/
#elif(DISP == ST7735_DISP)
    st7735_fill_rect(0,0,94,17,ST7735_WHITE);
    if(dcts_meas[TMPR_IN_AVG].valid == 1){
        if((dcts_act[TMPR_IN_HEATING].state.control == 1)||(dcts_act[TMPR_IN_COOLING].state.control == 1)){
            if((dcts_act[TMPR_IN_HEATING].state.control == 1)&&(dcts_act[TMPR_IN_COOLING].state.control == 1)){
                        sprintf(string,"T %.1f%s (ошибка)",(double)dcts_meas[TMPR_IN_AVG].value,dcts_meas[TMPR_IN_AVG].unit_cyr);
            }else if(dcts_act[TMPR_IN_HEATING].state.control == 1){
                sprintf(string,"T %.1f%s (%.1f%s)",(double)dcts_meas[TMPR_IN_AVG].value,dcts_meas[TMPR_IN_AVG].unit_cyr,(double)dcts_act[TMPR_IN_HEATING].set_value,dcts_meas[TMPR_IN_AVG].unit_cyr);
            }else if(dcts_act[TMPR_IN_COOLING].state.control == 1){
                sprintf(string,"T %.1f%s (%.1f%s)",(double)dcts_meas[TMPR_IN_AVG].value,dcts_meas[TMPR_IN_AVG].unit_cyr,(double)dcts_act[TMPR_IN_COOLING].set_value,dcts_meas[TMPR_IN_AVG].unit_cyr);
            }
        }else{
            sprintf(string,"T %.1f%s",(double)dcts_meas[TMPR_IN_AVG].value,dcts_meas[TMPR_IN_AVG].unit_cyr);
        }
    }else{
        sprintf(string,"Обрыв  обоих  ");
    }
    st7735_xy(align_text_right(string,Font_5x7)-36,7);
    st7735_print(string,&Font_5x7,ST7735_BLACK);

    if(dcts_meas[HUM_IN_AVG].valid == 1){
        if(dcts_act[HUM_IN].state.control == 1){
            sprintf(string,"Rh %.1f%s (%.1f%s)",(double)dcts_act[HUM_IN].meas_value,dcts_meas[HUM_IN_AVG].unit_cyr,(double)dcts_act[HUM_IN].set_value,dcts_meas[HUM_IN_AVG].unit_cyr);
        }else{
            sprintf(string,"Rh %.1f%s",(double)dcts_meas[HUM_IN_AVG].value,dcts_meas[HUM_IN_AVG].unit_cyr);
        }
    }else{
        sprintf(string,"датчиков    ");
    }
    st7735_xy(align_text_right(string,Font_5x7)-36,0);
    st7735_print(string,&Font_5x7,ST7735_BLACK);
#endif // DISP

    //time
#if(DISP == LCD_DISP)
    sprintf(string,"%02d:%02d:%02d",dcts.dcts_rtc.hour,dcts.dcts_rtc.minute,dcts.dcts_rtc.second);
    /*LCD_set_xy(align_text_center(string,Font_5x7),55);
    LCD_print(string,&Font_5x7,LCD_COLOR_BLACK);*/
#elif(DISP == ST7735_DISP)
    st7735_fill_rect(16,45,129,27,ST7735_WHITE);
    sprintf(string,"%02d:%02d:%02d",dcts.dcts_rtc.hour,dcts.dcts_rtc.minute,dcts.dcts_rtc.second);
    st7735_xy(align_text_center(string,Font_16x26)+16,45);
    st7735_print(string,&Font_16x26,ST7735_GREEN);
#endif // DISP
}

static void print_header(void){
    char string[50];
    //print header
#if(DISP == LCD_DISP)
    menuItem* temp = selectedMenuItem->Parent;
    sprintf(string, temp->Text);
    /*LCD_set_xy(align_text_center(string, Font_7x10),52);
    LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);
    LCD_invert_area(0,53,127,63);*/
#elif(DISP == ST7735_DISP)
    menuItem* temp = selectedMenuItem->Parent;
    sprintf(string, temp->Text);
    st7735_fill_rect(0,116,160,12,ST7735_ORANGE);
    st7735_xy(align_text_center(string, Font_7x10)+16,116);
    st7735_print(string,&Font_7x10,ST7735_WHITE);
#endif // DISP
}

static void menu_page_print(u8 tick){
    char string[50];
    print_header();

    menuItem* temp = selectedMenuItem->Parent;
#if(DISP == LCD_DISP)
    /*if(temp->Child_num >= 3){
        //print previous
        temp = selectedMenuItem->Previous;
        sprintf(string, temp->Text);
        LCD_set_xy(align_text_center(string, Font_7x10),39);
        LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);
    }

    //print selected
    sprintf(string, selectedMenuItem->Text);
    LCD_set_xy(align_text_center(string, Font_7x10),26);
    LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);
    LCD_invert_area(5,26,120,38);
    LCD_invert_area(6,27,119,37);

    //print next
    temp = selectedMenuItem->Next;
    sprintf(string, temp->Text);
    LCD_set_xy(align_text_center(string, Font_7x10),14);
    LCD_print(string,&Font_7x10,LCD_COLOR_BLACK);*/
#elif(DISP == ST7735_DISP)
    uint8_t line_pointer = 0;

    sprintf(string, selectedMenuItem->Text);
    st7735_fill_rect(5,102,150,13,ST7735_BLUE);
    st7735_xy(align_text_center(string, Font_7x10)+16,102);
    st7735_print(string,&Font_7x10,ST7735_WHITE);
    line_pointer++;

    menuItem* next = selectedMenuItem->Next;
    while((line_pointer < temp->Child_num)&&(line_pointer < DISP_MAX_LINES)){
        sprintf(string, next->Text);
        st7735_xy(align_text_center(string, Font_7x10)+16,102-13*line_pointer);
        st7735_print(string,&Font_7x10,ST7735_BLACK);
        next = next->Next;
        line_pointer++;
    }
#endif // DISP

    print_back();
    print_enter_right();
}


/**
 * @brief am2302_task
 * @param argument
 */

#define am2302_task_period 3000
void am2302_task (void const * argument){
    (void)argument;
    uint32_t last_wake_time = osKernelSysTick();
    am2302_init();
    am2302_data_t ch_2 = {0};
    uint8_t ch_2_lost_con_cnt = 0;
    uint32_t ch_2_recieved = 0;
    uint32_t ch_2_lost = 0;
    am2302_data_t ch_3 = {0};
    uint8_t ch_3_lost_con_cnt = 0;
    uint32_t ch_3_recieved = 0;
    uint32_t ch_3_lost = 0;
    am2302_data_t ch_4 = {0};
    uint8_t ch_4_lost_con_cnt = 0;
    uint32_t ch_4_recieved = 0;
    uint32_t ch_4_lost = 0;
    while(1){
        ch_2 = am2302_get(0);
        taskENTER_CRITICAL();
        /*if(ch_2.error == 1){
            ch_2_lost++;
            ch_2_lost_con_cnt++;
            if(ch_2_lost_con_cnt > 2){
                dcts_meas[HUM_OUT].valid = FALSE;
                dcts_meas[TMPR_OUT].valid = FALSE;
            }
        }else{
            ch_2_recieved++;
            ch_2_lost_con_cnt = 0;
            dcts_meas[HUM_OUT].value = (float)ch_2.hum/10;
            dcts_meas[HUM_OUT].valid = TRUE;
            dcts_meas[TMPR_OUT].value = (float)ch_2.tmpr/10;
            dcts_meas[TMPR_OUT].valid = TRUE;
        }*/
        taskEXIT_CRITICAL();

        ch_3 = am2302_get(1);
        taskENTER_CRITICAL();
        /*if(ch_3.error == 1){
            ch_3_lost++;
            ch_3_lost_con_cnt++;
            if(ch_3_lost_con_cnt > 2){
                dcts_meas[HUM_IN_1].valid = FALSE;
                dcts_meas[TMPR_IN_1].valid = FALSE;
            }
        }else{
            ch_3_recieved++;
            ch_3_lost_con_cnt = 0;
            dcts_meas[HUM_IN_1].value = (float)ch_3.hum/10;
            dcts_meas[HUM_IN_1].valid = TRUE;
            dcts_meas[TMPR_IN_1].value = (float)ch_3.tmpr/10;
            dcts_meas[TMPR_IN_1].valid = TRUE;
        }*/
        taskEXIT_CRITICAL();

        ch_4 = am2302_get(2);
        taskENTER_CRITICAL();
        /*if(ch_4.error == 1){
            ch_4_lost++;
            ch_4_lost_con_cnt++;
            if(ch_4_lost_con_cnt > 2){
                dcts_meas[HUM_IN_2].valid = FALSE;
                dcts_meas[TMPR_IN_2].valid = FALSE;
            }
        }else{
            ch_4_recieved++;
            ch_4_lost_con_cnt = 0;
            dcts_meas[HUM_IN_2].value = (float)ch_4.hum/10;
            dcts_meas[HUM_IN_2].valid = TRUE;
            dcts_meas[TMPR_IN_2].value = (float)ch_4.tmpr/10;
            dcts_meas[TMPR_IN_2].valid = TRUE;
        }*/
        taskEXIT_CRITICAL();

        /*if((dcts_meas[HUM_IN_1].valid)&&(dcts_meas[HUM_IN_2].valid)){
            dcts_meas[HUM_IN_AVG].value = (dcts_meas[HUM_IN_1].value + dcts_meas[HUM_IN_2].value)/2.0f;
            dcts_meas[TMPR_IN_AVG].value = (dcts_meas[TMPR_IN_1].value + dcts_meas[TMPR_IN_2].value)/2.0f;
            dcts_meas[HUM_IN_AVG].valid = TRUE;
            dcts_meas[TMPR_IN_AVG].valid = TRUE;
        }else if(dcts_meas[HUM_IN_1].valid){
            dcts_meas[HUM_IN_AVG].value = dcts_meas[HUM_IN_1].value;
            dcts_meas[TMPR_IN_AVG].value = dcts_meas[TMPR_IN_1].value;
            dcts_meas[HUM_IN_AVG].valid = TRUE;
            dcts_meas[TMPR_IN_AVG].valid = TRUE;
        }else if(dcts_meas[HUM_IN_2].valid){
            dcts_meas[HUM_IN_AVG].value = dcts_meas[HUM_IN_2].value;
            dcts_meas[TMPR_IN_AVG].value = dcts_meas[TMPR_IN_2].value;
            dcts_meas[HUM_IN_AVG].valid = TRUE;
            dcts_meas[TMPR_IN_AVG].valid = TRUE;
        }else{
            dcts_meas[HUM_IN_AVG].valid = FALSE;
            dcts_meas[TMPR_IN_AVG].valid = FALSE;
        }*/

        osDelayUntil(&last_wake_time, am2302_task_period);
    }
}

#define uart_task_period 5
void uart_task(void const * argument){
    (void)argument;
    uart_init(config.params.mdb_bitrate, 8, 1, PARITY_NONE, 10000, UART_CONN_LOST_TIMEOUT);
    uint16_t tick = 0;
    char string[100];
    uint32_t last_wake_time = osKernelSysTick();
    while(1){
        if((uart_1.state & UART_STATE_RECIEVE)&&\
                ((uint16_t)(us_tim_get_value() - uart_1.timeout_last) > uart_1.timeout)){
            memcpy(uart_1.buff_received, uart_1.buff_in, uart_1.in_ptr);
            uart_1.received_len = uart_1.in_ptr;
            uart_1.in_ptr = 0;
            uart_1.state &= ~UART_STATE_RECIEVE;
            uart_1.state &= ~UART_STATE_ERROR;
            uart_1.state |= UART_STATE_IN_HANDING;
            uart_1.conn_last = 0;
            uart_1.recieved_cnt ++;

            if(modbus_packet_for_me(uart_1.buff_received, uart_1.received_len)){
                uint16_t new_len = modbus_rtu_packet(uart_1.buff_received, uart_1.received_len);
                uart_send(uart_1.buff_received, new_len);
                uart_1.state &= ~UART_STATE_IN_HANDING;
            }
            if(uart_1.state & UART_STATE_IN_HANDING){
                dcts_packet_handle(uart_1.buff_received, uart_1.received_len);
            }else{
                uart_1.state &= ~UART_STATE_IN_HANDING;
            }
        }
        if((uart_1.conn_last > uart_1.conn_lost_timeout)||(uart_1.overrun_err_cnt > 2)){
            uart_deinit();
            uart_init(config.params.mdb_bitrate, 8, 1, PARITY_NONE, 10000, UART_CONN_LOST_TIMEOUT);
        }
        if(tick == 1000/uart_task_period){
            tick = 0;
            //HAL_GPIO_TogglePin(LED_PORT,LED_PIN);
            /*for(uint8_t i = 0; i < MEAS_NUM; i++){
                sprintf(string, "%s:\t%.1f(%s)\n",dcts_meas[i].name,(double)dcts_meas[i].value,dcts_meas[i].unit);
                if(i == MEAS_NUM - 1){
                    strncat(string,"\n",1);
                }
                uart_send((uint8_t*)string,(uint16_t)strlen(string));
            }
            sprintf(string, "test\n");
            uart_send((uint8_t*)string,(uint16_t)strlen(string));*/

        }else{
            tick++;
            uart_1.conn_last += uart_task_period;
        }

        osDelayUntil(&last_wake_time, uart_task_period);
    }
}

#define control_task_period 5
/*void control_task(void const * argument){
    (void)argument;
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    static pump_st_t pump_state = PUMP_EMPTY;
    static t_heat_t t_heat = T_HEAT_HEATING;
    static t_cool_t t_cool = T_COOL_COOLING;
    channels_init();
    channel_PWM_timer_init(5);
    channel_PWM_timer_init(6);
    uint32_t last_wake_time = osKernelSysTick();
    while(1){

        // input valve
        if(dcts_act[VALVE_IN].state.control){
            channel_PWM_duty_set(5, (dcts_act[VALVE_IN].set_value*0.05f+5.0f));
            HAL_TIM_PWM_Start(&htim3, input_ch[5].pwm_channel);
            //HAL_GPIO_WritePin(input_ch[5].port, input_ch[5].pin, GPIO_PIN_SET);
        }else {
            HAL_TIM_PWM_Stop(&htim3, input_ch[5].pwm_channel);
            HAL_GPIO_WritePin(input_ch[5].port, input_ch[5].pin, GPIO_PIN_RESET);
        }

        // output valve
        if(dcts_act[VALVE_OUT].state.control){
            channel_PWM_duty_set(6, (dcts_act[VALVE_OUT].set_value*0.05f+5.0f));
            HAL_TIM_PWM_Start(&htim3, input_ch[6].pwm_channel);
        }else {
            HAL_TIM_PWM_Stop(&htim3, input_ch[6].pwm_channel);
            HAL_GPIO_WritePin(input_ch[6].port, input_ch[6].pin, GPIO_PIN_RESET);
        }

        // temperature in (heating)
        if(dcts_act[TMPR_IN_HEATING].state.control){
            if(dcts_meas[TMPR_IN_AVG].valid){
                dcts_act[TMPR_IN_HEATING].meas_value = dcts_meas[TMPR_IN_AVG].value;

                switch(t_heat){
                case T_HEAT_HEATING:
                    dcts_act[TMPR_IN_HEATING].state.pin_state = 1;
                    if(dcts_act[TMPR_IN_HEATING].meas_value >= (dcts_act[TMPR_IN_HEATING].set_value + 0.5f*dcts_act[TMPR_IN_HEATING].hysteresis)){
                        t_heat = T_HEAT_COOLING;
                    }
                    break;
                case T_HEAT_COOLING:
                    dcts_act[TMPR_IN_HEATING].state.pin_state = 0;
                    if(dcts_act[TMPR_IN_HEATING].meas_value <= (dcts_act[TMPR_IN_HEATING].set_value - 0.5f*dcts_act[TMPR_IN_HEATING].hysteresis)){
                        t_heat = T_HEAT_HEATING;
                    }
                    break;
                }
            }else{
                // current value unknown
                dcts_act[TMPR_IN_HEATING].state.pin_state = 0;
            }
            // set rele_control if control_by_act enable
            if(dcts_rele[HEATER].state.control_by_act == 1){
                if(dcts_rele[HEATER].state.control != dcts_act[TMPR_IN_HEATING].state.pin_state){
                    dcts_rele[HEATER].state.control = dcts_act[TMPR_IN_HEATING].state.pin_state;
                }
            }
        }else{
            // disable rele_control if control_by_act enable
            if(dcts_rele[HEATER].state.control_by_act == 1){
                dcts_rele[HEATER].state.control = 0;
            }
        }

        // temperature in (cooling)
        if(dcts_act[TMPR_IN_COOLING].state.control){
            if(dcts_meas[TMPR_IN_AVG].valid){
                dcts_act[TMPR_IN_COOLING].meas_value = dcts_meas[TMPR_IN_AVG].value;

                switch(t_cool){
                case T_COOL_COOLING:
                    dcts_act[TMPR_IN_COOLING].state.pin_state = 1;
                    if(dcts_act[TMPR_IN_COOLING].meas_value <= (dcts_act[TMPR_IN_COOLING].set_value - 0.5f*dcts_act[TMPR_IN_COOLING].hysteresis)){
                        t_cool = T_COOL_HEATING;
                    }
                    break;
                case T_COOL_HEATING:
                    dcts_act[TMPR_IN_COOLING].state.pin_state = 0;
                    if(dcts_act[TMPR_IN_COOLING].meas_value >= (dcts_act[TMPR_IN_COOLING].set_value + 0.5f*dcts_act[TMPR_IN_COOLING].hysteresis)){
                        t_cool = T_COOL_COOLING;
                    }
                    break;
                }
            }else{
                // current value unknown
                dcts_act[TMPR_IN_COOLING].state.pin_state = 0;
            }
            // set rele_control if control_by_act enable
            if(dcts_rele[FREEZER].state.control_by_act == 1){
                if(dcts_rele[FREEZER].state.control != dcts_act[TMPR_IN_COOLING].state.pin_state)
                dcts_rele[FREEZER].state.control = dcts_act[TMPR_IN_COOLING].state.pin_state;
            }
        }else{
            // disable rele_control if control_by_act enable
            if(dcts_rele[FREEZER].state.control_by_act == 1){
                dcts_rele[FREEZER].state.control = 0;
            }
        }

        // hummidity in
        if(dcts_act[HUM_IN].state.control){
            if(dcts_meas[HUM_IN_AVG].valid){
                dcts_act[HUM_IN].meas_value = dcts_meas[HUM_IN_AVG].value;
            }else{
                // current value unknown
                dcts_act[HUM_IN].state.pin_state = 0;
            }
        }

        // water pump
        if(dcts_act[AUTO_PUMP].state.control){
            switch(pump_state){
            case PUMP_EMPTY:
                if(dcts_meas[WTR_MIN_RES].value < config.params.wtr_min_ref){
                    pump_state = PUMP_FILLING;
                }
                break;
            case PUMP_FILLING:
                if(dcts_meas[WTR_MIN_RES].value > config.params.wtr_min_ref){
                    pump_state = PUMP_EMPTY;
                }else if(dcts_meas[WTR_MAX_RES].value < config.params.wtr_max_ref){
                    pump_state = PUMP_ACTIVE;
                }
                break;
            case PUMP_ACTIVE:
                dcts_act[WTR_PUMP].state.pin_state = 1;
                if(dcts_meas[WTR_MIN_RES].value > config.params.wtr_min_ref){
                    dcts_act[WTR_PUMP].state.pin_state = 0;
                    pump_state = PUMP_EMPTY;
                }
                break;
            }
            if(dcts_rele[WTR_PUMP].state.control_by_act == 1){
                if(dcts_rele[WTR_PUMP].state.control != dcts_act[WTR_PUMP].state.pin_state)
                dcts_rele[WTR_PUMP].state.control = dcts_act[WTR_PUMP].state.pin_state;
            }
        }

        // control DO channels
        for(u8 i = 0; i < DO_NUM; i ++){
            if(dcts_rele[i].state.control == 1){
                //rele on
                if(dcts_rele[i].state.status == 0){
                    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
                    GPIO_InitStruct.Pin = do_ch[i].pin;
                    HAL_GPIO_Init (do_ch[i].port, &GPIO_InitStruct);
                    HAL_GPIO_WritePin(do_ch[i].port, do_ch[i].pin, GPIO_PIN_RESET);
                    dcts_rele[i].state.status = 1;
                }
            }else{
                //rele off
                if(dcts_rele[i].state.status == 1){
                    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                    GPIO_InitStruct.Pin = do_ch[i].pin;
                    HAL_GPIO_Init (do_ch[i].port, &GPIO_InitStruct);
                    //HAL_GPIO_WritePin(do_ch[i].port, do_ch[i].pin, GPIO_PIN_SET);
                    dcts_rele[i].state.status = 0;
                }
            }
        }
        osDelayUntil(&last_wake_time, control_task_period);
    }
}*/

/**
 * @brief Init timer for LCD backlight level control
 * @param channel - channel number
 * @return  0 - OK,\n
 *          -1 - TIM init error,\n
 *          -2 - TIM syncronyzation config error,\n
 *          -3 - PWM channel config error
 * @ingroup LCD
 */
static int channel_PWM_timer_init(u8 channel){
    int result = 0;
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();

    if(ch_config[channel].pwm_tim == TIM3){
        htim3.Instance = TIM3;
        htim3.Init.Prescaler = 719;
        htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim3.Init.Period = 2000;
        htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
        htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
        if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
        {
          result = -1;
        }
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
        {
          result = -2;
        }
    }/*else if(input_ch[channel].pwm_tim == TIM2){
        htim2.Instance = TIM2;
        htim2.Init.Prescaler = 71;
        htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim2.Init.Period = 100;
        htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
        if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
        {
          result = -1;
        }
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
        {
          result = -2;
        }
    }*/
    if(channel_PWM_duty_set(channel, 0) < 0){
        result = -3;
    }
    return result;
}

static int channel_PWM_duty_set(u8 channel, float duty){
    int result = 0;
    TIM_OC_InitTypeDef sConfigOC = {0};

    if(ch_config[channel].pwm_tim == TIM3){
        htim3.Instance = TIM3;
        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = (uint16_t)(duty * 20.0f);
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, ch_config[channel].pwm_channel) != HAL_OK)
        {
          result = -1;
        }
    }/*else if(input_ch[channel].pwm_tim == TIM2){
        htim2.Instance = TIM2;
        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = (uint16_t)(duty * 10.0f);
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, input_ch[channel].pwm_channel) != HAL_OK)
        {
          result = -1;
        }
    }*/
    return result;
}

uint16_t uint16_pow(uint16_t x, uint16_t pow){
    uint16_t result = 1;
    while(pow){
        result *= x;
        pow--;
    }
    return result;
}

/**
 * @brief Init us timer
 * @ingroup MAIN
 */
void tim2_init(void){
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;
    __HAL_RCC_TIM2_CLK_ENABLE();
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 71;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 0xFFFF;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)  {
        _Error_Handler(__FILE__, __LINE__);
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)  {
        _Error_Handler(__FILE__, __LINE__);
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)  {
        _Error_Handler(__FILE__, __LINE__);
    }
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
    if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK){
        _Error_Handler(__FILE__, __LINE__);
    }
}
/**
 * @brief Get value from global us timer
 * @return global us timer value
 * @ingroup MAIN
 */
uint32_t us_tim_get_value(void){
    uint32_t value = us_cnt_H + TIM2->CNT;
    return value;
}
/**
 * @brief Us delayy
 * @param us - delau value
 * @ingroup MAIN
 */
void us_tim_delay(uint32_t us){
    uint32_t current;
    uint8_t with_yield;
    current = TIM2->CNT;
    with_yield = 0;
    if(us > TIME_YIELD_THRESHOLD){
        with_yield =1;
    }
    while ((TIM2->CNT - current)<us){
        if(with_yield){
            osThreadYield();
        }
    }
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM1) {
        HAL_IncTick();
    }

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

static void print_back(void){
    char string[100];
#if(DISP == LCD_DISP)
    sprintf(string, "<назад");
    /*LCD_set_xy(0,0);
    LCD_print(string,&Font_5x7,LCD_COLOR_BLACK);
    LCD_invert_area(0,0,30,8);*/
#elif(DISP == ST7735_DISP)
    sprintf(string, "<назад");
    st7735_fill_rect(0,0,32,9,ST7735_BLACK);
    st7735_xy(0,0);
    st7735_print(string,&Font_5x7,ST7735_WHITE);
#endif // DISP
}

static void print_enter_right(void){
    char string[100];
#if(DISP == LCD_DISP)
    sprintf(string, "выбор>");
    /*LCD_set_xy(align_text_right(string,Font_5x7),0);
    LCD_print(string,&Font_5x7,LCD_COLOR_BLACK);
    LCD_invert_area(97,0,127,8);*/
#elif(DISP == ST7735_DISP)
    sprintf(string, "выбор>");
    st7735_fill_rect(128,0,32,9,ST7735_BLACK);
    st7735_xy(align_text_right(string,Font_5x7)+32,0);
    st7735_print(string,&Font_5x7,ST7735_WHITE);
#endif // DISP
}

static void print_enter_ok(void){
    char string[100];
#if(DISP == LCD_DISP)
    sprintf(string, "ввод*");
    /*LCD_set_xy(align_text_center(string,Font_5x7),0);
    LCD_print(string,&Font_5x7,LCD_COLOR_BLACK);
    LCD_invert_area(51,0,76,8);*/
#elif(DISP == ST7735_DISP)
    sprintf(string, "ввод*");
    st7735_fill_rect(66,0,27,9,ST7735_BLACK);
    st7735_xy(align_text_center(string,Font_5x7)+16,0);
    st7735_print(string,&Font_5x7,ST7735_WHITE);
#endif // DISP
}

static void print_change(void){
    char string[100];
#if(DISP == LCD_DISP)
    sprintf(string, "изменить>");
    /*LCD_set_xy(align_text_right(string,Font_5x7),0);
    LCD_print(string,&Font_5x7,LCD_COLOR_BLACK);
    LCD_invert_area(82,0,127,8);*/
#elif(DISP == ST7735_DISP)
    sprintf(string, "изменить>");
    st7735_fill_rect(113,0,47,9,ST7735_BLACK);
    st7735_xy(align_text_right(string,Font_5x7)+32,0);
    st7735_print(string,&Font_5x7,ST7735_WHITE);
#endif // DISP
}

static void save_params(void){
    static menuItem* current_menu;
    current_menu = selectedMenuItem;
    menuChange(&save_changes);

    // store ModBus params
    config.params.mdb_address = dcts.dcts_address;
    config.params.mdb_bitrate = (uint16_t)bitrate_array[bitrate_array_pointer];
    // store display params
    /*config.params.lcd_backlight_lvl = LCD.backlight_lvl;
    config.params.lcd_backlight_time = LCD.auto_off;*/
    // store dcts_act
    for(uint8_t i = 0; i < ACT_NUM; i++){
        config.params.act_set[i] = dcts_act[i].set_value;
        config.params.act_hyst[i] = dcts_act[i].hysteresis;
        config.params.act_enable[i] = dcts_act[i].state.control;
    }
    // store dcts_rele
    for(uint8_t i = 0; i < RELE_NUM; i++){
        config.params.rele[i] = dcts_rele[i].state.control_by_act;
    }

    int area_cnt = find_free_area();
    if(area_cnt < 0){
        uint32_t erase_error = 0;
        FLASH_EraseInitTypeDef flash_erase = {0};
        flash_erase.TypeErase = FLASH_TYPEERASE_PAGES;
        flash_erase.NbPages = 1;
        flash_erase.PageAddress = FLASH_SAVE_PAGE_ADDRESS;
        HAL_FLASH_Unlock();
        HAL_FLASHEx_Erase(&flash_erase, &erase_error);
        HAL_FLASH_Lock();
        area_cnt = 0;
    }
    for(uint8_t i = 0; i < SAVED_PARAMS_SIZE; i ++){
        save_to_flash(area_cnt, i, &config.word[i]);
    }
    // reinit uart
    uart_deinit();
    uart_init(config.params.mdb_bitrate, 8, 1, PARITY_NONE, 10000, UART_CONN_LOST_TIMEOUT);
    //delay for show message
    osDelay(2000);
    menuChange(current_menu);
}

/*static void restore_params(void){
    int area_cnt = find_free_area();
    if(area_cnt != 0){
        if(area_cnt == -1){
            // page is fill, actual values in last area
            area_cnt = SAVE_AREA_NMB - 1;
        }else{
            // set last filled area number
            area_cnt--;
        }
        uint16_t *addr;
        addr = (uint32_t)(FLASH_SAVE_PAGE_ADDRESS + area_cnt*SAVE_AREA_SIZE);
        for(uint8_t i = 0; i < SAVED_PARAMS_SIZE; i++){
            config.word[i] = *addr;
            addr++;
        }

        // restore dcts_address
        dcts.dcts_address = (uint8_t)config.params.mdb_address;
        // restore dcts_act
        for(uint8_t i = 0; i < ACT_NUM; i++){
            dcts_act[i].set_value = config.params.act_set[i];
            dcts_act[i].hysteresis = config.params.act_hyst[i];
            dcts_act[i].state.control = (uint8_t)config.params.act_enable[i];
        }

        // restore dcts_rele
        for(uint8_t i = 0; i < RELE_NUM; i++){
            dcts_rele[i].state.control_by_act = (uint8_t)(config.params.rele[i]);
        }
    }else{
        //init default values if saved params not found
        config.params.mdb_bitrate = BITRATE_115200;
    }
    for(bitrate_array_pointer = 0; bitrate_array_pointer < 14; bitrate_array_pointer++){
        if(bitrate_array[bitrate_array_pointer] == config.params.mdb_bitrate){
            break;
        }
    }
}*/

static void save_to_bkp(u8 bkp_num, uint16_t var){
    uint32_t data = var;
    /*if(bkp_num%2 == 1){
        data = data << 8;
    }*/
    HAL_PWR_EnableBkUpAccess();
    switch (bkp_num){
    case 0:
        BKP->DR1 = data;
        break;
    case 1:
        BKP->DR2 = data;
        break;
    case 2:
        BKP->DR3 = data;
        break;
    case 3:
        BKP->DR4 = data;
        break;
    case 4:
        BKP->DR5 = data;
        break;
    case 5:
        BKP->DR6 = data;
        break;
    case 6:
        BKP->DR7 = data;
        break;
    case 7:
        BKP->DR8 = data;
        break;
    case 8:
        BKP->DR9 = data;
        break;
    case 9:
        BKP->DR10 = data;
        break;
    }
    HAL_PWR_DisableBkUpAccess();
}

/*static void save_float_to_bkp(u8 bkp_num, float var){
    char buf[5] = {0};
    sprintf(buf, "%4.0f", (double)var);
    u8 data = (u8)atoi(buf);
    save_to_bkp(bkp_num, data);
}*/

static uint16_t read_bkp(u8 bkp_num){
    uint32_t data = 0;
    switch (bkp_num){
    case 0:
        data = BKP->DR1;
        break;
    case 1:
        data = BKP->DR2;
        break;
    case 2:
        data = BKP->DR3;
        break;
    case 3:
        data = BKP->DR4;
        break;
    case 4:
        data = BKP->DR5;
        break;
    case 5:
        data = BKP->DR6;
        break;
    case 6:
        data = BKP->DR7;
        break;
    case 7:
        data = BKP->DR8;
        break;
    case 8:
        data = BKP->DR9;
        break;
    case 9:
        data = BKP->DR10;
        break;
    }
    if(bkp_num%2 == 1){
        data = data >> 8;
    }
    return (uint16_t)(data & 0xFFFF);
}
/*static float read_float_bkp(u8 bkp_num, u8 sign){
    u8 data = read_bkp(bkp_num);
    char buf[5] = {0};
    if(sign == READ_FLOAT_SIGNED){
        sprintf(buf, "%d", (s8)data);
    }else{
        sprintf(buf, "%d", data);
    }
    return atoff(buf);
}*/

/*static void led_lin_init(void){
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = LED_PIN;
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
    HAL_GPIO_Init (LED_PORT, &GPIO_InitStruct);
}*/


static void channels_init(void){
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    for(u8 i = 0;i < DO_NUM; i++){
        GPIO_InitStruct.Pin = do_ch[i].pin;
        //HAL_GPIO_WritePin(do_ch[i].port, do_ch[i].pin, GPIO_PIN_SET);
        HAL_GPIO_Init (do_ch[i].port, &GPIO_InitStruct);
    }
    for(u8 i = 0; i < IN_CHANNEL_NUM; i++){
        switch (*ch_config[i].mode){
        case CH_MODE_AI_VLT:
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Pin = ch_config[i].pin;
            HAL_GPIO_Init(ch_config[i].port, &GPIO_InitStruct);
            break;
        case CH_MODE_DI_DRY:
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_PULLDOWN;
            GPIO_InitStruct.Pin = ch_config[i].pin;
            HAL_GPIO_Init(ch_config[i].port, &GPIO_InitStruct);
            break;
        case CH_MODE_DO:
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull = GPIO_PULLDOWN;
            GPIO_InitStruct.Pin = ch_config[i].pin;
            HAL_GPIO_Init(ch_config[i].port, &GPIO_InitStruct);
            break;
        case CH_MODE_AM3202:
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            GPIO_InitStruct.Pin = ch_config[i].pin;
            HAL_GPIO_Init(ch_config[i].port, &GPIO_InitStruct);
            break;
        case CH_MODE_PWM:
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_PULLDOWN;
            GPIO_InitStruct.Pin = ch_config[i].pin;
            HAL_GPIO_Init(ch_config[i].port, &GPIO_InitStruct);
            break;
        case CH_MODE_DS18B20:
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Pin = ch_config[i].pin;
            HAL_GPIO_Init(ch_config[i].port, &GPIO_InitStruct);
            break;
        default:
            ;
        }
    }
}

static void MX_IWDG_Init(void){

    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_128;
    hiwdg.Init.Reload = 3124;   //10sec
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
      Error_Handler();
    }
}

/*uint32_t uint32_pow(uint16_t x, uint8_t pow){
    uint32_t result = 1;
    while(pow){
        result *= x;
        pow--;
    }
    return result;
}

float float_pow(float x, int pow){
    float result = 1.0;
    if(pow > 0){
        while(pow > 0){
            result *= x;
            pow--;
        }
    }else if(pow < 0){
        while(pow < 0){
            result /= x;
            pow++;
        }
    }
    return  result;
}*/

void refresh_watchdog(void){
#if(RELEASE == 1)
    MX_IWDG_Init();
#endif//RELEASE
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

