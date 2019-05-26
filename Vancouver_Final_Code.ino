#include <math.h>


//****************************************************************************************************************************************//
// Code for Smith Lab Rapid Green Infrastructure Assesment Bioswale Sensors. Includes two soil sensors, one etape, and one thermistor.    //
// Data is transmitted to a google Sheet using this tutorial: https://www.hackster.io/gusgonnet/pushing-data-to-google-docs-02f9c4 .      //
// Compile on Particle platform only. Adapted from Philadelphia Water Department SoilCell 3G.                                             //
// Updated 4/30/2019 with a connection check and new sensor soil moisture thresholds plus a limit on timie to run at lower time interveal //
//  And depth sensor removed                                                                                                              //
//****************************************************************************************************************************************//



//  Set-up Stuff: //
FuelGauge fuel;                                                                 //Creates FuelGauge object for use in battery metrics
float bat, soilT, soil4, soil8;                                //Initializes variables for holding values to be sent to Google Sheet
int soil4Readings[5], soil8Readings[5], tempReadings[5];                        //Creates arrays for multiple readings 
float temp, steinhart;                                                          //float to find the temperature
int minuteNow, hourNow;
int count=1;
SYSTEM_MODE(SEMI_AUTOMATIC);                                                    //Forces electron into semi-automatic mode, causing connections to only be made when and if needed


// Functions to Find Medians of 5 Readings //

void swap(int &a, int &b){                                                      //Swaps values of two variables
    int temp = a;                                                               //Assigns value of a to temp
    a = b;                                                                      //Assigns value of b to a
    b = temp;                                                                   //Assigns value of temp to b
}

int findLow(int sorting[], int low, int high){                                  //Finds lowest value in a passed array between two determined indexes

    int smallest = sorting[low];                                                //Assigns a primary "lowest" value
    int smallIndex = low;                                                       //Assigns a value to keep track of the index of the lowest number
    for(int i = low; i < high; i++){                                            //Cycles through the array between the given boundaries to find the smallest number and its index
        if(sorting[i] < smallest){                                              //Checks whether the current number is smaller than the current record
            smallest = sorting[i];                                              //If it is, set that as the record and continue
            smallIndex = i;                                                     //Also updates the index value
        }
    }
    return smallIndex;                                                          //Returns the index of the smallest value between the given boundaries
}

void selectSort(int sorting[], int high){                                       //Sorting function
    int smallIndex;                                                             //Value for holding index of smallest value in given bounds of array
    for(int low = 0; low < high; low++){                                        //Cycles through the given array, calling findLow and swap
        smallIndex = findLow(sorting, low, high);                               //Calculates the index of the lowest value in the array between the given bounds
        if (smallIndex != low){                                                 //If the lowest value is not at the current index swaps the two values
            swap(sorting[low], sorting[smallIndex]);                            //Calls swap with the two values to be swapped
        }
    }
}


// Analog Pin Readings for Sensors //

int soilTRead(){                                                                 //Takes a voltage reading from the trigger soil moisture sensor 
    delay(400);                                                                 //Delay for stablizing and giving general value reading
    return (analogRead(A1));                                                    //Takes reading and returns raw data value
}

int soil4Read(){                                                                 //Takes a voltage reading from the soil moisture sensor4 and returns an adjusted value
    delay(400);                                                                 //Delay for stablizing and giving general value reading
    return (analogRead(A5));                                                    //Takes reading and returns raw data value
}

int soil8Read(){                                                                 //Takes a voltage reading from the soil moisture sensor8 and returns an adjusted value
    delay(400);                                                                 //Delay for stablizing and giving general value reading
    return (analogRead(A3));                                                    //Takes reading and returns raw data value
}

int tempRead(){                                                                 //Takes a voltage reading from the thermistor and returns an adjusted value
    delay(400);                                                                 //Delay for stablizing and giving general value reading
    return (analogRead(A0));                                                    //Takes reading and returns raw data value
}


// Full Readings for Sensors //

void getSoilTRead(int powerPin){                                                //Gets trigger reading
    digitalWrite(powerPin, HIGH);                                               //Turns on power to the trigger soil sensor
    soilT = soilTRead();                                                        // Gets trigger reading
    digitalWrite(powerPin, LOW);                                                //Turns off power to soil sensor to save power
}

void getSoil4Read(int powerPin){                                                //Populates soil readings array with values
    digitalWrite(powerPin, HIGH);                                               //Turns on power to the soil sensor4
    for(int i = 0; i < 5; i++){                                                 //Takes 5 soil readings
        soil4Readings[i] = soil4Read();                                         //Loads a soil reading into each index of the array
    }
    digitalWrite(powerPin, LOW);                                                //Turns off power to soil sensor to save power
}


void getSoil8Read(int powerPin){                                                //Populates soil readings array with values
    digitalWrite(powerPin, HIGH);                                               //Turns on power to the soil sensor8
    for(int i = 0; i < 5; i++){                                                 //Takes 5 soil readings
        soil8Readings[i] = soil8Read();                                         //Loads a soil reading into each index of the array
    }
    digitalWrite(powerPin, LOW);                                                //Turns off power to soil sensor to save power
}

