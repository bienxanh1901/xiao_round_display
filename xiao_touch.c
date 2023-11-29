#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2c.h"
#include "esp_lcd_types.h"
#include "esp_lcd_touch.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "xiao_touch_cst816s.h"
#include "xiao_common.h"
#include "xiao_touch.h"

static SemaphoreHandle_t touch_mux;
esp_lcd_touch_handle_t handle;

static void touch_callback(esp_lcd_touch_handle_t tp);

esp_err_t init_touch()
{
    esp_err_t ret = ESP_OK;

    touch_mux = xSemaphoreCreateBinary();

    /* Initilize I2C */
    const i2c_config_t i2c_conf = XIAO_I2C_CONF();
    ret = i2c_param_config(XIAO_TOUCH_I2C_PORT, &i2c_conf);
    ESP_RETURN_ON_ERROR(ret, TAG, "I2C configuration failed");
    ret = i2c_driver_install(XIAO_TOUCH_I2C_PORT, i2c_conf.mode, 0, 0, 0);
    ESP_RETURN_ON_ERROR(ret, TAG, "I2C initialization failed");
    
    /* Initialize touch HW */
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = XIAO_HOR_RES_MAX,
        .y_max = XIAO_VIR_RES_MAX,
        .rst_gpio_num = GPIO_NUM_NC,
        .int_gpio_num = XIAO_TOUCH_GPIO_INT,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
        .interrupt_callback = touch_callback,
        .user_data = &touch_mux
    };
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    const esp_lcd_panel_io_i2c_config_t tp_io_config = XIAO_TOUCH_IO_I2C_CST816S_CONFIG();

    ret = esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)XIAO_TOUCH_I2C_PORT, &tp_io_config, &tp_io_handle);
    ESP_RETURN_ON_ERROR(ret, TAG, "i2c new panel installing failed");
    return xiao_touch_new_i2c_cst816s(tp_io_handle, &tp_cfg, &handle);
}

static void touch_callback(esp_lcd_touch_handle_t tp)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(touch_mux, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}