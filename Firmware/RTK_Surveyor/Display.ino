//Given the system state, display the appropriate information
void updateDisplay()
{
  //Update the display if connected
  if (online.display == true)
  {
    if (millis() - lastDisplayUpdate > 500 || forceDisplayUpdate == true) //Update display at 2Hz
    {
      lastDisplayUpdate = millis();
      forceDisplayUpdate = false;

      oled.clear(PAGE); // Clear the display's internal buffer

      switch (systemState)
      {
        case (STATE_ROVER_NOT_STARTED):
          //Do nothing. Static display shown during state change.
          break;
        case (STATE_ROVER_NO_FIX):
          paintRoverNoFix();
          break;
        case (STATE_ROVER_FIX):
          paintRoverFix();
          break;
        case (STATE_ROVER_RTK_FLOAT):
          paintRoverRTKFloat();
          break;
        case (STATE_ROVER_RTK_FIX):
          paintRoverRTKFix();
          break;
        case (STATE_BASE_NOT_STARTED):
          //Do nothing. Static display shown during state change.
          break;
        case (STATE_BASE_TEMP_SETTLE):
          paintBaseTempSettle();
          break;
        case (STATE_BASE_TEMP_SURVEY_STARTED):
          paintBaseTempSurveyStarted();
          break;
        case (STATE_BASE_TEMP_TRANSMITTING):
          paintBaseTempTransmitting();
          break;
        case (STATE_BASE_TEMP_WIFI_STARTED):
          paintBaseTempWiFiStarted();
          break;
        case (STATE_BASE_TEMP_WIFI_CONNECTED):
          paintBaseTempWiFiConnected();
          break;
        case (STATE_BASE_TEMP_CASTER_STARTED):
          paintBaseTempCasterStarted();
          break;
        case (STATE_BASE_TEMP_CASTER_CONNECTED):
          paintBaseTempCasterConnected();
          break;
        case (STATE_BASE_FIXED_NOT_STARTED):
          paintBaseFixedNotStarted();
          break;
        case (STATE_BASE_FIXED_TRANSMITTING):
          paintBaseFixedTransmitting();
          break;
        case (STATE_BASE_FIXED_WIFI_STARTED):
          paintBaseFixedWiFiStarted();
          break;
        case (STATE_BASE_FIXED_WIFI_CONNECTED):
          paintBaseFixedWiFiConnected();
          break;
        case (STATE_BASE_FIXED_CASTER_STARTED):
          paintBaseFixedCasterStarted();
          break;
        case (STATE_BASE_FIXED_CASTER_CONNECTED):
          paintBaseFixedCasterConnected();
          break;
        case (STATE_BUBBLE_LEVEL):
          paintBubbleLevel();
          break;
        case (STATE_MARK_EVENT):
          //Do nothing. Static display shown during state change.
          break;
        case (STATE_DISPLAY_SETUP):
          paintDisplaySetup();
          break;
        case (STATE_WIFI_CONFIG_NOT_STARTED):
          displayWiFiConfigNotStarted(); //Display 'WiFi Config'
          break;
        case (STATE_WIFI_CONFIG):
          displayWiFiConfig(); //Display SSID and IP
          break;
        case (STATE_TEST):
          //Do nothing
          break;
        case (STATE_TESTING):
          paintSystemTest();
          break;
        case (STATE_SHUTDOWN):
          displayShutdown();
          break;
        default:
          Serial.printf("Unknown display: %d\n\r", systemState);
          displayError("Display");
          break;
      }

      oled.display(); //Push internal buffer to display
    }
  }
}

void displaySplash()
{
  if (online.display == true)
  {
    //Init and display splash
    oled.begin();     // Initialize the OLED
    oled.clear(PAGE); // Clear the display's internal memory

    int yPos = 0;
    int fontHeight = 8;

    printTextCenter("SparkFun", yPos, 0, 1, false); //text, y, font type, kerning, inverted

    yPos = yPos + fontHeight + 2;
    printTextCenter("RTK", yPos, 1, 1, false);

    yPos = yPos + fontHeight + 5;

    if (productVariant == RTK_SURVEYOR)
    {
      printTextCenter("Surveyor", yPos, 1, 1, false);
    }
    else if (productVariant == RTK_EXPRESS)
    {
      printTextCenter("Express", yPos, 1, 1, false);
    }
    else if (productVariant == RTK_EXPRESS_PLUS)
    {
      printTextCenter("Express+", yPos, 1, 1, false);
    }
    else if (productVariant == RTK_FACET)
    {
      printTextCenter("Facet", yPos, 1, 1, false);
    }

    yPos = yPos + fontHeight + 7;
    char unitFirmware[50];
#ifdef ENABLE_DEVELOPER
    sprintf(unitFirmware, "v%d.%d-DEV", FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR);
#else
    sprintf(unitFirmware, "v%d.%d", FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR);
#endif
    printTextCenter(unitFirmware, yPos, 0, 1, false);

    oled.display();
  }
}

void displayShutdown()
{
  displayMessage("Shutting Down...", 0);
}

//Displays a small error message then hard freeze
//Text wraps and is small but legible
void displayError(const char * errorMessage)
{
  if (online.display == true)
  {
    oled.clear(PAGE); // Clear the display's internal buffer

    oled.setCursor(0, 0); //x, y
    oled.setFontType(0); //Set font to smallest
    oled.print(F("Error:"));

    oled.setCursor(2, 10);
    //oled.setFontType(1);
    oled.print(errorMessage);

    oled.display(); //Push internal buffer to display

    while (1) delay(10); //Hard freeze
  }
}

