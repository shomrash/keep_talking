#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  delay(2000);
  display.setFont(&FreeSerif9pt7b);
  display.clearDisplay();

  for (byte i = 0; i < 4; i++)
  {
    display.drawRect(i*32, 0, 32, display.height(), SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
  }
  
  display.setTextSize(2);             
  display.setTextColor(SSD1306_INVERSE);        
  display.setCursor(3,26);             
  display.println("P");
  display.setCursor(32+3,26);             
  display.println("I");
  display.setCursor(64+3,26);             
  display.println("D");
  display.setCursor(98+3,26);             
  display.println("R");
  display.display();
  delay(2000); 
}
void loop() {
 }