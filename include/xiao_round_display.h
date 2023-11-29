#ifndef XIAO_ROUND_DISPLAY_H
#define XIAO_ROUND_DISPLAY_H
#include "esp_err.h"
/**
 * @file xiao_round_display.h
 * @brief lvgl porting for xiao round display module
 *
 * To use this driver:
 * - Initialize the driver with xiao_round_display_init()
 * - Deinitialize the driver with xiao_round_display_deinit()
 */

/**
 * @brief Initialize the xiao round display driver
 * 
 * @return esp_err_t  ESP_OK on success, otherwise on error.
 */
esp_err_t xiao_round_display_init();

/**
 * @brief Deinitialize the xiao round display driver
 * @return None
 */
void xiao_round_display_deinit();

#endif // XIAO_ROUND_DISPLAY_H