#ifndef XIAO_COMMON_H
#define XIAO_COMMON_H

#include "esp_err.h"
#include "hal/gpio_types.h"

// GPIO level
#define GPIO_LEVEL_HIGH 0x01
#define GPIO_LEVEL_LOW 0x00

// resolution
#define XIAO_HOR_RES_MAX 240
#define XIAO_VIR_RES_MAX 240

// SPI
#define XIAO_SPI_PCLK GPIO_NUM_7
#define XIAO_SPI_MISO GPIO_NUM_8
#define XIAO_SPI_MOSI GPIO_NUM_9
#define XIAO_SPI_HOST SPI2_HOST
#define XIAO_BUFFER_SIZE XIAO_HOR_RES_MAX * 80 * sizeof(uint16_t)

// I2C
#define XIAO_I2C_SCL GPIO_NUM_6
#define XIAO_I2C_SDA GPIO_NUM_5
#define XIAO_I2C_CLK_HZ 400000
#define XIAO_I2C_PORT 0
#define I2C_MASTER_TIMEOUT_MS       1000

#define XIAO_I2C_CONF()                       \
    {                                               \
        .mode = I2C_MODE_MASTER,                    \
        .sda_io_num = XIAO_I2C_SDA,           \
        .sda_pullup_en = GPIO_PULLUP_ENABLE,        \
        .scl_io_num = XIAO_I2C_SCL,           \
        .scl_pullup_en = GPIO_PULLUP_ENABLE,        \
        .master.clk_speed = XIAO_I2C_CLK_HZ   \
    }                                               \


// LCD
#define XIAO_LCD_CS GPIO_NUM_2
#define XIAO_LCD_DC GPIO_NUM_4
#define XIAO_LCD_BL GPIO_NUM_43
#define XIAO_LCD_CLOCK_SPEED_HZ  (40*1000*1000)

// TOUCH
#define XIAO_TOUCH_GPIO_INT GPIO_NUM_44

// SD CARD
#define XIAO_SD_CS GPIO_NUM_3
#define MOUNT_POINT "/sdcard"

// Battery ADC
#define BATTERY_GPIO GPIO_NUM_1

// Logging tag
#define TAG "XIAO_ROUND_DISPLAY"

esp_err_t init_common();

#endif // XIAO_COMMON_H
