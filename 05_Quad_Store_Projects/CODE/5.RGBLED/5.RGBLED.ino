int red = 9;
int green = 10;
int blue = 11;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
}

void loop() {
  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
  delay(1000);
  
  digitalWrite(red, LOW);
  digitalWrite(green, HIGH);
  delay(1000);
  
  digitalWrite(green, LOW);
  digitalWrite(blue, HIGH);
  delay(1000);
}
