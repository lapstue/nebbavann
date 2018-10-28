
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>

//Define Tempreader

//Must be a DIGITAL pin!
#define DHTPIN 2 
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
//Define pins for the range finder, digital
#define trigPin 10
#define echoPin 11

//How much a single cm changes the volume of liquid
#define literPerDistance 3.84
//The volume of liquid when the tank is full
#define fullCapacity 250
//Height from mountpoint to water level when tank is full
#define heightOffset 0

int volume;

LiquidCrystal_I2C lcd(0x3f, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
NewPing sonar(trigPin,echoPin);

void setup() {
  Serial.begin (9600);

  

  //Initialize LCD
  lcd.begin(16,2); 
  lcd.backlight();

  //Initialize range detector  
  //pinMode(trigPin, OUTPUT);
  //pinMode(echoPin, INPUT);
  dht.begin();


}

float findVolume(float temp){
  float duration, distance, measured_distance, speed;

  //Take the median of 7 readings
  duration = sonar.ping_median(7);
  //Speed of sound at current temp
  speed = (331.3+0.606*temp);

  //Find the distance by dividing a meter by the total roundtrip time divided by two.
  //Notice that we use 10,000 instead of 100
  distance = duration/(10000/speed)/2;
  //Weed need to account for the fact that the sensor is not at water level once the tank is full
  //Experiment with this value, try adding instead of subtracting
  distance = distance - heightOffset;
  
  //Serial.print("Avstanden er: ");
  //Serial.println(distance);
  float volume = (round(fullCapacity-(distance*literPerDistance))*10)/10;
  if(volume<0){
    return 0;
  }
  else{
    return volume;
  }
}

void LCD_progress_bar (int row, int var, int minVal, int maxVal)
{
  int block = map(var, minVal, maxVal, 0, 16);   // Block represent the current LCD space (modify the map setting to fit your LCD)
  int line = map(var, minVal, maxVal, 0, 80);     // Line represent the theoretical lines that should be printed
  int bar = (line-(block*5));                             // Bar represent the actual lines that will be printed
 
  /* LCD Progress Bar Characters, create your custom bars */

  byte bar1[8] = { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
  byte bar2[8] = { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
  byte bar3[8] = { 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
  byte bar4[8] = { 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E};
  byte bar5[8] = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
  lcd.createChar(1, bar1);
  lcd.createChar(2, bar2);
  lcd.createChar(3, bar3);
  lcd.createChar(4, bar4);
  lcd.createChar(5, bar5);
 
  for (int x = 0; x < block; x++)                        // Print all the filled blocks
  {
    lcd.setCursor (x, row);
    lcd.write (1023);
  }
 
  lcd.setCursor (block, row);                            // Set the cursor at the current block and print the numbers of line needed
  if (bar != 0) lcd.write (bar);
  if (block == 0 && line == 0) lcd.write (1022);   // Unless there is nothing to print, in this case show blank
 
  for (int x = 16; x > block; x--)                       // Print all the blank blocks
  {
    lcd.setCursor (x, row);
    lcd.write (1022);
  }
}

float readTemp(){
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  //Serial.print("Tempen er: ");
  //Serial.println(t,1);
  //Serial.print("PH er: ");
  //Serial.println(h);
 
  return t;
}

void loop() {
  //Get water level and temperature
  //We need to get the airtemp first, as the speed of sounds varies according to this factor
  float temp = readTemp();
  //Number of liters as integer
  volume = int(findVolume(temp));
  
  //Clear LCD litre meassurements from last time
  lcd.setCursor(3,0);
  lcd.print("    ");
  lcd.setCursor(3,0);

  lcd.print(volume);
  lcd.print("L");
  Serial.print("Vannmengde:");
  Serial.println(volume);
  
  //Print temperature
  lcd.setCursor(9,0);
  //Make sure we only show one decimal place
  lcd.print(temp,1);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  
  //Print fillbar
  LCD_progress_bar(1,volume,0,fullCapacity);

  //Wait five seconds before reading temp and volume again
  delay(5000);
}

