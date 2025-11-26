
#define MODE_BUTTON_PIN 5   // toggle Modes
unsigned long lastModeBtnChange = 0;
unsigned long modeBtnDebounce = 150;
byte modeBtnState;

// modes
#define MODE_OBSTACLE 0
#define MODE_DINOSAUR 1
int currentMode = MODE_OBSTACLE;

#include <LiquidCrystal.h>

#define BUTTON_PIN 2

#define LCD_RS_PIN A5
#define LCD_E_PIN A4
#define LCD_D4_PIN 6
#define LCD_D5_PIN 7
#define LCD_D6_PIN 8
#define LCD_D7_PIN 9


LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

// animation / timing
unsigned long lastTimeLegChange = 0;
unsigned long legChangeDelay = 120; 

// dino char states
int withLegBack = 0;
int withoutLegFront = 3;
int withoutLegBack = 2;
int withLegFront = 1;

int backState;
int frontState;
volatile int rowDinosaur = 1; 

// custom characters
byte customChar1[8] = {
  B00000,
  B00000,
  B00100,
  B00100,
  B00111,
  B00111,
  B00011,
  B00010
};
byte customChar2[8] = {
  B01111,
  B01011,
  B01111,
  B01100,
  B11100,
  B11100,
  B11000,
  B01000
};

byte customChar3[8] = {
  B00000,
  B00000,
  B00100,
  B00100,
  B00111,
  B00111,
  B00011,
  B00000
};

byte customChar4[8] = {
  B01111,
  B01011,
  B01111,
  B01100,
  B11100,
  B11100,
  B11000,
  B00000
};

byte customCharTree[8] = {
  B00000,
  B10001,
  B10101,
  B10101,
  B01110,
  B00100,
  B00100,
  B00100
};

// tree movement
unsigned long previousTimeTree = 0;
unsigned long treeInterval = 100; // ms between tree steps
int colTree = 15;

// score
long score = 0;

// bookkeeping
int previousRow = 1;

void togglePositionInterrupt() {
  // toggle between top and bottom rows
  rowDinosaur = (rowDinosaur == 0) ? 1 : 0;
}

void printTree(int col, int row) {
  // erase next position and print tree at col,row
  lcd.setCursor((col + 1) % 16, row);
  lcd.print(' ');
  lcd.setCursor(col, row);
  lcd.write((uint8_t)4);
}

void drawDinosaur() {
  lcd.setCursor(1, rowDinosaur);
  lcd.write((uint8_t)backState);
  lcd.write((uint8_t)frontState);
}

void clearDinosaur(int prevRow) {
  lcd.setCursor(1, prevRow);
  lcd.print(' ');
  lcd.print(' ');
}

void updateScoreDisplay() {
  
  lcd.setCursor(10, 0);
  lcd.print("Score");
  lcd.setCursor(12, 0);
  lcd.print("    ");
  lcd.setCursor(12, 0);
  lcd.print(score);
}

void gameOver() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("GAME OVER");
  lcd.setCursor(3, 1);
  lcd.print("Score: ");
  lcd.print(score);
  delay(1500);

  // reset game state
  lcd.clear();
  score = 0;
  colTree = 15;
  previousTimeTree = millis();
  lastTimeLegChange = millis();
  previousRow = rowDinosaur;
  backState = withLegBack;
  frontState = withoutLegFront;

  // show score label again
  updateScoreDisplay();
}

void setupDinosaur_only() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.begin(16, 2);

  lcd.createChar(0, customChar1);
  lcd.createChar(1, customChar2);
  lcd.createChar(2, customChar3);
  lcd.createChar(3, customChar4);
  lcd.createChar(4, customCharTree);

  backState = withLegBack;
  frontState = withoutLegFront;
  drawDinosaur();

  updateScoreDisplay();

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), togglePositionInterrupt, CHANGE);

  previousTimeTree = millis();
  lastTimeLegChange = millis();
}


void runDinosaurIteration() {
  unsigned long timeNow = millis();

  // dino leg animation
  if (timeNow - lastTimeLegChange > legChangeDelay) {
    if (previousRow != rowDinosaur) {
      clearDinosaur(previousRow);
    }
    previousRow = rowDinosaur;
    lastTimeLegChange = timeNow;

    if (backState == withLegBack) {
      backState = withoutLegBack;
      frontState = withLegFront;
    } else {
      backState = withLegBack;
      frontState = withoutLegFront;
    }
    drawDinosaur();
  }

  // tree movement (fixed interval)
  if (timeNow - previousTimeTree > treeInterval) {
    previousTimeTree = timeNow;

    lcd.setCursor((colTree + 1) % 16, 1);
    lcd.print(' ');
    colTree--;
    if (colTree < 0) colTree = 15;
    printTree(colTree, 1);
    if (colTree == 1) {
      if (rowDinosaur == 1) {
        gameOver();
        return; 
      } else {
        score++;
        updateScoreDisplay();
      }
    }
  }

  delay(5); 
}



