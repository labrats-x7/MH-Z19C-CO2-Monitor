#include <Arduino.h>
#include <MHZ19.h>                                    
#include <SoftwareSerial.h>
#include <Adafruit_SSD1306.h>

#define RX_PIN 10                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 11                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin) (not used)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial

unsigned long getDataTimer = 0;

void setup()
{
  Serial.begin(9600);                                     // Device to serial monitor feedback
  
  mySerial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
  myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 
  
  myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))

  //Setup OLED--------------------------
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(6,13);
  display.println(F("CO2 & TVOC"));
  display.setCursor(35,40);
  display.println(F("Meter"));
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
}

void loop()
{
  if (millis() - getDataTimer >= 2000){
    int CO2; 
  
    /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
    if below background CO2 levels or above range (useful to validate sensor). You can use the 
    usual documented command with getCO2(false) */
  
    CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)
    
    Serial.print("CO2 (ppm): ");                      
    Serial.println(CO2);                                
  
    int8_t Temp;
    Temp = myMHZ19.getTemperature();                     // Request Temperature (as Celsius)
    Serial.print("Temperature (C): ");                  
    Serial.println(Temp);                               
  
    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(13,0);
    display.print(F("CO2 Meter"));
    display.setCursor(1,12);
    display.print(F("Quali:"));
    display.setTextSize(2);
    
    if(CO2<800){
      display.print(F("gut"));
    }
    if(CO2>=800 && CO2<1000){
      display.print(F("mittel"));
    }
    if(CO2>=1000 && CO2<1400){
      display.print(F("minder"));
    }
    if(CO2>=1400 && CO2<2400){
      display.print(F("niedrig"));
    }
    if(CO2>=2400 && CO2<3500){
      display.print(F("lausig"));
    }  
    if(CO2>=3500 && CO2<5000){
      display.print(F("grausig"));
    } 
    if(CO2>=5000 && CO2<10000){
      display.print(F("DINmax"));
    } 
    if(CO2>=10000 && CO2<25000){
      display.print(F("ACHTUNG"));
    } 
    if(CO2>=25000){
      display.print(F("GEFAHR!"));
    } 
    display.setTextSize(1);
    display.setCursor(1,31);           
    display.print(F("CO2: "));
    display.setTextSize(2);
    display.print(CO2);
    display.println(F("ppm"));
    if(CO2>2400){
      display.invertDisplay(true);
    }
    else{
      display.invertDisplay(false);
    }
    display.display();
    getDataTimer = millis();
  }
}
