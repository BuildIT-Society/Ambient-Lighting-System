#include <QueueArray.h>
#include <LiquidCrystal.h>

#define GREENPIN 11
#define REDPIN 10
#define BLUEPIN 9
#define PUSHBUTTON_PIN 2
#define JOYSTICK_BUTTON_PIN 13
#define PRESS_LENGTH 1

// initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);

int state = 0;
int red = 0;
int green = 0;
int blue = 0;

int queueItems = 50; // Change this to make system less sensitive to music beats
QueueArray <int> queue;
int sum = 0;
int initValues = 10;

typedef struct listNode_ {
  int redVal;
  int greenVal;
  int blueVal;
  struct listNode_* next;
} listNode;

void setup() 
{
  lcd.begin(16, 2);
  pinMode(PUSHBUTTON_PIN, INPUT);
  pinMode(JOYSTICK_BUTTON_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  for(int i = 0; i < queueItems; i++) {
    queue.enqueue(initValues);
    sum = sum + initValues;
  }
}

void loop() 
{
  if(state == 0) {
    bgLighting();
  }
  else if(state == 1) {
    colorSwirl();
  }
  else if(state == 2) {
    joystick();
  }
  else if(state == 3) {
    listNode* newList = createColorList();
    partyMusic(newList);
    destroyList(&newList);
  }
}

listNode* createColorList() {
  int numColors = 8; // Red, orange, yellow, green, light blue, blue, purple, pink
  
  listNode* red = malloc(sizeof(*red));
  listNode* orange = malloc(sizeof(*orange));
  listNode* yellow = malloc(sizeof(*yellow));
  listNode* green = malloc(sizeof(*green));
  listNode* lblue = malloc(sizeof(*lblue));
  listNode* blue = malloc(sizeof(*blue));
  listNode* purple = malloc(sizeof(*purple));
  listNode* pink = malloc(sizeof(*pink));
  
  red->next = orange;
  orange->next=yellow;
  yellow->next = green;
  green->next = lblue;
  lblue->next = blue;
  blue->next = purple;
  purple->next = pink;
  pink->next = red; // Circularly linked
  
  fillNodeColors(red, 255, 0 ,0);
  fillNodeColors(orange, 255, 127 ,0);
  fillNodeColors(yellow, 255, 255 ,0);
  fillNodeColors(green, 0, 255 ,0);
  fillNodeColors(lblue, 0, 110 ,255);
  fillNodeColors(blue, 0, 0 ,255);
  fillNodeColors(purple, 127, 0 ,255);
  fillNodeColors(pink, 255, 0 ,200);

  return red; // Arbitrary since circularly linked
}

void fillNodeColors(listNode* node, int r, int g, int b) {
  node->redVal = r;
  node->greenVal = g;
  node->blueVal = b;
}

void destroyList(listNode** head) {
  listNode* currNode = *head;
  listNode* tmp = currNode;
  
  for(int i = 0; i < 8; i++) {
    tmp = currNode;
    currNode = currNode->next;
    free(tmp);
  }

  *head = NULL;
}

bool buttonIsPressed(int buttonPin) //pressed but not held
{
  int buttonPress = 0;
  while (digitalRead(buttonPin) == HIGH){
   digitalWrite(LED_BUILTIN, HIGH);
   buttonPress++; 
   delay(50);
  }
  digitalWrite(LED_BUILTIN, LOW);
  if(buttonPress > PRESS_LENGTH && buttonPress < PRESS_LENGTH + 5) // 10 = 1 second
  {
    lcd.clear();
    return true;
  }
  else
    return false;
}

bool checkSignal()
{      
      if(Serial.available() == 0) // Not connected to computer
      { 
        lcd.setCursor(0, 1);
        lcd.print("No signal");
        int timer = 6;
        while(timer >= 0)
        {
          lcd.print(".");
          timer--;
          if(buttonIsPressed(PUSHBUTTON_PIN))
          {
            return false;
          }
          for(int i = 0; i < 31000; i++) // Delay while checking if button is pressed (to be able to change state without having to wait)
          {
            if(buttonIsPressed(PUSHBUTTON_PIN))
             {
                return false;
             }
          }
          delay(100);
          if(Serial.available() != 0) 
          {
            lcd.clear();
            lcd.print("Signal found");
            lcd.setCursor(0,1);
            lcd.print("Connecting");
            for(int i = 0; i < 3; i++)
            {
              lcd.print(".");
              delay(1000);
            }
            return true;
          }
        }
        
        if(Serial.available() == 0) // if still not connected, move to next state
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("No signal found:");
          lcd.setCursor(0,1);
          lcd.print("Changing mode");
          for(int i = 0; i < 4; i++)
          {
            lcd.print(".");
            delay(1000);
          }
          return false;
      }
    }
    else
      return true;
}

