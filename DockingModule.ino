#define IS_TEST 1 // 1 - тестовая, 0 - релизная (в частности, для работы с LCD)

#include "DockerMama.hpp"
#include "DockerPapa.hpp"

constexpr int isPapa = 1; // 1 - папа, 0 - мама. ВРЕМЕНОЕ РЕШЕНИЕ!

DockerDrones *docker;

void setup() {
  switch (isPapa){
    case 0:
      docker = new DockerMama();
      break;
    case 1:
      docker = new DockerPapa();
      break;
    default:
      docker = new DockerPapa();
      break;
  }
}

void loop() {
  if (analogRead(START_A7) > 600){
    docker->scanDocking();
    docker->docking();    
  } else {
    docker->scanUndocking();
    docker->undocking();
  }
}