#include <EEPROM.h>

// Pins for object sketch
#define BUZZER_PIN 10
#define TRIGGER_PIN 4
#define ECHO_PIN 3

#define WARNING_LED_PIN 11
#define ERROR_LED_PIN 12
#define BUTTON_PIN_OBJ 2  

// Distance units
#define DISTANCE_UNIT_CM 0
#define DISTANCE_UNIT_IN 1
#define CM_TO_INCHES 0.393701
#define EEPROM_ADDRESS_DISTANCE_UNIT 50

// Thresholds
#define LOCK_DISTANCE 10.0
#define WARNING_DISTANCE 50.0

// LCD modes
#define LCD_MODE_DISTANCE 0
#define LCD_MODE_SETTINGS 1

// Buzzer type
#define BUZZER_IS_ACTIVE false
#define BUZZER_BEEP_FREQ 1000

// Smoothing
double previousDistance_obj = 400;

// Blinking
unsigned long lastWarningBlink_obj = 0;
unsigned long warningDelay_obj = 500;
byte warningLEDState_obj = LOW;

unsigned long lastErrorBlink_obj = 0;
unsigned long errorDelay_obj = 500;
byte errorLEDState_obj = LOW;

// Button debounce
unsigned long lastBtnChange_obj = 0;
unsigned long btnDebounce_obj = 500;
byte buttonState_obj;

// Flags
bool isLocked_obj = false;
int distanceUnit_obj = DISTANCE_UNIT_CM;
int lcdMode_obj = LCD_MODE_DISTANCE;


void setupObject_only() {
  Serial.begin(115200);
  lcd.begin(16, 2);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(WARNING_LED_PIN, OUTPUT);
  pinMode(ERROR_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN_OBJ, INPUT_PULLUP);

  // Ensure buzzer is off
  if (BUZZER_IS_ACTIVE) digitalWrite(BUZZER_PIN, LOW);
  else noTone(BUZZER_PIN);

  // Load EEPROM
  distanceUnit_obj = EEPROM.read(EEPROM_ADDRESS_DISTANCE_UNIT);
  if (distanceUnit_obj == 255) distanceUnit_obj = DISTANCE_UNIT_CM;

  buttonState_obj = digitalRead(BUTTON_PIN_OBJ);

  lcd.clear();
  lcd.print("Initializing...");
  delay(800);
  lcd.clear();
}

void triggerUltrasonic_obj() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(12);
  digitalWrite(TRIGGER_PIN, LOW);
}

double measureDistance_obj() {
  triggerUltrasonic_obj();
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) return previousDistance_obj;

  double dist = duration / 58.0;

  // smoothing
  dist = previousDistance_obj * 0.6 + dist * 0.4;
  previousDistance_obj = dist;
  return dist;
}

void startBuzzer_obj() {
  if (BUZZER_IS_ACTIVE) digitalWrite(BUZZER_PIN, HIGH);
  else tone(BUZZER_PIN, BUZZER_BEEP_FREQ);
}

void stopBuzzer_obj() {
  if (BUZZER_IS_ACTIVE) digitalWrite(BUZZER_PIN, LOW);
  else noTone(BUZZER_PIN);
}

void toggleErrorLEDandBuzzer_obj() {
  errorLEDState_obj = (errorLEDState_obj == HIGH) ? LOW : HIGH;
  digitalWrite(ERROR_LED_PIN, errorLEDState_obj);

  if (errorLEDState_obj == HIGH) startBuzzer_obj();
  else stopBuzzer_obj();
}

void toggleWarningLED_obj() {
  warningLEDState_obj = (warningLEDState_obj == HIGH) ? LOW : HIGH;
  digitalWrite(WARNING_LED_PIN, warningLEDState_obj);
}

void setWarningBlinkFromDistance_obj(double d) {
  warningDelay_obj = (unsigned long)(d * 4);
  if (warningDelay_obj < 60) warningDelay_obj = 60;
  if (warningDelay_obj > 2000) warningDelay_obj = 2000;
}

void lockSystem_obj() {
  if (!isLocked_obj) {
    isLocked_obj = true;
    warningLEDState_obj = LOW;
    errorLEDState_obj = LOW;
    digitalWrite(WARNING_LED_PIN, LOW);
    digitalWrite(ERROR_LED_PIN, LOW);
    lcd.clear();

    Serial.println("LOCKED");
  }
}

