#include "WEB.h"

#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
AsyncWebServer WebServer(80);
#else
#include <ESP8266WebServer.h>
ESP8266WebServer WebServer(80);
#endif

//#include <Arduino.h>
#include <XBase64.h>

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

void registerStaticUri(String uri, String filename) {
  //WebServer.serveStatic(uri.c_str(), SPIFFS, filename.c_str());
}

#ifdef ESP32
void registerNotFound(ArRequestHandlerFunction func) {
  WebServer.onNotFound(func);
#else
void registerNotFound(void (*func)(void)) {
  WebServer.onNotFound(func);
#endif
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

    Serial.print("serverResponse(): ");
    Serial.print(len);
    Serial.print(" bytes being sent");
    Serial.println();
    Serial.flush();

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
  // TODO: This will probably be different on ESP32
#ifdef ESP32
#else
  WebServer.handleClient();
#endif
}

///*******************************************************************************************************************************************
//   _    _      _                     __                  _   _
//  | |  | |    | |                   / _|                | | (_)
//  | |__| | ___| |_ __   ___ _ __   | |_ _   _ _ __   ___| |_ _  ___  _ __  ___
//  |  __  |/ _ \ | '_ \ / _ \ '__|  |  _| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  | |  | |  __/ | |_) |  __/ |     | | | |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|  |_|\___|_| .__/ \___|_|     |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
//                | |
//                |_|
//*/
//
//// Convert hex character to integer
//unsigned char h2int(char c) {
//  if (c >= '0' && c <= '9') {
//    return ((unsigned char)c - '0');
//  }
//  if (c >= 'a' && c <= 'f') {
//    return ((unsigned char)c - 'a' + 10);
//  }
//  if (c >= 'A' && c <= 'F') {
//    return ((unsigned char)c - 'A' + 10);
//  }
//  return (0);
//}
//
//// Decode a URL string
//String urldecode(String str) {
//
//  String encodedString = "";
//  char c;
//  char code0;
//  char code1;
//  for (int i = 0; i < str.length(); i++) {
//    c = str.charAt(i);
//    if (c == '+') {
//      encodedString += ' ';
//    } else if (c == '%') {
//      i++;
//      code0 = str.charAt(i);
//      i++;
//      code1 = str.charAt(i);
//      c = (h2int(code0) << 4) | h2int(code1);
//      encodedString += c;
//    } else {
//
//      encodedString += c;
//    }
//
//    yield();
//  }
//
//  return encodedString;
//}
//
//// encode a string for URLing safely
//String urlencode(String str) {
//  String encodedString = "";
//  char c;
//  char code0;
//  char code1;
//  char code2;
//  for (int i = 0; i < str.length(); i++) {
//    c = str.charAt(i);
//    if (c == ' ') {
//      encodedString += '+';
//    } else if (isalnum(c)) {
//      encodedString += c;
//    } else {
//      code1 = (c & 0xf) + '0';
//      if ((c & 0xf) > 9) {
//        code1 = (c & 0xf) - 10 + 'A';
//      }
//      c = (c >> 4) & 0xf;
//      code0 = c + '0';
//      if (c > 9) {
//        code0 = c - 10 + 'A';
//      }
//      code2 = '\0';
//      encodedString += '%';
//      encodedString += code0;
//      encodedString += code1;
//      //encodedString+=code2;
//    }
//    yield();
//  }
//  return encodedString;
//
//}


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

void addStaticPage(String uri, String filename, String title, bool hidden) {
#if _XDEBUG_
  Serial.print("WebServer::addStaticPage(");
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
  // TODO: this will be different for ESP32
  registerStaticUri(uri, filename);
}
void addStaticPage(String uri, String filename, String title) {
  addStaticPage(uri, filename, title, false);
}

void addStaticPage(String uri, String filename) {
  addStaticPage(uri, filename, "", true);
}

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
  //  showNotFound(request);
  //  return;
#else
void showFavIcon() {
  //  showNotFound(r);
  //  return;
#endif
#if _DEBUG_
  Serial.println("WebServer::showFavIcon(): called");
#endif
  //  //  const char* image PROGMEM = "data:image/png;base64,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";
  //char image[] PROGMEM = "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";
  char image[] PROGMEM = "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";

  Serial.print("WebServer::showFavIcon(): base64 encoded string is ");
  Serial.flush();
  Serial.print(strlen(image));
  Serial.print(" bytes long");
  Serial.println();
  int base64Len = strlen(image);

  //  unsigned char binary[64 * 1024];
  //  size_t len = decode_base64(base64, binary);

  int binaryLen = XBase64.decodedLength(image, base64Len);

  Serial.print("WebServer::showFavIcon(): base64 decoded into ");
  Serial.print(binaryLen);
  Serial.print(" bytes");
  Serial.println();

  char binary[binaryLen];
  XBase64.decode(binary, image, base64Len);

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showFavIcon(): responding");
#endif

  serverResponse(200, "image/png", binary, binaryLen);
  //  showNotFound(request);
  //  return;

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
  //if (!SPIFFS.begin()) {
  //  _static_files = false;
  //#if _DEBUG_
  //    Serial.println("         SPIFFS: Disabled");
  //#endif
  //  } else {
  //#if _DEBUG_
  //    Serial.println("         SPIFFS: Enabled");
  //#endif
  //  }

  // Public static pages hidden by default
  addStaticPage("/favicon.ico", "/favicon.png");
  addStaticPage("/favicon.png", "/favicon.png");
  addStaticPage("/log_start.png", "/log_start.png");
  addStaticPage("/log_stop.png", "/log_stop.png");
  addStaticPage("/srv_arm.png", "/srv_arm.png");
  addStaticPage("/srv_disarm.png", "/srv_disarm.png");
  addStaticPage("/ajax-loader-bar.gif", "/ajax-loader-bar.gif");

  // Public pages visible by default
  addPage("/", showRoot, "Home");
  addPage("/about", showAbout, "About");
  addPage("/stats", showStats, "Stats");

  // these pages should not be publically listed, so no title means hidden
  addPage("/favicon.png", showFavIcon);
  addPage("/api/log/start", handle_logStart);
  addPage("/api/log/stop", handle_logStop);
#if USE_SERVO
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
