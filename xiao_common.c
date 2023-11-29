#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "xiao_common.h"


static esp_err_t init_spi_bus();
static esp_err_t init_gpios();

esp_err_t init_common()
{
    ESP_RETURN_ON_ERROR(init_spi_bus(), TAG, "SPI initialization failed");
    ESP_RETURN_ON_ERROR(init_gpios(), TAG, "GPIOs initialization failed");

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