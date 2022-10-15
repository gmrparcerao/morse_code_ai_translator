/*---------------Header----------------------------
    SENAI Technology College "Mariano Ferraz"
    Sao Paulo, 27/08/2022
    Postgraduate - Internet of Things

    Names of postgraduate students:
      Claudinei, Guilherme, Renan
    Lecturer:
      André and Daniel

    Goals: 
      Software to train an AI to undestand morse code

    Hardware: 

    Libraries:

    Reviews: 
      R000 - begin
      R001 - send operationMode for 1 after train the network

    http://arduino.esp8266.com/stable/package_esp8266com_index.json
	  https://dl.espressif.com/dl/package_esp32_index.json

  */

//---------------Libraries-------------------------
  #include <Arduino.h>
  #include <String.h>

//---------------Global Variables------------------
const int sampleSize = 180;
const int sampleFrequency = 25; // milisseconds
char data[sampleSize] = {'0'};
String stringData;
char operationMode = '0';

int counter = 0;

long initialTime;
long actualTime;


// Pins
#define BUTTONPIN       15      // Button connected at pin 15
#define BUZZERPIN       23      // Buzzer connected at pin 23
#define LEDRECORDINGPIN 22      // Led that inform a recording in progress

//---------------Auxiliary Functions---------------
  void recordData(){
    actualTime = millis();

    // even 'sampleFrequency' seconds, create a new record, until sampleSize data
    if (actualTime > (initialTime + sampleFrequency)){
      initialTime = actualTime;

      if (counter < sampleSize){
        data[counter] = digitalRead(BUTTONPIN);
        counter ++;
      }
      else {
        // when sampleSize is over, reset the counter and enter in waiting mode
        counter = 0;
        operationMode = '4';
      }
      

    }  

  }

  void playData(){
    actualTime = millis();

    // even 'sampleFrequency' seconds, paly the data, until sampleSize data
    if (actualTime > (initialTime + sampleFrequency)){
      initialTime = actualTime;

      if (counter < sampleSize){
        digitalWrite(BUZZERPIN, data[counter]);
        counter ++;
      }
      else {
        // when sampleSize is over, reset the counter and enter in waiting mode
        counter = 0;
        operationMode = '0';
      }
      

    }  

  }

  void sendData(){
    stringData.clear();
    for (int i = 0; i < sampleSize; i++){      
      if (data[i] == HIGH){
        stringData.concat('1');
      }
      else{
        stringData.concat('0');
      }
      stringData.concat(";");
    }
    Serial.println(stringData);
    delay(100);
    operationMode = '1';
  }
 
 

//---------------Setup function--------------------
	void setup(){
    Serial.begin(9600);

    // pins mode
    pinMode(BUTTONPIN, INPUT_PULLDOWN);
    pinMode(BUZZERPIN, OUTPUT);
    pinMode(LEDRECORDINGPIN, OUTPUT);

	}

//---------------Main Function---------------------
	void loop(){

    if (Serial.available()){
      operationMode = Serial.read();
      // Serial.println(operationMode);
    }

    switch (operationMode)
    {
    case '0': // Waiting mode
      digitalWrite(LEDRECORDINGPIN, LOW);
      digitalWrite(BUZZERPIN, digitalRead(BUTTONPIN));
      break;
    
    case '1': // Waiting to record
      digitalWrite(LEDRECORDINGPIN, LOW);
      if (digitalRead(BUTTONPIN)){        
        operationMode = '2';
        
      }
      break;

    case '2': // Recording data
      recordData();
      digitalWrite(LEDRECORDINGPIN, HIGH);
      digitalWrite(BUZZERPIN, digitalRead(BUTTONPIN));      
      break;

    case '3': // Play last data
      playData();
      break;

    case '4': // Send data
      digitalWrite(LEDRECORDINGPIN, LOW);
      sendData(); 
      break;

    }

	}