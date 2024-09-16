#ifndef DATAEXCHANGE_HPP
#define DATAEXCHANGE_HPP

constexpr char BEGIN_MSG_BYTE = '8';
constexpr char END_MSG_BYTE = '9';

class DataExchange {
public:
  DataExchange(size_t size, uint8_t cnt = 3, size_t uartSpeed = 57600);
  void readMsg(String &str);
  void sendMsg(String &str);
private:
  String msg;           // Буффер для приема и отправки сообщений
  String msg_out;
  uint8_t msgSize = 0;
  uint8_t cntMsg = 0;   // сколько копий сообщения будет отправляться
};
#endif
