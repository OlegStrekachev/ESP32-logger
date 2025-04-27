#ifndef DHT_MODULE_H
#define DHT_MODULE_H

#include <DHT.h>  // <--- ⚡ You must also include this because you are exposing "DHT" type

extern DHT dht;  // 🔥 This exposes your dht object globally

void initDHT();
void printDHTValues();

#endif
