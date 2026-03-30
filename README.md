# PID-Controller-STM32
## Overview

As mentioned in the conclusion of my [Arduino Motor Controller](https://github.com/oscarmoore38/PID-Controller) repo, this is a continuation of that project. If you're evaluating my embedded background and landed here first, I'd recommend going back and checking that out. It was my true entry point into embedded work and will give you more context on what I've built and why, what I've learned, and how I think. It includes test data and a fairly detailed write-up.

So what are my goals here? Well, I like to go deeper on one thing rather than jump around. I still feel there's a lot of ground to cover with this project and I'm not done with it. I've genuinely enjoyed getting my hands on hardware, and I realized from a career perspective, I need work that involves that. This project has also reinforced that I have no interest in web or app development whatsoever. I want something real in front of me that I can probe at and figure out. Honestly, regardless of whether I actually make the pivot into an embedded role or not, I'll keep tinkering with hardware in my own time — it's just plain fun. 

With that said, here's what I want from this project:

---

### 1. Go Deeper With the Hardware

I've started to notice something. I'm reading embedded systems books in my own time, spending evenings and weekends just digging into how things actually work at the hardware level, and I'm genuinely enjoying it. This kind of pull tells me something about where I wanted to go: closer to the hardware, down not up, below the HALs to what's actually happening. Arduino was a great entry point. It let me understand important concepts and get a circuit running quickly. But it also abstracts away a lot of the details I actually want to wrestle with, and often left me feeling "but how does that actually work?" I want to be trawling through datasheets and manuals, scratching my head trying to figure out what on earth is going on at the hardware level. That may sound strange to some, but I genuinely love that state. Also, my goal with this project isn't just to learn, but to simulate what I may actually face as an embedded engineer: here's a board you've never touched, here's the datasheet and manuals, go figure it out and get something built. I'm trying to get comfortable in that space. To accomplish this goal, I'm upgrading my circuit with an STM32 board, which is a technical jump. More discussion on the specfic one I chose, and why, later. 

---

### 2. Introduce RTOS

I don't just want to follow a tutorial, slap RTOS on my resume, and call it done. I want it to make sense for the work I'm doing; to be a logical next step that actually improves what I'm building. That's when learning sticks for me. Two things need to happen first: the circuit needs more complexity, specifically more motors with potentially different timing requirements — that part is done. And I need a board that can actually support something like FreeRTOS. One look at the Nano's datasheet made it clear that isn't it. So STM32 first, then RTOS.

---

### 3. Be More Intentional About Software Design

As mentioned in my last repo, I'm currently reading Making Embedded Systems by Elecia White. I want to start incorporating what I'm learning into this project. I'll get a lot wrong, but I'd rather start trying to adopt practices that experienced people in the field recommend than keep figuring everything out in isolation. You'll notice from my last repo that I built a whole project tightly coupled to a specific board, now that I'm changing boards, a lot of my code across many files needs to change with it. I'm currently on a chapter covering the facade design pattern, which addresses exactly this problem: things change in embedded, frequently, so design for it from the start. More on how I apply this as the project develops.

Let's have some fun! 

## Hardware Design

### Overview 

The core of the circuit stays the same: buck converter, motor driver, OLED, breadboard. The meaningful changes are the MCU and the addition of a second motor.

I actually added the second motor while still on the Nano, and the limitations showed up fast. The Nano has only two hardware interrupt-capable pins, D2 and D3. Reliable encoder reading depends on hardware interrupts on both the A and B channels to accurately track direction and speed, which means two pins per encoder. I'm also planning to incorporate RTOS down the line, which the Nano couldn't accommodate. Sure, upgrading to a better Arduino board would have solved the flash and pin limitations well enough, and for a while I considered it. So why didn't I do that? Well, as mentioned above, I wanted more of a challenge. 

### STM32 Board Choice 

Ok, so what were the main problems I was looking to solve by upgrading my board? I needed more headroom, both memory and pins. The Nano's 2KB of RAM made an upgrade to RTOS unfeasible, and as mentioned previously, pins started getting tight once I introduced a second motor (I may add a third, who knows!). Cost is also a strong factor as this is a self-funded DIY project. So I was looking for something that was a meaningful step up from Arduino; more pins, more memory. While also being cost effective, and well documented to support my learning. Pretty much all signs pointed to the STM32 Nucleo-64 board. An added bonus is the onboard ST-LINK debugger, meaning no extra hardware needed to actually debug my programs. Plus, it's got a cool little button built into the board, which I'll be repurposing as an ON/OFF switch. All that for around $30, a no-brainer.

Next came the choice of MCU. Two initially stood out from my research, the L-family and F-family, specifically the STM32L476RG and the STM32F446RE. Both were similarly priced, well reviewed, well documented, and available for fast shipping. 

First stop was memory. This was my first real exposure to datasheets, going directly to ST's website and pulling the specs myself. Here's how they stacked up:

- **STM32F446RE** — 512KB Flash, 128KB RAM
- **STM32L476RG** — 1MB Flash, 128KB RAM

So both had identical RAM, more than enough for RTOS, but the STM32L476RG had double the Flash. It looked like the STM32L476RG was the winner.

**Hold on!**

While looking at the STM32L476RG I noticed it sat in ST's "ultra-low power" category, which got me wondering whether that came at a performance cost. Back to the datasheets. Both feature 32-bit ARM Cortex-M4 cores, a significant upgrade from the Nano's 8-bit ATmega328P, but the clock speeds told a different story. The STM32L476RG tops out at 80MHz, while the STM32F446RE reaches 180MHz. More than double, and over ten times faster than the Nano's 16MHz.

Honestly, either would have worked for my use case. But given I'm not running on battery power, the low power advantage of the L476RG wasn't relevant. The STM32F446RE's 512KB Flash is more than sufficient, and the performance advantage is significant. Clear winner.

### Hardware List 
- Motor / Encoder
2 x 12V DC motor with built-in quadrature encoder 
25GA370 DC Encoder Metal Gearmotor – 12V, 150 RPM, Two-Channel Hall-Effect Encoder

- Microcontroller
STM32 Nucleo Development Board with STM32F446RE MCU NUCLEO-F446RE

- Breadboard + Jumpers
BOJACK Solderless Breadboard Kit – 830pt + 400pt boards and jumper wires

- Power Supply (AC → DC)
12V 2A regulated wall adapter

- Buck Converter
Maxmoral LM2596 Step-Down (DC-DC) Module – for generating 5V logic power

- Motor Driver
DROK L298 Dual H-Bridge Motor Speed Controller – optocoupler isolation, 6.5V–27V input

- Display
ELEGOO 0.96" OLED I2C Display (SSD1306-compatible)

### Wiring Diagram

![Wiring Diagram](Diagrams/PID%20Controller%20Diagram.png)

#### Pin Mapping 
Most of the pin mapping was actually straightforward. I had the datasheet and user manual up in front of me. The datasheet was great for giving an overview of the MCU pins and which board pins they mapped to. The user manual would tell me the primary function of the pin. I could also reference the datasheet to see the alternate functions of pins in case I needed different functionality. Motor forward and reverse needed general GPIO pins, the OLED needed pins supporting SDA and SCL, and ENA for the motor driver needed pins that supported PWM. All simple enough. The hard part was actually figuring out the pins for my encoder signals, which I'd saved for last. On my Nano I'd simply used pins that were programmed as interrupts, so each time a signal hit, my processor stopped what it was doing and recorded that signal (never did seem particularly efficient). So I went looking for interrupt pins first. While doing that, I'd remembered what I'd read in the datasheet when looking at the timers for PWM: "They are capable of handling quadrature (incremental) encoder signals."

Side note, but I think the name "Timers" is slightly misleading. Sure, they're primarily used for tracking time, but under the hood they're essentially just a counter that you can increment up, and sometimes, based on the MCU and the timers in that MCU, also decrement. Now realizing I could use a timer to track encoder signals, meaning I wouldn't be interrupting the processor, I just needed to pick which ones to use. I went with TIM2 and TIM5 as they're the only timers with a width of 32 bits. I want to avoid overflowing. In this iteration, I'll be tracking direction as well as speed, so the rising and falling edges of both channel A and B, which is four in total. Each produces 408 pulses per revolution, so 1,632 pulses total per revolution. Modeling at worst case, and assuming I use the motor's fastest speed, that means 244,800 pulses would be produced per minute, which on a 16-bit timer, only capable of holding an integer up to 65,536, means I'd overflow after running my circuit for around 16 seconds — no good. A 32-bit timer, on the other hand, would allow me to run my loop for around 292 hours.

The one wrinkle was that TIM2_CH1 and TIM5_CH1, and TIM2_CH2 and TIM5_CH2, share the same pins: PA0 and PA1. There was an alternate pin for TIM2_CH1 and TIM2_CH2 — PA5 and PB3 — however, PA5 is shared with the onboard user LED, LD2. Checking the user manual section on onboard LEDs, it seemed I lucked out: LD2 is not essential. The important ones being LD1 (communication) and LD3 (power). So the green LED will probably blink while my circuit is running, but nothing detrimental should happen as far as I can tell at this stage.

#### Quick Reference 
**PWM**
- PE9 — TIM1_CH1 — ENA1 (Motor 1 speed)
- PC6 — TIM8_CH1 — ENA2 (Motor 2 speed)

**Direction**
- PC0 — IN1 (Motor 1 forward)
- PC1 — IN2 (Motor 1 reverse)
- PC2 — IN3 (Motor 2 forward)
- PC3 — IN4 (Motor 2 reverse)

**Encoder 1 (TIM2 — 32-bit)**
- PA5 — TIM2_CH1 — Channel A (note: shared with onboard LED LD2)
- PB3 — TIM2_CH2 — Channel B

**Encoder 2 (TIM5 — 32-bit)**
- PA0 — TIM5_CH1 — Channel A
- PA1 — TIM5_CH2 — Channel B

**OLED (I2C)**
- PB8 — SCL
- PB9 — SDA

