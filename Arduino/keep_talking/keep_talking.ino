#include "FastLED.h"
#include "GyverButton.h"
#include "GyverTimer.h"
#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int curent_module = 2;      //номер выбранного модуля
int errors = 0;             //кол-во ошибок
bool new_module = false;    //переменная для перехода 
int t_time = 0;             //переменная времени

//отвечает за индикацию модулей
#define cm_led_pin 11
CRGB cm_leds[8];

void setup(){
    Serial.begin(9600);
    setup_module();

    //FastLED.addLeds<WS2811, cm_led_pin, GRB>(cm_leds, 8).setCorrection(TypicalLEDStrip); //лента для индикации модулей
    FastLED.setBrightness(255);
}

void loop(){
    switch (curent_module)
    {
    case 1:
        loop_wires();
        break;
    case 2:
        loop_button();
        break;
    case 4:
        loop_morze();
        break;
    case 9: 
        loop_password();
    default:
        test();
        break;
    }

    if (new_module)
    {
        setup_module();
    } 
    //show_curent_module(); //вызов метода для отображения индикации модулей
    timer();
}
//тестовый воид
void test(){
    pinMode(2, OUTPUT);

    digitalWrite(2, 1);

}


//блок с модулем "1. Провода"
int w_right = 2; //номер верного провода
bool w_state[6];

void setup_wires(){
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(6, INPUT_PULLUP);
    pinMode(7, INPUT_PULLUP);

    for (int i = 0; i < 6; i++)
    {
        w_state[i] = false;
    }
}

void loop_wires(){
    for (int i = 0; i < 6; i++)
    {
        if (!w_state[i] && digitalRead(i+2) != w_state[i]){
            if (i == w_right)
            {
                next();
                w_state[i] = !w_state[i];
            }else   
            {
                error();
                w_state[i] = !w_state[i];
            }
            
            
        }
    }
}

//блок с модулем 2. Кнопка
#define b_button_pin 6
#define b_led_pin 11
int b_brightness = 0;
bool b_dir;
byte b_time;
CRGB b_leds[8];
GButton b_butt(b_button_pin);
GTimer b_timer(MS, 15);

void setup_button(){
    pinMode(b_button_pin, INPUT_PULLUP);
    pinMode(b_led_pin, OUTPUT);

    
    pinMode(10, OUTPUT);
    pinMode(12, OUTPUT);
    digitalWrite(10,0);
    digitalWrite(12,1);
    

    b_butt.setTickMode(MANUAL);

    FastLED.addLeds<WS2811, b_led_pin, GRB>(b_leds, 8).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(50);
}
void loop_button(){
    if (b_timer.isReady()){
        b_butt.tick();
        
        if (!b_butt.isClick()){
            if (b_butt.isHolded()){
                b_brightness = 100;
                b_dir = true;
                b_time = random(0,6);
                
                //b_time = b_butt.getHoldClicks();

                Serial.write("Holding. Release on: ");
                Serial.println(map(b_time, 0,5,0,9));
            }
            if (b_butt.isStep()){
                if (b_dir){
                    b_brightness += 5;
                }else
                {
                    b_brightness -= 5;
                }
                if (b_brightness <= 100)
                {
                    b_dir = true;
                }
                if (b_brightness >= 200)
                {
                    b_dir = false;
                }
            }
            if (b_butt.isRelease()){
                b_brightness = 0;
                int b_t = t_time - (int)(t_time/60)*60;
                b_t -= (int)(b_t/10)*10;

                Serial.println(b_t);

                if (map(b_time, 0,5,0,9) == b_t){
                    next();
                }else
                {
                    error();
                }
            }
        }
        

        for (int i = 0; i < 8; i++)
        {
            int color = map(b_time, 0,5,0,230);
            b_leds[i] = CHSV(color,255,b_brightness);
        }
        
        FastLED.show();
    }
}

//модуль 4. Азбука морзе
#define m_pot_pin 20
#define m_led_pin 2
#define m_but_pin 3
GButton m_butt(m_but_pin);
GTimer m_timer_f(MS, 500);
byte m_old_pos = 0;
byte m_pot_pos = 0;
int64_t m_morze = 0B10101000101100010110110100010110110100011010110110;//**** *- *--* *--* -*-- 50
int64_t m_mask = 1;
byte m_pos = 54; 
void setup_morze(){
    pinMode(m_pot_pin, INPUT);
    pinMode(m_led_pin, OUTPUT);
    pinMode(m_but_pin, OUTPUT);
    digitalWrite(m_but_pin, 0);
    
    m_butt.setDebounce(50);
    m_butt.setClickTimeout(80);
    m_butt.setTickMode(MANUAL);
    m_butt.setTimeout(1000);
    pinMode(18, OUTPUT);
    analogWrite(18, 0);

    
}
void loop_morze(){
    m_pot_pos = map(analogRead(m_pot_pin),0,1023,0,6);
    if (m_old_pos != m_pot_pos){
        m_old_pos = m_pot_pos;
        digitalWrite(m_but_pin, 1);
        m_butt.tick();
        delay(85);
        digitalWrite(m_but_pin, 0);
    }
    m_butt.tick();  
    if (m_butt.isHolded()) {
        if (m_pot_pos == 4) {
            next();
        }
        else{
            error();
        }
             
    }

    if(m_timer_f.isReady()){
        digitalWrite(m_led_pin, (m_morze >> m_pos) & 1);
        m_pos--;
        if (m_pos <= 0) {
            m_pos = 54;
        }
    }
}

