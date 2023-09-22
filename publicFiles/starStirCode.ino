/*
This code uses the PWM capabilities of a teensy to change the speed of stir plate units constructed from 12V motors.
Speed settings are saved on internal SD card in case of power interruption, and activates stirplates immediately after
power is restored unless stopped by user.

Code created 22 September 2023
By Nash Soderberg 
*/
//LIBRARIES
  #include <Wire.h>// I2C library
  #include <Adafruit_GFX.h>// OLED library from Adafruit
  #include <Adafruit_SSD1306.h>// OLED library from Adafruit
  #include <IRremote.h>// IR library
  #include "SdFat.h"//SD cart library
  #include <SPI.h>// Serial library
  
//DEFINITIONS
  //OLED
    #define OLED_RESET 8// OLED reset
    Adafruit_SSD1306 display(OLED_RESET);
    int oledPowerPin = 17;// Pin that powers the OLED
  //IR
    int irPowerPin = 20;// Pin that powers the IR sensor
    IRrecv irrecv(21);// Creates an IR receiving object irrecv and sets the pin that receives the IR signal
    decode_results results;// Creates a variable of type decode_results
    #define LEFTIR 16584943// IR signal read by sensor that translates to LEFTIR
    #define RIGHTIR 16601263// IR signal read by sensor that translates to RIGHTIR
    #define UPIR 16621663// IR signal read by sensor that translates to UPIR
    #define DOWNIR 16625743// IR signal read by sensor that translates to DOWNIR
    #define ENTERIR 16617583// IR signal read by sensor that translates to ENTERIR
  //IO
    int buttonPin = 22;// Pin attached to front button
    int motor1Pin = 14;// Pin to signal FET to drive fan motor 1
    int motor2Pin = 38;// Pin to signal FET to drive fan motor 2
    int motor3Pin = 37;// Pin to signal FET to drive fan motor 3
    int motor4Pin = 36;// Pin to signal FET to drive fan motor 4
    int motor5Pin = 35;// Pin to signal FET to drive fan motor 5
    int motor6Pin = 2;// Pin to signal FET to drive fan motor 6
    int motor7Pin = 3;// Pin to signal FET to drive fan motor 7
    int motor8Pin = 4;// Pin to signal FET to drive fan motor 8
  //Menu
    uint8_t menu = 9;// An index value to establish which navigation menu is currently selected
    uint8_t pos = 0;// An index value to establish which navigation position (within a menu) is currently selected
  //SD card
    SdFatSdio sd;
    File sampleParam;// Name of sample parameter file saved on SD card
    #define SAMPLE_PARAM_ROW 1// Number of rows in sampleParam.txt file
    #define SAMPLE_PARAM_COL 8// Number of columns in sampleParam.txt file
    int sampleParamArray[SAMPLE_PARAM_ROW][SAMPLE_PARAM_COL];// Create an sampleParamArray with SAMPLE_PARAM_ROW rows and SAMPLE_PARAM_COL columns   
  //Fan Motor Parameters
    int motor1Volt = 1.0; int motor2Volt = 1.0; int motor3Volt = 1.0; int motor4Volt = 1.0; int motor5Volt = 1.0; int motor6Volt = 1.0; int motor7Volt = 1.0; int motor8Volt = 1.0;// what voltage the fan motors receive
    int motorVoltChange = 5; //the amount voltage increments on the GUI

 
