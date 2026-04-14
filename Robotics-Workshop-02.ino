
int potPin = 34; // Potentiometer middle pin
int ledPin = 25; // LED pin

int potValue = 0; // store pot value

void setup() {
    pinMode(ledPin, OUTPUT);
    Serial.begin(115200);
}

void loop() {
// Read potentiometer (0 - 4095)
potValue = analogRead(potPin);

// Convert to LED brightness (0 - 255)
int brightness = map(potValue, 0, 4095, 0, 255);

// Set LED brightness
analogWrite(ledPin, brightness);

// Print value (optional)
Serial.println(brightness);
delay(10);
}