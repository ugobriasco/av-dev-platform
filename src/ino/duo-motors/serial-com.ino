String cmd;
int servo_id;
int value;


void listenToSerial(){

  delay(3000);
  Serial.println("Listening...");

  if(Serial.available() > 0 ){

    for(int i=0; i<2; i++) {
      cmd = Serial.readStringUntil(';');
      servo_id = char2int(cmd[0]);
      value = char2int(cmd[2])*10 + char2int(cmd[3]);

      Serial.print("-->");
      Serial.print(cmd);
      Serial.print(" servo_id: ");
      Serial.print(servo_id);
      Serial.print(" value: ");
      Serial.println(value);
    }


  }
}

int char2int(char a){
  return (int)a - 48;
}
