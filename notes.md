1. monetary budget with no soldering **DONE**
1. cadence/size calculation (to determine the required storage capacity) **DONE**
1. power budget calculation
1. option to add wireless (bluetooth or wifi) -- include in budget **DONE**
1. additional source items:
  1. usb sd card reader (for retrieval)
  1. thermometer (for calibration)
1. software to read
1. method to receive data
1. display/analysis of data
1. calibration study for sensor
1. cold-approach adafruit about packaging/soldering

dc current sensor
http://www.adafruit.com/product/904

### Data Logging Budget

```
2014-12-09T14:23:35Z -12.345
12345678901234567890123456789
```

29 bytes/observation * 1440 observations/day = 41760 bytes / day

1GB = 1073741824 bytes ÷ 41760 bytes/day ~= ~25k days (~70 years)
2GB = 2147483648 bytes ÷ 41760 bytes/day ~= ~50k days (~140 years)

### Power Budget

9v Alkaline = 565 mAh
1.5v AA Alkaline = 2200 mAh

From digikey:
```
Battery Life = Battery Capacity in mA per hour / Load Current in mA * 0.70
```

Test start: 1029


From somewhere else:
```
Average current = (10mA \* 0.5s + 16uA \* 10s) / 10.5s = 0.49mA
Battery time = 450mAh/0.49mA = 918 hrs = at least 38 days
```
