#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

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
void setDACValues(float voltage1, float voltage2, float maxVoltage1, float maxVoltage2);

unsigned long previousMillis = 0;
const long interval = 500; // Intervalo de .5 segundo
bool ledState = LOW;

int ch1state;
int ch2state;
int ch3state;

void setup()
{

  // Usamos el Serial para Bluetooth ya que está conectado a RX(0) y TX(1)
  Serial.begin(9600); // Configuración del Bluetooth

  DAC1.begin(0x60);
  DAC2.begin(0x61);


  pinMode(LED, OUTPUT);

  pinMode(CH1AT, INPUT);
  pinMode(CH2AT, INPUT);
  pinMode(CH3AT, INPUT);

  pinMode(RL1, OUTPUT);
  pinMode(RL2, OUTPUT);

  digitalWrite(ch1state, LOW);
  digitalWrite(ch1state, LOW);
  digitalWrite(ch1state, LOW);
  delay(100);
  digitalWrite(ch1state, HIGH);
  digitalWrite(ch1state, HIGH);
  digitalWrite(ch1state, HIGH);

  // Iniciar con los relés en LOW para asegurarse de que están en NC
  digitalWrite(RL1, HIGH);
  delay(50); // Tiempo muerto entre la activación de cada relé
  digitalWrite(RL2, HIGH);
  delay(50);
  digitalWrite(RL1, LOW);
  delay(50); // Tiempo muerto entre la activación de cada relé
  digitalWrite(RL2, LOW);
  delay(50); // Tiempo muerto entre la activación de cada relé
  digitalWrite(RL1, HIGH);
  delay(50); // Tiempo muerto entre la activación de cada relé
  digitalWrite(RL2, HIGH);
  delay(50); // Tiempo muerto entre la activación de cada relé
  digitalWrite(RL1, LOW);
  delay(50); // Tiempo muerto entre la activación de cada relé
  digitalWrite(RL2, LOW);
  delay(50); // Tiempo muerto entre la activación de cada relé
    // Puedes también forzar a cero el DAC si es necesario:
  DAC1.setVoltage(0, false);
  DAC2.setVoltage(0, false);

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

  potValue1input = analogRead(POT1INPUT);
  potValue2input = analogRead(POT2INPUT);

  voltage1input = potValue1input * (VREF / 1023.0);
  voltage2input = potValue2input * (VREF / 1023.0);

  if (ch1state == LOW)
  {
    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, HIGH);
    digitalWrite(LED, HIGH);

    // Actuar si el voltaje de los potenciómetros sobrepasa el umbral de 1.55V y 1.77V
    if (voltage1 > 2.50 || voltage2 > 2.72) {
      setDACValues(voltage1, voltage2, 2.50, 2.72); // Limitar el valor máximo del DAC
    } else {
      setDACValues(voltage1, voltage2, voltage1, voltage2); // Pasar directamente los valores del potenciómetro al DAC
    }

    displayData("CH1 Activo - Peaton", voltage1, voltage2, voltage1input, voltage2input);
  }
  else if (ch2state == LOW)
  {
    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, HIGH);
    digitalWrite(LED, HIGH);

    // Actuar en base a otro umbral para CH2
    setDACValues(voltage1, voltage2, 1.25, 3.75);

    displayData("CH2 Activo - Monta", voltage1, voltage2, voltage1input, voltage2input);
  }
  else if (ch3state == LOW)
  {
    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, HIGH);
    digitalWrite(LED, HIGH);

    // Actuar en base a otro umbral para CH3
    setDACValues(voltage1, voltage2, 3.75, 1.25);

    displayData("CH3 Activo - Zona", voltage1, voltage2, voltage1input, voltage2input);
  }
  else
  {
    digitalWrite(RL1, LOW);
    digitalWrite(RL2, LOW);
    digitalWrite(LED, HIGH);
    unsigned long currentMillis = millis();

    // Puedes también forzar a cero el DAC si es necesario:
    DAC1.setVoltage(0, false);
    DAC2.setVoltage(0, false);

    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      ledState = !ledState; // Cambiar el estado del LED
      digitalWrite(LED, ledState);
    }

    displayData("Sin Limite", voltage1input, voltage2input, voltage1input, voltage2input);
  }

  delay(500);
}

void setDACValues(float voltage1, float voltage2, float maxVoltage1, float maxVoltage2)
{
  // Limitar el voltaje solo si sobrepasa los máximos permitidos
  float scaledVoltage1 = (voltage1 > maxVoltage1) ? maxVoltage1 : voltage1;
  float scaledVoltage2 = (voltage2 > maxVoltage2) ? maxVoltage2 : voltage2;

  int dacValue1 = (int)((scaledVoltage1 / VREF) * 4095); // Escalar para el DAC
  int dacValue2 = (int)((scaledVoltage2 / VREF) * 4095); // Escalar para el DAC

  DAC1.setVoltage(dacValue1, false);
  DAC2.setVoltage(dacValue2, false);
}


void displayData(const char *message, float voltage1, float voltage2, float outputVoltage1, float outputVoltage2)
{
  Serial.println(message);
  Serial.print("Pot1 In: ");
  Serial.print(voltage1);
  Serial.println("V");

  Serial.print("Pot2 In: ");
  Serial.print(voltage2);
  Serial.println("V");

  Serial.print("DAC1 Out: ");
  Serial.print(outputVoltage1);
  Serial.println("V");

  Serial.print("DAC2 Out: ");
  Serial.print(outputVoltage2);
  Serial.println("V");

  Serial.println("-----------------------------");
}
