#include <SoftwareSerial.h>

//gsm module
SoftwareSerial GPS(50, 51);

//fingerprint sensor
#include <Adafruit_Fingerprint.h>
#define RXPIN 11
#define TXPIN 12
SoftwareSerial mySerial(RXPIN, TXPIN);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

//Cele doua librarii sunt folosite pentru LCD si modulul I2C
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//POSITIVE poate fi schimbat cu NEGAtIVE
//LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//Servo
#include <Servo.h>
Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position

/*4x4 Matrix Keypad connected to Arduino
This code prints the key pressed on the keypad to the serial port*/
#include <Keypad.h>
#define Password_Lenght 7 // Give enough room for six chars + NULL char
char Data[Password_Lenght]; // 6 is the number of chars it can hold + the null char = 7
char Master[Password_Lenght] = "123456"; 
char Master2[Password_Lenght] = "678900"; 
char Master3[Password_Lenght] = "135790"; 
char Master4[Password_Lenght] = "246800"; 
char Master5[Password_Lenght] = "142530"; 

byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;

const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad
//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols]=
{
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};
//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {22,23,24,25}; //Rows 0 to 3
byte colPins[numCols]= {26,27,28,29}; //Columns 0 to 3
//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

int smokeA0 = A0;
// Your threshold value. You might need to change it.
int sensorThres = 300;

int buzzerPin = 3; //Define buzzerPin




void setup()  
{
  lcd.begin();

  pinMode(smokeA0, INPUT);

  pinMode(buzzerPin, OUTPUT); //Set buzzerPin as output
  digitalWrite(buzzerPin, HIGH);




  GPS.begin(9600);
  Serial.begin(9600);
  delay(100);
  GPS.println("AT+CMGF=1");



   Serial.println("\n\nAdafruit finger detect test");
  // set the data rate for the sensor serial port
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");

  arataPeEcran();

}

void loop()                     // run over and over again
{

  int analogSensor = analogRead(smokeA0);

  if (analogSensor > sensorThres)
  {
     activeazaAlarma();
  }
  
  mySerial.listen();
  getFingerprintIDez();
  
//  int incercare = getFingerprintIDez();
//  if(incercare == 1){
//     //what to do when the id is 1
//    }
//    delay(1000);
//  }

  customKey = myKeypad.getKey();
  if (customKey) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
  {

    Data[data_count] = customKey; // store char into data array
    lcd.setCursor(data_count,1); // move cursor to show each new char
    lcd.print(Data[data_count]); // print char at said cursor
    data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
  }

  if(data_count == Password_Lenght-1) // if the array index is equal to the number of expected chars, compare data to master
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Parola este");


    if(!strcmp(Data, Master) || !strcmp(Data, Master2) || !strcmp(Data, Master3) || !strcmp(Data, Master4) || !strcmp(Data, Master5)) // equal to (strcmp(Data, Master) == 0)
      {
        lcd.setCursor(0, 1);
        lcd.print("Buna");
        pornesteBariera();
        
        }
        else{
        lcd.setCursor(0, 1);
      lcd.print("Gresita");
        }
    delay(1000);// added 1 second delay to make sure the password is completely shown on screen before it gets cleared.

  arataPeEcran();
  clearData();   



    delay(50);            //don't ned to run this at full speed.

  } 
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  if(finger.fingerID == 1){
    Serial.println("A venit sefu'");
  }else if(finger.fingerID == 2){
    Serial.println("A venit administratorul");
  }else if(finger.fingerID == 3){
    Serial.println("A venit paznicul");
  }else if(finger.fingerID == 4){
    Serial.println("A venit angajatul 3");
  }

  verificaId(finger.fingerID);
  return finger.fingerID; 
}

void verificaId(int id){


  
  if(id == 1){
        arataPeLCD("sefu");
        trimiteSMS("sefu");
  }else if(id == 2){       
        arataPeLCD("dl. admin.");
        trimiteSMS("administratorul");
  }else if(id == 3){
        arataPeLCD("dl. paznic");
        trimiteSMS("paznicul");
  }else if(id == 4){
        arataPeLCD("dl. angajat #4");
        trimiteSMS("angajatul #4");
  }
  

  Serial.print(" Mesajul a fost trimis cu faptul ca a ajuns angajatul cu id-ul: ");
  Serial.println(id);
  
}

void trimiteSMS(String angajatul){

      GPS.listen();
      delay(50);
      GPS.println("AT+CMGS=\"+40736094704\""); 
      delay(500); 
      GPS.print("A venit ");
      GPS.print(angajatul);
      GPS.write( 0x1a ); // ctrl+Z character
      delay(500);

      
}

void arataPeLCD(String angajatul){


    lcd.clear();
    lcd.print("Bine ati venit,");
    lcd.setCursor(0, 1); 
    lcd.print(angajatul);

    pornesteBariera();

  
}

void pornesteBariera(){
  for (pos = 90; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
          myservo.attach(45);// attaches the servo on pin 47 to the servo object
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(30);                       // waits 15ms for the servo to reach the position
  }
  delay(5000);
  for (pos = 180; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
     lcd.clear();
      lcd.print("Cade bariera, ");
    lcd.setCursor(0,1);
    lcd.print("nu intrati!");
    delay(100);
      }
  myservo.detach();
      
  arataPeEcran();
}

void clearData()
{
  while(data_count !=0)
  {   // This can be used for any array size, 
    Data[data_count--] = 0; //clear array for new data
  }
  return;
}

void arataPeEcran(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Parola:");
}

void activeazaAlarma(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ALARMA");
  lcd.setCursor(0,1);
  lcd.print("ACTIVATA");

  beep();


  GPS.listen();
  GPS.println("AT+CMGS=\"+40736094704\""); 
  delay(500); 
  GPS.print("ALARMA ACTIVATA!!!");
//  GPS.print(angajatul);
  GPS.write( 0x1a ); // ctrl+Z character
  delay(500);

  delay(5000);

  arataPeEcran();
  
}

void beep() { //creating function
  digitalWrite(buzzerPin, HIGH);
  delay(3000);
  digitalWrite(buzzerPin, LOW);
  delay(3000);
  
}
