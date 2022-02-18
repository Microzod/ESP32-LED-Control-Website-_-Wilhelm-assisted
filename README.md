# ESP32-LED-Control-Website-_-Wilhelm-assisted
Application to control my living-room LED light fixture.

These files contain a project which creates a local web-page called esp.lan through which a ESP32 receives
parameters that are used by the ESP32 to control the color temperature, intensity and automatic turn-ON/turn-OFF
of a LED light fixture in my living-room.

The LED light fixture is built with the following parts:
¤ - 1 single 1120 mm LED strip
  - Bridgelux® Vesta™ Tunable Linear Gen 3 Thrive 2 SMD Rows - featuring 2700K 3-Step MacAdam Ellipse & 5000K 3-Step MacAdam Ellipse LEDs.
  - https://www.bridgelux.com/sites/default/files/resource_media/DS139%20Bridgelux%20Vesta%20TL%20Gen%203%20Thrive%202%20Rows%20Data%20Sheet%20081320%20Rev%20A.pdf

¤ - 2 XLG-50-AB 50W Constant Power Mode LED Driver(Mean Well)
  - https://www.meanwellusa.com/upload/pdf/XLG-50/XLG-50-spec.pdf
  
¤ - Costum PrintedCircuitBoard(PCB) featuring:
    - ESP32-WROVER
    - Solid State Relays to switch power for the LED drivers ON/OFF.
    - LTC2633 12bit DAC + Op-Amp circuit to generate 0V - 10V LED driver control signal.
    - 
    - 
    - 
