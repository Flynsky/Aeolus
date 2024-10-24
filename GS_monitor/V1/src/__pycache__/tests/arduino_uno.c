void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

union long_to_bytes {
  unsigned long i;
  uint8_t b[4];
} ltb;

void loop() {
  // put your main code here, to run repeatedly:
  ltb.i = (unsigned long)((sin((float)millis()/1000.0)+1)*100.0);
  Serial.write(ltb.b,4);
  Serial.flush();
  // Serial.println((unsigned long)((sin((float)millis()/1000.0)+1)*100.0));
  delay(100);
}