//<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
//<----------------------------------------------------------------------SETUP----------------------------------------------------------------------------------->
//<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
void setup(){// Run setup code on initial power up
  //Communication setup     
    Wire.begin();// NOT SURE I NEED THIS
    Serial.begin(9600);// Initiate the serial connection, used for debugging
    delay (500);// Pause for a half second
  //OLED Setup
    pinMode(oledPowerPin, OUTPUT);// Set the oledPowerPin as output, to power the OLED
    digitalWrite(oledPowerPin, HIGH);// Set the oledPowerPin to high to turn on the OLED
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);// Begin and initialize the OLED display
    display.clearDisplay();// Clear the OLED display
  //Infrared Setup
    pinMode(irPowerPin, OUTPUT);// Set the irPowerPin as output, to power the IR sensor
    digitalWrite(irPowerPin, HIGH);// Set the irPowerPin to high to turn on the IR sensor   
    irrecv.enableIRIn();// Starts the IR receiving object irrecv
  //IO Setup
    pinMode(buttonPin, INPUT);// Set buttonPin as input
    
    pinMode(motor1Pin, OUTPUT);// Set the motor1Pin as output
    pinMode(motor2Pin, OUTPUT);// Set the motor2Pin as output
    pinMode(motor3Pin, OUTPUT);// Set the motor3Pin as output
    pinMode(motor4Pin, OUTPUT);// Set the motor4Pin as output
    pinMode(motor5Pin, OUTPUT);// Set the motor5Pin as output
    pinMode(motor6Pin, OUTPUT);// Set the motor6Pin as output
    pinMode(motor7Pin, OUTPUT);// Set the motor7Pin as output
    pinMode(motor8Pin, OUTPUT);// Set the motor8Pin as output
  //SD Setup
    if (!sd.begin()) { display.setTextColor(WHITE,BLACK);  display.setTextSize(1);  display.setCursor(2,8);  display.println("MicroSD not");display.setCursor(5,16);  display.println("detected!");display.display();sd.initErrorHalt("SdFatSdio begin() failed");}// If the SD card fails to communicate, display that the card is not detected on the OLED
    readSampleParamArray();// Call the function that reads the sample parameters off of the SD card
}

//<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
//<------------------------------------------------------------------MAIN LOOP----------------------------------------------------------------------------------->
//<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
void loop(){
  switch(menu){//Menu navigation loop to switch between active mode and the main menu
    case 0: mainMenuNavigation(); mainMenuDisplay(); break;//Loop the Main Menu navigation and display
    case 9: activeMode(); turnOffDisplay(); runMotors();//Loop active mode
  }
}

//<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
//<---------------------------------------------------------------MENU NAVIGATION-------------------------------------------------------------------------------->
//<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
void mainMenuNavigation(){//selects position, changes motor voltage, or enters the program into active mode based off of inputs from IR remote at any given position  
  if(irrecv.decode(&results)){// If there is an IR signal, return true and store signal in "results"
    switch(pos){// What is the selected position within this menu?
     case 0:// Position is 0
      switch (results.value){// What was the signal received from the IR remote?
        case DOWNIR: pos = 1; break;
      }break;
     case 1:// Position is 1
      switch (results.value){// What was the signal received from the IR remote?
          case LEFTIR: pos = 0; break; case RIGHTIR: pos = 11; break; case UPIR: pos = 0; break; case DOWNIR: pos = 2; break;// Navigate   
        }break;
     case 11:// Position is 11
      switch (results.value){// What was the signal received from the IR remote?
        case LEFTIR: pos = 1; break; case RIGHTIR: pos = 12; break; case UPIR: motor1Volt=motor1Volt+motorVoltChange; break; case DOWNIR: motor1Volt=motor1Volt-motorVoltChange; break;// Navigate or change voltage of motor 1 
        }break;
     case 12:// Position is 12
      switch (results.value){// What was the signal received from the IR remote?
        case LEFTIR: pos = 11; break; case RIGHTIR: pos = 13; break; case UPIR: motor2Volt=motor2Volt+motorVoltChange; break; case DOWNIR: motor2Volt=motor2Volt-motorVoltChange; break;// Navigate or change voltage of motor 2 
        }break;
     case 13:// Position is 13
      switch (results.value){// What was the signal received from the IR remote?
        case LEFTIR: pos = 12; break; case RIGHTIR: pos = 14; break; case UPIR: motor3Volt=motor3Volt+motorVoltChange; break; case DOWNIR: motor3Volt=motor3Volt-motorVoltChange; break;// Navigate or change voltage of motor 3 
        }break;
     case 14:// Position is 14
      switch (results.value){// What was the signal received from the IR remote?
        case LEFTIR: pos = 13; break; case RIGHTIR: pos = 2; break; case UPIR: motor4Volt=motor4Volt+motorVoltChange; break; case DOWNIR: motor4Volt=motor4Volt-motorVoltChange; break;// Navigate or change voltage of motor 4
        }break;
      case 2:// Position is 2
        switch (results.value){// What was the signal received from the IR remote?
          case LEFTIR: pos = 15; break; case RIGHTIR: pos = 21; break; case UPIR: pos= 1; break; case DOWNIR: pos = 3; break;// Navigate   
        }break;
      case 21:// Position is 21
        switch (results.value){// What was the signal received from the IR remote?
          case LEFTIR: pos = 2; break; case RIGHTIR: pos = 22; break; case UPIR: motor5Volt=motor5Volt+motorVoltChange; break; case DOWNIR: motor5Volt=motor5Volt-motorVoltChange; break;// Navigate or change voltage of motor 5  
        }break;
      case 22:// Position is 22
        switch (results.value){// What was the signal received from the IR remote?
          case LEFTIR: pos = 21; break; case RIGHTIR: pos = 23; break; case UPIR: motor6Volt=motor6Volt+motorVoltChange; break; case DOWNIR: motor6Volt=motor6Volt-motorVoltChange; break;// Navigate or change voltage of motor 6  
        }break;
      case 23:// Position is 23
        switch (results.value){// What was the signal received from the IR remote?
          case LEFTIR: pos = 22; break; case RIGHTIR: pos = 24; break; case UPIR: motor7Volt=motor7Volt+motorVoltChange; break; case DOWNIR: motor7Volt=motor7Volt-motorVoltChange; break;// Navigate or change voltage of motor 7 
        }break;
      case 24:// Position is 24
        switch (results.value){// What was the signal received from the IR remote?
          case LEFTIR: pos = 23; break; case RIGHTIR: pos = 3; break; case UPIR: motor8Volt=motor8Volt+motorVoltChange; break; case DOWNIR: motor8Volt=motor8Volt-motorVoltChange; break;// Navigate or change voltage of motor 8  
        }break;
      case 3:// Position is 3
        switch (results.value){// What was the signal received from the IR remote?
          case LEFTIR: pos = 21; break; case RIGHTIR: pos = 3; break; case UPIR: pos = 2; break; case DOWNIR: pos = 3; break; case ENTERIR: writeSampleParamArray(); menu = 9; break;// Navigate or write current parameters into sampleParam by initiating writeSampleParamArray function, and enter active mode
        }break;
    }
    irrecv.resume();// Continue receiving and looking for IR signals
    }
  }

