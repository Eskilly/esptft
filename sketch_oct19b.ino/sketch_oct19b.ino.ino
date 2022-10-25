#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>         //wifi库
#include <NTPClient.h>    //NTP库
#include <ArduinoJson.h>  //Json库
#include <HTTPClient.h>   //HTTP库
#include <WiFiUdp.h>
#include "Arduino.h"
#include "Ticker.h"  //定时器库
#include "MyFont.h"  // 自制字体模板库
#include "./Pic/ConnectWifi/Connect.h"
#include "./Pic/Astronaut/As.h"
#include "./Pic/weather/Weather.h"
#include "./Pic/picture/Picture.h"

/*********注意填写自己Wifi的账号密码**********/
const char* ssid = "DESKTOP-RP2SCQP4177";  //Wifi账号
const char* password = "www12345687";      //Wifi密码

const char* host = "api.seniverse.com";  //心知天气服务器地址
//const char* host2 = "api.bilibili.com";  // B站服务器地址
//String UID = "455581263";                // B站用户UID数字号
//int following = 0;                       // 关注数
//int follower = 0;                        // 粉丝数

String now_address = "", now_time = "", now_high_tem = "", now_low_tem = "", now_rainfall = "", now_wind_direction = "", now_wind_scale = "", now_hum = "", now_weather = "";  //用来存储报文得到的字符串
String weekDays[7] = { "周日", "周一", "周二", "周三", "周四", "周五", "周六" };
String weekEnglish[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
String months[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp3.aliyun.com");  // NTP获取时间
Ticker t1;                                       // 获取天气间隔时间
TFT_eSPI tft = TFT_eSPI();                       //设定屏幕
char determineqing[] = "晴";
char determineduoyun[] = "多云";
char determineyin[] = "阴";
char determineyu[] = "雨";
char determinexue[] = "雪";
//unsigned long currentSec;
int i = 0;
int n = 66;
int k = 0;
int ph;
//int flag = 1;
char* now_wea;
int tm_Hour, tm_Minute, monthDay, tm_Month;
String weekDay;
char* week;

/*************Connect Wifi********************/
void get_wifi() {
  WiFi.begin(ssid, password);            // 连接网络
  while (WiFi.status() != WL_CONNECTED)  //等待wifi连接
  {
    delay(300);
    //Serial.print(".");
  }
  //Serial.println("");
  //Serial.println("WiFi connected");  //连接成功
  //Serial.print("IP address: ");      //打印IP地址
  //Serial.println(WiFi.localIP());
  tft.fillScreen(TFT_WHITE);
  tft.pushImage(14 + 50, 65 + 120, 100, 20, ConnectWifi[5]);  //调用图片数据
  tft.setCursor(20 + 50, 30 + 120, 1);                        //设置文字开始坐标(20,30)及1号字体
  tft.setTextSize(1);
  tft.println("WiFi Connected!");
  //delay(200);
}
// void get_Bstation_follow() {
//   //创建TCP连接
//   WiFiClient client;
//   const int httpPort = 80;
//   if (!client.connect(host2, httpPort)) {
//     Serial.println("Connection failed");  //网络请求无响应打印连接失败
//     return;
//   }
//   //URL请求地址
//   String url = "/x/relation/stat?vmid=" + UID + "&jsonp=jsonp";  // B站粉丝数链接，注意修改UID
//   //发送网络请求
//   client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host2 + "\r\n" + "Connection: close\r\n\r\n");
//   delay(2000);
//   //定义answer变量用来存放请求网络服务器后返回的数据
//   String answer;
//   while (client.available()) {
//     String line = client.readStringUntil('\r');
//     answer += line;
//   }
//   //断开服务器连接
//   client.stop();
//   Serial.println();
//   Serial.println("closing connection");
//   //获得json格式的数据

//   String jsonAnswer;
//   int jsonIndex;
//   //找到有用的返回数据位置i 返回头不要
//   for (int i = 0; i < answer.length(); i++) {
//     if (answer[i] == '{') {
//       jsonIndex = i;
//       break;
//     }
//   }
//   jsonAnswer = answer.substring(jsonIndex);
//   Serial.println();
//   Serial.println("JSON answer: ");
//   Serial.println(jsonAnswer);
//   StaticJsonDocument<256> doc;

//   deserializeJson(doc, jsonAnswer);
//   JsonObject data = doc["data"];
//   following = data["following"];  // 59
//   follower = data["follower"];    // 411

//   Serial.println("Following: ");
//   Serial.println(following);
//   Serial.println("follower: ");
//   Serial.println(follower);
// }

/***************Weather******************/
void get_weather() {
  //创建TCP连接
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    //Serial.println("Connection failed");  //网络请求无响应打印连接失败
    return;
  }
  //URL请求地址
  String url = "/v3/weather/daily.json?key=SeAeuMjbRTJo4I_ZN&location=nanchang&language=zh-Hans&unit=c&start=0&days=5";
  //发送网络请求
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  delay(2000);
  //定义answer变量用来存放请求网络服务器后返回的数据
  String answer;
  while (client.available()) {
    String line = client.readStringUntil('\r');
    answer += line;
  }
  //断开服务器连接
  client.stop();
  //Serial.println();
  //Serial.println("closing connection");
  //获得json格式的数据
  String jsonAnswer;
  int jsonIndex;
  //找到有用的返回数据位置i 返回头不要
  for (int i = 0; i < answer.length(); i++) {
    if (answer[i] == '{') {
      jsonIndex = i;
      break;
    }
  }
  jsonAnswer = answer.substring(jsonIndex);
  Serial.println();
  Serial.println("JSON answer: ");
  Serial.println(jsonAnswer);

  StaticJsonDocument<1536> doc;
  deserializeJson(doc, jsonAnswer);

  JsonObject results_0 = doc["results"][0];

  JsonObject results_0_location = results_0["location"];
  const char* results_0_location_name = results_0_location["name"];  // "南昌"
  now_address = results_0_location_name;
  for (JsonObject elem : results_0["daily"].as<JsonArray>()) {
    //const char* date = elem["date"]; // "2021-07-20", "2021-07-21", "2021-07-22"
    const char* text_day = elem["text_day"];  // "中雨", "小雨", "小雨"
    now_weather = text_day;
    const char* high = elem["high"];  // "28", "29", "27"
    now_high_tem = high;
    const char* low = elem["low"];  // "22", "23", "22"
    now_low_tem = low;
    const char* rainfall = elem["rainfall"];  // "19.55", "11.51", "0.42"
    now_rainfall = rainfall;
    //const char* precip = elem["precip"]; // "0.99", "0.94", "0.33"
    const char* wind_direction = elem["wind_direction"];  // "东", "东", "东"
    now_wind_direction = wind_direction;
    //const char* wind_direction_degree = elem["wind_direction_degree"]; // "80", "89", "85"
    //const char* wind_speed = elem["wind_speed"]; // "13.46", "8.64", "10.84"
    const char* wind_scale = elem["wind_scale"];  // "3", "2", "2"
    now_wind_scale = wind_scale;
    const char* humidity = elem["humidity"];  // "100", "100", "98"
    now_hum = humidity;

    break;
  }
  if (strstr(now_weather.c_str(), determineqing) != 0) {
    now_wea = "晴";
    ph = 0;
  }
  if (strstr(now_weather.c_str(), determineduoyun) != 0) {
    now_wea = "多云";
    ph = 1;
  }
  if (strstr(now_weather.c_str(), determineyin) != 0) {
    now_wea = "阴";
    ph = 2;
  }
  if (strstr(now_weather.c_str(), determineyu) != 0) {
    now_wea = "雨";
    ph = 4;
  }
  if (strstr(now_weather.c_str(), determinexue) != 0) {
    now_wea = "雪";
    ph = 7;
  }
}



/**************Setup***********************/
void setup() {
  //Serial.begin(115200);
  //Serial.println("Start");
  tft.init();                           //初始化显示寄存器
  tft.fillScreen(TFT_WHITE);            //屏幕颜色
  tft.setTextColor(TFT_BLACK);          //设置字体颜色黑色
  tft.setCursor(15 + 50, 30 + 120, 1);  //设置文字开始坐标(15,30)及1号字体
  tft.setTextSize(1);

  tft.println("Connecting Wifi...");
  //tft.setRotation(4);//屏幕内容镜像显示或者旋转屏幕0-4  ST7735_Rotation中设置

  tft.setSwapBytes(true);  //使图片颜色由RGB->BGR
  
  for (int j = 0; j < 5; j++) {
    tft.pushImage(14 + 50, 65 + 120, 100, 20, ConnectWifi[j]);  //调用图片数据
    delay(400);
  }
  get_wifi();  // Wifi连接
  get_weather();
  //get_Bstation_follow();
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);  
  timeClient.begin();
  timeClient.setTimeOffset(28800);  //设置偏移时间（以秒为单位）以调整时区
  tft.fillScreen(TFT_BLACK);
}

