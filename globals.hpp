#ifndef GLOBALS_HPP
#define GLOBALS_HPP

constexpr byte ADDRESS_PAIRED_DEVICE   = 0x08;  // Адрес устройства с которым происходит сопряжение не должет превышать 7 бит

constexpr byte DOCKING_COMPLETED_A0    = A0;    // Концевик стыковки
constexpr byte ROD_IS_EXTENDER_A1      = A1;    // Штанга выдвинута
constexpr byte ROD_IS_RETRACTED_A2     = A2;    // Штанга задвинута
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
int velocity = 10;                    // Скорость
int velocityCW = minWidthPulseRotationCW + velocity * widthPulseIterrator;
int velocityCCW = minWidthPulseRotationCCW - velocity * widthPulseIterrator;
//--------------------------------------------------------------------------------------

unsigned long currentMillis;
unsigned long interval = 20;
unsigned long prevMillisScan = 0;
unsigned long previousMillis = 0;
unsigned long dockingCompliteMills = 0;

int curSpeedRotation = minWidthPulseRotationCW;         // Текущая скорость серво 
int speedInterval = 200;          // Интервал увеличения скорости вращения серво. мс

#endif
