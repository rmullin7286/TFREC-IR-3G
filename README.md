# TFREC-IR-3G
New version of the TFREC project, uses 3G to upload directly to Dropbox

This repo will have two Forks: one for the PiSensor and one for the PiHub.

DEPENDENCIES:
  Hardware:
    - MLX90614
    - nRF24l01
    - 2x Raspberry Pi 3
    - Adafruit Char LCD Shield
    - DIYMall 3G shield
    - AT&T GoPhone SIM Card
    - Cables for GPIO connection
  
  Software:
    - bcm2835 library from airspayce for mlx90614: http://www.airspayce.com/mikem/bcm2835/
    - RF24 library for nRF24l01: https://github.com/nRF24/RF24
    - ppp package for 3G: sudo apt-get install ppp
    - Sakis3G script for connecting to 3G: http://raspberry-at-home.com/installing-3g-modem/
    - Python 2.7
    - RPi.GPIO
    - WiringPi: http://wiringpi.com/download-and-install/
    - Adafruit Char LCD: https://learn.adafruit.com/adafruit-16x2-character-lcd-plus-keypad-for-raspberry-pi/usage
    - Python.h
    
    
