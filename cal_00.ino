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
#include <HTTPClient.h>
#include <ArduinoJson.h>



//#include <time.h>

TFT_eSPI tft = TFT_eSPI();

// Настройки Wi-Fi
const char* ssid = "SSDI";       // Замените на имя вашей Wi-Fi сети
const char* password = "PASSWD"; // Замените на пароль от вашей Wi-Fi сети

// Настройки NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800, 60000); // 10800 — смещение для GMT+3 (Москва)


// Настройки OpenWeatherMap
const String apiKey = "715eb8e0b9bdd106c145969ca601feb6"; // Ваш API-ключ от OpenWeatherMap
const String city = "Moscow";         // Город, для которого нужно получить погоду
const String countryCode = "RU";      // Код страны (опционально)


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

  //  Погода
  getWeather();

}


unsigned long tick=0; // отслеживание тиков

void loop() {
  // Основной цикл 
  tick++;


  // Получение погоды
  if (tick%600 == 0) {
    getWeather();
  }
   

  // отображение полного календаря каждые 60 тиков
  if (tick%600 == 0) {
    int oldDay = day();
    int oldYear = year();
    int oldMoth = month();

   // Serial.println("Update calendar");
    timeClient.update();
    setTime(timeClient.getEpochTime());
    if (oldDay!=day() || oldYear!=year() || oldMoth!=month()) { // если изменился день или месяц или год надо перерисовать календарь
      tft.fillScreen(TFT_BLACK);
      drawCalendar(month(), year(), day());
    }
  }


 // Serial.print(tick%10);

  // Получение текущего времени каждые 10 тиков
  if (tick%100 == 0) {
 //   Serial.println("Update time");
    setTime(timeClient.getEpochTime());
  }

  // вывод времени каждый тик
  if (tick%10 == 0) {
    printTime();
  }


  delay(100); // общая задержка 1 секунда

}

void drawCalendar(int month, int year, int currentDay) {
  int dayOfWeek = getFirstDayOfMonth(month, year);
  //int dayOfWeek = 3;
  /*
  Serial.print("Year="); 
  Serial.println(year); 
  Serial.print("Month="); 
  Serial.println(month); 
  Serial.print("dayOfWeek="); 
  Serial.println(dayOfWeek); 
  */

  int daysInMonth = daysInMonthFunc(month, year);
  //Serial.print("daysInMonth="); 
 // Serial.println(daysInMonth); 
 // int firstDayOfMonth = getFirstDayOfMonth(month, year);

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Отображение названия месяца и года
  tft.setCursor(10, 10);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.print(getMonthName(month));
  
  //Serial.print(month); 
  
  tft.print(" ");
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.print(year);

  // Отображение дней недели
  const char* daysOfWeek[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};  //сместил так как неделя с понедельника
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
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
    }
    else {
      if (dayOfWeek == 5 || dayOfWeek==6)
          tft.setTextColor(TFT_ORANGE, TFT_BLACK);  // если суббота воскресенье
      else
          tft.setTextColor(TFT_WHITE, TFT_BLACK);  // если обычный день
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

  // Использование алгоритма Зеллера
  
    if (month < 3) {
      month += 12;
      year--;
    }
    int q = 1; // первый день
    int m = month;
    int K = year % 100;
    int J = year / 100;
    //int h = (q + (13 * (m + 1)) / 5 + K + (K / 4) + (J / 4) + 5 * J) % 7; // 0 = суббота, 1 = воскресенье, 2 = понедельник, ..., 6 = пятница
    int h = (q + (13 * (m + 1)) / 5 + K + (K / 4) + (J / 4) + 5 * J + 5) % 7; // 0 = понедельник, ..., 6 = вовкрксенье
    return h;
  
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


  tft.drawRect(180,0,135,35,TFT_DARKGREY);

  tft.setTextSize(2);
  tft.setTextColor(TFT_RED , TFT_BLACK);
  tft.setCursor(x,10);
  tft.print(buffer);


}


void getWeather() {
  // Формируем URL для запроса
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric&lang=en";

//  String ss;


  // Отправляем HTTP-запрос
  HTTPClient http;
  Serial.println("Запрос ");
  Serial.println(url);

  http.begin(url);
  int httpCode = http.GET();
  delay(200);

  // Проверяем код ответа
  if (httpCode == HTTP_CODE_OK) {
    // Получаем JSON-ответ
    String payload = http.getString();
    Serial.println("Ответ от сервера:");
    Serial.println(payload);

    // Парсим JSON
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    // Извлекаем данные о погоде
    String weatherDescription = doc["weather"][0]["description"].as<String>();
    float temperature = doc["main"]["temp"].as<float>();
    float humidity = doc["main"]["humidity"].as<float>();

    // Выводим данные в Serial Monitor
    Serial.println("Погода в " + city + ":");
    Serial.println("Описание: " + weatherDescription);
    Serial.println("Температура: " + String(temperature) + " °C");
    Serial.println("Влажность: " + String(humidity) + " %");

   // ss = " "+ weatherDescription + " " + temperature + "C " + humidity +"%" ;
    // вывод данных
    tft.setTextSize(1);
    tft.setTextColor(TFT_GREEN , TFT_BLACK);
    tft.setCursor(5,225);
    tft.print(weatherDescription);
    tft.print(" ");
    
    tft.setCursor(130,220);
    tft.setTextSize(2);
    tft.setTextColor(TFT_SKYBLUE , TFT_BLACK);
    tft.print(temperature);
    tft.setTextColor(TFT_GREEN , TFT_BLACK);
    tft.print("C   ");
    tft.setTextColor(TFT_SKYBLUE , TFT_BLACK);
    tft.print(humidity);
    tft.setTextColor(TFT_GREEN , TFT_BLACK);
    tft.print("%");

    //tft.print(ss);




  } else {
    Serial.println("Ошибка при запросе: " + String(httpCode));
    
    tft.setTextSize(1);
    tft.setTextColor(TFT_RED , TFT_BLACK);
    tft.setCursor(5,225);
    tft.print(" - no data  -    ");
  }

  // Закрываем соединение
  http.end();

}
