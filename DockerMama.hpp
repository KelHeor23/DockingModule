#ifndef DOCKERMAMA_HPP
#define DOCKERMAMA_HPP

#include "DockerDrones.hpp"

class DockerMama : public DockerDrones
{
public:
  DockerMama();
  void docking()        override; // Функция стыковки
  void undocking()      override; // Функция расстыковки
  void scanDocking()    override; // Функция сканирования концевиков при стыковке
  void scanUndocking()  override; // Функция сканирования концевиков при расстыковке

private:
  void lockingHooks();            // Функция закрытия крюков
  void cargoTransferBegin();      // Функция передачи тележки
  void cargoTransferEnding();     // Функция завершения передачи тележки
  void transferStoping();         // Функция окончательной остановки передачи тележки 

private:
  bool hooksIsLock = false;   // Признак что оба крюка закрыты
  bool cargoMove   = false;   // Признак, что тележка находится на дроне, но не зафиксирована
};

#endif