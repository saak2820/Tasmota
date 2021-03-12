/*
  xdrv_47_fritzbox.ino - fritzbox for Tasmota

  Copyright (C) 2020  

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifdef USE_FRITZBOX

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#endif  // ESP8266
#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#endif  // ESP32

#include <tr064.h>


/*********************************************************************************************\
 * TR-064 protocol call
 *
 *
 * Tested with defines
 * #define USE_fritzbox                             // Support for tr-064 protocol
 \*********************************************************************************************/

#define XDRV_47                    47

#ifndef TR064_WIFI_CLIENT_TIMEOUT
#define TR064_WIFI_CLIENT_TIMEOUT   200    // Wifi TCP connection timeout (default is 5000 mSec)
#endif

// #define D_FRITZBOX "FritzBox"
// #define D_CONFIGURE_FRITZBOX "FritzBox konfigurieren"
// #define D_FRITZ_PARAMETERS "FritzBox-Einstellungen"
// #define D_FRITZBOX_ENABLE "FritzBox aktivieren"
// #define D_LOG_FRITZBOX "FBO: "          // FritzBox
// #define D_CMND_FRITZUSER "FritzBoxUser"
// #define D_CMND_FRITZPASSWORD "FritzBoxPassword"
// #define D_FRITZBOX_HOST "FritzBox Host"
// #define D_FRITZBOX_PORT "FritzBox Port"
// #define D_FRITZBOX_USER "FritzBox Benutzer"

WiFiClient client;

TR064 tr064;

void script_call_action(void(*func)(char *));
#define xPSTR(a) a
//sendfritzbox [server:port:user:passwd:service:action]//:params:nParam:req:nReq]
uint16_t SendFritzBox(char *buffer) {
  char *params,*oparams;
  const char *service;
  const char *action;
  const char *param;
  char *fparams,*fparamsend,*freq;
  char auth=0;
  uint16_t status=1;
  int ip,iq,j;
  //TR064 *tr064=0;
  uint16_t blen;
  char *endcmd;
  char *cp;
  // String tr064_params[][2];// = {{"NewX_AVM-DE_PhoneNumber", "**799"}};
  // String tr064_req[][2];// = {{}};
  String tr064_params[][2]= {{}};
  String tr064_req[][2] = {{}};

  AddLog_P(LOG_LEVEL_INFO, "Im in SendFritzbox");
while (*buffer==' ') buffer++;

if (*buffer!='[') {
      goto exit;
  }
  buffer++;

   endcmd=strchr(buffer,']');
  if (!endcmd) {
    goto exit;
  }


  blen=(uint32_t)endcmd-(uint32_t)buffer;
  oparams=(char*)calloc(blen+2,1);
  if (!oparams) return 4;
  params=oparams;
  strncpy(oparams,buffer,blen+2);
  oparams[blen]=0;
  
  service=strtok(params,",");
  if (!service) {
      goto exit;
  }
  
  action=strtok(NULL,",");
  AddLog_P(LOG_LEVEL_INFO, PSTR("action: %s"),action);
  if (!action) {
      goto exit;
  }
 
 fparams=strtok(NULL,"]");
//  {{
//  "NewX_AVM-DE_PhoneNumber", "**9"},{"bla","blub"
//  }},{{"bla","blub"}}]

  
 fparams = strstr(fparams,"{{");
 if (fparams) {
    
    fparams +=2;
    fparamsend = strstr(fparams,"}}");
    AddLog_P(LOG_LEVEL_INFO, PSTR("fparamsend: %s"),fparamsend);
    AddLog_P(LOG_LEVEL_INFO, PSTR("fparams: %s"),fparams);
    AddLog_P(LOG_LEVEL_INFO, PSTR("length: %d"),fparamsend- fparams);
    char found [fparamsend- fparams];
    strncpy ( found, fparams, fparamsend- fparams);
    found[fparamsend- fparams] = '\0';   /* null character manually added */
    AddLog_P(LOG_LEVEL_INFO, PSTR("found: %s"),found);
    fparams=strtok(found,"{,}\"");
    ip = 0;
    j = 0;
    while (fparams != NULL)
    {
      AddLog_P(LOG_LEVEL_INFO, PSTR("tokens2: %s"),fparams);
      tr064_params[ip][j]=String(FPSTR(fparams));
      fparams=strtok(NULL,"{,}\"");
      ip = ip +j;
      j = 1 -j;
    }
    
    AddLog_P(LOG_LEVEL_INFO, PSTR("fparamsend: %s"),fparamsend);
    if(fparamsend){
      freq= strstr(fparamsend,"{{");
      if (freq) {
        //freq +=2;
        iq = 0;
        j=0;
        freq=strtok(NULL,"{,}\"");
        while (freq != NULL)
        {
          
          tr064_req[iq][j]=String(FPSTR(freq));
          freq=strtok(NULL,"{,}\"");
          AddLog_P(LOG_LEVEL_INFO, PSTR("tokens: %s"),freq);
          
          iq=iq+j;j = 1 -j;
        }
      }
    }
 }else{
   goto exit;
 }
  
     status=0;

    AddLog_P(LOG_LEVEL_INFO, PSTR(D_LOG_FRITZBOX D_CONNECTED)); 
    AddLog_P(LOG_LEVEL_INFO," connection.action");
    if (!client.connected()) {
      AddLog_P(LOG_LEVEL_DEBUG, "FBInit: Client fail");
    }else{
      
    }
    tr064.action(service, action, tr064_params, ip, tr064_req, iq);
   // AddLog_P(LOG_LEVEL_INFO, PSTR("tr064_params: %s"),tr064.action(service, action, tr064_params, ip, tr064_req, iq));

exit:
  if (oparams) free(oparams);
  return status;
}

