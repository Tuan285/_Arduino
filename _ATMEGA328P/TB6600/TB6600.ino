#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define SS_PIN 10
#define RST_PIN 9

#define DIR_PIN 6
#define STEP_PIN 7
#define EN_PIN 5

#define BTUP 2
#define BTDOWN 3
#define BTCHOOSE 4

int position1, current_position, num_position, num_positions;
int posi_value;

struct pos {
  int pos1_2;
  int pos2_3;
  int pos3_4;
  int pos4_5;
  int pos5_6;
  int pos6_1;
};
pos position;

int timeDebouncing = 50;
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;
bool choose = 1;
typedef struct {
  bool currentSate;
  bool lastState;
  bool lastDebounceState;
  bool mode;
  unsigned long timeDebouce;
} BT;
BT BTUp, BTDown, BTChoose;
int numPositionTo, numPositionTo_s = 0;

String tag_UID1 = "9396DA6";
String tag_UID2 = "134B80A6";
String tag_UID3 = "D376D4A6";
String tag_UID4 = "F35A6AA6";
String tag_UID5 = "D3944AA6";
String tag_UID6 = "A38883A5";
String tag_UID7 = "43F91FA8";
String tagID = "";

MFRC522 mfrc522(SS_PIN, RST_PIN);
char *str;
int k = 1;
unsigned long t_prev;
bool check = 0;
unsigned int arr[7] = { 1, 1, 1, 1, 1, 1, 1 };      // mang chua cho trong
unsigned int arr_num[8] = { 0, 0, 0, 0, 0, 0, 0 };  // mang ktra the may lan
unsigned int arr_pos[7] = { 1, 1, 1, 1, 1, 1, 1 };  // mang luu vi tri
int the, otrong, xthe;

void initBT() {
  BTChoose.mode = 1;

  BTChoose.lastState = 0;
  BTDown.lastState = 0;
  BTUp.lastState = 0;

  BTChoose.lastDebounceState = 1;
  BTDown.lastDebounceState = 1;
  BTUp.lastDebounceState = 1;

  BTChoose.timeDebouce = 0;
  BTDown.timeDebouce = 0;
  BTUp.timeDebouce = 0;
}
void setup() {
  Serial.begin(9600);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
  pinMode(BTUP, INPUT);
  pinMode(BTDOWN, INPUT);
  pinMode(BTCHOOSE, INPUT);
  digitalWrite(EN_PIN, LOW);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  position1 = EEPROM.read(10);
  Serial.println(position1);
  runStep(position1, 1);
  EEPROM.write(10, 1);
  position1 = 1;
  num_position = 1;
  current_position = position1;
  initBT();
}

