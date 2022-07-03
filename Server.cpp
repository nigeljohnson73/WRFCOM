#include "Server.h"

#ifdef ESP32
#include <WiFi.h>
#include <SPIFFS.h>
// TODO: Implement missing bits for this :(
// https://raw.githubusercontent.com/RuiSantosdotme/ESP32-Course/master/code/WiFi_Web_Server_Outputs/WiFi_Web_Server_Outputs.ino
// ESP8266WebServer WebServer(80);
#else
#include <Arduino.h>
//#include <ESP8266WiFi.h>
//#include <Hash.h>
#include <ESP8266WebServer.h>
#include "FS.h"

ESP8266WebServer WebServer(80);
#endif

TrServer Server;

// Adding and manging local URLs for the about page mostly
const unsigned int pmax = 20;
unsigned int pcount = 0;
String urls[pmax];
String titles[pmax];
//void (*not_found_callback)(void);


/*******************************************************************************************************************************************
            _         _                  _           _
      /\   | |       | |                | |         | |
     /  \  | |__  ___| |_ _ __ __ _  ___| |_ ___  __| |   ___  ___ _ ____   _____ _ __
    / /\ \ | '_ \/ __| __| '__/ _` |/ __| __/ _ \/ _` |  / __|/ _ \ '__\ \ / / _ \ '__|
   / ____ \| |_) \__ \ |_| | | (_| | (__| ||  __/ (_| |  \__ \  __/ |   \ V /  __/ |
  /_/    \_\_.__/|___/\__|_|  \__,_|\___|\__\___|\__,_|  |___/\___|_|    \_/ \___|_|

*/
void serverBegin() {
  // TODO: This will probably be different on ESP32
  WebServer.begin();
}

void registerUri(String uri, void (*func)(void)) {
  // TODO: this will be differnet on ESP32
  WebServer.on(uri, func);
}

void registerStaticUri(String uri, String filename) {
  WebServer.serveStatic(uri.c_str(), SPIFFS, filename.c_str());
}

void serverResponse(int code, String type, String content) {
  WebServer.send(code, type, content);
}

void registerNotFound(void (*func)(void)) {
  WebServer.onNotFound(func);
}

void serverLoop() {
  // TODO: This will probably be different on ESP32
  WebServer.handleClient();
}

//// Use at the top of page shows that need to be authenticated
//bool isAuthenticated() {
//  if (!WebServer.authenticate(_WWW_USERNAME_, _WWW_PASSWORD_)) {
//    WebServer.requestAuthentication(DIGEST_AUTH, _AP_NAME_, "Authorisation failed");
//    return false;
//  }
//  return true;
//}
//
//// See if the passed form post has the expected token and it matches
//bool hasToken(String t) {
//  for (uint8_t i = 0; i < WebServer.args(); i++) {
//    if (WebServer.arg(i).length()) {
//      if (WebServer.argName(i) == "token" && WebServer.arg(i) == t) {
//        return true;
//      }
//    }
//  }
//  return false;
//}

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

// Convert hex character to integer
unsigned char h2int(char c) {
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}

// Decode a URL string
String urldecode(String str) {

  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == '+') {
      encodedString += ' ';
    } else if (c == '%') {
      i++;
      code0 = str.charAt(i);
      i++;
      code1 = str.charAt(i);
      c = (h2int(code0) << 4) | h2int(code1);
      encodedString += c;
    } else {

      encodedString += c;
    }

    yield();
  }

  return encodedString;
}

// encode a string for URLing safely
String urlencode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
      //encodedString+=code2;
    }
    yield();
  }
  return encodedString;

}


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

void addPage(String uri, void (*func)(void), String title, boolean hidden) {
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
  // TODO: This will be different on ESP32
  //  WebServer.on(uri, func);
  registerUri(uri, func);
}
void addPage(String uri, void (*func)(void), String title) {
  addPage(uri, func, title, false);
}
void addPage(String uri, void (*func)(void)) {
  addPage(uri, func, "", true);
}

