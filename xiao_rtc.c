#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2c.h"
#include <string.h>
#include "pcf8563.h"

#include "xiao_common.h"
#include "xiao_rtc.h"

/* rtc */
i2c_dev_t rtc_dev;

esp_err_t xiao_round_display_rtc_init()
{
    esp_err_t ret = ESP_OK;
    ret = i2cdev_init();
    ESP_RETURN_ON_ERROR(ret, TAG, "i2cdev initialization failed");
    memset(&rtc_dev, 0x00, sizeof(i2c_dev_t));
    ret = pcf8563_init_desc(&rtc_dev, XIAO_RTC_I2C_PORT, XIAO_I2C_SDA, XIAO_I2C_SCL);
    ESP_RETURN_ON_ERROR(ret, TAG, "pcf8563 initialization failed");
    return ret;
}

void xiao_round_display_rtc_deinit()
{
    ESP_ERROR_CHECK(pcf8563_free_desc(&rtc_dev));
    ESP_ERROR_CHECK(i2cdev_done());
}

esp_err_t xiao_round_display_rtc_get_time(struct tm *time)
{
    bool valid = false;
    return pcf8563_get_time(&rtc_dev, time, &valid);
}

esp_err_t xiao_round_display_rtc_set_time(struct tm *time)
{
    return pcf8563_set_time(&rtc_dev, time);
}