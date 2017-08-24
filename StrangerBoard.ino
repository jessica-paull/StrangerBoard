/*
 *  Forked from https://github.com/darrenpmeyer/StrangerBoard
 */

#include "FastLED.h"

#define DATA_PIN            6
#define NUM_LEDS            50
#define NUM_USED_LEDS       26
#define LED_TYPE            WS2811
#define BRIGHTNESS          200

#define TIME_LETTERON       600     // ms to leave lights on for each letter blink
#define TIME_LETTEROFF      300     // ms to leave blank between letters
#define TIME_BOARDOFF       12000   // ms between messages
#define NUM_MSG_FLASHES     5       // number of times to flash the message before resetting

#define PULSE_TIME          1000    // ms to pulse
#define PULSE_AMT           20      // amount to pulse (5, 10, 15, 20, 25 etc up to 255) 
#define PULSE_DELAY         9       // speed of the pulse (5-75+)
#define PULSE_BRIGHTNESS    0

CRGB leds[NUM_LEDS];

#define blank_leds fill_solid(leds, NUM_LEDS, CRGB::Black)
#define arraysize(x) sizeof(x)/sizeof(x[0])


/*
 *  The messages to cycle through
 */
String message[] = { 
  "HELP",
  "ITSHERE",  
  "RUN" 
};


/*
 *  Sets lightmap for a board that runs as follows:
 *    start at index 12
 *    1 row, left-to-right, 8 lights (A-H); skip 1 light
 *    1 row, right-to-left, 9 lights (I-Q, reversed); skip 1 light
 *    1 row, left-to-right, 9 lights (R-Z)
 */
uint8_t lightmap[NUM_USED_LEDS] = {
  12, //a
  13, //b
  14, //c
  15, //d
  16, //e
  17, //f
  18, //g
  19, //h
  29, //i
  28, //j
  27, //k
  26, //l
  25, //m
  24, //n
  23, //o
  22, //p
  21, //q
  31, //r
  32, //s
  33, //t
  34, //u
  35, //v
  36, //w
  37, //x
  38, //y
  39  //z
};


/*
 *  Sets the "Stranger Things" inspired LED color map
 *  see https://i.imgur.com/VJapZLS.jpg
 */
const CRGB stranger_map[NUM_USED_LEDS] = {
  CRGB::White,        //A 
  CRGB::Blue,         //B
  CRGB::Magenta,      //C
  CRGB::Green,        //D
  CRGB::Blue,         //E
  CRGB::Yellow,       //F
  CRGB::Red,          //G
  CRGB::Blue,         //H
  CRGB::Blue,         //I
  CRGB::Magenta,      //J
  CRGB::Blue,         //K
  CRGB::White,        //L
  CRGB::Yellow,       //M
  CRGB::Red,          //N
  CRGB::Magenta,      //O
  CRGB::Green,        //P
  CRGB::Magenta,      //Q
  CRGB::Green,        //R
  CRGB::White,        //S
  CRGB::Yellow,       //T
  CRGB::Blue,         //U
  CRGB::Magenta,      //V
  CRGB::Blue,         //W
  CRGB::Yellow,       //X
  CRGB::Red,          //Y
  CRGB::Magenta       //Z
};


/*
 *  Original LED color map from github project
 *  see https://i.imgur.com/VJapZLS.jpg
 */
const CRGB stranger_map2[NUM_USED_LEDS] = {
  CRGB::Blue, CRGB::Magenta, CRGB::Red, CRGB::Blue, CRGB::Yellow, CRGB::Magenta, CRGB::Blue,
  CRGB::Blue, CRGB::Magenta, CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Magenta, CRGB::Magenta, CRGB::Blue, CRGB::Magenta,
  CRGB::Green, CRGB::White, CRGB::Yellow, CRGB::Blue, CRGB::Magenta, CRGB::Blue, CRGB::Yellow, CRGB::Magenta, CRGB::Magenta,
  CRGB::Red
};


/*
 *  get_color(light) - gets the color for a particular light number from stranger_map
 *  return: CRGB struct for the color to 
 */
CRGB get_color(uint8_t light) {
  
  return stranger_map[light];
  
}


/*
 *  show_letter(letter) - lights up a specific letter
 */
void show_letter(char letter) {
  
  uint8_t cval = letter;
  uint8_t light = cval-65;  
  
  leds[lightmap[light]] = get_color(light); 
   
}


/*
 *  hide_letter(letter) - turns off a specific letter
 * 
 */
void hide_letter(char letter) {
  
  uint8_t cval = letter;
  uint8_t light = cval-65;  
  
  leds[lightmap[light]] = CRGB::Black;
  
}


/*
 *  blink_letter(letter) - blinks a specific letter
 */
void blink_letter(char letter) {
  
  show_letter(letter);
  FastLED.show();
  FastLED.delay(TIME_LETTERON);
  hide_letter(letter);
  FastLED.show();
  FastLED.delay(TIME_LETTEROFF);
  
}


/*
 *  display_message(msg) - blinks a messsage one char at a time
 * 
 *  Calls blink_letter for each char
 *  Then flash_message to show them all at once
 * 
 *  return false if the message fails (e.g. too short), true otherwise
 */
bool display_message(String msg) {
  
  msg.trim();
  
  if (msg.length() > 1) {   
    
    //reset the lights
    blank_leds;
    FastLED.delay(TIME_LETTEROFF); 
    //blink the letters
    for (uint8_t i = 0; i < msg.length(); i++) {
      blink_letter(msg.charAt(i));
    }
    //flash the letters all at once
    FastLED.delay(TIME_LETTEROFF); 
    flash_message(msg);
    
    return true;
    
  }

  return false;
  
}