void addStaticPage(String uri, String filename, String title, boolean hidden) {
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
  //WebServer.serveStatic(uri.c_str(), SPIFFS, filename.c_str());
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

  serverResponse(200, "text/html", ret);
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
  ret += "<link rel='preload' as='image' href='/ajax-loader-bar.gif' />";

  ret += "<style>";

  ret += "html, body {height: 100%;}";
  ret += "body {display:flex; flex-direction: column; background-color: #eee; font-family: Arial, Helvetica, Sans-Serif; color: #090; text-align: center;}";
  ret += "#main {flex: 1 0 auto; padding-bottom: 10px; padding-left: 30px;padding-right: 30px; }";
  ret += "footer {flex-shrink: 0;padding-bottom: 20px;}";
  ret += ".status-wrapper {clear: both;}";
  ret += ".status-wrapper .label, .status-wrapper .value {padding-top: 10px; padding-bottom: 10px; width: 49.5%; float: left; margin-bottom: 10px;}";
  ret += ".status, .button {width: 99%; padding-top: 10px; padding-bottom: 10px; border-radius: 5px; border: solid 1px #000; background-color: #ddd; color: #000;}";
  ret += ".button a {all: unset;}";
  ret += ".status-red {border-color: #a00; background-color: #faa; color: #a00;}";
  ret += ".status-amber {border-color: #a70; background-color: #fca; color: #a70;}";
  ret += ".status-green {border-color: #0a0; background-color: #afa; color: #0a0;}";

  ret += "footer ul.buttons, footer ul.buttons li, footer ul.buttons li a {all:unset;}";
  ret += "footer ul.buttons li {float: left; list-style: none; border-radius: 5px; border: solid 1px #000; background-color: #090; color:#fff; width: ";
  ret += floor(98 / pcount) - 2;
  ret += "%; padding-top: 10px; padding-bottom: 10px; margin-left:1%; margin-right:1%;}";
  ret += "footer ul.buttons li.selected {background-color:#ccc;}";
  ret += "footer .action {display:none;}";
  ret += "pre.log-summary {padding-top: 10px; padding-bottom: 10px; padding-left: 10px;padding-right: 10px; border:solid #666 1px; border-radius:5px; background-color:#e3e3e3; color:#666;}";

  ret += " </style> ";

  ret += "<script>";
  ret += "function hideFooterButtons() { document.getElementById('footer-buttons').style.display = 'none'; document.getElementById('footer-action').style.display = 'inline-block'; document.getElementById('logstat').style.display = 'none';}";
  ret += "</script>";

  ret += " </head> ";
  ret += "<body><div id='main'>";

  return ret;
}

String htmlFooter() {
  String content = "";

  content += "</div><footer>";
  content += "<ul id='footer-buttons' class='buttons'>";
  for (int i = 0; i < pcount; i++) {
    content += "<li";
    if (urls[i] == WebServer.uri()) {
      content += " class='selected'";
    }
    content += ">";
    content += "<a onclick='hideFooterButtons()' href='";
    content += urls[i];
    content += "'>";
    content += titles[i];
    content += "</a>";
    content += "</li>";
  }
  content += "</ul>";

  content += "<img id='footer-action' class='action' alt='loading icon' src='/ajax-loader-bar.gif' />";
  content += "</footer></body></html> ";

  return content;
}

String htmlPage(String title, String content) {
  return htmlHeader(title) + content + htmlFooter();
}

