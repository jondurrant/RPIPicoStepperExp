# RPIPicoStepperExp
Stepper Motor Experiments with 28BYJ-48 and ULN2003 driver from the Raspberrry PI Pico.

Three example used in video on my [youtube channel](https://www.youtube.com/@drjonea)

## 28byj-48
Simple C example of driving the 28-BYJ-48. 

ULN2003 driver connected on GP2 to 5. LED on GP0

## FreeRTOS-28byt-48
Driving the 28-BYJ-48 from a FreeRTOS task. The interfacing to the task through a queue. All encapsulated in an agent C++ class.

ULN2003 driver connected on GP2 to 5. LED on GP0

## Demo-27byj-48

Demo build of a test framework for the stepper motor. A 3D printed mount with optical slot detector and LED ring. Enable monitoring of accuracy by watching the disk rotate from RED LED to Green LED through yellow LEDs.

# Further Information
For more Pico Examples see [drjonea.co.uk/IoT](https://drjonea.co.uk/IoT)