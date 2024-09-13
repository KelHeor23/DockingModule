#define IS_PAPA 1 // 1 - версия для папы, 0 - версия для мамы
#define IS_TEST 1 // 1 - тестовая, 0 - релизная (в частности, для работы с LCD)

#if IS_PAPA
String moduleID = "Papa";
#else
String moduleID = "Mama";
#endif

#include "DataExchange.hpp"

String MSG_Docker = "0000000";
DataExchange *dataExchange;

void setup() {
  // put your setup code here, to run once:
  dataExchange = new DataExchange(MSG_Docker.length());
}

void loop() {
  // put your main code here, to run repeatedly:
  #if IS_PAPA
  MSG_Docker[0] = '1';
  MSG_Docker[6] = '1';
  #endif

  dataExchange->readMsg(MSG_Docker); 

  dataExchange->sendMsg(MSG_Docker);

  delay(2000);
}
