# EarthwatchSensorCode
Code for the Green Infrastructure Sensors for the Earthwatch Project US/Canada STP program (https://ewgis.org/earthwatch-stp/north-america/index.html)

This code is to collect data from Green Infrastructure sensors and send, via cell-network, to a google sheet.

The components of the sensors are as follows:
 - Particle Electron
 - Adafruit Thermistor
 - Two Vegetronix soil moisture sensors (buried at 10 cm and 20 cm)
 - One DFRobot soil moisture sensor (used as a trigger)
 - An Adafruit 6600 mAh lipo battery
 
The initial code (PWD_SoilCell3G_Template) was borrowed from a Philadelphia Water Department project, described here:http://greenstemnetwork.org/diy/soil-cell-3g/  This site also gives a good intro to constructing and waterproofing similar sensors.

The wiring for the sensors, with this code, is:
  1.	Before attaching sensors:  hook up GRND pins to the – line of the breadboard.  Each GRND pin should be attached to the – on its side of the breadboard
  2.	Attach 4 in soil moisture sensor (you will probably need to strip some of the plastic tube):
    a.	red wire to D5
    b.	black wire to A5
    c.	bare wire to – line on the right side of the breadboard
  3.	Attach 8 in soil moisture sensor (you will probably need to strip some of the plastic tube):
    a.	red wire to D3
    b.	black wire to A3
    c.	bare wire to – line on the right side of the breadboard
  4.	Attach etape:
    a.	White wire to A2
    b.	Red wire to D2
    c.	black wire to – line on the left side of the breadboard
  5.	Attach thermistor:
    a.	One end of thermistor connects to the – line on the left side of the breadboard
    b.	Connect the resistor that comes with the thermistor between pin D0 and a row on the breadboard past the edge of the board
    c.	Connect the other end of the thermistor to a hole in the same row as the resistor
    d.	Connect a wire between pin A0 and a hole in the breadboard in the same row as the resistor, with the resistor between the new wire and the thermistor


In order to receive data, the code needs to be slightly updated, and webhooks set up as follows:
  1. Update for the timezone of the city.  Around line 240 the code has “Time.zone(-5);”  the -5 represents a 5 hour difference between CDT and UTC timezones.  This needs to be changed based on the city location and time of year
  2.	Update for a new google sheet for the city (see below for more).  Around lines 265 and 272 the code has “Particle.publish("Test", "{\"temp\":\"" + String(temp) +"\", \"soil1\":\"" + String(soilT) + "\", . . . “ 
    a.	“Test” refers to the name of the google doc.  You want to change this to the city name and set up a google doc for the city as in the bullet below
  3.	Creating new google sheet hooked up to sensor code
    a.	Use the instructions laid out in these links: https://www.hackster.io/gusgonnet/pushing-data-to-google-docs-02f9c4
http://railsrescue.com/blog/2015-05-28-step-by-step-setup-to-send-form-data-to-google-sheets/


**Note that the output of the soil moisture sensors is not soil moisture and these sensors must be calibrated to a certain soil type, for info check the soil sensor documantation **
  

