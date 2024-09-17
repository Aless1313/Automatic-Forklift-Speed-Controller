#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED 8     // PIN 14 PB0
#define CH1AT 9   // PIN 15 PB1
#define CH2AT 10  // PIN 16 PB2
#define CH3AT 11  // PIN 17 PB3
#define RL1 12    // PIN 18 PB4
#define RL2 13    // PIN 19 PB5
#define POT1AT A0 // PIN 23
#define POT2AT A1 // PIN 24

Adafruit_MCP4725 DAC1;
Adafruit_MCP4725 DAC2;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const float VREF = 5.0;

bool ledState = LOW;                 // Variable para almacenar el estado del LED
unsigned long previousLedMillis = 0; // Almacena el tiempo previo para gestionar el parpadeo
const long ledInterval = 1000;       // Intervalo de parpadeo de 1 segundo

float scaledVoltage1 = 0.0;
float scaledVoltage2 = 0.0;

void setup()
{
  Serial.begin(9600);

  DAC1.begin(0x60);
  DAC2.begin(0x61);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();

  pinMode(LED, OUTPUT);

  pinMode(CH1AT, INPUT);
  pinMode(CH2AT, INPUT);
  pinMode(CH3AT, INPUT);

  pinMode(RL1, OUTPUT);
  pinMode(RL2, OUTPUT);
}

void loop()
{
  int ch1state = digitalRead(CH1AT);
  int ch2state = digitalRead(CH2AT);
  int ch3state = digitalRead(CH3AT);

  int potValue1 = analogRead(POT1AT);
  int potValue2 = analogRead(POT2AT);

  float voltage1 = potValue1 * (VREF / 1023.0);
  float voltage2 = potValue2 * (VREF / 1023.0);

  // Escalar el voltaje siempre, independientemente del estado de ch1state
  scaledVoltage1 = voltage1 / 2.0;
  int dacValue1 = (int)((scaledVoltage1 / VREF) * 4095);

  scaledVoltage2 = 2.5 + (voltage2 / 2.0);
  int dacValue2 = (int)((scaledVoltage2 / VREF) * 4095);

  float outputVoltage1 = (dacValue1 / 4095.0) * VREF;
  float outputVoltage2 = (dacValue2 / 4095.0) * VREF;

  DAC1.setVoltage(dacValue1, false);
  DAC2.setVoltage(dacValue2, false);

  if (ch1state == LOW)
  {
    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, HIGH);
    digitalWrite(LED, HIGH);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("CH1 Activo -Persona");

    display.setCursor(0, 10);
    display.print("Pot1 In: ");
    display.print(voltage1);
    display.print("V");

    display.setCursor(0, 20);
    display.print("Pot2 In: ");
    display.print(voltage2);
    display.print("V");

    display.setCursor(0, 30);
    display.print("DAC1 Out: ");
    display.print(outputVoltage1);
    display.print("V");

    display.setCursor(0, 40);
    display.print("DAC2 Out: ");
    display.print(outputVoltage2);
    display.print("V");

    display.setCursor(0, 50);
    display.print("Persona Cerca");

    display.display();
  }
  else if (ch2state == LOW)
  {
    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, HIGH);
    digitalWrite(LED, HIGH);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("CH2 Activo -Montacargas");

    display.setCursor(0, 10);
    display.print("Pot1 In: ");
    display.print(voltage1);
    display.print("V");

    display.setCursor(0, 20);
    display.print("Pot2 In: ");
    display.print(voltage2);
    display.print("V");

    display.setCursor(0, 30);
    display.print("DAC1 Out: ");
    display.print(outputVoltage1);
    display.print("V");

    display.setCursor(0, 40);
    display.print("DAC2 Out: ");
    display.print(outputVoltage2);
    display.print("V");

    display.display();
  }
  else if (ch3state == LOW)
  {
    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, HIGH);
    digitalWrite(LED, HIGH);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("CH3 Activo -Zona");

    display.setCursor(0, 10);
    display.print("Pot1 In: ");
    display.print(voltage1);
    display.print("V");

    display.setCursor(0, 20);
    display.print("Pot2 In: ");
    display.print(voltage2);
    display.print("V");

    display.setCursor(0, 30);
    display.print("DAC1 Out: ");
    display.print(outputVoltage1);
    display.print("V");

    display.setCursor(0, 40);
    display.print("DAC2 Out: ");
    display.print(outputVoltage2);
    display.print("V");

    display.display();
  }
  else
  {
    digitalWrite(RL1, LOW);
    digitalWrite(RL2, LOW);

    // Control del parpadeo del LED
    unsigned long currentMillis = millis();
    if (currentMillis - previousLedMillis >= ledInterval)
    {
      // Cambiar el estado del LED
      ledState = !ledState;
      digitalWrite(LED, ledState);

      // Guardar el tiempo actual
      previousLedMillis = currentMillis;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("Unlimited");

    display.display();
  }

  delay(100); // Mantén este pequeño retraso para evitar desbordar el buffer serie
}