//Print the classic battery icon with levels
void paintBatteryLevel()
{
  if (online.display == true)
  {
    //Current battery charge level
    if (battLevel < 25)
      oled.drawIcon(45, 0, Battery_0_Width, Battery_0_Height, Battery_0, sizeof(Battery_0), true);
    else if (battLevel < 50)
      oled.drawIcon(45, 0, Battery_1_Width, Battery_1_Height, Battery_1, sizeof(Battery_1), true);
    else if (battLevel < 75)
      oled.drawIcon(45, 0, Battery_2_Width, Battery_2_Height, Battery_2, sizeof(Battery_2), true);
    else //batt level > 75
      oled.drawIcon(45, 0, Battery_3_Width, Battery_3_Height, Battery_3, sizeof(Battery_3), true);
  }
}

//Display Bluetooth icon, Bluetooth MAC, or WiFi depending on connection state
void paintWirelessIcon()
{
  if (online.display == true)
  {
    //Bluetooth icon if paired, or Bluetooth MAC address if not paired
    if (radioState == BT_CONNECTED)
    {
      oled.drawIcon(4, 0, BT_Symbol_Width, BT_Symbol_Height, BT_Symbol, sizeof(BT_Symbol), true);
    }
    else if (radioState == WIFI_ON_NOCONNECTION)
    {
      //Blink WiFi icon
      if (millis() - lastWifiIconUpdate > 500)
      {
        lastWifiIconUpdate = millis();
        if (wifiIconDisplayed == false)
        {
          wifiIconDisplayed = true;

          //Draw the icon
          oled.drawIcon(6, 1, WiFi_Symbol_Width, WiFi_Symbol_Height, WiFi_Symbol, sizeof(WiFi_Symbol), true);
        }
        else
          wifiIconDisplayed = false;
      }
    }
    else if (radioState == WIFI_CONNECTED)
    {
      //Solid WiFi icon
      oled.drawIcon(6, 1, WiFi_Symbol_Width, WiFi_Symbol_Height, WiFi_Symbol, sizeof(WiFi_Symbol), true);
    }
    else
    {
      char macAddress[5];
      sprintf(macAddress, "%02X%02X", unitMACAddress[4], unitMACAddress[5]);
      oled.setFontType(0); //Set font to smallest
      oled.setCursor(0, 4);
      oled.print(macAddress);
    }
  }
}

//Display cross hairs and horizontal accuracy
//Display double circle if we have RTK (blink = float, solid = fix)
void paintHorizontalAccuracy()
{
  if (online.display == true)
  {
    //Blink crosshair icon until we achieve <5m horz accuracy (user definable)
    if (systemState == STATE_BASE_TEMP_SETTLE)
    {
      if (millis() - lastCrosshairIconUpdate > 500)
      {
        lastCrosshairIconUpdate = millis();
        if (crosshairIconDisplayed == false)
        {
          crosshairIconDisplayed = true;

          //Draw the icon
          oled.drawIcon(0, 18, CrossHair_Width, CrossHair_Height, CrossHair, sizeof(CrossHair), true);
        }
        else
          crosshairIconDisplayed = false;
      }
    }
    else if (systemState == STATE_ROVER_RTK_FLOAT)
    {
      if (millis() - lastCrosshairIconUpdate > 500)
      {
        lastCrosshairIconUpdate = millis();
        if (crosshairIconDisplayed == false)
        {
          crosshairIconDisplayed = true;

          //Draw dual crosshair
          oled.drawIcon(0, 18, CrossHairDual_Width, CrossHairDual_Height, CrossHairDual, sizeof(CrossHairDual), true);
        }
        else
          crosshairIconDisplayed = false;
      }
    }
    else if (systemState == STATE_ROVER_RTK_FIX)
    {
      //Draw dual crosshair
      oled.drawIcon(0, 18, CrossHairDual_Width, CrossHairDual_Height, CrossHairDual, sizeof(CrossHairDual), true);
    }
    else
    {
      //Draw crosshair
      oled.drawIcon(0, 18, CrossHair_Width, CrossHair_Height, CrossHair, sizeof(CrossHair), true);
    }

    oled.setFontType(1); //Set font to type 1: 8x16
    oled.setCursor(16, 20); //x, y
    oled.print(":");
    float hpa = i2cGNSS.getHorizontalAccuracy() / 10000.0;
    if (hpa > 30.0)
    {
      oled.print(F(">30m"));
    }
    else if (hpa > 9.9)
    {
      oled.print(hpa, 1); //Print down to decimeter
    }
    else if (hpa > 1.0)
    {
      oled.print(hpa, 2); //Print down to centimeter
    }
    else
    {
      oled.print("."); //Remove leading zero
      oled.printf("%03d", (int)(hpa * 1000)); //Print down to millimeter
    }
  }
}

