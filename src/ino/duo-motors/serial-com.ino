String cmd;
String data;


void listenToSerial(){

  delay(500);
  Serial.println("Listening...");

  if(Serial.available() > 0 ){

    cmd = Serial.readStringUntil('|');
    data = Serial.readString();

    Serial.print("command:");
    Serial.print(cmd);
    Serial.print(", data:");
    Serial.print(data);


    // Motion Forward
    // C1 | 11 11
    // command | left_motion(2 digits) right_motion(2digits)
    if(cmd == "C1"){
      // Serial.print("|Motion Forward|");
      motion_left  = char2int(data[0])*10 + char2int(data[1]);
      motion_right = char2int(data[2])*10 + char2int(data[3]);
      // Serial.print(motion_left);
      // Serial.print("\t");
      // Serial.println(motion_right);
    }

  }
}

int char2int(char a){
  return (int)a - 48;
}
