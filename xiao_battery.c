#include "esp_log.h"
#include "esp_check.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "xiao_common.h"
#include "xiao_battery.h"

/* battery ADC */
adc_oneshot_unit_handle_t battery_handle;
adc_cali_handle_t cali_handle = NULL;
adc_unit_t unit_id;
adc_channel_t channel;

static esp_err_t init_adc_calibration();
static void deinit_adc_calibration();


esp_err_t xiao_round_display_battery_ADC_init()
{
    esp_err_t ret = ESP_OK;

    ret = adc_oneshot_io_to_channel(BATTERY_GPIO, &unit_id, &channel);
    ESP_RETURN_ON_ERROR(ret, TAG, "adc oneshot failed.");
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = unit_id,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
            .atten = ADC_ATTEN_DB_11,
    };

    ret = adc_oneshot_new_unit(&init_config, &battery_handle);
    ESP_RETURN_ON_ERROR(ret, TAG, "adc oneshot unit initialization failed.");
    ret = adc_oneshot_config_channel(battery_handle, channel, &config);
    ESP_RETURN_ON_ERROR(ret, TAG, "adc oneshot channel configuration failed.");
    ret = init_adc_calibration();
    ESP_RETURN_ON_ERROR(ret, TAG, "adc oneshot calibration inittialization failed.");
    return ret;
}

void xiao_round_display_battery_ADC_deinit()
{
    ESP_ERROR_CHECK(adc_oneshot_del_unit(battery_handle));
    deinit_adc_calibration();
}

int32_t xiao_round_display_battery_level_read()
{   
    int voltage = 0;
    int level = 0;
    adc_oneshot_get_calibrated_result(battery_handle, cali_handle, channel, &voltage);

    level = (voltage - 1850) * 100 / 250; // 1850 ~ 2100
    level = (level < 0) ? 0 : ((level > 100) ? 100 : level); 
    ESP_LOGI(TAG, "Cali level: %d", level);
    return level;
}

static esp_err_t init_adc_calibration()
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit_id,
            .chan = channel,
            .atten = ADC_ATTEN_DB_11,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = ADC_ATTEN_DB_11,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    cali_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return ret;
}

static void deinit_adc_calibration()
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(cali_handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(cali_handle));
#endif
}