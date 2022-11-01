Bitcoin address verifier device
----

For use with ST3375 and ESP32-Cam


Pin setup
----
#define TFT_SCLK 14 // SCL
#define TFT_MOSI 13 // SDA
#define TFT_RST  12 // RES (RESET)
#define TFT_DC    2 // Data Command control pin (DC)
#define TFT_CS   15 // Chip select control pin (CS)
                    // BL (back light) and VCC -> 3V3 
