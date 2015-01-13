# Power Budget

1.5v AAA Alkaline = 860 mAh  
1.5v AA  Alkaline = 1800 mAh

```
Battery Life = Battery Capacity in mA per hour / Load Current in mA
```
|Cadence (min)|Average Current (mA)|3 x AAA x 1 (days)|3 x AAA x 2 (days)|3 x AA x 1 (days)|3 x AAA x 2 (days)|
|--:|--:|--:|--:|--:|--:|
|1|0.5775|62|124|130|260|
|5|0.4755|75|151|158|315|
|10|0.46275|77|155|162|324|
|15|0.4585|78|156|164|327|
|20|0.456375|79|157|164|329|
|30|0.45425|79|158|165|330|
|45|0.45283|79|158|166|331|
|60|0.452125|79|159|166|332|

```
3 x 1.5V = 4.5v
```

Formula:
```
Idle Current = 0.45mA
Busy Current = 8.10mA
Duty Cycle   = 1s

Hours = c / (0.45 * (n * 60 - 1) + 8.10) / (n * 60), 
        where c is the mAh capacity of the battery 
        and n is the logging cadence
Days = Hours / 24 (rounded)
```