/*******************Loop*******************/

void loop() {

  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  //Serial.println(epochTime);
  n = touchRead(32);   
  //Serial.println(n);
  if (n == 0) {
    digitalWrite(5, !digitalRead(5));
    delay(50);
  }
  String formattedTime = timeClient.getFormattedTime();
  int tm_Hour = timeClient.getHours();
  int tm_Minute = timeClient.getMinutes();
  int tm_Second = timeClient.getSeconds();
  String weekDay = weekDays[timeClient.getDay()];

  char week[weekDay.length() + 1];
  weekDay.toCharArray(week, weekDay.length() + 1);

  struct tm* ptm = gmtime((time_t*)&epochTime);
  int monthDay = ptm->tm_mday;
  int tm_Month = ptm->tm_mon + 1;
  String currentMonthName = months[tm_Month - 1];
  int tm_Year = ptm->tm_year + 1900;
  String currentDate = String(tm_Month) + "/" + String(monthDay);
  String currentTime, hour, minute;
  if (tm_Hour < 10)
    hour = "0" + String(tm_Hour);
  else
    hour = String(tm_Hour);
  if (tm_Minute < 10)
    minute = "0" + String(tm_Minute);
  else
    minute = String(tm_Minute);
  currentTime = hour + ":" + minute;

  //Serial.print("Cruuent Sec:");
  //Serial.println(currentSec);
  //if (epochTime - currentSec >= 5) {

  //tft.fillScreen(TFT_BLACK);
  show_weather(TFT_WHITE, TFT_BLACK);  // 显示天气界面
  //delay(3000);
  //tft.fillScreen(TFT_BLACK);
  // show_Bstation(TFT_WHITE, TFT_BLACK);  // 显示B站粉丝数界面
  // delay(3000);
  // tft.fillScreen(TFT_BLACK);
  // tft.setSwapBytes(true);  //使图片颜色由RGB->BGR
  // tft.pushImage(4, 4, 120, 120, DID);
  // delay(2000);
  // tft.pushImage(0, 0, 240, 240, pic4);
  // delay(2000);
  // tft.pushImage(0, 0, 240, 240, pic3);
  // delay(2000);
  // tft.pushImage(0, 0, 240, 240, pic2);
  // delay(2000);
  // tft.pushImage(0, 0, 240, 240, pic1);
  // delay(2000);
  //currentSec = timeClient.getEpochTime();
  // tft.fillScreen(TFT_BLACK);
  show_time(TFT_WHITE, TFT_BLACK, Astronaut, currentTime, currentDate, tm_Year, week);  // 显示时间界面

  //delay(50);
}
/*******************时间界面显示****************/
void show_time(uint16_t fg, uint16_t bg, const uint16_t* image[], String currentTime, String currentDate, int tm_Year, const char* week) {
  //tft.fillRect(10, 55,  64, 64, bg);
  tft.setSwapBytes(true);  //使图片颜色由RGB->BGR
  tft.pushImage(10 + 50, 55 + 120, 64, 64, image[i]);
  delay(100);
  i += 1;
  if (i > 8) { i = 0; }
  tft.drawFastHLine(10 + 50, 53 + 120, 108, tft.alphaBlend(0, bg, fg));
  showtext(15 + 50, 5 + 120, 2, 3, fg, bg, currentTime);
  showtext(75 + 50, 60 + 120, 1, 2, fg, bg, String(tm_Year));
  showtext(75 + 50, 80 + 120, 1, 2, fg, bg, currentDate);
  showMyFonts(80 + 50, 100 + 120, week, TFT_YELLOW);
}

