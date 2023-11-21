#include "esp_log.h"
#include "xiao_round_display.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"

static lv_obj_t *slider_label;

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    lv_label_set_text_fmt(slider_label, "%"LV_PRId32, lv_slider_get_value(slider));
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);
}


void xiao_round_display_hardware_test()
{

    lv_obj_t *scr = lv_scr_act();

    /* Task lock */
    lvgl_port_lock(0);

    lv_obj_t * slider = lv_slider_create(scr);
    lv_obj_set_width(slider, 120);
    lv_obj_set_pos(slider, 60, 120);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    slider_label = lv_label_create(scr);
    lv_label_set_text(slider_label, "0");
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);

    /* Task unlock */
    lvgl_port_unlock();
}

void app_main(void)
{
    xiao_round_display_init();
    xiao_round_display_hardware_test();
}
