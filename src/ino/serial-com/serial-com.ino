char serialData;
int pin = 2;

void setup() {
Serial.begin(9600);
pinMode(pin, OUTPUT);
}

void loop() {

  delay(1000);

  Serial.println("Listening...");


  if(Serial.available() > 0){
    serialData = Serial.read();
    Serial.println(serialData);

    if(serialData == '1'){
      Serial.println("high");
      digitalWrite(pin, HIGH);
    } else if (serialData == '0'){
      Serial.println("low");
      digitalWrite(pin, LOW);

    }

   }
}
