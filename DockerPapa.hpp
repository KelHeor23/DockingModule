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
};

#endif