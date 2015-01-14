# Power Budget

## Formulae
```
battery_life    = battery_capacity / average_current

average_current = (busy_current * busy_period + idle_current * idle_period) / (busy_period + idle_period)
                = (8.10 * 1 + 0.45 * (interval_min * 60) - 1) / (interval_min * 60)
```

## Performance
```
busy_current = 8.10mA
busy_period  = 1s
idle_current = 0.45mA
idle_period  = (interval_min * 60 - busy_period)

battery_life varies about battery_capacity (refer to table above)
```

## Data




## Tables & Plots

### Current Consumption (mAh)

Measurement<br>Interval<br>(minutes)|Average<br>Current<br>(mAh)
--:|--:
1|0.577500
5|0.475500
10|0.462750
15|0.458500
20|0.456375
30|0.454250
45|0.452830
60|0.452125

![alt text](http://i.imgur.com/oXNU2Rt.png "Current Consumption")

### Battery Life (days)

Measurement<br>Interval<br>(minutes)|Profile A<br>6 x AA<br>(4.5v,3600mAh)|Profile B<br>3 x AA<br>(4.5v,1800mAh)|Profile C<br>6 x AAA<br>(4.5v,1720mAh)|Profile D<br>3 x AAA<br>(4.5v,860mAh)|Profile E<br>1 x 9V<br>(9v,565mAh)
--:|--:|--:|--:|--:|--:
1|259|129|124|62|40
5|315|157|150|75|49
10|324|162|154|77|50
15|327|163|156|78|51
20|328|164|157|78|51
30|330|165|157|78|51
45|331|165|158|79|51
60|331|165|158|79|52

![alt text](http://i.imgur.com/v7cFTNq.png "Battery Life")

### Operating Cost (per Day)

Measurement Interval (min)|Profile A<br><br>6 x AA (4.5v, 3600mAh)|3 x AA (4.5v, 1800mAh)|6 x AAA (4.5v, 1720mAh)|3 x AAA (4.5v, 860mAh)|1 x 9V (9v, 565mAh)
--:|--:|--:|--:|--:|--:
1|$0.02 |$0.02 |$0.03 |$0.03 |$0.07 
5|$0.01 |$0.01 |$0.03 |$0.03 |$0.05 
10|$0.01 |$0.01 |$0.03 |$0.03 |$0.05 
15|$0.01 |$0.01 |$0.03 |$0.03 |$0.05 
20|$0.01 |$0.01 |$0.03 |$0.03 |$0.05 
30|$0.01 |$0.01 |$0.03 |$0.03 |$0.05 
45|$0.01 |$0.01 |$0.03 |$0.03 |$0.05 
60|$0.01 |$0.01 |$0.03 |$0.03 |$0.05 

![alt text](http://i.imgur.com/qOn5xVz.png "Operating Cost")

