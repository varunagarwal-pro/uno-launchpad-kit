int tilt = 8;
int led = 9;

void setup() {
  pinMode(tilt, INPUT);
  pinMode(led, OUTPUT);
}

void loop() {
  if(digitalRead(tilt) == HIGH){
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}