//Draw either a rover or base icon depending on screen
//Draw a different base if we have fixed coordinate base type
void paintBaseState()
{
  if (online.display == true)
  {
    if (systemState == STATE_ROVER_NO_FIX ||
        systemState == STATE_ROVER_FIX ||
        systemState == STATE_ROVER_RTK_FLOAT ||
        systemState == STATE_ROVER_RTK_FIX)
    {
      //Normal rover for ZED-F9P, fusion rover for ZED-F9R
      if (zedModuleType == PLATFORM_F9P)
      {
        oled.drawIcon(27, 3, Rover_Width, Rover_Height, Rover, sizeof(Rover), true);
      }
      else if (zedModuleType == PLATFORM_F9R)
      {
        //Blink fusion rover until we have calibration
        if (i2cGNSS.packetUBXESFSTATUS->data.fusionMode == 0) //Initializing
        {
          //Blink Fusion Rover icon until sensor calibration is complete
          if (millis() - lastBaseIconUpdate > 500)
          {
            lastBaseIconUpdate = millis();
            if (baseIconDisplayed == false)
            {
              baseIconDisplayed = true;

              //Draw the icon
              oled.drawIcon(27, 2, Rover_Fusion_Width, Rover_Fusion_Height, Rover_Fusion, sizeof(Rover_Fusion), true);
            }
            else
              baseIconDisplayed = false;
          }
        }
        else if (i2cGNSS.packetUBXESFSTATUS->data.fusionMode == 1) //Calibrated
        {
          //Solid fusion rover
          oled.drawIcon(27, 2, Rover_Fusion_Width, Rover_Fusion_Height, Rover_Fusion, sizeof(Rover_Fusion), true);
        }
        else if (i2cGNSS.packetUBXESFSTATUS->data.fusionMode == 2 || i2cGNSS.packetUBXESFSTATUS->data.fusionMode == 3) //Suspended or disabled
        {
          //Empty rover
          oled.drawIcon(27, 2, Rover_Fusion_Empty_Width, Rover_Fusion_Empty_Height, Rover_Fusion_Empty, sizeof(Rover_Fusion_Empty), true);
        }
      }
    }
    else if (systemState == STATE_BASE_TEMP_SETTLE ||
             systemState == STATE_BASE_TEMP_SURVEY_STARTED //Turn on base icon solid (blink crosshair in paintHorzAcc)
            )
    {
      //Blink base icon until survey is complete
      if (millis() - lastBaseIconUpdate > 500)
      {
        lastBaseIconUpdate = millis();
        if (baseIconDisplayed == false)
        {
          baseIconDisplayed = true;

          //Draw the icon
          oled.drawIcon(27, 0, BaseTemporary_Width, BaseTemporary_Height, BaseTemporary, sizeof(BaseTemporary), true); //true - blend with other pixels
        }
        else
          baseIconDisplayed = false;
      }
    }
    else if (systemState == STATE_BASE_TEMP_TRANSMITTING ||
             systemState == STATE_BASE_TEMP_WIFI_STARTED ||
             systemState == STATE_BASE_TEMP_WIFI_CONNECTED ||
             systemState == STATE_BASE_TEMP_CASTER_STARTED ||
             systemState == STATE_BASE_TEMP_CASTER_CONNECTED)
    {
      //Draw the icon
      oled.drawIcon(27, 0, BaseTemporary_Width, BaseTemporary_Height, BaseTemporary, sizeof(BaseTemporary), true); //true - blend with other pixels
    }
    else if (systemState == STATE_BASE_FIXED_TRANSMITTING ||
             systemState == STATE_BASE_FIXED_WIFI_STARTED ||
             systemState == STATE_BASE_FIXED_WIFI_CONNECTED ||
             systemState == STATE_BASE_FIXED_CASTER_STARTED ||
             systemState == STATE_BASE_FIXED_CASTER_CONNECTED)
    {
      //Draw the icon
      oled.drawIcon(27, 0, BaseFixed_Width, BaseFixed_Height, BaseFixed, sizeof(BaseFixed), true); //true - blend with other pixels
    }
  }
}

//Draw satellite icon and sats in view
//Blink icon if no fix
void paintSIV()
{
  if (online.display == true)
  {
    //Blink satellite dish icon if we don't have a fix
    if (i2cGNSS.getFixType() == 3 || i2cGNSS.getFixType() == 4 || i2cGNSS.getFixType() == 5) //3D, 3D+DR, or Time
    {
      //Fix, turn on icon
      oled.drawIcon(2, 35, Antenna_Width, Antenna_Height, Antenna, sizeof(Antenna), true);
    }
    else
    {
      if (millis() - lastSatelliteDishIconUpdate > 500)
      {
        //Serial.println("SIV Blink");
        lastSatelliteDishIconUpdate = millis();
        if (satelliteDishIconDisplayed == false)
        {
          satelliteDishIconDisplayed = true;

          //Draw the icon
          oled.drawIcon(2, 35, Antenna_Width, Antenna_Height, Antenna, sizeof(Antenna), true);
        }
        else
          satelliteDishIconDisplayed = false;
      }
    }

    oled.setFontType(1); //Set font to type 1: 8x16
    oled.setCursor(16, 36); //x, y
    oled.print(":");

    if (i2cGNSS.getFixType() == 0) //0 = No Fix
    {
      oled.print("0");
    }
    else
    {
      oled.print(i2cGNSS.getSIV());
    }

    if (settings.enableResetDisplay == true)
    {
      oled.setFontType(0); //Small font
      oled.setCursor(16 + (8 * 3) + 6, 38); //x, y
      oled.print(settings.resetCount);
    }
  }
}

//Draw log icon
//Turn off icon if log file fails to get bigger
void paintLogging()
{
  if (online.display == true)
  {
    if (logIncreasing == true)
    {
      //Animate icon to show system running
      if (millis() - lastLoggingIconUpdate > 500)
      {
        lastLoggingIconUpdate = millis();

        loggingIconDisplayed++; //Goto next icon
        loggingIconDisplayed %= 4; //Wrap
      }

      if (loggingIconDisplayed == 0)
        oled.drawIcon(64 - Logging_0_Width, 48 - Logging_0_Height, Logging_0_Width, Logging_0_Height, Logging_0, sizeof(Logging_0), true); //Draw the icon
      else if (loggingIconDisplayed == 1)
        oled.drawIcon(64 - Logging_1_Width, 48 - Logging_1_Height, Logging_1_Width, Logging_1_Height, Logging_1, sizeof(Logging_1), true); //Draw the icon
      else if (loggingIconDisplayed == 2)
        oled.drawIcon(64 - Logging_2_Width, 48 - Logging_2_Height, Logging_2_Width, Logging_2_Height, Logging_2, sizeof(Logging_2), true); //Draw the icon
      else if (loggingIconDisplayed == 3)
        oled.drawIcon(64 - Logging_3_Width, 48 - Logging_3_Height, Logging_3_Width, Logging_3_Height, Logging_3, sizeof(Logging_3), true); //Draw the icon
    }
  }
}

