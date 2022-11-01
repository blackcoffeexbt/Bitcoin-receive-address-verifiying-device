#include <Arduino.h>
#include <ESP32QRCodeReader.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include "Bitcoin.h"

/**
 * For use with ST3375 and ESP32-Cam
 */

ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);

#define TFT_SCLK 14 // SCL
#define TFT_MOSI 13 // SDA
#define TFT_RST  12 // RES (RESET)
#define TFT_DC    2 // Data Command control pin (DC)
#define TFT_CS   15 // Chip select control pin (CS)
                    // BL (back light) and VCC -> 3V3

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

String pub = "zpub6r6Fo22jHfnfmyXKRDtq7WNquZ4WynwUgBbfVXdXLJA8jipdaeULX1CTSLcmnwgmMcWMdiqFm63bPG5pyLXiP6V6nvnzT5QJXb7TYrdz5YG";
int32_t maxAddresses = 200;

bool hasDecodedQr = false;
const char* payload;
const char* payloadMessage;
TaskHandle_t xHandle;

void onQrCodeTask(void *pvParameters)
{
  struct QRCodeData qrCodeData;

  while (true)
  {
    if (reader.receiveQrCode(&qrCodeData, 100))
    {
      Serial.println("Found QRCode");
      if (qrCodeData.valid)
      {
        Serial.print("Payload: ");
        Serial.println((const char *)qrCodeData.payload);
        payloadMessage = "QR code detected. Reset to scan again.";
        payload = (const char *)qrCodeData.payload;
        hasDecodedQr = true;
        vTaskSuspend( xHandle );
      }
      else
      {
        Serial.print("Invalid: ");
        Serial.println((const char *)qrCodeData.payload);
//        payloadMessage = "Couldn't read QR code:";
//        payload = (const char *)qrCodeData.payload;
//        showPayload();
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);

  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  initTft();

  tft_drawtext(20, 20, "Ready.", 2, ST77XX_GREEN);
  tft_drawtext(10, 70, "Scan a bitcoin", 1, ST77XX_GREEN);
  tft_drawtext(10, 80, "receive address", 1, ST77XX_GREEN);
  tft_drawtext(10, 90, "QR code", 1, ST77XX_GREEN);

  reader.setup();

  Serial.println("Setup QRCode Reader");

  reader.beginOnCore(1);

  Serial.println("Begin on Core 1");

  xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, &xHandle);
}

void initTft() {
  // TFT display init
  tft.initR(INITR_BLACKTAB); // you might need to use INITR_REDTAB or INITR_BLACKTAB to get correct text colors
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
  Serial.println("Loop");
  if(hasDecodedQr) {
    initTft();
    checkIsValidAddress((String)payload);
    while(true){}
  }
  delay(100);
  }

// draw test on TFT
void tft_drawtext(int16_t x, int16_t y, String text, uint8_t font_size, uint16_t color) {
  tft.setCursor(x, y);
  tft.setTextSize(font_size); // font size 1 = 6x8, 2 = 12x16, 3 = 18x24
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(strcpy(new char[text.length() + 1], text.c_str()));
}

void checkIsValidAddress(String addressToCheck) {
  bool foundAddress = false;
  addressToCheck.replace("bitcoin:", "");
  Serial.println("Checking " + addressToCheck);

  tft.fillScreen(ST77XX_BLACK);
  tft_drawtext(0, 5, "Checking address", 1, ST7735_BLUE);
  tft_drawtext(0, 20, addressToCheck, 1, ST7735_BLUE);
  tft_drawtext(0, 45, "Can be derived", 1, ST7735_BLUE);
  tft_drawtext(0, 50, "from xpub:", 1, ST7735_BLUE);
  tft_drawtext(0, 70, pub, 1, ST7735_BLUE);
  tft_drawtext(0, 130, "This may take some", 1, ST7735_BLUE);
  tft_drawtext(0, 140, "time....", 1, ST7735_BLUE);
  
  HDPublicKey hd(pub);

  if(!hd){ // check if it is valid
    Serial.println("Invalid xpub");
    return;
  }

//  Serial.println("Master public key:");
//  Serial.println(hd);
//  Serial.println("First 5 receiving addresses:");
  for(int32_t i=0; i<maxAddresses; i++){
    String path = String("m/0/")+i;
      Serial.println(i);
//     Serial.println(hd.derive(path).segwitAddress());
//     tft.fillScreen(ST77XX_BLACK);
//     tft_drawtext(10, 80, "Checking address " + String(i), 1, ST7735_BLUE);
     
     if(addressToCheck == (String)hd.derive(path).segwitAddress()) {
      i = maxAddresses;
      foundAddress = true;
      Serial.println("Found address");
     }
  }

  if(foundAddress) {
    tft.fillScreen(ST77XX_BLACK);
    tft_drawtext(0, 10, addressToCheck, 1, ST7735_GREEN);
    tft_drawtext(10, 45, "Is valid", 2, ST7735_GREEN);
    tft_drawtext(10, 80, "Reset to check", 1, ST7735_GREEN);
    tft_drawtext(10, 90, "another address", 1, ST7735_GREEN);
  } else {      
    tft.fillScreen(ST77XX_BLACK);
    tft_drawtext(10, 10, "Sorry :(", 2, ST7735_RED);
    tft_drawtext(10, 40, "I didn't find", 1, ST7735_RED);
    tft_drawtext(10, 50, "this address", 1, ST7735_RED);
    tft_drawtext(10, 60, "in the first " + (String)maxAddresses, 1, ST7735_RED);
    tft_drawtext(10, 70, "receive addresses.", 1, ST7735_RED);
    tft_drawtext(10, 90, "Reset to scan", 1, ST7735_BLUE);
    tft_drawtext(10, 100, "another.", 1, ST7735_BLUE);
  }
}
