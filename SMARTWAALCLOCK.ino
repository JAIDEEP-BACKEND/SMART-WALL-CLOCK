//THIS SMARTWATCH CODE WILLHELP YOU TO BUILD YOUR OWN SMART WRIST BAND OR A SMALL CUTE WALL CLOCK THIS WATCH IS CONSTRUCTED AND CODED BY JAIDEEP EVERYTHING WILL BE AVAILABLE AT BACKEND-JAIDEEP GITHUB PROFILE
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON_PIN 14
#define BUZZER_PIN 33 // Buzzer pin

// WiFi Credentials
const char* ssid = "NOWHERE";
const char* password = "12345678";

int mode = 0;
unsigned long lastButtonPress = 0;
bool hotspotStarted = false;
bool wifiConnected = false;

unsigned long lastAlarmCheck = 0;
bool isAlarmActive = false;
unsigned long alarmStartTime = 0;

String memes[] = {
  "404: Sleep Not Found",
  "Ctrl+Alt+Del Problems",
  "I void warranties.",
  "There is no cloud",
  "Trust me, I'm root."
};

void showBootFace() {
  display.clearDisplay();
  display.fillRoundRect(30, 20, 20, 20, 5, SSD1306_WHITE); // Left eye
  display.fillRoundRect(80, 20, 20, 20, 5, SSD1306_WHITE); // Right eye
  display.fillRect(50, 50, 30, 3, SSD1306_WHITE); // Mouth bold
  display.display();
  delay(2000);
}

void connectWiFi() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Connecting WiFi...");
  display.display();

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }

  display.clearDisplay();
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    display.setCursor(0, 10);
    display.println("WiFi Connected!");
    display.display();
    delay(1000);
  } else {
    wifiConnected = false;
    display.setCursor(0, 10);
    display.println("WiFi Failed!");
    display.display();
    delay(2000);
  }
}

void initTime() {
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov"); // IST offset (5.5 hrs)
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    delay(500);
  }
}

void startHotspot() {
  if (!hotspotStarted) {
    WiFi.softAP("JAIDEEP'S SMARTBAND", "SMARTBAND1.0");
    hotspotStarted = true;
  }
}

void showHotspotInfo() {
  startHotspot();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("HOTSPOT ACTIVE");
  display.setCursor(0, 15);
  display.println("SSID: JAIDEEP'S SMARTBAND");
  display.setCursor(0, 25);
  display.println("PASS: SMARTBAND1.0");

  int numClients = WiFi.softAPgetStationNum();
  display.setCursor(0, 40);
  display.print("Devices: ");
  display.println(numClients);
}

void showMeme() {
  int index = random(0, sizeof(memes)/sizeof(memes[0]));
  display.setTextSize(1);
  display.setCursor(0, 25);
  display.println(memes[index]);
}

void showMood() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    int hour = timeinfo.tm_hour;
    display.setTextSize(1);
    display.setCursor(10, 30);
    if (hour == 9) display.println("Bhai neend aa rahi h");
    else if (hour == 12) display.println("Bhai bhukh lagi h");
    else if (hour == 14) display.println("Bhai sone ka mann h");
    else if (hour == 15) display.println("Bhai kab khatam hoga ye sab");
    else if (hour == 16) display.println("Bhai soja");
    else if (hour == 18) display.println("Bhai kuch krte h");
    else if (hour == 22) display.println("Bhai game hi khel le");
    else if (hour == 0) display.println("Soja bsdk");
    else display.println("Mood: Normal");
  } else {
    display.setCursor(20, 30);
    display.println("Mood Error");
  }
}

void checkAlarm() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  int h = timeinfo.tm_hour;
  int m = timeinfo.tm_min;

  if ((h == 7 && m == 30) || (h == 8 && m == 0) || (h == 8 && m == 30) ||
      (h == 16 && m == 30) || (h == 18 && m == 0) || (h == 19 && m == 30)) {
    if (!isAlarmActive) {
      isAlarmActive = true;
      alarmStartTime = millis();
      digitalWrite(BUZZER_PIN, HIGH);
    }
  }

  if (isAlarmActive && millis() - alarmStartTime >= 15000) {
    isAlarmActive = false;
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void showAlarmText() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  int h = timeinfo.tm_hour;
  int m = timeinfo.tm_min;

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 30);
  if ((h == 7 && m == 30) || (h == 8 && m == 0) || (h == 8 && m == 30)) display.println("Wake up!");
  else if (h == 16 && m == 30) display.println("Snacks time!");
  else if (h == 18 && m == 0) display.println("Work time!");
  else if (h == 19 && m == 30) display.println("Dinner time!");
  display.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  randomSeed(analogRead(0));

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while (1);
  }

  showBootFace();
  connectWiFi();
  if (wifiConnected) {
    initTime();
  }
  startHotspot();
}

void loop() {
  checkAlarm();

  if (isAlarmActive) {
    showAlarmText();
    return;
  }

  if (digitalRead(BUTTON_PIN) == LOW && millis() - lastButtonPress > 500) {
    mode = (mode + 1) % 5;
    lastButtonPress = millis();
  }

  display.clearDisplay();

  if (mode == 0) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      char timeStr[10];
      strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
      display.setTextSize(3);
      display.setCursor(20, 20);
      display.print(timeStr);
    } else {
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("Time Error");
    }
  }

  else if (mode == 1) {
    showMood();
  }

  else if (mode == 2) {
    showMeme();
  }

  else if (mode == 3) {
    showHotspotInfo();
  }

  else if (mode == 4) {
    display.setCursor(10, 30);
    display.println("More features soon!");
  }

  display.display();
  delay(500);
}
