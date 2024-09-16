#ifndef DOCKERPAPA_HPP
#define DOCKERPAPA_HPP

#include "DockerDrones.hpp"

class DockerPapa : public DockerDrones
{
public:
  DockerPapa();
  void docking()        override; // Функция стыковки
  void undocking()      override; // Функция расстыковки
  void scanDocking()    override; // Функция сканирования концевиков при стыковке
  void scanUndocking()  override; // Функция сканирования концевиков при расстыковке
private:
  void rodExtension();          // Функция выдвижения штанги
  void rodRetracting();         // Функция подтягивания штанги, для стягивания дронов
  void cargoTransfer();         // Функция передачи тележки
  void cargoTransferEnding();   // Функция завершения передачи
  void transferStoping();       // Функция окончательной остановки передачи тележки

private:
  bool rodIsRetracted   = 0;        // Штанга задвинута
  bool rodIsExtended    = 0;        // Штанга выдвинута
  bool dockingCompleted = 0;        // Концевики стыковки
  bool cargoOnBorder    = 0;        // Телега пересекла границу. Ушла.
  bool cargoAtHome      = 0;        // Телега зафиксировалась.   Признак начала ухода.
};

#endif