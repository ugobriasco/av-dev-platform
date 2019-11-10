# RPI-Arduino serial communication protocol

Version 1.0
Applicable models: turtle-s

## C0 - Get Device Information

## C1 - Calibrate motors


## C2 - Go Forwards

Format: C2|LLRR
Where:
C2| - Command key
LL - Left wheel: 2 digits natural number from 00 to 99
RR - Right wheel: 2 digits natural number from 00 to 99

Examples:
```
  C29999
  # Go straight forward at max speed (99%)

  C25099
  # Turn left by a factor 1/2

  C29950
  # Turn right by a factor 1/2
```

## C3 - Go Backwards

Format: C3|LLRR
Where:
C2| - Command key
LL - Left wheel: 2 digits natural number from 00 to 99
RR - Right wheel: 2 digits natural number from 00 to 99

Examples:
```
  C29999
  # Go straight backwards at max speed (99%)

  C25099
  # Turn left by a factor 1/2

  C29950
  # Turn right by a factor 1/2
```

## C4 - Stop

Format: C4|
Where:
C4| - Command key


Examples:
```
  C4|
  # Stop

```
