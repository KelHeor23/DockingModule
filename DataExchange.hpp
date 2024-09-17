#ifndef DATAEXCHANGE_HPP
#define DATAEXCHANGE_HPP

constexpr char BEGIN_MSG_BYTE = '8';
constexpr char END_MSG_BYTE = '9';

class DataExchange {
public:
  DataExchange(size_t size_read, size_t size_send, uint8_t cnt = 3, size_t uartSpeed = 57600);
  void readMsg(String &str);
  void sendMsg(String &str);
  void setModuleID(String &str) {
    moduleID = str;
  };
  
private:
  String msg_read;       // Буффер для приема
  String msg_send;       // Буффер для отправки сообщения

  uint8_t cntMsg = 0;   // сколько копий сообщения будет отправляться

  String moduleID = "";
};
#endif