//модуль 9 пароль
GButton p_b1(2);
GButton p_b2(3);
GButton p_b3(4);
GButton p_b4(5);
GButton p_b5(6);
#define p_let_max 4
char letters[4][5] = {
                    {'P', '4', '8', '3', '6'},
                    {'2', 'I', '8', '3', '6'},
                    {'2', '4', 'D', '3', '6'},
                    {'2', '4', '8', 'R', '6'}
                    };
int letter[4] = {0,1,2,3};

void setup_password(){
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println("SSD1306 allocation failed");
    }

    delay(2000);
    display.setFont(&FreeSerif9pt7b);
    display.setTextSize(2);             
    display.setTextColor(SSD1306_INVERSE);  
    display.clearDisplay();

    for (byte i = 0; i < 4; i++)
    {
        display.drawRect(i*32, 0, 32, display.height(), SSD1306_WHITE);
        display.setCursor(i*32+3,26);             
        display.println(letters[i][letter[i]]);
    }
    display.display(); 

    
}
void loop_password(){
    p_b1.tick();
    if (p_b1.isSingle()){
        letter[0]++;
        if (letter[0]>p_let_max)
        {
            letter[0] = 0;
        }
        p_refill(0);
    }
    if (p_b1.isDouble()){
        letter[0]--;
        if (letter[0]<0)
        {
            letter[0] = p_let_max;
        }
        p_refill(0);
    }

    p_b2.tick();
    if (p_b2.isSingle()){
        letter[1]++;
        if (letter[1]>p_let_max)
        {
            letter[1] = 0;
        }
        p_refill(1);
    }
    if (p_b2.isDouble()){
        letter[1]--;
        if (letter[1]<0)
        {
            letter[1] = p_let_max;
        }
        p_refill(1);
    }

    p_b3.tick();
    if (p_b3.isSingle()){
        letter[2]++;
        if (letter[2]>p_let_max)
        {
            letter[2] = 0;
        }
        p_refill(2);
    }
    if (p_b3.isDouble()){
        letter[2]--;
        if (letter[2]<0)
        {
            letter[2] = p_let_max;
        }
        p_refill(2);
    }

    p_b4.tick();
    if (p_b4.isSingle()){
        letter[3]++;
        if (letter[3]>p_let_max)
        {
            letter[3] = 0;
        }
        p_refill(3);
    }
    if (p_b4.isDouble()){
        letter[3]--;
        if (letter[3]<0)
        {
            letter[3] = p_let_max;
        }
        p_refill(3);
    }

    p_b5.tick();
    if (p_b5.isClick()){
        String tmp = "";
        for (byte i = 0; i < 5; i++)
        {
            tmp += letters[i][letter[i]];
        }
        if (tmp == "PIDR"){
            next();
        }else
        {
            error();
        }
        
    }
}
void p_refill(byte i){
    display.fillRect(i*32+1, 1, 30, display.height()-2, SSD1306_BLACK);
    display.setCursor(i*32+3,26);             
    display.println(letters[i][letter[i]]);

    display.display(); 
}




//методы для упрощения работы
void setup_module(){
    switch (curent_module)
    {
    case 1:
        setup_wires();
        break;
    case 2:
        setup_button();
        break;
    case 4:
        setup_morze();
        break;
    case 9:
        setup_password();
        break;
    default:
        break;
    }

    new_module = false;
}
void next(){
    Serial.println("Module done!");
    //curent_module++;
    //new_module = true;
}
void error(){
    errors++;
    Serial.write("Error! shots of vodka: ");
    Serial.println(errors);

    /* закоментированно для дебага. Подсвечивает крассным ошибку
    for (byte i = 0; i < 8; i++)
    {
        if (i+1 <curent_module){
            cm_leds[i] = CHSV(96,255,50);
        }
        if (i+1 == curent_module){
            cm_leds[i] = CHSV(0,255,150);
        }
        if (i+1 > curent_module){
            cm_leds[i] = CHSV(64,255,50);
        }
    }

    FastLED.show();
    delay(1000);
    */
}

GTimer t_timer(MS, 1000);

void timer(){
    if (t_timer.isReady()){
        t_time++;
        
        Serial.print(t_time/60);
        Serial.print(":");
        Serial.println(t_time - (int)(t_time/60)*60);
        
    }
}

//индикация модулей
int cm_brightness = 0;
bool cm_dir = true;
GTimer cm_timer(MS, 15);
void show_curent_module(){
    if (cm_timer.isReady()){
         if (cm_dir){
            cm_brightness += 5;
        }else
        {
            cm_brightness -= 5;
        }
        if (cm_brightness <= 50)
        {
            cm_dir = true;
        }
        if (cm_brightness >= 150)
        {
            cm_dir = false;
        }

        if (curent_module = 8){
            for (byte i = 0; i < 8; i++)
            {
                cm_leds[i] = CHSV(96,255,cm_brightness);
            }
        }else
        {
            for (byte i = 0; i < 8; i++)
            {
                if (i+1 <curent_module){
                    cm_leds[i] = CHSV(96,255,50);
                }
                if (i+1 == curent_module){
                    cm_leds[i] = CHSV(64,255,cm_brightness);
                }
                if (i+1 > curent_module){
                    cm_leds[i] = CHSV(64,255,50);
                }
            }
        }
        FastLED.show();
    }
}