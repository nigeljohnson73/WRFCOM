#include "WEB.h"
TrWEB WEB;
TrWEB::TrWEB() {};

#if !_USE_WIFI_

void TrWEB::begin() {
#if _DEBUG_ && _DISABLED_DEBUG_
  Serial.println(String("WEB initialised: disabled");
#endif // _DEBUG_
}

void TrWEB::loop() {}

#else // !_USE_WIFI_

#include "lib/XBase64.h"
#include "lib/XBase64.cpp"

#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
AsyncWebServer WebServer(80);
#else // ESP32
#include <ESP8266WebServer.h>
ESP8266WebServer WebServer(80);
#endif // ESP32



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
#else // ESP32
void registerUri(String uri, void (*func)(void)) {
  WebServer.on(uri, func);
}
#endif // ESP32

#ifdef ESP32
void registerNotFound(ArRequestHandlerFunction func) {
#else // ESP32
void registerNotFound(void (*func)(void)) {
#endif // ESP32
  WebServer.onNotFound(func);
}

String getUri() {
#ifdef ESP32
  if (__request) {
    return __request->url();
  } else {
    return "/unknown";
  }
#else // ESP32
  return WebServer.uri();
#endif // ESP32
}

void serverResponse(int code, String type, const char * content, size_t len) {
#ifdef ESP32
  if (__request) {

#if _DEBUG_ && _XDEBUG_
    Serial.print("serverResponse(<binary data>): ");
    Serial.print(len);
    Serial.print(" bytes being sent");
    Serial.println();
#endif // _DEBUG_ && _XDEBUG_

    // This only sends a code 200. The input code is redundant
    AsyncResponseStream *response = __request->beginResponseStream(type, len);
    for (size_t i = 0; i < len; i++) {
      response->write(pgm_read_byte(content + i));
    }
    __request->send(response);
  }
#else // ESP32
  // TBD
#endif // ESP32
}

void serverResponse(int code, String type, String content) {
#ifdef ESP32
  if (__request) {
    __request->send(code, type, content);
    //serverResponse(code, type, (uint8_t *)content.c_str(), content.length());
  }
#else // ESP32
  WebServer.send(code, type, content);
#endif // ESP32
}

void serverLoop() {
#ifdef ESP32
  // It's all handled in the background
#else // ESP32
  WebServer.handleClient();
#endif // ESP32
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
#else // ESP32
void addPage(String uri, void (*func)(void), String title, bool hidden) {
#endif // ESP32

#if _DEBUG && _XDEBUG_
  Serial.print("WebServer::addPage(");
  Serial.print(uri);
  if (hidden) Serial.print(", hidden");
  else Serial.print(String(", '") + title + "'");
  Serial.print("): called");
  Serial.println();
#endif // _DEBUG_ && _XDEBUG_

  if (!hidden) {
    titles[pcount] = title;
    urls[pcount++] = uri;
  }

  registerUri(uri, func);
}

#ifdef ESP32
void addPage(String uri, ArRequestHandlerFunction func, String title) {
#else // ESP32
void addPage(String uri, void (*func)(void), String title) {
#endif // ESP32
  addPage(uri, func, title, false);
}

#ifdef ESP32
void addPage(String uri, ArRequestHandlerFunction func) {
#else // ESP32
void addPage(String uri, void (*func)(void)) {
#endif // ESP32
  addPage(uri, func, "", true);
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

  ret += "<link rel='shortcut icon' href='/favicon.png' type='image/png' />";
  ret += "<link rel='apple-touch-icon' href='/favicon.png' />";
  ret += "<style>";

  ret += "html, body {height: 100%;}";
  ret += "body {display:flex; flex-direction: column; background-color: #eee; font-family: Arial, Helvetica, Sans-Serif; color: #090; text-align: center;}";
  ret += "#main {flex: 1 0 auto; padding-bottom: 10px; padding-left: 30px;padding-right: 30px; }";
  ret += "img.responsive {max-width: 100%; height: auto; }";
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
#else // ESP32
void showNotFound() {
#endif // ESP32

#if _DEBUG_
  String uri = getUri();
  Serial.print("WebServer::showNotFound(");
  Serial.print(uri);
  Serial.print("): called");
  Serial.println();
#endif
  String content = "<h1>Ooopsie... </h1> <p>Well, we looked everywhere, but we don't seem to have found that. Sorry.</p>";
#if _DEBUG_ && _XDEBUG_
  Serial.println("WebServer::showNotFound(): responding");
#endif // _DEBUG_ && _XDEBUG_

  serverResponse(404, "text/html", htmlPage("Ooops", content));
#if _DEBUG && _XDEBUG_
  Serial.println("WebServer::showNotFound(): complete");
#endif // _DEBUG_ && _XDEBUG_
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
#else // EPS32
void showStats() {
#endif // ESP32

#if _DEBUG_
  Serial.println("WebServer::showRoot(): called");
#endif // _DEBUG_
  String title;
  String status;
  String text;
  String content = "<h1>Data Snapshot</h1>";

#if _USE_NTP_
  title = "NET Timestamp";
  status = "green";
  text = NET.getTimestamp();
  if (text.length() == 0) {
    status = "disabled";
    text = "[not configured]";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";
#endif

#if _USE_RTC_
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
#endif

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

#if _USE_IMU_
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
#endif

#if _USE_EMU_
  title = "EMU Temperature";
  status = "green";
  text = String(EMU.getTemperature());
  if (!EMU.isEnabled()) {
    status = "disabled";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  } else {
    text += " C";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "EMU Pressure";
  status = "green";
  text = String(EMU.getPressure());
  if (!EMU.isEnabled()) {
    status = "disabled";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  } else {
    text += " hPa (";
    text += EMU.getSeaLevelPressure();
    text += " MSL)";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "EMU Altitude";
  status = "green";
  text = String(EMU.getAltitude());
  if (!EMU.isEnabled()) {
    status = "disabled";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  } else {
    text += " m";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";
#endif

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
#endif // _DEBUG_ && _XDEBUG_

  serverResponse(200, "text/html", htmlPage(String(DEVICE_NAME) + " - Stats", content));

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showRoot(): complete");
#endif // _DEBUG_ && _XDEBUG_
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
#else // ESP32
void showAbout() {
#endif // ESP32

#if _DEBUG_
  Serial.println("WebServer::showAbout(): called");
#endif // _DEBUG_

  String content = "<h1>About ";
  content += DEVICE_NAME;
  content += "</h1>";
  content += "<div>";
  content += String("<p>Application Version: ") + VERSION + "</p>";
  content += String("<p>Application Build: ") + __DATE__ + " @ " + __TIME__ + "</p>";
  content += "<p>A Nigel Johnson <a href='https://github.com/nigeljohnson73/WRFCOM'>Github project</a>.</p>";
  content += "</div>";
  content += "<div>";
  content += "<img src='aboutqr.png' alt='QR Code for project' class='responsive'/>";
  content += "</div>";

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showAbout(): responding");
#endif // _DEBUG_ && _XDEBUG_

  serverResponse(200, "text/html", htmlPage(String(DEVICE_NAME) + " - About", content));

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showAbout(): complete");
#endif // _DEBUG_ && _XDEBUG_
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
#else // ESP32
void showRoot() {
#endif // ESP32

#if _DEBUG_
  Serial.println("WebServer::showRoot(): called");
#endif // _DEBUG_
  String content = String("<h2>") + DEVICE_NAME + "</h2>";
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

#if _USE_EMU_
  status = "red";
  text = "DISABLED";
  if (EMU.isEnabled()) {
    status = "green";
    text = "OK";
  };
  content += "<div class='status-wrapper'><div class='label'>EMU</div><div class='status value status-" + status + "'>" + text + "</div></div>";
#endif

#if _USE_IMU_
  status = "red";
  text = "DISABLED";
  if (IMU.isEnabled()) {
    status = "green";
    text = "OK";
  };
  content += "<div class='status-wrapper'><div class='label'>IMU</div><div class='status value status-" + status + "'>" + text + "</div></div>";
#endif

#if _USE_RTC_
  status = "red";
  text = "DISABLED";
  if (RTC.isEnabled()) {
    status = "green";
    text = "OK";
  };
  content += "<div class='status-wrapper'><div class='label'>RTC</div><div class='status value status-" + status + "'>" + text + "</div></div>";
#endif

#if _USE_SERVO_
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
#endif // _USE_SERVO_

#if _USE_BMS_
  status = "disabled";
  text = "DISABLED";
  if (BMS.isEnabled()) {
    double pcnt = BMS.getCapacityPercent();
    double volt = BMS.getCapacityVoltage();
    text = String(volt) + "v, " + String(pcnt) + "%";
    if (pcnt <= 10) {
      status = "red";
    } else if (pcnt <= 30) {
      status = "amber";
    } else {
      status = "green";
    }
  };
  content += "<div class='status-wrapper'><div class='label'>BMS</div><div class='status value status-" + status + "'>" + text + "</div></div>";
#endif // _USE_BMS_

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

  //  content += "<img alt='logo' src='/favicon.png' />";

  String log_summary = LOG.getLogSummary();
  if (log_summary.length() > 0) {
    content += "<pre id='log-summary'>";
    content += log_summary;
    content += "</pre>";
  }
#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showRoot(): responding");
#endif // _DEBUG_ && _XDEBUG_

  serverResponse(200, "text/html", htmlPage(String(DEVICE_NAME) + " - Home", content));

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showRoot(): complete");
#endif // _DEBUG_ && _XDEBUG_
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
#else // ESP32
void handle_logStart() {
#endif // ESP32

#if _DEBUG_
  Serial.println("WebServer::handle_logStart(): called");
#endif // _DEBUG_
  LOG.capture(true);
  goToUrl("/");
}

#ifdef ESP32
void handle_logStop(AsyncWebServerRequest * request) {
  __request = request;
#else // ESP32
void handle_logStop() {
#endif // ESP32

#if _DEBUG_
  Serial.println("WebServer::handle_logStop(): called");
#endif // _DEBUG_
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
#endif // _DEBUG_
  SRV.arm(true);
  goToUrl("/");
}

#ifdef ESP32
void handle_srvDisarm(AsyncWebServerRequest * request) {
  __request = request;
#else // ESP32
void handle_srvDisarm() {
#endif // ESP32

#if _DEBUG_
  Serial.println("WebServer::handle_srvDisarm(): called");
#endif // _DEBUG_
  SRV.arm(false);
  goToUrl("/");
}

#ifdef ESP32
void showFavIcon(AsyncWebServerRequest * request) {
  __request = request;
#else // ESP32
void showFavIcon() {
#endif // ESP32

#if _DEBUG_
  Serial.println("WebServer::showFavIcon(): called");
#endif // _DEBUG_
  // 64x64
  static char image[] PROGMEM = "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";

  int base64Len = strlen(image);
  int binaryLen = Base64.decodedLength(image, base64Len);

  char binary[binaryLen];
  Base64.decode(binary, image, base64Len);

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showFavIcon(): responding");
#endif // _DEBUG_ && _XDEBUG_

  serverResponse(200, "image/png", binary, binaryLen);

#if _DEBUG_ &&  _XDEBUG_
  Serial.println("WebServer::showFavIcon(): complete");
#endif // _DEBUG_ && _XDEBUG_
}

#ifdef ESP32
void showAboutQr(AsyncWebServerRequest * request) {
  __request = request;
#else // ESP32
void showAboutQr() {
#endif // ESP32

#if _DEBUG_
  Serial.println("WebServer::showAboutQr(): called");
#endif // _DEBUG_
  static char image[] PROGMEM = "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";

  int base64Len = strlen(image);
  int binaryLen = Base64.decodedLength(image, base64Len);

  char binary[binaryLen];
  Base64.decode(binary, image, base64Len);

#if _DEBUG_ && _XDEBUG_
  Serial.println("WebServer::showAboutQr(): responding");
#endif // _DEBUG_ && _XDEBUG_

  serverResponse(200, "image/png", binary, binaryLen);

#if _DEBUG_ && _XDEBUG_
  Serial.println("WebServer::showAboutQr(): complete");
#endif // _DEBUG_ && _XDEBUG_
}


/*******************************************************************************************************************************************
   _______   _____
  |__   __| / ____|
     | |_ _| (___   ___ _ ____   _____ _ __
     | | '__\___ \ / _ \ '__\ \ / / _ \ '__|
     | | |  ____) |  __/ |   \ V /  __/ |
     |_|_| |_____/ \___|_|    \_/ \___|_|
*/

void TrWEB::begin() {

  // Public pages visible by default
  addPage("/", showRoot, "Home");
  addPage("/about", showAbout, "About");
  addPage("/stats", showStats, "Stats");

  // these pages should not be publically listed, so no title means hidden
  addPage("/favicon.png", showFavIcon);
  addPage("/aboutqr.png", showAboutQr);
  addPage("/api/log/start", handle_logStart);
  addPage("/api/log/stop", handle_logStop);

#if _USE_SERVO_
  // Allow for dervo hndling if enabled
  addPage("/api/srv/arm", handle_srvArm);
  addPage("/api/srv/disarm", handle_srvDisarm);
#endif

  // Add a 404 handler
  registerNotFound(showNotFound);

  // Start the server
  serverBegin();

#if _DEBUG_
  //  Serial.println(String("     Web server: http://") + NET.getIpAddress() + "/");
  Serial.println(String("WEB initialised: http://") + NET.getIpAddress() + "/");
  if (!NET.isApMode()) {
    Serial.println(String("                 http://") + NET.getHostname() + ".local/");
    //  Serial.println(String("                 http://") + NET.getHostname() + "/");
  }
#endif // _DEBUG_
}

void TrWEB::loop() {
  serverLoop();
}

#endif // !_USE_WIFI
