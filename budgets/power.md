Formulae:
```
battery_life    = battery_capacity / average_current

average_current = (busy_current * busy_period + idle_current * idle_period) / (busy_period + idle_period)
                = (8.10 * 1 + 0.45 * (interval_min * 60) - 1) / (interval_min * 60)
```

Performance:
```
busy_current = 8.10mA
busy_period  = 1s
idle_current = 0.45mA
idle_period  = (interval_min * 60 - busy_period)

battery_life varies about battery_capacity (refer to table above)
```

# Power Budget

|Reading Interval|Average Current|3 x AAA (4.5v, 860mAh)|6 x AAA (4.5v, 1720mAh)|3 x AA (4.5v, 1800mAh)|6 x AA (4.5v, 3600mAh)|1 x 9V (9v, 565mAh)|
|--:|--:|--:|--:|--:|--:|--:|
| 1 min|0.577500mAh|62 days|124 days|129 days|259 days|40 days|
| 5 min|0.475500mAh|75 days|150 days|157 days|315 days|49 days|
|10 min|0.462750mAh|77 days|154 days|162 days|324 days|50 days|
|15 min|0.458500mAh|78 days|156 days|163 days|327 days|51 days|
|20 min|0.456375mAh|78 days|157 days|164 days|328 days|51 days|
|30 min|0.454250mAh|78 days|157 days|165 days|330 days|51 days|
|45 min|0.452830mAh|79 days|158 days|165 days|331 days|51 days|
|60 min|0.452125mAh|79 days|158 days|165 days|331 days|52 days|

![alt text](http://i.imgur.com/v7cFTNq.png "Battery Life")

![alt text](http://i.imgur.com/qOn5xVz.png "Operating Cost")