//Base screen. Display BLE, rover, battery, HorzAcc and SIV
//Blink SIV until fix
void paintRoverNoFix()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    paintHorizontalAccuracy();

    paintSIV();

    paintLogging();
  }
}

//Currently identical to RoverNoFix because paintSIV and paintHorizontalAccuracy takes into account system states
void paintRoverFix()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    paintHorizontalAccuracy();

    paintSIV();

    paintLogging();
  }
}

//Currently identical to RoverNoFix because paintSIV and paintHorizontalAccuracy takes into account system states
void paintRoverRTKFloat()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    paintHorizontalAccuracy();

    paintSIV();

    paintLogging();
  }
}

void paintRoverRTKFix()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    paintHorizontalAccuracy();

    paintSIV();

    paintLogging();
  }
}

//Start of base / survey in / NTRIP mode
//Screen is displayed while we are waiting for horz accuracy to drop to appropriate level
//Blink crosshair icon until we have we have horz accuracy < user defined level
void paintBaseTempSettle()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    paintHorizontalAccuracy(); //2nd line

    paintSIV();

    paintLogging();
  }
}

//Survey in is running. Show 3D Mean and elapsed time.
void paintBaseTempSurveyStarted()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    oled.setFontType(0);
    oled.setCursor(0, 23); //x, y
    oled.print("Mean:");

    oled.setCursor(29, 20); //x, y
    oled.setFontType(1);
    if (svinMeanAccuracy < 10.0) //Error check
      oled.print(svinMeanAccuracy, 2);
    else
      oled.print(">10");

    oled.setCursor(0, 39); //x, y
    oled.setFontType(0);
    oled.print("Time:");

    oled.setCursor(30, 36); //x, y
    oled.setFontType(1);
    if (svinObservationTime < 1000) //Error check
      oled.print(svinObservationTime);
    else
      oled.print("0");

    paintLogging();
  }
}

//Show transmission of RTCM packets
void paintBaseTempTransmitting()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    int textX = 1;
    int textY = 17;
    int textKerning = 8;
    oled.setFontType(1);
    printTextwithKerning("Xmitting", textX, textY, textKerning);

    oled.setCursor(0, 39); //x, y
    oled.setFontType(0);
    oled.print("RTCM:");

    if (rtcmPacketsSent < 100)
      oled.setCursor(30, 36); //x, y - Give space for two digits
    else
      oled.setCursor(28, 36); //x, y - Push towards colon to make room for log icon

    oled.setFontType(1); //Set font to type 1: 8x16
    oled.print(rtcmPacketsSent); //rtcmPacketsSent is controlled in processRTCM()

    paintLogging();
  }
}

//Show transmission of RTCM packets
//Blink WiFi icon
void paintBaseTempWiFiStarted()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    int textX = 1;
    int textY = 17;
    int textKerning = 8;
    oled.setFontType(1);
    printTextwithKerning("Xmitting", textX, textY, textKerning);

    oled.setCursor(0, 39); //x, y
    oled.setFontType(0);
    oled.print("RTCM:");

    if (rtcmPacketsSent < 100)
      oled.setCursor(30, 36); //x, y - Give space for two digits
    else
      oled.setCursor(28, 36); //x, y - Push towards colon to make room for log icon

    oled.setFontType(1); //Set font to type 1: 8x16
    oled.print(rtcmPacketsSent); //rtcmPacketsSent is controlled in processRTCM()

    paintLogging();
  }
}

//Show transmission of RTCM packets
//Solid WiFi icon
//This is identical to paintBaseTempWiFiStarted
void paintBaseTempWiFiConnected()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    int textX = 1;
    int textY = 17;
    int textKerning = 8;
    oled.setFontType(1);
    printTextwithKerning("Xmitting", textX, textY, textKerning);

    oled.setCursor(0, 39); //x, y
    oled.setFontType(0);
    oled.print("RTCM:");

    if (rtcmPacketsSent < 100)
      oled.setCursor(30, 36); //x, y - Give space for two digits
    else
      oled.setCursor(28, 36); //x, y - Push towards colon to make room for log icon

    oled.setFontType(1); //Set font to type 1: 8x16
    oled.print(rtcmPacketsSent); //rtcmPacketsSent is controlled in processRTCM()

    paintLogging();
  }
}

//Show connecting to caster service
//Solid WiFi icon
void paintBaseTempCasterStarted()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    int textX = 11;
    int textY = 17;
    int textKerning = 8;

    printTextwithKerning("Caster", textX, textY, textKerning);

    textX = 3;
    textY = 33;
    textKerning = 6;
    oled.setFontType(1);

    printTextwithKerning("Connecting", textX, textY, textKerning);
  }
}

//Show transmission of RTCM packets to caster service
//Solid WiFi icon
void paintBaseTempCasterConnected()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    int textX = 4;
    int textY = 17;
    int textKerning = 8;
    oled.setFontType(1);
    printTextwithKerning("Casting", textX, textY, textKerning);

    oled.setCursor(0, 39); //x, y
    oled.setFontType(0);
    oled.print("RTCM:");

    if (rtcmPacketsSent < 100)
      oled.setCursor(30, 36); //x, y - Give space for two digits
    else
      oled.setCursor(28, 36); //x, y - Push towards colon to make room for log icon

    oled.setFontType(1); //Set font to type 1: 8x16
    oled.print(rtcmPacketsSent); //rtcmPacketsSent is controlled in processRTCM()

    paintLogging();
  }
}

//Show transmission of RTCM packets
void paintBaseFixedNotStarted()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center
  }
}

