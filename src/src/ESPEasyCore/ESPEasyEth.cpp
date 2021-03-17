#include "../ESPEasyCore/ESPEasyEth.h"

#ifdef HAS_ETHERNET

#include "ESPEasyNetwork.h"
#include "ETH.h"
#include "eth_phy/phy.h"
#include "../Globals/NetworkState.h"
#include "../Globals/Settings.h"
#include "../Helpers/StringConverter.h"
#include "../ESPEasyCore/ESPEasy_Log.h"

bool ethUseStaticIP() {
  return Settings.ETH_IP[0] != 0 && Settings.ETH_IP[0] != 255;
}

void ethSetupStaticIPconfig() {
  if (!ethUseStaticIP()) { return; }
  const IPAddress ip     = Settings.ETH_IP;
  const IPAddress gw     = Settings.ETH_Gateway;
  const IPAddress subnet = Settings.ETH_Subnet;
  const IPAddress dns    = Settings.ETH_DNS;

  if (loglevelActiveFor(LOG_LEVEL_INFO)) {
    String log = F("ETH IP   : Static IP : ");
    log += formatIP(ip);
    log += F(" GW: ");
    log += formatIP(gw);
    log += F(" SN: ");
    log += formatIP(subnet);
    log += F(" DNS: ");
    log += formatIP(dns);
    addLog(LOG_LEVEL_INFO, log);
  }
  ETH.config(ip, gw, subnet, dns);
}

bool ethCheckSettings() {
  bool result = true;
  if (!isValid(Settings.ETH_Phy_Type))
    result = false;
  if (!isValid(Settings.ETH_Clock_Mode))
    result = false;
  if (!isValid(Settings.NetworkMedium))
    result = false;
  if (Settings.ETH_Pin_mdc > MAX_GPIO)
    result = false;
  if (Settings.ETH_Pin_mdio > MAX_GPIO)
    result = false;
  if (Settings.ETH_Pin_power > MAX_GPIO)
    result = false;
  return result;
}

bool ethPrepare() {
  if (!ethCheckSettings())
  {
    addLog(LOG_LEVEL_ERROR, F("ETH: Settings not correct!!!"));
    return false;
  }
  char hostname[40];
  safe_strncpy(hostname, NetworkCreateRFCCompliantHostname().c_str(), sizeof(hostname));
  ETH.setHostname(hostname);
//  ETH.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  ethSetupStaticIPconfig();
  return true;
}

void ethPrintSettings() {
  if (loglevelActiveFor(LOG_LEVEL_INFO)) {
    String log;
    log.reserve(115);
//    log += F("ETH/Wifi mode: ");
//    log += toString(active_network_medium);
    log += F("ETH PHY Type: ");
    log += toString(Settings.ETH_Phy_Type);
    log += F(" PHY Addr: ");
    log += Settings.ETH_Phy_Addr;
    log += F(" Eth Clock mode: ");
    log += toString(Settings.ETH_Clock_Mode);
    log += F(" MDC Pin: ");
    log += String(Settings.ETH_Pin_mdc);
    log += F(" MIO Pin: ");
    log += String(Settings.ETH_Pin_mdio);
    log += F(" Power Pin: ");
    log += String(Settings.ETH_Pin_power);
    addLog(LOG_LEVEL_INFO, log);
  }
}

uint8_t * ETHMacAddress(uint8_t* mac) {
    esp_eth_get_mac(mac);
    return mac;
}

bool ETHConnectRelaxed() {
  ethPrintSettings();
  if (!ETH.begin( Settings.ETH_Phy_Addr,
                  Settings.ETH_Pin_power,
                  Settings.ETH_Pin_mdc,
                  Settings.ETH_Pin_mdio,
                  (eth_phy_type_t)Settings.ETH_Phy_Type,
                  (eth_clock_mode_t)Settings.ETH_Clock_Mode)) 
  {
    return false;
  }
  addLog(LOG_LEVEL_INFO, F("After ETH.begin"));
  /*
  if (!ethPrepare()) {
    // Dead code for now...
    addLog(LOG_LEVEL_ERROR, F("ETH : Could not prepare ETH!"));
    return false;
  }
  */
  return true;
}

bool ETHConnected() {
  return eth_connected;
}

#endif