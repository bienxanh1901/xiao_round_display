#ifndef XIAO_BATTERY_H
#define XIAO_BATTERY_H
#include "esp_err.h"

/**
 * @brief Initialize the xiao round display battery ADC
 *
 * @return esp_err_t  ESP_OK on success, otherwise on error.
 */
esp_err_t xiao_round_display_battery_ADC_init();

/**
 * @brief Deinitialize the xiao round display battery ADC
 *
 * @return None.
 */
void xiao_round_display_battery_ADC_deinit();

/**
 * @brief Read the current battery level
 *
 * @return battery level in %.
 */
int32_t xiao_round_display_battery_level_read();

#endif // XIAO_BATTERY_H