// void show_Bstation(uint16_t fg, uint16_t bg) {
//   tft.setSwapBytes(true);  //使图片颜色由RGB->BGR
//   //tft.pushImage(4, 20, 120, 50, pic9);

//   showMyFonts(20, 80, "关注数", fg);
//   showMyFonts(20, 100, "粉丝数", fg);
//   showtext(85, 80, 1, 2, TFT_YELLOW, bg, String(following));
//   showtext(85, 100, 1, 2, TFT_YELLOW, bg, String(follower));
// }
/*******************天气界面显示****************/
void show_weather(uint16_t fg, uint16_t bg) {

  tft.setSwapBytes(true);  //使图片颜色由RGB->BGR
  tft.pushImage(20 + 50, 0, 64, 64, weather[ph]);

  showMyFonts(90 + 50, 20, now_address.c_str(), TFT_WHITE);
  showMyFonts(90 + 50, 40, now_wea, TFT_WHITE);

  tft.pushImage(0 + 50, 65, 30, 30, temIcon);
  tft.pushImage(0 + 50, 95, 30, 30, humIcon);
  tft.pushImage(55 + 50, 65, 30, 30, rainIcon);
  tft.pushImage(55 + 50, 95, 30, 30, windIcon);

  showtext(30 + 50, 75, 1, 1, fg, bg, now_high_tem + "/" + now_low_tem);
  showtext(85 + 50, 75, 1, 1, fg, bg, now_rainfall + "mm");
  showtext(30 + 50, 105, 1, 1, fg, bg, now_hum + "%");
  String now_wind = now_wind_direction;
  showMyFonts(85 + 50, 100, now_wind.c_str(), TFT_WHITE);
}

