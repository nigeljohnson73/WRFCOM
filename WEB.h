#ifndef _WEB_h
#define _WEB_h

#include "App.h"

class TrWEB {
  public:
    TrWEB();

    void begin();
    void loop();

    bool fsWorking() {
      return _static_files;
    };

  private:
    bool _static_files = true;
};

extern TrWEB WEB;

#endif
