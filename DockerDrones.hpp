#ifndef DOCKERDRONES_HPP
#define DOCKERDRONES_HPP

#include <Servo.h>
#include "globals.hpp"

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

  Servo servoRod;                   // Серва вращения стрелы
  Servo servoCargo;                 // Серва вращения телеги  
  Servo servoRightHook;             // Серва на правом крюке
  Servo servoLeftHook;              // Серва на левом крюке

  String moduleID;                  // Текстовое обозначение модуля
};

#endif