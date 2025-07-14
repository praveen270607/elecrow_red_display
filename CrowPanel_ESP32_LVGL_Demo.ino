/**************************CrowPanel ESP32 HMI Display Example Code************************
Version     :	1.1
Suitable for:	CrowPanel ESP32 HMI Display
Product link:	https://www.elecrow.com/esp32-display-series-hmi-touch-screen.html
Code	  link:	https://github.com/Elecrow-RD/CrowPanel-ESP32-Display-Course-File
Lesson	link:	https://www.youtube.com/watch?v=WHfPH-Kr9XU
Description	:	The code is currently available based on the course on YouTube, 
				        if you have any questions, please refer to the course video: Introduction 
				        to ask questions or feedback.
**************************************************************/
#include <WiFi.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
/**************************************************************/
#include <TFT_eSPI.h>
/**************************TFT_eSPI************************
If you don't know how to configure TFT_eSPI library for the display and touch driver
Please refer to the content of the fifth lesson
**************************************************************/
#include <Arduino.h>
#include <SPI.h>

/**************************LVGL and UI************************
if you want to use the LVGL demo. you need to include <demos/lv_demos.h> and <examples/lv_examples.h>. 
if not, please do not include it. It will waste your Flash space.
**************************************************************/
#include <lvgl.h>
#include "ui.h"
// #include <demos/lv_demos.h>
// #include <examples/lv_examples.h>
/**************************LVGL and UI END************************/

/*******************************************************************************
 * Please define the corresponding macros based on the board you have purchased.
 * CrowPanel_24 means CrowPanel ESP32 HMI 2.4inch Board
 * CrowPanel_28 means CrowPanel ESP32 HMI 2.8inch Board
 * CrowPanel_35 means CrowPanel ESP32 HMI 3.5inch Board
 ******************************************************************************/
//#define CrowPanel_24
//#define CrowPanel_28
#define CrowPanel_35

#if defined (CrowPanel_35)
/*screen resolution*/
static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 320;
uint16_t calData[5] = { 353, 3568, 269, 3491, 7  };     /*touch caldata*/

#elif defined (CrowPanel_24)
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;
uint16_t calData[5] = { 557, 3263, 369, 3493, 3  };

#elif defined (CrowPanel_28)
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;
uint16_t calData[5] = { 189, 3416, 359, 3439, 1 };
#endif


TFT_eSPI lcd = TFT_eSPI(); /* TFT entity */

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[ screenWidth * screenHeight / 13 ];

/**************************************************************/
// --- Wi-Fi + MQTT Config ---
/**************************************************************/
// const char* ssid = "colocation";
// const char* password = "colocation";
const char* ssid = "colocation";
const char* password = "colocation";
const char broker[] = "collocationstudy.kaatru.org";
//const char broker[] = "colocation.kaatru.org";
int port = 1883;
const char topic[] = "dev/SIN11";
//const char topic[] = "test";

/**************************************************************/
// --- MQTT & WiFi Clients ---
/**************************************************************/
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
#define JSON_BUFFER_SIZE 512


//_______________________
/* display flash */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
  uint32_t w = ( area->x2 - area->x1 + 1 );
  uint32_t h = ( area->y2 - area->y1 + 1 );

  lcd.startWrite();
  lcd.setAddrWindow( area->x1, area->y1, w, h );
  lcd.pushColors( ( uint16_t * )&color_p->full, w * h, true );
  lcd.endWrite();

  lv_disp_flush_ready( disp );
}

uint16_t touchX, touchY;
/*touch read*/
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{

  bool touched = lcd.getTouch( &touchX, &touchY, 600);
  if ( !touched )
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*set location*/
    data->point.x = touchX;
    data->point.y = touchY;

    //Serial.print( "Data x " );
    //Serial.println( touchX );

    //Serial.print( "Data y " );
    //Serial.println( touchY );
  }
}

