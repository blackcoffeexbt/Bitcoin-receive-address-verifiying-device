Bitcoin address verifier device
----

For use with ST3375 and ESP32-Cam

1. Set an xpub on line 22
1. Set a maxAddress limit. This is the max number of addresses that will be derived from the xpub when checking the scanned address
1. Connect ST7735 to ESP32-Cam
1. Set board as AI Thinker ESP32-CAM
1. Upload to ESP-32
1. Scan a QR code


Pin setup
----
#define TFT_SCLK 14 // SCL
#define TFT_MOSI 13 // SDA
#define TFT_RST  12 // RES (RESET)
#define TFT_DC    2 // Data Command control pin (DC)
#define TFT_CS   15 // Chip select control pin (CS)
                    // BL (back light) and VCC -> 3V3 
