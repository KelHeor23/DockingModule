#include "DataExchange.hpp"

DataExchange::DataExchange(size_t size, uint8_t cnt = 3, size_t uartSpeed = 57600){
  msgSize = size + 3; // Три дополнительных байта на начальный и завершающий символ + байт для количества сообщений
  cntMsg = cnt;
  Serial.begin(uartSpeed);
  msg.reserve(msgSize);

  for (size_t i = 0; i < msgSize; i++)
    msg += '0';

  msg[0] = BEGIN_MSG_BYTE;
  msg[msgSize - 1] = END_MSG_BYTE;

  #if IS_TEST
  lcd.init();                       //  Инициируем работу с LCD дисплеем
  lcd.backlight();                  //  Включаем подсветку LCD дисплея
  #endif
}

void DataExchange::readMsg(String &str){
  while (Serial.available()) {
    for (size_t i = 0; i < msgSize - 1; i++) msg[i] = msg[i + 1];
    msg[msgSize - 1] = Serial.read();
    if (msg[0] == BEGIN_MSG_BYTE && msg[msgSize - 1] == END_MSG_BYTE) { // пришло полное сообщение
      str = msg.substring(2, msgSize - 2);      // Копирование результирующего сообщения
      
      #if IS_TEST
      lcd.setCursor(0,0);
      lcd.print("Read msg." + moduleID);
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
    msg[2 + i] = str[i];

  #if IS_TEST
  lcd.setCursor(0,2);
  lcd.print("Send msg.");
  lcd.setCursor(0,3);
  lcd.print(str);
  #endif

  for (size_t i = 0; i < cntMsg; i++) {
    msg[1] = i;
    for (size_t ii = 0; ii < msgSize; ii++) 
      Serial.write(msg[ii]);    
    delay(5);
  }
}