//<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
//<--------------------------------------------------------------------MENU DISPLAY------------------------------------------------------------------------------>
//<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
void mainMenuDisplay(){// Text to display in the main menu
  display.clearDisplay();// Clear the display  
  display.setTextSize(2);// Set the text size to 2 
  if (pos==0){display.setTextColor(BLACK,WHITE);} else{display.setTextColor(WHITE,BLACK);}// If position 0 is selected, highlight text in that position
  display.setCursor(0,0); display.println("  <MAIN>  ");// Set cursor to first line and display "<MAIN>"
  display.setTextSize(1);// Set the text size to 1 
  if (pos==1){display.setTextColor(BLACK,WHITE);} else{display.setTextColor(WHITE,BLACK);} display.setCursor(0,16); display.println("1-4:"); //if position is 1, highlight text in this position
  if (pos==11){display.setTextColor(BLACK,WHITE);} else{display.setTextColor(WHITE,BLACK);} display.setCursor(24,16); display.println(motor1Volt); display.setTextColor(WHITE,BLACK); display.setCursor(42,16); display.println(",");//if position is 11, highlight text in this position. Displays motor1Volt as it changes
  if (pos==12){display.setTextColor(BLACK,WHITE);} else{display.setTextColor(WHITE,BLACK);} display.setCursor(48,16); display.println(motor2Volt); display.setTextColor(WHITE,BLACK); display.setCursor(66,16); display.println(",");//if position is 12, highlight text in this position. Displays motor2Volt as it changes
  if (pos==13){display.setTextColor(BLACK,WHITE);} else{display.setTextColor(WHITE,BLACK);} display.setCursor(72,16); display.println(motor3Volt); display.setTextColor(WHITE,BLACK); display.setCursor(90,16); display.println(",");//if position is 13, highlight text in this position. Displays motor3Volt as it changes
  if (pos==14){display.setTextColor(BLACK,WHITE);} else{display.setTextColor(WHITE,BLACK);} display.setCursor(96,16); display.println(motor4Volt); //if position is 14, highlight text in this position. Displays motor4Volt as it changes
  if (pos==2){display.setTextColor(BLACK,WHITE);} else{display.setTextColor(WHITE,BLACK);} display.setCursor(0,30); display.println("5-8:");//if position is 2, highlight text in this position
  if (pos==21){display.setTextColor(BLACK,WHITE);} else{display.setTextColor(WHITE,BLACK);} display.setCursor(24,30); display.println(motor5Volt); display.setTextColor(WHITE,BLACK); display.setCursor(42,30); display.println(",");//if position is 21, highlight text in this position. Displays motor5Volt as it changes
  if (pos==22){display.setTextColor(BLACK,WHITE);} else{display.setTextColor(WHITE,BLACK);} display.setCursor(48,30); display.println(motor6Volt); display.setTextColor(WHITE,BLACK); display.setCursor(66,30); display.println(",");//if position is 22, highlight text in this position. Displays motor6Volt as it changes
  if (pos==23){display.setTextColor(BLACK,WHITE);} else{display.setTextColor(WHITE,BLACK);} display.setCursor(72,30); display.println(motor7Volt); display.setTextColor(WHITE,BLACK); display.setCursor(90,30); display.println(",");//if position is 23, highlight text in this position. Displays motor7Volt as it changes
  if (pos==24){display.setTextColor(BLACK,WHITE);} else{display.setTextColor(WHITE,BLACK);} display.setCursor(96,30); display.println(motor8Volt); //if position is 24, highlight text in this position. Displays motor8Volt as it changes
  if (pos==3){display.setTextColor(BLACK,WHITE);} else{display.setTextColor(WHITE,BLACK);} display.setCursor(0,44); display.println("ENTER SET");//if position is 3, highlight text in this position
  numberCorrect();// Correct all displayed numbers if they go out of bounds
  display.display();// Update the display
}

