#include <Arduino.h>
#include <avr/wdt.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>
#include <elapsedMillis.h>
#include <rm4.h>

#define SECS_PER_MIN (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY (SECS_PER_HOUR * 24L)

#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) ((_time_ % SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) (_time_ / SECS_PER_DAY)
static const int kData0Pin = A3;
static const int kData1Pin = A2;
static const int kData2Pin = A1;
static const int kData3Pin = A0;
RM4 remote(kData0Pin, kData1Pin, kData2Pin, kData3Pin);
//timere
elapsedMillis timerDisplay, timerTest, timerAvarie, timerSec, timerTelecomanda, timer_m_sus, timer_m_jos, timerAfisare, timerSetari, timerRadio, timerRadio_A_M, timerStartCiclu;

#define ON 0 // inversare citire pin din HIGHT to LOW
#define OFF 1

#define PIN_M_SUS 5 // 4
#define PIN_M_JOS 4 // 5
//#define PIN_RELEU_3 6
#define PIN_RELEU_4 7

#define PIN_STATUS 6
#define PIN_BTN_JOS 8
#define PIN_BTN_SUS 9
#define PIN_BTN_MANUAL 10
#define PIN_BTN_AUTOMAT 11

#define SENZOR_JOS 12
#define SENZOR_SUS 13

#define BTN_TELECOM_AUTOMAT A0
#define BTN_TELECOM_MANUAL A1
#define BTN_TELECOM_SUS A2
#define BTN_TELECOM_JOS A3

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
#define NUMFLAKES 10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

Bounce BTN_AUTOMAT = Bounce();
Bounce BTN_MANUAL = Bounce();
Bounce BTN_JOS = Bounce();
Bounce BTN_SUS = Bounce();
Bounce CAPAT_SUS = Bounce();
Bounce CAPAT_JOS = Bounce();
boolean radio_automat = false, radio_manual = false, radio_jos = false, radio_sus = false;

int nrOre = 2;
unsigned long count_sec = 0;
long sec = nrOre * SECS_PER_HOUR - count_sec;
int h = numberOfHours(sec);
int m = numberOfMinutes(sec);
int s = numberOfSeconds(sec);
enum miscare_m
{
  MISCARE_SUS,
  MISCARE_JOS,
  MISCARE_STOP
} miscare;

enum etape_t
{
  AVARIE,
  CURATARE_SUS,
  CURATARE_JOS,
  AUTOMAT,
  MANUAL,
  SETARI,
  STOP
};
etape_t etape, ecran;

#include "global.h"