//Show transmission of RTCM packets
void paintBaseFixedTransmitting()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    int textX = 1;
    int textY = 17;
    int textKerning = 8;
    oled.setFontType(1);
    printTextwithKerning("Xmitting", textX, textY, textKerning);

    oled.setCursor(0, 39); //x, y
    oled.setFontType(0);
    oled.print("RTCM:");

    if (rtcmPacketsSent < 100)
      oled.setCursor(30, 36); //x, y - Give space for two digits
    else
      oled.setCursor(28, 36); //x, y - Push towards colon to make room for log icon

    oled.setFontType(1); //Set font to type 1: 8x16
    oled.print(rtcmPacketsSent); //rtcmPacketsSent is controlled in processRTCM()

    paintLogging();
  }
}

//Show transmission of RTCM packets
//Blink WiFi icon
void paintBaseFixedWiFiStarted()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    int textX = 1;
    int textY = 17;
    int textKerning = 8;
    oled.setFontType(1);
    printTextwithKerning("Xmitting", textX, textY, textKerning);

    oled.setCursor(0, 39); //x, y
    oled.setFontType(0);
    oled.print("RTCM:");

    if (rtcmPacketsSent < 100)
      oled.setCursor(30, 36); //x, y - Give space for two digits
    else
      oled.setCursor(28, 36); //x, y - Push towards colon to make room for log icon

    oled.setFontType(1); //Set font to type 1: 8x16
    oled.print(rtcmPacketsSent); //rtcmPacketsSent is controlled in processRTCM()

    paintLogging();
  }
}

//Show transmission of RTCM packets
//Solid WiFi icon
//This is identical to paintBaseTempWiFiStarted
void paintBaseFixedWiFiConnected()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    int textX = 1;
    int textY = 17;
    int textKerning = 8;
    oled.setFontType(1);
    printTextwithKerning("Xmitting", textX, textY, textKerning);

    oled.setCursor(0, 39); //x, y
    oled.setFontType(0);
    oled.print("RTCM:");

    if (rtcmPacketsSent < 100)
      oled.setCursor(30, 36); //x, y - Give space for two digits
    else
      oled.setCursor(28, 36); //x, y - Push towards colon to make room for log icon

    oled.setFontType(1); //Set font to type 1: 8x16
    oled.print(rtcmPacketsSent); //rtcmPacketsSent is controlled in processRTCM()

    paintLogging();
  }
}

//Show connecting to caster service
//Solid WiFi icon
void paintBaseFixedCasterStarted()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    int textX = 11;
    int textY = 18;
    int textKerning = 8;

    printTextwithKerning("Caster", textX, textY, textKerning);

    textX = 3;
    textY = 33;
    textKerning = 6;
    oled.setFontType(1);

    printTextwithKerning("Connecting", textX, textY, textKerning);
  }
}

//Show transmission of RTCM packets to caster service
//Solid WiFi icon
void paintBaseFixedCasterConnected()
{
  if (online.display == true)
  {
    paintBatteryLevel(); //Top right corner

    paintWirelessIcon(); //Top left corner

    paintBaseState(); //Top center

    int textX = 4;
    int textY = 17;
    int textKerning = 8;
    oled.setFontType(1);
    printTextwithKerning("Casting", textX, textY, textKerning);

    oled.setCursor(0, 39); //x, y
    oled.setFontType(0);
    oled.print("RTCM:");

    if (rtcmPacketsSent < 100)
      oled.setCursor(30, 36); //x, y - Give space for two digits
    else
      oled.setCursor(28, 36); //x, y - Push towards colon to make room for log icon

    oled.setFontType(1); //Set font to type 1: 8x16
    oled.print(rtcmPacketsSent); //rtcmPacketsSent is controlled in processRTCM()

    paintLogging();
  }
}

void displayBaseStart(uint16_t displayTime)
{
  if (online.display == true)
  {
    oled.clear(PAGE);

    uint8_t fontHeight = 15; //Assume fontsize 1
    uint8_t yPos = LCDHEIGHT / 2 - fontHeight;

    printTextCenter("Base", yPos, 1, 1, false);  //text, y, font type, kerning, inverted
    oled.display();

    oled.display();

    delay(displayTime);
  }
}

void displayBaseSuccess(uint16_t displayTime)
{
  if (online.display == true)
  {
    oled.clear(PAGE);

    uint8_t fontHeight = 15; //Assume fontsize 1
    uint8_t yPos = LCDHEIGHT / 2 - fontHeight;

    printTextCenter("Base", yPos, 1, 1, false);  //text, y, font type, kerning, inverted
    printTextCenter("Started", yPos + fontHeight, 1, 1, false);  //text, y, font type, kerning, inverted

    oled.display();

    delay(displayTime);
  }
}

void displayBaseFail(uint16_t displayTime)
{
  if (online.display == true)
  {
    oled.clear(PAGE);

    uint8_t fontHeight = 15; //Assume fontsize 1
    uint8_t yPos = LCDHEIGHT / 2 - fontHeight;

    printTextCenter("Base", yPos, 1, 1, false);  //text, y, font type, kerning, inverted
    printTextCenter("Failed", yPos + fontHeight, 1, 1, false);  //text, y, font type, kerning, inverted

    oled.display();

    delay(displayTime);
  }
}

void displayGNSSFail(uint16_t displayTime)
{
  displayMessage("GNSS Failed", displayTime);
}

void displayRoverStart(uint16_t displayTime)
{
  if (online.display == true)
  {
    oled.clear(PAGE);

    uint8_t fontHeight = 15;
    uint8_t yPos = LCDHEIGHT / 2 - fontHeight;

    printTextCenter("Rover", yPos, 1, 1, false);  //text, y, font type, kerning, inverted
    //  printTextCenter("Started", yPos + fontHeight, 1, 1, false);  //text, y, font type, kerning, inverted

    oled.display();

    delay(displayTime);
  }
}

