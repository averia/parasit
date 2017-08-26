#include <EEPROM.h>
#include "LedControl.h"

#define ENABLED 255                         // Value to enable the 7 bytes slot at the current EEPROM addr
#define DISABLED 0                          // Value to disable the 7 bytes slot at the current EEPROM addr
#define NUM_MAX_SLOTS 80                  // Number of 7 bytes slots in the EEPROM
#define EEPROM_SAVE_TIME  60               // Value thats defines the time (in seconds) for saving to EEPROM.

int version = 70;
// Analog Inputs
int potPin = 2;                              // Potentiometer to control the clkTime
// Digital Inputs
int switchPin = 11;                          // Photo Cellules to initiate counter
int relaisPin = 12;                          // Relais to emit noise while counting.
int resetPin = 2;                           // Reset Pin to reset counter
int relState = 0;

int digit[]={ 1,0,6,0,6,0,2,4,3,0,1,2,9,9,9,9};      // Initial Value to display during the Check Digits
int milli=0;
int seconds=0;
int minutes=0;
int hours=0;
int days=0;
int months=0;
int years=0;
int monthyears=0;
int monthDays[]={
  31,28,31,30,31,30,31,31,30,31,30,31};
int temp;                      // Temporary variable for general purpose
int i=0;
int j=0;
int number=0;
int index=0;
int clkTime = 1000;
byte isInitialised = 1;                // Initialization flag
unsigned int addr = 0;      // Initial EEPROM address where to read and write
unsigned int slot = 0;      // Initial EEPROM 7 byte slot where to read and write

LedControl lc = LedControl (3,4,5,2);       // DataPin, ClockPin, EnablePin, Num of MAX7219
//LedControl frame2 = LedControl (6,7,8,1);       // DataPin, ClockPin, EnablePin, Num of MAX7219

void setup()
{
  lc.shutdown(0, false); // Wake up the display
  lc.setIntensity(0, 15); // Set Brightness 0-15
  lc.clearDisplay(0);  // Clear display
  lc.shutdown(1, false); // Wake up the display
  lc.setIntensity(1, 15); // Set Brightness 0-15
  lc.clearDisplay(1);  // Clear display
  
  pinMode(switchPin, INPUT);           // sets the Switch pin as input
  pinMode(relaisPin, OUTPUT);          // sets the Relais pin as output
  pinMode(resetPin, INPUT);            // sets the Reset pin as input  

}


void update_time()
{
  milli++;
  if (milli == 1000)
  {
    seconds++;
    milli = 0;
  }
  
  seconds++;
  if (seconds == 60)
  {
    minutes++;
    seconds = 0;
  }
  if (minutes == 60)
  {
    hours++;
    minutes = 0;
  } 

  if (hours == 24)
  {
    days++;
    hours = 0;
  }

  //if (days == monthDays[months])
  if (days == 30)
  {
    months++;
    days = 0;
  }

  if (months == 12)
  {
    years++;
    months = 0;
  }  

  if (years == 99)
  {
    years = 0;
  }
  
}

void update_leds()
{

  digit[1]= milli%100;
  digit[0] = milli/100;
  digit[3]= seconds%10;
  digit[2] = seconds/10;
  digit[5] = minutes%10;
  digit[4] = minutes/10;
  digit[7] = hours%10;
  digit[6] = hours/10;
  digit[9] = days%10;
  digit[8] = days/10;
  digit[11] = months%10;
  digit[10] = months/10;
  digit[13] = years%10;
  digit[12] = years/10;
  digit[15] = monthyears%100;
  digit[14] = monthyears/100;
  // Update 8 first Digits (Frame1)
  lc.setDigit (0,0,digit[0],false);
  lc.setDigit (0,1,digit[1],false);
  lc.setDigit (0,2,digit[2],false);
  lc.setDigit (0,3,digit[3],true);
  lc.setDigit (0,4,digit[4],false);
  lc.setDigit (0,5,digit[5],true);
  lc.setDigit (0,6,digit[6],false);
  lc.setDigit (0,7,digit[7],true);
  // Update 8 Last Digits (Frame2)
  lc.setDigit (1,0,digit[8],false);
  lc.setDigit (1,1,digit[9],false);
  lc.setDigit (1,2,digit[10],false);
  lc.setDigit (1,3,digit[11],true);
  lc.setDigit (1,4,digit[12],false);
  lc.setDigit (1,5,digit[13],true);
  lc.setDigit (1,6,digit[14],false);
  lc.setDigit (1,7,digit[15],true);

 
}
void checkDigits()
{

  // Set Initial Default Values for check
  for(i=0;i<8;i++)
  {
    lc.setDigit (0,i,i,false);
  }

  delay(13000);   // Wait 3000 milliseconds

  for(i=0;i<8;i++)
  {
    lc.setDigit (0,i,digit[i],false);
  }  

}

