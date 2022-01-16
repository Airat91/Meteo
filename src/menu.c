#include "menu.h"
#include "main.h"
#include "buttons.h"
#include "dcts.h"
#include "dcts_config.h"
#include "string.h"

/**
  * @defgroup menu
  * @brief work with menu
  */


static const menuItem Null_Menu = {
    .Next = (void*)0,
    .Previous = (void*)0,
    .Parent = (void*)0,
    .Child = (void*)0,
    .Child_num = 0,
    .Page = 0,
    .Text = {0},
};
const menuItem edit_value = {
    .Next = (void*)0,
    .Previous = (void*)0,
    .Parent = (void*)0,
    .Child = (void*)0,
    .Child_num = 0,
    .Page = EDIT,
    .Text = {0},
};

menuItem* selectedMenuItem;

//                  NAME           NEXT            PREV            PARENT          CHILD        GHILD_NUM   PAGE                    TEXT
MAKE_MENU       (main_page,     NULL_ENTRY,     NULL_ENTRY,     NULL_ENTRY,     common_info,    8,          MAIN_PAGE,          "Главное меню");
  MAKE_MENU     (common_info,   meas_channels,  date,           main_page,      info,           1,          COMMON_INFO,        "Об устройстве");
    MAKE_MENU   (info,          NULL_ENTRY,     NULL_ENTRY,     common_info,    NULL_ENTRY,     0,          INFO,               "Об устройстве");
  MAKE_MENU     (meas_channels, ch_0_page,      common_info,    main_page,      self_tmpr,      4,          MEAS_CHANNELS,      "Изм. каналы");
    MAKE_MENU   (self_tmpr,     self_hum,       vbat,           meas_channels,  NULL_ENTRY,     0,          SELF_TMPR,          "Темп. собств.");
    MAKE_MENU   (self_hum,      vref,           self_tmpr,      meas_channels,  NULL_ENTRY,     0,          SELF_HUM,           "Вл-ть собств.");
    MAKE_MENU   (vref,          vbat,           self_hum,       meas_channels,  NULL_ENTRY,     0,          SELF_VREF,          "Опорное напр. В");
    MAKE_MENU   (vbat,          self_tmpr,      vref,           meas_channels,  NULL_ENTRY,     0,          SELF_VBAT,          "Батарейка В");
  MAKE_MENU     (ch_0_page,     ch_1_page,      meas_channels,  main_page,      ch_0_mode,      11,         CH_0_PAGE,          "Канал 0");
    MAKE_MENU   (ch_0_mode,     ch_0_do_st,     ch_0_di_st,     ch_0_page,      EDITED_VAL,     0,          CH_0_MODE_PAGE,     "Режим");
    MAKE_MENU   (ch_0_do_st,    ch_0_do_strl,   ch_0_mode,      ch_0_page,      NULL_ENTRY,     0,          CH_0_DO_STATE_PAGE, "DO сост.");
    MAKE_MENU   (ch_0_do_strl,  ch_0_pwm_duty,  ch_0_do_st,     ch_0_page,      EDITED_VAL,     0,          CH_0_DO_CTRL_PAGE,  "DO упр.");
    MAKE_MENU   (ch_0_pwm_duty, ch_0_pwm_ctrl,  ch_0_do_strl,   ch_0_page,      EDITED_VAL,     0,          CH_0_PWM_DUTY_PAGE, "ШИМ скв-ть");
    MAKE_MENU   (ch_0_pwm_ctrl, ch_0_tmpr,      ch_0_pwm_duty,  ch_0_page,      EDITED_VAL,     0,          CH_0_PWM_CTRL_PAGE, "ШИМ упр.");
    MAKE_MENU   (ch_0_tmpr,     ch_0_hum,       ch_0_pwm_ctrl,  ch_0_page,      NULL_ENTRY,     0,          CH_0_TMPR_PAGE,     "Темп.");
    MAKE_MENU   (ch_0_hum,      ch_0_adc,       ch_0_tmpr,      ch_0_page,      NULL_ENTRY,     0,          CH_0_HUM_PAGE,      "Вл-ть");
    MAKE_MENU   (ch_0_adc,      ch_0_vlt,       ch_0_hum,       ch_0_page,      NULL_ENTRY,     0,          CH_0_ADC_PAGE,      "АЦП");
    MAKE_MENU   (ch_0_vlt,      ch_0_cnt,       ch_0_adc,       ch_0_page,      NULL_ENTRY,     0,          CH_0_VLT_PAGE,      "Напряжение");
    MAKE_MENU   (ch_0_cnt,      ch_0_di_st,     ch_0_vlt,       ch_0_page,      EDITED_VAL,     0,          CH_0_CNT_PAGE,      "Счетчик");
    MAKE_MENU   (ch_0_di_st,    ch_0_mode,      ch_0_cnt,       ch_0_page,      NULL_ENTRY,     0,          CH_0_DI_STATE_PAGE, "DI сост.");
  MAKE_MENU     (ch_1_page,     ch_2_page,      ch_0_page,      main_page,      ch_1_mode,      11,         CH_1_PAGE,          "Канал 1");
    MAKE_MENU   (ch_1_mode,     ch_1_do_st,     ch_1_di_st,     ch_1_page,      EDITED_VAL,     0,          CH_1_MODE_PAGE,     "Режим");
    MAKE_MENU   (ch_1_do_st,    ch_1_do_strl,   ch_1_mode,      ch_1_page,      NULL_ENTRY,     0,          CH_1_DO_STATE_PAGE, "DO сост.");
    MAKE_MENU   (ch_1_do_strl,  ch_1_pwm_duty,  ch_1_do_st,     ch_1_page,      EDITED_VAL,     0,          CH_1_DO_CTRL_PAGE,  "DO упр.");
    MAKE_MENU   (ch_1_pwm_duty, ch_1_pwm_ctrl,  ch_1_do_strl,   ch_1_page,      EDITED_VAL,     0,          CH_1_PWM_DUTY_PAGE, "ШИМ скв-ть");
    MAKE_MENU   (ch_1_pwm_ctrl, ch_1_tmpr,      ch_1_pwm_duty,  ch_1_page,      EDITED_VAL,     0,          CH_1_PWM_CTRL_PAGE, "ШИМ упр.");
    MAKE_MENU   (ch_1_tmpr,     ch_1_hum,       ch_1_pwm_ctrl,  ch_1_page,      NULL_ENTRY,     0,          CH_1_TMPR_PAGE,     "Темп.");
    MAKE_MENU   (ch_1_hum,      ch_1_adc,       ch_1_tmpr,      ch_1_page,      NULL_ENTRY,     0,          CH_1_HUM_PAGE,      "Вл-ть");
    MAKE_MENU   (ch_1_adc,      ch_1_vlt,       ch_1_hum,       ch_1_page,      NULL_ENTRY,     0,          CH_1_ADC_PAGE,      "АЦП");
    MAKE_MENU   (ch_1_vlt,      ch_1_cnt,       ch_1_adc,       ch_1_page,      NULL_ENTRY,     0,          CH_1_VLT_PAGE,      "Напряжение");
    MAKE_MENU   (ch_1_cnt,      ch_1_di_st,     ch_1_vlt,       ch_1_page,      EDITED_VAL,     0,          CH_1_CNT_PAGE,      "Счетчик");
    MAKE_MENU   (ch_1_di_st,    ch_1_mode,      ch_1_cnt,       ch_1_page,      NULL_ENTRY,     0,          CH_1_DI_STATE_PAGE, "DI сост.");
  MAKE_MENU     (ch_2_page,     ch_3_page,      ch_1_page,      main_page,      ch_2_mode,      11,         CH_2_PAGE,          "Канал 2");
    MAKE_MENU   (ch_2_mode,     ch_2_do_st,     ch_2_di_st,     ch_2_page,      EDITED_VAL,     0,          CH_2_MODE_PAGE,     "Режим");
    MAKE_MENU   (ch_2_do_st,    ch_2_do_strl,   ch_2_mode,      ch_2_page,      NULL_ENTRY,     0,          CH_2_DO_STATE_PAGE, "DO сост.");
    MAKE_MENU   (ch_2_do_strl,  ch_2_pwm_duty,  ch_2_do_st,     ch_2_page,      EDITED_VAL,     0,          CH_2_DO_CTRL_PAGE,  "DO упр.");
    MAKE_MENU   (ch_2_pwm_duty, ch_2_pwm_ctrl,  ch_2_do_strl,   ch_2_page,      EDITED_VAL,     0,          CH_2_PWM_DUTY_PAGE, "ШИМ скв-ть");
    MAKE_MENU   (ch_2_pwm_ctrl, ch_2_tmpr,      ch_2_pwm_duty,  ch_2_page,      EDITED_VAL,     0,          CH_2_PWM_CTRL_PAGE, "ШИМ упр.");
    MAKE_MENU   (ch_2_tmpr,     ch_2_hum,       ch_2_pwm_ctrl,  ch_2_page,      NULL_ENTRY,     0,          CH_2_TMPR_PAGE,     "Темп.");
    MAKE_MENU   (ch_2_hum,      ch_2_adc,       ch_2_tmpr,      ch_2_page,      NULL_ENTRY,     0,          CH_2_HUM_PAGE,      "Вл-ть");
    MAKE_MENU   (ch_2_adc,      ch_2_vlt,       ch_2_hum,       ch_2_page,      NULL_ENTRY,     0,          CH_2_ADC_PAGE,      "АЦП");
    MAKE_MENU   (ch_2_vlt,      ch_2_cnt,       ch_2_adc,       ch_2_page,      NULL_ENTRY,     0,          CH_2_VLT_PAGE,      "Напряжение");
    MAKE_MENU   (ch_2_cnt,      ch_2_di_st,     ch_2_vlt,       ch_2_page,      EDITED_VAL,     0,          CH_2_CNT_PAGE,      "Счетчик");
    MAKE_MENU   (ch_2_di_st,    ch_2_mode,      ch_2_cnt,       ch_2_page,      NULL_ENTRY,     0,          CH_2_DI_STATE_PAGE, "DI сост.");
  MAKE_MENU     (ch_3_page,     algoritms,      ch_2_page,      main_page,      ch_3_mode,      11,         CH_3_PAGE,          "Канал 3");
    MAKE_MENU   (ch_3_mode,     ch_3_do_st,     ch_3_di_st,     ch_3_page,      EDITED_VAL,     0,          CH_3_MODE_PAGE,     "Режим");
    MAKE_MENU   (ch_3_do_st,    ch_3_do_strl,   ch_3_mode,      ch_3_page,      NULL_ENTRY,     0,          CH_3_DO_STATE_PAGE, "DO сост.");
    MAKE_MENU   (ch_3_do_strl,  ch_3_pwm_duty,  ch_3_do_st,     ch_3_page,      EDITED_VAL,     0,          CH_3_DO_CTRL_PAGE,  "DO упр.");
    MAKE_MENU   (ch_3_pwm_duty, ch_3_pwm_ctrl,  ch_3_do_strl,   ch_3_page,      EDITED_VAL,     0,          CH_3_PWM_DUTY_PAGE, "ШИМ скв-ть");
    MAKE_MENU   (ch_3_pwm_ctrl, ch_3_tmpr,      ch_3_pwm_duty,  ch_3_page,      EDITED_VAL,     0,          CH_3_PWM_CTRL_PAGE, "ШИМ упр.");
    MAKE_MENU   (ch_3_tmpr,     ch_3_hum,       ch_3_pwm_ctrl,  ch_3_page,      NULL_ENTRY,     0,          CH_3_TMPR_PAGE,     "Темп.");
    MAKE_MENU   (ch_3_hum,      ch_3_adc,       ch_3_tmpr,      ch_3_page,      NULL_ENTRY,     0,          CH_3_HUM_PAGE,      "Вл-ть");
    MAKE_MENU   (ch_3_adc,      ch_3_vlt,       ch_3_hum,       ch_3_page,      NULL_ENTRY,     0,          CH_3_ADC_PAGE,      "АЦП");
    MAKE_MENU   (ch_3_vlt,      ch_3_cnt,       ch_3_adc,       ch_3_page,      NULL_ENTRY,     0,          CH_3_VLT_PAGE,      "Напряжение");
    MAKE_MENU   (ch_3_cnt,      ch_3_di_st,     ch_3_vlt,       ch_3_page,      EDITED_VAL,     0,          CH_3_CNT_PAGE,      "Счетчик");
    MAKE_MENU   (ch_3_di_st,    ch_3_mode,      ch_3_cnt,       ch_3_page,      NULL_ENTRY,     0,          CH_3_DI_STATE_PAGE, "DI сост.");
  MAKE_MENU     (algoritms,     connection,     ch_3_page,      main_page,      on_off_1,       8,          ALGORITHMS,         "Алгоритмы");
    MAKE_MENU   (on_off_1,      on_off_2,       vlt_comp_2,     algoritms,      NULL_ENTRY,     0,          ON_OFF_TIMER_1,     "Таймер 1");
    MAKE_MENU   (on_off_2,      on_off_3,       on_off_1,       algoritms,      NULL_ENTRY,     0,          ON_OFF_TIMER_2,     "Таймер 2");
    MAKE_MENU   (on_off_3,      on_off_4,       on_off_2,       algoritms,      NULL_ENTRY,     0,          ON_OFF_TIMER_3,     "Таймер 3");
    MAKE_MENU   (on_off_4,      therm_1,        on_off_3,       algoritms,      NULL_ENTRY,     0,          ON_OFF_TIMER_4,     "Таймер 4");
    MAKE_MENU   (therm_1,       therm_2,        on_off_4,       algoritms,      NULL_ENTRY,     0,          THERMOSTAT_1,       "Термостат 1");
    MAKE_MENU   (therm_2,       vlt_comp_1,     therm_1,        algoritms,      NULL_ENTRY,     0,          THERMOSTAT_2,       "Термостат 2");
    MAKE_MENU   (vlt_comp_1,    vlt_comp_2,     therm_2,        algoritms,      NULL_ENTRY,     0,          VLT_COMPARE_1,      "Компаратор 1");
    MAKE_MENU   (vlt_comp_2,    on_off_1,       vlt_comp_1,     algoritms,      NULL_ENTRY,     0,          VLT_COMPARE_2,      "Компаратор 2");
  MAKE_MENU     (connection,    display,        algoritms,      main_page,      mdb_addr,       8,          CONNECTION,         "Связь");
    MAKE_MENU   (mdb_addr,      bitrate,        noise_err,      connection,     EDITED_VAL,     0,          MDB_ADDR,           "Адрес ModBUS");
    MAKE_MENU   (bitrate,       recieved,       mdb_addr,       connection,     EDITED_VAL,     0,          MDB_BITRATE,        "Битрейт");
    MAKE_MENU   (recieved,      sended,         bitrate,        connection,     NULL_ENTRY,     0,          MDB_RECIEVED,       "Получено");
    MAKE_MENU   (sended,        overrun_err,    recieved,       connection,     NULL_ENTRY,     0,          MDB_SENT,           "Отправлено");
    MAKE_MENU   (overrun_err,   parity_err,     sended,         connection,     NULL_ENTRY,     0,          MDB_OVERRUN_ERR,    "Ошибки чтения");
    MAKE_MENU   (parity_err,    frame_err,      overrun_err,    connection,     NULL_ENTRY,     0,          MDB_PARITY_ERR,     "Ошибки паритета");
    MAKE_MENU   (frame_err,     noise_err,      parity_err,     connection,     NULL_ENTRY,     0,          MDB_FRAME_ERR,      "Ошибки кадра");
    MAKE_MENU   (noise_err,     mdb_addr,       frame_err,      connection,     NULL_ENTRY,     0,          MDB_NOISE_ERR,      "Ошибки помехи");
  MAKE_MENU     (display,       time,           connection,     main_page,      light_lvl,      2,          DISPLAY,            "Дисплей");
    MAKE_MENU   (light_lvl,     auto_off,       auto_off,       display,        EDITED_VAL,     0,          LIGHT_LVL,          "Яркость");
    MAKE_MENU   (auto_off,      light_lvl,      light_lvl,      display,        EDITED_VAL,     0,          AUTO_OFF,           "Автовыкл. подсветки");
  MAKE_MENU     (time,          date,           display,        main_page,      time_hour,      3,          TIME,               "Время");
    MAKE_MENU   (time_hour,     time_min,       time_sec,       time,           EDITED_VAL,     0,          TIME_HOUR,          "Часы");
    MAKE_MENU   (time_min,      time_sec,       time_hour,      time,           EDITED_VAL,     0,          TIME_MIN,           "Минуты");
    MAKE_MENU   (time_sec,      time_hour,      time_min,       time,           EDITED_VAL,     0,          TIME_SEC,           "Секунды");
  MAKE_MENU     (date,          common_info,    time,           main_page,      date_day,       3,          DATE,               "Дата");
    MAKE_MENU   (date_day,      date_month,     date_year,      date,           EDITED_VAL,     0,          DATE_DAY,           "День");
    MAKE_MENU   (date_month,    date_year,      date_day,       date,           EDITED_VAL,     0,          DATE_MONTH,         "Месяц");
    MAKE_MENU   (date_year,     date_day,       date_month,     date,           EDITED_VAL,     0,          DATE_YEAR,          "Год");

MAKE_MENU       (save_changes,  NULL_ENTRY,     NULL_ENTRY,     NULL_ENTRY,     NULL_ENTRY,     0,          SAVE_CHANGES,       "Сохранить изм.");


/*========== FUNCTIONS ==========*/

void menu_init (void){
    selectedMenuItem = &main_page;
}

void menuChange(menuItem* NewMenu){
    if ((NewMenu != &NULL_ENTRY)&&(NewMenu != &EDITED_VAL)){
        selectedMenuItem = NewMenu;
    }else if(NewMenu == &EDITED_VAL){
        navigation_style = DIGIT_EDIT;
    }
}
