#ifndef _NET_h
#define _NET_h

#include "App.h"

class TrNET
{
  public:
    TrNET();

    void begin(String ssid, String pass, long wait_secs);
    void loop();

    void setHostname(String name);

    String getTimestamp();

    bool isEnabled() {
      return _enabled;
    };

    String getIpAddress() {
      return _ip_address;
    }

    bool isApMode() {
      return _ap_ssid.length() > 0;
    }

    String getHostname() {
      return _hostname;
    }

  private:
    bool _enabled;
    String _hostname;
    String _ip_address;

    String _ap_ssid;
    String _ap_pass;

};

extern TrNET NET;

#endif
