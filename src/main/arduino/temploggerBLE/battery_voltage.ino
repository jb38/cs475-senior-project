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

// http://forum.rfduino.com/index.php?topic=265.0
float getBatteryVoltage() {
  
  analogReference(VBG); // Sets the Reference to 1.2V band gap           
  analogSelection(VDD_1_3_PS);  //Selects VDD with 1/3 prescaling as the analog source
  
  int sensorValue = analogRead(1); // the pin has no meaning, it uses VDD pin
  float batteryVoltage = sensorValue * (3.6 / 1023.0); // convert value to voltage
  
  analogReference(DEFAULT); // switch back to default reference
  
  return batteryVoltage;
}

