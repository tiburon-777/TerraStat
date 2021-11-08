#include <Arduino.h>

// Подключаем библиотеки:
// Библиотека для работы I2C
#include <Wire.h>
//библиотека для работы со внутренней памятью ардуино
#include <EEPROM.h>
// Библиотека для работы с OLED дисплеем
#include "OzOled.h"
// Библиотека для работы с RTC
#include "iarduino_RTC.h"
// Библиотека для работы со светодиодами NeoPixel
#include "iarduino_NeoPixel.h"
// Библиотека для работы с датчиками DHT22
#include "dhtnew.h"

// Распиновка
struct {
  int pixels = 6;
  int dht = 7;
  struct {
    int up = 2;
    int down = 3;
    int right = 4;
    int left = 5;
  } button;
  int humi = 13;
  int heat = 8;
} pin;

// Инициализируем объекты устройств
// Объявляем объект часов реального времени
iarduino_RTC rtc(RTC_DS3231);
// Объявляем объект LED указывая (№ вывода Arduino к которому подключён модуль NeoPixel, количество используемых светодиодов)
iarduino_NeoPixel led(pin.pixels, 1);
DHTNEW sensor(pin.dht);

//#pragma once

// (Horizontal) byte array of heater icon 16 x 16 px:
const unsigned char bmpHeater[] PROGMEM = {
  0xe0, 0xf8, 0xfe, 0x1c, 0x00, 0xe0, 0xf8, 0xfe, 0x1c, 0x00, 0xe0, 0xf8, 0xfe, 0x1c, 0x00, 0x00, 
  0xe3, 0xff, 0xef, 0xe7, 0xe0, 0xe3, 0xff, 0xef, 0xe7, 0xe0, 0xe3, 0xff, 0xef, 0xe7, 0xe0, 0x00, 
};
// (Horizontal) byte array of humidifier icon 16 x 16 px:
const unsigned char bmpHumidifier[] PROGMEM = { 
  0x00, 0x54, 0x00, 0xaa, 0x00, 0x54, 0x00, 0xaa, 0x00, 0x54, 0x00, 0xaa, 0x00, 0x54, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x02, 0x00, 0x95, 0xc0, 0xea, 0xc0, 0x95, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 
};

struct menuItem {
  int Len;
  char* Text[7];
};

struct Time {
  byte Hours;
  byte Minutes;
};

struct Date {
  byte Day;
  byte Month;
};

// Глобальные переменные
struct {
  // Состояние кнопок
  struct {
    bool Up;
    bool Down;
    bool Left;
    bool Right;
  } Button;
  // Данные сенсоров
  struct {
    float Temp;
    float Humi;
  } Sensor;
  // Состояние исполнительных устройств
   struct {
     bool Heater;
     bool Humidifier;
   } Actor;
  // Пункты меню и положение курсора
  struct {
    bool Show = false;
    int Sect = 0;
    int Pos = 1;
    int PosOld = 1;
    menuItem Item[6] = {
      {6, {">>>>> MENU <<<<<", "TIMES...", "DROUGHTS...", "MONSOONS...", "PANIC LIMS...", "SYSTEM..."}},
      {5, {">>>> TIMES <<<<<", "Date/Time", "Sunrize",  "Sunset",  "Latency (min)"}},
      {6, {">>> DROUGHTS <<<", "Pick date", "Temp. daytime", "Temp. night", "Humi. daytime", "Humi. night"}},
      {6, {">>> MONSOONS <<<", "Pick date", "Temp. daytime", "Temp. night", "Humi. daytime", "Humi. night"}},
      {5, {"> PANIC LIMITS <", "Temp. MAX", "Temp. MIN", "Humi. MAX", "Humi. MIN"}},
      {2, {">>>> SYSTEM <<<<", "Reset"}},      
    };
  } Menu;
  struct {
    struct {
      Time Sunrize;
      Time Sunset;
      byte Latency;
    } Times;
    struct {
      Date Pick;
      byte TempDaytime;
      byte TempNight;
      byte HumiDaytime;
      byte HumiNight;
    } Droughts;
    struct {
      Date Pick;
      byte TempDaytime;
      byte TempNight;
      byte HumiDaytime;
      byte HumiNight;
    } Monsoons;
    struct {
      byte TempMAX;
      byte TempMIN;
      byte HumiMAX;
      byte HumiMIN;
    } Limits;
  } Params;
} state;

