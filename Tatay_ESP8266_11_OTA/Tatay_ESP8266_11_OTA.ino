/*
   00 - switch email victor/tet = blue
   01 - buzzer / TXD
   02 - switch email che2/paul = gray
   03 - switch email paz/butch = red / RXD
   04 - \ LCD
   05 - / LCD
   12 - switch email PC Technical Prob
   13 - switch email EMERGENCY = red
   14 - RED LED (error or no Wifi)
   15 - switch LCD read = green
   16 - sleep, cannot use
*/

// Ping
//#include <ESP8266Ping.h>

//IPAddress remote_ip(192, 168, 1, 1);
//int ipcntr = 1;

//buton debounce ===========================================================================
#include <Bounce2.h>
#define BUTTON_PIN_00 00
#define BUTTON_PIN_02 02
#define BUTTON_PIN_03 03
#define BUTTON_PIN_12 12
#define BUTTON_PIN_13 13
#define BUTTON_PIN_15 15
#define debouncedelay 10
// Instantiate a Bounce object
Bounce debouncer00 = Bounce();
Bounce debouncer02 = Bounce();
Bounce debouncer03 = Bounce();
Bounce debouncer12 = Bounce();
Bounce debouncer13 = Bounce();
Bounce debouncer15 = Bounce();

// OUTPUTS
#define REDLED 14 // tatay board
#define buzz 01

// i2c LCD ========================================================================================
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 20, 4);
int lcdcursor = 0;

// BLYNK =========================================================================================
#define BLYNK_MAX_SENDBYTES 128
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
WidgetLED statled(V1);
WidgetLED msgled(V2);
WidgetTerminal terminal(V3);
char auth[] = "ab238bf5b42a4002981695dafe5abae4";
//char ssid[] = "SingaporeMilitia";
//char pass[] = "123Qweasd";
char ssid[] = "VicenteTagayun";
char pass[] = "27Author";
//char ssid[] = "PLDTHOMEFIBRAntipoloPolice";
//char pass[] = "TAGAYUNFAMILY";

// Timer for Blynk ==============================================================================
#include <SimpleTimer.h>          //Timer Rotation Library
SimpleTimer timer;

// OTA
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
const char* host = "Vicente-ESP-Messenger";

// my variable
int email_cntr = 0;
bool lcd_on = false;
int intitial_lcd = 0;
bool wifi_stat = false;
int wifi_stat_cntr = 0;
bool blynk_stat = false;
int blynk_stat_cntr = 0;
bool buzz_on = false;
bool msg_stat = false;
bool warning_stat = false;
int lcdpage = 0;
int connection_stat = 4; //0 = ok , 1 = no net , 2 = nowifi

String LCD_MSG [3][4] = {{"", "", "", ""},
  { "  No Internet !!!! ",
    "Pls power off router",
    "then power it again ",
    "Reset this board too"
  },
  { "  Router is off!!   ",
    "Pls power off router",
    "then power it again ",
    "Reset this board too"
  }
};

void email_power_on() {
  // Send e-mail when your hardware gets connected to Blynk Server
  // Just put the recepient's "e-mail address", "Subject" and the "message body"
  // Blynk.email("vic.tagayun@yahoo.com,vic_tagayun@yahoo.com", "Power on", "Just On");
}

void lcd_init() {
  //lcd.begin();
  lcd.backlight();
  lcd.home();
  Serial.println("LCD Init");
}

void print_status() {
  Serial.println("==========================");
  Serial.print("email_cntr =      "); Serial.println(email_cntr);
  Serial.print("lcd_on =          "); Serial.println(lcd_on);
  Serial.print("intitial_lcd =    "); Serial.println(intitial_lcd);
  Serial.print("wifi_stat =       "); Serial.println(wifi_stat);
  Serial.print("wifi_stat_cntr =  "); Serial.println(wifi_stat_cntr);
  Serial.print("blynk_stat =      "); Serial.println(blynk_stat);
  Serial.print("blynk_stat_cntr = "); Serial.println(blynk_stat_cntr);
  Serial.print("buzz_on =         "); Serial.println(buzz_on);
  Serial.print("msg_stat =        "); Serial.println(msg_stat);
  Serial.print("lcdpage =         "); Serial.println(lcdpage);
  Serial.print("connection_stat = "); Serial.println(connection_stat);
  Serial.print("warning_stat =    "); Serial.println(warning_stat);
  Serial.print("lcdcursor =       "); Serial.println(lcdcursor);
  Serial.println("==========================");
}