void FBInit(void) {
  if (!TasmotaGlobal.global_state.network_down) {
    AddLog_P(LOG_LEVEL_INFO," FBInit");
    EspClient.setTimeout(MQTT_WIFI_CLIENT_TIMEOUT);
    tr064.setClient(client);
    tr064.debug_level = DEBUG_VERBOSE;
    tr064.setServer(FRITZBOX_HOST, Settings.fritzbox_port,FRITZBOX_USER, FRITZBOX_PASS);
    tr064.init();
    //client = Webserver->client();
    // if (!client.connected()) {
    //   AddLog_P(LOG_LEVEL_INFO, "FBInit: Client fail");
    // }else{
      AddLog_P(LOG_LEVEL_INFO,PSTR(" client -> connected %s "),FRITZBOX_HOST);
      // tr064 = new TR064(client, Settings.fritzbox_port, FRITZBOX_HOST, FRITZBOX_USER, FRITZBOX_PASS);
      // tr064->debug_level = DEBUG_VERBOSE; //0: None, 1: Errors, 2: Warning, 3: Info, 4: Verbose
      // tr064->init();
      AddLog_P(LOG_LEVEL_INFO, "FBInit: after TR064.init");
    // }
  }else{
    AddLog_P(LOG_LEVEL_INFO, "FBInit: network.down");
  }
}




// 	connection.action("urn:dslforum-org:service:X_VoIP:1", "X_AVM-DE_DialNumber", params, 1, req, 0);
// }
// void FritzLoop(void) {
//   if (!TasmotaGlobal.global_state.network_down) {
//       switch (FritzBox.state) {
//       case 0:
//         // FritzBoxnit();
//         FritzBox.state++;
//         break;
//       case 1:
//         FritzBox.state++;
//         break;
//       case 2:
//         if (FritzBox.wait) {
//           FritzBox.wait--;
//         } else {
//           FritzBox.state = 1;
//         }
//       }
//   }
// }
  

/*********************************************************************************************\
 * Presentation
\*********************************************************************************************/

#ifdef USE_WEBSERVER

#define WEB_HANDLE_FRITZ "fr"

const char S_CONFIGURE_FRITZ[] PROGMEM = D_CONFIGURE_FRITZBOX;

const char HTTP_BTN_MENU_FRITZ[] PROGMEM =
  "<p><form action='" WEB_HANDLE_FRITZ "' method='get'><button>" D_CONFIGURE_FRITZBOX "</button></form></p>";

