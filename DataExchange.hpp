#ifndef DATAEXCHANGE_HPP
#define DATAEXCHANGE_HPP

#if IS_TEST
#include <LiquidCrystal_I2C.h>        //  Подключаем библиотеку для работы с LCD дисплеем по шине I2C
LiquidCrystal_I2C lcd(0x27, 20, 4);     //  Объявляем  объект библиотеки, указывая параметры дисплея (адрес I2C = 0x27, количество столбцов = 20, количество строк = 4)
#endif

class DataExchange {
public:
  DataExchange(size_t size, uint8_t cnt = 3, size_t uartSpeed = 57600);
  void readMsg(String &str);
  void sendMsg(String &str);
private:
  static constexpr byte BEGIN_MSG_BYTE = 0x10;
  static constexpr byte END_MSG_BYTE = 0x20;

  String msg;           // Буффер для приема и отправки сообщений
  uint8_t msgSize = 0;
  uint8_t cntMsg = 0;   // сколько копий сообщения будет отправляться
};
#endif
