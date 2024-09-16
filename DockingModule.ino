#define IS_PAPA 1 // 1 - версия для папы, 0 - версия для мамы
#define IS_TEST 1 // 1 - тестовая, 0 - релизная (в частности, для работы с LCD)

#if IS_PAPA
String moduleID = "Papa";
#else
String moduleID = "Mama";
#endif

#include "DataExchange.hpp"

String MSG_read = "0000000";
String MSG_send = MSG_read;
DataExchange *dataExchange;

void setup() {
  // put your setup code here, to run once:
  dataExchange = new DataExchange(MSG_send.length());
}

void loop() {
  // put your main code here, to run repeatedly:
  #if IS_PAPA
  MSG_send[0] = '1';  
  MSG_send[6] = '1';
  dataExchange->sendMsg(MSG_send);
  dataExchange->readMsg(MSG_read);  
  #else
  MSG_send[1] = '1';
  MSG_send[2] = '1';
  MSG_send[3] = '1';
  MSG_send[4] = '1';
  MSG_send[5] = '1';
  dataExchange->readMsg(MSG_read);  
  dataExchange->sendMsg(MSG_send);
  #endif

  delay(2000);
}
