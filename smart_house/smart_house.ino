#include <DHT.h>
#include <Servo.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735 TFT

const int trigPin = 10; // Ultrasonic sensor trig pin
const int echoPin = 9; // Ultrasonic sensor echo pin
const int servoPin = 6; // Servo motor pin
const int DHTPin = 5; // DHT22 sensor pin
const int BUTTON_PIN = 4; // Touch sensor pin
const int LED_PIN = 12; // LED pin

#define DHTType DHT22
DHT dht(DHTPin, DHTType);

Servo servoMotor;

Adafruit_ST7735 tft = Adafruit_ST7735(11, 7, 8);

struct touch {
  byte wasPressed = LOW;
  byte isPressed = LOW;
};
touch touch;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  servoMotor.attach(servoPin);
  dht.begin();

  tft.initR(INITR_BLACKTAB); // Initialize display using 'initR' with ST7735B chip
  tft.setRotation(3);         // Adjust rotation if needed, depending on your display orientation

  tft.fillScreen(ST7735_BLACK);
}

void loop() {
  long duration, distance;
  float humidity, temperature;

  // Send ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure pulse duration
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance
  distance = duration * 0.034 / 2;

  // Print distance to serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Read temperature and humidity from DHT22 sensor
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Check if any reads failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Clear previous readings
  tft.fillRect(0, 0, tft.width(), 40, ST7735_BLACK);

  // Print temperature and humidity on TFT display
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.setTextColor(ST7735_WHITE);
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.println(" C");

  tft.setCursor(10, 30);
  Serial.print("Humidity: ");
  Serial.print(humidity);
  tft.println(" %");

  // Check touch sensor state
  touch.isPressed = isTouchPressed(BUTTON_PIN);
  if (touch.wasPressed != touch.isPressed) {
    if (touch.isPressed == HIGH) {
      Serial.println("Touch pressed");
      digitalWrite(LED_PIN, HIGH); // Turn on LED when touched
    } else {
      Serial.println("Touch released");
      digitalWrite(LED_PIN, LOW); // Turn off LED when released
    }
  }
  touch.wasPressed = touch.isPressed;

  // If distance is less than 20 cm, open the door
  if (distance < 20) {
    openDoor();
  }
  // If distance is greater than or equal to 20 cm, close the door
  else {
    closeDoor();
  }

  delay(500); // Delay between measurements
}

void openDoor() {
  Serial.println("Opening door");
  servoMotor.write(90); // Move servo to open position
  delay(1000); // Wait for the door to open
}

void closeDoor() {
  Serial.println("Closing door");
  servoMotor.write(0); // Move servo to close position
  delay(1000); // Wait for the door to close
}

bool isTouchPressed(int pin) {
  return digitalRead(pin) == HIGH;
}
