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

    http://arduino.esp8266.com/stable/package_esp8266com_index.json
    https://dl.espressif.com/dl/package_esp32_index.json

  */

//---------------Libraries-------------------------
// morse_model.h contains the array you exported from Python with xxd or tinymlgen  
  #include <EloquentTinyML.h>
  #include <eloquent_tinyml/tensorflow.h>
// morse_model.h contains the array you exported from Python with xxd or tinymlgen
  #include "morse_model.h"
  #include <Arduino.h>
  #include <String.h>
  #include <Adafruit_SSD1306.h>
  #include <Adafruit_GFX.h>
  #include <Wire.h>

//---------------Global Variables------------------

const int sampleSize = 180;
const int sampleFrequency = 25; // milisseconds
char data[sampleSize] = {'0'};
String stringData;
char operationMode = '0';
char vogal = ' ';
float letter[sampleSize] = {0};

int counter = 0;

long initialTime;
long actualTime;

// Pins
#define BUTTONPIN       15      // Button connected at pin 15
#define BUZZERPIN       23      // Buzzer connected at pin 23
#define LEDRECORDINGPIN 18      // Led that inform a recording in progress

//Tensorflow - Lite
#define N_INPUTS 180
#define N_OUTPUTS 5

// in future projects you may need to tweak this value: it's a trial and error process
#define TENSOR_ARENA_SIZE 14*1024  

Eloquent::TinyML::TensorFlow::TensorFlow<N_INPUTS, N_OUTPUTS, TENSOR_ARENA_SIZE> tf;

// Display
#define SCREEN_WIDTH 128 // largura da tela OLED, em pixels
#define SCREEN_HEIGHT 64 // Altura da tela OLED, em pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


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
        operationMode = '5';
      }
      
   //************* operationMode = '0'; // alt 1
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
    operationMode = '0';
  }

 void predict_letter(){
  for (int i = 0; i < sampleSize; i++){
    if (data[i] == LOW){
      letter[i] = 0; 
    }
    else {
      letter[i] = 1;
    }
  }
  //unsigned char predicted[5];
  tf.predictClass(letter); 
  if (tf.getScoreAt(0) < 0.5) {
    Serial.println("A");
    vogal = 'A';
  }

  else if (tf.getScoreAt(0) < 1.5) {
    Serial.println("E");
    vogal = 'E';
  }
  
  else if (tf.getScoreAt(0) < 2.5) {
    Serial.println("I");
    vogal = 'I';
    }

  else if (tf.getScoreAt(0) < 3.5) {
    Serial.println("O");
    vogal = 'O';
  }

  else {
    Serial.println("U");
    vogal = 'U';
  }         
  
  //Serial.println(tf.getScoreAt(0));
 
  operationMode = '1';
  digitalWrite(LEDRECORDINGPIN, LOW);
 }
 

//---------------Setup function--------------------
  void setup(){
    Serial.begin(115200); // para 115200 adicionar 'monitor_speed = 115200' no platformio.ini

    // pins mode
    pinMode(BUTTONPIN, INPUT);
    pinMode(BUZZERPIN, OUTPUT);
    pinMode(LEDRECORDINGPIN, OUTPUT);
    //Serial.println("Before delay");
    delay(4000);
//    Serial.println("Before tf ok");
    tf.begin(morse_model_tflite);
    //Serial.println("Before tf ok");

  // check if model loaded fine
  if (!tf.isOk()) {
      Serial.print("ERROR: ");
      Serial.println(tf.getErrorMessage());

      while (true) delay(1000);
    }
     operationMode = '1';
    // Serial.println("After tf ok");
  
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("Falha na alocação SSD1306"));
      for(;;);
    }
    delay(2000);
    display.clearDisplay();
    display.setTextColor(WHITE);
  }

//---------------Main Function---------------------
  void loop(){

    // Serial.println("IS LOOP");

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
      sendData();
      break;

    case '5': // Send data
      recordData();
      digitalWrite(LEDRECORDINGPIN, HIGH);
      digitalWrite(BUZZERPIN, digitalRead(BUTTONPIN));
      predict_letter();
      //Serial.println(data);
      //operationMode = '1';

     // Displaying the result on LCD
      display.clearDisplay();
      display.setCursor(45, 4);
      display.setTextSize(7);
      display.print(vogal);
      break;

    }

    display.display();
  }
