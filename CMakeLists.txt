idf_component_register(
    SRCS "xiao_round_display.c" 
        "xiao_common.c" 
        "xiao_lcd.c" 
        "xiao_touch.c" 
        "xiao_lvgl.c"
        "xiao_lcd_touch_cst816s.c"
        "xiao_rtc.c"
        "xiao_battery.c"
        "xiao_sd_card.c"
    INCLUDE_DIRS "include"
    REQUIRES "fatfs vfs sdmmc esp_adc"
)
