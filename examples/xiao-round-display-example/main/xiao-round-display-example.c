#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "xiao_round_display.h"
#include "xiao_sd_card.h"
#include "xiao_battery.h"
#include "xiao_rtc.h"

#define NUM_ADC_SAMPLE 20

static lv_obj_t *slider_label;
static lv_obj_t *battery_bar, *battery_label;


void rtc_clock_cb(lv_timer_t *timer)
{
    lv_obj_t *rtc_clock = (lv_obj_t *)timer->user_data;
    struct tm time_data;
    xiao_round_display_rtc_get_time(&time_data);
    char rtc_time[10];
    sprintf(rtc_time, "%d:%d:%d", time_data.tm_hour, time_data.tm_min, time_data.tm_sec);
    lv_label_set_text(rtc_clock, rtc_time);
}

static void event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *obj = lv_event_get_target(e);
        LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
    }
}

static void hardware_polled_cb(lv_timer_t *timer)
{
    lv_obj_t *tf_state = (lv_obj_t *)timer->user_data;
    if (xiao_round_display_sd_card_init() == ESP_OK) {
        lv_obj_add_state(tf_state, LV_STATE_CHECKED);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        xiao_round_display_sd_card_deinit();
    } else {
        lv_obj_clear_state(tf_state, LV_STATE_CHECKED);
    }
    lv_label_set_text_fmt(battery_label, "%"LV_PRId32"%%", xiao_round_display_battery_level_read());
}

static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    lv_label_set_text_fmt(slider_label, "%"LV_PRId32, lv_slider_get_value(slider));
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);
}

void xiao_round_display_hardware_test()
{

    lv_obj_t *scr = lv_scr_act();

    /* Task lock */
    lvgl_port_lock(0);

    /* Slider */
    lv_obj_t *slider = lv_slider_create(scr);
    lv_obj_set_width(slider, 120);
    lv_obj_set_pos(slider, 60, 120);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    slider_label = lv_label_create(scr);
    lv_label_set_text(slider_label, "0");
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);
    // lv_obj_set_style_bg_color(slider, lv_color_hex(0xfa0000), LV_PART_MAIN | LV_STATE_DEFAULT );

    /* SD card  check*/
    lv_obj_t *tf_label = lv_label_create(lv_scr_act());
    lv_label_set_text(tf_label, "tf-card");
    lv_obj_set_pos(tf_label, 90, 170);
    lv_obj_t *tf_state = lv_switch_create(lv_scr_act());
    lv_obj_set_pos(tf_state, 90, 190);
    lv_obj_add_state(tf_state, LV_STATE_CHECKED | LV_STATE_DISABLED);
    lv_obj_add_event_cb(tf_state, event_handler, LV_EVENT_ALL, NULL);
    lv_timer_create(hardware_polled_cb, 7000, tf_state);

    /* rtc */
    struct tm time_data = { 0 };
    xiao_round_display_rtc_get_time(&time_data);
    char rtc_time[10];
    sprintf(rtc_time, "%d:%d:%d", time_data.tm_hour, time_data.tm_min, time_data.tm_sec);
    lv_obj_t *rtc_clock = lv_label_create(lv_scr_act());
    lv_label_set_text(rtc_clock, rtc_time);
    lv_obj_set_pos(rtc_clock, 55, 45);
    lv_timer_create(rtc_clock_cb, 1000, rtc_clock);

    /* Battery */
    lv_obj_t *battery_outline = lv_obj_create(lv_scr_act());
    lv_obj_set_style_border_width(battery_outline, 2, 0);
    lv_obj_set_style_pad_all(battery_outline, 0, 0);
    lv_obj_set_style_radius(battery_outline, 8, 0);
    lv_obj_clear_flag(battery_outline, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(battery_outline, 40, 20);
    lv_obj_set_pos(battery_outline, 128, 42);
    // lv_obj_set_style_bg_color(battery_outline, lv_color_hex(0x08EB25), LV_PART_MAIN | LV_STATE_DEFAULT );

    battery_bar = lv_bar_create(battery_outline);
    lv_obj_set_size(battery_bar, 40, 20);
    lv_obj_align_to(battery_bar, battery_outline, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_range(battery_bar, 0, 100);
    lv_bar_set_value(battery_bar, 80, LV_ANIM_OFF);
    // lv_obj_set_style_bg_color(battery_bar, lv_palette_main(LV_PALETTE_GREEN), 0);

    battery_label = lv_label_create(battery_outline);
    lv_obj_align_to(battery_label, battery_outline, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text_fmt(battery_label, "80%%");

    /* Task unlock */
    lvgl_port_unlock();
}

void app_main(void)
{
    xiao_round_display_init();
    xiao_round_display_battery_ADC_init();
    xiao_round_display_hardware_test();
}