void setup() {
  Serial.begin(9600);
  //вспоминаем данные из EEPROM
  EEPROM.get(0, state.Params);
  
  // Объявляем пины кнопок
  pinMode(pin.button.up, INPUT);
  pinMode(pin.button.down, INPUT);
  pinMode(pin.button.right, INPUT);
  pinMode(pin.button.left, INPUT);
  // Объявляем пины акторов
  pinMode(pin.humi, OUTPUT);
  pinMode(pin.heat, OUTPUT);
  
  // Объявляем пины исполнительных устройств
  // Инициализируем адресные светодиоды:
  led.begin();
  //  Выключаем все светодиоды и ждём завершение инициализации и калибровки (задержка 0,3с):
  led.setColor(NeoPixelAll, 254, 0, 0); led.write(); delay(300);
  led.setColor(NeoPixelAll, 0, 254, 0); led.write(); delay(300);
  led.setColor(NeoPixelAll, 0, 0, 254); led.write(); delay(300);
  led.setColor(NeoPixelAll, 0, 0, 0); led.write(); delay(300);
  // Инициализируем дисплей на шине I2C
  OzOled.init();
  OzOled.clearDisplay();
  // Инициализируем часы и устанавливаем время/дату в вформате (сек(0+), мин(0+), час(0+), число(0+), месяц(0+), год(2000+), день недели(0+)).
  rtc.begin();
  // Настройка сенсоров
  sensor.setSuppressError(true);
}

void readButtons();
void readSensors();
void actorLeds();
void screenTime();
void screenMenu();
void screenSetRTC(byte MenuSect, byte MenuPos);
uint16_t screenSetScalar(byte MenuSect, byte MenuPos, uint16_t param, uint16_t limitMIN, uint16_t limitMAX);
Date screenSetDate(byte MenuSect, byte MenuPos, Date date);
Time screenSetTime(byte MenuSect, byte MenuPos, Time time);
void actorLeds();
void OLEDPrintDigits16(uint16_t number, uint8_t signs, uint8_t column, uint8_t line);


// Основной цикл
void loop() {
  // Опрос кнопок
  readButtons();
  // Опрос датчиков
  readSensors();
  // Расчет параметров
  // Отработка исполнительных
  actorLeds();
  
  // Отображение и селектор экранов
  if (state.Menu.Show) {
    screenMenu();
  } else {
    screenTime();
  }
}

// Функция опроса сенсорных кнопок.
void readButtons() {
  state.Button.Up = (digitalRead(pin.button.up) == HIGH);
  state.Button.Down =(digitalRead(pin.button.down) == HIGH);
  state.Button.Right = (digitalRead(pin.button.right) == HIGH);
  state.Button.Left = (digitalRead(pin.button.left) == HIGH);
}

// Функция опроса датчиков температуры и влажности.
void readSensors() {
  if (millis() - sensor.lastRead() > 2000) {
    sensor.setDisableIRQ(false);
    if (sensor.read() != DHTLIB_WAITING_FOR_READ) {
      state.Sensor.Temp = sensor.getTemperature();
      state.Sensor.Humi = sensor.getHumidity();
    }
    sensor.setDisableIRQ(true);
  }
}

// Экран основного режима
void screenTime() {
  // Верхняя строка (текущая температура и влажность)
  OzOled.printNumber16(state.Sensor.Temp, 0, 0); OzOled.printString16("C", 4, 0);
  OzOled.printNumber16(state.Sensor.Humi, 10, 0); OzOled.printString16("%", 14, 0);
  // Мигалка
  if ((millis() / 1000) % 2 == 0) {
    OzOled.printBigNumber(rtc.gettime("H:i"), 0, 2);
    // Состояние нагревателя в верхней строке
    if (state.Actor.Heater) {
      OzOled.drawBitmap(bmpHeater, 6, 0, 2, 2);
    }
    // Состояние увлажнителя в верхней строке
    if (state.Actor.Humidifier) {    
      OzOled.drawBitmap(bmpHumidifier, 8, 0, 2, 2);
    }
  } else {
    OzOled.printBigNumber(rtc.gettime("H i"), 0, 2);
    // Выключаем иконки акторов
    OzOled.printString16(" ", 6, 0);
    OzOled.printString16(" ", 8, 0);
  }
  // Нижняя строка (текущая дата)
  OzOled.printString16(rtc.gettime("d-m-y"), 0, 6);
  // Вызов меню по кнопке RIGHT
  if (state.Button.Right) {
    OzOled.clearDisplay();
    state.Menu.Show = true;
  }
}

