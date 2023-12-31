#ifndef XIAO_RTC_H
#define XIAO_RTC_H
#include "esp_err.h"
#include <time.h>

/**
 * @brief Read the current rtc time
 *
 * @return esp_err_t  ESP_OK on success, otherwise on error.
 */
esp_err_t xiao_round_display_rtc_get_time(struct tm *time);

/**
 * @brief Set the time for rtc
 *
 * @return esp_err_t  ESP_OK on success, otherwise on error.
 */
esp_err_t xiao_round_display_rtc_set_time(struct tm *time);

#endif // XIAO_RTC_H