void eeprom_save ()
{
  int time2Save;

  // Set the time to save to every EEPROM_SAVE_TIME seconds
  time2Save = (minutes * 60 + seconds) % EEPROM_SAVE_TIME;


  if (time2Save == 0)
  {

    // Enable the next 7 bytes slot in EEPROM
    addr = 8* slot;
    EEPROM.write (addr, ENABLED);
    EEPROM.write ((addr + 1), milli);
    EEPROM.write ((addr + 2), seconds);
    EEPROM.write ((addr + 3), minutes);
    EEPROM.write ((addr + 4), hours);
    EEPROM.write ((addr + 5), days);
    EEPROM.write ((addr + 6), months);
    EEPROM.write ((addr + 7), years);
    EEPROM.write ((addr + 8), monthyears);
    
    if (slot == 0)
    {
      addr = 8 * NUM_MAX_SLOTS;
    }
    else
    {
      addr = 8 * (slot - 1);
    }
    EEPROM.write (addr, DISABLED);
    EEPROM.write ((addr + 1), 0);
    EEPROM.write ((addr + 2), 0);
    EEPROM.write ((addr + 3), 0);
    EEPROM.write ((addr + 4), 0);
    EEPROM.write ((addr + 5), 0);
    EEPROM.write ((addr + 6), 0);
    EEPROM.write ((addr + 7), 0);
    EEPROM.write ((addr + 8), 0);

    slot++;
    if (slot > NUM_MAX_SLOTS) slot = 0;

  }
}


void initialize()
{
  
  byte val;
    
  //  If reset is pressed when initializing the EEPROM is reset to zero
  if (digitalRead(resetPin) == LOW)
  {
    
    for(i=0; i<=NUM_MAX_SLOTS ; i++ )
    {
      addr = 7 * i;
      EEPROM.write (addr, DISABLED);
      EEPROM.write ((addr + 1), 0);
      EEPROM.write ((addr + 2), 0);
      EEPROM.write ((addr + 3), 0);
      EEPROM.write ((addr + 4), 0);
      EEPROM.write ((addr + 5), 0);
      EEPROM.write ((addr + 6), 0);
      EEPROM.write ((addr + 7), 0);
      EEPROM.write ((addr + 8), 0);
    }
    
  }

  // Find the slot in the EEPROM where the counter value was stored
  for(i=0; i<=NUM_MAX_SLOTS ; i++ )
  {
    val = EEPROM.read ( (8 * i) );
    if (val == ENABLED) slot = i; 
  }  

  // Read the stored values in the EEPROM
  addr = 7 * slot;
  milli = EEPROM.read ((addr + 1));
  seconds = EEPROM.read ((addr + 2));
  minutes = EEPROM.read ((addr + 3));
  hours = EEPROM.read ((addr + 4));
  days = EEPROM.read ((addr + 5));
  months = EEPROM.read ((addr + 6));
  years = EEPROM.read ((addr + 7));
  monthyears = EEPROM.read ((addr + 8));

  // Clear Display
  lc.clearDisplay(0);
  // Update digits
  update_leds();

}


void loop()
{

  if (isInitialised == 1)
  {

    //Initial Check of the Digits operation
    checkDigits ();
    delay (100); // Wait 5000  milliseconds

    initialize();
    isInitialised = 0; 

  }

  // Read the value of the potentiometer
  clkTime = analogRead(potPin);
  //  clkTime = 700;

  // Set the time increment
  if (clkTime >= 500)
  { 
    if (digitalRead(switchPin) == HIGH)
    {
      if (relState == 0)
      {
        digitalWrite (relaisPin, HIGH);
        relState = 1;
      }
      else
      {
        digitalWrite (relaisPin, LOW);
        relState = 0;
      }

      update_leds();
      delay(1000);   // Wait 1000 milliseconds
      update_time();
      eeprom_save();
    }
  }
  else
  {
    update_leds();
    delay(clkTime);   // Wait clkTime milliseconds
    update_time();
  }

}