// Экран режима меню
void screenMenu() {
  // Debug section
  // Serial.print("MenuSection: "); Serial.print(state.Menu.Sect); Serial.print("   MenuPosition: "); Serial.print(state.Menu.Pos); Serial.println(" ");

  // Отображение заголовка меню
  OzOled.printString(state.Menu.Item[state.Menu.Sect].Text[0], 0, 0);
  // Отображение пунктов меню
  for (int i = 1; i < state.Menu.Item[state.Menu.Sect].Len; i++) {
    OzOled.printString(state.Menu.Item[state.Menu.Sect].Text[i], 2, i);
  }
  // Отображение курсора
  if (state.Menu.PosOld != state.Menu.Pos) {
    OzOled.printString("  ", 0, state.Menu.PosOld);
  }
  OzOled.printString("#>", 0, state.Menu.Pos);
  // Опрос состояния кнопок
  // На позицию вверх
  if (state.Button.Up && state.Menu.Pos > 1 && state.Menu.Pos <= state.Menu.Item[state.Menu.Sect].Len-1) {
    state.Menu.PosOld = state.Menu.Pos; state.Menu.Pos--;
  }
  // На позицию вниз
  if (state.Button.Down && state.Menu.Pos >= 1 && state.Menu.Pos < state.Menu.Item[state.Menu.Sect].Len-1) {
    state.Menu.PosOld = state.Menu.Pos; state.Menu.Pos++;
  }
  // Переходы вглубь меню и запуск сеттеров
  if (state.Button.Right) {
    switch (state.Menu.Sect) {
      case 0:
        state.Menu.Sect = state.Menu.Pos;
        state.Menu.Pos = 1;
        OzOled.clearDisplay();
        break;
      case 1:
        switch (state.Menu.Pos) {
          case 1:
            // Set Date/Time
            screenSetRTC(state.Menu.Sect, state.Menu.Pos);
            break;
          case 2:
          // Set Sunrize
            state.Params.Times.Sunrize = screenSetTime(state.Menu.Sect, state.Menu.Pos, state.Params.Times.Sunrize);
            break;
          case 3:
          // Set Sunset
            state.Params.Times.Sunset = screenSetTime(state.Menu.Sect, state.Menu.Pos, state.Params.Times.Sunset);
            break;
          case 4:
          // Set Latency
            state.Params.Times.Latency = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Times.Latency, 0, 99);
            break;
        }
        break;
      case 2:
        switch (state.Menu.Pos) {
          case 1:
          // Set Pick date
            state.Params.Droughts.Pick = screenSetDate(state.Menu.Sect, state.Menu.Pos, state.Params.Droughts.Pick);
            break;
          case 2:
          // Set Temp. daytime
            state.Params.Droughts.TempDaytime = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Droughts.TempDaytime, state.Params.Limits.TempMIN, state.Params.Limits.TempMAX);
            break;
          case 3:
          // Set Temp. night
            state.Params.Droughts.TempNight = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Droughts.TempNight, state.Params.Limits.TempMIN, state.Params.Limits.TempMAX);
            break;
          case 4:
          // Set Humi. daytime
            state.Params.Droughts.HumiDaytime = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Droughts.HumiDaytime, state.Params.Limits.HumiMIN, state.Params.Limits.HumiMAX);
            break;
          case 5:
          // Set Humi. nights
            state.Params.Droughts.HumiNight = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Droughts.HumiNight, state.Params.Limits.HumiMIN, state.Params.Limits.HumiMAX);
            break;
        }
        break;
      case 3:
        switch (state.Menu.Pos) {
          case 1:
          // Set Pick date
            state.Params.Monsoons.Pick = screenSetDate(state.Menu.Sect, state.Menu.Pos, state.Params.Monsoons.Pick);
            break;
          case 2:
          // Set Temp. daytime
            state.Params.Monsoons.TempDaytime = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Monsoons.TempDaytime, state.Params.Limits.TempMIN, state.Params.Limits.TempMAX);
            break;
          case 3:
          // Set Temp. night
            state.Params.Monsoons.TempNight = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Monsoons.TempNight, state.Params.Limits.TempMIN, state.Params.Limits.TempMAX);
            break;
          case 4:
          // Set Humi. daytime
            state.Params.Monsoons.HumiDaytime = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Monsoons.HumiDaytime, state.Params.Limits.HumiMIN, state.Params.Limits.HumiMAX);
            break;
          case 5:
          // Set Humi. nights
            state.Params.Monsoons.HumiNight = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Monsoons.HumiNight, state.Params.Limits.HumiMIN, state.Params.Limits.HumiMAX);
            break;
        }
        break;
      case 4:
        switch (state.Menu.Pos) {
          case 1:
          // Temp. MAX
            state.Params.Limits.TempMAX = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Limits.TempMAX, state.Params.Limits.TempMIN, 98);
            break;
          case 2:
          // Temp. MIN
            state.Params.Limits.TempMIN = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Limits.TempMIN, 0, state.Params.Limits.TempMAX);
            break;
          case 3:
          // Humi. MAX
            state.Params.Limits.HumiMAX = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Limits.HumiMAX, state.Params.Limits.HumiMIN, 98);
            break;
          case 4:
          // Humi. MIN
            state.Params.Limits.HumiMIN = screenSetScalar(state.Menu.Sect, state.Menu.Pos, state.Params.Limits.HumiMIN, 0, state.Params.Limits.HumiMAX);
            break;
        }
        break;
      case 5:
        switch (state.Menu.Pos) {
            case 1:
            // EEPROM reset
              for (uint16_t i = 0; i < EEPROM.length(); i++) EEPROM.update(i, 0);
              EEPROM.get(0, state.Params);
              OzOled.clearDisplay();
              for (uint8_t i=0;i<5;i++) {
                OzOled.printString16("RESETING", 0, 2);
                OzOled.printString16("EEPROM", 2, 5);
                delay(500);
                OzOled.clearDisplay();
              }
              state.Menu.Show=false;
              break;
        }
        break;
    }
  }
  // Переход вверх по меню, запись в EEPROM и выход
  if (state.Button.Left && state.Menu.Sect>0) {
    state.Menu.Pos=state.Menu.Sect;
    state.Menu.Sect=0;
    OzOled.clearDisplay();
  } else if (state.Button.Left) {
    state.Menu.Show=false;
    EEPROM.put(0, state.Params);
    OzOled.clearDisplay();
  }
}

