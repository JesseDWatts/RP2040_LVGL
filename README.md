# Arduino Project for Wavshare RP2040 1.28 Round TFT

An example project to show usage of the the GC9A01 (display) and CST816S (touch) on the RP2040 in Arduino.

[Waveshare Round 1.28](https://www.waveshare.com/product/rp2040-touch-lcd-1.28.htm)

[Waveshare Docs](https://www.waveshare.com/wiki/RP2040-Touch-LCD-1.28)

## Get started

If you want to test this code you will need the additional board manager in Arduino: https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.jso which includes "Waveshare RP2040 LCD 1.28".

Clone the repository and load ui.ino on to your controller.

## GC9A01 and TFT_eSPI

The display controller is a GC9A01. This works with Bodmers TFT_eSPI. 
User_Setup.h is the only file that needed changing. Most importantly defining the driver and pins. Do not define TOUCH_CS, the CST816S is not compatible with TFT_eSPI and will need to be setup later.

## CST816S

The touch controller is a CST816S. Most libraries I found are specifically for the ESP. Waveshares demo code included the libraries I have used.
**Initialise:**

```
#include <CST816S.h>
CST816S myTouch();
void Touch_INT_callback();
uint8_t touch_flag = 0;
```

**In your Setup():**

```
  CST816S_init(CST816S_ALL_Mode);
  DEV_KEY_Config(Touch_INT_PIN);
  attachInterrupt(Touch_INT_PIN, &Touch_INT_callback, RISING);
```

**What happens when touch is Registered**
```
void Touch_INT_callback(){
  CST816S_Get_Point();
  touch_flag = 1;
}
```
**Reading the Touchpad**
```
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
  const uint CST816S_MAX_X = 240;
  const uint CST816S_MAX_Y = 240;

  uint last_x = 0;
  uint last_y = 0;

  // Get touch coordinates
  last_x = Touch_CTS816.x_point;
  last_y = Touch_CTS816.y_point;

  // Check if touch coordinates are within a reasonable range
  if (touch_flag == 1) {
    data->state = LV_INDEV_STATE_PR; // Set state to pressed
    data->point.x = last_x;  // Set pressed coordinates
    data->point.y = last_y;  

  } else {
    data->state = LV_INDEV_STATE_REL; // Set state to released
  }
  data->point.x = last_x;           // Set pressed coordinates
  data->point.y = last_y; 
  touch_flag = 0;
}
```
