#ifndef _NET_h
#define _NET_h

#include "App.h"

class TrNET
{
  public:
    TrNET();

    void begin(String ssid, String pass, long wait_secs);
    void loop();

    bool isApMode();
    void setHostname(String name);
    String getHostname();

    String getIpAddress() {
      return _ip_address;
    }

    String getTimestamp();

  private:
    String _hostname;
    String _ip_address;

    String _ap_ssid;
    String _ap_pass;

};

extern TrNET NET;

#endif
