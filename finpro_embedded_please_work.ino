#include <Wire.h>
#include <MPU6050.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
MPU6050 mpu;

#define buzzer 12
#define led 13

float xg, yg, zg;
unsigned long lastBeepTime = 0;
unsigned long lastLogTime = 0;
int earthquakeLevel = 0;

#define mildVal 200
#define mediumVal 300
#define severeVal 400
#define logInterval 1000

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  delay(1000);
  lcd.print("Earthquake Detector");
  lcd.setCursor(0,1);
  lcd.print("Initializing...");
  delay(2000);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(buzzer, LOW);
  digitalWrite(led, LOW);

  Wire.begin();
  mpu.initialize();
  
  lcd.clear();
  lcd.print("Status: Normal");
  lcd.setCursor(0,1);
  lcd.print("X:");
  lcd.setCursor(0,2);
  lcd.print("Y:");
  lcd.setCursor(0,3);
  lcd.print("Z:");
  Serial.println("Time(ms),X,Y,Z,EarthquakeLevel");
}

void loop() {
  unsigned long currentTime = millis();

  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Convert to g force
  xg = ax / 16384.0;
  yg = ay / 16384.0;
  zg = az / 16384.0;

  float totalG = sqrt(xg*xg + yg*yg + zg*zg) - 1.0; // Remove 1g due to gravity

  lcd.setCursor(2,1);
  lcd.print("    ");
  lcd.setCursor(2,1);
  lcd.print(xg, 2);
  lcd.setCursor(2,2);
  lcd.print("    ");
  lcd.setCursor(2,2);
  lcd.print(yg, 2);
  lcd.setCursor(2,3);
  lcd.print("    ");
  lcd.setCursor(2,3);
  lcd.print(zg, 2);
  
  if(totalG >= severeVal) {
    earthquakeLevel = 3;
  } else if(totalG >= mediumVal) {
    earthquakeLevel = 2;
  } else if(totalG >= mildVal) {
    earthquakeLevel = 1;
  } else {
    earthquakeLevel = 0;
  }
  
  switch(earthquakeLevel) {
    case 1:
      lcd.setCursor(0,0);
      lcd.print("Status: Mild Quake ");
      blinkAndBeep(1000, 1);
      break;
    case 2:
      lcd.setCursor(0,0);
      lcd.print("Status: Medium Quake");
      blinkAndBeep(500, 2);
      break;
    case 3:
      lcd.setCursor(0,0);
      lcd.print("Status: Severe Quake");
      blinkAndBeep(250, 4);
      break;
    default:
      lcd.setCursor(0,0);
      lcd.print("Status: Normal      ");
      digitalWrite(buzzer, LOW);
      digitalWrite(led, LOW);
  }
  
  if (currentTime - lastLogTime >= logInterval) {
    lastLogTime = currentTime;
    logData(currentTime, xg, yg, zg, earthquakeLevel);
  }
  
  delay(100);
}

void blinkAndBeep(int interval, int count) {
  unsigned long currentTime = millis();
  if (currentTime - lastBeepTime >= interval) {
    lastBeepTime = currentTime;
    for (int i = 0; i < count; i++) {
      digitalWrite(buzzer, HIGH);
      digitalWrite(led, HIGH);
      delay(50);
      digitalWrite(buzzer, LOW);
      digitalWrite(led, LOW);
      delay(50);
    }
  }
}

void logData(unsigned long time, float x, float y, float z, int level) {
  Serial.print(time);
  Serial.print(",");
  Serial.print(x, 2);
  Serial.print(",");
  Serial.print(y, 2);
  Serial.print(",");
  Serial.print(z, 2);
  Serial.print(",");
  Serial.println(level);
}
