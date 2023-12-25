
#ifndef XIAO_SD_CARD_H
#define XIAO_SD_CARD_H
#include "esp_err.h"

/**
 * @brief Initialize the xiao round display SD card
 *
 * @return esp_err_t  ESP_OK on success, otherwise on error.
 */
esp_err_t xiao_round_display_sd_card_init();

/**
 * @brief Deinitialize the xiao round display SD card
 *
 * @return None.
 */
void xiao_round_display_sd_card_deinit();

#endif // XIAO_SD_CARD_H