const char HTTP_FORM_FRITZ1[] PROGMEM =
  "<fieldset><legend><b>&nbsp;" D_FRITZ_PARAMETERS "&nbsp;</b></legend>"
  "<form method='get' action='" WEB_HANDLE_FRITZ "'>"
  "<p><b>" D_IP_ADDRESS "</b> (" FRITZBOX_HOST ")<br><input id='fh' placeholder=\"" FRITZBOX_HOST "\" value=\"%s\"></p>"
  "<p><b>" D_PORT "</b> (" STR(FRITZBOX_PORT) ")<br><input id='fl' placeholder='" STR(FRITZBOX_PORT) "' value='%d'></p>";
const char HTTP_FORM_FRITZ2[] PROGMEM =
  "<p><b>" D_USER "</b> (" FRITZBOX_USER ")<br><input id='fu' placeholder=\"" FRITZBOX_USER "\" value=\"%s\"></p>"
  "<p><label><b>" D_PASSWORD "</b><input type='checkbox' onclick='sp(\"fp\")'></label><br><input id='fp' type='password' placeholder=\"" D_PASSWORD "\" value=\"" D_ASTERISK_PWD "\"></p>";

void HandleFritzBoxConfiguration(void)
{
  if (!HttpCheckPriviledgedAccess()) { return; }

  AddLog_P(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_CONFIGURE_FRITZBOX));

  if (Webserver->hasArg("save")) {
    FritzBoxSaveSettings();
    HandleConfiguration();                      
    FBInit();                                                                         
    return;
  }

  char str[TOPSZ];

  WSContentStart_P(PSTR(D_CONFIGURE_FRITZBOX));
  WSContentSendStyle();
  WSContentSend_P(HTTP_FORM_FRITZ1,
    SettingsText(SET_FRITZBOX_HOST),
    Settings.fritzbox_port);
  WSContentSend_P(HTTP_FORM_FRITZ2,
    (!strlen(SettingsText(SET_FRITZBOX_USER))) ? "0" : SettingsText(SET_FRITZBOX_USER));
  WSContentSend_P(HTTP_FORM_END);
  WSContentSpaceButton(BUTTON_CONFIGURATION);
  WSContentStop();
}

void FritzBoxSaveSettings(void)
{
  char tmp[TOPSZ];
  char stemp[TOPSZ];
  char stemp2[TOPSZ];

  WebGetArg("fh", tmp, sizeof(tmp));
  AddLog_P(LOG_LEVEL_INFO,"fh ",tmp);
  SettingsUpdateText(SET_FRITZBOX_HOST, (!strlen(tmp)) ? FRITZBOX_HOST : (!strcmp(tmp,"0")) ? "" : tmp);
  WebGetArg("fl", tmp, sizeof(tmp));
  Settings.fritzbox_port = (!strlen(tmp)) ? FRITZBOX_PORT : atoi(tmp);
  WebGetArg("fu", tmp, sizeof(tmp));
  SettingsUpdateText(SET_FRITZBOX_USER, (!strlen(tmp)) ? "USER" : (!strcmp(tmp,"0")) ? "" : tmp);
  WebGetArg("fp", tmp, sizeof(tmp));
  SettingsUpdateText(SET_FRITZBOX_PASS, (!strlen(tmp)) ? "" : (!strcmp(tmp, D_ASTERISK_PWD)) ? SettingsText(SET_FRITZBOX_PASS) : tmp);
  
  AddLog_P(LOG_LEVEL_INFO, SettingsText(SET_FRITZBOX_HOST), Settings.fritzbox_port, SettingsText(SET_FRITZBOX_USER));
  
}
#endif  // USE_WEBSERVER

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xdrv47(uint8_t function)
{
  bool result = false;
  
 if (Settings.flag.fritzbox_enabled) {  // SetOption32 - Enable 
    switch (function) {
      
      case FUNC_COMMAND:
        result=true;
        break;      
#ifdef USE_WEBSERVER
      case FUNC_WEB_ADD_BUTTON:
        WSContentSend_P(HTTP_BTN_MENU_FRITZ);
        break;
      case FUNC_WEB_ADD_HANDLER:        
        WebServer_on(PSTR("/" WEB_HANDLE_FRITZ), HandleFritzBoxConfiguration);
        
        break;
#endif  // USE_WEBSERVER
      case FUNC_MQTT_INIT:
        AddLog_P(LOG_LEVEL_INFO,"FUNC_PRE_INIT");
        FBInit();
        break;
    }
  }
  return result;
}


#endif  // USE_fritzbox