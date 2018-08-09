#include <IRremote.h>
#include <FastLED.h>

#define IR_PIN          3
#define LED_PIN         13    
#define LED_COUNT       120

#define IR_None         0x000000
#define IR_Up           0xFF02FD
#define IR_Down         0xFF9867

#define TV_Power_UNDEFINED      0xFF
#define TV_Power_UP             1
#define TV_Power_DOWN           0
  
uint8_t PowerStatusPrevious = TV_Power_UNDEFINED;
uint8_t PowerStatusCurrent = TV_Power_UNDEFINED;

decode_results IRStatusPrevious;
decode_results IRStatusCurrent;
IRrecv IR(IR_PIN);

CRGB leds[LED_COUNT];
uint8_t currentHue = 0;

void LED_Off()
{
  for(uint8_t i = 0; i <255; i++)
  {
    for(uint8_t j= 0; j<LED_COUNT; j++)
    {
      leds[j] = CHSV( currentHue, 255, 254-i);
    }
    FastLED.show();
    delay(6);

  }
}

void LED_On()
{
  for(uint8_t i = 0; i <255; i++)
  {
    for(uint8_t j= 0; j<LED_COUNT; j++)
    {
      leds[j] =  CHSV( currentHue, 255, i);
    }
    FastLED.show();
    delay(6);
  }
}

void GetPowerStatus()
{
  float PowerVoltage = analogRead(A7)*(5.0/1024.0);
  EVERY_N_SECONDS(10) 
  {
    Serial.print("TV USB Voltage: ");
    Serial.print(PowerVoltage);
    Serial.println("V");
  }
  if(PowerVoltage < 4.0)
    PowerStatusCurrent = TV_Power_DOWN;
  else if(PowerVoltage > 4.2)
    PowerStatusCurrent = TV_Power_UP;
}

void TVPowerManage()
{
  GetPowerStatus();
  if(PowerStatusCurrent != PowerStatusPrevious)
  {
    PowerStatusPrevious = PowerStatusCurrent;
    if(PowerStatusCurrent == TV_Power_UP)
    {
      Serial.println("TV Power: UP");
      LED_On();
    }
    else
    {
      Serial.println("TV Power: DOWN");
      LED_Off();
    }
  }
}

bool IR_idle() 
{
  return IR.decode(&IRStatusCurrent) || IRStatusCurrent.rawlen == 0;
}

void IR_Manage()
{
  if (IR.decode(&IRStatusCurrent))
  {
    if(IRStatusCurrent.value != IRStatusPrevious.value)
    {
      IRStatusPrevious = IRStatusCurrent;
      Serial.println(IRStatusCurrent.value);
      switch (IRStatusCurrent.value) 
      {
        case IR_Up:
          break;
        case IR_Down:
          break;
      }
    }
    IR.resume();
  }
}

void LED_Manage()
{
  EVERY_N_SECONDS(1)
  {
    currentHue = (currentHue+1)%255;
    for(uint8_t i = 0; i < LED_COUNT; i++)
      leds[i] = CHSV( currentHue, 255, 255);
    FastLED.show();
  }
}

void setup() 
{
  Serial.begin(9600);
   Serial.println("Serial: Enabled");
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  IR.enableIRIn();
  Serial.println("IR: Enabled");
}

void loop()
{
  TVPowerManage();
  IR_Manage();
  if(PowerStatusCurrent == TV_Power_UP)
  {
    if (IR_idle()) 
      LED_Manage();
      
    else 
      Serial.println("skipped FastLED show()");
  }
}
