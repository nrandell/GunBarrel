#include <FastLED.h>
#include <DFRobotDFPlayerMini.h>


#define LED_PIN_1     13
#define LED_PIN_2     14
#define BUTTON_PIN 21
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS   40 // 100

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 60

DFRobotDFPlayerMini player;
bool gReverseDirection = false;

CRGB leds[NUM_LEDS];


void setup() {
  delay(3000); // sanity delay
  Serial.begin(9600);
  Serial.println("Welcome");
  FastLED.addLeds<CHIPSET, LED_PIN_1, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, LED_PIN_2, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  pinMode(BUTTON_PIN, INPUT);
  Serial1.begin(9600);
  if (!player.begin(Serial1)) {
    Serial.println(F("Unable to begin"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));  
  player.volume(30);
}

void setColour(CRGB colour) {
  for (int i = 0;  i < NUM_LEDS; i++) {
    leds[i] = colour;
  }
  FastLED.show();
}

#define PIXELS_LIT 20
#define PIXELS_INC 5
#define SHOT_DELAY 1000

void showPulse(int startAt, CRGB colour) {
  for (int p = 0; p < PIXELS_LIT; p++) {
    int index = startAt + p;
    if ((index >= 0) && (index < NUM_LEDS)) {
      leds[index] = colour;
    }
  }
  FastLED.show();
  for (int p = 0; p < PIXELS_LIT; p++) {
    int index = startAt + p;
    if ((index >= 0) && (index < NUM_LEDS)) {
      leds[index] = CRGB::Black;
    }
  }
}

void fire() {
  player.play(1);
  setColour(CRGB::Black);
  FastLED.show();

  for (int i = -PIXELS_LIT; i < NUM_LEDS; i += PIXELS_INC) {
    showPulse(i, CRGB::Red);
    delay(1);
  }
  FastLED.show();
}

void waitForTrigger(int state) {
  while (true) {
    int buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == state) {
      return;
    }
    delay(1);
  }

}

void loop() {
  waitForTrigger(HIGH);
  fire();
  waitForTrigger(LOW);
  // delay(SHOT_DELAY);
}