void setup()
{
  pinMode(PIN_M_SUS, OUTPUT);
  pinMode(PIN_M_JOS, OUTPUT);
  pinMode(PIN_STATUS, OUTPUT);
  pinMode(BTN_TELECOM_AUTOMAT, INPUT);
  pinMode(BTN_TELECOM_MANUAL, INPUT);
  pinMode(BTN_TELECOM_SUS, INPUT);
  pinMode(BTN_TELECOM_JOS, INPUT);
  status(OFF);
  motoare_off();
  // Serial.begin(9600);
  Serial.println("PORNIT");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  BTN_AUTOMAT.attach(PIN_BTN_AUTOMAT);
  BTN_AUTOMAT.interval(50);
  BTN_MANUAL.attach(PIN_BTN_MANUAL);
  BTN_MANUAL.interval(50);
  BTN_JOS.attach(PIN_BTN_JOS);
  BTN_JOS.interval(50);
  BTN_SUS.attach(PIN_BTN_SUS);
  BTN_SUS.interval(50);
  CAPAT_JOS.attach(SENZOR_JOS);
  CAPAT_JOS.interval(50);
  CAPAT_SUS.attach(SENZOR_SUS);
  CAPAT_SUS.interval(50);
  display.clearDisplay();
  display.display();
  display.setCursor(15, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.println("Robert");
  display.setCursor(15, 16);
  display.println("    Reman");
  display.display();
  delay(3000);
  etape = AUTOMAT;
  ecran = AUTOMAT;
  miscare = MISCARE_STOP;
  timerStartCiclu = 0;
}

void loop()
{
  BTN_AUTOMAT.update();
  BTN_MANUAL.update();
  BTN_JOS.update();
  BTN_SUS.update();
  CAPAT_JOS.update();
  CAPAT_SUS.update();

  if (timerTest > 1000)
  {
    Serial.print("SEC: ");
    Serial.println(sec);
    timerTest = 0;
  }
  //  ---- INCEPUT COD INTRARE IN SETARI INTERVAL
  const int button_code = remote.buttonCode();
  switch (button_code)
  {
  case 2:
    Serial.println("A");
    radio_automat = true;
    break;
  case 8:
    Serial.println("B");
    radio_manual = true;
    break;
  case 1:
    Serial.println("C");
    radio_jos = true;
    break;
  case 4:
    Serial.println("D");
    radio_sus = true;
    break;
  default:
    radio_automat = false;
    radio_manual = false;
    radio_jos = false;
    radio_sus = false;
    break;
  }
  if ((BTN_AUTOMAT.read() || radio_automat) && etape == AUTOMAT)
  {
    if (timerStartCiclu > 5000)
    {
      sec = 0;
      timerStartCiclu = 0;
    }
  }
  else
  {
    timerStartCiclu = 0;
  }
  if (BTN_AUTOMAT.rose())
  {
    etape = AUTOMAT;
    ecran = AUTOMAT;
  }
  if (BTN_MANUAL.rose())
  {
    etape = MANUAL;
    ecran = MANUAL;
  }
  if (timerRadio_A_M > 100)
  {
    if (radio_automat)
    {
      etape = AUTOMAT;
      ecran = AUTOMAT;
    }
    if (radio_manual)
    {
      etape = MANUAL;
      ecran = MANUAL;
    }
    timerRadio_A_M = 0;
  }

  switch (miscare)
  {
  case MISCARE_SUS:
    if (timerAfisare > 500)
    {
      ecran = CURATARE_SUS;
      timerAfisare = 0;
    }
    motor_on("sus");
    if (!CAPAT_SUS.read())
    {
      miscare = MISCARE_JOS;
      ecran = CURATARE_JOS;
    }
    break;
  case MISCARE_JOS:
    if (timerAfisare > 500)
    {
      ecran = CURATARE_JOS;
      timerAfisare = 0;
    }
    motor_on("jos");
    if (!CAPAT_JOS.read())
    {
      resetare_cronometru();
      miscare = MISCARE_STOP;
    }
    break;
  case STOP:
    /* code */
    break;
  }
  if (!CAPAT_SUS.read() && !CAPAT_JOS.read())
  {
    //avarie senzori

    etape = STOP;
  }
  switch (etape)
  {
  case AUTOMAT:
    status(OFF);

    if (timerSec > 1000 && sec > 0)
    {
      count_sec++;
      if (count_sec > nrOre * SECS_PER_HOUR)
      {
        count_sec = 0;
      }
      sec = nrOre * SECS_PER_HOUR - count_sec;
      h = numberOfHours(sec);
      m = numberOfMinutes(sec);
      s = numberOfSeconds(sec);

      motoare_off();
      ecran = AUTOMAT;
      timerSec = 0;
      timerAfisare = 1000;
    }
    else if (sec == 0)
    {
      if (miscare == MISCARE_STOP)
      {
        if (!CAPAT_JOS.read())
        {
          ecran = CURATARE_SUS;
          miscare = MISCARE_SUS;
        }
        else if (!CAPAT_SUS.read())
        {
          ecran = CURATARE_JOS;
          miscare = MISCARE_JOS;
        }
        else if (CAPAT_SUS.read() && CAPAT_JOS.read())
        {
          ecran = CURATARE_SUS;
          miscare = MISCARE_SUS;
        }
      }
    }
    if (BTN_SUS.rose() || BTN_JOS.rose() || radio_jos || radio_sus)
    {
      timerSetari = 0;
      etape = SETARI;
    }
    break;

  case MANUAL:
    if (miscare != MISCARE_STOP)
    {
      miscare = MISCARE_STOP;
      motoare_off();
      resetare_cronometru();
    }

    status(ON);
    if ((BTN_JOS.read() || radio_jos) && !BTN_SUS.read())
    {
      if (timerAfisare > 500)
      {
        ecran = CURATARE_JOS;
        timerAfisare = 0;
      }
      motor_on("jos");
    }
    else if ((BTN_SUS.read() || radio_sus) && !BTN_JOS.read())
    {
      if (timerAfisare > 500)
      {
        ecran = CURATARE_SUS;
        timerAfisare = 0;
      }
      motor_on("sus");
    }
    else
    {
      if (timerAfisare > 500)
      {
        ecran = MANUAL;
        timerAfisare = 0;
      }
      motoare_off();
    }
    break;

  case SETARI:
    if (BTN_SUS.rose())
    {
      timerSetari = 0;
      if (nrOre < 24)
      {
        nrOre++;
        ecran = SETARI;
      }
    }
    else if (BTN_JOS.rose())
    {
      timerSetari = 0;
      if (nrOre > 1)
      {
        nrOre--;
        ecran = SETARI;
      }
    }
    if (timerRadio > 100)
    {
      if (radio_sus)
      {
        timerSetari = 0;
        if (nrOre < 24)
        {
          nrOre++;
          ecran = SETARI;
        }
      }
      else if (radio_jos)
      {
        timerSetari = 0;
        if (nrOre > 1)
        {
          nrOre--;
          ecran = SETARI;
        }
      }
      timerRadio = 0;
    }
    if (timerSetari > 5000)
    {
      etape = AUTOMAT;
    }
    break;

  case STOP:
    miscare = MISCARE_STOP;
    motoare_off();
    if (timerAvarie < 500)
    {
      ecran = AVARIE;
      status(ON);
    }
    else if (timerAvarie > 500 && timerAvarie < 1000)
    {

      status(OFF);
    }
    else
    {
      timerAvarie = 0;
    }

    break;
  }

  switch (ecran)
  {
  case AVARIE:

    Serial.println("ECRAN: CURATARE_MANUALA ");
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 0); // Start at top-left corner
    display.println("AVARIE!");
    display.setTextSize(2);
    display.setCursor(20, 15); // Start at top-left corner
    display.println("SENZORI");
    display.display();
    ecran = STOP;

    break;
  case CURATARE_SUS:
    if (timerDisplay > 100)
    {
      Serial.println("ECRAN: CURATARE_MANUALA ");
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(20, 0); // Start at top-left corner
      display.println("- MOTOR MIC ON -");
      display.setTextSize(2);
      display.drawLine(90, 10, 99, 15, SSD1306_WHITE);
      display.drawLine(89, 10, 98, 15, SSD1306_WHITE);
      display.drawLine(90, 10, 81, 15, SSD1306_WHITE);
      display.drawLine(89, 10, 80, 15, SSD1306_WHITE);
      display.drawLine(90, 10, 90, 25, SSD1306_WHITE);
      display.drawLine(89, 10, 89, 25, SSD1306_WHITE);
      display.display();
      timerDisplay = 0;
      ecran = STOP;
    }
    break;
  case CURATARE_JOS:
    if (timerDisplay > 100)
    {
      Serial.println("ECRAN: CURATARE_MANUALA ");
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(20, 0); // Start at top-left corner
      display.println("- MOTOR MARE ON -");
      display.setTextSize(2);
      display.drawLine(35, 25, 44, 20, SSD1306_WHITE);
      display.drawLine(34, 25, 43, 20, SSD1306_WHITE);
      display.drawLine(35, 25, 26, 20, SSD1306_WHITE);
      display.drawLine(34, 25, 25, 20, SSD1306_WHITE);
      display.drawLine(35, 25, 35, 10, SSD1306_WHITE);
      display.drawLine(34, 25, 34, 10, SSD1306_WHITE);
      display.display();
      timerDisplay = 0;
      ecran = STOP;
    }
    break;
  case AUTOMAT:
    if (timerDisplay > 100)
    {
      Serial.println("ECRAN: AUTOMAT ");

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(20, 0);
      display.println("Status :");
      display.setCursor(70, 0);
      display.println(" Automat");
      display.setCursor(30, 10);
      display.println("Urm curatare: ");
      display.setCursor(15, 20);
      display.println(h);
      display.setCursor(35, 20);
      display.println("h");
      display.setCursor(55, 20);
      display.println(m);
      display.setCursor(75, 20);
      display.println("m");
      display.setCursor(95, 20);
      display.println(s);
      display.setCursor(115, 20);
      display.println("s");
      display.display();
      timerDisplay = 0;
      ecran = STOP;
    }
    break;

  case MANUAL:
    if (timerDisplay > 100)
    {
      Serial.println("ECRAN: MANUAL ");

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(20, 0);
      display.println("Status :");
      display.setCursor(70, 0);
      display.println(" Manual");
      display.display();
      timerDisplay = 0;
      ecran = STOP;
    }
    break;

  case SETARI:
    if (timerDisplay > 100)
    {
      Serial.println("ECRAN: SETARI");
      display.clearDisplay();
      display.setTextSize(1);              // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.setCursor(20, 0);            // Start at top-left corner

      display.println("Setare interval :");
      display.setTextSize(2);
      if (nrOre > 9)
      {
        display.setCursor(51, 12);
      }
      else
      {
        display.setCursor(58, 12);
      }
      display.println(nrOre);
      display.drawLine(35, 25, 44, 20, SSD1306_WHITE);
      display.drawLine(34, 25, 43, 20, SSD1306_WHITE);
      display.drawLine(35, 25, 26, 20, SSD1306_WHITE);
      display.drawLine(34, 25, 25, 20, SSD1306_WHITE);
      display.drawLine(35, 25, 35, 10, SSD1306_WHITE);
      display.drawLine(34, 25, 34, 10, SSD1306_WHITE);

      display.drawLine(90, 10, 99, 15, SSD1306_WHITE);
      display.drawLine(89, 10, 98, 15, SSD1306_WHITE);
      display.drawLine(90, 10, 81, 15, SSD1306_WHITE);
      display.drawLine(89, 10, 80, 15, SSD1306_WHITE);
      display.drawLine(90, 10, 90, 25, SSD1306_WHITE);
      display.drawLine(89, 10, 89, 25, SSD1306_WHITE);
      display.setTextSize(1);
      display.setCursor(103, 17);
      display.println("ore");
      display.display();
      timerDisplay = 0;
      ecran = STOP;
    }
    break;

  case STOP:

    break;
  }
}