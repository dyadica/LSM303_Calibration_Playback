// Loop used for playback
void PlaybackLoop()
{
  // Read data from the compass
  compass.read();
  // Get the heading
  int heading = compass.heading((LSM303::vector<int16_t>){0,-1,0});
  // Output the heading
  Serial.println(heading);
  // Pause a little to make things easyer to read.
  delay(100);
}