void bgLighting()
{
    lcd.setCursor(0, 0);
    lcd.print("BG Lighting");
    
    if(checkSignal() == false)
      goto nextstate1;

    lcd.clear();
    while(!buttonIsPressed(PUSHBUTTON_PIN))
    { 
    lcd.setCursor(0, 0);
    lcd.print("   BG Lighting");
    //protocol expects data in format of 4 bytes
    //(xff) as a marker to ensure proper synchronization always
    //followed by red, green, blue bytes
    if (Serial.available() >= 4) {
      if(Serial.read() == 0xff){
        red = Serial.read();
        green= Serial.read();
        blue = Serial.read();
      }
    }
    // finally control led brightness through pulse-width modulation
    analogWrite(REDPIN, red);
    analogWrite(GREENPIN, green);
    analogWrite(BLUEPIN, blue);
    Serial.write('R');

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   BG Lighting");
    lcd.setCursor(3,1);
    lcd.print(red);
    lcd.print(" ");
    lcd.print(green);
    lcd.print(" ");
    lcd.print(blue);
   
  }
  nextstate1: state++;
}

void colorSwirl()
{
  int FADESPEED = 5;
  while(!buttonIsPressed(PUSHBUTTON_PIN))
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("     Color");
    lcd.setCursor(5, 1);
    lcd.print("Swirl");
      int r = 0;
      int g = 0;
      int b = 0;
       //fade from blue to violet
      for (r = 0; r < 256; r++) { 
        if(buttonIsPressed(PUSHBUTTON_PIN))
          goto exitfunction;
          
        analogWrite(REDPIN, r);
        delay(FADESPEED);
      } 
       //fade from violet to red
      for (b = 255; b > 0; b--) { 
        if(buttonIsPressed(PUSHBUTTON_PIN))
          goto exitfunction;
          
        analogWrite(BLUEPIN, b);
        delay(FADESPEED);
      } 
      // fade from red to yellow
      for (g = 0; g < 256; g++) { 
        if(buttonIsPressed(PUSHBUTTON_PIN))
          goto exitfunction;
          
        analogWrite(GREENPIN, g);
        delay(FADESPEED);
      } 
      // fade from yellow to green
      for (r = 255; r > 0; r--) { 
        if(buttonIsPressed(PUSHBUTTON_PIN))
          goto exitfunction;
          
        analogWrite(REDPIN, r);
        delay(FADESPEED);
      } 
      // fade from green to teal
      for (b = 0; b < 256; b++) { 
        if(buttonIsPressed(PUSHBUTTON_PIN))
          goto exitfunction;
          
        analogWrite(BLUEPIN, b);
        delay(FADESPEED);
      } 
      // fade from teal to blue
      for (g = 255; g > 0; g--) { 
        if(buttonIsPressed(PUSHBUTTON_PIN))
          goto exitfunction;
          
        analogWrite(GREENPIN, g);
        delay(FADESPEED);
      } 
  }
  exitfunction: state++;
}

