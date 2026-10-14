int ir = 8;
int led = 9;

void setup() {
  pinMode(ir, INPUT);
  pinMode(led, OUTPUT);
}

void loop() {
  if(digitalRead(ir) == LOW){
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}
