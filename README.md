# Robot-car-control-STM32

## Description
The project has been developed as part of the EIE3105 course assignment taken at PolyU in my final year with Dr Lawrence.
I had 2 tasks: 
- Task1: make the car move in different directions within 1 min: forward, backward, forward-right, forward-left, backward-right, backward-left
- Task2: make the car to follow a specified path. Accuracy and Speed are important factors. 

#### Hardware 
- Microcontroller: STM32F103RB 
- Infrared Floor sensor with lighting board: 74HC299
- 2 Motor drivers with wheel revolution counters 
- power supply / batteries 
- cable to connect the board to computer
- *Note: The body of the car with its connections has already been built and prepared. I didn't construct the robot myself. The car is the property of the University.* 
- ![image of car](/images/car.jpg)
- Pin configurations used in this project:
- ![pin layout](/images/pin_diagram.png)

#### Software
- Keil Uvision 5 IDE 
- no HAL libraries used 

## Task 1
The demonstration video for the reference can be found [here](https://youtu.be/Gmcn4qOGeh0).
<br/> Main ARM concepts used: counter, PWM, timer

## Task2
The path for this task is as indicated in this diagram:
![arena](/images/path_arena.png)
<br/>The demonstration video for the reference can be found [here](https://youtu.be/g4ZNgE-4EPs).
<br/>Main ARM concepts used: SPI communication, USART for debugging, interrupt

#### Further information
**Detailed description of the logic applied while approaching the tasks can be found in this [brief project report](project_report.pdf)**
