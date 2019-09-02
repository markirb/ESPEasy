#ifdef USES_P088
//#######################################################################################################
//#################################### Plugin 088: Heatpump IR ##########################################
//#######################################################################################################

#define PLUGIN_088
#define PLUGIN_ID_088         88
#define PLUGIN_NAME_088       "Heatpump IR transmitter"


/*
 * ESPEasy plugin to send air conditioner / heatpump IR signals
 * * Use the device type 'Heatpump IR transmitter' as the device type in Devices -> Edit
 * * Connect and IR LED + series resistor between the GPIO pin configured for this device and ground
 * * This is not a standalone plugin, also the _C002.ino modification is needed for Domoticz MQTT to work
 *
 * Send commands through http, like this example (assuming the IP address of the ESP node is 192.168.0.61):
 * * curl http://192.168.0.61/control?cmd=heatpumpir,panasonic_ckp,1,1,0,22,0,0
 *
 * Send commands through OpenHAB MQTT with Mosquitto, like this example,
 * assuming the 'Name' of the ESP node in ESPEasy Main Settings page is 'ESP_Easy')
 * * mosquitto_pub -t /ESP_Easy/cmd -m heatpumpir,panasonic_ckp,1,1,0,22,0,0
 *
 * Send commands through Domoticz MQTT, like this example,
 * assuming the IDX of the heatpump device in both Domoticz and ESP Easy is 13:
 * * Create a 'Dummy' hardware in Domoticz, create a 'Text' virtual sensor into the 'Dummy' device (assumed we got IDX 13)
 * * Define the Domoticz MQTT protocol in ESP Easy, define the 'Heatpump IR transmitter' device using the same IDX as in Domoticz
 * * Update the 'Text' sensor using the Domoticz API, like navigating to this URL (assuming Domoticz is at 192.168.0.5:8080):
 * *   http://192.168.0.5:8080/json.htm?type=command&param=udevice&idx=13&svalue=panasonic_ckp,1,1,0,22,0,0
 *
 * Take a look at https://github.com/ToniA/cabin-village-project/blob/eventscripts/script_device_hp.lua for Domoticz event examples
 *
 * The parameters are (in this order)
 * * The type of the heatpump as a string, see the implementations of different models, like https://github.com/ToniA/arduino-heatpumpir/blob/master/MitsubishiHeatpumpIR.cpp
 * * power state (see https://github.com/ToniA/arduino-heatpumpir/blob/master/HeatpumpIR.h for modes)
 * * operating mode
 * * fan speed
 * * temperature
 * * vertical air direction
 * * horizontal air direction
 *
 * See the HeatpumpIR library for further information: https://github.com/ToniA/arduino-heatpumpir
 *
 */

 #include <FujitsuHeatpumpIR.h>
 #include <PanasonicCKPHeatpumpIR.h>
 #include <PanasonicHeatpumpIR.h>
 #include <CarrierHeatpumpIR.h>
 #include <MideaHeatpumpIR.h>
 #include <MitsubishiHeatpumpIR.h>
 #include <SamsungHeatpumpIR.h>
 #include <SharpHeatpumpIR.h>
 #include <DaikinHeatpumpIR.h>
 #include <MitsubishiHeavyHeatpumpIR.h>
 #include <MitsubishiSEZKDXXHeatpumpIR.h>
 #include <HyundaiHeatpumpIR.h>
 #include <HisenseHeatpumpIR.h>
 #include <GreeHeatpumpIR.h>
 #include <FuegoHeatpumpIR.h>
 #include <ToshibaHeatpumpIR.h>
 #include <ToshibaDaiseikaiHeatpumpIR.h>
 #include <IVTHeatpumpIR.h>
 #include <HitachiHeatpumpIR.h>
 #include <BalluHeatpumpIR.h>
 #include <AUXHeatpumpIR.h>

// Array with all supported heatpumps
HeatpumpIR *heatpumpIR[] = {new PanasonicCKPHeatpumpIR(), new PanasonicDKEHeatpumpIR(), new PanasonicJKEHeatpumpIR(),
                            new PanasonicNKEHeatpumpIR(), new PanasonicLKEHeatpumpIR(),
                            new CarrierNQVHeatpumpIR(), new CarrierMCAHeatpumpIR(),
                            new MideaHeatpumpIR(), new FujitsuHeatpumpIR(),
                            new MitsubishiFDHeatpumpIR(), new MitsubishiFEHeatpumpIR(), new MitsubishiMSYHeatpumpIR(), new MitsubishiFAHeatpumpIR(),
                            new SamsungAQVHeatpumpIR(), new SamsungFJMHeatpumpIR(),new SharpHeatpumpIR(), new DaikinHeatpumpIR(),
                            new MitsubishiHeavyZJHeatpumpIR(), new MitsubishiHeavyZMHeatpumpIR(),
                            new MitsubishiSEZKDXXHeatpumpIR(),
                            new HyundaiHeatpumpIR(), new HisenseHeatpumpIR(),
                            new GreeGenericHeatpumpIR(), new GreeYANHeatpumpIR(), new GreeYAAHeatpumpIR(),
                            new FuegoHeatpumpIR(), new ToshibaHeatpumpIR(), new ToshibaDaiseikaiHeatpumpIR(),
                            new IVTHeatpumpIR(), new HitachiHeatpumpIR(),
                            new BalluHeatpumpIR(), new AUXHeatpumpIR(),
                            NULL};

IRSenderIRremoteESP8266 *Plugin_088_irSender;

int panasonicCKPTimer = 0;