void displayRoverSuccess(uint16_t displayTime)
{
  if (online.display == true)
  {
    oled.clear(PAGE);

    uint8_t fontHeight = 15;
    uint8_t yPos = LCDHEIGHT / 2 - fontHeight;

    printTextCenter("Rover", yPos, 1, 1, false);  //text, y, font type, kerning, inverted
    printTextCenter("Started", yPos + fontHeight, 1, 1, false);  //text, y, font type, kerning, inverted

    oled.display();

    delay(displayTime);
  }
}

void displayRoverFail(uint16_t displayTime)
{
  if (online.display == true)
  {
    oled.clear(PAGE);

    uint8_t fontHeight = 15;
    uint8_t yPos = LCDHEIGHT / 2 - fontHeight;

    printTextCenter("Rover", yPos, 1, 1, false);  //text, y, font type, kerning, inverted
    printTextCenter("Failed", yPos + fontHeight, 1, 1, false);  //text, y, font type, kerning, inverted

    oled.display();

    delay(displayTime);
  }
}

//When user enters serial config menu the display will freeze so show splash while config happens
void displaySerialConfig()
{
  displayMessage("Serial Config", 0);
}

//When user enters WiFi Config mode from setup, show splash while config happens
void displayWiFiConfigNotStarted()
{
  displayMessage("WiFi Config", 0);
}

void displayWiFiConfig()
{
  //Draw the WiFi icon
  if (radioState == WIFI_ON_NOCONNECTION)
  {
    //Blink WiFi icon
    if (millis() - lastWifiIconUpdate > 500)
    {
      lastWifiIconUpdate = millis();
      if (wifiIconDisplayed == false)
      {
        wifiIconDisplayed = true;

        //Draw the icon
        oled.drawIcon((LCDWIDTH / 2) - (WiFi_Symbol_Width / 2), 0, WiFi_Symbol_Width, WiFi_Symbol_Height, WiFi_Symbol, sizeof(WiFi_Symbol), true);
      }
      else
        wifiIconDisplayed = false;
    }
  }
  else if (radioState == WIFI_CONNECTED)
  {
    //Solid WiFi icon
    oled.drawIcon((LCDWIDTH / 2) - (WiFi_Symbol_Width / 2), 0, WiFi_Symbol_Width, WiFi_Symbol_Height, WiFi_Symbol, sizeof(WiFi_Symbol), true);
  }


  int yPos = WiFi_Symbol_Height + 3;
  int fontHeight = 8;

  printTextCenter("SSID:", yPos, 0, 1, false); //text, y, font type, kerning, inverted

  yPos = yPos + fontHeight + 1;
  printTextCenter("RTK Config", yPos, 0, 1, false);

  yPos = yPos + fontHeight + 3;
  printTextCenter("IP:", yPos, 0, 1, false);

  yPos = yPos + fontHeight + 1;

  //char temp[50];
  //sprintf(temp, "%s", WiFi.softAPIP());
  //printTextCenter((const*)temp, yPos, 0, 1, false);
  printTextCenter("192.168.4.1", yPos, 0, 1, false);
}

//When user does a factory reset, let us know
void displaySytemReset()
{
  displayMessage("Factory Reset", 0);
}

void displaySurveyStart(uint16_t displayTime)
{
  if (online.display == true)
  {
    oled.clear(PAGE);

    uint8_t fontHeight = 15;
    uint8_t yPos = LCDHEIGHT / 2 - fontHeight;

    printTextCenter("Survey", yPos, 1, 1, false);  //text, y, font type, kerning, inverted
    //printTextCenter("Started", yPos + fontHeight, 1, 1, false);  //text, y, font type, kerning, inverted

    oled.display();

    delay(displayTime);
  }
}

void displaySurveyStarted(uint16_t displayTime)
{
  if (online.display == true)
  {
    oled.clear(PAGE);

    uint8_t fontHeight = 15;
    uint8_t yPos = LCDHEIGHT / 2 - fontHeight;

    printTextCenter("Survey", yPos, 1, 1, false);  //text, y, font type, kerning, inverted
    printTextCenter("Started", yPos + fontHeight, 1, 1, false);  //text, y, font type, kerning, inverted

    oled.display();

    delay(displayTime);
  }
}

//If the SD card is detected but is not formatted correctly, display warning
void displaySDFail(uint16_t displayTime)
{
  if (online.display == true)
  {
    oled.clear(PAGE);

    uint8_t fontHeight = 15;
    uint8_t yPos = LCDHEIGHT / 2 - fontHeight;

    printTextCenter("Format", yPos, 1, 1, false);  //text, y, font type, kerning, inverted
    printTextCenter("SD Card", yPos + fontHeight, 1, 1, false);  //text, y, font type, kerning, inverted

    oled.display();

    delay(displayTime);
  }
}

//Draw a frame at outside edge
void drawFrame()
{
  //Init and draw box at edge to see screen alignment
  int xMax = 63;
  int yMax = 47;
  oled.line(0, 0, xMax, 0); //Top
  oled.line(0, 0, 0, yMax); //Left
  oled.line(0, yMax, xMax, yMax); //Bottom
  oled.line(xMax, 0, xMax, yMax); //Right
}

void displayForcedFirmwareUpdate()
{
  displayMessage("Forced Update", 0);
}

void displayFirmwareUpdateProgress(int percentComplete)
{
  //Update the display if connected
  if (online.display == true)
  {
    oled.clear(PAGE); // Clear the display's internal buffer

    int yPos = 3;
    int fontHeight = 8;

    printTextCenter("Firmware", yPos, 0, 1, false); //text, y, font type, kerning, inverted

    yPos = yPos + fontHeight + 1;
    printTextCenter("Update", yPos, 0, 1, false); //text, y, font type, kerning, inverted

    yPos = yPos + fontHeight + 3;
    char temp[50];
    sprintf(temp, "%d%%", percentComplete);
    printTextCenter(temp, yPos, 1, 1, false); //text, y, font type, kerning, inverted

    oled.display(); //Push internal buffer to display
  }
}

