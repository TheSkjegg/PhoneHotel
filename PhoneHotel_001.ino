// Description: 
//    Phone Hotel
//    Outputs: 7segment LED display
//    Inputs: Select switch + slot switches
//
// Usage:
//    Developed on Arduino Nano Every, but should be compatible with all "Arduino UNO" boards.
//    Supports up to 6 phone slots. HOTEL sign and upper window LED are optional.
//
// Author: 
//    The Skjegg 28/10/2026
//
// Versions:
//    v001: First version

#include <LedControl.h>
#include <FastLED.h>

#define NUM_LEDS 11
CRGB leds[NUM_LEDS];
uint8_t brightness = 50;

// IO def
#define LED_PIN 9
#define sw_reset 8
#define Buzz A0

// Slot IO def
const uint8_t switch_IO[] = {2,3,4,5,6,7};


LedControl lc = LedControl(12, 10, 11, 1); // CLK=10, CS=11 and DIN=12

unsigned long lastMillis, currentMillis, sneakMillis;
unsigned long slotTime[] = {0,0,0,0,0,0};
int slotEmpty[] = {3,3,3,3,3,3};
volatile bool resetPressed = false;
bool sneakPeek = false; 
int slotToDisplay = 0;
int sneakPeekSlot = 0;
int colorNo = 0;

CRGB color[] = {CRGB::White, CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::DodgerBlue, CRGB::Yellow, CRGB::Plum, CRGB::LightSalmon, CRGB::Fuchsia, CRGB::Gold, CRGB::Magenta, CRGB::Brown, CRGB::OrangeRed};

void setup() {

  Serial.begin(9600);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);

    FastLED.clear(true);
        for (int i = 0; i < NUM_LEDS; i++)
        leds[i] = CRGB::OrangeRed;
      FastLED.show();
  FastLED.show();

  //IO
  for (int i=0; i<6; i++)
    pinMode(switch_IO[i], INPUT_PULLUP);

  pinMode(sw_reset, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sw_reset), handleResetInterrupt, FALLING);

  // 7segment
  lc.shutdown(0,false);
  lc.setIntensity(0,3);
  dispHello();
  lc.clearDisplay(0);
  delay(500);

} // Setup exit

void handleResetInterrupt(){
  resetPressed = true;
}


void dispHello(){
    for(signed int i=8;i>=-5;i--){ // Hello start message
    lc.clearDisplay(0);
    lc.setChar(0,i+4,'H',false);
    lc.setChar(0,i+3,'E',false);
    lc.setChar(0,i+2,'L',false);
    lc.setChar(0,i+1,'L',false);
    lc.setChar(0,i,'0',false);
    if(i==1){
      delay(1500);
    }
    else
      delay(50);
  }
  //successSound();
}

void dispHello2(){
    lc.clearDisplay(0);
    lc.setChar(0,5,'H',false);
    lc.setChar(0,4,'E',false);
    lc.setChar(0,3,'L',false);
    lc.setChar(0,2,'L',false);
    lc.setChar(0,1,'0',false);
    successSound();
    lc.clearDisplay(0);
}

void dispLoser(){
  lc.clearDisplay(0);
  lc.setChar(0,5,'L',false);
  lc.setChar(0,4,'0',false);
  lc.setChar(0,3,'5',false);
  lc.setChar(0,2,'E',false);
  lc.setChar(0,1,'A',false);
  failedSound();
  lc.clearDisplay(0);
  }

void successSound(){
    pinMode(Buzz, OUTPUT);
    tone(Buzz, 523);
    delay(100);
    noTone(Buzz);
    delay(50);
    tone(Buzz, 1047);
    delay(500);
    noTone(Buzz);
    pinMode(Buzz, INPUT);
}

void failedSound(){
    pinMode(Buzz, OUTPUT);
    tone(Buzz, 98); // Note G2
    delay(500);
    noTone(Buzz);
    pinMode(Buzz, INPUT);
}

