#include "DockerPapa.hpp"
#include "globals.hpp"

DockerPapa::DockerPapa(){
  moduleID = "Papa";

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(RIGHT_HOOK_ACTIVE_8, INPUT);        // Правый крюк нажат
  pinMode(LEFT_HOOK_ACTIVE_9, INPUT);         // Левый крюк нажат
}

inline void DockerPapa::rodExtension()  { //  Функция выдвижения штанги
  if (!rodIsExtended){ // Пока штанга не выдвинута полностью
    Serv::servoSetSpeed(servoRod, SERVO_ROD_5, velocityCW);
  } else {  
    Serv::servoStop(servoRod);
    MSG_Docker[2] = '1';  
  }
}

inline void DockerPapa::rodRetracting(){  // Функция подтягивания штанги, для стягивания дронов
  if (!dockingCompleted) {
    Serv::servoSetSpeed(servoRod, SERVO_ROD_5, velocityCCW);  // задвигаю штангу
  } else {
    Serv::servoStop(servoRod);
    MSG_Docker[4] = '1';
    dockingCompliteMills = millis();
  }
}

inline void DockerPapa::cargoTransfer() { // Функция передачи тележки
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= speedInterval) {
    previousMillis = currentMillis;      
    if (curSpeedRotation < maxWidthPulseRotationCW - widthPulseIterrator)
      curSpeedRotation += widthPulseIterrator;
    Serv::servoSetSpeed(servoCargo, SERVO_CARGO_4, curSpeedRotation);
  }
  if (currentMillis - dockingCompliteMills >= 20000) { // Через секунду проверяем покинула ли тележка папу
    if (!cargoOnBorder)
      MSG_Docker[5] = '1';
  }
}

inline void DockerPapa::cargoTransferEnding() {  // Функция завершения передачи
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= speedInterval) {
    previousMillis = currentMillis;     
    if (curSpeedRotation > minWidthPulseRotationCW + 2 * widthPulseIterrator)
      curSpeedRotation -= widthPulseIterrator;
    Serv::servoSetSpeed(servoCargo, SERVO_CARGO_4, curSpeedRotation);
  }
}

inline void DockerPapa::transferStoping() { // Функция окончательной остановки передачи тележки 
  Serv::servoStop(servoCargo);
}

void DockerPapa::docking(){         // Функция стыковки
  if (MSG_Docker[2] == '0')  // Пока не завершена работа со штангой
    rodExtension();    
  else if (MSG_Docker[3] == '1' && MSG_Docker[4] == '0') // Пока стыковки полностью не завершена
    rodRetracting();      
  else if (MSG_Docker[4] == '1' && MSG_Docker[5] == '0')
    cargoTransfer();
  else if (MSG_Docker[5] == '1' && MSG_Docker[6] == '0')
    cargoTransferEnding();
  else if (MSG_Docker[6] == '1')
    transferStoping();
}

void DockerPapa::undocking(){
  if (MSG_Docker[4] == '1'){    // Дроны сцепились
    if (!rodIsExtended && MSG_Docker[3] == '1'){ // Пока штанга не выдвинута полностью и крюки закрыты
      Serv::servoSetSpeed(servoRod, SERVO_ROD_5, velocityCW);
    } else {
      if (MSG_Docker[3] == '1')   // Ждем пока крюки не расцепятся 
        Serv::servoStop(servoRod);
      else {
        if (!rodIsRetracted)
          Serv::servoSetSpeed(servoRod, SERVO_ROD_5, velocityCCW);
        else{
          Serv::servoStop(servoRod);
          MSG_Docker[4] = '0';
        }
      } 
    }
  } else {
    if (MSG_Docker[5] == '1')
      MSG_Docker[5] = '0';
    else {
      if (cargoOnBorder)
        Serv::servoSetSpeed(servoCargo, SERVO_CARGO_4, velocityCW);
      else
        Serv::servoStop(servoCargo);
    }

    if (MSG_Docker[2] == '1'){
      if (!rodIsRetracted){
        Serv::servoSetSpeed(servoRod, SERVO_ROD_5, velocityCCW);
      } else {
        Serv::servoStop(servoRod);
        MSG_Docker[2] = '0';
      }
    }
  }

}

void DockerPapa::scanDocking(){
  if (MSG_Docker[2] == '0'){ // Пока не завершена работа со штангой
    if (analogRead(ROD_IS_RETRACTED_A2) > 600)
      rodIsRetracted = 1;
    else
      rodIsRetracted = 0;
    if (analogRead(ROD_IS_EXTENDER_A1) > 600)
      rodIsExtended = 1;
    else
      rodIsExtended = 0;
  } else if (MSG_Docker[3] == '1' && MSG_Docker[4] == '0') {
    if (analogRead(DOCKING_COMPLETED_A0) > 600)
      dockingCompleted = 1;
    else
      dockingCompleted = 0;
  } 
  //if (MSG_Docker[4] == '1') {
    if (analogRead(CARGO_ON_BORDER_A3) < 1000)
      cargoOnBorder = 1;
    else
      cargoOnBorder = 0;
    if (analogRead(CARGO_AT_HOME_A6) < 1000)
      cargoAtHome = 1;
    else
      cargoAtHome = 0;
  //}
}

void DockerPapa::scanUndocking(){
  if (MSG_Docker[2] == '1') {
    if (analogRead(ROD_IS_RETRACTED_A2) > 600)
      rodIsRetracted = 1;
    else
      rodIsRetracted = 0;
    if (analogRead(ROD_IS_EXTENDER_A1) > 600)
      rodIsExtended = 1;
    else
      rodIsExtended = 0;
  }
  if (MSG_Docker[4] == '1'){ // Телега не дом но и не покинула
    if (analogRead(CARGO_ON_BORDER_A3) < 1000)
      cargoOnBorder = 1;
    else
      cargoOnBorder = 0;
    if (analogRead(CARGO_AT_HOME_A6) < 1000)
      cargoAtHome = 1;
    else
      cargoAtHome = 0;
  }
}