void output_init() {
  pinMode(REDLED, OUTPUT);
  pinMode(buzz, OUTPUT);
  analogWriteFreq(4000); // https://github.com/esp8266/Arduino/issues/582
}

void lcd_display_ip() {
  lcd.print(WiFi.localIP());
  lcd.setCursor(0, 1);
  lcd.print("This Device is now");
  lcd.setCursor(0, 2);
  lcd.print("connected to the net");
  lcd.setCursor(0, 3);
  lcd.print("Turning off the LCD.");
  Serial.println(WiFi.localIP());
  Serial.println("This Device is now");
  Serial.println("connected to the net");
  Serial.println("Turning off the LCD.");
}

void blynk_begin() {
  lcd.setCursor(0, 0);
  lcd.print("Connecting to Wifi..");
  lcd.setCursor(0, 1);
  lcd.print("This message should");
  lcd.setCursor(0, 2);
  lcd.print("dissappear if ");
  lcd.setCursor(0, 3);
  lcd.print("connected to wifi..");
  Serial.println("Connecting to Wifi..");
  Serial.println("This message should");
  Serial.println("disappear if ");
  Serial.println("connected to wifi..");

  Blynk.begin(auth, ssid, pass);
  while (Blynk.connect() == false) { // Wait until connected
  }
  wifi_stat = true; connection_stat = 0;
}

void debouncer_setup() {
  // Setup the first button with an internal pull-up :

  pinMode(BUTTON_PIN_00, INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  debouncer00.attach(BUTTON_PIN_00);
  debouncer00.interval(debouncedelay); // interval in ms

  //pinMode(BUTTON_PIN_02, INPUT_PULLUP);
  debouncer02.attach(BUTTON_PIN_02);
  debouncer02.interval(debouncedelay); // interval in ms

  pinMode(BUTTON_PIN_03, INPUT_PULLUP);
  debouncer03.attach(BUTTON_PIN_03);
  debouncer03.interval(debouncedelay); // interval in ms

  pinMode(BUTTON_PIN_12, INPUT_PULLUP);
  debouncer12.attach(BUTTON_PIN_12);
  debouncer12.interval(debouncedelay); // interval in ms

  pinMode(BUTTON_PIN_13, INPUT_PULLUP);
  debouncer13.attach(BUTTON_PIN_13);
  debouncer13.interval(debouncedelay); // interval in ms

  pinMode(BUTTON_PIN_15, INPUT_PULLUP);
  debouncer15.attach(BUTTON_PIN_15);
  debouncer15.interval(debouncedelay); // interval in ms
}

void setup()
{
  Serial.begin(115200);

  // Blynk wifi
  blynk_begin();

  // OTA
  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]()  // Display and Ser
  {
    Serial.println("Starting OTA");
  });

  ArduinoOTA.onEnd([]() // Display and Ser
  {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    ESP.restart();
  });

  ArduinoOTA.begin();

  //lcd
  lcd_init();

  //outputs
  output_init();

  //display ip
  lcd_display_ip();

  timer.setInterval(1000L, every_onesecond);
  statled.off();
  msgled.off();
  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.println("-------------");
  terminal.flush();

  email_power_on();

  //deboucer setup
  debouncer_setup();
}

void buzzer_on() {
  if (buzz_on) {
    buzz_on = false;
    analogWrite(buzz, 0);
  } else {
    buzz_on = true;
    analogWrite(buzz, 512);
  }
}

