#include "WEB.h"
#include "XBase64.h"

#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
AsyncWebServer WebServer(80);
#else
#include <ESP8266WebServer.h>
ESP8266WebServer WebServer(80);
#endif


TrWEB WEB;

// Adding and manging local URLs for the about page mostly
const unsigned int pmax = 20;
unsigned int pcount = 0;
String urls[pmax];
String titles[pmax];

AsyncWebServerRequest * __request;

/*******************************************************************************************************************************************
            _         _                  _           _
      /\   | |       | |                | |         | |
     /  \  | |__  ___| |_ _ __ __ _  ___| |_ ___  __| |   ___  ___ _ ____   _____ _ __
    / /\ \ | '_ \/ __| __| '__/ _` |/ __| __/ _ \/ _` |  / __|/ _ \ '__\ \ / / _ \ '__|
   / ____ \| |_) \__ \ |_| | | (_| | (__| ||  __/ (_| |  \__ \  __/ |   \ V /  __/ |
  /_/    \_\_.__/|___/\__|_|  \__,_|\___|\__\___|\__,_|  |___/\___|_|    \_/ \___|_|

*/
void serverBegin() {
  WebServer.begin();
}

#ifdef ESP32
void registerUri(String uri, ArRequestHandlerFunction func) {
  WebServer.on(uri.c_str(), HTTP_GET, func);
}
#else
void registerUri(String uri, void (*func)(void)) {
  WebServer.on(uri, func);
}
#endif

//void registerStaticUri(String uri, String filename) {
//  //WebServer.serveStatic(uri.c_str(), SPIFFS, filename.c_str());
//}

