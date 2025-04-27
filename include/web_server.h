#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>

extern WebServer server;  // 👈 only "extern" declaration here!

void startWebServer();
void handleClient();

#endif
