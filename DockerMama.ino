#include "DockerMama.hpp"
#include "globals.hpp"

DockerMama::DockerMama(){
  moduleID = "Mama";
  dataExchange->setModuleID(moduleID);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(RIGHT_HOOK_ACTIVE_8, INPUT);        // Правый крюк нажат
  pinMode(LEFT_HOOK_ACTIVE_9, INPUT);         // Левый крюк нажат

  servoCargo.attach(SERVO_CARGO_4);           // Серва вращения тележки

  servoRightHook.attach(SERVO_RIGHT_HOOK_2);
  servoRightHook.write(hookStartPosition);
  delay(100);

  servoLeftHook.attach(SERVO_LEFT_HOOK_3);
  servoLeftHook.write(hookStartPosition);
  delay(100);

  dataExchange = new DataExchange(MSG_papa.length(), MSG_mama.length());
}

DockerMama::~DockerMama(){
  delete dataExchange;
}

inline void DockerMama::lockingHooks(){ // Функция закрытия крюков
  if (!hooksIsLock) {
    if (rightHookActive)    
      servoRightHook.write(rightHookLockPosition);   
    delay(100); 
    if (leftHookActive)    
      servoLeftHook.write(leftHookLockPosition); 
    delay(100);   
    if (rightHookActive && leftHookActive)    
      hooksIsLock = 1;
  } else {
    MSG_mama[1] = '1';
  }
}

inline void DockerMama::cargoTransferBegin() { // Функция передачи тележки
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= speedInterval) {
    previousMillis = currentMillis;      
    if (curSpeedRotation < maxWidthPulseRotationCW - widthPulseIterrator)
      curSpeedRotation += widthPulseIterrator;
    Serv::servoSetSpeed(servoCargo, SERVO_CARGO_4, curSpeedRotation);
  }
}

inline void DockerMama::cargoTransferEnding(){  // Функция завершения передачи тележки
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= speedInterval) {
    previousMillis = currentMillis;     
    if (curSpeedRotation > minWidthPulseRotationCW + 2 * widthPulseIterrator)
      curSpeedRotation -= widthPulseIterrator;
    Serv::servoSetSpeed(servoCargo, SERVO_CARGO_4, curSpeedRotation);
  }
  if (cargoAtHome){
    cargoMove = 0;
    MSG_mama[2] = '1';
    Serv::servoStop(servoCargo);
  }  
}

inline void DockerMama::transferStoping() { // Функция окончательной остановки передачи тележки 
  Serv::servoStop(servoCargo);
}

void DockerMama::docking(){         // Функция стыковки
  if (MSG_papa[1] == '1' && MSG_mama[1] == '0')   // Работа со штангой завершена работаем с крюками
    lockingHooks();
  else  if (MSG_papa[2] == '1' && MSG_papa[3] == '0')  // Стыковка закончилась, готовлю серво
    cargoTransferBegin();
  else if (MSG_papa[3] == '1' && MSG_mama[2] == '0') // Тележка пересекла границу
    cargoTransferEnding();
  else if (MSG_mama[2] == '1')
    transferStoping();
}

void DockerMama::undocking(){            // Функция прерывания стыковки   
  if (MSG_mama[1] == '1'){
    servoRightHook.write(hookStartPosition);
    delay(100);
    servoLeftHook.write(hookStartPosition);
    delay(100);
    MSG_mama[1] = '0';
  } else {
    if (MSG_mama[2] == '1') // Телега уже зафиксировалась, можно забить
      MSG_mama[2] = '0';

    if (cargoMove) {  // Телега зашла, но не на месте. Избавься от нее
      servoCargo.writeMicroseconds(maxWidthPulseRotationCCW);
      static bool firstIn = 1;
      if (cargoOnBorder && firstIn){
        previousMillis = millis();
        firstIn = 0;
      }
      if (!firstIn){
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= 500){
          Serv::servoStop(servoCargo);   
          cargoMove = 0;
          firstIn = 1;
        }
      }
    } else
      Serv::servoStop(servoCargo);
  } 
}

void DockerMama::scanUndocking(){  // Сканирование концевиков при расстыковке 
  if (analogRead(CARGO_ON_BORDER_A3) < 600)    
    cargoOnBorder = 1;
  else
    cargoOnBorder = 0;
    
  if (analogRead(CARGO_AT_HOME_A6) < 600)
    cargoAtHome = 1;
  else
    cargoAtHome = 0;  
}

void DockerMama::scanDocking(){ // Сканирование концевиков при стыковке
  if (MSG_papa[1] == '1' && MSG_mama[1] == '0'){   // Работа со штангой завершена работаем с крюками
    rightHookActive = digitalRead(RIGHT_HOOK_ACTIVE_8);       // Правый крюк нажат
    leftHookActive  = digitalRead(LEFT_HOOK_ACTIVE_9);        // Левый крюк нажат
  } else if (MSG_papa[2] == '1'){
    if (analogRead(CARGO_ON_BORDER_A3) < 600){    
      cargoOnBorder = 1;
      cargoMove = 1;
    }
    else
      cargoOnBorder = 0;
    if (analogRead(CARGO_AT_HOME_A6) < 600)
      cargoAtHome = 1;
    else
      cargoAtHome = 0;
  }
}

bool DockerMama::isReady() {
  return ready && MSG_papa[0] == '1'; // Была дана команда о начале, нет отказов в работе и папа готов 
}
