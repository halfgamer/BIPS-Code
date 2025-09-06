#define Dig_pin 7  // Input pin for the DO pin of the sensor

int Dig_out = LOW;           
int Ana_out = 0;

int consecutiveMaxReadings = 0;
bool hitDetected = false;

bool cooldownActive = false;            // Flag to track if cooldown is active
unsigned long cooldownStartTime = 0;    // Time when cooldown started
const unsigned long cooldownDuration = 5000;  // 5 seconds in milliseconds

void setup() {
  Serial.begin(9600);
  pinMode(Dig_pin, INPUT);
}

void loop() {
  Dig_out = digitalRead(Dig_pin);
  Ana_out = analogRead(A0);

  unsigned long currentTime = millis();

  // Check if cooldown is active
  if (cooldownActive) {
    if (currentTime - cooldownStartTime >= cooldownDuration) {
      cooldownActive = false;  // Cooldown expired
    } else {
      return;  // Skip processing if in cooldown
    }
  }

  if (Dig_out == LOW) {  // Tap or vibration detected
    if (Ana_out == 1023) {
      consecutiveMaxReadings++;
      hitDetected = true;
    } else if (hitDetected) {
      // Evaluate the hit after 1023 streak ends
      if (consecutiveMaxReadings == 1) {
        Serial.println("Hit!");
      } else if (consecutiveMaxReadings == 2) {
        Serial.println("Hard Hit!");
      } else if (consecutiveMaxReadings >= 3) {
        Serial.println("Oh! He's knocked out!");
      }

      // Start cooldown
      cooldownActive = true;
      cooldownStartTime = currentTime;

      // Reset hit tracking
      consecutiveMaxReadings = 0;
      hitDetected = false;
    }

    delay(50);  // Small delay for stability
  } else {
    // Reset tracking if no hit is currently being built
    consecutiveMaxReadings = 0;
    hitDetected = false;
  }
}
