# RPI-Arduino communication protocol

Using TLV-BER

# Commands

## 0xC1: getDeviceInfo
- Lenght: 0 bytes
- Value: N/A

```
  C1 00 00
  ## Returns specs from device
  ##  {
  ##  version: 1.1.0
  ## }
```

## 0xC2: calibrate
tbd

## 0xC3: motion
- Lenght: 2 bytes
- Value: (first 1 byte left motor, last 2 byte right motor)

```
  C3 02 00 00
  ## Stops motion

  C3 02 64 64
  

```
