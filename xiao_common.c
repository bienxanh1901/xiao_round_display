#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "xiao_common.h"

SemaphoreHandle_t i2c_mux =  NULL;

static esp_err_t init_spi_bus();
static esp_err_t init_gpios();
static esp_err_t init_i2c();

esp_err_t init_common()
{
    ESP_RETURN_ON_ERROR(init_spi_bus(), TAG, "SPI initialization failed");
    ESP_RETURN_ON_ERROR(init_gpios(), TAG, "GPIOs initialization failed");
    ESP_RETURN_ON_ERROR(init_i2c(), TAG, "I2C initialization failed");

    return ESP_OK;
}


static esp_err_t init_spi_bus()
{
    esp_err_t ret = ESP_OK;

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = XIAO_SPI_MOSI,
        .miso_io_num = XIAO_SPI_MISO,
        .sclk_io_num = XIAO_SPI_PCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = XIAO_BUFFER_SIZE,
    };

    ret = spi_bus_initialize(XIAO_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize spi bus.");
    }

    return ret;
}

static esp_err_t init_gpios()
{
    const gpio_config_t lcd_cs_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = BIT64(XIAO_LCD_CS)
    };

    const gpio_config_t lcd_bl_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = BIT64(XIAO_LCD_BL)
    };

    ESP_RETURN_ON_ERROR(gpio_config(&lcd_cs_cfg), TAG, "Failed to config LCD CS GPIO.");
    ESP_RETURN_ON_ERROR(gpio_config(&lcd_bl_cfg), TAG, "Failed to config LCD BL GPIO.");

    return ESP_OK;
}

static esp_err_t init_i2c()
{
    esp_err_t ret = ESP_OK;

    i2c_mux = xSemaphoreCreateMutex();

    if (!i2c_mux) {
        ESP_LOGE(TAG, "Could not create i2c mutex");
        return ESP_FAIL;
    }

    const i2c_config_t i2c_conf = XIAO_I2C_CONF();

    ret = i2c_param_config(XIAO_I2C_PORT, &i2c_conf);
    if (ret == ESP_OK) {
        ret = i2c_driver_install(XIAO_I2C_PORT, i2c_conf.mode, 0, 0, 0);
    } else {
        ESP_LOGE(TAG, "I2C configuration failed");
    }

    return ret;
}
