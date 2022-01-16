#include "stdint.h"

#ifndef MENU_H
#define MENU_H 1

/*========== DEFINES ==========*/

#define MAKE_MENU(Name, Next, Previous, Parent, Child, Child_num, Page, Text) \
    extern const menuItem Next;     \
    extern const menuItem Previous; \
    extern const menuItem Parent;   \
    extern const menuItem Child;  \
    const menuItem Name = {(const void*)&Next, (const void*)&Previous, (const void*)&Parent, (const void*)&Child, (const uint16_t)Child_num, (const uint16_t)Page, { Text }}

/*#define PREVIOUS   ((menuItem*)pgm_read_word(&selectedMenuItem->Previous))
#define NEXT       ((menuItem*)pgm_read_word(&selectedMenuItem->Next))
#define PARENT     ((menuItem*)pgm_read_word(&selectedMenuItem->Parent))
#define CHILD      ((menuItem*)pgm_read_word(&selectedMenuItem->Child))
#define SELECT		(pgm_read_byte(&selectedMenuItem->Select))*/
#define NULL_ENTRY  Null_Menu
#define EDITED_VAL  edit_value

/*========== TYPEDEFS ==========*/

typedef enum {
    MAIN_PAGE = 0,
    MAIN_MENU,

    COMMON_INFO,
    INFO,

    MEAS_CHANNELS,
    SELF_TMPR,
    SELF_HUM,
    SELF_VREF,
    SELF_VBAT,

    CH_0_PAGE,
    CH_0_MODE_PAGE,
    CH_0_DO_STATE_PAGE,
    CH_0_DO_CTRL_PAGE,
    CH_0_PWM_DUTY_PAGE,
    CH_0_PWM_CTRL_PAGE,
    CH_0_TMPR_PAGE,
    CH_0_HUM_PAGE,
    CH_0_ADC_PAGE,
    CH_0_VLT_PAGE,
    CH_0_CNT_PAGE,
    CH_0_DI_STATE_PAGE,

    CH_1_PAGE,
    CH_1_MODE_PAGE,
    CH_1_DO_STATE_PAGE,
    CH_1_DO_CTRL_PAGE,
    CH_1_PWM_DUTY_PAGE,
    CH_1_PWM_CTRL_PAGE,
    CH_1_TMPR_PAGE,
    CH_1_HUM_PAGE,
    CH_1_ADC_PAGE,
    CH_1_VLT_PAGE,
    CH_1_CNT_PAGE,
    CH_1_DI_STATE_PAGE,

    CH_2_PAGE,
    CH_2_MODE_PAGE,
    CH_2_DO_STATE_PAGE,
    CH_2_DO_CTRL_PAGE,
    CH_2_PWM_DUTY_PAGE,
    CH_2_PWM_CTRL_PAGE,
    CH_2_TMPR_PAGE,
    CH_2_HUM_PAGE,
    CH_2_ADC_PAGE,
    CH_2_VLT_PAGE,
    CH_2_CNT_PAGE,
    CH_2_DI_STATE_PAGE,

    CH_3_PAGE,
    CH_3_MODE_PAGE,
    CH_3_DO_STATE_PAGE,
    CH_3_DO_CTRL_PAGE,
    CH_3_PWM_DUTY_PAGE,
    CH_3_PWM_CTRL_PAGE,
    CH_3_TMPR_PAGE,
    CH_3_HUM_PAGE,
    CH_3_ADC_PAGE,
    CH_3_VLT_PAGE,
    CH_3_CNT_PAGE,
    CH_3_DI_STATE_PAGE,

    ALGORITHMS,
    ON_OFF_TIMER_1,
    ON_OFF_TIMER_2,
    ON_OFF_TIMER_3,
    ON_OFF_TIMER_4,
    THERMOSTAT_1,
    THERMOSTAT_2,
    VLT_COMPARE_1,
    VLT_COMPARE_2,

    CONNECTION,
    MDB_ADDR,
    MDB_BITRATE,
    MDB_RECIEVED,
    MDB_SENT,
    MDB_OVERRUN_ERR,
    MDB_PARITY_ERR,
    MDB_FRAME_ERR,
    MDB_NOISE_ERR,

    DISPLAY,
    LIGHT_LVL,
    AUTO_OFF,

    TIME,
    TIME_HOUR,
    TIME_MIN,
    TIME_SEC,

    DATE,
    DATE_DAY,
    DATE_MONTH,
    DATE_YEAR,

    SAVE_CHANGES,
    EDIT,
} menu_page_t;

typedef struct {
    const void          *Next;
    const void          *Previous;
    const void          *Parent;
    const void          *Child;
    const uint16_t      Child_num;
    const menu_page_t   Page;
    const char          Text[20];
} menuItem;



/*========= GLOBAL VARIABLES ==========*/

extern const menuItem main_page;
extern const menuItem save_changes;
extern menuItem* selectedMenuItem;
extern const menuItem edit_value;

/*========== FUNCTION PROTOTYPES ==========*/

void menu_init (void);

void menuChange(menuItem* NewMenu);

#endif // MENU_H
