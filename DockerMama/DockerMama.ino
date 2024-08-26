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
constexpr byte ROD_IS_EXTENDER_A1      = A1;    // Штанга выдвинута
constexpr byte ROD_IS_RETRACTED_A2     = A2;    // Штанга задвинута
// У папы и мамы разные дома
constexpr byte CARGO_ON_BORDER_A3      = A6;    // Телега зашла
constexpr byte CARGO_AT_HOME_A6        = A3;    // Телега остановилась
constexpr byte START_A7                = A7;    // Пин начала стыковки
constexpr byte SERVO_RIGHT_HOOK_2      = 2;     // Серва на правом крюке
constexpr byte SERVO_LEFT_HOOK_3       = 3;     // Серва на левом крюке
constexpr byte SERVO_CARGO_4           = 4;     // Серва вращения телеги
constexpr byte SERVO_ROD_5             = 5;     // Серва вращения стрелы
constexpr byte RIGHT_HOOK_ACTIVE_8     = 8;     // Правый крюк нажат
constexpr byte LEFT_HOOK_ACTIVE_9      = 9;     // Левый крюк нажат

//--------------------------------Сервы------------------------------------------------
int minWidthPulseRotationCW = 1550;   // Вращение по часовой стрелке минимально
int maxWidthPulseRotationCW = 2500;   // Вращение по часовой стрелке максиамльно
int minWidthPulseRotationCCW = 1450;  // Вращение против часовой стрелки минимльно
int maxWidthPulseRotationCCW = 500;   // Вращение против часовой стрелки максиамльно
int widthPulseStop = 1500;            // Ширина импульста для остановки сервопривода
int widthPulseIterrator = 50;         // Шаг увеличения ширины импульса
//--------------------------------------------------------------------------------------
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
//--------------------------------Телега------------------------------------------------
bool cargoOnBorder  = 0;          // Телега пересекла границу. Для папы - ушла, для мамы пришла
bool cargoAtHome    = 0;          // Телега зафиксировалась.   Для папы - признак начала ухода, для мамы - признак окончания передачи
bool cargoMove      = 0;          // Телега начала движение по матери

Servo servoCargo;                 // Серва вращения телеги
int curSpeedRotation = minWidthPulseRotationCW;         // Текущая скорость серво 
int speedInterval = 500;          // Интервал увеличения скорости вращения серво. мс
unsigned long previousMillis = 0; // Всомогательная переменная для таймера
//--------------------------------------------------------------------------------------

void setup() {
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

  Wire.begin(ADDRESS_PAIRED_DEVICE);          // Инициализируем подключение к шине
  Wire.onReceive(receiveEvent);               // регистрируем полученное событие 
  Wire.onRequest(requestEvent);               // регистрируем запрошенное событие 
  Serial.begin(9600);
  delay(100);
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
    if (i == 0 || i == 2 || i == 4 || i == 5) // 0, 2, 4, 5 записываются в папе
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
  Wire.write(MSG_Docker.c_str());     // отправляем и саму строку которая записанна в строке str
  //Serial.print("Wrte: ");
  //printMsg();
  tmr = 0;                              // сбрасываем таймер в 0
}

inline void lockingHooks(){ // Функция закрытия крюков
    if (rightHookActive)    
      servoRightHook.write(rightHookLockPosition);   
      delay(100); 
    if (leftHookActive)    
      servoLeftHook.write(leftHookLockPosition); 
      delay(100);   
    if (rightHookActive && leftHookActive)    
      MSG_Docker[3] = '1';    
}

inline void cargoTransferBegin() { // Функция передачи тележки
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= speedInterval) {
    previousMillis = currentMillis;      
    if (curSpeedRotation < maxWidthPulseRotationCW - widthPulseIterrator)
      curSpeedRotation += widthPulseIterrator;
    servoSetSpeed(servoCargo, SERVO_CARGO_4, curSpeedRotation);
  }
}

inline void cargoTransferEnding(){  // Функция завершения передачи тележки
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= speedInterval) {
    previousMillis = currentMillis;     
    if (curSpeedRotation > minWidthPulseRotationCW + 2 * widthPulseIterrator)
      curSpeedRotation -= widthPulseIterrator;
    servoSetSpeed(servoCargo, SERVO_CARGO_4, curSpeedRotation);
  }
  if (cargoAtHome){
    cargoMove = 0;
    MSG_Docker[6] = '1';
    servoCargo.writeMicroseconds(widthPulseStop);
  }  
}

inline void transferStoping() { // Функция окончательной остановки передачи тележки 
  servoStop(servoCargo);
}

void docking(){         // Функция стыковки
  if (MSG_Docker[2] == '1' && MSG_Docker[3] == '0')   // Работа со штангой завершена работаем с крюками
    lockingHooks();
  else  if (MSG_Docker[4] == '1' && MSG_Docker[5] == '0')  // Стыковка закончилась, готовлю серво
    cargoTransferBegin();
  else if (MSG_Docker[5] == '1' && MSG_Docker[6] == '0') // Тележка пересекла границу
    cargoTransferEnding();
  else if (MSG_Docker[6] == '1')
    transferStoping();
}

void rast(){            // Функция прерывания стыковки   
  if (MSG_Docker[3] == '1'){
    servoRightHook.write(rightHookStartPosition);
    delay(100);
    servoLeftHook.write(leftHookStartPosition);
    delay(100);
    MSG_Docker[3] = '0';
  } else {
    if (MSG_Docker[6] == '1') // Телега уже зафиксировалась, можно забить
      MSG_Docker[6] = '0';

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
          servoCargo.writeMicroseconds(widthPulseStop);     
          cargoMove = 0;
          firstIn = 1;
        }
      }
    } else
      servoCargo.writeMicroseconds(widthPulseStop); 
  } 
}

void scanUndocking(){  // Сканирование концевиков при расстыковке 
  if (analogRead(CARGO_ON_BORDER_A3) > 600)    
    cargoOnBorder = 1;
  else
    cargoOnBorder = 0;
  if (analogRead(CARGO_AT_HOME_A6) > 600)
    cargoAtHome = 1;
  else
    cargoAtHome = 0;  
}

void scanDocking(){ // Сканирование концевиков при стыковке
  if (MSG_Docker[2] == '1' && MSG_Docker[3] == '0'){   // Работа со штангой завершена работаем с крюками
    rightHookActive = digitalRead(RIGHT_HOOK_ACTIVE_8);       // Правый крюк нажат
    leftHookActive  = digitalRead(LEFT_HOOK_ACTIVE_9);        // Левый крюк нажат
  } else if (MSG_Docker[4] == '1'){
    if (analogRead(CARGO_ON_BORDER_A3) > 600){    
      cargoOnBorder = 1;
      cargoMove = 1;
    }
    else
      cargoOnBorder = 0;
    if (analogRead(CARGO_AT_HOME_A6) > 600)
      cargoAtHome = 1;
    else
      cargoAtHome = 0;
  }
}
void scanConc(){        // Функция сканирования концевиков
  if (MSG_Docker[0] == '1')
    scanDocking();
  else
    scanUndocking();
}

void loop() {
  scanConc(); 

  if (MSG_Docker[0] == '1' && MSG_Docker[1] == '0')  // MSG_Docker[0] - Сигнал от оператора, MSG_Docker[1] - Признак наличия ошибки
    docking(); 
  else
    rast();
}