//<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
//<-------------------------------------------------------------------SAMPLING MODE------------------------------------------------------------------------------>
//<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
void activeMode(){//active mode is on, motors are getting powered with user selected voltage
  if (digitalRead(buttonPin) == HIGH){menu = 0;}//if the button is pressed, exit active mode
}

void turnOffDisplay(){//turn off OLED display
  digitalWrite (oledPowerPin, LOW);
}

void runMotors(){
    analogWrite(motor1Pin, motor1Volt);//sends voltage in sampleParam file to motor 1
    analogWrite(motor2Pin, motor2Volt);//sends voltage in sampleParam file to motor 2
    analogWrite(motor3Pin, motor3Volt);//sends voltage in sampleParam file to motor 3
    analogWrite(motor4Pin, motor4Volt);//sends voltage in sampleParam file to motor 4 
    analogWrite(motor5Pin, motor5Volt);//sends voltage in sampleParam file to motor 5
    analogWrite(motor6Pin, motor6Volt);//sends voltage in sampleParam file to motor 6
    analogWrite(motor7Pin, motor7Volt);//sends voltage in sampleParam file to motor 7
    analogWrite(motor8Pin, motor8Volt);//sends voltage in sampleParam file to motor 8              
    if (digitalRead(buttonPin) == HIGH){analogWrite (motor1Pin, 0);analogWrite (motor2Pin, 0);analogWrite (motor3Pin, 0);analogWrite (motor4Pin, 0);analogWrite (motor5Pin, 0); analogWrite (motor6Pin, 0); analogWrite (motor7Pin, 0); analogWrite (motor8Pin, 0);//If the button is pressed, stop applying voltage to motors
       digitalWrite (oledPowerPin, HIGH); //power the OLED
       display.begin(SSD1306_SWITCHCAPVCC, 0x3C);// Begin and initialize the OLED display
       display.clearDisplay();//clear the display
       menu = 0;}//return to main menu 
  }

