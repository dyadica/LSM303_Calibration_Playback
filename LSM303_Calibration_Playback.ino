// Simple LSM303 Calibration via EEPROM

// The following script demonstrates the calibration and saving of calibration data
// to the arduino's EEPROM. This data can then be recalled to configure the LSM303.
// This script utilises serial communication to switch beetween both calibration and
// playback modes. In order to switch just send an "enter keypress" via the Arduino
// terminal. When in calibration mode move the sensor around untill the LED no longer 
// lights. I utilise a figure of 8 action to achieve this.

// Wiring for the Arduino Uno/Duemilanove -> LSM303 board

// 5V            ->  VIN
// GND           ->  GND
// Analog Pin 5  ->  SCL
// Analog Pin 4  ->  SDA

// Wiring for the Arduino Mega  -> LSM303 board

// 5V              ->  VIN
// GND             ->  GND
// Digital Pin 21  ->  SCL
// Digital Pin 20  ->  SDA 

// http://www.dyadica.co.uk/journal/simple-serial-string-parsing/
// This code utilises the simple serial string parsing example which can be
// found at the above location.

// http://playground.arduino.cc/Code/EEPROMWriteAnything
// This code utilises the EEPROM Write Anything example which can be found at 
// the above location.

// https://github.com/pololu/LSM303
// In order to utilise this code you will need to download the Pololu LSM303
// lib and install it within your Arduino lib folder. The example files used
// for this lib formulate the basis of this script.

// Arduino EEPROM
#include <EEPROM.h>
// Custom code by Arduino that enables the saving of long data
#include "EEPROMAnything.h"

// Wire library for I2C
#include <Wire.h>
// LSM303 library by Pololu
#include <LSM303.h>

// Define the compass
LSM303 compass;
// Set min and max variables for calibration
LSM303::vector<int16_t> running_min = {2047, 2047, 2047}, running_max = {-2048, -2048, -2048};

// Define a struct to store data to
struct config_t
{
  int E_min_X; 
  int E_min_Y; 
  int E_min_Z;
  int E_max_X; 
  int E_max_Y; 
  int E_max_Z; 
  boolean Calibrated; 
}configuration;

// Define an LED port to display status
int LED = A0;

// By default set the calibration mode to
// true so we start in playback mode. This
// is overridden in setup.
boolean CalibrationMode = true;

// Buffer for the incoming data
char inData[100];
// Buffer for the parsed data chunks
char *inParse[100];

// Store for function identifier
char *func;
// Store for property value
char *prop;

// Incoming data id
int index = 0;
// Read state of incoming data
boolean stringComplete = false;

// Set up the system
void setup() 
{
  // Deefine the LED as an output.
  pinMode(LED, OUTPUT);
  
  // Load stored data from the EEPROM if not 
  // showing saved data (call contained within)
  // EEPROM_readAnything(0, configuration);
  
  // Initialise the serial port
  Serial.begin(9600);
  
  // Load and show data via serial.
  ShowSavedData();
  // Pause so that data can be read.
  delay(5000);
  
  // Initiate the Wire library and join the I2C bus as 
  // a master or slave
  Wire.begin();  
  // Initialise the compass
  compass.init();
  compass.enableDefault();
  
  // Calibration values. Use the Calibrate example program 
  // to get the values for your compass.
  
  // Minimum values
  compass.m_min.x = configuration.E_min_X; 
  compass.m_min.y = configuration.E_min_Y; 
  compass.m_min.z = configuration.E_min_Z;
  // Maximum values
  compass.m_max.x = configuration.E_max_X;  
  compass.m_max.y = configuration.E_max_Y; 
  compass.m_max.z = configuration.E_max_Z;
  
  // Get the calibration mode
  CalibrationMode = configuration.Calibrated;
}

// Master loop
void loop() 
{
  // Check to see if we have any serial input
  // and need to change mode.
  InputCheck();
  
  // Run the loop for either calibration or
  // playback depending upon calibration mode.
  if(CalibrationMode){ PlaybackLoop(); } 
  else { CalibrationLoop(); }
}

// Check for serial input.
void InputCheck()
{
  if (stringComplete) 
  {
    // Reset the system for further data   
    stringComplete = false; 
    
    // Update the calibration mode
    CalibrationMode = !CalibrationMode;
    
    // If calibration then set to false and save
    // the data to the EEPROM
    if(CalibrationMode)
    {
      // Show some status
      Serial.println("Saving calibration data!");
      // Set calibration to calibrated
      configuration.Calibrated = true;
      // Save data to the EEPROM
      EEPROM_writeAnything(0, configuration);
      // Pause to ensure that the save has completed
      delay(500);
      // Show some status
      Serial.println("Calibration data saved!");
      // Pause a little to make things easyer to read.
      delay(500);
    }
    else
    {
      // Set the configuration store calibration to false.
      configuration.Calibrated = false;
      
      // Show some status
      Serial.println("Clearing calibration data!");
      
      // write a 0 to all 512 bytes of the EEPROM
      for (int i = 0; i < 512; i++) { EEPROM.write(i, 0); }
      
      // Show some status
      Serial.println("Calibration data cleared!");
      // Pause a little to make things easyer to read.
      delay(500);
    }
  }
}

// Read data from EEPROM and print to serial
void ShowSavedData()
{
  // Print a seperation line for easy reading
  Serial.println("******************************");
  // Read in the data from the EEPROM
  EEPROM_readAnything(0, configuration);
  // Pause a little just in case 
  delay(500);
  // Output Min X via serial
  Serial.print("EEPROM_Min_X: ");
  Serial.print(configuration.E_min_X);
  Serial.println();
  // Output Min Y via serial
  Serial.print("EEPROM_Min_Y: ");
  Serial.print(configuration.E_min_Y);
  Serial.println();
  // Output Min Z via serial
  Serial.print("EEPROM_Min_Z: ");
  Serial.print(configuration.E_min_Z);
  Serial.println();
  // Output Max X via serial
  Serial.print("EEPROM_Max_X: ");
  Serial.print(configuration.E_max_X);
  Serial.println();
  // Output Max Y via serial
  Serial.print("EEPROM_Max_Y: ");
  Serial.print(configuration.E_max_Y);
  Serial.println();
  // Output Max Z via serial
  Serial.print("EEPROM_Max_Z: ");
  Serial.print(configuration.E_max_Z);
  Serial.println();
  // Output Calibration mode via serial
  Serial.print("Calibrated: ");
  Serial.print(configuration.Calibrated);
  Serial.println();
  // Print a seperation line for easy reading
  Serial.println("******************************");
}

void serialEvent()
{
  // Read while we have data
  while (Serial.available()) 
  {
    // Read a character
    char inChar = Serial.read(); 
    // Store it
    inData[index] = inChar; 
    // Increment where to write next
    index++;     
    // Check for termination character
    if (inChar == '\n') 
    {
      // Reset the index
      index = 0;
      // Set completion of read to true
      stringComplete = true;
    }
  }
}
