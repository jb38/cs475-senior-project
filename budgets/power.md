# Power Budget

1.5v AAA Alkaline = 860mAh  
1.5v AA  Alkaline = 1800mAh
9v 9V Alkaline = 565mAh

|Reading Interval|Average Current|3 x AAA (4.5v, 860mAh)|6 x AAA (4.5v, 1720mAh)|3 x AA (4.5v, 1800mAh)|6 x AA (4.5v, 3600mAh)|1 x 9V (9v, 565mAh)|
|--:|--:|--:|--:|--:|--:|--:|
|1 min|0.5775mAh|62 days|124 days|130 days|260 days||
|5 min|0.4755mAh|75 days|151 days|158 days|315 days||
|10 min|0.46275mAh|77 days|155 days|162 days|324 days||
|15 min|0.4585mAh|78 days|156 days|164 days|327 days||
|20 min|0.456375mAh|79 days|157 days|164 days|329 days||
|30 min|0.45425mAh|79 days|158 days|165 days|330 days||
|45 min|0.45283mAh|79 days|158 days|166 days|331 days||
|60 min|0.452125mAh|79 days|159 days|166 days|332 days||

Formula:
```
battery_life = battery_capacity / average_current
```

Performance:
```
busy_current = 8.10mA
busy_period  = 1s
idle_current = 0.45mA
idle_period  = (interval_min * 60 - busy_period)

average_current = (busy_current * busy_period + idle_current * idle_period) / (busy_period + idle_period)
average_current = (8.10 * 1 + 0.45 * (interval_min * 60) - 1) / (interval_min * 60)

battery_life varies about battery_capacity (refer to table above)
```