/**************************************************************/
// --- MQTT Message Handler ---
/**************************************************************/
void onMqttMessage(int messageSize) {
  //Serial.println("Hello came inside the function");
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  DeserializationError err = deserializeJson(doc, mqttClient);
  if (err) {
    Serial.print("JSON parse error: ");
    Serial.println(err.f_str());
    return;
  }

  if (doc.containsKey("sPM1")) {
    float val = doc["sPM1"].as<float>();
    Serial.println(val);
    lv_arc_set_value(ui_pm1Arc, (int)val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", val);
    //lv_label_set_text(ui_LabelPM, buf);
    lv_label_set_text(ui_pm1Val, buf);
    
  }

  if (doc.containsKey("sPM2")) {
    float val = doc["sPM2"].as<float>();
    Serial.println(val);
    lv_arc_set_value(ui_pm25Arc, (int)val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", val);
    //lv_label_set_text(ui_LabelPM, buf);
    lv_label_set_text(ui_pm25Val, buf);
    //lv_chart_set_next_value(ui_Chart2, ui_Chart2_series_1, (int)val);
  }

  if (doc.containsKey("sPM4")) {
    float val = doc["sPM4"].as<float>();
    Serial.println(val);
    lv_arc_set_value(ui_pm4Arc, (int)val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", val);
    //lv_label_set_text(ui_LabelPM, buf);
    lv_label_set_text(ui_pm4Val, buf);
    //lv_chart_set_next_value(ui_Chart2, ui_Chart2_series_2, (int)val);
  }

   if (doc.containsKey("sPM10")) {
    float val = doc["sPM10"].as<float>();
    Serial.println(val);
    //lv_arc_set_value(ui_ArcPM, (int)val);
    lv_arc_set_value(ui_pm10Arc, (int)val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", val);
    lv_label_set_text(ui_pm10Val, buf);
    //lv_label_set_text(ui_LabelPM, buf);
    //lv_chart_set_next_value(ui_Chart2, ui_Chart2_series_3, (int)val);
  }

  if (doc.containsKey("sTemp")) {
    float val = doc["sTemp"].as<float>();
    Serial.println(val);
    lv_arc_set_value(ui_tempArc, (int)val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", val);
    //lv_label_set_text(ui_LabelTemp, buf);
    lv_label_set_text(ui_tempVal, buf);
    //lv_label_set_text(ui_tempVal2, buf);
  }

  if (doc.containsKey("k30Co2")) {
    float val = doc["k30Co2"].as<float>();
    Serial.println(val);
    //lv_arc_set_value(ui_ArcRH, (int)val);
    lv_arc_set_value(ui_Co2Arc, (int)val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", val);
    //lv_label_set_text(ui_LabelRH, buf);
    lv_label_set_text(ui_Co2Val, buf);
  }

   if (doc.containsKey("sVocI")) {
    float val = doc["sVocI"].as<float>();
    Serial.println(val);
    //lv_arc_set_value(ui_ArcRH, (int)val);
    lv_arc_set_value(ui_Arc1, (int)val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", val);
    //lv_label_set_text(ui_LabelRH, buf);
    lv_label_set_text(ui_VOC_Value, buf);
  }

  if (doc.containsKey("co_ppb")) {
    float val = doc["co_ppb"].as<float>();
    Serial.println(val);
    //lv_arc_set_value(ui_ArcRH, (int)val);
    lv_arc_set_value(ui_CoArc, (int)val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", val);
    //lv_label_set_text(ui_LabelRH, buf);
    lv_label_set_text(ui_CoVal, buf);
  }

  if (doc.containsKey("so2_ppb")) {
    float val = doc["so2_ppb"].as<float>();
    Serial.println(val);
    //lv_arc_set_value(ui_ArcRH, (int)val);
    lv_arc_set_value(ui_So2Arc, (int)val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", val);
    //lv_label_set_text(ui_LabelRH, buf);
    lv_label_set_text(ui_So2Val, buf);
  }

  if (doc.containsKey("no2_ppb")) {
    float val = doc["no2_ppb"].as<float>();
    Serial.println(val);
    //lv_arc_set_value(ui_ArcRH, (int)val);
    lv_arc_set_value(ui_No2Arc, (int)val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", val);
    //lv_label_set_text(ui_LabelRH, buf);
    lv_label_set_text(ui_No2Val, buf);
  }

  if (doc.containsKey("o3_ppb")) {
    float val = doc["o3_ppb"].as<float>();
    Serial.println(val);
    //lv_arc_set_value(ui_ArcRH, (int)val);
    lv_arc_set_value(ui_O3Arc, (int)val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", val);
    //lv_label_set_text(ui_LabelRH, buf);
    lv_label_set_text(ui_O3Val, buf);
  }

   if (doc.containsKey("rh")) {
    float val = doc["rh"].as<float>();
    lv_arc_set_value(ui_humArc, (int)val);
    Serial.println(val);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", val);
    //lv_label_set_text(ui_LabelRH, buf);
    lv_label_set_text(ui_HumVal, buf);
    //lv_label_set_text(ui_humidityVal2, buf);
  }

  //Serial.println("Just before the chart function");

  //lv_chart_refresh(ui_Chart2);
  //Serial.println("came to end of the message fucntion");

  if (doc.containsKey("dTS")) {
    unsigned long epochSeconds = doc["dTS"].as<unsigned long>();
    struct tm* timeinfo = localtime((time_t*)&epochSeconds);
    if (timeinfo) {
      char timeBuf[16];
      strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", timeinfo);
      lv_label_set_text(ui_timeVal, timeBuf);
      lv_label_set_text(ui_timeVal2, timeBuf);
    }
  }
}


void setup()
{
  Serial.begin( 9600 ); /*serial init */

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  mqttClient.onMessage(onMqttMessage);
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1)
      ;
  }
  Serial.println("Connected to MQTT broker");

  mqttClient.subscribe(topic);
  Serial.print("Subscribed to topic: ");
  Serial.println(topic);

  //Port_D
  pinMode(25, OUTPUT);
  digitalWrite(25, LOW);

  //LCD init
  lcd.begin();          
  lcd.setRotation(1); 
  lcd.fillScreen(TFT_BLACK);
  lcd.setTouch(calData);
  delay(100);
  //background light pin
  pinMode(27, OUTPUT);
  digitalWrite(27, HIGH);

  //lvgl init
  lv_init();
  
  lv_disp_draw_buf_init( &draw_buf, buf1, NULL, screenWidth * screenHeight / 13 );

  /* Initialize the display */
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init( &disp_drv );
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register( &disp_drv );

  /* Initialize the (dummy) input device driver */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init( &indev_drv );
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register( &indev_drv );

  lcd.fillScreen(TFT_BLACK);

  //lv_demo_widgets();    // LVGL demo
  ui_init();

  //Serial.println( "Setup done" );
}


void loop()
{
  mqttClient.poll();
  lv_timer_handler();
  delay(5);
}
