/*******************************************************************
    Hello World for the ESP32 Cheap Yellow Display.

    https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display

    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/

    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/

// Make sure to copy the UserSetup.h file into the library as
// per the Github Instructions. The pins are defined in there.

// Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
// Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
// Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
// Note the following larger fonts are primarily numeric only!
// Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
// Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
// Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.

//#include <TFT_eSPI.h>

// A library for interfacing with LCD displays
//
// Can be installed from the library manager (Search for "TFT_eSPI")
//https://github.com/Bodmer/TFT_eSPI


#include <TFT_eSPI.h>
#include <TimeLib.h> // Time Library provides Time and Date conversions

#include <WiFi.h>    // To connect to WiFi
#include <WiFiUdp.h> // To communicate with NTP server
#include <Timezone.h>
#include <NTPClient.h>


//#include <time.h>

TFT_eSPI tft = TFT_eSPI();

// Настройки Wi-Fi
const char* ssid = "GL-GORINICH";       // Замените на имя вашей Wi-Fi сети
const char* password = "09061973"; // Замените на пароль от вашей Wi-Fi сети

// Настройки NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800, 60000); // 10800 — смещение для GMT+3 (Москва)


void setup() {

  Serial.begin(115200 );


  // Инициализация дисплея
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);


  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Подключение к Wi-Fi");
  tft.print("Connect to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.print(".");
  }
  Serial.println();
  Serial.println("Подключено к Wi-Fi!");
  
  tft.fillScreen(TFT_BLACK);

  // Инициализация NTPClient
  timeClient.begin();
  timeClient.update();

  // Установка времени в библиотеку TimeLib
  setTime(timeClient.getEpochTime());



//  setTime(12, 0, 0, 31, 01, 2025); // Установите текущее время: часы, минуты, секунды, день, месяц, год


  delay(500); // Задержка 

  // Отображение календаря
  drawCalendar(month(), year(), day());
}

void loop() {
  // Основной цикл пуст, так как календарь отображается один раз

  // Обновление времени каждую минуту
  timeClient.update();

  // Получение текущего времени
  unsigned long epochTime = timeClient.getEpochTime();
  setTime(epochTime); // Обновление времени в TimeLib

  // Вывод времени на Serial Monitor
  printTime();

  delay(1000); // Задержка 1 секунда

}

void drawCalendar(int month, int year, int currentDay) {
  int dayOfWeek = getFirstDayOfMonth(month, year);
  //int dayOfWeek = 3;
  Serial.print(dayOfWeek); 

  int daysInMonth = daysInMonthFunc(month, year);
 // int firstDayOfMonth = getFirstDayOfMonth(month, year);

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Отображение названия месяца и года
  tft.setCursor(10, 10);
  tft.print(getMonthName(month));
  
  Serial.print(month); 
  
  tft.print(" ");
  tft.print(year);

  // Отображение дней недели
  const char* daysOfWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  for (int i = 0; i < 7; i++) {
    tft.setCursor(10 + i * 45, 40);
    tft.print(daysOfWeek[i]);
  }

  // Отображение дней месяца
  int x = 10;
  int y = 70;
  for (int day = 1; day <= daysInMonth; day++) {
    if (day == currentDay) {
      tft.setTextColor(TFT_BLACK, TFT_YELLOW); // Выделение текущего дня
    } else {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }

//    tft.setCursor(x + (firstDayOfMonth + dayOfWeek) * 30, y);
    tft.setCursor(x + (dayOfWeek) * 45, y);

    tft.print(day);

    dayOfWeek++;
    if (dayOfWeek > 6) {
      dayOfWeek = 0;
      y += 30;
    }
  }
}

int getFirstDayOfMonth(int month, int year) {
  tmElements_t tm;
  tm.Year = CalendarYrToTm(year);
  tm.Month = month;
  tm.Day = 1;
  time_t t = makeTime(tm);
  return weekday(t) - 1; // Возвращает день недели (0 - воскресенье, 6 - суббота)
}

int daysInMonthFunc(int month, int year) {
  if (month == 2) {
    return isLeapYear(year) ? 29 : 28;
  } else if (month == 4 || month == 6 || month == 9 || month == 11) {
    return 30;
  } else {
    return 31;
  }
}

bool isLeapYear(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

const char* getMonthName(int month) {
  const char* months[] = {"January", "February", "March", "April", "May", "June", 
                          "July", "August", "September", "October", "November", "December"};
  return months[month - 1];
}

void printTime() {
  // Получение текущего времени
  int hourNow = hour();
  int minuteNow = minute();
  int secondNow = second();
  int dayNow = day();
  int monthNow = month();
  int yearNow = year();

  int x=200;


  char buffer[10]; // Буфер для хранения форматированной строки
  snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hourNow, minuteNow, secondNow );

  tft.setTextSize(2);
  tft.setTextColor(TFT_RED , TFT_BLACK);
  tft.setCursor(x,10);
  tft.print(buffer);


  // Вывод времени
  // Serial.print("Текущее время: ");
  // Serial.print(hourNow);
  // Serial.print(":");
  // if (minuteNow < 10) Serial.print("0");
  // Serial.print(minuteNow);
  // Serial.print(":");
  // if (secondNow < 10) Serial.print("0");
  // Serial.print(secondNow);
  // Serial.print(" ");
  // Serial.print(dayNow);
  // Serial.print(".");
  // Serial.print(monthNow);
  // Serial.print(".");
  // Serial.println(yearNow);
}