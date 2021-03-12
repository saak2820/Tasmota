/*
  xdrv_78_fritzbox.ino - fritzbox for Tasmota

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
  pio device monitor > logFile.txt
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

WiFiClient tr064client;
/*********************************************************************************************\
 * TR-064 protocol call
 *
 *
 * Tested with defines
 * #define USE_fritzbox                             // Support for tr-064 protocol
 \*********************************************************************************************/

#define XDRV_78                    78

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



TR064 tr064;

// void script_call_action(void(*func)(char *));
// #define xPSTR(a) a

uint16_t SendFritzBox(char *buffer) {
  char *params,*oparams;
  const char *service;
  const char *action;
  const char *param;
  char *fparams,*fparamsend,*freq;
  char auth=0;
  uint16_t status=1;
  int ip=0,iq=0,j;
  //TR064 *tr064=0;
  uint16_t blen;
  char *endcmd;
  char *cp;
  // String tr064_params[][2];// = {{"NewX_AVM-DE_PhoneNumber", "**799"}};
  // String tr064_req[][2];// = {{}};
  String tr064_params[][2]= {{}};
  String tr064_req[][2] = {{}};

  AddLog_P(LOG_LEVEL_DEBUG_MORE,PSTR(D_LOG_FRITZBOX   "Im in SendFritzbox"));
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
  
  service=strtok(params,",\"");
  AddLog_P(LOG_LEVEL_DEBUG_MORE, PSTR(D_LOG_FRITZBOX "service: %s"),service);
  if (!service) {
      goto exit;
  }
  
  action=strtok(NULL,",\"");
  AddLog_P(LOG_LEVEL_DEBUG_MORE, PSTR(D_LOG_FRITZBOX "action: %s"),action);
  if (!action) {
      goto exit;
  }
 
 fparams=strtok(NULL,"]");

  
 if (fparams && strstr(fparams,"{{")){
    
    fparams +=2;
    fparamsend = strstr(fparams,"}}");
    char found [fparamsend- fparams];
    strncpy ( found, fparams, fparamsend- fparams);
    found[fparamsend- fparams] = '\0';   /* null character manually added */
    AddLog_P(LOG_LEVEL_DEBUG_MORE, PSTR(D_LOG_FRITZBOX "found params: %s"),found);
    fparams=strtok(found,"{,}\"");
    
    j = 0;
    while (fparams != NULL)
    {
      AddLog_P(LOG_LEVEL_DEBUG_MORE, PSTR(D_LOG_FRITZBOX "parameter: %s"),fparams);
      tr064_params[ip][j]=String(FPSTR(fparams));
      fparams=strtok(NULL,"{,}\"");
      ip = ip +j;
      j = 1 -j;
    }
    
    if(fparamsend){
      freq= strstr(fparamsend,"{{");
      if (freq) {
        //freq +=2;
        
        j=0;
        freq=strtok(NULL,"{,}\"");
        while (freq != NULL)
        {
          
          tr064_req[iq][j]=String(FPSTR(freq));
          freq=strtok(NULL,"{,}\"");
          AddLog_P(LOG_LEVEL_DEBUG_MORE, PSTR(D_LOG_FRITZBOX "requests: %s"),freq);
          
          iq=iq+j;j = 1 -j;
        }
      }
    }
 }
  AddLog_P(LOG_LEVEL_INFO, PSTR(D_LOG_FRITZBOX"FBInit: TR064 Services: %s"),tr064.state()<0?"NoServices":"OK");
      if(tr064.state()<0){
        tr064.init();
      }
      
      if(tr064.state()==0){      
        
        AddLog_P(LOG_LEVEL_INFO, PSTR(D_LOG_FRITZBOX "SendFritzBox: send action: %s"),action);


          tr064.action(service, action, tr064_params, ip);//, tr064_req, iq);
          
          status=0; 
          AddLog_P(LOG_LEVEL_INFO, "FBInit: TR064.init %s",tr064.state()<0?"NoServices":"OK");
        
      }else{
        AddLog_P(LOG_LEVEL_ERROR, PSTR(D_LOG_FRITZBOX " no Services Loaded")); 
      }
      AddLog_P(LOG_LEVEL_INFO, PSTR(D_LOG_FRITZBOX "SendFritzBox: exit")); 
    
exit:
  if (oparams) free(oparams);
  return status;
}

void FBInit(void) {
  if (!TasmotaGlobal.global_state.network_down) {

    tr064.debug_level = Settings.seriallog_level;
    tr064.setServer(FRITZBOX_HOST, Settings.fritzbox_port,FRITZBOX_USER, FRITZBOX_PASS);
    AddLog_P(LOG_LEVEL_INFO, "FBInit: TR064.init %s",tr064.state()<0?"NoServices":"OK");
  }else{
    AddLog_P(LOG_LEVEL_INFO, "FBInit: network.down");
  }
}

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

bool Xdrv78(uint8_t function)
{
  bool result = false;
 if (Settings.flag5.fritzbox_enabled) {  // SetOption120 - Enable 
    switch (function) {
      
      case FUNC_COMMAND:
        result=true;
        break;      
#ifdef USE_WEBSERVER
      case FUNC_WEB_ADD_BUTTON:
        AddLog_P(LOG_LEVEL_DEBUG,"[Xdrv78] ADD_BUTTON: ");
        WSContentSend_P(HTTP_BTN_MENU_FRITZ);
        break;
      case FUNC_WEB_ADD_HANDLER:        
        WebServer_on(PSTR("/" WEB_HANDLE_FRITZ), HandleFritzBoxConfiguration);
        
        break;
#endif  // USE_WEBSERVER
      case FUNC_MQTT_INIT:
        AddLog_P(LOG_LEVEL_DEBUG,"[Xdrv78] FUNC_MQTT_INIT");        
        FBInit();
        break;
    }
  }
  return result;
}


#endif  // USE_fritzbox