int ldr = 2;
int led = 9;

void setup() {
  	pinMode(led, OUTPUT); 
pinMode(ldr, INPUT);
}

void loop() {
  int val = digitalRead(ldr);
  if(val == 500){
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}
