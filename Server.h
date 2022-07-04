#ifndef _WEB_h
#define _WEB_h

#include "App.h"

class TrServer {
  public:
    TrServer();

    void begin();
    void loop();
};

extern TrServer Server;

#endif
