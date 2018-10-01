#include "FastLED.h"

#define DATA_PIN        6
#define LETTER_LEDS     26
#define NUM_LEDS        50
#define LED_TYPE        WS2811
#define DEFAULT_BRIGHTNESS  70

#define TIME_LETTERON   600     // ms to leave lights on for each letter blink
#define TIME_LETTEROFF  300     // ms to leave blank between letters
#define TIME_BOARDOFF   60000   // ms to leave board blank between messages
#define NUM_MSG_FLASHES 5       // number of times to flash the message before resetting

int pulse_amount = 20;          // amount to fade (5, 10, 15, 20, 25 etc up to 255) 
int pulse_delay = 9;            // Sets the speed of the fade (5-75+)
int pulse_brightness = 0;

CRGB leds[NUM_LEDS];

#define blank_leds fill_solid(leds, NUM_LEDS, CRGB::Black)
#define arraysize(x) sizeof(x)/sizeof(x[0])

/*
 *  The messages to cycle through
 */
String message[] = { "HELP", "ITSHERE", "RUN", "GETOUT" };

/*
 *  Maps each letter to an led position
 *    start at index 12
 *    1 row, left-to-right, 8 lights (A-H); skip 2 light
 *    1 row, right-to-left, 9 lights (I-Q, reversed); skip 2 light
 *    1 row, left-to-right, 9 lights (R-Z)
 */
uint8_t lightmap[LETTER_LEDS] = {
  11, //a
  12, //b
  13, //c
  14, //d
  15, //e
  16, //f
  17, //g
  18, //h
  29, //i
  28, //j
  27, //k
  26, //l
  25, //m
  24, //n
  23, //o
  22, //p
  21, //q
  32, //r
  33, //s
  34, //t
  35, //u
  36, //v
  37, //w
  38, //x
  39, //y
  40, //z
};

/*
 *  Sets the "Stranger Things" inspired LED color map
 *  see https://i.imgur.com/VJapZLS.jpg
 */
const CRGB stranger_map[NUM_LEDS] = {
  CRGB::White,        //non-letter 
  CRGB::Blue,         //non-letter
  CRGB::Magenta,      //non-letter
  CRGB::Green,        //non-letter
  CRGB::Blue,         //non-letter
  CRGB::Yellow,       //non-letter
  CRGB::Red,          //non-letter
  CRGB::Blue,         //non-letter
  CRGB::Magenta,      //non-letter
  CRGB::Green,        //non-letter
  CRGB::White,        //A 
  CRGB::Blue,         //B
  CRGB::Magenta,      //C
  CRGB::Green,        //D
  CRGB::Blue,         //E
  CRGB::Yellow,       //F
  CRGB::Red,          //G
  CRGB::Blue,         //H
  CRGB::Green,        //non-letter
  CRGB::White,        //non-letter
  CRGB::Blue,         //I
  CRGB::Magenta,      //J
  CRGB::Blue,         //K
  CRGB::White,        //L
  CRGB::Yellow,       //M
  CRGB::Red,          //N
  CRGB::Magenta,      //O
  CRGB::Green,        //P
  CRGB::Magenta,      //Q
  CRGB::Yellow,       //non-letter
  CRGB::Red,          //non-letter
  CRGB::Green,        //R
  CRGB::White,        //S
  CRGB::Yellow,       //T
  CRGB::Blue,         //U
  CRGB::Magenta,      //V
  CRGB::Blue,         //W
  CRGB::Yellow,       //X
  CRGB::Red,          //Y
  CRGB::Magenta,      //Z
  CRGB::White,        //non-letter 
  CRGB::Blue,         //non-letter
  CRGB::Magenta,      //non-letter
  CRGB::Green,        //non-letter
  CRGB::Blue,         //non-letter
  CRGB::Yellow,       //non-letter
  CRGB::Red,          //non-letter
  CRGB::Blue,         //non-letter
  CRGB::Magenta,      //non-letter
  CRGB::Green         //non-letter
};

/** get_color(light) - gets the color for a particular light number from stranger_map
 *  return: CRGB struct for the color to 
 */
CRGB get_color(uint8_t light) {
  return stranger_map[light];
}

/** show_letter(letter) - causes a letter to light up 
 */
void show_letter(char letter) {
  uint8_t cval = letter;
  uint8_t light = cval-65;  // change A-Z to 0-25
  
  //turn on the right light
  leds[lightmap[light]] = get_color(light);  
}

/** hide_letter(letter) - causes a letter to turn off
 * 
 */
void hide_letter(char letter) {
  uint8_t cval = letter;
  uint8_t light = cval-65;  
  
  //turn on the right light
  leds[lightmap[light]] = CRGB::Black;
}

/** blink_letter(letter) - causes a letter to blink
 */
void blink_letter(char letter) {
  show_letter(letter);
  FastLED.show();
  FastLED.delay(TIME_LETTERON);
  hide_letter(letter);
  FastLED.show();
  FastLED.delay(TIME_LETTEROFF);
}

