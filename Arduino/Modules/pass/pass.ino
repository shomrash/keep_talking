#include "U8glib.h"
U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_gdr25);
 
  u8g.drawFrame(0,0,128,32);
  u8g.drawLine(31, 0, 31, 32);
  u8g.drawLine(63, 0, 63, 32);
  u8g.drawLine(94, 0, 94, 32);  
}

void setup(void) {
  // flip screen, if required
  // u8g.setRot180();
}

void loop(void) {
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(500);
}