// Экран установки часов реального времени
void screenSetRTC(byte MenuSect, byte MenuPos) {
  OzOled.clearDisplay();
  rtc.gettime();
  struct dateTime {
    uint16_t digit;
    uint16_t column;
    uint16_t line;
  };
  dateTime t[5] ={
      {rtc.day, 0, 3},
      {rtc.month, 6, 3},
      {rtc.year, 12, 3},
      {rtc.Hours, 3, 6},
      {rtc.minutes, 9, 6},
  };
  uint8_t pos = 0;
  // Отображение заголовка пункта меню
  OzOled.printString(state.Menu.Item[MenuSect].Text[MenuPos], 0, 0);
  while(true){
    // Опрос кнопок
    readButtons();
    // Отображение значения даты и времени
    for (int i = 0; i < 5; i++) {
      OLEDPrintDigits16(t[i].digit, 2, t[i].column, t[i].line);
    }
    OzOled.printString16("-", 4, t[0].line);
    OzOled.printString16("-", 10, t[1].line);
    OzOled.printString16(":", 7, t[3].line);

    // Отображение курсора
    OzOled.printString16("  ", t[pos].column, t[pos].line);
    delay(300);

    // Сдвигаем курсор вправо
    if (state.Button.Right) {
      if (pos <4) {
        pos++;
      } else {
        pos=0;
      }
    }
    
    // Увеличиваем
    if (state.Button.Up && ((pos==0&&t[pos].digit<31)||(pos==1&&t[pos].digit<12)||(pos==2&&t[pos].digit<99)||(pos==3&&t[pos].digit<23)||(pos==4&&t[pos].digit<59))) {
      t[pos].digit++;
    }
    // Уменьшаем
    if (state.Button.Down && ((pos<=1&&t[pos].digit>1)||(pos>1&&t[pos].digit>0))) {
      t[pos].digit--;      
    }
    
    // Сохранить и выйти
    if (state.Button.Left) {
      break;
    } 
  }
  rtc.settime(rtc.seconds, t[4].digit, t[3].digit, t[0].digit, t[1].digit, t[2].digit, rtc.weekday);
}

// Экран установки скалярных значений
uint16_t screenSetScalar(byte MenuSect, byte MenuPos, uint16_t param, uint16_t limitMIN, uint16_t limitMAX) {
  OzOled.clearDisplay();
  if (param>limitMAX) {
    param=limitMAX;
  }
  if (param<limitMIN) {
    param=limitMIN;
  }
  // Отображение заголовка пункта меню
  OzOled.printString(state.Menu.Item[MenuSect].Text[MenuPos], 0, 0);
  while(true){
    // Опрос кнопок
    readButtons();
    // Отображение параметра
    OLEDPrintDigits16(param, 2, 6, 3);
    
    // Увеличиваем
    if (state.Button.Up && param<limitMAX) {
      param++;
    }
    // Уменьшаем
    if (state.Button.Down && param>limitMIN) {
      param--;
    }
    
    // Сохранить и выйти
    if (state.Button.Left) {
      return param;
    } 
  }
}

