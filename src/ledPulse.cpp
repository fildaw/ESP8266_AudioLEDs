#include <ledPulse.h>

void ledPulse(int speed, bool infinite, int times) {
  if (infinite) {
    while (true) {
      digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on by making the voltage LOW
      delay(speed);                      
      digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
      delay(speed);
    }
  } else {
    for (int i = 0; i < times; i++) {
      digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on by making the voltage LOW
      delay(speed);                      
      digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
      delay(speed);
    }
  }
}