int fire = 8;
int buzzer = 9;

void setup() {
  pinMode(fire, INPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  if(digitalRead(fire) == HIGH){
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
  }
}
