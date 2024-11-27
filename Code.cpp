#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo servo;

#define sensorPin1 A2
#define sensorPin2 A3
#define buzzerPin 6

int senVal1 = 0;
int senVal2 = 0;

#define RST_PIN 8
#define SS_PIN 10

int card1Balance = 500;
int card2Balance = 500;

#define num 7
char Data[num];
byte data_count = 0;

String num1, num2, card, card2;
char Key;

bool recharge = true;
bool register_mode = false;

MFRC522 mfrc522(SS_PIN, RST_PIN);

int state = 0;

const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {7, 6, 5, 4};
byte colPins[COLS] = {3, 2, A0, A1};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  servo.attach(9);
  servo.write(90);

  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(buzzerPin, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.setCursor(0, 0);
  lcd.print(" Automatic toll");
  lcd.setCursor(0, 1);
  lcd.print("collection system");
  delay(3000);
  lcd.clear();
}

bool vehicle = false;
bool paidBill = false;

void loop() {
  if (recharge == 0 || register_mode) {
    reCharge();
  } else {
    lcd.setCursor(0, 0);
    lcd.print("   Welcome!!!");
    sensorRead();
    rfid();
    keypad(); 
    if (senVal1 == 0 && !vehicle) {
      servoDown();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vehicle detected");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Put your card to");
      lcd.setCursor(0, 1);
      lcd.print("the reader......");
      delay(1000);
      lcd.clear();
      vehicle = true;
      paidBill = false;
    } else if (senVal2 == 0 && state == 1 && paidBill) {
      servoUp();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Have a safe");
      lcd.setCursor(0, 1);
      lcd.print("journey");
      delay(1000);
      lcd.clear();
      state = 0;
      vehicle = false;
      paidBill = true;
    }
  }
}

// ... (rest of the code) ...

void keypad() { 
  char customKey = customKeypad.getKey();

  if (customKey) {
    if (customKey == 'A') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Recharging Mode.");
      lcd.setCursor(0, 1);
      lcd.print("................");
      delay(1500);
      lcd.clear();
      recharge = 0;
      register_mode = false;
      clearData();
    } else if (customKey == 'B') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter the amount");
      lcd.setCursor(0, 1);
      lcd.print("................");
      delay(1500);
      lcd.clear();
      recharge = 0;
      register_mode = true;
      clearData();
    }
  }
}

void servoDown() {
  servo.attach(9);
  servo.write(42);
  delay(1000);
  servo.write(90);
  delay(1000);
}

void servoUp() {
  servo.attach(9);
  servo.write(140);
  delay(1000);
  servo.write(90);
  delay(1000);
}

void sensorRead() {
  senVal1 = digitalRead(sensorPin1);
  senVal2 = digitalRead(sensorPin2);
}

void rfid() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  if (content.substring(1) == "06 F3 66 AF") {
    if (card1Balance >= 500 && !paidBill) {
      lcdPrint();
      card1Balance = card1Balance - 500;
      lcd.setCursor(9, 1);
      lcd.print(card1Balance);
      delay(1000);
      lcd.clear();
      state = 1;
      paidBill = true;
    } else if (paidBill) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bill already paid");
      lcd.setCursor(0, 1);
      lcd.print("for this vehicle");
      delay(2000);
      lcd.clear();
    } else {
      card = content.substring(1);
      LcdPrint();
      lcd.setCursor(9, 1);
      lcd.print(card1Balance);
      lcd.print(" Tk");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Press A to Recharge");
      lcd.setCursor(0, 1);
      lcd.print("Press B to Register");
      delay(2000);
      lcd.clear();
      state = 0;
    }
  } else if (content.substring(1) == "F4 C2 37 BB") {
    if (card2Balance >= 500 && !paidBill) {
      lcdPrint();
      card2Balance = card2Balance - 500;
      lcd.setCursor(9, 1);
      lcd.print(card2Balance);
      delay(1000);
      lcd.clear();
      state = 1;
    } else if (paidBill) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bill already paid");
      lcd.setCursor(0, 1);
      lcd.print("for this vehicle");
      delay(2000);
      lcd.clear();
    } else {
      card = content.substring(1);
      LcdPrint();
      lcd.setCursor(9, 1);
      lcd.print(card2Balance);
      lcd.print(" Tk");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Press A to Recharge");
      lcd.setCursor(0, 1);
      lcd.print("Press B to Register");
      delay(2000);
      lcd.clear();
      state = 0;
    }
  } else {
    digitalWrite(buzzerPin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Unknown Vehicle");
    lcd.setCursor(0, 1);
    lcd.print("Access denied");
    delay(1500);
    lcd.clear();
    digitalWrite(buzzerPin, LOW);
  }
}



void clearData() {
 for (byte i = 0; i < num; i++) {
   Data[i] = 0;
 }
 data_count = 0;
}

void reCharge() {
 lcd.setCursor(0, 0);
 lcd.print("Enter the amount");

 char customKey = customKeypad.getKey();

 if (customKey) {
   if (customKey == 'D') {
     if (card == "06 F3 66 AF") {
       num1 = Data;
       int amount = num1.toInt();
       if (amount + card1Balance >= 500) {
         card1Balance = amount + card1Balance;
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("Your current");
         lcd.setCursor(0, 1);
         lcd.print("balance: ");
         lcd.setCursor(9, 1);
         lcd.print(card1Balance);
         lcd.print(" Tk");
         delay(3000);
         clearData();
         lcd.clear();
         recharge = 1;
         register_mode = false;
       } else {
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("Insufficient Amount");
         lcd.setCursor(0, 1);
         lcd.print("Try Again");
         delay(2000);
         lcd.clear();
         clearData();
       }
     } else if (card == "F4 C2 37 BB") {
       num2 = Data;
       int amount = num2.toInt();
       if (amount + card2Balance >= 500) {
         card2Balance = amount + card2Balance;
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("Your current");
         lcd.setCursor(0, 1);
         lcd.print("balance: ");
         lcd.setCursor(9, 1);
         lcd.print(card2Balance);
         lcd.print(" Tk");
         delay(3000);
         clearData();
         lcd.clear();
         recharge = 1;
         register_mode = false;
       } else {
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("Insufficient Amount");
         lcd.setCursor(0, 1);
         lcd.print("Try Again");
         delay(2000);
         lcd.clear();
         clearData();
       }
     }
   } else {
     if (data_count < num) {
       Data[data_count] = customKey;
       lcd.setCursor(data_count, 1);
       lcd.print(Data[data_count]);
       data_count++;
     }
   }
 }
}

void lcdPrint() {
 digitalWrite(buzzerPin, HIGH);
 delay(200);
 digitalWrite(buzzerPin, LOW);
 delay(100);
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("  Successfully");
 lcd.setCursor(0, 1);
 lcd.print(" paid your bill");
 delay(1500);
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Your Remaining");
 lcd.setCursor(0, 1);
 lcd.print("balance: ");
}
void LcdPrint() {
 digitalWrite(buzzerPin, HIGH);
 delay(200);
 digitalWrite(buzzerPin, LOW);
 delay(100);
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("  Your balance");
 lcd.setCursor(0, 1);
 lcd.print(" is insufficient");
 delay(1500);
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Your Remaining");
 lcd.setCursor(0, 1);
 lcd.print("balance: ");
}