#ifdef ESP32
void registerNotFound(ArRequestHandlerFunction func) {
#else
void registerNotFound(void (*func)(void)) {
#endif
  WebServer.onNotFound(func);
}

String getUri() {
#ifdef ESP32
  if (__request) {
    return __request->url();
  } else {
    return "/unknown";
  }
#else
  return WebServer.uri();
#endif
}

void serverResponse(int code, String type, const char * content, size_t len) {
#ifdef ESP32
  if (__request) {

#if _DEBUG_ && _XDEBUG_
    Serial.print("serverResponse(<binary data>): ");
    Serial.print(len);
    Serial.print(" bytes being sent");
    Serial.println();
#endif

    // This only sends a code 200. The input code is redundant
    AsyncResponseStream *response = __request->beginResponseStream(type, len);
    for (size_t i = 0; i < len; i++) {
      response->print(content[i]);
    }
    __request->send(response);
  }
#else
  // TBD
#endif
}

void serverResponse(int code, String type, String content) {
#ifdef ESP32
  if (__request) {
    __request->send(code, type, content);
    //serverResponse(code, type, (uint8_t *)content.c_str(), content.length());
  }
#else
  WebServer.send(code, type, content);
#endif
}

void serverLoop() {
#ifdef ESP32
  // It's all handled in the background
#else
  WebServer.handleClient();
#endif
}

/*******************************************************************************************************************************************
   _    _      _                     __                  _   _
  | |  | |    | |                   / _|                | | (_)
  | |__| | ___| |_ __   ___ _ __   | |_ _   _ _ __   ___| |_ _  ___  _ __  ___
  |  __  |/ _ \ | '_ \ / _ \ '__|  |  _| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
  | |  | |  __/ | |_) |  __/ |     | | | |_| | | | | (__| |_| | (_) | | | \__ \
  |_|  |_|\___|_| .__/ \___|_|     |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                | |
                |_|
*/

/*******************************************************************************************************************************************
   _    _ _____  _____                  _     _             _   _
  | |  | |  __ \|_   _|                (_)   | |           | | (_)
  | |  | | |__) | | |    _ __ ___  __ _ _ ___| |_ _ __ __ _| |_ _  ___  _ __
  | |  | |  _  /  | |   | '__/ _ \/ _` | / __| __| '__/ _` | __| |/ _ \| '_ \
  | |__| | | \ \ _| |_  | | |  __/ (_| | \__ \ |_| | | (_| | |_| | (_) | | | |
   \____/|_|  \_\_____| |_|  \___|\__, |_|___/\__|_|  \__,_|\__|_|\___/|_| |_|
                                   __/ |
                                  |___/
*/

#ifdef ESP32
void addPage(String uri, ArRequestHandlerFunction func, String title, bool hidden) {
#else
void addPage(String uri, void (*func)(void), String title, bool hidden) {
#endif
#if _XDEBUG_
  Serial.print("WebServer::addPage(");
  Serial.print(uri);
  if (hidden) Serial.print(", hidden");
  else Serial.print(String(", '") + title + "'");
  Serial.print("): called");
  Serial.println();
#endif

  if (!hidden) {
    titles[pcount] = title;
    urls[pcount++] = uri;
  }

  registerUri(uri, func);
}

#ifdef ESP32
void addPage(String uri, ArRequestHandlerFunction func, String title) {
#else
void addPage(String uri, void (*func)(void), String title) {
#endif
  addPage(uri, func, title, false);
}

#ifdef ESP32
void addPage(String uri, ArRequestHandlerFunction func) {
#else
void addPage(String uri, void (*func)(void)) {
#endif
  addPage(uri, func, "", true);
}

//void addStaticPage(String uri, String filename, String title, bool hidden) {
//#if _XDEBUG_
//  Serial.print("WebServer::addStaticPage(");
//  Serial.print(uri);
//  if (hidden) Serial.print(", hidden");
//  else Serial.print(String(", '") + title + "'");
//  Serial.print("): called");
//  Serial.println();
//#endif
//
//  if (!hidden) {
//    titles[pcount] = title;
//    urls[pcount++] = uri;
//  }
//  // TODO: this will be different for ESP32
//  registerStaticUri(uri, filename);
//}
//void addStaticPage(String uri, String filename, String title) {
//  addStaticPage(uri, filename, title, false);
//}
//
//void addStaticPage(String uri, String filename) {
//  addStaticPage(uri, filename, "", true);
//}

/*******************************************************************************************************************************************
   _    _ _______ __  __ _                                          _          __  __
  | |  | |__   __|  \/  | |                                        | |        / _|/ _|
  | |__| |  | |  | \  / | |         _ __   __ _  __ _  ___      ___| |_ _   _| |_| |_
  |  __  |  | |  | |\/| | |        | '_ \ / _` |/ _` |/ _ \    / __| __| | | |  _|  _|
  | |  | |  | |  | |  | | |____    | |_) | (_| | (_| |  __/    \__ \ |_| |_| | | | |
  |_|  |_|  |_|  |_|  |_|______|   | .__/ \__,_|\__, |\___ |   |___/\__|\__,_|_| |_|
                                   | |           __/ |
                                   |_|          |___/
*/

void goToUrl(String url) {
  String ret = "";

  ret += "<!DOCTYPE html><html>";
  ret += "<head>";
  ret += "<title>redirection</title>";
  ret += "</head> ";
  ret += "<body>";
  ret += "<script>";
  ret += "window.location.href='";
  ret += url;
  ret += "';";
  ret += "</script>";
  ret += "</body></html>";

  serverResponse(200, "text/html", ret.c_str());
}

String htmlHeader(String title) {
  String ret = "";

  ret += "<!DOCTYPE html><html>";
  ret += "<head>";
  ret += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  ret += "<title>";
  ret += title;
  ret += "</title>";

  ret += "<link rel='shortcut icon' href='/favicon.png' />";
  //  if (WEB.fsWorking()) {
  //    ret += "<link rel='preload' as='image' href='/ajax-loader-bar.gif' />";
  //  }

  ret += "<style>";

  ret += "html, body {height: 100%;}";
  ret += "body {display:flex; flex-direction: column; background-color: #eee; font-family: Arial, Helvetica, Sans-Serif; color: #090; text-align: center;}";
  ret += "#main {flex: 1 0 auto; padding-bottom: 10px; padding-left: 30px;padding-right: 30px; }";
  ret += "footer {flex-shrink: 0;padding-bottom: 20px;}";
  ret += ".status-wrapper {clear: both;}";
  ret += ".status-wrapper .label, .status-wrapper .value {padding-top: 10px; padding-bottom: 10px; width: 49.5%; float: left; margin-bottom: 10px;}";
  ret += ".status {width: 99%; margin-bottom: 10px; padding-top: 10px; padding-bottom: 10px; border-radius: 5px; border: solid 1px #000; background-color: #ddd; color: #000;}";
  ret += "button{all:unset; width: 99%; margin-bottom: 10px; padding-top: 10px; padding-bottom: 10px; border-radius: 5px; border: solid 1px #060; background-color: #090; color: #fff;}";
  ret += "footer button {float: left; width: ";
  ret += floor(98 / pcount) - 2;
  ret += "%; margin-left:1%; margin-right:1%;}";
  ret += "footer button.selected {background-color:#ccc; border-color: #aaa;}";
  ret += ".status-red {border-color: #a00; background-color: #faa; color: #a00;}";
  ret += ".status-amber {border-color: #a70; background-color: #fca; color: #a70;}";
  ret += ".status-green {border-color: #0a0; background-color: #afa; color: #0a0;}";
  ret += ".status-disabled {border-color: #aaa; background-color: #ddd; color: #fff;}";
  ret += "footer .action {display:none;}";
  ret += "#log-summary {margin-top:2px; padding-top: 10px; padding-bottom: 10px; padding-left: 10px;padding-right: 10px; border:solid #666 1px; border-radius:5px; background-color:#e3e3e3; color:#666;}";
  ret += "a#logstat {margin-top:7px;}";

  ret += " </style> ";

  ret += "<script>";
  ret += "function hideActions() { document.getElementById('footer-buttons').style.display = 'none'; document.getElementById('footer-action').style.display = 'inline-block'; document.getElementById('actions').style.display = 'none'; document.getElementById('log-summary').style.display = 'none';}";
  ret += "</script>";

  ret += " </head> ";
  ret += "<body><div id='main'>";

  return ret;
}

String htmlFooter() {
  String content = "";

  content += "</div><footer>";
  content += "<div id='footer-buttons'>";
  String this_uri = getUri();
  for (int i = 0; i < pcount; i++) {
    content += "<button";
    if (urls[i] == this_uri) {
      content += " class='selected'";
    }
    content += " onclick='window.location.href=\"";
    content += urls[i];
    content += "\"; hideActions();'>";
    content += titles[i];
    content += "</button>";
  }
  content += "</div>";
  content += "<p id='footer-action' class='action'>Please wait...</p>";
  content += "</footer></body></html> ";

  return content;
}

String htmlPage(String title, String content) {
  return htmlHeader(title) + content + htmlFooter();
}

/*******************************************************************************************************************************************

*/

#ifdef ESP32
void showNotFound(AsyncWebServerRequest * request) {
  __request = request;
#else
void showNotFound() {
#endif
#if _DEBUG_
  String uri = getUri();
  Serial.print("WebServer::showNotFound(");
  Serial.print(uri);
  Serial.print("): called");
  Serial.println();
#endif
  String content = "<h1>Ooopsie... </h1> <p>Well, we looked everywhere, but we don't seem to have found that. Sorry.</p>";
#if _XDEBUG_
  Serial.println("WebServer::showNotFound(): responding");
#endif

  serverResponse(404, "text/html", htmlPage("Ooops", content));
#if _XDEBUG_
  Serial.println("WebServer::showNotFound(): complete");
#endif
}

/*******************************************************************************************************************************************
       _                    _____ _        _
      | |                  / ____| |      | |
   ___| |__   _____      _| (___ | |_ __ _| |_ ___
  / __| '_ \ / _ \ \ /\ / /\___ \| __/ _` | __/ __|
  \__ \ | | | (_) \ V  V / ____) | || (_| | |_\__ \
  |___/_| |_|\___/ \_/\_/ |_____/ \__\__,_|\__|___/

*/

#ifdef ESP32
void showStats(AsyncWebServerRequest * request) {
  __request = request;
#else
void showStats() {
#endif
#if _DEBUG_
  Serial.println("WebServer::showRoot(): called");
#endif
  String title;
  String status;
  String text;
  String content = "<h1>Data Snapshot</h1>";

  title = "NET Timestamp";
  status = "green";
  text = NET.getTimestamp();
  if (text.length() == 0) {
    status = "disabled";
    text = "[not configured]";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "RTC Timestamp";
  status = "green";
  text = RTC.getTimestamp();
  if (!RTC.isEnabled()) {
    status = "disabled";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "GPS Timestamp";
  status = "green";
  text = GPS.getTimestamp();
  if (!GPS.isEnabled()) {
    status = "disabled";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "amber";
    text = "[not locked]";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "IMU Temperature";
  status = "green";
  text = String(IMU.getTemperature());
  if (!IMU.isEnabled()) {
    status = "disabled";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  } else {
    text += " C";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "BMP Temperature";
  status = "green";
  text = String(BMP.getTemperature());
  if (!BMP.isEnabled()) {
    status = "disabled";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  } else {
    text += " C";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "BMP Pressure";
  status = "green";
  text = String(BMP.getPressure());
  //  Serial.print("BMP pressure: ");
  //  Serial.print(text);
  //  Serial.println();
  if (!BMP.isEnabled()) {
    status = "disabled";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  } else {
    text += " hPa (";
    text += BMP.getSeaLevelPressure();
    text += " MSL)";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "BMP Altitude";
  status = "green";
  text = String(BMP.getAltitude());
  if (!BMP.isEnabled()) {
    status = "disabled";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  } else {
    text += " m";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "GPS Altitude";
  status = "green";
  text = "";
  if (!GPS.isEnabled()) {
    status = "disabled";
    text = "[disconnected]";
  } else if (!GPS.isConnected()) {
    status = "amber";
    text = "[not locked]";
  } else {
    text = String(GPS.getAltitude());
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "GPS Location";
  status = "green";
  text = "";
  if (!GPS.isEnabled()) {
    status = "disabled";
    text = "[disconnected]";
  } else if (!GPS.isConnected()) {
    status = "amber";
    text = "[not locked]";
  } else {
    text += GPS.getLatitude();
    text += " N, ";
    text += GPS.getLongitude();
    text += " E";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "GPS State";
  status = "green";
  int siv = GPS.getSatsInView();
  text = String(GPS.getRefreshRate()) + " Hz, " + String(siv) + " lock" + ((siv == 1) ? "" : "s");
  if (!GPS.isEnabled()) {
    status = "disabled";
    text = "[disconnected]";
  } else if (!GPS.isConnected()) {
    status = "amber";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";



#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showRoot(): responding");
#endif

  serverResponse(200, "text/html", htmlPage(String(_AP_NAME_) + " - Stats", content));

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showRoot(): complete");
#endif
}

/*******************************************************************************************************************************************
       _                          _                 _
      | |                   /\   | |               | |
   ___| |__   _____      __/  \  | |__   ___  _   _| |_
  / __| '_ \ / _ \ \ /\ / / /\ \ | '_ \ / _ \| | | | __|
  \__ \ | | | (_) \ V  V / ____ \| |_) | (_) | |_| | |_
  |___/_| |_|\___/ \_/\_/_/    \_\_.__/ \___/ \__,_|\__|

*/

#ifdef ESP32
void showAbout(AsyncWebServerRequest * request) {
  __request = request;
#else
void showAbout() {
#endif
#if _DEBUG_
  Serial.println("WebServer::showAbout(): called");
#endif
  String content = "<h1>About ";
  content += _AP_NAME_;
  content += "</h1>";
  content += String("<p>Application Version: ") + VERSION + "</p>";
  content += String("<p>Application Build: ") + __DATE__ + " @ " + __TIME__ + "</p>";

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showAbout(): responding");
#endif

  serverResponse(200, "text/html", htmlPage(String(_AP_NAME_) + " - About", content));

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showAbout(): complete");
#endif
}

/*******************************************************************************************************************************************
       _                   _____             _
      | |                 |  __ \           | |
   ___| |__   _____      _| |__) |___   ___ | |_
  / __| '_ \ / _ \ \ /\ / /  _  // _ \ / _ \| __|
  \__ \ | | | (_) \ V  V /| | \ \ (_) | (_) | |_
  |___/_| |_|\___/ \_/\_/ |_|  \_\___/ \___/ \__|

*/

#ifdef ESP32
void showRoot(AsyncWebServerRequest* request) {
  __request = request;
#else
void showRoot() {
#endif
#if _DEBUG_
  Serial.println("WebServer::showRoot(): called");
#endif
  String content = String("<h2>") + _AP_NAME_ + "</h2>";
  String status = "";
  String text = "";

  status = "red";
  text = "DISABLED";
  if (LOG.isEnabled()) {
    if (LOG.isCapturing()) {
      status = "green";
      text = "LOGGING";
    } else {
      status = "amber";
      text = "IDLE";
    }
  };
  content += "<div class='status-wrapper'><div class='label'>LOG</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  status = "red";
  text = "DISABLED";
  if (GPS.isEnabled()) {
    status = "amber";
    text = "NOT LOCKED";
    if (GPS.isConnected()) {
      status = "green";
      text = "LOCKED";
    };
  }
  content += "<div class='status-wrapper'><div class='label'>GPS</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  status = "red";
  text = "DISABLED";
  if (BMP.isEnabled()) {
    status = "green";
    text = "OK";
  };
  content += "<div class='status-wrapper'><div class='label'>BMP</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  status = "red";
  text = "DISABLED";
  if (IMU.isEnabled()) {
    status = "green";
    text = "OK";
  };
  content += "<div class='status-wrapper'><div class='label'>IMU</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  status = "red";
  text = "DISABLED";
  if (RTC.isEnabled()) {
    status = "green";
    text = "OK";
  };
  content += "<div class='status-wrapper'><div class='label'>RTC</div><div class='status value status-" + status + "'>" + text + "</div></div>";

#if USE_SERVO
  status = "disabled";
  text = "DISABLED";
  if (SRV.isEnabled()) {
    if (SRV.isArmed()) {
      status = "green";
      text = "ARMED";
    } else {
      status = "amber";
      text = "DISARMED";
    }
  };
  content += "<div class='status-wrapper'><div class='label'>SRV</div><div class='status value status-" + status + "'>" + text + "</div></div>";
#endif

#if USE_LIPO
  status = "disabled";
  text = "DISABLED";
  if (BAT.isEnabled()) {
    double pcnt = BAT.getCapacityPercent();
    double volt = BAT.getCapacityVoltage();
    if (pcnt > 0.5) {
      text = String(volt) + "v, " + String(pcnt) + "%";
      if (pcnt < 10) {
        status = "red";
      } else if (pcnt < 30) {
        status = "amber";
      } else {
        status = "green";
      }
    }
  };
  content += "<div class='status-wrapper'><div class='label'>BAT</div><div class='status value status-" + status + "'>" + text + "</div></div>";
#endif

  content += "<div style='clear:both'></div>";
  content += "<div id='actions'>";
  if (LOG.isCapturing()) {
    content += "<button onclick='window.location.href=\"/api/log/stop\"; hideActions();'>Stop Capture</button>";
  } else {
    if (LOG.isEnabled()) {
      content += "<button onclick='window.location.href=\"/api/log/start\"; hideActions();'>Start Capture</button>";
    }
    if (SRV.isEnabled()) {
      if (SRV.isArmed()) {
        content += "<button onclick='window.location.href=\"/api/srv/disarm\"; hideActions();'>Disarm</button>";
      } else {
        content += "<button onclick='window.location.href=\"/api/srv/arm\"; hideActions();'>Arm</button>";
      }
    }
  }

  content += "</div>";

  String log_summary = LOG.getLogSummary();
  if (log_summary.length() > 0) {
    content += "<pre id='log-summary'>";
    content += log_summary;
    content += "</pre>";
  }
#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showRoot(): responding");
#endif

  serverResponse(200, "text/html", htmlPage(String(_AP_NAME_) + " - Home", content));

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showRoot(): complete");
#endif
}

/*******************************************************************************************************************************************
   _                       _                 _                     _ _
  | |                     (_)               | |                   | | |
  | |     ___   __ _  __ _ _ _ __   __ _    | |__   __ _ _ __   __| | | ___ _ __ ___
  | |    / _ \ / _` |/ _` | | '_ \ / _` |   | '_ \ / _` | '_ \ / _` | |/ _ \ '__/ __|
  | |___| (_) | (_| | (_| | | | | | (_| |   | | | | (_| | | | | (_| | |  __/ |  \__ \
  |______\___/ \__, |\__, |_|_| |_|\__, |   |_| |_|\__,_|_| |_|\__,_|_|\___|_|  |___/
                __/ | __/ |         __/ |
               |___/ |___/         |___/

*/
#ifdef ESP32
void handle_logStart(AsyncWebServerRequest * request) {
  __request = request;
#else
void handle_logStart() {
#endif
#if _DEBUG_
  Serial.println("WebServer::handle_logStart(): called");
#endif
  LOG.capture(true);
  goToUrl("/");
}

#ifdef ESP32
void handle_logStop(AsyncWebServerRequest * request) {
  __request = request;
#else
void handle_logStop() {
#endif
#if _DEBUG_
  Serial.println("WebServer::handle_logStop(): called");
#endif
  LOG.capture(false);
  goToUrl("/");
}

/*******************************************************************************************************************************************
*/
#ifdef ESP32
void handle_srvArm(AsyncWebServerRequest * request) {
  __request = request;
#else
void handle_srvArm() {
#endif
#if _DEBUG_
  Serial.println("WebServer::handle_srvArm(): called");
#endif
  SRV.arm(true);
  goToUrl("/");
}

#ifdef ESP32
void handle_srvDisarm(AsyncWebServerRequest * request) {
  __request = request;
#else
void handle_srvDisarm() {
#endif
#if _DEBUG_
  Serial.println("WebServer::handle_srvDisarm(): called");
#endif
  SRV.arm(false);
  goToUrl("/");
}











#ifdef ESP32
void showFavIcon(AsyncWebServerRequest * request) {
  __request = request;
#else
void showFavIcon() {
#endif
#if _DEBUG_
  Serial.println("WebServer::showFavIcon(): called");
#endif
  char image[] PROGMEM = "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";
  //64x64 fails :( char image[] PROGMEM = "iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABGdBTUEAALGPC/xhBQAACklpQ0NQc1JHQiBJRUM2MTk2Ni0yLjEAAEiJnVN3WJP3Fj7f92UPVkLY8LGXbIEAIiOsCMgQWaIQkgBhhBASQMWFiApWFBURnEhVxILVCkidiOKgKLhnQYqIWotVXDjuH9yntX167+3t+9f7vOec5/zOec8PgBESJpHmomoAOVKFPDrYH49PSMTJvYACFUjgBCAQ5svCZwXFAADwA3l4fnSwP/wBr28AAgBw1S4kEsfh/4O6UCZXACCRAOAiEucLAZBSAMguVMgUAMgYALBTs2QKAJQAAGx5fEIiAKoNAOz0ST4FANipk9wXANiiHKkIAI0BAJkoRyQCQLsAYFWBUiwCwMIAoKxAIi4EwK4BgFm2MkcCgL0FAHaOWJAPQGAAgJlCLMwAIDgCAEMeE80DIEwDoDDSv+CpX3CFuEgBAMDLlc2XS9IzFLiV0Bp38vDg4iHiwmyxQmEXKRBmCeQinJebIxNI5wNMzgwAABr50cH+OD+Q5+bk4eZm52zv9MWi/mvwbyI+IfHf/ryMAgQAEE7P79pf5eXWA3DHAbB1v2upWwDaVgBo3/ldM9sJoFoK0Hr5i3k4/EAenqFQyDwdHAoLC+0lYqG9MOOLPv8z4W/gi372/EAe/tt68ABxmkCZrcCjg/1xYW52rlKO58sEQjFu9+cj/seFf/2OKdHiNLFcLBWK8ViJuFAiTcd5uVKRRCHJleIS6X8y8R+W/QmTdw0ArIZPwE62B7XLbMB+7gECiw5Y0nYAQH7zLYwaC5EAEGc0Mnn3AACTv/mPQCsBAM2XpOMAALzoGFyolBdMxggAAESggSqwQQcMwRSswA6cwR28wBcCYQZEQAwkwDwQQgbkgBwKoRiWQRlUwDrYBLWwAxqgEZrhELTBMTgN5+ASXIHrcBcGYBiewhi8hgkEQcgIE2EhOogRYo7YIs4IF5mOBCJhSDSSgKQg6YgUUSLFyHKkAqlCapFdSCPyLXIUOY1cQPqQ28ggMor8irxHMZSBslED1AJ1QLmoHxqKxqBz0XQ0D12AlqJr0Rq0Hj2AtqKn0UvodXQAfYqOY4DRMQ5mjNlhXIyHRWCJWBomxxZj5Vg1Vo81Yx1YN3YVG8CeYe8IJAKLgBPsCF6EEMJsgpCQR1hMWEOoJewjtBK6CFcJg4Qxwicik6hPtCV6EvnEeGI6sZBYRqwm7iEeIZ4lXicOE1+TSCQOyZLkTgohJZAySQtJa0jbSC2kU6Q+0hBpnEwm65Btyd7kCLKArCCXkbeQD5BPkvvJw+S3FDrFiOJMCaIkUqSUEko1ZT/lBKWfMkKZoKpRzame1AiqiDqfWkltoHZQL1OHqRM0dZolzZsWQ8ukLaPV0JppZ2n3aC/pdLoJ3YMeRZfQl9Jr6Afp5+mD9HcMDYYNg8dIYigZaxl7GacYtxkvmUymBdOXmchUMNcyG5lnmA+Yb1VYKvYqfBWRyhKVOpVWlX6V56pUVXNVP9V5qgtUq1UPq15WfaZGVbNQ46kJ1Bar1akdVbupNq7OUndSj1DPUV+jvl/9gvpjDbKGhUaghkijVGO3xhmNIRbGMmXxWELWclYD6yxrmE1iW7L57Ex2Bfsbdi97TFNDc6pmrGaRZp3mcc0BDsax4PA52ZxKziHODc57LQMtPy2x1mqtZq1+rTfaetq+2mLtcu0W7eva73VwnUCdLJ31Om0693UJuja6UbqFutt1z+o+02PreekJ9cr1Dund0Uf1bfSj9Rfq79bv0R83MDQINpAZbDE4Y/DMkGPoa5hpuNHwhOGoEctoupHEaKPRSaMnuCbuh2fjNXgXPmasbxxirDTeZdxrPGFiaTLbpMSkxeS+Kc2Ua5pmutG003TMzMgs3KzYrMnsjjnVnGueYb7ZvNv8jYWlRZzFSos2i8eW2pZ8ywWWTZb3rJhWPlZ5VvVW16xJ1lzrLOtt1ldsUBtXmwybOpvLtqitm63Edptt3xTiFI8p0in1U27aMez87ArsmuwG7Tn2YfYl9m32zx3MHBId1jt0O3xydHXMdmxwvOuk4TTDqcSpw+lXZxtnoXOd8zUXpkuQyxKXdpcXU22niqdun3rLleUa7rrStdP1o5u7m9yt2W3U3cw9xX2r+00umxvJXcM970H08PdY4nHM452nm6fC85DnL152Xlle+70eT7OcJp7WMG3I28Rb4L3Le2A6Pj1l+s7pAz7GPgKfep+Hvqa+It89viN+1n6Zfgf8nvs7+sv9j/i/4XnyFvFOBWABwQHlAb2BGoGzA2sDHwSZBKUHNQWNBbsGLww+FUIMCQ1ZH3KTb8AX8hv5YzPcZyya0RXKCJ0VWhv6MMwmTB7WEY6GzwjfEH5vpvlM6cy2CIjgR2yIuB9pGZkX+X0UKSoyqi7qUbRTdHF09yzWrORZ+2e9jvGPqYy5O9tqtnJ2Z6xqbFJsY+ybuIC4qriBeIf4RfGXEnQTJAntieTE2MQ9ieNzAudsmjOc5JpUlnRjruXcorkX5unOy553PFk1WZB8OIWYEpeyP+WDIEJQLxhP5aduTR0T8oSbhU9FvqKNolGxt7hKPJLmnVaV9jjdO31D+miGT0Z1xjMJT1IreZEZkrkj801WRNberM/ZcdktOZSclJyjUg1plrQr1zC3KLdPZisrkw3keeZtyhuTh8r35CP5c/PbFWyFTNGjtFKuUA4WTC+oK3hbGFt4uEi9SFrUM99m/ur5IwuCFny9kLBQuLCz2Lh4WfHgIr9FuxYji1MXdy4xXVK6ZHhp8NJ9y2jLspb9UOJYUlXyannc8o5Sg9KlpUMrglc0lamUycturvRauWMVYZVkVe9ql9VbVn8qF5VfrHCsqK74sEa45uJXTl/VfPV5bdra3kq3yu3rSOuk626s91m/r0q9akHV0IbwDa0b8Y3lG19tSt50oXpq9Y7NtM3KzQM1YTXtW8y2rNvyoTaj9nqdf13LVv2tq7e+2Sba1r/dd3vzDoMdFTve75TsvLUreFdrvUV99W7S7oLdjxpiG7q/5n7duEd3T8Wej3ulewf2Re/ranRvbNyvv7+yCW1SNo0eSDpw5ZuAb9qb7Zp3tXBaKg7CQeXBJ9+mfHvjUOihzsPcw83fmX+39QjrSHkr0jq/dawto22gPaG97+iMo50dXh1Hvrf/fu8x42N1xzWPV56gnSg98fnkgpPjp2Snnp1OPz3Umdx590z8mWtdUV29Z0PPnj8XdO5Mt1/3yfPe549d8Lxw9CL3Ytslt0utPa49R35w/eFIr1tv62X3y+1XPK509E3rO9Hv03/6asDVc9f41y5dn3m978bsG7duJt0cuCW69fh29u0XdwruTNxdeo94r/y+2v3qB/oP6n+0/rFlwG3g+GDAYM/DWQ/vDgmHnv6U/9OH4dJHzEfVI0YjjY+dHx8bDRq98mTOk+GnsqcTz8p+Vv9563Or59/94vtLz1j82PAL+YvPv655qfNy76uprzrHI8cfvM55PfGm/K3O233vuO+638e9H5ko/ED+UPPR+mPHp9BP9z7nfP78L/eE8/stRzjPAAAAIGNIUk0AAHomAACAhAAA+gAAAIDoAAB1MAAA6mAAADqYAAAXcJy6UTwAAAAJcEhZcwAACxMAAAsTAQCanBgAABqISURBVHic7VtpdBRV2n6quqr3vTud7uwbWSCBsISwhFUW2fcPGRBQxGUYBVERcVBR0WEUBUZBXBhFQQQVRASEQXYIJIQACUmALJ093Z1O71t1V30/YiIRRP0OyHfO+Jxzz6nT9973Pu9TVXd532qC4zj8N4O82wTuNv4U4G4TuNv4U4C7TeBu408B7jaBuw0KAAiCuNPjyABECwQCrUyikLXYm68KKImYRxORFE3SXp/HKaPDRX4vc8HBNNSg9cZwP5Y7CupODwAKfXPSJryoF2SkO1u8VSJoQt7wBpFAStRSAYVIJlb1oiVBQsLXtOzJ2/gW47VWdjYMSDdaiqpIflDh8wXcDrvjAADrnaBHcBx3R5+AXprZ2zI1U6ZbbeYTSgNdXmctbwy5BF2EhPJeX8h2WcBq5V5XwBAWJRFw2roqiVimZoM8uc1p4rTySEIoEuBUxedbzhsPPAbAebv53WkB9N2w6E0Z4jIZQbWEIZ3RLBinxeuyxGqig3KtiH+mrCQmRqQuszK2RDIoE2r5ChcpDjBypTxQY2yx6pFCiMSi8JP+l/s4g41XbjfBO/oK9IyY8nyqqpOqsLiCGjY+/tuZT/XJr77S0kkkpoPGMpPDHwgKJ6nTDXazrzk8Shn0+XzqqlIzTxOmqqdIvq+q1JRRcPxacsAWHSsJxg5y4g8WIExpuF+vjs+i+RSnUej1+WU/7GyxWS4AsAAw/5pxqSs1gRBLR5PwYPYTI/7SIzu5ICu7vVr+4/gtABQA3ABCANRonfwIl9OjKLl4TfrpqqK3C781TAfwwf/V0V/CLwoQoU16anTSkqfNRp8n5GQTJB45eisSJ9OGkM8RaCw+Vrl5Plhc+qX+PAgok8Oiq3O4IZeF3jfEaAp+1sRx3bXtumtL24VUJrZopBFQKEv2hyFymQvy5AAct/UpuKkAMn74lKHapcurLtk4Ut9QKpQQThvjEymkaiXrF9sJc0x2pjT4z0LH1skAvACg0+lA0zSam5vB0T4InNExifHRVWZ7bezI8QNOGGI07fYvXLiALVu2QCKR3JSUw+HA3LlzkZGRAbGSlmYPj9106Vjz+Dpj9JAAim+/AC+99FL7D3Idgfyv2KjPDv1H8eKCe9a89M5Tb7NcUEESVCjEMWRdTb26JL8hcu0jgeWF2N0bcB0FAI1Gg/z8fIjFYgDA4Z1Foq/Xnz0Mo69s8NTk4wBQU1ODhQsXYufOnb9K7NNPP8XRo0eRlpbmGnt/H9eZb8wlB42qMQA23k4BwHHcDeXMkeKBkRhbtHrJ9rGtv7FguVCHNiOTl28dO+TBCSNGDWq3dfny5fZ6lgvQc0a+mmTAw8tyj1xQNjsaoNVqb8qBpuheIDCQJyC6ECTCr68bM34Uaoy1qCtzj9ZgZj2gMEhlfEgkEqjV6l98in4rCI7jsGLFCvj9fkgkEsyfPx86nQ5uC5cUYgih3ICiJUuWIC8vD9nZ2cjOzsakSZPgtgWzJErKfOzED1WDBtwDAMjMzEReXh4oqvXNOrSzIIsXkKcMnp70WVbvnsjPKwBaJzmtThU9SCIVhzU2NFUPTpwni9In5pw8ceK8j2spIqTOuDo218p4OVkoFMrrlplxsfD8RVXlOcccnUG6TxJBlrU5MGPGDGzbtu3X/OT9WFgAwRsE+Pk+IKtvJh549D4AoD/78Gvq1PGzbVtSDgDSu6fxlr/6ZGDHtm/oLz/9TonWScwLAH/5y18wc+ZMjB49GgDIL3ZuZh95+DGp3eIZkxCTktlFP2SKXBRGyih9QCWJSKs3VVo4bf3fk5NT/Id3FepcbFM96xaH6xVxU6Mi4vpcrblYd7zykzWGVOrz51c8U8fYBJTPyYakcgnX0NCA1157DX6/v80d8AmpmiX9KSpJeIZcqo4OD9NHcwTLCWixHF5RbLHx9Fdmp3EdWledGwSQkiQG8ImwiYGQIIeFuVFKxEjD+ZEid8BDcxzr8RA+t5Nr0gFmPiBAAj32/ekLMv6R0TcBJ47mYv369aApGnwBjSDpJf0ebnA0P+uZ7LhpSRpJdISnhTVZzfZwu99MevwMXytVEbQiAJPDGEqI6QSBiMdrdjbUGsTppb6woj31FyGT+uN6Ftn21lVw3+4CcLqN/M8gAsCGU90NBmSNVCoVo3XqiGQFPyqV4wUhFajQbGnO94iqLpqDRUnHyj5fAKCowyogF4QPGJe2dK+AliIyQVXqdNsz/J6QMDpJUx/fWR/0OALCuiqzKL13vJvikV6b2evrOzy1IKlX6wxfdKEUAMAEGTBBhidCxMyx8Y8uEjDaJsc1rqGerdE7Q2YzQbe4pOHgiQlWXm+/Qok9UrtYLFYVVV80BTzQKWmVyqe8PIyp9/aQykVGuT50OZbtXqCxp4nv+ZuaCE8DSi9WIiMjA4FAAEHCg+Pbq2JzT1c+7gg2O/QaVEtE4uiiq4XRQS7P6odH3AifUAJZTC9NV3+SeorMoQ3cX2j56qUOAmiYbsObi2Tgwo2vz3k+Z0+nnhqv8VqDvP6aw9tncHqpUEpRar0Ybhvj5NFESCihWAAEAOLhhx/mPvigdZ8ioEUZ/bqMWZIgywkry/W0SDQWlV/fEt1YUyPtkh4TljM65/1OnWOsE+fkXCi/UmPwegJEeIS6+ezh0ozLBRWJLo+DLc6rFtaWsqlSnqJzvdncnRQHxriEea/nVzLfJYU6Y/v2L5GcnIzFixcDABY/DEt5of3U5fNVaclZYY7k3urn8k4UJVqaWvyGaK3f42B6uJtDho9fyIuuqLwW5mHUqTOm3S/s8AokYMI2Awb0MaH8uRc+GHFo1kMTTSGGIx1WL9/r8fvEEgGUOjFcdh8omgehmAYAzJ8/Hx9++GGrijSSUugxa3Li7h/caK2mAnBU6OMUuXyNm1mwYkJe956ZBaFgyMSjeLU3eYyB1qMzVXKupsuJfZd6f7ph7+Agw1H9+vShy3LNwwo9WzbVugsWA7ADwLx5834a+xaw2Btw+vRJLBuz9w0Nm/n0VeR+kzXBOxHX5wU6YcbFGVFfHO5L//1JAPJde7+46TLZVtasWQO1Wt3eX8JXZt2jX9r02oyDX8xI3/CxATOqp/b8+7sfvrZvsanelnkrW79QxNZm69hP/nngzamdX/vHg33Wvzc7aQvXI3bUDgDKtnHnz59/Sztbt25t55hOzPtyALHW0h2LcyV8pbY1IkSACOOnxmjRVUPqazwyPeMGSMHE0dPx5Zdf3lTRBx98EIsWLYLV2npMFwqFYwfonniDY7mVZqai8HxRyaQpk3tt37j/qe3znrv3ozCDovBXb9ON8KjUqj2znxn+/EMvDHczPLuryVv2Vaxn6tTMyDEftImwefNmrF27FgzD3NTIpk2bAABddaMT+Vx8l/T+qnKS9tVldu3pAcdxiFP1eSBb/rdXcgQvHBwsW8H1EizcQEEibDOwcePGDoquXr0afD7/+jGogRELDs/vtm1PP938JwyY7X558Uevchxn+D/c9V8sprqW3g+NXvXcQM2zp2dEbOEiZOlzriehVCqxevXq9vZNTU24fPkyzp07h/KayxiYOUEch8cqp+g/2nSP4pV3ASqNAoDNW96/+OqsPQqbwyNl/XSLgJa3iMUCMkQzcNsDeOSRR5CQkIBhw4Zh1qxZ2LJlSweFNWTKHE2gp08U5tvbfEHy7uhRSR8tX/3Ai2g93d02hEUoz77xyQJu9bNb/Sd2XSWiuVEf+2SmFqvTtBsAbDYbnnrqKeTm5mLo0KF499134ff7odPp4PCZYbzgj0hEuggCH8cPilKkiCEpAJj98OS6bpHTqu0BR6XPpdGrZLwztVfrhDK10NM2eCAQQGpqKsrKyjqQIikkpkcNmGm2le0++x/b06NHxu99f+/ipQBxW51vg1IryZswa7DbavlWJ/Z1jXUesL5kxUf7AQTa2uzYsQN6vR4HDx5EdXU1WJZFTIoGG5855f3XR0eNXTRhjc0mxjLtnnkuCgBSo/opruZaF8ulYlrBk8e5ghX06rX/tPJFAOPnQNM0GIZBQ0PDDYSiiAGz4qnhScX+U+OECHmmzh/4KEBZbmh4G9FrSKcKhWr6V7knS88fPxr9b00ocUpzsPzz69ts27YNJpMJLMuCZVkI5CFUHuMxqcgM1lQ2xZ3zXmoZN2iCkQKAsqJyeu6MJZtqK6z35Z+pBuURjnjllZePsFzoVx3R8FISasotlSGOb1jwfPZ7I6b0rblTjl8HX5hBUzR6Up+CypPuB97//PLrQPl3uC7G4PF4IJPJQFEUgsEgxCoCtSIb44CPjpTGTMkI+H545513+BQA+FyiLiaHyXjFfMUfAt/GBWE7fSJf2Lt/ZvuI69atw8KFCzuwkMDQN1aTPtRYV6/Xp0r/vfDlaev+AOcBADyly7t93SEExewGHVLf8yOtnwUl+9vq3W73DfsDA9E7LoXuXWWsqcmiKcnp1LiBrfEAEky4o5JMSzBE7zpdUTuWQHOt1+epBQCXywWpVHrdgeMnqJGittZxviDsyOzZ4yRJ3vkoextkAi2GjhykKlU3HSGaVWXrdh2dj+sEUKvVePrpp8GyLPh8PozmYhxcYxOFGK5OQAuOuBib6mrtnigKAF5Z/hrvsy1bzCoe6Y1EF1SjMfbeCfdArVIgGOAgl8vBsuwNJGg+recCRJJSy9/0+JsjPwFAA7j5Yvw7cObMGXz88ccQCAQ3jVizLAufz4/e/TPt48dPYc8dqtosQOy868cXiURYunRph/6Pla1iTu1uzEo2dPI3NIaSmu31cgoAEgax/7r6avkDCS09twilfihcOnL7kbPI7JXR3nnUqFGoqKjoQESOxCSNVIOTlu/9EyaNw2OP/pWxWmy3dM5utyMzMxOTJ0++af369euxYMGC3yTU1s1q/TOi5xCy6Y1JGB9uAiHiwDEAUFdXh4kTJ2LEiBFoampCbKIBTaGL9gDU/TkWHkug5dKDU54MUABw9XJNsJt+WFh9gwcWVMOQ6r54OHcvdu/7GjTFh9/vx8mTJzsMHiZOnCaShDyNtkt7RJAlTZ02AAMHDIDZdOsEjt/vR3h4+C/Wjxw5EtXV1VCr1R226T+HVCrF559+ZX1hzpZP9fGS81w9fZX2qSIDsLZPhLt378bu3bvb+xjILDqeN7bC0miX+OAPZyjWQgFAXFc5GOVpa6hBAxnkhY1XneTSxS/pA4yv8ZcIKLlOaXGRKQ84+AFxRV3z1sSUKCQmJCMx4Zb+d0AoFILH07rVEIvF4PF4SExM/M39+RqPz8vR+vCwiIDZ7ivW+DqPasCJkrZ6hUIBpVKJYDCIANWMcHuSlG9THiH4/mmRPH3L9i/W+0gAeO/D9/B9yf5O4XwNBEKe0x7yxax5Y72D4ziEQiHY7fZ2YlKpFJQUsHuttYHqiDjGrNCJSNRFx0QDaF1+nE4nXC7XLxa3uzWesWLFCsjlcsjlchw4cOC3K/cj4mLiQcFvO3zmnNfY3CzQKjXKpSsex8qVK7Fw4ULs2LEDZWVlyM/Ph6nKizEzs6wVuCiWCRUyZ8hrFfGj4ikAqLxWAQkUDhCEUyFV9eT7mKrYTnoPAJAkCblcjmvXrnUY/OWFG6veXbcXFMSYPuV/jmtkeix7fim2btkGo9EIqVR6S/IEQbTffQBYtGgRiouL4fF4bvnot0EqlaKysRgUiMpwKo30Bt0mgZSqfPmFl0H/eFBcuHAhRowY0d5HhDC2v2zxcIunATKlLGpkxrhmCgCiI+JhAkEHSY+tzmKPpiVe2/niUygsLEAgwIAgCOj1etA0DZIkQfIZfHX4kyoJMoMpdCa1d9+3MW/vfuTM9fO/y+X6VSeux5UrV/DMM8/8rj4AoMKItHXPzas9fPaw1OU1cfRPp2TMmDEDMTExUCqVUEXQ2L+uQb9/fxOiZKR99qKBr3635/shFABkD+yG/K+Pe/xe0iNX8Y7JkFa+fMmbEg6+1sAhQWD37t1ISkpCS0sLDHEKCByJmqWPfenycE6lmkubQTAHjnAIWPAH5PTbQIJHyBAjq2optD3y/PDvBTzpTWPkLMdCQEnAE7ClBBg/Lyiv/GrHt7mmYtFbrTtBO4d5s/5a+/Vn5y0RjK7ayzgtCpFS5aPMbr+LhUwmQ69evaDX69uNxj3aOe/LD45uLyuofZgLhiTJ0iEJLOVNvGo7lvsH+Q8hocqIEcRplr/zQXOI77z04urHWbR+XMECwOeff451637anEYS/bNSeNMqrN6mHFUoJl1nCG4nAYDxkvjumwMFYWK53OXyjQoQDlFlbZnF6wiCZVnY7fYOzreh970Zx2Ra/kEKgsgwdfhgnTx+HAHqxoZ3CDxO3NnrC2jffXKV8YFHp9M/hvzbd2xr1669Lp7A4v77ZykaQ6YwAcRMyZWiMIVCZKcAYNJ9I3Fq+8c9rBXOeDEUaPJVitdu+KevX58BsLc4YbVa0a1bN2RnZ3cg0Kd/57oju4rq4iKUlvpK8aywJF5ziq9vS6np+Jt/hABa9H8yKUN2QpBQl0+K4onrU50mkwmbNm1qD9yERUlAi7E3BM88sVwYy7nixwyZmLmBAoDXV62EQp3mVQR1O0MhJtJYJ4kKumjo9BoQHA98Pv+mKagBozufKcnPTq2raaquNJpeJG2RA7NiJyeVmXIPcmAu3EnnJbQqsRPdR3fy0rF9Xzy+XHew8y5TVFRke/2qVavw1ltvdegTQfSPjca9jVIFbSZF/q8tdc5qEgCqKhvwQ/6FSIqlC4mgsEHLE9n+8eYryEzPwvDhwzFhwgT069cPTz/9NILBDpklr86gPaGV62zdRoQdO1ZZQPCtMZGDU2a9AOCOvgrJqiHrxFK+NF6YwR7adTJu2NBBHerb9hrXQ0AK0+RCdUZ1TYuuqclrmPLgwDIKAEYOG47LuUdi7C6b2GHzdlJJI/H2q+9AFS4EnydBWVkZli9fjtWrVyMjIwNz5vwUihv3YM9rPB7JP3simtqx4/isC+WF+uSYIZOj+CUFtYHclXfCeYOq00MRwcGjr1hLjs9dOCB36IQeZ9Gan+AYhsGyZcuwceONSWRfKBCK7aJkjRfLBPc93LUofbDWTQLAxEnjoINa4rcpo3hQCa2h8qIAHBAKJDAYDB3UnDt3LtLT03HkyBEAAI9H+gA4eudkHHti4ZgNhciHyWLeliDtH0/w0Pk2+SzXKPXT5UJtTrg2cm4Kpm8McM4ryV2U+2Y8MaTthMYBwNatW/HmmzefgljwhCI1dyBCFN60+ZPP2JPn/9M6azSYaxGCP98LIysgJfqgVyB8avGz4FFAKHCjoeLiYgwZMgQ5OTkYOnQoevbsifHjx+OFt+auLSoyjt5z6MTwcbqhJ8bHL37j2LWP/91it948tv5bQCC6R9SYDSniMdpa7sTX2mD3lYkJiQeLrpwvHjV1QGlUguYa0LrxWr58OdasWXNTM2OnDQZ1NYs+dKQkohblzmcXz2zo331k6/cBhBiIwMDlEyP+VZGOhZfvjVg502K2YMLU0b+JY0xMDA4dOgSO49Dc3Jz2zJy387vhb8aZiZuvzc5e44/UJG8EEPs7XRcI+KKsNNGkT8aFrcsbEPXIyHGG1dYMLC7pG/7oK+8t/c+/OY6TcxwHhmHQq1evWxpb/o9F2PLGqfhkPP9xCh53qOSREXa3uTU77AmZ8ci4Nc+c2cctTJEkNF6VbH7LKS8stTexSf6A1xb0s0aS4AU5cDyOYxvQuuCmoDXsXY/W9Di7cuVKbtmyZUCIxZjeS9//oaBy3iDR0O8T0lXJRc377KVNJwosvoqNXAj5tyJLQxzTSzdzFQLCow63xRmbEN3d42ZGSklDdLmpqHDwvV2b1u98ajGAukAggH79+uHcuXMAAB6E4EGEAFrAhxI05CTNI/Qu2lgfRw4L03uGv1mK0tnZQ/1Rew5tqSPaDh6XzpfHjOnx2p5EYXKsNMrZ5PI5wlQynTIhNcLb4Cy7FGQDHgQEKqOxUltvqfq+T+Y9RmuNJ6uktrTOgspN4YbA2ddXr8DMGbMBAPnHS3I+eGvns7t2Xcke3qXvZ+HRcm+T0ZljZSr5l+3flnhYy36ny2kCL6QheaA4cAFwEEbpkrroqAxhsEVUJNXwoJLqH3RXyQcQQqZBFu3/JmdEV/ztpamrAFQxDIOMjIy2UL2cADWMT0j0FEQsTfEjSFYQ4PNETJQ6JcEVsGs8dn9zMBQzOSXSYPvm0ou9ZSpxC3H9yeuv49/YuuPb4wNZqJU94pLyYnUR1sKzjZN1cgnAY+F2+djoBJ25qdZ2Ni5Fu276or7KhNRIPkUT56O6CUpIgtdmigDABRi/asmsd177cnv+o3qhoSYlLeK9gId0Ex7J/eJIn94RrDtKeGURNE1HCMX8GIrkszKegUeLCOZS0/enBPb4Pu4mQl7qzi/v3j1+16LlM04PHNc1l6J4dUBrSmzOnDmgBSQE/rR+OfETdyt1tMYX8HJahd7j8jn5wZCfxwYJKAThhLnJSgh1vsoNexZP1Wq1BcCPH0i0gWGYhK83/zDH3GDN4PyCmsO7LiSfL7p0b0KncFPAGyQ0OsWVuMTI8527JleFvPwrA8d1PtmlT+QtQ0B2m9NwIb94+Hsv7Vty5mR5FzkpPc2yUrNKrh5OE0E/E8BlpdBg8LiDrD1goUO0M9LEtPDCSDVc/HpHUopqf79BPRrmL5n4lS5Sefx627W1tRCJhZBKpSg7bcuQa+kMbbQ4FPAzrFBKmUIMKwqFWAFINkjRhJtHkwRFCGpoit9+tiducvYmAHBBJmRosTUP4Ison4gvqSd4BEmRlJcgCC9AmAG48DtSX7YWW3LxhSt9j+8pGZV3rDSzpdFdJaaUboFQEH2u5HIWzaPt4QploSKC82liBEqOJcz3PXTvN1n9u5Zpw1SlJI+81YeZbZHP330SvZkAdxicDCC02zYcHrzvs4J4gZSVDf6fTkx1RZOovsxjz+iRXD1/2ZhdaI3wNuI25xd/jrsgQDuEgUAghWU5TigUlLIsJwHHKUke2QDA90eRuJsC/L/Af/1fZv4U4G4TuNv4U4C7TeBu479egP8F4ZJWLiW3XCUAAAAASUVORK5CYII=";
  int base64Len = strlen(image);
  int binaryLen = Base64.decodedLength(image, base64Len);

  char binary[binaryLen];
  Base64.decode(binary, image, base64Len);

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showFavIcon(): responding");
#endif

  serverResponse(200, "image/png", binary, binaryLen);

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showFavIcon(): complete");
#endif

}

/*******************************************************************************************************************************************
   _______   _____
  |__   __| / ____|
     | |_ _| (___   ___ _ ____   _____ _ __
     | | '__\___ \ / _ \ '__\ \ / / _ \ '__|
     | | |  ____) |  __/ |   \ V /  __/ |
     |_|_| |_____/ \___|_|    \_/ \___|_|
*/

TrWEB::TrWEB() {};

void TrWEB::begin() {
  // Public pages visible by default
  addPage("/", showRoot, "Home");
  addPage("/about", showAbout, "About");
  addPage("/stats", showStats, "Stats");

  // these pages should not be publically listed, so no title means hidden
  addPage("/favicon.png", showFavIcon);
  addPage("/api/log/start", handle_logStart);
  addPage("/api/log/stop", handle_logStop);

#if USE_SERVO
// Allow for dervo hndling if enabled
  addPage("/api/srv/arm", handle_srvArm);
  addPage("/api/srv/disarm", handle_srvDisarm);
#endif

  // Add a 404 handler
  registerNotFound(showNotFound);

  // Start the server
  serverBegin();

#if _DEBUG_
  Serial.println(String("     Web server: http://") + NET.getIpAddress() + "/");
  if (!NET.isApMode()) {
    Serial.println(String("                 http://") + NET.getHostname() + ".local/");
    //  Serial.println(String("                 http://") + NET.getHostname() + "/");
  }
#endif
}

void TrWEB::loop() {
  serverLoop();
}
