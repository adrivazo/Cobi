#include <CapacitiveSensor.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

////////////////////////// CAPACITIVE TOUCH  //////////////////////////
CapacitiveSensor   touchSensor54 = CapacitiveSensor(5,4);// resistor between p5 and p3       

////////////////////////// MOTION SENSOR //////////////////////////
#define publish_delay 30000 // public delay for motion
#define publish_delay_button 2000
unsigned int lastPublish = 0;
unsigned int lastPublishButton = 0;
const int trigPin = 2;//output
const int echoPin = 3;//input
const int CLOSE_ENOUGH_CM = 10;

//////////////////////////  SPEAKER //////////////////////////
int speakerPin = 9;

int length = 15; // the number of notes
//char notes[] = "ccggaagffeeddc "; // a space represents a rest
//char notes2[] = "acbc ga bcbc abc cb ag e ";
//int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
//int beats2[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2, 2 };

char notes2[] = "acbc";
int beats2[] = { 1, 1, 1, 2 };
int tempo = 300;

//////////////////////////  NEOPIXELS //////////////////////////


#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 12
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

uint32_t green = strip.Color(3, 255, 94);
uint32_t yellow = strip.Color(255, 247, 9);
uint32_t blue = strip.Color(76, 196, 255);
uint32_t pink = strip.Color(232, 52, 226);

uint32_t my_color = yellow;
uint32_t other_color = green;
uint32_t another_color = blue;

const int up_to_0[2] = {0,1};
const int up_to_1[6] = {0,1,2,10,11,12}; 
const int up_to_2[9] = {0,1,2,3,9,10,11,12,13}; 
const int up_to_3[13] = {0,1,2,3,4,8,9,10,11,12,13,14,15}; 
const int all[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

#define NONE -1
#define SPIRAL 1//around in a circle
#define GLOW 2 //glow and fade 
#define SECTIONS 3 // by sections
#define CHASE 4 // thather chase

void setup() {  
  touchSensor54.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  pinMode(speakerPin, OUTPUT);
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

// the loop function runs over and over again forever
void loop() {

  boolean cobiStroked = false;
  
  cobiStroked = checkStroke();
    if(cobiStroked){
      cobiReactToStroke();
      }

  boolean someoneWaved = false; 
  someoneWaved = checkWave();   
  if (someoneWaved){
      cobiReactToWave();
    }

  delay(10);
}



boolean checkWave(){
  boolean sawSomething = false;
  unsigned long now = millis();
  
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, inches, cm;

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  Serial.println(cm);
  if (cm < CLOSE_ENOUGH_CM ){//if close enough and more than the desired delay has occured
    //check if it's new motion!
    if( (now - lastPublish) > publish_delay){
      sawSomething = true;
    lastPublish = now;
    }
    
  }

 else{
  sawSomething = false;
  }
  
 return sawSomething;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

////////////////////////// CAPACITIVE TOUCH  //////////////////////////

boolean checkStroke(){
  long total1 =  touchSensor54.capacitiveSensor(50);
  boolean longEnough =false;
  if(total1>40){
    longEnough = true;
    }

   else{
    longEnough = false;
    }
  
  Serial.print("\t");                    // tab character for debug window spacing
  Serial.print(total1);                  // print sensor output 1
  Serial.print("\t");

  return longEnough;
    
  }

////////////////////////// CAPACITIVE TOUCH END //////////////////////////

//////////////////////////  SPEAKER //////////////////////////

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void playMusic(){
    for (int i = 0; i < length; i++) {
    if (notes2[i] == ' ') {
      delay(beats2[i] * tempo); // rest
    } else {
      playNote(notes2[i], beats2[i] * tempo);
      Serial.println(notes2[i]);
    }

    // pause between notes
    delay(tempo / 2); 
  }
  }

//////////////////////////  SPEAKER END //////////////////////////


//////////////////////////  NEOPIXELS  //////////////////////////
  


void startShow(int i, uint32_t c, uint8_t wait) {
   Serial.println("Got single color message");
   // reset the brightness
   
  switch (i) {
    //-1
    case NONE: 
          colorWipe(strip.Color(0, 0, 0), 10);    // Black/off
         strip.setBrightness(255);
      break;
    case SPIRAL: spiralInAndOut(c, wait); //rainbow(20);
      break;
    case GLOW: colorGlow(c, wait);//theaterChase(strip.Color(  0,   0, 127), 10);//rainbowCycle(20);
      break;
    case SECTIONS: 
          lightSection(c, wait, up_to_0,  sizeof(up_to_0)/sizeof(up_to_0[0]));
          lightSection(c, wait, up_to_1, sizeof(up_to_1)/sizeof(up_to_1[0]));
          lightSection(c, wait, up_to_2,sizeof(up_to_2)/sizeof(up_to_2[0]));
          lightSection(c, wait, up_to_3, sizeof(up_to_3)/sizeof(up_to_3[0]));
          lightSection(c, wait, all,  sizeof(all)/sizeof(all[0]));          
      break;
    case CHASE: theaterChase(strip.Color(  0,   0, 127), 10);//colorWipe(strip.Color(0, 0, 0), 10);    // Black/off
      break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Fill the dots one after the other with a color
void spiralInAndOut(uint32_t c, uint8_t wait) {
// turn them on one by one, starting with center pixel

Serial.print("Spiral in and out with color ");
 Serial.println(c);
 
  for (int i = strip.numPixels()-1; i>-1 ; i--) {
    strip.setPixelColor(i, c);
    Serial.println(i);
    strip.show();
    delay(wait*5);
  }
  delay(wait * 20);
  
// turn them off one by one, starting with outer pixel
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0,0,0)); 
        Serial.println(i);
    strip.show();
    delay(wait*4);
  }
}



//lights up all pixes in a fade in, fade out way
void colorGlow(uint32_t c, uint8_t wait){
  for(int j = 0; j < 256; j++) {
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.setBrightness(j);
      strip.show();
    }
  }
  delay(wait*4);
  for(int j = 255; j > 0; j--) {
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.setBrightness(j);
      strip.show();
    }
  }
}

// lights up the pixels in the pixels array
void lightSection(uint32_t c, uint8_t wait, const int pixels[], int numberOfPixels){
  Serial.print("Lighting up pixels ");
  Serial.println(numberOfPixels);
  for (uint16_t i =0; i<numberOfPixels; i++){
      strip.setPixelColor(pixels[i], c);
  }
  strip.show();
  delay(wait*30);
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//////////////////////////  NEOPIXELS END //////////////////////////


void cobiReactToStroke(){
  //turn on neo pixels
  //make a noise
  playMusic();

  //IDEALLY THIS WOULD BE ON A THREAD :(
  startShow(SPIRAL, other_color, 10);
  delay(100);
  }

 void cobiReactToWave(){
  //turn on neo pixels

  startShow(SPIRAL, my_color, 10);
  delay(100);
  }