/** display_message(msg) - blinks a messsage one char at a time
 * 
 *  Calls blink_letter for each char
 *  Then lights them all up at once
 * 
 *  return false if the message fails (e.g. too short), true otherwise
 */
bool display_message(String msg) {
  glow_all();
  pulse_all(500);
  blank_leds;
  FastLED.delay(1200); 
  msg.trim();
  if (msg.length() > 1) {   
    //reset the lights
    FastLED.delay(TIME_LETTEROFF); 
    //blink the letters
    for (uint8_t i = 0; i < msg.length(); i++) {
      blink_letter(msg.charAt(i));
    }
    //flash all the letters at once
    FastLED.delay(TIME_LETTEROFF); 
    flash_message(msg);
    
    return true;
  }

  return false;
}

/** flash_message(msg) - flashes all the message characters at once
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

/** buffer_message() - buffers message from serial input
 *  
 *  Reads a string terminated by \n, upper-cases it, and pushes it to the end
 *  of the global message[] array, shifting other messages up (FIFO).
 *  
 *  return 0 if success, < 0 for errors
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

/** flash_error(times, gap, color) - flash the whole string to indicate a status
 *  
 *  this should probably be "statusflash", but eh.
 *  
 *  Blinks the string 'color', 'times' times, with each on/off cycle lasting 'gap' ms.
 *  
 *  So flash_error(5, 100, CRGB::Blue) would flash the string blue 5 times; each flash would be
 *  50ms on and 50ms off.
 */
void flash_error(uint8_t times, unsigned int gap, CRGB color) {
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

/** lightup_all() - turns all the leds to their default color
 */
void lightup_all() {
  for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = get_color(i);  
  }
  FastLED.show();
}

void pulse_all(int duration) {
  unsigned long secs = millis();
  while (millis() - secs <= duration) {
    for(int i = 0; i < NUM_LEDS; i++ ) {
      leds[i] = get_color(i); //]
      leds[i].fadeLightBy(pulse_brightness);
    }
    FastLED.show();
    pulse_brightness = pulse_brightness + pulse_amount;
    // reverse the direction of the fading at the ends of the fade: 
    if (pulse_brightness == 0 || pulse_brightness == 255) {
      pulse_brightness = -pulse_brightness ; 
    }    
    delay(pulse_delay);
  }
}

void glow_all() {
  unsigned long secs = millis();
  uint8_t minBright = 40;
  uint8_t maxBright = 125;
  uint8_t fadeAmount = 5;
  while (millis() - secs <= 3000) {
    for(int i = 0; i < NUM_LEDS; i++ ) {
      leds[i] = get_color(i); // Set Color HERE!!!
      leds[i].fadeLightBy(minBright);
    }
    FastLED.show();
    minBright = minBright + fadeAmount;
    // reverse the direction of the fading at the ends of the fade: 
    if(minBright == 0 || minBright == maxBright) {
      fadeAmount = -fadeAmount ; 
    }    
    delay(9);
  }
}

/** setup() - run once at Arduino init, sets up lightmap and FastLED
 *  
 *  Opens Serial port at 9600, 8-N-1
 */
void setup() {
  delay(1500);  // for interrupting during startup
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, DATA_PIN, RGB>(leds, NUM_LEDS);
  //turn the lights on by default
  lightup_all();
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);
  FastLED.delay(TIME_BOARDOFF);
}

/** loop() - main Arduino event loop, called repeatedly
 *  
 *  Cycles through each item in message[] and displays it using 'display_message'
 *  
 *  If there is serial data available between messages, adds it to the queue;
 *  blinks green twice when new message is inserted, red 5 times if there's an
 *  error inserting it.
 *  
 *  Because the serial interrupt is processed between messages, only the last message
 *  sent during display will be queued. Clients have to wait for a queue confirmation
 *  before sending the next message.
 */
void loop() {
  static uint8_t m_idx = 0;

  if (Serial.available()) {
    // add the message to the end of the message buffer
    if (buffer_message() < 0) {  
      // error state: string too short or too long
      flash_error(5, 100, CRGB::Red);
      Serial.println("Message size error");
    } else {
      m_idx = arraysize(message)-1;  // tee it up to be displayed next
      Serial.print("Queued message '"); 
      Serial.print(message[m_idx]); 
      Serial.print("'");
      Serial.println("");
      flash_error(2, 50, CRGB::Green);
    }
  }

  // delay between messages, only if message was displayed
  if (display_message(message[m_idx])) { 
    //do a quick pulse
    pulse_all(1000);
    //then go back to default
    FastLED.setBrightness(DEFAULT_BRIGHTNESS);
    lightup_all();
    FastLED.delay(TIME_BOARDOFF); 
  }

  m_idx++;
  if (m_idx >= arraysize(message)) { 
    m_idx = 0; 
  }
}
