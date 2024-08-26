/* Проверка производилась на:

   Тест кода производился никогда
*/

#include <Wire.h>
#include <Servo.h>

/*
MSG_Docker байты:
  MSG_Docker[0] - Начало стыковки. 1 - начало, 0 - нейтраль или отсоединение
  MSG_Docker[1] - Признак наличия ошибки. 1 - ошибка, разоединяемся, 0 - все ок
  MSG_Docker[2] - Стрела выдвинута 1 - выдвинута, 2 - задвинута
  MSG_Docker[3] - Хуки закрылись 1 - закрыты, 2 - открыты
  MSG_Docker[4] - Дроны стянулись 1 - закрыты, 2 - открыты
  MSG_Docker[5] - Телега покинула папу 1 - да, 2 - нет
  MSG_Docker[6] - Телега передана маме 1 - да, 2 - нет
*/
constexpr size_t MSG_SIZE = 7;        // Размер сообщения байт

String str;
String MSG_Docker = "0000000";

constexpr byte ADDRESS_PAIRED_DEVICE   = 0x08;  // Адрес устройства с которым происходит сопряжение не должет превышать 7 бит

constexpr byte DOCKING_COMPLETED_A0    = A0;    // Концевик стыковки
constexpr byte ROD_IS_EXTENDER_A1      = A2;    // Штанга выдвинута
constexpr byte ROD_IS_RETRACTED_A2     = A1;    // Штанга задвинута
constexpr byte CARGO_ON_BORDER_A3      = A3;    // Телега зашла
constexpr byte CARGO_AT_HOME_A6        = A6;    // Телега остановилась
constexpr byte START_A7                = A7;    // Пин начала стыковки
constexpr byte SERVO_RIGHT_HOOK_2      = 2;     // Серва на правом крюке
constexpr byte SERVO_LEFT_HOOK_3       = 3;     // Серва на левом крюке
constexpr byte SERVO_CARGO_4           = 4;     // Серва вращения телеги
constexpr byte SERVO_ROD_5             = 5;     // Серва вращения стрелы
constexpr byte RIGHT_HOOK_ACTIVE_8     = 8;     // Правый крюк нажат
constexpr byte LEFT_HOOK_ACTIVE_9      = 9;     // Левый крюк нажат


//--------------------------------Крюки------------------------------------------------
Servo servoRightHook;   // Серва на правом крюке
Servo servoLeftHook;    // Серва на левом крюке
bool rightHookActive = 0;          // Правый крюк нажат
bool leftHookActive = 0;           // Левый крюк нажат

int hookStartPosition = 100;

int rotate = 41;
int leftHookLockPosition = hookStartPosition - rotate;
int rightHookLockPosition = hookStartPosition + rotate;
//--------------------------------------------------------------------------------------
//--------------------------------Сервы------------------------------------------------
int minWidthPulseRotationCW = 1550;   // Вращение по часовой стрелке минимально
int maxWidthPulseRotationCW = 2500;   // Вращение по часовой стрелке максиамльно
int minWidthPulseRotationCCW = 1450;  // Вращение против часовой стрелки минимльно
int maxWidthPulseRotationCCW = 500;   // Вращение против часовой стрелки максиамльно
int widthPulseStop = 1500;            // Ширина импульста для остановки сервопривода
int widthPulseIterrator = 50;         // Шаг увеличения ширины импульса
int velocity = 10;                     // Скорость
int velocityCW = minWidthPulseRotationCW + velocity * widthPulseIterrator;
int velocityCCW = minWidthPulseRotationCCW - velocity * widthPulseIterrator;
//--------------------------------------------------------------------------------------
Servo servoRod;                   // Серва вращения стрелы
Servo servoCargo;                 // Серва вращения телеги

unsigned long currentMillis;
unsigned long interval = 20;
unsigned long prevMillisScan = 0;
unsigned long previousMillis = 0;
unsigned long dockingCompliteMills = 0;

int curSpeedRotation = minWidthPulseRotationCW;         // Текущая скорость серво 
int speedInterval = 200;          // Интервал увеличения скорости вращения серво. мс

byte cargoOnBorder  = 0;          // Телега пересекла границу. Для папы - ушла, для мамы пришла
byte cargoAtHome    = 0;          // Телега зафиксировалась.   Для папы - признак начала ухода, для мамы - признак окончания передачи

//-----------------------------------Глобальные переменные------------------------------------
byte rodIsRetracted = 0;          // Штанга задвинута
byte rodIsExtended = 0;           // Штанга выдвинута
byte dockingCompleted = 0;        // Концевики стыковки
//--------------------------------------------------------------------------------------

void servoSetSpeed(Servo &servo, byte address, int speed){
  servo.attach(address);
  servo.writeMicroseconds(speed);
}

void servoStop(Servo &servo){
  servo.writeMicroseconds(widthPulseStop);
  servo.detach();
}

void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(RIGHT_HOOK_ACTIVE_8, INPUT);        // Правый крюк нажат
  pinMode(LEFT_HOOK_ACTIVE_9, INPUT);         // Левый крюк нажат

  Wire.begin(ADDRESS_PAIRED_DEVICE);          // Инициализируем подключение к шине
  
  Wire.onReceive(receiveEvent);               // регистрируем полученное событие 
  Wire.onRequest(requestEvent);               // регистрируем запрошенное событие 
  
  Serial.begin(9600);
  delay(200);
}

