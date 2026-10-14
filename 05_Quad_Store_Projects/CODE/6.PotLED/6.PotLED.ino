int potPin = A0;
int led = 9;
int val = 0;

void setup() {
  pinMode(led, OUTPUT);
}

void loop() {
  val = analogRead(potPin);
  val = map(val, 0, 1023, 0, 255);
  analogWrite(led, val);
}
