#ifndef _BLE_h
#define _BLE_h

#include "App.h"

class TrBLE {
  public:
    TrBLE();

    void begin();
    void loop();

    bool isEnabled() {
      return _enabled;
    };

  private:
    bool _enabled = false;
    unsigned long _refresh_last = 0;
    unsigned long _refresh_millis = 1000 / BLE_HZ;

	 unsigned long _last_disconnect = 0; // Allow for stack to unwind on a disconnection
	 bool _disconnecting = false; // A disconnect has happened to pick up readvertising once the stack has caught up
};

extern TrBLE BLE;

#endif