void printMsg(){
  char str[MSG_SIZE * 5 + MSG_SIZE]; // Увеличили размер буфера для умещения сообщения формата "X = Y"
  // Для каждого элемента массива MSG_Docker форматируем строку и выводим ее
  for (int i = 0; i < MSG_SIZE; i++) {
      sprintf(str, "%d = %c", i, MSG_Docker[i]);
      Serial.print(str);
      Serial.print(" ");
  }
  Serial.print('\n');
}

void receiveEvent(int howMany){    // Функция для чтения сообщения
  int i = 0;
  while(Wire.available()) {
    if (i == 1 || i == 3 || i == 6) // 1, 3, 6 записываются в маме
      MSG_Docker[i] = Wire.read();
    else
      Wire.read();
    i++;
  }
  Serial.print("Read: ");
  printMsg();
}
 
void requestEvent(){    // Функция для записи данных в сообщение
  static int tmr = 0;
  if(tmr == 0){                       // Переменная служит для определения в какую функцию входить, если она 0 то
    Wire.write((char*)&MSG_SIZE);     // отправляем по запросу от мастера длинну строки которую собираемся передать 
    if(MSG_SIZE == 0){                // если размер строки равен 0 то
      return;                         // выходим из функции
    }                                 // если данные для передачи есть
    tmr++;                            // устанавливаем таймер на 1
    return;                           // и выходим из функции
  }                                   // при следующем запросе от мастера мы начнем выполнять код с этого места так как таймер равен 1
  Wire.write(MSG_Docker.c_str());    // отправляем и саму строку которая записанна в строке str
  Serial.print("Wrte: ");
  printMsg();
  tmr = 0;                             // сбрасываем таймер в 0
}

inline void rodExtension()  { //  Функция выдвижения штанги
  if (!rodIsExtended){ // Пока штанга не выдвинута полностью
    servoSetSpeed(servoRod, SERVO_ROD_5, velocityCW);
  } else {  
    servoStop(servoRod);
    MSG_Docker[2] = '1';  
  }
}

inline void rodRetracting(){  // Функция подтягивания штанги, для стягивания дронов
  if (!dockingCompleted) {
    servoSetSpeed(servoRod, SERVO_ROD_5, velocityCCW);  // задвигаю штангу
  } else {
    servoStop(servoRod);
    MSG_Docker[4] = '1';
    dockingCompliteMills = millis();
  }
}

inline void cargoTransfer() { // Функция передачи тележки
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= speedInterval) {
    previousMillis = currentMillis;      
    if (curSpeedRotation < maxWidthPulseRotationCW - widthPulseIterrator)
      curSpeedRotation += widthPulseIterrator;
    servoSetSpeed(servoCargo, SERVO_CARGO_4, curSpeedRotation);
  }
  if (currentMillis - dockingCompliteMills >= 20000) { // Через секунду проверяем покинула ли тележка папу
    if (!cargoOnBorder)
      MSG_Docker[5] = '1';
  }
}

inline void cargoTransferEnding() {  // Функция завершения передачи
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= speedInterval) {
    previousMillis = currentMillis;     
    if (curSpeedRotation > minWidthPulseRotationCW + 2 * widthPulseIterrator)
      curSpeedRotation -= widthPulseIterrator;
    servoSetSpeed(servoCargo, SERVO_CARGO_4, curSpeedRotation);
  }
}

inline void transferStoping() { // Функция окончательной остановки передачи тележки 
  servoStop(servoCargo);
}

void docking(){         // Функция стыковки
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

void undocking(){
  if (MSG_Docker[4] == '1'){    // Дроны сцепились
    if (!rodIsExtended && MSG_Docker[3] == '1'){ // Пока штанга не выдвинута полностью и крюки закрыты
      servoSetSpeed(servoRod, SERVO_ROD_5, velocityCW);
    } else {
      if (MSG_Docker[3] == '1')   // Ждем пока крюки не расцепятся 
        servoStop(servoRod);
      else {
        if (!rodIsRetracted)
          servoSetSpeed(servoRod, SERVO_ROD_5, velocityCCW);
        else{
          servoStop(servoRod);
          MSG_Docker[4] = '0';
        }
      } 
    }
  } else {
    if (MSG_Docker[5] == '1')
      MSG_Docker[5] = '0';
    else {
      if (cargoOnBorder)
        servoSetSpeed(servoCargo, SERVO_CARGO_4, velocityCW);
      else
        servoStop(servoCargo);
    }

    if (MSG_Docker[2] == '1'){
      if (!rodIsRetracted){
        servoSetSpeed(servoRod, SERVO_ROD_5, velocityCCW);
      } else {
        servoStop(servoRod);
        MSG_Docker[2] = '0';
      }
    }
  }

}

void scanDocking(){
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

void scanUndocking(){
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

void scanConc(){        // Функция сканирования концевиков
  if (analogRead(START_A7) > 600){
    MSG_Docker[0] = '1';
    scanDocking();
  } else {
    MSG_Docker[0] = '0';
    scanUndocking();
  } 
}

void loop() {
  scanConc();  
  if (MSG_Docker[0] == '1' && MSG_Docker[1] == '0')  // MSG_Docker[0] - Сигнал от оператора, MSG_Docker[1] - Признак наличия ошибки
    docking(); 
  else
    undocking(); 
}