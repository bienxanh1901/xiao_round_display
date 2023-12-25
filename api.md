# API Reference

## Header files

- [include/xiao_battery.h](#file-includexiao_batteryh)
- [include/xiao_round_display.h](#file-includexiao_round_displayh)
- [include/xiao_rtc.h](#file-includexiao_rtch)
- [include/xiao_sd_card.h](#file-includexiao_sd_cardh)

## File include/xiao_battery.h






## Functions

| Type | Name |
| ---: | :--- |
|  void | [**xiao\_round\_display\_battery\_ADC\_deinit**](#function-xiao_round_display_battery_adc_deinit) () <br>_Deinitialize the xiao round display battery ADC._ |
|  esp\_err\_t | [**xiao\_round\_display\_battery\_ADC\_init**](#function-xiao_round_display_battery_adc_init) () <br>_Initialize the xiao round display battery ADC._ |
|  int32\_t | [**xiao\_round\_display\_battery\_level\_read**](#function-xiao_round_display_battery_level_read) () <br>_Read the current battery level._ |



## Functions Documentation

### function `xiao_round_display_battery_ADC_deinit`

_Deinitialize the xiao round display battery ADC._
```c
void xiao_round_display_battery_ADC_deinit () 
```


**Returns:**

None.
### function `xiao_round_display_battery_ADC_init`

_Initialize the xiao round display battery ADC._
```c
esp_err_t xiao_round_display_battery_ADC_init () 
```


**Returns:**

esp\_err\_t ESP\_OK on success, otherwise on error.
### function `xiao_round_display_battery_level_read`

_Read the current battery level._
```c
int32_t xiao_round_display_battery_level_read () 
```


**Returns:**

battery level in %.


## File include/xiao_round_display.h

_lvgl porting for xiao round display module_

To use this driver:

* Initialize the driver with [**xiao\_round\_display\_init()**](#function-xiao_round_display_init)
* Deinitialize the driver with [**xiao\_round\_display\_deinit()**](#function-xiao_round_display_deinit)


## Functions

| Type | Name |
| ---: | :--- |
|  void | [**xiao\_round\_display\_deinit**](#function-xiao_round_display_deinit) () <br>_Deinitialize the xiao round display driver._ |
|  esp\_err\_t | [**xiao\_round\_display\_init**](#function-xiao_round_display_init) () <br>_Initialize the xiao round display driver._ |



## Functions Documentation

### function `xiao_round_display_deinit`

_Deinitialize the xiao round display driver._
```c
void xiao_round_display_deinit () 
```


**Returns:**

None
### function `xiao_round_display_init`

_Initialize the xiao round display driver._
```c
esp_err_t xiao_round_display_init () 
```


**Returns:**

esp\_err\_t ESP\_OK on success, otherwise on error.


## File include/xiao_rtc.h






## Functions

| Type | Name |
| ---: | :--- |
|  esp\_err\_t | [**xiao\_round\_display\_rtc\_get\_time**](#function-xiao_round_display_rtc_get_time) (struct tm \*time) <br>_Read the current rtc time._ |
|  esp\_err\_t | [**xiao\_round\_display\_rtc\_set\_time**](#function-xiao_round_display_rtc_set_time) (struct tm \*time) <br>_Set the time for rtc._ |



## Functions Documentation

### function `xiao_round_display_rtc_get_time`

_Read the current rtc time._
```c
esp_err_t xiao_round_display_rtc_get_time (
    struct tm *time
) 
```


**Returns:**

esp\_err\_t ESP\_OK on success, otherwise on error.
### function `xiao_round_display_rtc_set_time`

_Set the time for rtc._
```c
esp_err_t xiao_round_display_rtc_set_time (
    struct tm *time
) 
```


**Returns:**

esp\_err\_t ESP\_OK on success, otherwise on error.


## File include/xiao_sd_card.h






## Functions

| Type | Name |
| ---: | :--- |
|  void | [**xiao\_round\_display\_sd\_card\_deinit**](#function-xiao_round_display_sd_card_deinit) () <br>_Deinitialize the xiao round display SD card._ |
|  esp\_err\_t | [**xiao\_round\_display\_sd\_card\_init**](#function-xiao_round_display_sd_card_init) () <br>_Initialize the xiao round display SD card._ |



## Functions Documentation

### function `xiao_round_display_sd_card_deinit`

_Deinitialize the xiao round display SD card._
```c
void xiao_round_display_sd_card_deinit () 
```


**Returns:**

None.
### function `xiao_round_display_sd_card_init`

_Initialize the xiao round display SD card._
```c
esp_err_t xiao_round_display_sd_card_init () 
```


**Returns:**

esp\_err\_t ESP\_OK on success, otherwise on error.


