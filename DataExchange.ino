#include "DataExchange.hpp"

#if IS_TEST
#include <LiquidCrystal_I2C.h>        //  Подключаем библиотеку для работы с LCD дисплеем по шине I2C
LiquidCrystal_I2C lcd(0x27, 20, 4);     //  Объявляем  объект библиотеки, указывая параметры дисплея (адрес I2C = 0x27, количество столбцов = 20, количество строк = 4)
#endif

DataExchange::DataExchange(size_t size, uint8_t cnt = 3, size_t uartSpeed = 57600){
  #if IS_TEST
  lcd.init();                       //  Инициируем работу с LCD дисплеем
  lcd.backlight();                  //  Включаем подсветку LCD дисплея
  #endif
  msgSize = size + 3; // Три дополнительных байта на начальный и завершающий символ + байт для количества сообщений
  cntMsg = cnt;
  Serial.begin(uartSpeed);
  msg.reserve(msgSize);

  for (size_t i = 0; i < msgSize; i++){
    msg += '0';
    msg_out += '0';
  }
}

void DataExchange::readMsg(String &str){
  while (Serial.available()) {
    for (size_t i = 0; i < msgSize - 1; i++) msg[i] = msg[i + 1];
    msg[msgSize - 1] = Serial.read();
    if (msg[0] == BEGIN_MSG_BYTE && msg[msgSize - 1] == END_MSG_BYTE) { // пришло полное сообщение      
      str = msg.substring(2, 2 + str.length()).c_str();      // Копирование результирующего сообщения
      #if IS_TEST
      lcd.setCursor(0,0);
      lcd.print("Read msg. " + moduleID);
      lcd.setCursor(0,1);
      lcd.print(str);
      #endif
      if (msg[1] < cntMsg - 1) {                // если копия не последняя, сбрасываем все последующие
        while (Serial.available()) 
          Serial.read();
      }
    }
    
  }
}

void DataExchange::sendMsg(String &str){
  for (int i = 0; i < msgSize - 3; i++)
    msg_out[2 + i] = str[i];

  msg_out[0] = BEGIN_MSG_BYTE;
  msg_out[msgSize - 1] = END_MSG_BYTE;  

  #if IS_TEST
  lcd.setCursor(0,2);
  lcd.print("Send msg.");
  lcd.setCursor(0,3);
  lcd.print(str);
  #endif

  for (size_t i = 0; i < cntMsg; i++) {
    msg_out[1] = i;
    for (size_t ii = 0; ii < msgSize; ii++) 
      Serial.write(msg_out[ii]);    
    delay(5);
  }
}
