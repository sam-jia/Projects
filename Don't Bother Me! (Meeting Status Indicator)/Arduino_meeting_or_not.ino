int incomingByte = 0;   // for incoming serial data

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);   
  pinMode(2, OUTPUT);
  Serial.begin(115200);           //  setup serial
      digitalWrite(13, LOW);
    digitalWrite(2, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
//  digitalWrite(13, HIGH);
//  delay(1000);
//  digitalWrite(13, LOW);
//  digitalWrite(2, HIGH);
//  delay(1000);
//  digitalWrite(2, LOW);

          if (Serial.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial.read();

                // say what you got:
//                Serial.print("I received: ");
//                Serial.println(incomingByte, DEC);
        }
  switch (incomingByte)
  {
    case 48:
      digitalWrite(13, HIGH);
      digitalWrite(2, LOW);
      //Serial.println(incomingByte, DEC);
      break;
    case 49:
      digitalWrite(13, LOW);
      digitalWrite(2, HIGH);
      //Serial.println(incomingByte, DEC);
      break;
  }
}
