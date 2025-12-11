
## About This Project


This project was created as an educational exploration of CPU-like programmability on embedded hardware.  

The work was completed as part of a **university STM32 project**, where the requirement was to create something non-trivial using the STM32L475VG microcontroller.  
My curiosity about how hardware becomes programmable led me to build a small virtual machine with a compiler and custom instruction set.

My goal was to build a simple game console with two buttons, a joystick, and an [LCD](https://www.adafruit.com/product/4694) screen, along with an assembly-like language that can be used to write games compiling to my instruction set.

## Implementation 
I wanted to keep the compiler as simple as possible since the university project was meant to focus on the embedded code and I had limited time.
To make compilation easier I chose a 16 bit instruction set which mostly shares the same format:
> [opcode:4][arg1_type:1][arg1_value:5][arg2_type:1][arg2_value:5] 

Only exceptions are `JMP`, `JZ`, `LBI`, they take only one argument from the next 16 bits
> [opcode:4][dummy_data:12][arg1:16]


### Opcodes / Syntax

Register values are prefixed with `R`. For arithmetic operations, results are always written into `R0`.
- **Arithmetic**
  - `ADD <register/immediate> <register/immediate>` – adds two values and stores the result in `R0`
  - `SUB <register/immediate> <register/immediate>` – subtracts second value from first, stores result in `R0`
  - `MLT <register/immediate> <register/immediate>` – multiplies two values, stores result in `R0`

- **Jumps**
  - `JMP <label>` – jump to label
  - `JZ <label>` – jump to label if `R0` is zero
  - `HALT` – stops the virtual machine; required at the end of a program

- **Moving values**
  - `MOV <register> <immediate>` – move an immediate value into a register
  - `LBI <register> <immediate>` – load a big integer into a register

- **Graphics**
  - `BTN <register> <button_number>` – writes 1 into register if pressed, 0 if not
  - `JOY <register>` – writes a value from 0–8 depending on joystick direction
  - `PIX <register/immediate> <register/immediate>` – pushes a pixel onto the pixel stack
  - `LINE <0 or 1>` – pops two pixels from the stack and draws a line between them (0 = black, 1 = white)
  - `RECT <0 or 1>` – pops two pixels from the stack and draws a rectangle
  - `CLS` – clears the screen
  - `DRAW` – writes the frame buffer to the LCD screen
  - `WAIT <register/immediate>` – delays in milliseconds (maximum 1000 ms)

See file `compiler/demo.in` for an example.