void getTempRead(int powerPin){                                                 //Populates temp readings array with values
    digitalWrite(powerPin, HIGH);                                               //Turns on power to the thermistor
    for(int i = 0; i < 5; i++){                                                 //Takes 5 temp readings
        tempReadings[i] = tempRead();                                           //Loads a temp reading into each index of the array
    }
    digitalWrite(powerPin, LOW);                                                //Turns off power to depth sensor to save power
}


// Get values for all sensors //
void getValues(){                                                               //Collects the values to be sent to Ubidots

    getSoil4Read(D5);                                                           //Populates soil readings array with values
    selectSort(soil4Readings,5);                                                //Sorts the array of values
    soil4 = soil4Readings[2];                                                   //Selects the median value as the one to be sent

    getSoil8Read(D3);                                                           //Populates soil readings array with values
    selectSort(soil8Readings,5);                                                //Sorts the array of values
    soil8 = soil8Readings[2];                                                   //Selects the median value as the one to be sent
    
    getTempRead(D0);                                                            //Populates temp readings array with values
    selectSort(tempReadings,5);                                                 //Sorts the array of values
    temp = tempReadings[2];                                                     //Selects the median value as the one to be sent
    
    temp = 10000 / (4095/temp - 1);                                             // Turn analog temperature into Farenheit
    steinhart = temp / 10000;                                                   // (R/Ro)
    steinhart = log(steinhart);                                                 // ln(R/Ro)
    steinhart /= 3950;                                                          // 1/B * ln(R/Ro)
    steinhart += 1.0 / (25 + 273.15);                                           // + (1/To)
    steinhart = 1.0 / steinhart;                                                // Invert
    steinhart -= 273.15;  
    temp = steinhart*9/5+32;                                                    // Final temp in Farenheit

    bat = fuel.getSoC();                                                        //Gets the current battery percentage

}

// void printValues(){                                                             //Prints values to the serial monitor if needed
//    Serial.println(soil4);
//    Serial.print("8 in Soil Reading:");
//    Serial.println(soil8);
//    Serial.print("Battery percentage:");
//    Serial.println(bat);
//    Serial.println("%");
//    Serial.print("Network Strength:");
//    Serial.print(networkStrength);
//    Serial.println("dBm");
//    Serial.print("Fahrenheit: ");
//    Serial.println(temperature);
//}


//setupruns only when it turns on and wakes from deep sleep

void setup() {
    Serial.begin(9600);                                                         //Sets up serial communication
    pinMode(D1, OUTPUT);                                                        //Sets pin mode for trigger sensor
    pinMode(D0, OUTPUT);                                                        //Sets pin mode for power pin for soil sensor4
    pinMode(D5, OUTPUT);                                                        //Sets pin mode for power pin for soil sensor8
    pinMode(D3, OUTPUT);                                                        //Thermistor power pin
}

// Here is the actual code loop that runs //
void loop() {


    if (count==1){                                                              //set time on start-up
        Particle.connect();
            while(!Particle.connected()){
                delay(1000);
            }
        Time.zone(-7);                                                          // Set timezone (7 hours from UTC for Vancouver)
    }
    
    getSoilTRead(D1);                                                           // Check trigger value 
    minuteNow=Time.minute();
    int i = 0;
    
    if (minuteNow<6){
        if (soilT < 2500){
           for (int i = 1; i < 13; i++){
                getValues();
                delay(1000);
                if (soil4 < 1000 && soil8 < 1000){
                    break;
                }
                delay(1000);
                if (waitFor(Particle.connected, 60000)) // wait max. 60sec
                    Particle.publish("Vancouver", "{\"temp\":\"" + String(temp) +"\", \"soil1\":\"" + String(soilT) + "\", \"soil4\":\"" + String(soil4) + "\", \"soil8\":\"" + String(soil8) + "\", \"iter\":\"" + String(i) + "\", \"bat\":\"" + String(bat) +"\"}", 60, PRIVATE);
                else
                    Serial.println("No connection, need to find other ways");
                delay(1000);
                System.sleep(D2,RISING,1198,SLEEP_NETWORK_STANDBY);
            }
        }
        if (Time.hour()%6==0){
            getValues();
            delay(1000);
            int i = 0;
            if (waitFor(Particle.connected, 60000)) // wait max. 60sec
                Particle.publish("Vancouver", "{\"temp\":\"" + String(temp) +"\", \"soil1\":\"" + String(soilT) + "\", \"soil4\":\"" + String(soil4) + "\", \"soil8\":\"" + String(soil8) + "\", \"iter\":\"" + String(i) + "\", \"bat\":\"" + String(bat) +"\"}", 60, PRIVATE);
            else
                Serial.println("No connection, need to find other ways");
            delay(1000);
        }
        System.sleep(SLEEP_MODE_SOFTPOWEROFF, ((60-Time.minute())*60));             // If no put all other modules to sleep until an hour even
    }
    else {
        System.sleep(SLEEP_MODE_SOFTPOWEROFF, ((60-Time.minute())*60));             // If no put all other modules to sleep until an hour even
    }
    
    delay(1000);
    count=2;
}