// V1 LED Widget is blinking
void every_onesecond() {
  //inital LCD
  if (intitial_lcd < 15) {
    intitial_lcd = intitial_lcd + 1;
  } else {
    if (intitial_lcd == 15) {
      lcd.clear();
      lcd.noBacklight();
      intitial_lcd = intitial_lcd + 1;
      Serial.println("LCD Clear");
    }
  }

  if (blynk_stat) {
    if (statled.getValue()) {
      statled.off();
      digitalWrite(REDLED, LOW);
    } else {
      statled.on();
      digitalWrite(REDLED, HIGH);
    }
  }

  if (!wifi_stat) {
    wifi_stat_cntr++;
  } else {
    wifi_stat_cntr = 0;
  }

  if (!blynk_stat) {
    blynk_stat_cntr++;
  } else {
    blynk_stat_cntr = 0;
    warning_stat = false;
    connection_stat = 0;
  }

  // int connection_stat = 4; //0 = ok , 1 = no net , 2 = router off
  // Case 1: hav wifi no internet ; blynk_stat_cntr > 0 , wifi_stat_cntr = 0
  // case 2: router off ; blynk_stat_cntr = wifi_stat_cntr +1

  if (blynk_stat_cntr >= 30) {
    if (blynk_stat_cntr > (wifi_stat_cntr + 15)) { // hav wifi but no internet
      connection_stat = 1;
    } else if ((wifi_stat_cntr + 15) > blynk_stat_cntr) { // router off
      connection_stat = 2;
    } else { // other
      connection_stat = 0; // ??
    }
  }

  if (connection_stat >= 1 || lcdpage == 2)
  {
    if (msg_stat == 1)
    { // have message in Q
      if (lcdpage == 1)
      { // switch bet actual message and warning no wifi/net
        lcdpage = 2; // change to warning no wifi/net
        // display warning
        buzzer_on();
        lcd.backlight();
        lcd.clear();
        for (int cntr = 0 ; cntr < 4; cntr++)
        {
          lcd.print(LCD_MSG [connection_stat][cntr]);
          Serial.println(LCD_MSG [connection_stat][cntr]);
        }
      } else
      {
        lcdpage = 1; // change to actual message
        // display actual message
        buzzer_on();
        lcd.backlight();
        lcd.clear();
        for (int cntr = 0 ; cntr < lcdcursor; cntr++)
        {
          lcd.print(LCD_MSG [lcdpage - 1][cntr]);
          Serial.println(LCD_MSG [lcdpage - 1][cntr]);
        }
      }
    } else
    { // display the warning
      if (!warning_stat) // check is warning is already displayed
      {
        warning_stat = true;
        lcd.backlight();
        lcd.clear();
        for (int cntr = 0 ; cntr < 4; cntr++)
        {
          lcd.print(LCD_MSG [connection_stat][cntr]);
          Serial.println(LCD_MSG [connection_stat][cntr]);
        }
      }
    }
  }

  // email cntr sent section
  if (email_cntr > 0)  {
    email_cntr = email_cntr + 1;
    if ( email_cntr > 70 )    {
      email_cntr = 0;
      lcd.clear();
      lcd.noBacklight();
    }
  }

  //print_status();

}

// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
BLYNK_WRITE(V3)
{
  if (String("STATUS") == param.asStr())  {
    terminal.print("email_cntr: ");
    terminal.println(email_cntr);
    terminal.print("lcd_on: ");
    terminal.println(lcd_on);
    terminal.print("intitial_lcd : ");
    terminal.println(intitial_lcd);
    terminal.print("msg_stat : ");
    terminal.println(msg_stat);
    terminal.print("lcdpage : ");
    terminal.println(lcdpage);
    terminal.print("lcdcursor : ");
    terminal.println(lcdcursor);
    terminal.flush();
  } else if (String("CLEAR") == param.asStr())  {
    lcd.clear();
    lcd.noBacklight();
    msgled.off();
    lcdcursor = 0;
    lcdpage = 0;
    msg_stat = false;
    terminal.flush();
  }  else if (String("DISCONNECT") == param.asStr())  {
    WiFi.mode(WIFI_OFF);
  }  else  if (String("EMAIL") == param.asStr())  {
    Blynk.email("vic.tagayun@yahoo.com,vic_tagayun@yahoo.com,victor.t@asmpt.com", "EMAIL TEST", "Need Help!");
  }  else  if (String("BLYNKAPP") == param.asStr())  {
    Blynk.email("vic.tagayun@yahoo.com,vic_tagayun@yahoo.com,victor.t@asmpt.com", "EMAIL TEST", "Need Help!");
  } else  { // first run, clear LCD
    if (lcdcursor == 0)    {
      lcd.clear();
    }
    if (param.getLength() > 20 )    {
      terminal.println("Sentence more that 20chars! This message not sent!") ;
    }
    else    {
      if (lcdcursor == 4) { // 2nd chk and more check if last line
        terminal.println("Last line already!! Type CLEAR to clear LCD. This message not Sent!") ;
      }
      else      {
        msgled.on();
        lcdpage = 1;
        lcd.backlight();
        lcd.setCursor(0, lcdcursor);
        LCD_MSG [lcdpage - 1][lcdcursor] = param.asStr();
        lcd.print(LCD_MSG [lcdpage - 1][lcdcursor]);
        Serial.println(LCD_MSG [lcdpage - 1][lcdcursor]);
        terminal.println(param.asStr());
        lcdcursor = lcdcursor + 1 ;
        msg_stat = true;
        if (lcdcursor == 4)  { // 1st check if already 4th line
          terminal.println("Last line already!! Type CLEAR to clear LCD.") ;
        }
      }
    }
    terminal.flush();
  }
}