// Экран установки Даты
Date screenSetDate(byte MenuSect, byte MenuPos, Date date) {
  OzOled.clearDisplay();
  rtc.gettime();
  struct dateMonth {
    uint8_t digit;
    uint8_t column;
    uint8_t line;
  };
  if (date.Day == 0) {
    date.Day=1;
  }
  if (date.Month == 0) {
    date.Month=1;
  }
  dateMonth t[2] ={
    {date.Day, 2, 4},
    {date.Month, 8, 4},
  };
  uint8_t pos = 0;
  // Отображение заголовка пункта меню
  OzOled.printString(state.Menu.Item[MenuSect].Text[MenuPos], 0, 0);
  while(true){
    // Опрос кнопок
    readButtons();
    // Отображение значения
    for (int i = 0; i < 2; i++) {
      OLEDPrintDigits16(t[i].digit, 2, t[i].column, t[i].line);
    }
    OzOled.printString16("-", 6, t[0].line);

    // Отображение курсора
    OzOled.printString16("        ", 0, t[pos].line+2);
    OzOled.printString16("--", t[pos].column, t[pos].line+2);

    // Сдвигаем курсор вправо
    if (state.Button.Right) {
      if (pos==0) {
        pos++;
      } else {
        pos=0;
      }
    }
    
    // Увеличиваем
    if (state.Button.Up && ((pos==0&&t[pos].digit<28)||(pos==1&&t[pos].digit<12))) {
      t[pos].digit++;
    }
    // Уменьшаем
    if (state.Button.Down && t[pos].digit>1) {
      t[pos].digit--;
    }
    
    // Сохранить и выйти
    if (state.Button.Left) {
      break;
    }
  }
  return Date{t[0].digit, t[1].digit};
}


// Экран установки Времени
Time screenSetTime(byte MenuSect, byte MenuPos, Time time) {
  OzOled.clearDisplay();
  rtc.gettime();
  struct timeSlice {
    uint8_t digit;
    uint8_t column;
    uint8_t line;
  };
  timeSlice t[2] ={
    {time.Hours, 2, 4},
    {time.Minutes, 8, 4},
  };
  uint8_t pos = 0;
  // Отображение заголовка пункта меню
  OzOled.printString(state.Menu.Item[MenuSect].Text[MenuPos], 0, 0);
  while(true){
    // Опрос кнопок
    readButtons();
    // Отображение значения
    for (int i = 0; i < 2; i++) {
      OLEDPrintDigits16(t[i].digit, 2, t[i].column, t[i].line);
    }
    OzOled.printString16(":", 6, t[0].line);

    // Отображение курсора
    OzOled.printString16("        ", 0, t[pos].line+2);
    OzOled.printString16("--", t[pos].column, t[pos].line+2);

    // Сдвигаем курсор вправо
    if (state.Button.Right) {
      if (pos==0) {
        pos++;
      } else {
        pos=0;
      }
    }
    
    // Увеличиваем
    if (state.Button.Up && ((pos==0&&t[pos].digit<23)||(pos==1&&t[pos].digit<59))) {
      t[pos].digit++;
    }
    // Уменьшаем
    if (state.Button.Down && t[pos].digit>0) {
      t[pos].digit--;
    }
    
    // Сохранить и выйти
    if (state.Button.Left) {
      break;
    }
  }
  return Time{t[0].digit, t[1].digit};
}

// Отработка светодиодом в зависимости от влажности
void actorLeds() {
  if (state.Sensor.Humi < state.Params.Limits.HumiMIN) {
    led.setColor(0, 100, 0, 0); led.write();
    state.Actor.Humidifier=true;
    digitalWrite(pin.humi, HIGH);
  } else if (state.Sensor.Humi < state.Params.Limits.HumiMAX) {
    led.setColor(0, 0, 3, 0); led.write();
    state.Actor.Humidifier=false;
    digitalWrite(pin.humi, LOW);
  } else {
    led.setColor(0, 20, 10, 0); led.write();
    state.Actor.Humidifier=false;
    digitalWrite(pin.humi, LOW);
  }
}

// Принтер в форматированные цифры
void OLEDPrintDigits16(uint16_t number, uint8_t signs, uint8_t column, uint8_t line) {
  // Определяем кол-во знаков
  int n = String(number).length();
  // Валидируем ввод
  if (signs>=n) {
    for (byte i=0;i<signs-n;i++) {
      OzOled.printNumber16(0, column+i*2, line);
    }
    OzOled.printNumber16(number, column+(signs-n)*2, line);
  } else {
    for (byte i=0;i<signs;i++) {
      OzOled.printString16("#", column+i*2, line);
    }
  }
}
