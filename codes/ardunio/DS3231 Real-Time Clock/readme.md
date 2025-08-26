# DS3231 Real-Time Clock with Arduino

The DS3231 is a highly accurate real-time clock (RTC) module that works well with Arduino. Here's a comprehensive guide to using it:
## Wiring Connections

|DS3231 Pin|Arduino Pin|
|---|---|
| VCC  |  5V   |
| GND  |  GND  |
| SDA  |  A4   |
| SCL  |  A5   |

## Required Libraries

```cpp

    #include <Wire.h>
    #include <RTClib.h>
    
```