void emailcannotsend() {
  lcd.setCursor(0, 2);
  lcd.print("Email Cannot Send");
  lcd.setCursor(0, 3);
  lcd.print("need to wait 1min.");
  Serial.println("Email Cannot Send");
  Serial.println("need to wait 1min.");
}

//void ping() {
//  if (ipcntr < 256) {
//    IPAddress remote_ip(192, 168, 1, ipcntr);
//    Serial.print("Pinging ip ");
//    Serial.println(remote_ip);
//
//    if (Ping.ping(remote_ip)) {
//      Serial.println("Success!!");
//    } else {
//      Serial.println("Error :(");
//    }
//    ipcntr = ipcntr + 1;
//  }
//}

void debounce_update() {
  // Update the Bounce instances :
  debouncer00.update();
  debouncer02.update();
  debouncer03.update();
  debouncer12.update();
  debouncer13.update();
  debouncer15.update();
}

void lcdBacklight_email() {
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Email sent to");
  lcd.setCursor(0, 1);
  Serial.println("Email sent to");
}

void loop()
{
  // OTA
  ArduinoOTA.handle();

  // Blynk
  Blynk.run();
  timer.run();

  if (WiFi.status() == WL_CONNECTED) {
    wifi_stat = true;
  } else {
    wifi_stat = false;
  }

  if (Blynk.connected()) {
    blynk_stat = true;
  } else {
    blynk_stat = false;
  }

  debounce_update();

  if (debouncer00.fell())
  {
    if (email_cntr > 0)
    {
      emailcannotsend();
    }
    else
    {
      lcdBacklight_email();
      lcd.print("Cocoy and Tet");
      Serial.println("Hello Tatay Here, Need Help!");
      Blynk.email("vic.tagayun@yahoo.com,vic_tagayun@yahoo.com,victor.t@asmpt.com,maritespesimotagayun@yahoo.com", "Hello Tatay Here", "Need Help!");
      email_cntr = 1;
    }
  }

  if (debouncer02.fell())
  {
    if (email_cntr > 0)
    {
      emailcannotsend();
    }
    else
    {
      lcdBacklight_email();
      lcd.print("Che2 and Paul");
      Serial.println("Che2 and Paul");
      Blynk.email("vic.tagayun@yahoo.com,vic_tagayun@yahoo.com", "Che2 and Paul", "Che2 and Paul");
      email_cntr = 1;
    }
  }

  if (debouncer03.fell())
  {
    if (email_cntr > 0)
    {
      emailcannotsend();
    }
    else
    {
      lcdBacklight_email();
      lcd.print("Pachit and Butz");
      Serial.println("Pachit and Butz");
      Blynk.email("vic.tagayun@yahoo.com,vic_tagayun@yahoo.com", "Pachit and Butz", "Pachit and Butz");
      email_cntr = 1;
    }
  }

  if (debouncer12.fell())
  {
    if (email_cntr > 0)
    {
      emailcannotsend();
    }
    else
    {
      lcdBacklight_email();
      lcd.print("Victor Tech Prob");
      Serial.println("Victor Tech Prob");
      Blynk.email("vic.tagayun@yahoo.com,vic_tagayun@yahoo.com", "Victor Tech Prob", "Victor Tech Prob");
      email_cntr = 1;
    }
  }

  if (debouncer13.fell())
  {
    if (email_cntr > 0)
    {
      emailcannotsend();
    }
    else
    {
      lcdBacklight_email();
      lcd.print("EMERGENCY!!!!");
      Serial.println("EMERGENCY!!!!");
      Blynk.email("vic.tagayun@yahoo.com,vic_tagayun@yahoo.com", "EMERGENCY!!!!", "EMERGENCY!!!!");
      email_cntr = 1;
    }
  }

  if (debouncer15.rose())
  {
    if (email_cntr > 0)
    {
      emailcannotsend();
    }
    else
    {
      lcd.clear();
      lcd.noBacklight();
      msgled.off();
      lcdcursor = 0;
      lcdpage = lcdpage - 1;
      //      for (int cntr = 0; cntr < 4; cntr++) {
      //        LCD_MSG [lcdpage][cntr] = "";
      //      }
    }
  }
}
