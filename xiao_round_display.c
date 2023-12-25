#include "esp_log.h"
#include "esp_check.h"

#include "xiao_round_display.h"
#include "xiao_common.h"
#include "xiao_lcd.h"
#include "xiao_touch.h"
#include "xiao_lvgl.h"


esp_err_t xiao_round_display_init()
{
    ESP_RETURN_ON_ERROR(init_common(), TAG, "");
    ESP_RETURN_ON_ERROR(init_lcd(), TAG, "LCD initialization failed");
    ESP_RETURN_ON_ERROR(init_touch(), TAG, "Touch initialization failed");
    ESP_RETURN_ON_ERROR(init_lvgl(), TAG, "LVGL initialization failed");

    ESP_LOGI(TAG, "XIAO round display initialization succeed");
    return ESP_OK;
}

void xiao_round_display_deinit()
{

}
