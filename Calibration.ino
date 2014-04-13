// Loop used for calibration
void CalibrationLoop()
{
  // Read data from the compass
   compass.read();
  
  // Update the minimum values
  running_min.x = min(running_min.x, compass.m.x);
  running_min.y = min(running_min.y, compass.m.y);
  running_min.z = min(running_min.z, compass.m.z);

  // Update the maximum values
  running_max.x = max(running_max.x, compass.m.x);
  running_max.y = max(running_max.y, compass.m.y);
  running_max.z = max(running_max.z, compass.m.z);
  
  // Output the current totals for min and max to
  // serial for visual check of data
  
  // Minimum values
  Serial.print("M min ");
  Serial.print("X: ");
  Serial.print((int)running_min.x);
  Serial.print(" Y: ");
  Serial.print((int)running_min.y);
  Serial.print(" Z: ");
  Serial.print((int)running_min.z);
  
  // Maximum values
  Serial.print(" M max ");  
  Serial.print("X: ");
  Serial.print((int)running_max.x);
  Serial.print(" Y: ");
  Serial.print((int)running_max.y);
  Serial.print(" Z: ");
  Serial.println((int)running_max.z);
  
  // Turn off the led
  digitalWrite(LED, LOW);
  
  // Turn LED back on if we have any change
  // in the minimum values.
  if(running_min.x != configuration.E_min_X || running_min.y != configuration.E_min_Y || running_min.z != configuration.E_min_Z)
  { digitalWrite(LED, HIGH); }
  
  // Turn LED back on if we have any change
  // in the maximum values.
  if(running_max.x != configuration.E_max_X || running_max.y != configuration.E_max_Y || running_max.z != configuration.E_max_Z)
  { digitalWrite(LED, HIGH); }
  
  // Update the configuration store ready for
  // saving data to the EEPROM
  
  // Update the minimum values
  configuration.E_min_X = running_min.x;
  configuration.E_min_Y = running_min.y;
  configuration.E_min_Z = running_min.z;
  
  // Update the maximum values
  configuration.E_max_X = running_max.x;
  configuration.E_max_Y = running_max.y;
  configuration.E_max_Z = running_max.z;
  
  // Pause a little to make things easyer to read.
  delay(100);
}