void displayEventMarked(uint16_t displayTime)
{
  displayMessage("Event Marked", displayTime);
}

void displayNoLogging(uint16_t displayTime)
{
  displayMessage("No Logging", displayTime);
}

//Display unit self-tests until user presses a button to exit
//Allows operator to check:
// Display alignment
// Internal connections to: SD, Accel, Fuel guage, GNSS
// External connections: Loop back test on DATA
void paintSystemTest()
{
  if (online.display == true)
  {
    int xOffset = 2;
    int yOffset = 2;

    int charHeight = 7;

    char macAddress[5];
    sprintf(macAddress, "%02X%02X", unitMACAddress[4], unitMACAddress[5]);

    drawFrame(); //Outside edge

    //Test SD, accel, batt, GNSS, mux
    oled.setFontType(0); //Set font to smallest
    oled.setCursor(xOffset, yOffset); //x, y
    oled.print(F("SD:"));

    if (online.microSD == false)
      beginSD(); //Test if SD is present
    if (online.microSD == true)
      oled.print(F("OK"));
    else
      oled.print(F("FAIL"));

    oled.setCursor(xOffset, yOffset + (1 * charHeight) ); //x, y
    oled.print(F("Accel:"));
    if (online.accelerometer == true)
      oled.print(F("OK"));
    else
      oled.print(F("FAIL"));

    oled.setCursor(xOffset, yOffset + (2 * charHeight) ); //x, y
    oled.print(F("Batt:"));
    if (online.battery == true)
      oled.print(F("OK"));
    else
      oled.print(F("FAIL"));

    i2cGNSS.checkUblox();
    oled.setCursor(xOffset, yOffset + (3 * charHeight) ); //x, y
    oled.print(F("GNSS:"));
    int satsInView = i2cGNSS.getSIV();
    if (online.gnss == true && satsInView > 8)
    {
      oled.print(F("OK"));
      oled.print(F("/"));
      oled.print(satsInView);
    }
    else
      oled.print(F("FAIL"));

    oled.setCursor(xOffset, yOffset + (4 * charHeight) ); //x, y
    oled.print(F("Mux:"));

    //Set mux to channel 3 and toggle pin and verify with loop back jumper wire inserted by test technician

    setMuxport(MUX_ADC_DAC); //Set mux to DAC so we can toggle back/forth
    pinMode(pin_dac26, OUTPUT);
    pinMode(pin_adc39, INPUT_PULLUP);

    digitalWrite(pin_dac26, HIGH);
    if (digitalRead(pin_adc39) == HIGH)
    {
      digitalWrite(pin_dac26, LOW);
      if (digitalRead(pin_adc39) == LOW)
        oled.print(F("OK"));
      else
        oled.print(F("FAIL"));
    }
    else
      oled.print(F("FAIL"));

    //Display MAC address
    oled.setCursor(xOffset, yOffset + (5 * charHeight) ); //x, y
    oled.print(macAddress);
    oled.print(":");
    if (incomingBTTest == 0)
      oled.print(F("FAIL"));
    else
    {
      oled.write(incomingBTTest);
      oled.print(F("-OK"));
    }
  }
}

//Globals but only used for Bubble Level
double averagedRoll = 0.0;
double averagedPitch = 0.0;

//A bubble level
void paintBubbleLevel()
{
  if (online.display == true)
  {
    forceDisplayUpdate = true; //Update the display as quickly as possible

    getAngles();

    //Draw dot in middle
    oled.pixel(LCDWIDTH / 2, LCDHEIGHT / 2);
    oled.pixel(LCDWIDTH / 2 + 1, LCDHEIGHT / 2);
    oled.pixel(LCDWIDTH / 2, LCDHEIGHT / 2 + 1);
    oled.pixel(LCDWIDTH / 2 + 1, LCDHEIGHT / 2 + 1);

    //Draw circle relative to dot
    const int radiusLarge = 10;
    const int radiusSmall = 4;

    oled.circle(LCDWIDTH / 2 - averagedPitch, LCDHEIGHT / 2 + averagedRoll, radiusLarge);
    oled.circle(LCDWIDTH / 2 - averagedPitch, LCDHEIGHT / 2 + averagedRoll, radiusSmall);
  }
}

void getAngles()
{
  if (online.accelerometer == true)
  {
    averagedRoll = 0.0;
    averagedPitch = 0.0;
    const int avgAmount = 16;

    //Take an average readings
    for (int reading = 0 ; reading < avgAmount ; reading++)
    {
      while (accel.available() == false) delay(1);

      float accelX = 0;
      float accelY = 0;
      float accelZ = 0;

      //Express Accel orientation is different from Facet
      if (productVariant == RTK_EXPRESS || productVariant == RTK_EXPRESS_PLUS)
      {
        accelX = accel.getX();
        accelZ = accel.getY();
        accelY = accel.getZ();
        accelZ *= -1.0;
        accelX *= -1.0;
      }
      else if (productVariant == RTK_FACET)
      {
        accelZ = accel.getX();
        accelX = accel.getY();
        accelY = accel.getZ();
        accelZ *= -1.0;
        accelY *= -1.0;
        accelX *= -1.0;
      }

      double roll = atan2(accelY , accelZ) * 57.3;
      double pitch = atan2((-accelX) , sqrt(accelY * accelY + accelZ * accelZ)) * 57.3;

      averagedRoll += roll;
      averagedPitch += pitch;
    }

    averagedRoll /= (float)avgAmount;
    averagedPitch /= (float)avgAmount;

    //Avoid -0 since we're not printing the decimal portion
    if (averagedRoll < 0.5 && averagedRoll > -0.5) averagedRoll = 0;
    if (averagedPitch < 0.5 && averagedPitch > -0.5) averagedPitch = 0;
  }
}