void showNotFound() {
#if _DEBUG_
  String uri = WebServer.uri();
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

void showStats() {
#if _DEBUG_
  Serial.println("WebServer::showRoot(): called");
#endif
  String title;
  String status;
  String text;
  String content = "<h1>Data Snapshot</h1>";

  title = "NET time";
  status = "green";
  text = Network.getTimestamp();
  if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "RTC time";
  status = "green";
  text = RTC.getTimestamp();
  if (!RTC.isEnabled()) {
    status = "amber";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "GPS time";
  status = "green";
  text = GPS.getTimestamp();
  if (!GPS.isEnabled()) {
    status = "amber";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "IMU temp";
  status = "green";
  text = String(IMU.getTemperature());
  if (!IMU.isEnabled()) {
    status = "amber";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  } else {
    text += " C";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "BMP temp";
  status = "green";
  text = String(BMP.getTemperature());
  if (!BMP.isEnabled()) {
    status = "amber";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  } else {
    text += " C";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "BMP pressure";
  status = "green";
  text = String(BMP.getPressure());
  //  Serial.print("BMP pressure: ");
  //  Serial.print(text);
  //  Serial.println();
  if (!BMP.isEnabled()) {
    status = "amber";
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
    status = "amber";
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
  text = String(GPS.getAltitude());
  if (!GPS.isEnabled()) {
    status = "amber";
    text = "[disconnected]";
  } else if (text.length() == 0) {
    status = "red";
    text = "[not configured]";
  } else {
    text += " m";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "GPS Location";
  status = "green";
  text = "";
  if (!GPS.isEnabled()) {
    status = "amber";
    text = "[disconnected]";
  } else if (!GPS.isConnected()) {
    status = "amber";
    text = "[warming up]";
  } else {
    text += GPS.getLatitude();
    text += " N, ";
    text += GPS.getLongitude();
    text += " E";
  }
  content += "<div class='status-wrapper'><div class='label'>" + title + "</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  title = "GPS Sats";
  status = "green";
  text = GPS.getSatsInView();
  if (!GPS.isEnabled()) {
    status = "amber";
    text = "[disconnected]";
  } else if (!GPS.isConnected()) {
    status = "amber";
    text = "[warming up]";
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

void showAbout() {
#if _DEBUG_
  Serial.println("WebServer::showAbout(): called");
#endif
  String content = "<h1>About ";
  content += _AP_NAME_;
  content += "</h1>";
  content += String("<p>Application version: ") + VERSION + "</p>";
  content += String("<p>Application Build: ") + __DATE__ + " " + __TIME__ + "</p>";

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

void showRoot() {
#if _DEBUG_
  Serial.println("WebServer::showRoot(): called");
#endif
  String content = String("<h1>") + _AP_NAME_ + "</h1>";
  String status = "";
  String text = "";

  status = "green";
  text = "IDLE";
  if (Logger.isEnabled()) {
    status = "amber";
    text = "LOGGING";
  };
  content += "<div class='status-wrapper'><div class='label'>Logger</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  status = "green";
  text = "OK";
  if (!GPS.isEnabled()) {
    status = "red";
    text = "DISABLED";
  } else if (!GPS.isConnected()) {
    status = "amber";
    text = "CONNECTING";
  };
  content += "<div class='status-wrapper'><div class='label'>GPS</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  status = "green";
  text = "OK";
  if (!BMP.isEnabled()) {
    status = "red";
    text = "DISABLED";
  };
  content += "<div class='status-wrapper'><div class='label'>BMP</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  status = "green";
  text = "OK";
  if (!IMU.isEnabled()) {
    status = "red";
    text = "DISABLED";
  };
  content += "<div class='status-wrapper'><div class='label'>IMU</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  status = "green";
  text = "OK";
  if (!RTC.isEnabled()) {
    status = "red";
    text = "DISABLED";
  };
  content += "<div class='status-wrapper'><div class='label'>RTC</div><div class='status value status-" + status + "'>" + text + "</div></div>";

  content += "<div style='clear:both'></div>";
  if (Logger.isEnabled()) {
    content += "<a id='logstat' onclick='hideFooterButtons()' href='/api/log/stop'><img src='/log_stop.png' alt='stop logging' /></a>";
  } else {
    content += "<a id='logstat' onclick='hideFooterButtons()' href='/api/log/start'><img src='/log_start.png' alt='start logging' /></a>";
  }

  String log_summary = Logger.getLogSummary();
  if (log_summary.length() > 0) {
    content += "<pre class='log-summary'>";
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
void handle_logStart() {
#if _DEBUG_
  Serial.println("WebServer::handle_logStart(): called");
#endif
  Logger.enable(true);
  goToUrl("/");
}

void handle_logStop() {
#if _DEBUG_
  Serial.println("WebServer::handle_logStop(): called");
#endif
  Logger.enable(false);
  goToUrl("/");
}

/*******************************************************************************************************************************************
   _______   _____
  |__   __| / ____|
     | |_ _| (___   ___ _ ____   _____ _ __
     | | '__\___ \ / _ \ '__\ \ / / _ \ '__|
     | | |  ____) |  __/ |   \ V /  __/ |
     |_|_| |_____/ \___|_|    \_/ \___|_|
*/

TrServer::TrServer() {};

void TrServer::begin() {
  // Public static pages hidden by default
  addStaticPage("/favicon.ico", "/favicon.png");
  addStaticPage("/favicon.png", "/favicon.png");
  addStaticPage("/log_start.png", "/log_start.png");
  addStaticPage("/log_stop.png", "/log_stop.png");
  addStaticPage("/ajax-loader-bar.gif", "/ajax-loader-bar.gif");

  // Public pages visible by default
  addPage("/", showRoot, "Home");
  addPage("/about", showAbout, "About");
  addPage("/stats", showStats, "Stats");

  // these pages should not be publically listed, so no title means hidden
  addPage("/api/log/start", handle_logStart);
  addPage("/api/log/stop", handle_logStop);

  // Add a 404 handler
  registerNotFound(showNotFound);

  // Start the server
  serverBegin();
#if _DEBUG_
  Serial.println(String("     Web server: http://") + Network.getIpAddress() + "/");
  Serial.println(String("                 http://") + Network.getHostname() + ".local/");
  //  Serial.println(String("                 http://") + Network.getHostname() + "/");
#endif
}

void TrServer::loop() {
  serverLoop();
}