void displayTime(unsigned long millisek){
  unsigned long over;
  int num;
  int numOfDigits = 0;

  if(millisek > 360000000)
    numOfDigits = 8;
  else if (millisek > 36000000)
    numOfDigits = 7;
  else if (millisek > 3600000)
    numOfDigits = 6;
  else if (millisek > 600000)
    numOfDigits = 5;
  else if (millisek > 60000)
    numOfDigits = 4;
  else if (millisek > 10000)
    numOfDigits = 3;
  else if (millisek > 1000)
    numOfDigits = 2;
  else if (millisek > 100)
    numOfDigits = 1;

  num=int(millisek/360000000);  // h_hundreds
  if(numOfDigits > 7)
    lc.setDigit(0,7,num,false);
  over=millisek%360000000;
  num=int(over/36000000);  // h_tens
  if(numOfDigits > 6)
    lc.setDigit(0,6,num,false);
  over=over%36000000;
  num=int(over/3600000);  // h_ones
    if(numOfDigits > 5)
  lc.setDigit(0,5,num,true);
  over=over%3600000;  
  num=int(over/600000);  // m_tens
  if(numOfDigits > 4)
    lc.setDigit(0,4,num,false);
  over=over%600000;
  num=int(over/60000);   // m_ones
  if(numOfDigits > 3)
    lc.setDigit(0,3,num,true);
  over=over%60000;
  num=int(over/10000);   // s_tens
    if(numOfDigits > 2)
  lc.setDigit(0,2,num,false);
  over=over%10000;
  num=int(over/1000);    // s_ones
    if(numOfDigits > 1)
  lc.setDigit(0,1,num,true);
  over=over%1000;
  num=int(over/100);  // ms_hundreds
    if(numOfDigits > 0)
  lc.setDigit(0,0,num,false);
} // displayTime

void resetAll(){
  slotTime[0] = 0;
  slotTime[1] = 0;
  slotTime[2] = 0;
  slotTime[3] = 0;
  slotTime[4] = 0;
  slotTime[5] = 0;

  FastLED.clear(true);
  FastLED.show();

  dispHello();

  lastMillis = currentMillis = millis();

}

void loop() {

  currentMillis = millis();

  for(int i=0;i<6;i++){
    if(!digitalRead(switch_IO[i])){
      slotTime[i] = slotTime[i] + (currentMillis - lastMillis);
      if(slotEmpty[i] == 1){ // Slot empty on last read, say hello and display time
        dispHello2();
        slotEmpty[i] = 0;
        sneakPeek = true;
        sneakPeekSlot = i;
        sneakMillis = millis();
      }
      else
        slotEmpty[i] = 0; // Phone in slot
    }
    else{ // No phone in slot
      if(slotEmpty[i] == 0){ // Phone was removed since last read
        dispLoser();
      }
      slotEmpty[i] = 1; 
    }
  }

  lastMillis = currentMillis;

  if(resetPressed){
    delay(200);

    if(!sneakPeek){
        sneakPeekSlot = slotToDisplay;
        sneakPeek = true;
      } // already pressed, show next
      
      sneakPeekSlot++;
      int k = 0;
      while(slotTime[sneakPeekSlot] < 99){ // Skip unused slots
        sneakPeekSlot++;
        if(sneakPeekSlot > 4)
          sneakPeekSlot = 0;
        k++;
        Serial.print("k: ");
        Serial.println(k);
        if(k > 4)
          break;
      }

    if(sneakPeekSlot > 4)
      sneakPeekSlot = 0;
    sneakMillis = millis();

    Serial.println(sneakPeekSlot);

    // Detect reset hold
    while(!digitalRead(sw_reset)){
      delay(200);
      if(millis() > (sneakMillis + 5000)){
        resetAll();
        sneakPeek = false;
        break;
      }
    }

    resetPressed = false;
    lc.clearDisplay(0);

    colorNo++;
    if(colorNo > 12)
      colorNo = 0;

    for (int i = 5; i < NUM_LEDS; i++)
      leds[i] = color[colorNo];

  }


  slotToDisplay = 0; // Display the greatest time

  for(int i=0;i<5;i++){
    if(slotTime[i] > slotTime[slotToDisplay])
      slotToDisplay = i;
  }

  for(int i=0;i<5;i++){ // Check for multiple with same time (reset with multiple phones inserted, make all green)
    if(slotTime[i] == slotTime[slotToDisplay])
      leds[i] = CRGB::Green;
  }

  for(int i=0;i<5;i++){
    if(slotTime[i] < slotTime[slotToDisplay])
      leds[i] = CRGB::White;
  }

  int looserSlot = slotToDisplay; // Start at greatest


  for(int i=0;i<5;i++){
    if(slotTime[i] < 99){
      leds[i] = CRGB::Black; // Turn off unused slots
    }
    else if(slotTime[i] < slotTime[looserSlot])
      looserSlot = i;
  }

  if(looserSlot != slotToDisplay) // more than one slot in use
    leds[looserSlot] = CRGB::Red;

  if(sneakPeek){ // SneakPeek activated, show only selected slot

    for(int i=0;i<5;i++){ // Turn off all other slots than the sneakPeek one
      if(i != sneakPeekSlot)
        leds[i] = CRGB::Black;
    }

    displayTime(slotTime[sneakPeekSlot]); // Display the sneakPeek slot time
    
    if(millis() > (sneakMillis + 3000)) // Turn off sneakPeek after 5sec
      sneakPeek = false;
  }
  else
    displayTime(slotTime[slotToDisplay]); // Display the best time

    FastLED.show(); // Update leds
}