void joystick()
{
  lcd.setCursor(0, 0);
  lcd.print("   Joystick");
  lcd.setCursor(0, 1);
  lcd.print("      OFF");
  
  float scale = 255.0/1024;
  int xpin = A1;    // select the input pin for the potentiometer
  int ypin = A0;
  int pushpin = JOYSTICK_BUTTON_PIN;
  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
  pinMode(pushpin, INPUT);
  
  int X = 0; 
  int Y = 0;
  int Z = 0;
  
  while(!buttonIsPressed(PUSHBUTTON_PIN))
  {
    X = analogRead(ypin);
    Y = analogRead(xpin);
    Z = digitalRead(pushpin);
  
    X = ((float) X * scale);
    Y = ((float) Y * scale);
    Z = !Z; // Joystick switch is asserted when not pushed

    // LCD displays rough estimate of color

    if(Z) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("   Joystick");
        lcd.setCursor(0, 1);
        lcd.print("      OFF");
        analogWrite(REDPIN, 0);
        analogWrite(GREENPIN, 0);
        analogWrite(BLUEPIN, 0);
    }
    else {
      if(X < 10) // magenta to blue
      {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Joystick");
          lcd.setCursor(0, 1);
          lcd.print("   BLUE-PINK");
          analogWrite(REDPIN, Y);
          analogWrite(GREENPIN, 0);
          analogWrite(BLUEPIN, 255);
      }
      else if(Y < 10) // bottom third of wheel
      {
        if(X < 125) // from blue to cyan
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Joystick");
          lcd.setCursor(0, 1);
          lcd.print("   BLUE-PINK");
          analogWrite(REDPIN, 0);
          analogWrite(GREENPIN, 2*X);
          analogWrite(BLUEPIN, 255);
        }
        else // from cyan to green
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Joystick");
          lcd.setCursor(0, 1);
          lcd.print("  GREEN-BLUE");
          analogWrite(REDPIN, 0);
          analogWrite(GREENPIN, 255);
          analogWrite(BLUEPIN, 255 - X);
        }
      }
      else if(X > 245) // left third of wheel
      {
        if(Y < 255) // from magenta to blue
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Joystick");
          lcd.setCursor(0, 1);
          lcd.print(" GREEN-YELLOW");
          analogWrite(REDPIN, Y);
          analogWrite(GREENPIN, 255);
          analogWrite(BLUEPIN, 0);
        }
      }
      else if(Y > 245) // top third
      {
        if(X > 119) // right half of top third
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Joystick");
          lcd.setCursor(0, 1);
          lcd.print("  RED-YELLOW");
          analogWrite(REDPIN, 255);
          analogWrite(GREENPIN, (X - 120)*2);   
          analogWrite(BLUEPIN, 0);
        }
        else
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Joystick");
          lcd.setCursor(0, 1);
          lcd.print("   PINK-RED");
          analogWrite(REDPIN, 255);
          analogWrite(GREENPIN, 0);
          analogWrite(BLUEPIN, 255 - 2*X);
        }
      }
    } 
    delay(10);

    // Serial monitor values for debugging purposes
//    Serial.print(X);
//    Serial.print("     ");
//    Serial.print(Y);
//    Serial.print("     ");
//    Serial.println(Z);

  } 
  state++;
}

void partyMusic(listNode* colorList) 
{
  lcd.setCursor(0, 0);
  lcd.print("   Party Mode");
  
  const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
  unsigned int sample;
  pinMode(A5, INPUT);
  int average = sum / queueItems;
  int count = 0;
  
  while(!buttonIsPressed(PUSHBUTTON_PIN)) {
    unsigned long startMillis= millis();  // Start of sample window
    unsigned int peakToPeak = 0;   // peak-to-peak level
    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;
    
   // Collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(A5);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   
   peakToPeak = signalMax - signalMin;  // Peak to peak amplitude

   int value = queue.dequeue();
   sum = sum - value + peakToPeak;
   queue.enqueue(peakToPeak);
   average = sum / queueItems;
   float weightedAvg = average;
   if(average < 15) {
      weightedAvg = weightedAvg * 1.6;
   }
   else {
      weightedAvg = weightedAvg * 1.2; // When noise is very low, make less sensitive to other (background) noises
   }
   

   if(peakToPeak > weightedAvg) {
      Serial.print(peakToPeak);
      Serial.print("    ");
      Serial.println(weightedAvg);
      analogWrite(REDPIN, colorList->redVal);
      analogWrite(GREENPIN, colorList->greenVal);
      analogWrite(BLUEPIN, colorList->blueVal);
      colorList = colorList->next;
   }

   
  }
  state = 0;
}

