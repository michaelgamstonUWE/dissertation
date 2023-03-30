# Port of our ESP32-CAM code to ESP-IDF
Main changes are removing arduino dependencies (mainly around SPI + replacing Serial prints with printf) and adding in CNN functionality. <br>
Currently CNN is not functional, but the tinyml code should run without crashing and give indictive results for timing purposes. <br>
Have tested this all works up to the point where we actually send stuff over SPI.