//<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
//<--------------------------------------------------------------------SD CARD CODE------------------------------------------------------------------------------>
//<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
void readSampleParamArray(){
  sampleParam = sd.open("SAMPLEPARAM.TXT", FILE_WRITE);// Open SAMPLEPARAM.txt file on sd card
  if (!sampleParam){Serial.println("open failed");}// If opening failed, write "open failed" to the serial port for debugging
  sampleParam.rewind();// Rewind the file for read.
  // Array for data.
  int i = 0;// Index for array rows
  int j = 0;// Index for array columns
  size_t n;// Length of returned field with delimiter.
  char str[20];// Longest field with delimiter and zero byte.
  char *ptr;// Test for valid field.
  // Read the file and store the data.
  for (i = 0; i < SAMPLE_PARAM_ROW; i++) {// Go through each row of the sampleParam.txt file
    for (j = 0; j < SAMPLE_PARAM_COL; j++) {// Within each row, go through each column of data
      n = readField(&sampleParam, str, sizeof(str), ",\n");// Read each character
      if (n == 0) {Serial.println("Too few lines");}
      sampleParamArray[i][j] = strtol(str, &ptr, 10);
      if (ptr == str) {Serial.println("bad number");}
      while (*ptr == ' ') {ptr++;}
      if (*ptr != ',' && *ptr != '\n' && *ptr != '\0') {Serial.println("extra characters in field");}
      if (j < (SAMPLE_PARAM_COL-1) && str[n-1] != ',') {Serial.println("line with too few fields");}
    }
    if (str[n-1] != '\n' && sampleParam.available()) {Serial.println("missing endl");}    
  }
  sampleParam.sync(); delay(500);
  //Read in data
  motor1Volt = sampleParamArray[0][0]; motor2Volt = sampleParamArray[0][1]; motor3Volt = sampleParamArray[0][2]; motor4Volt = sampleParamArray[0][3]; motor5Volt = sampleParamArray[0][4]; motor6Volt = sampleParamArray[0][5]; motor7Volt = sampleParamArray[0][6]; motor8Volt = sampleParamArray[0][7];    
}

size_t readField(File* sampleParam, char* str, size_t size, const char* delim) {
  char ch;
  size_t n = 0;
  while ((n + 1) < size && sampleParam->read(&ch, 1) == 1) {
    if (ch == '\r') {continue;}// Delete Carriage Return.
    str[n++] = ch;
    if (strchr(delim, ch)) {break;}
  }
  str[n] = '\0';
  return n;
}

void writeSampleParamArray(){
  sampleParam = sd.open("SAMPLEPARAM.TXT", FILE_WRITE);// Open the SAMPLEPARAM.TXT file on the SD card
  if (!sampleParam) {Serial.println("open failed");}// If opening the file failed, write a open failed to serial debugging
  //CREATE SAMPLE PARAMETER STRINGS (one per line)
  String sampleParamString0 = String(motor1Volt) +","+String(motor2Volt) +","+String(motor3Volt) +","+String(motor4Volt)+","+String(motor5Volt) +","+String(motor6Volt) +","+String(motor7Volt) +","+String(motor8Volt)+"\r\n";// Create string of the first line of sample parameters  
  
  //WRITE STRINGS TO SD CARD
  sampleParam.rewind(); delay(50);// Rewind to the beginning of the SAMPLEPARAM.txt file
  sampleParam.print(sampleParamString0); delay(50);// Overwrite the line of the SAMPLEPARAM.txt file
        
  sampleParam.sync(); delay(500);// Sync the SAMPLEPARAM.txt file

  display.clearDisplay(); display.setTextSize(2); display.setTextColor(WHITE,BLACK); display.setCursor(0,0); display.println(" <SAVED>");// Put "<SAVED>" on the first line
  display.display();// Update the display
  delay(1000);
}

void numberCorrect(){// constrain pump voltages from 1 to 255
  if (motor1Volt > 255){motor1Volt = 1;} if (motor1Volt < 1){motor1Volt = 255;} if (motor2Volt > 255){motor2Volt = 1;} if (motor2Volt < 1){motor2Volt = 255;} if (motor3Volt > 255){motor3Volt = 1;} if (motor3Volt < 1){motor3Volt = 255;} if (motor4Volt > 255){motor4Volt = 1;} if (motor4Volt < 1){motor4Volt = 255;} if (motor5Volt > 255){motor5Volt = 1;} if (motor5Volt < 1){motor5Volt = 255;} if (motor6Volt > 255){motor6Volt = 1;} if (motor6Volt < 1){motor6Volt = 255;} if (motor7Volt > 255){motor7Volt = 1;} if (motor7Volt < 1){motor7Volt = 255;} if (motor8Volt > 255){motor8Volt = 1;} if (motor8Volt < 1){motor8Volt = 255;}//if the user tries to increase voltage past 255, it will loop the number on the main menu back to 1. If the user tries to decrease voltage below 0, it will loop the number on the main menu up to 255
}