void unlockSystem_obj() {
  if (isLocked_obj) {
    isLocked_obj = false;
    stopBuzzer_obj();
    digitalWrite(ERROR_LED_PIN, LOW);
    lcd.clear();

    Serial.println("UNLOCKED");
  }
}

void printDistanceOnLCD_obj(double dist) {
  if (isLocked_obj) {
    lcd.setCursor(0, 0);
    lcd.print("!!! Obstacle !!!  ");
    lcd.setCursor(0, 1);
    lcd.print("Press to unlock.  ");
    return;
  }

  if (lcdMode_obj == LCD_MODE_DISTANCE) {
    lcd.setCursor(0, 0);
    lcd.print("Dist: ");

    if (distanceUnit_obj == DISTANCE_UNIT_IN) {
      lcd.print(String(dist * CM_TO_INCHES, 1));
      lcd.print(" in ");
    } else {
      lcd.print(String(dist, 1));
      lcd.print(" cm ");
    }

    lcd.setCursor(0, 1);
    if (dist > WARNING_DISTANCE)
      lcd.print(" No obstacle.     ");
    else
      lcd.print("!! Warning !!     ");

  } else {
    lcd.setCursor(0, 0);
    lcd.print("Hold button to");
    lcd.setCursor(0, 1);
    lcd.print("reset settings");
  }
}

void resetSettings_obj() {
  distanceUnit_obj = DISTANCE_UNIT_CM;
  EEPROM.write(EEPROM_ADDRESS_DISTANCE_UNIT, distanceUnit_obj);

  lcd.clear();
  lcd.print("Settings reset");
  delay(900);
  lcd.clear();
}

void toggleLCDScreen_obj() {
  lcdMode_obj = (lcdMode_obj == LCD_MODE_DISTANCE) ? LCD_MODE_SETTINGS : LCD_MODE_DISTANCE;
  lcd.clear();
}

void runObjectIteration() {
  unsigned long now = millis();

  // BUTTON HANDLING
  if (now - lastBtnChange_obj > btnDebounce_obj) {
    byte newBtn = digitalRead(BUTTON_PIN_OBJ);
    if (newBtn != buttonState_obj) {
      buttonState_obj = newBtn;
      lastBtnChange_obj = now;

      if (buttonState_obj == LOW) {   // pressed
        if (isLocked_obj) unlockSystem_obj();
        else toggleLCDScreen_obj();
      }
    }
  }

  // DISTANCE MEASUREMENT
  static unsigned long lastUltra = 0;
  if (now - lastUltra >= 60) {
    lastUltra = now;

    double dist = measureDistance_obj();
    setWarningBlinkFromDistance_obj(dist);
    printDistanceOnLCD_obj(dist);

    Serial.print("Distance: ");
    Serial.println(dist);

    if (!isLocked_obj && dist < LOCK_DISTANCE)
      lockSystem_obj();
  }

  // LED / BUZZER LOGIC
  if (isLocked_obj) {
    if (now - lastErrorBlink_obj >= errorDelay_obj) {
      lastErrorBlink_obj = now;
      toggleErrorLEDandBuzzer_obj();
      toggleWarningLED_obj();
    }
  } else {
    if (now - lastWarningBlink_obj >= warningDelay_obj) {
      lastWarningBlink_obj = now;
      toggleWarningLED_obj();
    }
  }
}


void setup() {
  
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  modeBtnState = digitalRead(MODE_BUTTON_PIN);
  lastModeBtnChange = millis();
  setupObject_only();   
  setupDinosaur_only(); 

  currentMode = MODE_OBSTACLE;
  lcd.clear();
  if (currentMode == MODE_OBSTACLE) {
    lcd.print("Obstacle Mode");
  } else {
    lcd.print("Dinosaur Mode");
  }
  delay(600);
  lcd.clear();
}


void loop() {
  unsigned long now = millis();
  if (now - lastModeBtnChange > modeBtnDebounce) {
    byte newModeBtn = digitalRead(MODE_BUTTON_PIN);
    if (newModeBtn != modeBtnState) {
      modeBtnState = newModeBtn;
      lastModeBtnChange = now;
      if (modeBtnState == LOW) {
        if (currentMode == MODE_OBSTACLE) {
          currentMode = MODE_DINOSAUR;
          lcd.clear();
          lcd.print("Switching to");
          lcd.setCursor(0,1);
          lcd.print("Dinosaur");
          delay(250);
          lcd.clear();
        } else {
          currentMode = MODE_OBSTACLE;
          lcd.clear();
          lcd.print("Switching to");
          lcd.setCursor(0,1);
          lcd.print("Obstacle");
          delay(250);
          lcd.clear();
        }
      }
    }
  }

  if (currentMode == MODE_OBSTACLE) {
    runObjectIteration();
  } else {
    runDinosaurIteration();
  }
}