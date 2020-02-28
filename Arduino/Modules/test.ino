
void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  digitalWrite(3, 0);
  digitalWrite(4, 0);
  digitalWrite(5, 0);
}
void loop() {
   // digitalWrite(3, !digitalRead(3));

    delay(1000);
 }