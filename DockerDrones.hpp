#ifndef DOCKERDRONES_HPP
#define DOCKERDRONES_HPP

#include <Servo.h>

#include "globals.hpp"
#include "DataExchange.hpp"

/*Функции для работы с сервами*/
namespace Serv {
  void servoSetSpeed(Servo &servo, byte address, int speed){
    servo.attach(address);
    servo.writeMicroseconds(speed);
  }

  void servoStop(Servo &servo){
    servo.writeMicroseconds(widthPulseStop);
    servo.detach();
  }
}

class DockerDrones {
public:
  virtual void docking() = 0;       // Функция стыковки
  virtual void undocking() = 0;     // Функция расстыковки
  virtual void scanDocking() = 0;   // Функция сканирования концевиков при стыковке
  virtual void scanUndocking() = 0; // Функция сканирования концевиков при расстыковке
  virtual bool isReady() = 0;       // Функия проверки готовности

  Servo servoRod;                   // Серва вращения стрелы
  Servo servoCargo;                 // Серва вращения телеги  
  Servo servoRightHook;             // Серва на правом крюке
  Servo servoLeftHook;              // Серва на левом крюке

  String moduleID;                  // Текстовое обозначение модуля

/*
  MSG_mama
  0 - готовность
  1 - хуки закрылись
  2 - телега пришла к маме
*/
  String MSG_mama = "000";          // Сообщение отправленное от мамы

/*
  MSG_papa
  0 - готовность
  1 - стрела выдвинулась
  2 - дроны стянулись
  3 - телега покинула папу
*/
  String MSG_papa = "0000";         // Сообщение отправленное от папы

  DataExchange *dataExchange;       // Механизм обмена сообщениями

  bool ready = true;              // Флаг готовности. При ошибке, он опустится
};

#endif