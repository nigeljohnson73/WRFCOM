#ifndef _SERVER_h
#define _SERVER_h

#include "App.h"

class TrServer {
  public:
    TrServer();

    void begin();
    void loop();
};

extern TrServer Server;

#endif
