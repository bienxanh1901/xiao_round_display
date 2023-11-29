#include "esp_log.h"
#include "esp_check.h"
#include <string.h>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "xiao_common.h"
#include "xiao_rtc.h"

#define PCF8563_I2C_ADDR 0x51

#define REG_CTRL_STATUS1 0x00
#define REG_CTRL_STATUS2 0x01
#define REG_VL_SECONDS   0x02
#define REG_MINUTES      0x03
#define REG_HOURS        0x04
#define REG_DAYS         0x05
#define REG_WEEKDAYS     0x06
#define REG_CENT_MONTHS  0x07
#define REG_YEARS        0x08
#define REG_ALARM_MIN    0x09
#define REG_ALARM_HOUR   0x0a
#define REG_ALARM_DAY    0x0b
#define REG_ALARM_WDAY   0x0c
#define REG_CLKOUT       0x0d
#define REG_TIMER_CTRL   0x0e
#define REG_TIMER        0x0f

#define BIT_YEAR_CENTURY 7
#define BIT_VL 7
#define BIT_AE 7
#define BIT_CLKOUT_FD 0
#define BIT_CLKOUT_FE 7

#define BIT_CTRL_STATUS2_TIE 0
#define BIT_CTRL_STATUS2_AIE 1
#define BIT_CTRL_STATUS2_TF  3
#define BIT_CTRL_STATUS2_AF  4

#define BIT_TIMER_CTRL_TE 7

#define MASK_TIMER_CTRL_TD 0x03
#define MASK_ALARM 0x7f

#define MASK_MIN  0x7f
#define MASK_HOUR 0x3f
#define MASK_MDAY 0x3f
#define MASK_WDAY 0x07
#define MASK_MON  0x1f

#define BV(x) ((uint8_t)(1 << (x)))

extern SemaphoreHandle_t i2c_mux;

static esp_err_t pcf8563_get_time(struct tm *time, bool *valid);
static esp_err_t pcf8563_set_time(struct tm *time);
static uint8_t bcd2dec(uint8_t val);
static uint8_t dec2bcd(uint8_t val);

esp_err_t xiao_round_display_rtc_get_time(struct tm *time)
{
    bool valid = false;
    return pcf8563_get_time(time, &valid);
}

esp_err_t xiao_round_display_rtc_set_time(struct tm *time)
{
    return pcf8563_set_time(time);
}

static esp_err_t pcf8563_get_time(struct tm *time, bool *valid)
{
    esp_err_t ret = ESP_OK;
    uint8_t reg_addr = REG_VL_SECONDS;
    uint8_t data[7];

    if (xSemaphoreTake(i2c_mux, portMAX_DELAY)) {
        ret =  i2c_master_write_read_device(XIAO_I2C_PORT, PCF8563_I2C_ADDR, &reg_addr, 1, data, 7, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
        xSemaphoreGive(i2c_mux);
    }

    if (ret == ESP_OK) {
        *valid = data[0] & BV(BIT_VL) ? false : true;
        time->tm_sec  = bcd2dec(data[0] & ~BV(BIT_VL));
        time->tm_min  = bcd2dec(data[1] & MASK_MIN);
        time->tm_hour = bcd2dec(data[2] & MASK_HOUR);
        time->tm_mday = bcd2dec(data[3] & MASK_MDAY);
        time->tm_wday = bcd2dec(data[4] & MASK_WDAY);
        time->tm_mon  = bcd2dec(data[5] & MASK_MON) - 1;
        time->tm_year = bcd2dec(data[6]) + (data[5] & BV(BIT_YEAR_CENTURY) ? 200 : 100);
    }

    return ret;
}

static esp_err_t pcf8563_set_time(struct tm *time)
{
    esp_err_t ret = ESP_OK;
    bool ovf = time->tm_year >= 200;

    uint8_t data[8] = {
        REG_VL_SECONDS,
        dec2bcd(time->tm_sec),
        dec2bcd(time->tm_min),
        dec2bcd(time->tm_hour),
        dec2bcd(time->tm_mday),
        dec2bcd(time->tm_wday),
        dec2bcd(time->tm_mon + 1) | (ovf ? BV(BIT_YEAR_CENTURY) : 0),
        dec2bcd(time->tm_year - (ovf ? 200 : 100))
    };

    if (xSemaphoreTake(i2c_mux, portMAX_DELAY)) {
        ret =  i2c_master_write_to_device(XIAO_I2C_PORT, PCF8563_I2C_ADDR, data, 8, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
        xSemaphoreGive(i2c_mux);
    }

    return ret;
}

static uint8_t bcd2dec(uint8_t val)
{
    return (val >> 4) * 10 + (val & 0x0f);
}

static uint8_t dec2bcd(uint8_t val)
{
    return ((val / 10) << 4) + (val % 10);
}
