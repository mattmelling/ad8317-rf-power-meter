#include <Arduino.h>

#define ledPin PC13

#define ADC0 0

#define VREF 1200
#define ADCH 4096

int lastAdcVal = 0;
byte command = NULL;

double adc2mv(int adc) {
  return ((1.0 * adc) / (1.0 * ADCH)) * VREF;
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin,OUTPUT);
  analogReadResolution(12);
  digitalWrite(ledPin, LOW);
}

void loop() {
  lastAdcVal = analogRead(ADC0);

  if(Serial.available() > 0) {
    digitalWrite(ledPin, HIGH);
    command = Serial.read();
    switch(command) {
    case 'm':
      // measure
      Serial.print(lastAdcVal);
      // Serial.print(",");
      // Serial.print(adc2mv(lastAdcVal));
      // Serial.print("mV");
      Serial.print("\r\n");
      break;
    default:
      break;
    }
    digitalWrite(ledPin, LOW);
  }
}
