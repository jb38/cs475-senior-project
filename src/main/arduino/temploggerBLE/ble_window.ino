/*

The MIT License (MIT)

Copyright (c) 2015 Jim Blaney

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <RTC_DS3231.h>

bool shouldBleBeEnabled(DateTime dt) {

  // off on weekends
  int weekday = dt.dayOfWeek();  
  if (weekday == 0 || weekday == 6) {
    return false; 
  }
  
  // off between 1730 and 0900 (local)
  int hour = (24 + dt.hour() + TIME_ZONE_OFFSET_HOURS) % 24;
  int minutes = hour * 60 + dt.minute();
  if (540 <= minutes && minutes <= 1050) {
    return (minutes % 10 == 0); // only on when the minute ends with '0'
  }
  
  // off if no positive condition is satisfied
  return false; 
}

