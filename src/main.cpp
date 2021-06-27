#include <Arduino.h>
#include <FastLED.h>
#include <DFRobotDFPlayerMini.h>

#define XIAO_BOARD

#ifdef XIAO_BOARD
#define LED_PIN_1 0
#define LED_PIN_2 1
#define LED_PIN_3 2
#define LED_PIN_4 3
#define TOGGLE_PIN 8
#define LEFT_PIN 10
#define RIGHT_PIN 9

#else

#define LED_PIN_1 13
#define LED_PIN_2 14
#define BUTTON_PIN 21
#endif

enum BUTTON_RESULT
{
  LEFT_TOGGLE_ON,
  LEFT_TOGGLE_OFF,
  RIGHT_TOGGLE_ON,
  RIGHT_TOGGLE_OFF
};

#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define NUM_LEDS 40

#define BRIGHTNESS 255

DFRobotDFPlayerMini player;
bool gReverseDirection = false;

CRGB leds[NUM_LEDS];

void setup()
{
  delay(3000); // sanity delay
  Serial.begin(9600);
  Serial.println("Welcome");
  FastLED.addLeds<CHIPSET, LED_PIN_1, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, LED_PIN_2, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, LED_PIN_3, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, LED_PIN_4, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(TOGGLE_PIN, INPUT_PULLUP);
  Serial1.begin(9600);
  while (true)
  {
    if (!player.begin(Serial1))
    {
      Serial.println(F("Unable to begin"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
      delay(3000);
    }
    else
    {
      break;
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  player.volume(30);
}

void setColour(CRGB colour)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = colour;
  }
  FastLED.show();
}

#define PIXELS_LIT 20
#define PIXELS_INC 5
#define SHOT_DELAY 1000

void showPulse(int startAt, CRGB colour)
{
  for (int p = 0; p < PIXELS_LIT; p++)
  {
    int index = startAt + p;
    if ((index >= 0) && (index < NUM_LEDS))
    {
      leds[index] = colour;
    }
  }
  FastLED.show();
  for (int p = 0; p < PIXELS_LIT; p++)
  {
    int index = startAt + p;
    if ((index >= 0) && (index < NUM_LEDS))
    {
      leds[index] = CRGB::Black;
    }
  }
}

void fire(BUTTON_RESULT buttonResult)
{
  int track;
  CRGB colour;
  switch (buttonResult)
  {
  case LEFT_TOGGLE_OFF:
    colour = CRGB::Red;
    track = 1;
    break;

  case RIGHT_TOGGLE_OFF:
    colour = CRGB::Red;
    track = 1;
    break;

  case LEFT_TOGGLE_ON:
    colour = CRGB::Blue;
    track = 1;
    break;

  default:
    colour = CRGB::Blue;
    track = 1;
    break;
  }

  player.play(track);
  setColour(CRGB::Black);
  FastLED.show();

  for (int i = -PIXELS_LIT; i < NUM_LEDS; i += PIXELS_INC)
  {
    showPulse(i, colour);
    delay(1);
  }
  FastLED.show();
}

BUTTON_RESULT waitForTrigger()
{
  bool first = true;
  while (true)
  {
    int leftButtonState = digitalRead(LEFT_PIN);
    int rightButtonState = digitalRead(RIGHT_PIN);
    int toggleState = digitalRead(TOGGLE_PIN);
    if (first)
    {
      Serial.printf("Button state = %d,%d,%d, waiting for %d\n", leftButtonState, rightButtonState, toggleState, LOW);
      first = false;
    }
    if (leftButtonState == LOW)
    {
      if (toggleState == LOW)
      {
        return LEFT_TOGGLE_ON;
      }
      return LEFT_TOGGLE_OFF;
    }
    if (rightButtonState == LOW)
    {
      if (toggleState == LOW)
      {
        return RIGHT_TOGGLE_ON;
      }
      return RIGHT_TOGGLE_OFF;
    }
    delay(1);
  }
}

void waitForRelease()
{
  while (true)
  {
    int leftButtonState = digitalRead(LEFT_PIN);
    int rightButtonState = digitalRead(RIGHT_PIN);
    if ((leftButtonState == HIGH) && (rightButtonState == HIGH))
    {
      return;
    }
    delay(1);
  }
}

void loop()
{
  BUTTON_RESULT result = waitForTrigger();
  fire(result);
  waitForRelease();
}