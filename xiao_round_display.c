#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "hal/gpio_types.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_gc9a01.h"
#include "esp_lcd_touch_cst816s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "xiao_round_display.h"

// resolution
#define XIAO_HOR_RES_MAX 240
#define XIAO_VIR_RES_MAX 240

// display SPI configuration 
#define XIAO_LCD_PCLK GPIO_NUM_7
#define XIAO_LCD_MOSI GPIO_NUM_9
#define XIAO_LCD_CS GPIO_NUM_2
#define XIAO_LCD_DC GPIO_NUM_4
#define XIAO_LCD_BL GPIO_NUM_43
#define XIAO_LCD_HOST SPI3_HOST
#define XIAO_LCD_CLOCK_SPEED_HZ  (40*1000*1000)
#define XIAO_LCD_BL_ON_LEVEL 1
#define XIAO_BUFFER_SIZE XIAO_HOR_RES_MAX * 80 * sizeof(uint16_t)


#define XIAO_TOUCH_ADDRESS 0x2E
#define XIAO_TOUCH_I2C_SCL GPIO_NUM_6
#define XIAO_TOUCH_I2C_SDA GPIO_NUM_5
#define XIAO_TOUCH_GPIO_INT GPIO_NUM_44
#define XIAO_TOUCH_I2C_NUM 0
#define XIAO_TOUCH_I2C_CLK_HZ 400000

#define XIAO_TOUCH_I2C_CONF()                       \
    {                                               \
        .mode = I2C_MODE_MASTER,                    \
        .sda_io_num = XIAO_TOUCH_I2C_SDA,           \
        .sda_pullup_en = GPIO_PULLUP_ENABLE,        \
        .scl_io_num = XIAO_TOUCH_I2C_SCL,           \
        .scl_pullup_en = GPIO_PULLUP_ENABLE,        \
        .master.clk_speed = XIAO_TOUCH_I2C_CLK_HZ   \
    }

#define XIAO_TOUCHPAD_CONF(TOUCH_CALLBACK)          \
    {                                               \
        .x_max = XIAO_HOR_RES_MAX,                  \
        .y_max = XIAO_VIR_RES_MAX,                  \
        .rst_gpio_num = GPIO_NUM_NC,                \
        .int_gpio_num = XIAO_TOUCH_GPIO_INT,        \
        .levels = {                                 \
            .reset = 0,                             \
            .interrupt = 0,                         \
        },                                          \
        .flags = {                                  \
            .swap_xy = 0,                           \
            .mirror_x = 0,                          \
            .mirror_y = 0,                          \
        },                                          \
        .interrupt_callback = TOUCH_CALLBACK,       \
    }                                               \

#define XIAO_TOUCH_IO_I2C_CST816S_CONFIG()  \
    {                                       \
        .dev_addr = 0x2E,                   \
        .control_phase_bytes = 1,           \
        .dc_bit_offset = 0,                 \
        .lcd_cmd_bits = 8,                  \
        .flags =                            \
        {                                   \
            .disable_control_phase = 1,     \
        }                                   \
    }

// Logging tag
static const char* TAG = "XIAO-ROUND-DISPLAY";

/* LCD IO and panel */
static esp_lcd_panel_io_handle_t lcd_io = NULL;
static esp_lcd_panel_handle_t lcd_panel = NULL;
static esp_lcd_touch_handle_t touch_handle = NULL;

/* LVGL display and touch */
static lv_disp_t *lvgl_disp = NULL;
static lv_indev_t *lvgl_touch_indev = NULL;
static SemaphoreHandle_t touch_mux;
static lv_indev_drv_t indev_drv_tp;

static esp_err_t xiao_lcd_init()
{
    esp_err_t ret = ESP_OK;

    /* LCD initialization */
    ESP_LOGD(TAG, "Initialize SPI bus");
    const spi_bus_config_t buscfg = GC9A01_PANEL_BUS_SPI_CONFIG(
                                        XIAO_LCD_PCLK, 
                                        XIAO_LCD_MOSI, 
                                        XIAO_BUFFER_SIZE
                                    );

    ESP_RETURN_ON_ERROR(
        spi_bus_initialize(XIAO_LCD_HOST, &buscfg, SPI_DMA_CH_AUTO),
        TAG,
        "SPI init failed"
    );

    ESP_LOGD(TAG, "Install panel IO");
    const esp_lcd_panel_io_spi_config_t io_config = GC9A01_PANEL_IO_SPI_CONFIG(
                                                        XIAO_LCD_CS,
                                                        XIAO_LCD_DC,
                                                        NULL,
                                                        NULL
                                                    );
    
    ESP_GOTO_ON_ERROR(
        esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)XIAO_LCD_HOST, &io_config, &lcd_io), 
        err, 
        TAG, 
        "New panel IO failed"
    );

    ESP_LOGD(TAG, "Install GC9A01 driver");

    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
#else
        .rgb_endian = LCD_RGB_ENDIAN_RGB,