void loop() {
  CheckBT();
  if (BTChoose.mode == 0) {
    if (numPositionTo_s != numPositionTo) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Che do thu cong !");
      lcd.setCursor(0, 1);
      lcd.print("Vi Tri Den : ");
      lcd.print(numPositionTo);
      numPositionTo_s = numPositionTo;
    }
    if (choose == 0) {
      choose = 1;
      Serial.println("Thu cong Dang chay");
      runStep(current_position, numPositionTo);
      current_position = numPositionTo;
      EEPROM.write(10, current_position);
    }
  }
  if (BTChoose.mode == 1) {
    if (millis() - t_prev >= 3000) {
      //Serial.println("Che do tu dong !");
      while (readID()) {
        k = 1;
        while (k < 7) {
          delay(10);
          if (arr[k] == 0) {
            k++;
          } else {
            posi_value = k;
            // Serial.print(" Posi :");
            // Serial.print(posi_value);
            // Serial.println();
            break;
          }
        }
        if (tagID == tag_UID1) {
          the = 1;
          check_the(the);
        } else if (tagID == tag_UID2) {
          the = 2;
          check_the(the);
        } else if (tagID == tag_UID3) {
          the = 3;
          check_the(the);
        } else if (tagID == tag_UID4) {
          the = 4;
          check_the(the);
        } else if (tagID == tag_UID5) {
          the = 5;
          check_the(the);
        } else if (tagID == tag_UID6) {
          the = 6;
          check_the(the);
        } else if (tagID == tag_UID7) {
          the = 7;
          check_the(the);
        }
        if (check == 1) {
          if (xthe == 1) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Dang Tim O Trong");
          } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Dang Tim Xe");
          }
          Serial.println();
          Serial.print("Begin : ");
          Serial.print(current_position);
          Serial.println();
          Serial.print("End : ");
          Serial.println(num_positions);
          runStep(current_position, num_positions);
          Serial.println("Da chay xong Step !");

          current_position = num_positions;
          EEPROM.write(10, current_position);
          check = 0;
        }
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hay Quet The !");
      otrong = 0;
      for (int f = 1; f < 7; f++) {
        if (arr[f] == 0) {
          otrong++;
        }
        //Serial.print(arr[f]);
      }
      lcd.setCursor(0, 1);
      lcd.print("So O Trong: ");
      lcd.print(otrong);
      lcd.print("/6");

      t_prev = millis();
    }
  }
}
int check_the(int thesolan) {
  int incorrect = arr[1] + arr[2] + arr[3] + arr[4] + arr[5] + arr[6];
  arr_num[thesolan] = arr_num[thesolan] + 1;
  Serial.print("the ");
  Serial.print(thesolan);
  Serial.print(" : ");
  Serial.print(arr_num[thesolan]);
  Serial.println();
  if (incorrect == 0) {
    if (arr_num[thesolan] == 2) {         // the 1 lan 2
      num_positions = arr_pos[thesolan];  // lay vi tri trong bo nho tam
      arr_num[thesolan] = 0;              // reset the lai
      arr[num_positions] = 1;             // ko co xe o vi tri 1
      check = 1;
      xthe = 2;
    }
    arr_num[thesolan] = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Da Day Xe !!!");
    Serial.println("Xe day roi !!!");
  }
  if (incorrect != 0) {
    if (arr_num[thesolan] == 1) {        // the 1 lan 1
      if (arr[posi_value] == 1) {        // chua co xe
        arr[posi_value] = 0;             // co xe
        num_positions = posi_value;      //
        arr_pos[thesolan] = posi_value;  // luu vi tri xe vao bo nho tam
        check = 1;
        xthe = 1;
      }
    }
    if (arr_num[thesolan] == 2) {         // the 1 lan 2
      num_positions = arr_pos[thesolan];  // lay vi tri trong bo nho tam
      arr_num[thesolan] = 0;              // reset the lai
      arr[num_positions] = 1;             // ko co xe o vi tri 1
      check = 1;
      xthe = 2;
    }
  }
}
void runStep(int begin, int end) {
  int deg_value;
  bool dir;
  int start = begin;
  int stop = end;
  // Serial.print("Start : ");
  // Serial.print(start);
  // Serial.println();
  // Serial.print("Stop : ");
  // Serial.print(stop);
  // Serial.println();
  int num[7] = { 0, 0, 0, 0, 0, 0, 0 };
  if (start < stop) {  // chhieu thuan
    //Serial.println("NUM CW !");
    dir = 0;
    for (int i = start; i < stop; i++) {
      num[i] = 1;
    }
  }
  if (start > stop) {  // chieu nguoc
    dir = 1;
    //Serial.println("NUM CCW !");
    for (int f = start - 1; f >= stop; f--) {
      num[f] = 1;
    }
  }
  if (num[1] == 1) {
    position.pos1_2 = -25;
  }
  if (num[1] == 0) {
    position.pos1_2 = 0;
  }
  if (num[2] == 1) {
    position.pos2_3 = -18;
  }
  if (num[2] == 0) {
    position.pos2_3 = 0;
  }
  if (num[3] == 1) {
    position.pos3_4 = -18;
  }
  if (num[3] == 0) {
    position.pos3_4 = 0;
  }
  if (num[4] == 1) {
    position.pos4_5 = -25;
  }
  if (num[4] == 0) {
    position.pos4_5 = 0;
  }
  if (num[5] == 1) {
    position.pos5_6 = -22;
  }
  if (num[5] == 0) {
    position.pos5_6 = 0;
  }
  if (dir == 0) {
    deg_value = position.pos1_2 + position.pos2_3 + position.pos3_4 + position.pos4_5 + position.pos5_6;
  }
  if (dir == 1) {
    deg_value = -(position.pos1_2 + position.pos2_3 + position.pos3_4 + position.pos4_5 + position.pos5_6);
  }
  for (int l = 1; l < 7; l++) {
    Serial.print(num[l]);
  }
  Serial.println();
  Serial.print("Angle : ");
  Serial.print(deg_value);
  Serial.println();
  rotateDeg(deg_value, 0.05);
}
// unsigned long pressedTime  = 0;
// unsigned long releasedTime = 0;
// bool isPressing = false;
// bool isLongDetected = false;
void CheckBT() {
  long pressDur;
  BTChoose.currentSate = digitalRead(BTCHOOSE);
  if (BTChoose.lastState == HIGH && BTChoose.currentSate == LOW) {  // press
    pressedTime = millis();
  } else if (BTChoose.lastState == LOW && BTChoose.currentSate == HIGH) {  // release
    releasedTime = millis();

    pressDur = releasedTime - pressedTime;
    if (BTChoose.mode == 0) {
      if (pressDur < 3000) {
        choose = 0;
      }
    }
    if (pressDur >= 3000) {
      if (BTChoose.mode == 1) {
        BTChoose.mode = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Che do thu cong !");
        lcd.setCursor(0, 1);
        lcd.print("Vi Tri Den : ");
        lcd.print(numPositionTo);
      } else if (BTChoose.mode == 0) {
        BTChoose.mode = 1;
      }
      Serial.println("Che do :");
      Serial.println(BTChoose.mode);
      pressDur = 0;
    }
  }
  BTChoose.lastState = BTChoose.currentSate;

  BTDown.currentSate = digitalRead(BTDOWN);
  //Serial.print("Nut Nhan Down : ");
  //Serial.println(BTDown.currentSate);
  if (BTDown.currentSate != BTDown.lastState) {
    BTDown.timeDebouce = millis();
    BTDown.lastState = BTDown.currentSate;
  }
  if (millis() - BTDown.timeDebouce >= timeDebouncing) {
    if (BTDown.lastDebounceState == LOW && BTDown.currentSate == HIGH) {
      numPositionTo++;  // selected
      if (numPositionTo > 6) {
        numPositionTo = 1;
      }
      Serial.print("Vi Tri muon den :");
      Serial.println(numPositionTo);
    }
    BTDown.lastDebounceState = BTDown.currentSate;
  }

  BTUp.currentSate = digitalRead(BTUP);
  //Serial.print("Nut Nhan Up : ");
  //Serial.println(BTUp.currentSate);
  if (BTUp.currentSate != BTUp.lastState) {
    BTUp.timeDebouce = millis();
    BTUp.lastState = BTUp.currentSate;
  }
  if (millis() - BTUp.timeDebouce >= timeDebouncing) {
    if (BTUp.lastDebounceState == LOW && BTUp.currentSate == HIGH) {
      numPositionTo--;  // selected
      if (numPositionTo <= 0) {
        numPositionTo = 6;
      }
      Serial.print("Vi Tri muon den :");
      Serial.println(numPositionTo);
    }
    BTUp.lastDebounceState = BTUp.currentSate;
  }
}

void rotate(int steps, float speed) {
  //rotate a specific number of microsteps (8 microsteps per step) - (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  int dir = (steps > 0) ? HIGH : LOW;
  steps = abs(steps);

  digitalWrite(DIR_PIN, dir);

  float usDelay = (1 / speed) * 70;

  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(usDelay);

    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(usDelay);
  }
}

void rotateDeg(float deg, float speed) {
  //rotate a specific number of degrees (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  int dir = (deg > 0) ? HIGH : LOW;
  digitalWrite(DIR_PIN, dir);

  int steps = abs(deg) * (1 / 0.01);  //0.225
  float usDelay = (1 / speed) * 70;

  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(usDelay);

    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(usDelay);
  }
}
boolean readID() {
  //Check if a new tag is detected or not. If not return.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }
  //Check if a new tag is readable or not. If not return.
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  tagID = "";
  // Read the 4 byte UID
  for (uint8_t i = 0; i < 4; i++) {
    //readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));  // Convert the UID to a single String
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA();  // Stop reading
  return true;
}