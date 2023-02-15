#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED 寬度像素
#define SCREEN_HEIGHT 64 // OLED 高度像素

#include "icons.h"
#include "notes.h"

int LED[5] = {6,7,8,5,4}; // LED pins
int BUTTON[5] = {15,14,16,10,18}; // button pins
int SPEAKER_PIN = 9; // connect beeper with this pin and ground
int LEVELTIME[4] = {1000, 750, 500, 350};
int BLANK_TIME = 250; // blank between color change

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

bool target[5] = {false, false, false, false, false};
bool answered = false;
long long lastTime = 0;
int score = 0;

int currentLevel = 0;
long long currentTime = 0;
int interval = LEVELTIME[currentLevel];

void drawScore() {
  char buff[4];
  display.clearDisplay();
  display.setTextSize(9);             // 設定文字大小
  display.setTextColor(1);        // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
  display.setCursor(12,0);             // 設定起始座標
  sprintf(buff, "%02d", score);
  display.print(buff);        // 要顯示的字串
  display.display();                  // 要有這行才會把文字顯示出來
}

void circleLED(int time=100){
  digitalWrite(LED[1], LOW);
  digitalWrite(LED[2], LOW);
  digitalWrite(LED[3], LOW);
  digitalWrite(LED[4], LOW);
  digitalWrite(LED[0], HIGH);
  delay(time);
  digitalWrite(LED[0], LOW);
  digitalWrite(LED[1], HIGH);
  delay(time);
  digitalWrite(LED[1], LOW);
  digitalWrite(LED[2], HIGH);
  delay(time);
  digitalWrite(LED[2], LOW);
  digitalWrite(LED[3], HIGH);
  delay(time);
  digitalWrite(LED[3], LOW);
  digitalWrite(LED[4], HIGH);
  delay(time);
  digitalWrite(LED[4], LOW);
}

void drawBootImage(void) {
  display.clearDisplay();
  display.drawBitmap(32,30,target_bmp, 52, 32, 1);
  display.display();  // 要有這行才會把文字顯示出來
  circleLED();
  display.clearDisplay();
  display.drawBitmap(32,30,target_bmp, 52, 32, 1);
  display.drawBitmap(100, 16,hammer_bmp, 41, 31, 1);
  display.display();
  circleLED();
  display.clearDisplay();
  display.drawBitmap(32,30,target_bmp, 52, 32, 1);
  display.drawBitmap(76, 2,hammer_bmp, 41, 31, 1);
  display.display();
  circleLED();
  display.clearDisplay();
  display.drawBitmap(32,30,target_bmp, 52, 32, 1);
  display.drawBitmap(54, -5,hammer_bmp, 41, 31, 1);
  display.display();
  circleLED();
  display.clearDisplay();
  display.drawBitmap(32,30,target_bmp, 52, 32, 1);
  display.drawBitmap(39, 5,hammer_hit_bmp, 52, 29, 1);
  display.display();
  circleLED();
}

void showWinScene(){
  display.clearDisplay();
  display.drawBitmap(90,27,firework1_bmp, 10, 10, 1);
  display.display();
  tone(SPEAKER_PIN, NOTE_C6);
  circleLED(50);
  display.clearDisplay();
  display.drawBitmap(79,16,firework2_bmp, 32, 32, 1);
  display.display();
  tone(SPEAKER_PIN, NOTE_D6);
  circleLED(50);
  display.clearDisplay();
  display.drawBitmap(63,0,firework3_bmp, 64, 64, 1);
  display.drawBitmap(20,40,firework1_bmp, 10, 10, 1);
  display.display();
  tone(SPEAKER_PIN, NOTE_E6);
  circleLED(50);
  display.clearDisplay();
  display.drawBitmap(63,0,firework4_bmp, 64, 64, 1);
  display.drawBitmap(9,29,firework2_bmp, 32, 32, 1);
  display.display();
  tone(SPEAKER_PIN, NOTE_F6);
  circleLED(50);
  display.clearDisplay();
  display.drawBitmap(-7,13,firework3_bmp, 64, 64, 1);
  display.display();
  tone(SPEAKER_PIN, NOTE_G6);
  circleLED(50);
  display.clearDisplay();
  display.drawBitmap(-7,13,firework4_bmp, 64, 64, 1);
  display.drawBitmap(49,27,firework1_bmp, 10, 10, 1);
  display.display();
  tone(SPEAKER_PIN, NOTE_A6);
  circleLED(50);
  display.clearDisplay();
  display.drawBitmap(47,16,firework2_bmp, 32, 32, 1);
  display.display();
  tone(SPEAKER_PIN, NOTE_B6);
  circleLED(50);
  display.clearDisplay();
  display.drawBitmap(31,0,firework3_bmp, 64, 64, 1);
  display.display();
  tone(SPEAKER_PIN, NOTE_C7);
  circleLED(50);
  display.clearDisplay();
  display.drawBitmap(31,0,firework4_bmp, 64, 64, 1);
  display.display();
  circleLED(100);
  tone(SPEAKER_PIN, NOTE_C5, 1000);
}

void drawSpeedup(){
  display.clearDisplay();
  display.drawBitmap(32,0,speed_bmp, 64, 64, 1);
  display.display();
  for(int i=0;i<=4;i++)circleLED();
}


void playRightTone(){
  tone(SPEAKER_PIN, NOTE_E6);
  delay(100);
  tone(SPEAKER_PIN, NOTE_A6, 200);
}

void playWrongTone(){
  tone(SPEAKER_PIN, NOTE_F4);
  delay(100);
  tone(SPEAKER_PIN, NOTE_F3, 200);
}

void setup() {

  // 偵測是否安裝好OLED了
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 一般1306 OLED的位址都是0x3C
    for(;;)circleLED(500); // Don't proceed, loop forever
  }
  display.setRotation(2); // 轉180度
  //showWinScene();
  drawBootImage();    // 顯示圖形
  for(int i=0;i<=4;i++){
    pinMode(LED[i], OUTPUT);
    digitalWrite(LED[i], LOW);
    pinMode(BUTTON[i], INPUT_PULLUP);
  }
  pinMode(A3, INPUT);
  randomSeed(analogRead(A3));
  drawScore();
}


void loop() {
  if(millis() > lastTime + interval){ // timeout, change color
    for(int i=0;i<=4;i++) {
      digitalWrite(LED[i], LOW);
      target[i] = false;
    }
    if(score < 0) score = 0;    
    else if(score == 20 && currentLevel == 0){
      drawSpeedup();
      currentLevel = 1;
      interval = LEVELTIME[1];
    }
    else if(score == 50 && currentLevel == 1){
      drawSpeedup();
      currentLevel = 2;
      interval = LEVELTIME[2];
    }
    else if(score == 80 && currentLevel == 2){
      drawSpeedup();
      currentLevel = 3;
      interval = LEVELTIME[3];
    }
    else if(score >= 100){
      showWinScene();
      score = 0;
      currentLevel = 0;
    }
    delay(BLANK_TIME);
    int x = random(0,5);
    digitalWrite(LED[x], HIGH);
    target[x] = true;
    answered = false;
    lastTime = millis();
  }
  if(!answered){
    for(int i=0;i<=4;i++){
      if(!digitalRead(BUTTON[i])) {
        if(target[i]) {
          playRightTone();
          score += 1;
        }
        else {
          playWrongTone();
          score -= 1;
        }
        if(score < 0) score = 0;
        answered = true;
      }
    }
  }
  drawScore();

}


