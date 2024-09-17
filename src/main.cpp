#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define LED 8    // PIN 14 PB0
#define CH1AT 9  // PIN 15 PB1
#define CH2AT 10 // PIN 16 PB2
#define CH3AT 11 // PIN 17 PB3
#define RL1 12   // PIN 18 PB4
#define RL2 13   // PIN 19 PB5

#define POT1AT A0    // PIN 23
#define POT2AT A1    // PIN 24
#define POT1INPUT A3 // PIN
#define POT2INPUT A2 // PIN

Adafruit_MCP4725 DAC1;
Adafruit_MCP4725 DAC2;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const float VREF = 5.0;

float potValue1;
float potValue2;
float potValue1input;
float potValue2input;
float voltage1;
float voltage2;
float voltage1input;
float voltage2input;

void displayData(const char *message, float voltage1, float voltage2, float outputVoltage1, float outputVoltage2);

unsigned long previousMillis = 0;
const long interval = 1000; // Intervalo de 1 segundo
bool ledState = LOW;

SoftwareSerial BTSerial(2, 3); // RX, TX

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

    digitalWrite(RL1, HIGH);
  digitalWrite(RL2, HIGH);
  delay(1000);
  digitalWrite(RL1, LOW);
  digitalWrite(RL2, LOW);

  // BTSerial.begin(9600); // Configuración del Bluetooth
}

void loop()
{
  int ch1state = digitalRead(CH1AT);
  int ch2state = digitalRead(CH2AT);
  int ch3state = digitalRead(CH3AT);

  potValue1 = analogRead(POT1AT);
  potValue2 = analogRead(POT2AT);

  potValue1input = analogRead(POT1INPUT);
  potValue2input = analogRead(POT2INPUT);

  voltage1 = potValue1 * (VREF / 1023.0);
  voltage2 = potValue2 * (VREF / 1023.0);

  voltage1input = potValue1input * (VREF / 1023.0);
  voltage2input = potValue2input * (VREF / 1023.0);

  if (ch1state == LOW)
  {
    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, HIGH);
    digitalWrite(LED, HIGH);

    // 1/2 Velocidad (limitar si se supera el umbral de 2.5V para voltage1 y si es menor a 2.5V para voltage2)
    float scaledVoltage1 = (voltage1 > 3.0) ? 3.0 : voltage1;
    float scaledVoltage2 = (voltage2 > 2.50) ? 2.50 : voltage2;

    // Convertir los voltajes escalados a vlores de DAC (0-4095)
    int dacValue1 = (int)((scaledVoltage1 / VREF) * 4095);
    int dacValue2 = (int)((scaledVoltage2 / VREF) * 4095);

    DAC1.setVoltage(dacValue1, false);
    DAC2.setVoltage(dacValue2, false);

    displayData("CH1 Activo - Peaton", voltage1, voltage2, scaledVoltage1, scaledVoltage2);
  }
  else if (ch2state == LOW)
  {

    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, HIGH);
    digitalWrite(LED, HIGH);

    // 1/4 Velocidad (limitar si se supera el umbral de 1.25V para voltage1 y si es menor a 3.75V para voltage2)
    float scaledVoltage1 = (voltage1 > 3.0) ? 3.0 : voltage1;
    float scaledVoltage2 = (voltage2 > 2.50) ? 2.50 : voltage2;

    int dacValue1 = (int)((scaledVoltage1 / VREF) * 4095);
    int dacValue2 = (int)((scaledVoltage2 / VREF) * 4095);

    DAC1.setVoltage(dacValue1, false);
    DAC2.setVoltage(dacValue2, false);

    displayData("CH2 Activo - Monta", voltage1, voltage2, scaledVoltage1, scaledVoltage2);
  }
  else if (ch3state == LOW)
  {

    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, HIGH);
    digitalWrite(LED, HIGH);

    // 3/4 Velocidad (limitar si se supera el umbral de 3.75V para voltage1 y si es menor a 1.25V para voltage2)
    float scaledVoltage1 = (voltage1 > 3.0) ? 3.0 : voltage1;
    float scaledVoltage2 = (voltage2 > 2.50) ? 2.50 : voltage2;

    int dacValue1 = (int)((scaledVoltage1 / VREF) * 4095);
    int dacValue2 = (int)((scaledVoltage2 / VREF) * 4095);

    DAC1.setVoltage(dacValue1, false);
    DAC2.setVoltage(dacValue2, false);

    displayData("CH3 Activo - Zona", voltage1, voltage2, scaledVoltage1, scaledVoltage2);
  }
  else
  {
    digitalWrite(RL1, LOW);
    digitalWrite(RL2, LOW);

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      ledState = !ledState; // Cambiar el estado del LED
      digitalWrite(LED, ledState);
    }

    displayData("Sin Limite", voltage1input, voltage2input, voltage1input, voltage2input);
  }

  delay(100);
}

void displayData(const char *message, float voltage1, float voltage2, float outputVoltage1, float outputVoltage2)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print(message);

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
  /*
    BTSerial.println(message);
    BTSerial.print("Pot1 In: ");
    BTSerial.print(voltage1);
    BTSerial.println("V");

    BTSerial.print("Pot2 In: ");
    BTSerial.print(voltage2);
    BTSerial.println("V");

    BTSerial.print("DAC1 Out: ");
    BTSerial.print(outputVoltage1);
    BTSerial.println("V");

    BTSerial.print("DAC2 Out: ");
    BTSerial.print(outputVoltage2);
    BTSerial.println("V");

    BTSerial.println("-----------------------------"); // Separador
    */
}