/*******************整句字符串显示****************/
void showtext(int16_t x, int16_t y, uint8_t font, uint8_t s, uint16_t fg, uint16_t bg, const String str) {
  //设置文本显示坐标，和文本的字体，默认以左上角为参考点，
  tft.setCursor(x, y, font);
  // 设置文本颜色为白色，文本背景黑色
  tft.setTextColor(fg, bg);
  //设置文本大小，文本大小的范围是1-7的整数
  tft.setTextSize(s);
  // 设置显示的文字，注意这里有个换行符 \n 产生的效果
  tft.println(str);
}

/*******************单个汉字显示****************/
void showMyFont(int32_t x, int32_t y, const char c[3], uint32_t color) {
  for (int k = 0; k < 26; k++)  // 根据字库的字数调节循环的次数
    if (hanzi[k].Index[0] == c[0] && hanzi[k].Index[1] == c[1] && hanzi[k].Index[2] == c[2]) {
      tft.drawBitmap(x, y, hanzi[k].hz_Id, hanzi[k].hz_width, 16, color);
    }
}
/*******************整句汉字显示****************/
void showMyFonts(int32_t x, int32_t y, const char str[], uint32_t color) {  //显示整句汉字，字库比较简单，上下、左右输出是在函数内实现
  int x0 = x;
  for (int i = 0; i < strlen(str); i += 3) {
    showMyFont(x0, y, str + i, color);
    x0 += 17;
  }
}