//Show transmission of RTCM packets to caster service
//Solid WiFi icon
void paintDisplaySetup()
{
  if (online.display == true)
  {
    if (zedModuleType == PLATFORM_F9P)
    {
      if (setupState == STATE_MARK_EVENT)
      {
        printTextCenter("Mark", 12 * 0, 1, 1, true); //string, y, font type, kerning, inverted
        printTextCenter("Rover", 12 * 1, 1, 1, false);
        printTextCenter("Base", 12 * 2, 1, 1, false);
        printTextCenter("Bubble", 12 * 3, 1, 1, false);
      }
      else if (setupState == STATE_ROVER_NOT_STARTED)
      {
        printTextCenter("Mark", 12 * 0, 1, 1, false);
        printTextCenter("Rover", 12 * 1, 1, 1, true);
        printTextCenter("Base", 12 * 2, 1, 1, false);
        printTextCenter("Bubble", 12 * 3, 1, 1, false);
      }
      else if (setupState == STATE_BASE_NOT_STARTED)
      {
        printTextCenter("Mark", 12 * 0, 1, 1, false); //string, y, font type, kerning, inverted
        printTextCenter("Rover", 12 * 1, 1, 1, false);
        printTextCenter("Base", 12 * 2, 1, 1, true);
        printTextCenter("Bubble", 12 * 3, 1, 1, false);
      }
      else if (setupState == STATE_BUBBLE_LEVEL)
      {
        printTextCenter("Mark", 12 * 0, 1, 1, false); //string, y, font type, kerning, inverted
        printTextCenter("Rover", 12 * 1, 1, 1, false);
        printTextCenter("Base", 12 * 2, 1, 1, false);
        printTextCenter("Bubble", 12 * 3, 1, 1, true);
      }
      else if (setupState == STATE_WIFI_CONFIG_NOT_STARTED)
      {
        printTextCenter("Rover", 12 * 0, 1, 1, false);
        printTextCenter("Base", 12 * 1, 1, 1, false);
        printTextCenter("Bubble", 12 * 2, 1, 1, false);
        printTextCenter("Config", 12 * 3, 1, 1, true);
      }
    } //end type F9P
    else if (zedModuleType == PLATFORM_F9R)
    {
      if (setupState == STATE_MARK_EVENT)
      {
        printTextCenter("Mark", 12 * 0, 1, 1, true); //string, y, font type, kerning, inverted
        printTextCenter("Rover", 12 * 1, 1, 1, false);
        printTextCenter("Bubble", 12 * 2, 1, 1, false);
        printTextCenter("Config", 12 * 3, 1, 1, false);
      }
      else if (setupState == STATE_ROVER_NOT_STARTED)
      {
        printTextCenter("Mark", 12 * 0, 1, 1, false);
        printTextCenter("Rover", 12 * 1, 1, 1, true);
        printTextCenter("Bubble", 12 * 2, 1, 1, false);
        printTextCenter("Config", 12 * 3, 1, 1, false);
      }
      else if (setupState == STATE_BUBBLE_LEVEL)
      {
        printTextCenter("Mark", 12 * 0, 1, 1, false);
        printTextCenter("Rover", 12 * 1, 1, 1, false);
        printTextCenter("Bubble", 12 * 2, 1, 1, true);
        printTextCenter("Config", 12 * 3, 1, 1, false);
      }
      else if (setupState == STATE_WIFI_CONFIG_NOT_STARTED)
      {
        printTextCenter("Mark", 12 * 0, 1, 1, false);
        printTextCenter("Rover", 12 * 1, 1, 1, false);
        printTextCenter("Bubble", 12 * 2, 1, 1, false);
        printTextCenter("Config", 12 * 3, 1, 1, true);
      }
    } //end type F9R
  } //end display online
}

//Given text, and location, print text center of the screen
void printTextCenter(const char *text, uint8_t yPos, uint8_t fontType, uint8_t kerning, bool highlight) //text, y, font type, kearning, inverted
{
  uint8_t fontWidth = 0;
  if (fontType == 0) fontWidth = 5; //Smallest 5x7
  if (fontType == 1)
  {
    fontWidth = 7; //8x16, but widest character is only 7 pixels.
  }

  uint8_t xStart = (LCDWIDTH / 2) - ((strlen(text) * (fontWidth + kerning)) / 2);

  oled.setFontType(fontType);
  oled.setDrawMode(XOR);
  uint8_t xPos = xStart;
  for (int x = 0 ; x < strlen(text) ; x++)
  {
    oled.setCursor(xPos, yPos);
    oled.print(text[x]);
    xPos += fontWidth + kerning;
  }

  if (highlight) //Draw a box, inverted over text
  {
    uint8_t textPixelWidth = strlen(text) * (fontWidth + kerning);
    oled.rectFill(xStart - 5, yPos, textPixelWidth + 9, 11, WHITE, XOR); //x, y, width, height, color, mode
  }
}

//Given a message (one or two words) display centered
void displayMessage(const char* message, uint16_t displayTime)
{
  if (online.display == true)
  {
    char temp[20];
    uint8_t fontHeight = 15; //Assume fontsize 1

    //Count words based on spaces
    uint8_t wordCount = 0;
    strcpy(temp, message); //strtok modifies the message so make copy
    char * token = strtok(temp, " ");
    while (token != NULL)
    {
      wordCount++;
      token = strtok(NULL, " ");
    }

    uint8_t yPos = (LCDHEIGHT / 2) - (fontHeight / 2);
    if (wordCount == 2) yPos -= (fontHeight / 2);

    oled.clear(PAGE);

    //drawFrame();

    strcpy(temp, message);
    token = strtok(temp, " ");
    while (token != NULL)
    {
      printTextCenter(token, yPos, 1, 1, false); //text, y, font type, kerning, inverted
      token = strtok(NULL, " ");
      yPos += fontHeight;
    }

    oled.display();

    delay(displayTime);
  }
}
