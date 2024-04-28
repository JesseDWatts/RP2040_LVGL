#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include "hardware/gpio.h"
#include <CST816S.h>

/*Don't forget to set Sketchbook location in File/Preferencesto the path of your UI project (the parent foder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

CST816S myTouch(); 
void Touch_INT_callback();
uint8_t touch_flag = 0;



/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
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
    data->state = LV_INDEV_STATE_PR;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
  data->point.x = last_x;           // Set pressed coordinates
  data->point.y = last_y; 
  touch_flag = 0;
}


// void lv_port_indev_init(void) {
//   static lv_indev_drv_t indev_drv;
//   CST816S_init(CST816S_ALL_Mode);
//   DEV_KEY_Config(Touch_INT_PIN);
//   CST816S_Get_Point();
//   touch_flag = 1;
//   Serial.println("TouchChecker");
//   // attachInterrupt(Touch_INT_PIN, &Touch_INT_callback, CHANGE);
// }


void setup() {
  Serial.begin(115200); /* prepare for possible serial debug */
  delay(100);
  Serial.println("STARTING");
  DEV_SET_PWM(100);

  if (DEV_Module_Init() != 0)
    Serial.println("GPIO Init Fail!");
  else
    Serial.println("GPIO Init successful!");
  CST816S_init(CST816S_ALL_Mode);
  DEV_KEY_Config(Touch_INT_PIN);
  attachInterrupt(Touch_INT_PIN, &Touch_INT_callback, RISING);

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println(LVGL_Arduino);
  Serial.println("I am LVGL_Ard uino");

  lv_init();

  tft.begin();        /* TFT init */
  tft.setRotation(0); /* Landscape orientation, flipped */

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  ui_init();

  Serial.println("Setup done");
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}

void Touch_INT_callback(){
  CST816S_Get_Point();
  touch_flag = 1;
}