/*
 *  flash_message(msg) - flashes all the message chars at once
 * 
 *  Number of times the message flashes is set by NUM_MSG_FLASHES
 */
void flash_message(String msg) {
  
  for (uint8_t i = 0; i < NUM_MSG_FLASHES; i++) {
    
    for (uint8_t n = 0; n < msg.length(); n++) {
      show_letter(msg.charAt(n));
      FastLED.show();
    }
    
    FastLED.delay(TIME_LETTEROFF);
    blank_leds;
    FastLED.delay(TIME_LETTEROFF);
    
  } 
  
  blank_leds;
  
}


/*
 *  buffer_message() - buffers message from serial input
 *  
 *  Reads a string terminated by \n, uppercases it, and pushes it to the end
 *  of the global message[] array, shifting other messages up (FIFO)
 *  
 *  return 0 if successful, < 0 for errors
 *  
 *  errors: 
 *    return -1 if message is too long (>100 chars) or too short (<3 chars)
 */
int buffer_message() {
  
  String new_msg;

  while (Serial.available()) {
    new_msg = Serial.readStringUntil("\n");
    new_msg.toUpperCase();
    new_msg.trim();
  }

  if (new_msg.length() > 100 || new_msg.length() < 3) {
    return -1;  // message too long or short
  }
  
  for (uint8_t i = 1; i < arraysize(message); i++) {
    message[i-1] = message[i];
  }
  message[arraysize(message)-1] = new_msg;

  return 0;
  
}


/*
 *  status_flash(times, gap, color) - flash the whole string to indicate a status
 *  
 *  
 *  Blinks the string 'color', 'times' times, with each on/off cycle lasting 'gap' ms
 *  
 *  So status_flash(5, 100, CRGB::Blue) would flash the string blue 5 times; each flash 
 *  would be 50ms on and 50ms off
 */
void status_flash(uint8_t times, unsigned int gap, CRGB color) {
  
  blank_leds;
  
  for (uint8_t i = 0; i < times; i++) {
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
    FastLED.delay(gap/2);
    blank_leds;
    FastLED.show();
    FastLED.delay(gap/2);
  }
  
}


/*
 *  lightup_all() - turns all the leds to their default color
 *  ignoring lights that arent letters, so reference NUM_USED_LEDS
 */
void lightup_all() {
  
  for (int i = 0; i < NUM_USED_LEDS; i++) {
      leds[lightmap[i]] = get_color(i);  
  }
  
  FastLED.show();
  
}


/*
 *  pulse_all() - pulses all the leds their default color
 *  ignoring lights that arent letters, so reference NUM_USED_LEDS
 */
void pulse_all() {
  
  unsigned long secs = millis();
  
  while (millis() - secs <= PULSE_TIME) { 
    
    for (int i = 0; i < NUM_USED_LEDS; i++ ) {
      leds[lightmap[i]] = get_color(i); 
      leds[lightmap[i]].fadeLightBy(PULSE_BRIGHTNESS);
    }
    FastLED.show();
    PULSE_BRIGHTNESS = PULSE_BRIGHTNESS + PULSE_AMT;
    
    // reverse the direction of the fading at the ends of the fade 
    if (PULSE_BRIGHTNESS == 0 || PULSE_BRIGHTNESS == 255) {
      PULSE_BRIGHTNESS = -PULSE_BRIGHTNESS ; 
    }    
    delay(PULSE_DELAY);
    
  }
  
}


/*
 *  setup() - runs once at Arduino init, sets up lightmap and FastLED
 *  
 *  Opens Serial port at 9600, 8-N-1
 */
void setup() {
  
  delay(1500);  //for interrupting during startup
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, DATA_PIN, RGB>(leds, NUM_LEDS);
  
  //turn the lights on by default
  lightup_all();
  FastLED.delay(TIME_BOARDOFF);
  
}


/*
 *  loop() - main Arduino event loop, called repeatedly
 *  
 *  Cycles through each item in message[] and displays it using 'display_message'
 *  
 *  If there is serial data available between messages, adds it to the queue;
 *  blinks green twice when new message is inserted, red 5 times if there's an
 *  error inserting it
 *  
 *  Because the serial interrupt is processed between messages, only the last message
 *  sent during display will be queued. Clients have to wait for a queue confirmation
 *  before sending the next message
 */
void loop() {
  
  static uint8_t msg_index = 0;

  if (Serial.available()) {
    //add the message to the end of the message buffer
    if (buffer_message() < 0) {  
      //error state: string too short or too long
      Serial.println("Message size error");
      status_flash(5, 100, CRGB::Red);
    } else {
      //tee it up to be displayed next
      msg_index = arraysize(message)-1; 
      Serial.print("Queued message '"); 
      Serial.print(message[msg_index]); 
      Serial.print("'");
      Serial.println("");
      status_flash(2, 50, CRGB::Green);
    }
  }

  //delay between messages, only if message was displayed
  if (display_message(message[msg_index])) { 
    //do a quick pulse
    pulse_all();
    //then go back to default
    FastLED.setBrightness(BRIGHTNESS);    200  lightup_all();
    FastLED.delay(TIME_BOARDOFF); 
  }

  //reset the msg_index once we reach the end
  msg_index++;
  if (msg_index >= arraysize(message)) { 
    msg_index = 0; 
  }
  
}
