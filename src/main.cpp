#include <Arduino.h>
#include "RenderManager.h"

RenderManager renderManager;
void setup(){
	renderManager.begin();
}

void loop(){
	renderManager.loop();
}