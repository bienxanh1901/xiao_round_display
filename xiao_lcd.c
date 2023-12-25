#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "esp_lcd_types.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_gc9a01.h"
#include "xiao_common.h"
#include "xiao_lcd.h"

esp_lcd_panel_io_handle_t io = NULL;
esp_lcd_panel_handle_t panel = NULL;

esp_err_t init_lcd()
{
    esp_err_t ret = ESP_OK;

    ESP_LOGD(TAG, "Install panel IO");
    const esp_lcd_panel_io_spi_config_t io_config = GC9A01_PANEL_IO_SPI_CONFIG(
                XIAO_LCD_CS,
                XIAO_LCD_DC,
                NULL,
                NULL
            );
    ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)XIAO_SPI_HOST, &io_config, &io);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "New panel IO failed");

    ESP_LOGD(TAG, "Install GC9A01 driver");

    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
        .rgb_endian = LCD_RGB_ENDIAN_RGB,
        .bits_per_pixel = 16,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_gc9a01(io, &panel_config, &panel), err, TAG, "New panel failed");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel, false));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel, true));

    /* LCD backlight on */
    gpio_set_level(XIAO_LCD_BL, GPIO_LEVEL_HIGH);

    return ret;

err:
    if (panel) {
        esp_lcd_panel_del(panel);
    }

    if (io) {
        esp_lcd_panel_io_del(io);
    }
    return ret;
}
