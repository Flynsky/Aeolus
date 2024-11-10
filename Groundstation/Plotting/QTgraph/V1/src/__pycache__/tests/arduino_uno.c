void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

struct packet{
  unsigned long sine;
  float a;
} packet_test;

union package_to_bytes {
  struct packet p;
  uint8_t b[sizeof(struct packet)];
} ptb;

void loop() {
  // put your main code here, to run repeatedly:
  packet_test.sine = (unsigned long)((sin((float)millis()/100.0)+1)*100.0);
  memcpy(&ptb, &packet_test, sizeof(struct packet));

  packet_test.a = 5.0;
  
  Serial.write(ptb.b,sizeof(struct packet));
  Serial.flush();
  // Serial.println((unsigned long)((sin((float)millis()/1000.0)+1)*100.0));
  delay(10);
}