boolean Plugin_088(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_088;
        Device[deviceCount].Type = DEVICE_TYPE_SINGLE;
        Device[deviceCount].VType = SENSOR_TYPE_NONE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = false;
        Device[deviceCount].ValueCount = 0;
        Device[deviceCount].SendDataOption = false;
        Device[deviceCount].TimerOption = false;
        Device[deviceCount].TimerOptional = false;
        Device[deviceCount].GlobalSyncOption = false;
        Device[deviceCount].DecimalsOnly = false;

        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_088);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        // We need the index of the controller we are: 0-CONTROLLER_MAX
        byte controllerNr = 0;
          for (byte i=0; i < CONTROLLER_MAX; i++)
          {
            if (Settings.Protocol[i] == 2) { controllerNr = i; }
          }

        if (Settings.ControllerEnabled[controllerNr])
        {
          addHtml(F("<TR><TD>IDX:<TD>"));
          String id = F("TDID");   //="taskdeviceid"
          id += controllerNr + 1;
          addNumericBox(id, Settings.TaskDeviceID[controllerNr][event->TaskIndex], 0, 9999);
        }
        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        success = true;
        break;
      }

    case PLUGIN_INIT:
      {
        int irPin = Settings.TaskDevicePin1[event->TaskIndex];
        if (irPin != -1)
        {
          addLog(LOG_LEVEL_INFO, F("P088: Heatpump IR transmitter activated"));
          if (Plugin_088_irSender != NULL)
          {
            delete Plugin_088_irSender;
          }
          Plugin_088_irSender = new IRSenderIRremoteESP8266(irPin);
        }
        if (Plugin_088_irSender != 0 && irPin == -1)
        {
          addLog(LOG_LEVEL_INFO, F("P088: Heatpump IR transmitter deactivated"));
          delete Plugin_088_irSender;
          Plugin_088_irSender = NULL;
        }
        success = true;
        break;
      }

    case PLUGIN_READ:
      {
        success = true;
        break;
      }

    case PLUGIN_WRITE:
      {
        String heatpumpModel;
        unsigned int powerMode = POWER_ON;
        unsigned int operatingMode = MODE_HEAT;
        unsigned int fanSpeed = FAN_2;
        unsigned int temperature = 22;
        unsigned int vDir = VDIR_UP;
        unsigned int hDir = HDIR_AUTO;
        char command[80];
        command[0] = 0;
        String TmpStr1 = "";
        string.toCharArray(command, 80);

        String tmpString = string;
        int argIndex = tmpString.indexOf(',');
        if (argIndex) tmpString = tmpString.substring(0, argIndex);

        if (tmpString.equalsIgnoreCase(F("HEATPUMPIR")) && Plugin_088_irSender != NULL)
        {
          if (GetArgv(command, TmpStr1, 2)) heatpumpModel = TmpStr1;
          if (GetArgv(command, TmpStr1, 3)) powerMode = str2int(TmpStr1.c_str());
          if (GetArgv(command, TmpStr1, 4)) operatingMode = str2int(TmpStr1.c_str());
          if (GetArgv(command, TmpStr1, 5)) fanSpeed = str2int(TmpStr1.c_str());
          if (GetArgv(command, TmpStr1, 6)) temperature = str2int(TmpStr1.c_str());
          if (GetArgv(command, TmpStr1, 7)) vDir = str2int(TmpStr1.c_str());
          if (GetArgv(command, TmpStr1, 8)) hDir = str2int(TmpStr1.c_str());

          int i = 0;
          do
          {
            const char* shortName = heatpumpIR[i]->model();

            if (strcmp_P(heatpumpModel.c_str(), shortName) == 0)
            {
              #ifdef PLUGIN_016
              if (irReceiver != 0)
              irReceiver->disableIRIn(); // Stop the receiver
              #endif
              heatpumpIR[i]->send(*Plugin_088_irSender, powerMode, operatingMode, fanSpeed, temperature, vDir, hDir);
              #ifdef PLUGIN_016
              if (irReceiver != 0)
              irReceiver->enableIRIn(); // Start the receiver
              #endif
              addLog(LOG_LEVEL_INFO, F("P088: Heatpump IR code transmitted"));
              if (printToWeb)
              {
                printWebString += F("P088: Heatpump IR code transmitted");
              }

              // Panasonic CKP can only be turned ON/OFF by using the timer,
              // so cancel the timer in 2 minutes, after the heatpump has turned on or off
              if (strcmp(heatpumpModel.c_str(), "panasonic_ckp") == 0)
              {
                panasonicCKPTimer = 120;
              }

              success = true;
              break;
            }
          }
          while (heatpumpIR[++i] != NULL);
        }
        break;
      }

    case PLUGIN_EXIT:
    	{
        addLog(LOG_LEVEL_INFO, F("P088: Heatpump IR transmitter deactivated"));

        if (Plugin_088_irSender != NULL)
        {
          delete Plugin_088_irSender;
        }

    	  break;
    	}

    case PLUGIN_ONCE_A_SECOND:
      {
        if (panasonicCKPTimer > 0)
        {
          panasonicCKPTimer--;
          if (panasonicCKPTimer == 0)
          {
            PanasonicCKPHeatpumpIR *panasonicHeatpumpIR = new PanasonicCKPHeatpumpIR();

            #ifdef PLUGIN_016
            if (irReceiver != 0)
            irReceiver->disableIRIn(); // Stop the receiver
            #endif
            panasonicHeatpumpIR->sendPanasonicCKPCancelTimer(*Plugin_088_irSender);
             #ifdef PLUGIN_016
            if (irReceiver != 0)
            irReceiver->enableIRIn(); // Start the receiver
            #endif
            addLog(LOG_LEVEL_INFO, F("P088: The TIMER led on Panasonic CKP should now be OFF"));
          }
        }
        success = true;
        break;
      }

    case PLUGIN_TEN_PER_SECOND:
      {
        success = true;
        break;
      }
  }

  return success;
}

#endif // USES_P088