#include "DataExchange.hpp"

#if IS_TEST
#include <LiquidCrystal_I2C.h>        //  Подключаем библиотеку для работы с LCD дисплеем по шине I2C
LiquidCrystal_I2C lcd(0x27, 20, 4);     //  Объявляем  объект библиотеки, указывая параметры дисплея (адрес I2C = 0x27, количество столбцов = 20, количество строк = 4)
#endif

DataExchange::DataExchange(size_t size_read, size_t size_send, uint8_t cnt = 3, size_t uartSpeed = 57600){
  #if IS_TEST
  lcd.init();                       //  Инициируем работу с LCD дисплеем
  lcd.backlight();                  //  Включаем подсветку LCD дисплея
  #endif
  
  cntMsg = cnt;
  Serial.begin(uartSpeed);

  msg_read.reserve(size_read + 3);
  msg_send.reserve(size_send + 3);

  for (size_t i = 0; i < size_read + 3; i++)
    msg_read += '0';

  for (size_t i = 0; i < size_send + 3; i++)
    msg_send += '0';
}

void DataExchange::readMsg(String &str){
  while (Serial.available()) {
    for (size_t i = 0; i < msg_read.length() - 1; i++) 
      msg_read[i] = msg_read[i + 1];

    msg_read[msg_read.length() - 1] = Serial.read();
    if (msg_read[0] == BEGIN_MSG_BYTE && msg_read[msg_read.length() - 1] == END_MSG_BYTE) { // пришло полное сообщение      
      str = msg_read.substring(2, 2 + str.length()).c_str();      // Копирование результирующего сообщения.
      #if IS_TEST
      lcd.setCursor(0,0);
      lcd.print("Read msg. " + moduleID);
      lcd.setCursor(0,1);
      lcd.print(str);
      #endif
      if (msg_read[1] < cntMsg - 1) {                // если копия не последняя, сбрасываем все последующие
        while (Serial.available()) 
          Serial.read();
      }
    }
    
  }
}

void DataExchange::sendMsg(String &str){
  for (int i = 0; i < msg_send.length() - 3; i++)
    msg_send[2 + i] = str[i];

  msg_send[0] = BEGIN_MSG_BYTE;
  msg_send[msg_send.length() - 1] = END_MSG_BYTE;  

  #if IS_TEST
  lcd.setCursor(0,2);
  lcd.print("Send msg.");
  lcd.setCursor(0,3);
  lcd.print(str);
  #endif

  for (size_t i = 0; i < cntMsg; i++) {
    msg_send[1] = i;
    for (size_t ii = 0; ii < msg_send.length(); ii++) 
      Serial.write(msg_send[ii]);    
    delay(5);
  }
}