#endif
        .bits_per_pixel = 16,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_gc9a01(lcd_io, &panel_config, &lcd_panel), err, TAG, "New panel failed");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(lcd_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(lcd_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(lcd_panel, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(lcd_panel, true, false));
    
    
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
    ESP_ERROR_CHECK(esp_lcd_panel_disp_off(lcd_panel, false));
#else
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(lcd_panel, true));
#endif

    /* LCD backlight on */
    ESP_ERROR_CHECK(gpio_set_level(XIAO_LCD_BL, XIAO_LCD_BL_ON_LEVEL));

    return ret;

err:
    if (lcd_panel) {
        esp_lcd_panel_del(lcd_panel);
    }
    if (lcd_io) {
        esp_lcd_panel_io_del(lcd_io);
    }
    spi_bus_free(XIAO_LCD_HOST);
    return ret;

}

static void touch_callback(esp_lcd_touch_handle_t tp)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(touch_mux, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

static void xiao_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    esp_lcd_touch_handle_t tp = (esp_lcd_touch_handle_t) indev_drv->user_data;
    uint16_t touchpad_x[1] = {0};
    uint16_t touchpad_y[1] = {0};
    uint8_t touchpad_cnt = 0;

    assert(tp);

    /* Read data from touch controller into memory */
    if (xSemaphoreTake(touch_mux, 0) == pdTRUE) {
        esp_lcd_touch_read_data(tp);
    }

    /* Read data from touch controller */
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(tp, touchpad_x, touchpad_y, NULL, &touchpad_cnt, 1);

    if (touchpad_pressed && touchpad_cnt > 0) {
        data->point.x = touchpad_x[0];
        data->point.y = touchpad_y[0];
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static esp_err_t xiao_touch_init()
{
    touch_mux = xSemaphoreCreateBinary();
    /* Initilize I2C */
    const i2c_config_t i2c_conf = XIAO_TOUCH_I2C_CONF();
    ESP_RETURN_ON_ERROR(i2c_param_config(XIAO_TOUCH_I2C_NUM, &i2c_conf), TAG, "I2C configuration failed");
    ESP_RETURN_ON_ERROR(i2c_driver_install(XIAO_TOUCH_I2C_NUM, i2c_conf.mode, 0, 0, 0), TAG, "I2C initialization failed");

    i2c_cmd_handle_t cmd;
    for (int i = 0; i < 0x7f; i++) {
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        if (i2c_master_cmd_begin(XIAO_TOUCH_I2C_NUM, cmd, portMAX_DELAY) == ESP_OK) {
            ESP_LOGW(TAG, "%02X", i);
        }
        i2c_cmd_link_delete(cmd);
    }
    
    /* Initialize touch HW */
    const esp_lcd_touch_config_t tp_cfg = XIAO_TOUCHPAD_CONF(touch_callback);
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    const esp_lcd_panel_io_i2c_config_t tp_io_config = XIAO_TOUCH_IO_I2C_CST816S_CONFIG();

    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)XIAO_TOUCH_I2C_NUM, &tp_io_config, &tp_io_handle), TAG, "");
    return esp_lcd_touch_new_i2c_cst816s(tp_io_handle, &tp_cfg, &touch_handle);
}

static esp_err_t lvgl_init()
{
    /* Initialize LVGL */
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,         /* LVGL task priority */
        .task_stack = 4096,         /* LVGL task stack size */
        .task_affinity = -1,        /* LVGL task pinned to core (-1 is no affinity) */
        .task_max_sleep_ms = 500,   /* Maximum sleep in LVGL task */
        .timer_period_ms = 5        /* LVGL timer tick period in ms */
    };

    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port initialization failed");

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = lcd_io,
        .panel_handle = lcd_panel,
        .buffer_size = XIAO_BUFFER_SIZE,
        .double_buffer = 1,
        .hres = XIAO_HOR_RES_MAX,
        .vres = XIAO_VIR_RES_MAX,
        .monochrome = false,
        /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
        .rotation = {
            .swap_xy = false,
            .mirror_x = true,
            .mirror_y = true,
        },
        .flags = {
            .buff_dma = true,
        }
    };
    lvgl_disp = lvgl_port_add_disp(&disp_cfg);

    /* Register a touchpad input device */
    lv_indev_drv_init(&indev_drv_tp);
    indev_drv_tp.type = LV_INDEV_TYPE_POINTER;
    indev_drv_tp.read_cb = xiao_touchpad_read;
    indev_drv_tp.user_data = touch_handle;
    indev_drv_tp.disp = lvgl_disp;
    lvgl_touch_indev = lv_indev_drv_register(&indev_drv_tp);

    return ESP_OK;
}

esp_err_t xiao_round_display_init()
{
    ESP_RETURN_ON_ERROR(xiao_lcd_init(), TAG, "xiao_lcd_init_failed");
    ESP_RETURN_ON_ERROR(xiao_touch_init(), TAG, "xiao_touch_init_failed");
    ESP_RETURN_ON_ERROR(lvgl_init(), TAG, "lvgl_init_failed");

    return ESP_OK;
}
