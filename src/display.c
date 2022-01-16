/**
 * @file control.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup src
 * @ingroup src
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
/*
 * Copyright (c) 2018 Snema Service
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the sofi PLC.
 *
 * Author: Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 */
#ifndef DISPLAY_C
#define DISPLAY_C 1
#include "main.h"
#include "display.h"
#include "cmsis_os.h"
#include "task.h"
#include "FreeRTOS.h"
#include "dcts.h"
//#include "usbd_cdc_if.h"
#include "ssd1306.h"
#include "buttons.h"
#include "stm32f1xx_ll_gpio.h"
#include "control.h"
#include "menu.h"
#include "flash.h"

extern IWDG_HandleTypeDef hiwdg;
extern RTC_HandleTypeDef hrtc;
extern osThreadId defaultTaskHandle;
extern osThreadId displayTaskHandle;
extern osThreadId menuTaskHandle;
extern osThreadId buttonsTaskHandle;

static u8 display_time(u8 y);
static void clock_set(void);
static void max_reg_temp_set(void);
static void print_header(void);
static void main_page_print(u8 tick, skin_t skin);
static void menu_page_print(u8 tick);
static void value_print(u8 tick);
static void screensaver(u8 tick);
static void error_page_print(menu_page_t page);
static void save_page_print (u8 tick);
static void info_print (void);
static void print_back(void);
static void print_enter_right(void);
static void print_enter_ok(void);
static void print_change(void);
static int get_param_value(char* string, menu_page_t page);
static void set_edit_value(menu_page_t page);
static void save_params(void);
static brick_t get_brick_coord(u8 ind);
static const char off_on_descr[2][10] = {
    "¬˚ÍÎ.",
    "¬ÍÎ.",
};
static const char manual_auto_descr[2][10] = {
    "–Û˜ÌÓÈ",
    "¿‚ÚÓ",
};
static const char weekday_descr[8][3] = {
    "Œ¯",
    "œÌ",
    "¬Ú",
    "—",
    "◊Ú",
    "œÚ",
    "—·",
    "¬Ò",
};
const char skin_descr[3][20]={
    {"œÓÎÌ˚È"},
    {"¬ÂÏˇ"},
    {"“ÂÏÔÂ"},
};
static const char ch_mode_descr[7][10] = {
    "ÕÂ‡ÍÚË‚ÂÌ",
    "¿Ì‡ÎÓ„Ó‚˚È",
    "ƒËÒ. ‚ıÓ‰",
    "ƒËÒ. ‚˚ıÓ‰",
    "AM2302",
    "ÿ»Ã-‚˚ıÓ‰",
    "DS18B20",
};
/*enum menu_page_t {
    PAGE_CLOCK,
    PAGE_HYSTERESIS,
    PAGE_PROGRAMM,
    PAGE_STATISTIC,
    PAGE_MAX_TEMP_REG,
    PAGE_END_OF_LIST,
};
#define MENU_LEVEL_NUM 2
typedef struct {
    u8 level;
    u8 page[MENU_LEVEL_NUM];
}navigation_t;
static navigation_t menu;
static const u8 menu_max_page[] = {4,1};*/
#define display_task_period 100
void display_task( const void *parameters){
    (void) parameters;
    menu_init();
    skin_t skin = SKIN_FULL; // add from config
    u8 tick = 0;
    u8 tick_2 = 0;
    menu_page_t last_page = selectedMenuItem->Page;
    uint32_t last_wake_time = osKernelSysTick();
    taskENTER_CRITICAL();
    refresh_watchdog();
    SSD1306_Init();
    SSD1306.on_off = 1;
    refresh_watchdog();
    SSD1306_UpdateScreen();
    taskEXIT_CRITICAL();
    while(1){
        /* Reinit SSD1306 if error */
        //char buff[32];
        if(SSD1306.error_num){
            SSD1306.Initialized = 0;
        }
        if((tick % 5) == 0){
            if(!SSD1306.Initialized ){
                SSD1306_Init();
            }
        }
        refresh_watchdog();
        SSD1306_Fill(SSD1306_COLOR_BLACK);
        if(last_page != selectedMenuItem->Page){
            tick = 0;
            last_page = selectedMenuItem->Page;
        }
        switch (selectedMenuItem->Page) {
        case MAIN_PAGE:
            main_page_print(tick, skin);
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
        if(SSD1306.on_off == 1){
            SSD1306_UpdateScreen();
        }
        if((config.params.lcd_backlight_time != 0)&&(SSD1306.on_off == 1)){
            SSD1306.auto_off_timeout += display_task_period;
            if(SSD1306.auto_off_timeout > (uint32_t)config.params.lcd_backlight_time * 10000){
                SSD1306.auto_off_timeout = 0;
                SSD1306.on_off = 0;
                SSD1306_Fill(SSD1306_COLOR_BLACK);
                SSD1306_UpdateScreen();
            }
        }
        if(tick_2 == 500/display_task_period){
            tick_2 = 0;
            tick++;
        }
        tick_2++;
        osDelayUntil(&last_wake_time, display_task_period);
    }
}

static void main_page_print(u8 tick, skin_t skin){
    char buff[50];
    switch (skin) {
    case SKIN_FULL:
        screensaver(tick);
        /*sprintf(buff,"› –¿Õ");
        SSD1306_GotoXY(15, 15);
        SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
        sprintf(buff,"œ–»¬≈“—“¬»ﬂ");
        SSD1306_GotoXY(7, 29);
        SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
        if(sensor_state.error == SENSOR_OK){
            sprintf(buff,"%2.1f", (double)dcts_act[HEATING].meas_value);
           SSD1306_GotoXY(0, 14);
            SSD1306_Puts(buff, &Font_16x26, SSD1306_COLOR_WHITE);
        }else if(sensor_state.error == SENSOR_BREAK){
            SSD1306_DrawFilledRectangle(0,14,64,26,SSD1306_COLOR_BLACK);    // clear area
            sprintf(buff,"Œ¡–€¬");
            SSD1306_GotoXY(15, 15);
            SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
            sprintf(buff,"ƒ¿“◊» ¿");
            SSD1306_GotoXY(7, 29);
            SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
        }else if(sensor_state.error == SENSOR_SHORT){
            SSD1306_DrawFilledRectangle(0,14,64,26,SSD1306_COLOR_BLACK);    // clear area
            sprintf(buff,"«¿Ã€ ¿Õ»≈");
            SSD1306_GotoXY(0, 15);
            SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
            sprintf(buff,"ƒ¿“◊» ¿");
            SSD1306_GotoXY(7, 29);
            SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
        }

        if(dcts_act[HEATER].state.control == TRUE){
            sprintf(buff,"”ÒÚ %2.0f%s", (double)dcts_act[HEATING].set_value, dcts_act[HEATING].unit);
        }else{
            sprintf(buff,"¬˚ÍÎ˛˜ÂÌ");
        }
        SSD1306_GotoXY(70, 16);
        SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);

        sprintf(buff,"–Â„%3.0f%s", (double)dcts_meas[TMPR_REG_GRAD].value, dcts_meas[TMPR_REG_GRAD].unit);
        SSD1306_GotoXY(70, 29);
        SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);

        display_time(0);*/
        break;
    case SKIN_TIME:
        /*if(sensor_state.error == SENSOR_OK){
            sprintf(buff,"%2.1f", (double)dcts_act[0].meas_value);
            SSD1306_GotoXY(0, 14);
            SSD1306_Puts(buff, &Font_16x26, SSD1306_COLOR_WHITE);
        }else if(sensor_state.error == SENSOR_BREAK){
            SSD1306_DrawFilledRectangle(0,14,64,26,SSD1306_COLOR_BLACK);    // clear area
            sprintf(buff,"Œ¡–€¬");
            SSD1306_GotoXY(15, 15);
            SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
            sprintf(buff,"ƒ¿“◊» ¿");
            SSD1306_GotoXY(7, 29);
            SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
        }else if(sensor_state.error == SENSOR_SHORT){
            SSD1306_DrawFilledRectangle(0,14,64,26,SSD1306_COLOR_BLACK);    // clear area
            sprintf(buff,"«¿Ã€ ¿Õ»≈");
            SSD1306_GotoXY(0, 15);
            SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
            sprintf(buff,"ƒ¿“◊» ¿");
            SSD1306_GotoXY(7, 29);
            SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
        }

        if(dcts_act[HEATER].state.control == TRUE){
            sprintf(buff,"”ÒÚ %2.0f%s", (double)dcts_act[0].set_value, dcts_act[0].unit);
        }else {
            sprintf(buff,"¬˚ÍÎ˛˜ÂÌ");
        }
        SSD1306_GotoXY(70, 16);
        SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);*/
        break;
    case SKIN_TEMP:
        /*if(sensor_state.error == SENSOR_OK){
            sprintf(buff,"%2.1f", (double)dcts_act[0].meas_value);
            SSD1306_GotoXY(0, 14);
            SSD1306_Puts(buff, &Font_16x26, SSD1306_COLOR_WHITE);
        }else if(sensor_state.error == SENSOR_BREAK){
            SSD1306_DrawFilledRectangle(0,14,64,26,SSD1306_COLOR_BLACK);    // clear area
            sprintf(buff,"Œ¡–€¬");
            SSD1306_GotoXY(15, 15);
            SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
            sprintf(buff,"ƒ¿“◊» ¿");
            SSD1306_GotoXY(7, 29);
            SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
        }else if(sensor_state.error == SENSOR_SHORT){
            SSD1306_DrawFilledRectangle(0,14,64,26,SSD1306_COLOR_BLACK);    // clear area
            sprintf(buff,"«¿Ã€ ¿Õ»≈");
            SSD1306_GotoXY(0, 15);
            SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
            sprintf(buff,"ƒ¿“◊» ¿");
            SSD1306_GotoXY(7, 29);
            SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);
        }

        if(dcts_act[HEATER].state.control == TRUE){
            sprintf(buff,"”ÒÚ %2.0f%s", (double)dcts_act[0].set_value, dcts_act[0].unit);
        }else {
            sprintf(buff,"¬˚ÍÎ˛˜ÂÌ");
        }
        SSD1306_GotoXY(70, 16);
        SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);

        display_time(0);*/
        break;
    default:
        break;
    }
}
#define BRICK_WIDTH     12
#define BRICK_HEIGTH    5
#define BRICK_LINES     13
#define BRICK_POS       11

static const u8 snake_array[BRICK_POS*BRICK_LINES] = {
    132,133,134,135,136,137,138,139,140,141,142,
    131,130,129,128,127,126,125,124,123,122,121,
    110,111,112,113,114,115,116,117,118,119,120,
    109,108,107,106,105,104,103,102,101,100, 99,
     88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98,
     87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77,
     66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
     65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55,
     44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
     43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33,
     22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
     21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11,
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10
};

static void screensaver(u8 tick){
    char buff[50];
    static u8 brick_state[BRICK_POS*BRICK_LINES] = {0};
    brick_t brick = {0};
    static u8 int_tick = 0;
    static u8 filling = BRICK_FILL;
    static u8 filled = 0;
    if(filling == BRICK_FILL){
        brick_state[snake_array[int_tick]] = BRICK_FILL;
        int_tick++;
        if(int_tick == BRICK_POS*BRICK_LINES){
            int_tick = 0;
            filling = BRICK_EMPTY;
        }
    }else if(filling == BRICK_EMPTY){
        brick_state[snake_array[int_tick]] = BRICK_EMPTY;
        int_tick++;
        if(int_tick == BRICK_POS*BRICK_LINES){
            int_tick = 0;
            filling = BRICK_FILL;
        }
    }
    /*for(u8 i = 0; i < BRICK_POS*BRICK_LINES; i++){
        if(rand()%2 == 0){
            brick_state[i] = BRICK_FILL;
        }else{
            brick_state[i] = BRICK_EMPTY;
        }
    }*/
    SSD1306_DrawFilledRectangle(0,0,127,63,SSD1306_COLOR_BLACK);    // clear area
    sprintf(buff,"BRIC");
    SSD1306_GotoXY(align_text_center(buff,Font_16x26), 1);
    SSD1306_Puts(buff, &Font_16x26, SSD1306_COLOR_WHITE);
    sprintf(buff,"QUADRO");
    SSD1306_GotoXY(align_text_center(buff,Font_16x26), 30);
    SSD1306_Puts(buff, &Font_16x26, SSD1306_COLOR_WHITE);
    for(u8 i = 0; i < BRICK_POS*BRICK_LINES; i++){
        brick = get_brick_coord(i);
        switch (brick_state[i]) {
        case BRICK_FILL:
            SSD1306_DrawRectangle(brick.x0,brick.y0,brick.x1 - brick.x0,brick.y1 - brick.y0,SSD1306_COLOR_BLACK);
            SSD1306_DrawFilledRectangle(brick.x0+1,brick.y0+1,brick.x1 - brick.x0-1,brick.y1 - brick.y0-1,SSD1306_COLOR_WHITE);
            break;
        case BRICK_EMPTY:
            //SSD1306_DrawRectangle(brick.x0,brick.y0,brick.x1 - brick.x0,brick.y1 - brick.y0,SSD1306_COLOR_BLACK);
            break;
        }
    }
}

static brick_t get_brick_coord(u8 ind){
    brick_t brick = {0};
    s16 x0,y0,x1,y1,line,pos,add;
    add = 0;
    line = ind/BRICK_POS;
    pos = ind%BRICK_POS;

    if(line%2 == 1){
        add = 6;
    }
    x0 = pos * BRICK_WIDTH - add;
    y0 = line * BRICK_HEIGTH;

    if((line%2 == 1)&&(pos == BRICK_POS - 1)){
        x1 = 127;
    }else{
        x1 = x0 + BRICK_WIDTH;
    }
    y1 = y0 + BRICK_HEIGTH;

    if(x0 < 0){
        x0 = 0;
    }
    if(x1 > 127){
        x1 = 127;
    }
    if(y1 > 63){
        y1 = 63;
    }

    brick.x0 = (u8)x0;
    brick.x1 = (u8)x1;
    brick.y0 = (u8)y0;
    brick.y1 = (u8)y1;
    return brick;
}

static void info_print (void){
    char string[50];
    print_header();

    sprintf(string, "»Ïˇ:%s",dcts.dcts_name_cyr);
    SSD1306_GotoXY(2,13);
    SSD1306_Puts(string,&Font_5x7,SSD1306_COLOR_WHITE);
    sprintf(string, "¿‰ÂÒ:%d",dcts.dcts_address);
    SSD1306_GotoXY(2,21);
    SSD1306_Puts(string,&Font_5x7,SSD1306_COLOR_WHITE);
    sprintf(string, "¬ÂÒËˇ:%s",dcts.dcts_ver);
    SSD1306_GotoXY(2,29);
    SSD1306_Puts(string,&Font_5x7,SSD1306_COLOR_WHITE);
    sprintf(string, "œËÚ‡ÌËÂ:%.1f¬",(double)dcts.dcts_pwr);
    SSD1306_GotoXY(2,37);
    SSD1306_Puts(string,&Font_5x7,SSD1306_COLOR_WHITE);
    sprintf(string, "¡‡Ú‡ÂÈÍ‡:%.1f¬",(double)dcts_meas[VBAT_VLT].value);
    SSD1306_GotoXY(2,45);
    SSD1306_Puts(string,&Font_5x7,SSD1306_COLOR_WHITE);
    sprintf(string, "%02d:%02d:%02d", dcts.dcts_rtc.hour, dcts.dcts_rtc.minute, dcts.dcts_rtc.second);
    SSD1306_GotoXY(75,21);
    SSD1306_Puts(string,&Font_5x7,SSD1306_COLOR_WHITE);
    sprintf(string, "%02d.%02d.%04d", dcts.dcts_rtc.day, dcts.dcts_rtc.month, dcts.dcts_rtc.year);
    SSD1306_GotoXY(75,13);
    SSD1306_Puts(string,&Font_5x7,SSD1306_COLOR_WHITE);

    print_back();
}

static void print_header(void){
    char string[50];
    //print header
    menuItem* temp = selectedMenuItem->Parent;
    sprintf(string, temp->Text);
    SSD1306_DrawFilledRectangle(0,0,128,10, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(align_text_center(string, Font_7x10),1);
    SSD1306_Puts(string,&Font_7x10,SSD1306_COLOR_BLACK);
}

static void menu_page_print(u8 tick){
    char string[50];
    print_header();

    menuItem* temp = selectedMenuItem->Parent;
    if(temp->Child_num >= 3){
        //print previous
        temp = selectedMenuItem->Previous;
        sprintf(string, temp->Text);
        SSD1306_GotoXY(align_text_center(string, Font_7x10),14);
        SSD1306_Puts(string,&Font_7x10,SSD1306_COLOR_WHITE);
    }

    //print selected
    sprintf(string, selectedMenuItem->Text);
    SSD1306_GotoXY(align_text_center(string, Font_7x10),28);
    SSD1306_Puts(string,&Font_7x10,SSD1306_COLOR_WHITE);
    SSD1306_DrawRectangle(1,25,126,14,SSD1306_COLOR_WHITE);

    temp = selectedMenuItem->Parent;
    if(temp->Child_num >= 2){
        //print next
        temp = selectedMenuItem->Next;
        sprintf(string, temp->Text);
        SSD1306_GotoXY(align_text_center(string, Font_7x10),42);
        SSD1306_Puts(string,&Font_7x10,SSD1306_COLOR_WHITE);
    }

    print_back();
    print_enter_right();
}

static void save_page_print (u8 tick){
    /*char string[50];

    SSD1306_DrawRectangle(5,2,123,26,SSD1306_COLOR_WHITE);
    sprintf(string, "—Œ’–¿Õ≈Õ»≈ ÕŒ¬€’");
    SSD1306_GotoXY(align_text_center(string, Font_7x10),4);
    SSD1306_Puts(string,&Font_7x10,SSD1306_COLOR_WHITE);
    sprintf(string, " Œ›‘‘»÷»≈Õ“Œ¬");
    SSD1306_GotoXY(align_text_center(string, Font_7x10),14);
    SSD1306_Puts(string,&Font_7x10,SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(55,42);
    SSD1306_Putc(1,&Icon_16x16,SSD1306_COLOR_WHITE);
    switch(tick%4){
    case 0:
        SSD1306_DrawFilledRectangle(55,42,16,16,SSD1306_COLOR_BLACK);
        break;
    case 1:
        SSD1306_DrawFilledRectangle(55,46,16,16,SSD1306_COLOR_BLACK);
        break;
    case 2:
        SSD1306_DrawFilledRectangle(55,50,16,16,SSD1306_COLOR_BLACK);
        break;
    }*/
}

static void value_print(u8 tick){
    char string[50];
    print_header();
    int prev = 0;
    int cur = 0;
    int next = 0;

    menuItem* temp = selectedMenuItem->Parent;
    if(temp->Child_num >= 3){
        //print previous name
        temp = selectedMenuItem->Previous;
        sprintf(string, temp->Text);
        SSD1306_GotoXY(2,14);
        SSD1306_Puts(string,&Font_7x10,SSD1306_COLOR_WHITE);
        SSD1306_DrawFilledRectangle(80,14,47,11,SSD1306_COLOR_BLACK);
        prev = get_param_value(string, temp->Page);
        SSD1306_GotoXY(align_text_right(string,Font_7x10),14);
        SSD1306_Puts(string,&Font_7x10,SSD1306_COLOR_WHITE);

        if(prev == -2){
            // invalid value
            SSD1306_DrawLine(98,19,127,19,SSD1306_COLOR_WHITE);
        }
    }

    //print selected name
    sprintf(string, selectedMenuItem->Text);
    SSD1306_GotoXY(2,28);
    SSD1306_print_ticker(string,&Font_7x10,SSD1306_COLOR_WHITE, 11, tick);
    SSD1306_InvertRectangle(1,25,79,14);
    cur = get_param_value(string, selectedMenuItem->Page);
    SSD1306_GotoXY(align_text_right(string,Font_7x10),28);
    SSD1306_Puts(string,&Font_7x10,SSD1306_COLOR_WHITE);
    if(cur == -2){
        // invalid value
        SSD1306_DrawLine(98,33,127,33,SSD1306_COLOR_WHITE);
    }

    temp = selectedMenuItem->Parent;
    if(temp->Child_num >= 2){
        //print next name
        temp = selectedMenuItem->Next;
        sprintf(string, temp->Text);
        SSD1306_GotoXY(2,42);
        SSD1306_Puts(string,&Font_7x10,SSD1306_COLOR_WHITE);
        SSD1306_DrawFilledRectangle(80,42,47,11,SSD1306_COLOR_BLACK);
        next = get_param_value(string, temp->Page);
        SSD1306_GotoXY(align_text_right(string,Font_7x10),42);
        SSD1306_Puts(string,&Font_7x10,SSD1306_COLOR_WHITE);
        if(next == -2){
            // invalid value
            SSD1306_DrawLine(98,47,127,47,SSD1306_COLOR_WHITE);
        }
    }

    print_back();

    if(navigation_style == MENU_NAVIGATION){
        set_edit_value(selectedMenuItem->Page);
        if((selectedMenuItem->Child == &EDITED_VAL)&&(cur != -3)){
            print_change();
        }
    }else if(navigation_style == DIGIT_EDIT){
        print_enter_ok();
        if(edit_val.digit < 0){
            SSD1306_InvertRectangle(127-(u8)(edit_val.digit+edit_val.select_shift)*edit_val.select_width,25,edit_val.select_width,14);
        }else{
            SSD1306_InvertRectangle(127-(u8)(edit_val.digit+edit_val.select_shift+1)*edit_val.select_width,25,edit_val.select_width,14);
        }
    }
}

static void print_back(void){
    char string[100];
    sprintf(string, "<Ì‡Á‡‰");
    SSD1306_DrawFilledRectangle(0,55,30,8,SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(0,56);
    SSD1306_Puts(string,&Font_5x7,SSD1306_COLOR_BLACK);
}

static void print_enter_right(void){
    char string[100];
    sprintf(string, "‚˚·Ó>");
    SSD1306_DrawFilledRectangle(97,55,30,8,SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(align_text_right(string,Font_5x7),56);
    SSD1306_Puts(string,&Font_5x7,SSD1306_COLOR_BLACK);
}

static void print_enter_ok(void){
    char string[100];
    sprintf(string, "‚‚Ó‰*");
    SSD1306_DrawFilledRectangle(51,55,25,8,SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(align_text_center(string,Font_5x7),56);
    SSD1306_Puts(string,&Font_5x7,SSD1306_COLOR_BLACK);
}

static void print_change(void){
    char string[100];
    sprintf(string, "ËÁÏÂÌËÚ¸>");
    SSD1306_DrawFilledRectangle(82,55,45,8,SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(align_text_right(string,Font_5x7),56);
    SSD1306_Puts(string,&Font_5x7,SSD1306_COLOR_BLACK);
}


u8 display_time(u8 y){
    char buff[20] = {0};
    sprintf(buff,"%02d.%02d.%04d ", dcts.dcts_rtc.day, dcts.dcts_rtc.month, dcts.dcts_rtc.year);
    SSD1306_GotoXY(0, y);
    SSD1306_Puts(buff, &Font_5x7, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(70, y);
    SSD1306_Puts(weekday_descr[dcts.dcts_rtc.weekday], &Font_5x7, SSD1306_COLOR_WHITE);

    sprintf(buff,"%02d:%02d:%02d", dcts.dcts_rtc.hour, dcts.dcts_rtc.minute, dcts.dcts_rtc.second);
    SSD1306_GotoXY(align_text_right(buff, Font_5x7), y);
    SSD1306_Puts(buff, &Font_5x7, SSD1306_COLOR_WHITE);

    return 0x00;
}

/*void menu_task( const void *parameters){
    (void) parameters;
    vTaskSuspend(NULL); // Suspend menu_task after create
    uint32_t last_wake_time = osKernelSysTick();
    while(1){*/
        /*if(eTaskGetState(displayTaskHandle) != eSuspended){
            vTaskSuspend(displayTaskHandle);
        }*/

        /* Read buttons */
        /*if (pressed_time.left){
            SSD1306_DrawFilledRectangle(0,0,128,64,SSD1306_COLOR_BLACK);    // clear display
            SSD1306_UpdateScreen();
            vTaskSuspend(buttonsTaskHandle);
            pressed_time.left = 0;
            vTaskResume(displayTaskHandle);
            vTaskSuspend(NULL);
        }
        if (pressed_time.right || pressed_time.ok){
            if(menu.level < MENU_LEVEL_NUM){
                menu.level++;
                SSD1306_DrawFilledRectangle(0,0,128,64,SSD1306_COLOR_BLACK);    // clear display
                SSD1306_UpdateScreen();
            }
        }
        if (pressed_time.down){
            if(menu.page[menu.level] == menu_max_page[menu.level]){
                menu.page[menu.level] = 0;
            }else{
                menu.page[menu.level]++;
            }
            SSD1306_DrawFilledRectangle(0,0,128,64,SSD1306_COLOR_BLACK);    // clear display
            SSD1306_UpdateScreen();
        }
        if (pressed_time.up){
            if(menu.page[menu.level] == 0){
                menu.page[menu.level] = menu_max_page[menu.level] - 1;
            }else{
                menu.page[menu.level]--;
            }
            SSD1306_DrawFilledRectangle(0,0,128,64,SSD1306_COLOR_BLACK);    // clear display
            SSD1306_UpdateScreen();
        }*/

        /* Print menu */
        /*if(menu.level == 0){
            SSD1306_GotoXY(50, 0);
            SSD1306_Puts("Ã≈Õﬁ", &Font_7x10, SSD1306_COLOR_WHITE);
            if(menu.page[menu.level] <= PAGE_STATISTIC){
                SSD1306_GotoXY(8, 16);
                SSD1306_Puts("ƒ‡Ú‡ Ë ‚ÂÏˇ", &Font_7x10, SSD1306_COLOR_WHITE);
                SSD1306_GotoXY(8, 27);
                SSD1306_Puts("√ËÒÚÂÂÁËÒ", &Font_7x10, SSD1306_COLOR_WHITE);
                SSD1306_GotoXY(8, 38);
                SSD1306_Puts("–ÂÊËÏ ‡·ÓÚ˚", &Font_7x10, SSD1306_COLOR_WHITE);
                SSD1306_GotoXY(8, 49);
                SSD1306_Puts("—Ú‡ÚËÒÚËÍ‡", &Font_7x10, SSD1306_COLOR_WHITE);
            }else if(menu.page[menu.level] > PAGE_STATISTIC && menu.page[menu.level] <= PAGE_END_OF_LIST){
                SSD1306_GotoXY(8, 16);
                SSD1306_Puts("Ã‡ÍÒ. “ Â„-‡", &Font_7x10, SSD1306_COLOR_WHITE);
            }*/

            /* Print cursor */
            /*SSD1306_GotoXY(0, 16 + 11 * (menu.page[menu.level] % 4));
            SSD1306_Puts(">", &Font_7x10, SSD1306_COLOR_WHITE);

        }else if(menu.level == 1){
            if(menu.page[menu.level - 1] == PAGE_CLOCK){
                vTaskSuspend(buttonsTaskHandle);
                pressed_time.ok = 0;
                pressed_time.right = 0;
                clock_set();
            }else if(menu.page[menu.level - 1] == PAGE_HYSTERESIS){
                // hysteresis_set();
                SSD1306_DrawFilledRectangle(0,0,128,64,SSD1306_COLOR_BLACK);    // clear display
                SSD1306_UpdateScreen();
                vTaskResume(displayTaskHandle);
                vTaskSuspend(NULL);
            }else if(menu.page[menu.level - 1] == PAGE_PROGRAMM){
                // programm_set();
                SSD1306_DrawFilledRectangle(0,0,128,64,SSD1306_COLOR_BLACK);    // clear display
                SSD1306_UpdateScreen();
                vTaskResume(displayTaskHandle);
                vTaskSuspend(NULL);
            }else if(menu.page[menu.level - 1] == PAGE_STATISTIC){
                // statistic_info();
                SSD1306_DrawFilledRectangle(0,0,128,64,SSD1306_COLOR_BLACK);    // clear display
                SSD1306_UpdateScreen();
                vTaskResume(displayTaskHandle);
                vTaskSuspend(NULL);
            }else if(menu.page[menu.level - 1] == PAGE_MAX_TEMP_REG){
                vTaskSuspend(buttonsTaskHandle);
                pressed_time.ok = 0;
                pressed_time.right = 0;
                max_reg_temp_set();
            }
        }

        SSD1306_UpdateScreen();
        HAL_IWDG_Refresh(&hiwdg);
        osDelayUntil(&last_wake_time, DISPLAY_TASK_PERIOD);
        if(eTaskGetState(buttonsTaskHandle) == eSuspended){
            vTaskDelay(DISPLAY_TASK_PERIOD);
            vTaskResume(buttonsTaskHandle);
        }
    }

}*/
/*
#define RTC_MAX_DAY 31
#define RTC_MAX_MONTH 12
#define RTC_MAX_YEAR 3000
#define RTC_MIN_YEAR 2000
#define RTC_MAX_WEEKDAY 7
#define RTC_MAX_HOUR 24
#define RTC_MAX_MINUTE 59

static void clock_set(void){
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    uint32_t last_wake_time = osKernelSysTick();
    vTaskSuspend(defaultTaskHandle);
    SSD1306_DrawFilledRectangle(0,0,128,64,SSD1306_COLOR_BLACK);    // clear display
    u8 position = 0;
    u8 x = 0;
    while(1){*/
        /* Read buttons */
        /*if (pressed_time.left){
            if(position == 0){
                position = 12;
            }else{
                position--;
            }
            SSD1306_DrawFilledRectangle(0,10,128,54,SSD1306_COLOR_BLACK);    // clear display
            SSD1306_UpdateScreen();
        }
        if (pressed_time.right){
            if(position == 13){
                position = 0;
            }else{
                position++;
            }
            SSD1306_DrawFilledRectangle(0,10,128,54,SSD1306_COLOR_BLACK);    // clear display
            SSD1306_UpdateScreen();
        }
        if (pressed_time.down){
            switch (position) {
            case 0:
                if(dcts.dcts_rtc.day < 10){
                    dcts.dcts_rtc.day = 0;
                }else{
                    dcts.dcts_rtc.day -= 10;
                }
                break;
            case 1:
                if(dcts.dcts_rtc.day < 1){
                    dcts.dcts_rtc.day = 0;
                }else{
                    dcts.dcts_rtc.day -= 1;
                }
                break;
            case 2:
                if(dcts.dcts_rtc.month < 10){
                    dcts.dcts_rtc.month = 0;
                }else{
                    dcts.dcts_rtc.month -= 10;
                }
                break;
            case 3:
                if(dcts.dcts_rtc.month < 1){
                    dcts.dcts_rtc.month = 0;
                }else{
                    dcts.dcts_rtc.month -= 1;
                }
                break;
            case 4:
                if(dcts.dcts_rtc.year < RTC_MIN_YEAR){
                    dcts.dcts_rtc.year = RTC_MIN_YEAR;
                }else{
                    dcts.dcts_rtc.year -= 1000;
                }
                break;
            case 5:
                if(dcts.dcts_rtc.year < RTC_MIN_YEAR){
                    dcts.dcts_rtc.year = RTC_MIN_YEAR;
                }else{
                    dcts.dcts_rtc.year -= 100;
                }
                break;
            case 6:
                if(dcts.dcts_rtc.year < RTC_MIN_YEAR){
                    dcts.dcts_rtc.year = RTC_MIN_YEAR;
                }else{
                    dcts.dcts_rtc.year -= 10;
                }
                break;
            case 7:
                if(dcts.dcts_rtc.year < RTC_MIN_YEAR){
                    dcts.dcts_rtc.year = RTC_MIN_YEAR;
                }else{
                    dcts.dcts_rtc.year -= 1;
                }
                break;
            case 8:
                if(dcts.dcts_rtc.weekday < 2){
                    dcts.dcts_rtc.weekday = 1;
                }else{
                    dcts.dcts_rtc.weekday -= 1;
                }
                break;
            case 9:
                if(dcts.dcts_rtc.hour < 10){
                    dcts.dcts_rtc.hour = 0;
                }else{
                    dcts.dcts_rtc.hour -= 10;
                }
                break;
            case 10:
                if(dcts.dcts_rtc.hour < 1){
                    dcts.dcts_rtc.hour = 0;
                }else{
                    dcts.dcts_rtc.hour -= 1;
                }
                break;
            case 11:
                if(dcts.dcts_rtc.minute < 10){
                    dcts.dcts_rtc.minute = 0;
                }else{
                    dcts.dcts_rtc.minute -= 10;
                }
                break;
            case 12:
                if(dcts.dcts_rtc.minute < 1){
                    dcts.dcts_rtc.minute = 0;
                }else{
                    dcts.dcts_rtc.minute -= 1;
                }
                break;
            }
            SSD1306_DrawFilledRectangle(0,10,128,54,SSD1306_COLOR_BLACK);    // clear display
            SSD1306_UpdateScreen();
        }
        if (pressed_time.up){
            switch (position) {
            case 0:
                dcts.dcts_rtc.day += 10;
                if(dcts.dcts_rtc.day > RTC_MAX_DAY){
                    dcts.dcts_rtc.day = RTC_MAX_DAY;
                }
                break;
            case 1:
                dcts.dcts_rtc.day += 1;
                if(dcts.dcts_rtc.day > RTC_MAX_DAY){
                    dcts.dcts_rtc.day = RTC_MAX_DAY;
                }
                break;
            case 2:
                dcts.dcts_rtc.month += 10;
                if(dcts.dcts_rtc.month > RTC_MAX_MONTH){
                    dcts.dcts_rtc.month = RTC_MAX_MONTH;
                }
                break;
            case 3:
                dcts.dcts_rtc.month += 1;
                if(dcts.dcts_rtc.month > RTC_MAX_MONTH){
                    dcts.dcts_rtc.month = RTC_MAX_MONTH;
                }
                break;
            case 4:
                dcts.dcts_rtc.year += 1000;
                if(dcts.dcts_rtc.year > RTC_MAX_YEAR){
                    dcts.dcts_rtc.year = RTC_MAX_YEAR;
                }
                break;
            case 5:
                dcts.dcts_rtc.year += 100;
                if(dcts.dcts_rtc.year > RTC_MAX_YEAR){
                    dcts.dcts_rtc.year = RTC_MAX_YEAR;
                }
                break;
            case 6:
                dcts.dcts_rtc.year += 10;
                if(dcts.dcts_rtc.year > RTC_MAX_YEAR){
                    dcts.dcts_rtc.year = RTC_MAX_YEAR;
                }
                break;
            case 7:
                dcts.dcts_rtc.year += 1;
                if(dcts.dcts_rtc.year > RTC_MAX_YEAR){
                    dcts.dcts_rtc.year = RTC_MAX_YEAR;
                }
                break;
            case 8:
                dcts.dcts_rtc.weekday += 1;
                if(dcts.dcts_rtc.weekday > RTC_MAX_WEEKDAY){
                    dcts.dcts_rtc.weekday = RTC_MAX_WEEKDAY;
                }
                break;
            case 9:
                dcts.dcts_rtc.hour += 10;
                if(dcts.dcts_rtc.hour > RTC_MAX_HOUR){
                    dcts.dcts_rtc.hour = RTC_MAX_HOUR;
                }
                break;
            case 10:
                dcts.dcts_rtc.hour += 1;
                if(dcts.dcts_rtc.hour > RTC_MAX_HOUR){
                    dcts.dcts_rtc.hour = RTC_MAX_HOUR;
                }
                break;
            case 11:
                dcts.dcts_rtc.minute += 10;
                if(dcts.dcts_rtc.minute > RTC_MAX_MINUTE){
                    dcts.dcts_rtc.minute = RTC_MAX_MINUTE;
                }
                break;
            case 12:
                dcts.dcts_rtc.minute += 1;
                if(dcts.dcts_rtc.minute > RTC_MAX_MINUTE){
                    dcts.dcts_rtc.minute = RTC_MAX_MINUTE;
                }
                break;
            }
            SSD1306_DrawFilledRectangle(0,10,128,54,SSD1306_COLOR_BLACK);    // clear display
            SSD1306_UpdateScreen();
        }
        if (pressed_time.ok){
            SSD1306_DrawFilledRectangle(0,0,128,64,SSD1306_COLOR_BLACK);    // clear display
            SSD1306_UpdateScreen();
            break;
        }*/

        /* print values on screen */

        /*SSD1306_GotoXY(22, 0);
        SSD1306_Puts("ƒ‡Ú‡ Ë ‚ÂÏˇ", &Font_7x10,SSD1306_COLOR_WHITE);
        display_time(20);
        switch (position){
        case 0:
            x = 0;
            break;
        case 1:
            x = 7;
            break;
        case 2:
            x = 21;
            break;
        case 3:
            x = 28;
            break;
        case 4:
            x = 42;
            break;
        case 5:
            x = 49;
            break;
        case 6:
            x = 56;
            break;
        case 7:
            x = 63;
            break;
        case 8:
            x = 76;
            break;
        case 9:
            x = 92;
            break;
        case 10:
            x = 99;
            break;
        case 11:
            x = 113;
            break;
        case 12:
            x = 120;
            break;
        }
        SSD1306_GotoXY(x, 32);
        SSD1306_Putc('^', &Font_7x10,SSD1306_COLOR_WHITE);

        SSD1306_UpdateScreen();
        HAL_IWDG_Refresh(&hiwdg);
        osDelayUntil(&last_wake_time, DISPLAY_TASK_PERIOD);
        if(eTaskGetState(buttonsTaskHandle) == eSuspended){
            vTaskDelay(DISPLAY_TASK_PERIOD);
            vTaskResume(buttonsTaskHandle);
        }
    }*/
    /* save new time and data */
    /*time.Hours = dcts.dcts_rtc.hour;
    time.Minutes = dcts.dcts_rtc.minute;
    time.Seconds = 0;

    date.Date = dcts.dcts_rtc.day;
    date.Month = dcts.dcts_rtc.month;
    date.Year = (uint8_t)(dcts.dcts_rtc.year - 2000);
    date.WeekDay = dcts.dcts_rtc.weekday;

    HAL_RTC_SetDate(&hrtc,&date,RTC_FORMAT_BIN);
    HAL_RTC_SetTime(&hrtc,&time,RTC_FORMAT_BIN);
    menu.level--;

    vTaskResume(defaultTaskHandle);
}*/
/*
#define REG_MAX_TMPR    150
#define REG_MIN_TMPR    10
static void max_reg_temp_set(void){
    char buff[7] = {0};
    uint32_t last_wake_time = osKernelSysTick();
    SSD1306_DrawFilledRectangle(0,0,128,64,SSD1306_COLOR_BLACK);    // clear display
    SSD1306_GotoXY(22,0);
    SSD1306_Puts("Ã‡ÍÒËÏ‡Î¸Ì‡ˇ", &Font_7x10, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(25,12);
    SSD1306_Puts("ÚÂÏÔÂ‡ÚÛ‡", &Font_7x10, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(29, 24);
    SSD1306_Puts("ÒËÏÏËÒÚÓ‡", &Font_7x10, SSD1306_COLOR_WHITE);
    while(1){*/
        /* Read buttons */
        /*if (pressed_time.ok){
            SSD1306_DrawFilledRectangle(0,0,128,64,SSD1306_COLOR_BLACK);    // clear display
            SSD1306_UpdateScreen();
            break;
        }
        if(pressed_time.up){
            if(semistor_state.max_tmpr > REG_MAX_TMPR){
                semistor_state.max_tmpr = REG_MAX_TMPR;
            }else{
                semistor_state.max_tmpr++;
            }
        }
        if(pressed_time.down){
            if(semistor_state.max_tmpr < REG_MIN_TMPR){
                semistor_state.max_tmpr = REG_MIN_TMPR;
            }else{
                semistor_state.max_tmpr--;
            }
        }*/

        /* Print screen */
        /*SSD1306_GotoXY(46, 46);
        sprintf(buff, "%3.0f∞C", (double)semistor_state.max_tmpr);
        SSD1306_Puts(buff, &Font_7x10, SSD1306_COLOR_WHITE);

        SSD1306_UpdateScreen();
        HAL_IWDG_Refresh(&hiwdg);
        osDelayUntil(&last_wake_time, DISPLAY_TASK_PERIOD);
        if(eTaskGetState(buttonsTaskHandle) == eSuspended){
            vTaskDelay(DISPLAY_TASK_PERIOD);
            vTaskResume(buttonsTaskHandle);
        }
    }

    HAL_PWR_EnableBkUpAccess();
    BKP->DR7 = (uint32_t)semistor_state.max_tmpr;
    HAL_PWR_DisableBkUpAccess();
    menu.level--;
}*/

/**
 * @brief Calculate start position to put string on center of display
 * @param string - pionter to string buffer
 * @param font - pointer to structure with used font
 * @return x position for LCD_print()
 */
uint8_t align_text_center(char* string, FontDef_t font){
    uint8_t len = (uint8_t)strlen(string);
    return (uint8_t)(128-len*font.FontWidth)/2;
}

/**
 * @brief Calculate start position to put string on right of display
 * @param string - pionter to string buffer
 * @param font - pointer to structure with used font
 * @return x position for LCD_print()
 */
uint8_t align_text_right(char* string, FontDef_t font){
    uint8_t len = (uint8_t)strlen(string);
    return (uint8_t)(127-len*font.FontWidth);
}

/**
 * @brief get_param_value
 * @param string - buffer for set value
 * @param page -
 * @return  0 - haven't additional data,\n
 *          -1 - valid value,\n
 *          -2 - invalid value,\n
 *          -3 - don't change,
 */
static int get_param_value(char* string, menu_page_t page){
    int result = 0;
    uint8_t pos = 0;
    switch (page) {
    case SELF_TMPR:
    case SELF_HUM:
    case SELF_VREF:
    case SELF_VBAT:
        pos = (uint8_t)(page - SELF_TMPR);
        sprintf(string, "%.1f", (double)dcts_meas[pos].value);
        if(dcts_meas[pos].valid == 1){
            result = -1;
        }else{
            result = -2;
        }
        break;

    case CH_0_MODE_PAGE:
    case CH_1_MODE_PAGE:
    case CH_2_MODE_PAGE:
    case CH_3_MODE_PAGE:
        pos = (uint8_t)((page - CH_0_MODE_PAGE)/(CH_1_MODE_PAGE - CH_0_MODE_PAGE));
        sprintf(string, "%s", ch_mode_descr[config.params.ch_mode[pos]]);
        break;
    case CH_0_DO_STATE_PAGE:
    case CH_1_DO_STATE_PAGE:
    case CH_2_DO_STATE_PAGE:
    case CH_3_DO_STATE_PAGE:
        pos = (uint8_t)((page - CH_0_DO_STATE_PAGE)/(CH_1_DO_STATE_PAGE - CH_0_DO_STATE_PAGE));
        sprintf(string, "%s", off_on_descr[dcts_rele[pos].state.control]);
        if(dcts_rele[pos].state.control_by_act == 1){
            result = -3;
        }
        break;
    case CH_0_DO_CTRL_PAGE:
    case CH_1_DO_CTRL_PAGE:
    case CH_2_DO_CTRL_PAGE:
    case CH_3_DO_CTRL_PAGE:
        pos = (uint8_t)((page - CH_0_DO_CTRL_PAGE)/(CH_1_DO_CTRL_PAGE - CH_0_DO_CTRL_PAGE));
        sprintf(string, "%s", manual_auto_descr[dcts_rele[pos].state.control_by_act]);
        break;
    case CH_0_PWM_DUTY_PAGE:
    case CH_1_PWM_DUTY_PAGE:
    case CH_2_PWM_DUTY_PAGE:
    case CH_3_PWM_DUTY_PAGE:
        pos = (uint8_t)((page - CH_0_PWM_DUTY_PAGE)/(CH_1_PWM_DUTY_PAGE - CH_0_PWM_DUTY_PAGE));
        sprintf(string, "%.1f%s", (double)dcts_act[pos].set_value, dcts_act[pos].unit_cyr);
        break;
    case CH_0_PWM_CTRL_PAGE:
    case CH_1_PWM_CTRL_PAGE:
    case CH_2_PWM_CTRL_PAGE:
    case CH_3_PWM_CTRL_PAGE:
        pos = (uint8_t)((page - CH_0_PWM_CTRL_PAGE)/(CH_1_PWM_CTRL_PAGE - CH_0_PWM_CTRL_PAGE));
        sprintf(string, "%s", off_on_descr[dcts_act[pos].state.control]);
        break;
    case CH_0_TMPR_PAGE:
    case CH_1_TMPR_PAGE:
    case CH_2_TMPR_PAGE:
    case CH_3_TMPR_PAGE:
        pos = (uint8_t)((page - CH_0_TMPR_PAGE)/(CH_1_TMPR_PAGE - CH_0_TMPR_PAGE)*(CH_1_TMPR - CH_0_TMPR)+CH_0_TMPR);
        sprintf(string, "%.1f", (double)dcts_meas[pos].value);
        if(dcts_meas[pos].valid == 1){
            result = -1;
        }else{
            result = -2;
        }
        break;
    case CH_0_HUM_PAGE:
    case CH_1_HUM_PAGE:
    case CH_2_HUM_PAGE:
    case CH_3_HUM_PAGE:
        pos = (uint8_t)((page - CH_0_HUM_PAGE)/(CH_1_HUM_PAGE - CH_0_HUM_PAGE)*(CH_1_HUM - CH_0_HUM)+CH_0_HUM);
        sprintf(string, "%.1f", (double)dcts_meas[pos].value);
        if(dcts_meas[pos].valid == 1){
            result = -1;
        }else{
            result = -2;
        }
        break;
    case CH_0_ADC_PAGE:
    case CH_1_ADC_PAGE:
    case CH_2_ADC_PAGE:
    case CH_3_ADC_PAGE:
        pos = (uint8_t)((page - CH_0_ADC_PAGE)/(CH_1_ADC_PAGE - CH_0_ADC_PAGE)*(CH_1_ADC - CH_0_ADC)+CH_0_ADC);
        sprintf(string, "%.1f", (double)dcts_meas[pos].value);
        if(dcts_meas[pos].valid == 1){
            result = -1;
        }else{
            result = -2;
        }
        break;
    case CH_0_VLT_PAGE:
    case CH_1_VLT_PAGE:
    case CH_2_VLT_PAGE:
    case CH_3_VLT_PAGE:
        pos = (uint8_t)((page - CH_0_VLT_PAGE)/(CH_1_VLT_PAGE - CH_0_VLT_PAGE)*(CH_1_VLT - CH_0_VLT)+CH_0_VLT);
        sprintf(string, "%.1f", (double)dcts_meas[pos].value);
        if(dcts_meas[pos].valid == 1){
            result = -1;
        }else{
            result = -2;
        }
        break;
    case CH_0_CNT_PAGE:
    case CH_1_CNT_PAGE:
    case CH_2_CNT_PAGE:
    case CH_3_CNT_PAGE:
        pos = (uint8_t)((page - CH_0_CNT_PAGE)/(CH_1_CNT_PAGE - CH_0_CNT_PAGE)*(CH_1_CNT - CH_0_CNT)+CH_0_CNT);
        sprintf(string, "%.1f", (double)dcts_meas[pos].value);
        if(dcts_meas[pos].valid == 1){
            result = -1;
        }else{
            result = -2;
        }
        break;
    case CH_0_DI_STATE_PAGE:
    case CH_1_DI_STATE_PAGE:
    case CH_2_DI_STATE_PAGE:
    case CH_3_DI_STATE_PAGE:
        pos = (uint8_t)((page - CH_0_DI_STATE_PAGE)/(CH_1_DI_STATE_PAGE - CH_0_DI_STATE_PAGE)*(CH_1_DI - CH_0_DI)+CH_0_DI);
        sprintf(string, "%.1f", (double)dcts_meas[pos].value);
        if(dcts_meas[pos].valid == 1){
            result = -1;
        }else{
            result = -2;
        }
        break;
/*
    case ACT_EN_0:
    case ACT_EN_1:
        sprintf(string, "%s", off_on_descr[dcts_act[(uint8_t)(page - ACT_EN_0)/5].state.control]);
        break;

    case ACT_SET_0:
    case ACT_SET_1:
        sprintf(string, "%.1f%s", (double)dcts_act[(uint8_t)(page - ACT_EN_0)/5].set_value, dcts_act[(uint8_t)(page - ACT_EN_0)/5].unit_cyr);
        break;

    case ACT_HYST_0:
    case ACT_HYST_1:
        sprintf(string, "%.1f%s", (double)dcts_act[(uint8_t)(page - ACT_HYST_0)/5].hysteresis, dcts_act[(uint8_t)(page - ACT_HYST_0)/5].unit_cyr);
        break;

    case ACT_CUR_0:
    case ACT_CUR_1:
        sprintf(string, "%.1f%s", (double)dcts_act[(uint8_t)(page - ACT_CUR_0)/5].meas_value, dcts_act[(uint8_t)(page - ACT_CUR_0)/5].unit_cyr);
        break;

    case RELE_AUTO_MAN_0:
    case RELE_AUTO_MAN_1:
        sprintf(string, "%s", manual_auto_descr[dcts_rele[(uint8_t)(page - RELE_AUTO_MAN_0)/3].state.control_by_act]);
        break;

    case RELE_CONTROL_0:
    case RELE_CONTROL_1:
        sprintf(string, "%s", off_on_descr[dcts_rele[(uint8_t)(page - RELE_CONTROL_0)/3].state.control]);
        if(dcts_rele[(uint8_t)(page - RELE_CONTROL_0)/3].state.control_by_act == 1){
            result = -3;
        }
        break;*/

    case LIGHT_LVL:
        sprintf(string, "%d%%", config.params.lcd_backlight_lvl);
        SSD1306_SET_LIGHTLEVEL(config.params.lcd_backlight_lvl*255/100);
        break;
    case AUTO_OFF:
        sprintf(string, "%dÒ", config.params.lcd_backlight_time*10);
        break;
    /*case SKIN:
        sprintf(string, "%s", skin_descr[config.params.skin]);
        break;*/
    case TIME_HOUR:
        sprintf(string, "%02d", dcts.dcts_rtc.hour);
        break;
    case TIME_MIN:
        sprintf(string, "%02d", dcts.dcts_rtc.minute);
        break;
    case TIME_SEC:
        sprintf(string, "%02d", dcts.dcts_rtc.second);
        break;
    case DATE_DAY:
        sprintf(string, "%02d", dcts.dcts_rtc.day);
        break;
    case DATE_MONTH:
        sprintf(string, "%02d", dcts.dcts_rtc.month);
        break;
    case DATE_YEAR:
        sprintf(string, "%04d", dcts.dcts_rtc.year);
        break;
    }
    return result;
}

static void set_edit_value(menu_page_t page){
    switch(page){
    /*case ACT_EN_0:
        edit_val.type = VAL_UINT8;
        edit_val.digit_max = 0;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint8 = 0;
        edit_val.val_max.uint8 = 1;
        edit_val.p_val.p_uint8 = &dcts_act[HEATING].state.control;
        edit_val.select_shift = 0;
        edit_val.select_width = Font_7x10.FontWidth*5;
        break;
    case ACT_EN_1:
        edit_val.type = VAL_UINT8;
        edit_val.digit_max = 0;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint8 = 0;
        edit_val.val_max.uint8 = 1;
        edit_val.p_val.p_uint8 = &dcts_act[SEMISTOR].state.control;
        edit_val.select_shift = 0;
        edit_val.select_width = Font_7x10.FontWidth*5;
        break;
    case ACT_SET_0:
        edit_val.type = VAL_FLOAT;
        edit_val.digit_max = 2;
        edit_val.digit_min = -1;
        edit_val.digit = 0;
        edit_val.val_min.vfloat = 0.0;
        edit_val.val_max.vfloat = 40.0;
        edit_val.p_val.p_float = &dcts_act[HEATING].set_value;
        edit_val.select_shift = 4;
        edit_val.select_width = Font_7x10.FontWidth;
        break;
    case ACT_SET_1:
        edit_val.type = VAL_FLOAT;
        edit_val.digit_max = 2;
        edit_val.digit_min = -1;
        edit_val.digit = 0;
        edit_val.val_min.vfloat = 0.0;
        edit_val.val_max.vfloat = 100.0;
        edit_val.p_val.p_float = &dcts_act[SEMISTOR].set_value;
        edit_val.select_shift = 4;
        edit_val.select_width = Font_7x10.FontWidth;
        break;
    case ACT_HYST_0:
        edit_val.type = VAL_FLOAT;
        edit_val.digit_max = 2;
        edit_val.digit_min = -1;
        edit_val.digit = 0;
        edit_val.val_min.vfloat = 0.0;
        edit_val.val_max.vfloat = 100.0;
        edit_val.p_val.p_float = &dcts_act[HEATING].hysteresis;
        edit_val.select_shift = 4;
        edit_val.select_width = Font_7x10.FontWidth;
        break;
    case ACT_HYST_1:
        edit_val.type = VAL_FLOAT;
        edit_val.digit_max = 2;
        edit_val.digit_min = -1;
        edit_val.digit = 0;
        edit_val.val_min.vfloat = 0.0;
        edit_val.val_max.vfloat = 100.0;
        edit_val.p_val.p_float = &dcts_act[SEMISTOR].hysteresis;
        edit_val.select_shift = 4;
        edit_val.select_width = Font_7x10.FontWidth;
        break;
    case RELE_AUTO_MAN_0:
        edit_val.type = VAL_UINT8;
        edit_val.digit_max = 0;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint8 = 0;
        edit_val.val_max.uint8 = 1;
        edit_val.p_val.p_uint8 = &dcts_rele[HEATER].state.control_by_act;
        edit_val.select_shift = 0;
        edit_val.select_width = Font_7x10.FontWidth*6;
        break;
    case RELE_AUTO_MAN_1:
        edit_val.type = VAL_UINT8;
        edit_val.digit_max = 0;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint8 = 0;
        edit_val.val_max.uint8 = 1;
        edit_val.p_val.p_uint8 = &dcts_rele[LED].state.control_by_act;
        edit_val.select_shift = 0;
        edit_val.select_width = Font_7x10.FontWidth*6;
        break;
    case RELE_CONTROL_0:
        if(dcts_rele[HEATER].state.control_by_act == 0){
            edit_val.type = VAL_UINT8;
            edit_val.digit_max = 0;
            edit_val.digit_min = 0;
            edit_val.digit = 0;
            edit_val.val_min.uint8 = 0;
            edit_val.val_max.uint8 = 1;
            edit_val.p_val.p_uint8 = &dcts_rele[HEATER].state.control;
            edit_val.select_shift = 0;
            edit_val.select_width = Font_7x10.FontWidth*5;
        }
        break;
    case RELE_CONTROL_1:
        if(dcts_rele[LED].state.control_by_act == 0){
            edit_val.type = VAL_UINT8;
            edit_val.digit_max = 0;
            edit_val.digit_min = 0;
            edit_val.digit = 0;
            edit_val.val_min.uint8 = 0;
            edit_val.val_max.uint8 = 1;
            edit_val.p_val.p_uint8 = &dcts_rele[LED].state.control;
            edit_val.select_shift = 0;
            edit_val.select_width = Font_7x10.FontWidth*5;
        }
        break;*/
    case LIGHT_LVL:
        edit_val.type = VAL_UINT16;
        edit_val.digit_max = 2;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint16 = 1;
        edit_val.val_max.uint16 = 100;
        edit_val.p_val.p_uint16 = &config.params.lcd_backlight_lvl;
        edit_val.select_shift = 1;
        edit_val.select_width = Font_7x10.FontWidth;
        break;
    case AUTO_OFF:
        edit_val.type = VAL_UINT16;
        edit_val.digit_max = 1;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint16 = 0;
        edit_val.val_max.uint16 = 60;
        edit_val.p_val.p_uint16 = &config.params.lcd_backlight_time;
        edit_val.select_shift = 2;
        edit_val.select_width = Font_7x10.FontWidth;
        break;
    /*case SKIN:
        edit_val.type = VAL_UINT8;
        edit_val.digit_max = 0;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint8 = 0;
        edit_val.val_max.uint8 = 2;
        edit_val.p_val.p_uint8 = &config.params.skin;
        edit_val.select_shift = 0;
        edit_val.select_width = Font_7x10.FontWidth*6;
        break;*/
    case TIME_HOUR:
        edit_val.type = VAL_UINT8;
        edit_val.digit_max = 1;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint8 = 0;
        edit_val.val_max.uint8 = 23;
        edit_val.p_val.p_uint8 = &dcts.dcts_rtc.hour;
        edit_val.select_shift = 0;
        edit_val.select_width = Font_7x10.FontWidth;
        break;
    case TIME_MIN:
        edit_val.type = VAL_UINT8;
        edit_val.digit_max = 1;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint8 = 0;
        edit_val.val_max.uint8 = 59;
        edit_val.p_val.p_uint8 = &dcts.dcts_rtc.minute;
        edit_val.select_shift = 0;
        edit_val.select_width = Font_7x10.FontWidth;
        break;
    case TIME_SEC:
        edit_val.type = VAL_UINT8;
        edit_val.digit_max = 1;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint8 = 0;
        edit_val.val_max.uint8 = 59;
        edit_val.p_val.p_uint8 = &dcts.dcts_rtc.second;
        edit_val.select_shift = 0;
        edit_val.select_width = Font_7x10.FontWidth;
        break;
    case DATE_DAY:
        edit_val.type = VAL_UINT8;
        edit_val.digit_max = 1;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint8 = 1;
        edit_val.val_max.uint8 = 31;
        edit_val.p_val.p_uint8 = &dcts.dcts_rtc.day;
        edit_val.select_shift = 0;
        edit_val.select_width = Font_7x10.FontWidth;
        break;
    case DATE_MONTH:
        edit_val.type = VAL_UINT8;
        edit_val.digit_max = 1;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint8 = 1;
        edit_val.val_max.uint8 = 12;
        edit_val.p_val.p_uint8 = &dcts.dcts_rtc.month;
        edit_val.select_shift = 0;
        edit_val.select_width = Font_7x10.FontWidth;
        break;
    case DATE_YEAR:
        edit_val.type = VAL_UINT16;
        edit_val.digit_max = 3;
        edit_val.digit_min = 0;
        edit_val.digit = 0;
        edit_val.val_min.uint16 = 2000;
        edit_val.val_max.uint16 = 3000;
        edit_val.p_val.p_uint16 = &dcts.dcts_rtc.year;
        edit_val.select_shift = 0;
        edit_val.select_width = Font_7x10.FontWidth;
        break;
    }
}

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
        /*if(button_click(BUTTON_BREAK,BUTTON_CLICK_TIME)){
            if(LCD.auto_off == 0){
                LCD_backlight_toggle();
            }
        }
        if(button_click(BUTTON_SET,BUTTON_CLICK_TIME)){
            save_params();
        }*/

        /*if((pressed_time[BUTTON_BREAK].pressed > 0)&&(pressed_time[BUTTON_BREAK].pressed < navigation_task_period)){
            if(LCD.auto_off == 0){
                LCD_backlight_toggle();
            }
        }*/
        if(button_clamp(BUTTON_OK, 1000)){
            save_params();
            pressed_time[BUTTON_OK].pressed = 0;
        }
        osDelayUntil(&last_wake_time, navigation_task_period);
    }
}

static void save_params(void){
    static menuItem* current_menu;
    current_menu = selectedMenuItem;
    menuChange(&save_changes);

    // store ModBus params
    config.params.mdb_address = dcts.dcts_address;
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
    //delay for show message
    osDelay(2000);
    menuChange(current_menu);
}

void restore_params(void){
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
        set_def_params();
    }
}

int set_def_params(void){
    int result = 0;
    config.params.mdb_address = 10;
    dcts.dcts_address = (uint8_t)config.params.mdb_address;
    config.params.mdb_bitrate = BITRATE_115200;
    config.params.lcd_backlight_lvl = 3;
    config.params.lcd_backlight_time = 12;
    for(uint8_t i = 0; i < ACT_NUM; i++){
        config.params.act_enable[i] = 0;
        config.params.act_set[i] = 0.0f;
        config.params.act_hyst[i] = 1.0f;
        dcts_act[i].set_value = config.params.act_set[i];
        dcts_act[i].hysteresis = config.params.act_hyst[i];
        dcts_act[i].state.control = (uint8_t)config.params.act_enable[i];
    }
    for(uint8_t i = 0; i < RELE_NUM; i++){
        config.params.rele[i] = 0;
        dcts_rele[i].state.control_by_act = (uint8_t)(config.params.rele[i]);
    }
    for(uint8_t i = 0; i < CH_NUM; i++){
        config.params.ch_mode[i] = CH_MODE_NONE;
    }
    return result;
}


uint32_t uint32_pow(uint16_t x, uint8_t pow){
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
}

#endif //DISPLAY_C
