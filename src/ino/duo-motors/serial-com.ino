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

    // Get device Information
    // C0 |
    // command |
    if(cmd =="C0"){
      println("Version 0.0.0");
    }

    // Calibrate
    // C1 | lll LLL rrr RRR
    // command | left_min (3 digidt) left_max(3 digits) right_min (3 digits) right_max(3 digits))
    else if(cmd == "C1"){
        println("Calibrate");
        //todo: implement calibration
    }

    // Motion Forward
    // C2 | 11 11
    // command | left_motion(2 digits) right_motion(2digits)
    else if(cmd == "C2"){
      motion_direction = 0;
      motion_left  = char2int(data[0])*10 + char2int(data[1]);
      motion_right = char2int(data[2])*10 + char2int(data[3]);
    }

    // Motion Back
    // C3 | 11 11
    // command | left_motion(2 digits) right_motion(2digits)
    else if(cmd == "C3"){
      motion_direction = 1;
      motion_left  = char2int(data[0])*10 + char2int(data[1]);
      motion_right = char2int(data[2])*10 + char2int(data[3]);
    }

    // Stop
    // C4 |
    // command | 
    else if(cmd == "C4"){
      motion_direction = 0;
      motion_left  = 0;
      motion_right = 0;
    }

  }
}

int char2int(char a){
  return (int)a - 48;
}
