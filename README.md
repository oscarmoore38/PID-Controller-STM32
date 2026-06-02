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

**Hold the phone!**

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

### Pin Mapping 
Most of the pin mapping was actually straightforward. I had the datasheet and user manual up in front of me. The datasheet was great for giving an overview of the MCU pins and which board pins they mapped to. The user manual would tell me the primary function of the pin. I could also reference the datasheet to see the alternate functions of pins in case I needed different functionality. Motor forward and reverse needed general GPIO pins, the OLED needed pins supporting SDA and SCL, and ENA for the motor driver needed pins that supported PWM. All simple enough. The hard part was actually figuring out the pins for my encoder signals, which I'd saved for last. On my Nano I'd simply used pins that were programmed as interrupts, so each time a signal hit, my processor stopped what it was doing and recorded that signal (never did seem particularly efficient). So I went looking for interrupt pins first. While doing that, I'd remembered what I'd read in the datasheet when looking at the timers for PWM: "They are capable of handling quadrature (incremental) encoder signals."

Side note, but I think the name "Timers" is slightly misleading. Sure, they're primarily used for tracking time, but under the hood they're essentially just a counter that you can increment up, and sometimes, based on the MCU and the timers in that MCU, also decrement. Now realizing I could use a timer to track encoder signals, meaning I wouldn't be interrupting the processor, I just needed to pick which ones to use. I went with TIM2 and TIM5 as they're the only timers with a width of 32 bits and I want to avoid overflowing. In this iteration, I'll be tracking direction as well as speed, so the rising and falling edges of both channel A and B, which is four in total. Each produces 408 pulses per revolution, so that's 1,632 pulses total per revolution. Modeling at worst case, assuming I use the motor's fastest speed of 150rpm, that means 244,800 pulses would be produced per minute, which on a 16-bit timer, only capable of holding an integer up to 65,536, means I'd overflow after running my circuit for around 16 seconds — no good. A 32-bit timer, on the other hand, would allow me to run my loop for around 292 hours. 

With timers selected, I first picked arbitrary channels within these timers, figuring they must all be the same. However, going to the reference manual, specifically section 17.3.12 covering Encoder Interface Mode, it confirmed that only CH1 and CH2 of the timers are designed to interface with encoders (I wasn't joking when I said I enjoy trawling manuals!). This led me to my one major wrinkle -- TIM2_CH1 and TIM5_CH1, and TIM2_CH2 and TIM5_CH2, share the same pins: PA0 and PA1. There were alternate pins for TIM2_CH1 and TIM2_CH2, PA5 and PB3, however, PA5 is shared with the onboard user LED, LD2. Checking the user manual section on onboard LEDs, it seemed I lucked out: LD2 is not essential. The important ones being LD1 (communication) and LD3 (power). So the green LED will probably blink while my circuit is running, but nothing detrimental should happen as far as I can tell at this stage.

### Quick Reference 
**PWM**
- PA8 — TIM1_CH1 — ENA1 (Motor 1 speed)
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

## Software Design

### Tooling and Development Environment
Similar to my last project, with a couple of small changes to my platformio.ini file, I'm still able to use PlatformIO and VS Code with my STM32 board. PlatformIO even bundles the tools needed to enable the onboard ST-LINK debugger, so I have everything I need out of the box. As discussed above, I'm implementing this project bare-metal with CMSIS headers, no HAL required.

### Tinkering
Whenever I'm faced with a new situation, I always do the same thing. Before fully diving into manuals, before design, before writing a single line of my actual program code, I just try to do the simplest thing possible. This step, I think, helps build a better foundation to then carry into design. Similar to my last project, I put myself in a position where I'd never done anything like this before, never written a line of bare-metal code, never configured a register with bitwise operations. So just like before, the simplest first step after powering on was blinking the onboard LED. It was actually fairly straightforward. I found the stm32f446xx.h header in my PlatformIO config, and that coupled with the reference manual gave me everything I needed. Fifteen lines of code and a couple of hours later, I had a blinking LED and it taught me basically everything I'll need for this project; I enabled peripheral clocks via RCC registers, then set pin mode and output state using the appropriate registers from the reference manual. I also opted to use a timer to track intervals rather than burning CPU cycles, which allowed me to get my hands on the prescaler, the auto-reload register, and update flags. You assume the first time you work with timers that they must have a built-in way to track real time - nope - they just count clock ticks. I used the prescaler to slow the tick rate to match ms and the ARR to set the overflow interval. Side note on something I found interesting: I  learned that the default system clock (HSI) runs at 16MHz, not the 180MHz maximum. Getting there requires configuring the PLL, which is beyond the scope of this phase, but I'd like to play with that in the future. 

### Design 
As mentioned above, I wanted to be more intentional about software design this time around. Below is a block diagram showing the general outline. I've gone with a facade design pattern, where the hardware layer is abstracted away from the application layer — main, my software objects, and types. All hardware setup lives in dedicated setup files, grouped by peripheral family: GPIO, Timers, PWM, and I2C. If my board changes, I add new setup files and update a few lines in main, everything else stays untouched. I'm sure there's plenty I'm still missing, but the core idea is to isolate the code that changes so it's all in one place rather than scattered across the project.

![Design Diagram](Diagrams/PI-Controller%20Software%20Design.png)

## Software Implementation

### Iteration 1 — Spin Motors with PWM
I like to break my work into chunks and start with the simplest thing first. To me, that was just spinning both motors with PWM, not yet worrying about timing intervals, encoder signals, RPM calculation, PID math, or the display. Nothing here was too complicated. I initialized the peripherals I needed by enabling their clocks via the RCC peripheral, then set mode and state on each peripheral's appropriate registers. GPIO pins were straightforward. Enabling the timers for PWM mode was slightly more involved than my previous experience setting up a timer for intervals. Still used the prescaler to bring down the tick rate. Then ARR, which in the context of PWM determines the cycle length of the PWM signal (frequency), and CCR, which determines how long the signal stays high before going low. Together, these two produce the duty cycle. Simple enough, if ARR is 10 and CCR is 5, I'm producing a 50% duty cycle PWM signal.

At first I thought the values in ARR and CCR could be arbitrary as long as they produced the desired duty cycle, so 10 and 5 does the same thing as 1000 and 500. I was wrong. Fewer counts in ARR means the timer overflows faster, which means a higher PWM frequency. You have to choose a frequency within the range your motor driver can handle. I checked the spec sheet, 0 to 10kHz. I picked 1kHz. It's towards the lower end of the range, still higher than what Arduino's analogWrite() defaults to, and the math is easier. Here's how I determined my values:

- HSI default clock = 16MHz = 16,000,000 ticks per second.
- Without a prescaler, my 16-bit timer would overflow in about 4ms, roughly 250Hz, too slow.
- A prescaler value of 15 (off by one like ARR, CCR) brings the tick rate to 1,000,000 ticks per second.
- Setting ARR to 999 means the timer overflows every 1000 ticks, 1000 times per second, or 1kHz.
- CCR of 499 gives me 50% duty cycle.

One other thing I learned. TIM1 and TIM8 are advanced timers. Beyond basic setup, you need to enable the MOE bit in the BDTR register, which is a master output switch. Without it, output is disabled by default. I also had to enable CC1E for channel 1, the channel my timer is using. Without it, the PWM signal is generated internally but never routed to the physical pin.

I like writing bare-metal. It's amazing how much more you learn. Beyond a basic understanding of PWM, I never had to think about any of this with analogWrite(). Arduino just handled it. My Arduino setup was roughly 15 lines of code for the entire project. I'm not even done with setup here and I'm already at 70+ lines.

### Iteration 1 — Debug
I wrote the code under the "Iteration 1 - spinning two motors with PWM" commit, flashed it to my board, and nothing. Motors didn't spin. Checklist:

- **Power** — LD3 was illuminated. My gut said this was a circuit issue, not software.

- **Loose connections** — checked everything. Still nothing.

- **Multimeter** — direction pins outputting ~3.3V high and 0V low. PWM pins outputting ~1.67V, correct for 50% duty cycle. So now fairly certain this wasn't a software issue.

- **Accidental discovery** — while probing, I knocked the board's GND wire loose and the motor started spinning. Weird. Checked that everything shared a common ground. It did.

- **Replication** — put the GND back and started pulling other wires. Removing the signal wires to the motor driver also spun the motor. Even weirder.

- **Simplify** — I had a lot going on. OLED, two motors, wires everywhere. I stripped it back to one motor to isolate the issue. Confirmed that removing the high signal wire consistently spun the motor. So it was a signal issue, specifically with the high signal.

- **Multimeter back on the pin** — reading 3.3V high. Then the thought hit me: does the driver actually register 3.3V as high?

- **Spec sheet** — "If using 5V logic supply: VIH ≈ 3.5V minimum. If using 3.3V logic supply: VIH ≈ 2.3V minimum." I was powering the driver logic supply with 5V. My STM32 high signal of 3.3V was below the 3.5V threshold. The driver wasn't reliably recognizing it as high.

- **Fix** — I dropped my buck converter output to 3.3V and powered both the STM32 and the driver logic supply from that rail. The driver accepts 3.3V to 6.5V on the logic supply pins despite the 5V label. With logic supply at 3.3V, minimum for high now drops to 2.3V.

**Wheels started spinning!**

I was still curious why removing the GND and high wires had spun the motor earlier. My best guess is that removing those wires caused the driver input pins to float. The motor driver likely has internal pull-up resistors that pulled the floating inputs high, although I couldn't confirm this in the spec sheet. Either way, floating inputs producing unpredictable behavior is a good lesson in itself.

This debug took me a few hours, and I basically discovered the issue accidently. My main takeaway is to reach for circuit component spec sheets earlier. 

---

### Iteration 2 - PID Logic 

This was a fun iteration. The core of it was configuring interrupts, setting up my ISR, and re-implementing my C++ classes -- PID, Motor -- in C. The biggest task being figuring out how to set up an interrupt to fire on an event and call an ISR, so that will be the bulk of the below reflection. I took my time with it, as, aside from just a life with children and a full-time job, I wanted to really learn about interrupts on bare-metal STM32. Back with my Arduino, it was as simple as defining my ISR in main and passing it to attachInterrupt(), but as with all things bare-metal: that was not the case here. 

I started by sketching out a rough diagram of the flow with a pencil and paper. I'm talking very basic stuff: a timing event is happening here, when that happens, values need to be gathered from my encoder timers and sent to the main loop to be processed. I then created a rough list of tasks, and separated it into what I knew and what I didn't. If I wasn't sure how to do something, instead of writing a clear task I would simply write the behaviour I wanted to see. I handled the easier stuff I'd nailed down previously first; bringing peripherals online, setting AF mode for encoders by looking up the correct AF number in the datasheet, and configuring a timer for intervals, to name a few.

#### Configuring an Interrupt 

First stop in my unknowns was how to actually configure my interval timer to fire an interrupt. I knew I wanted this to happen on an overflow event, or when the timer hit the value I'd defined in ARR. I went to the reference manual RM0390 and looked at the list of registers for my timer. The first register that jumped out at me was the DIER register (interrupt enable register). Jumping to that section, I found flipping bit 0 (UIE) enables an interrupt to fire on an update event. Checking back on EGR, I confirmed setting bit 0 (UG) generates an update event for the timer, but when you do that -- including in setup -- it re-initializes the counter and sets the UIF flag in SR. So I clear that in my setup code before touching the DIER register, to avoid an interrupt firing immediately.

Next up on the list was the big one: how do I configure things so that when an event happens, it calls my ISR? Section 10, Interrupts and Events, in RM0390, introduced me to the NVIC (Nested Vectored Interrupt Controller), a component of the actual core that controls interrupts for the processor, and Table 38 in section 10.2, which is the vector table for the STM32F446xx. The vector table details all the supported interrupt events, their priority, and an acronym for each. I found the one I was looking for -- TIM1_BRK_TIM9, at position 24 with a priority of 31.

A couple of things I started thinking about at this stage. I'm using both Timer 1 and 9, although only 9 is configured to fire an interrupt. If both did, I'd have to start thinking about handling which timer fired when TIM1_BRK_TIM9 runs. I also learned about weak symbols, which in the context of these handlers basically means I can override them with my own definition. To confirm this, I actually went and found the assembly file startup_stm32f446xx.s, and, sure enough, you can see it's defined with the .weak symbol. 

```asm
.weak      TIM1_BRK_TIM9_IRQHandler            
.thumb_set TIM1_BRK_TIM9_IRQHandler,Default_Handler
```

>**Side note:** I've been networking with embedded engineers and I'm really interested in bring-up work. I noticed the reset handler was also defined in the vector table, which is called before main() when the processor resets. Which got me thinking: can I override this and do my own custom bring-up, more for the experience than anything else? Sure enough, the reset handler is defined as weak in the startup file:

```asm
.section  .text.Reset_Handler
.weak  Reset_Handler
.type  Reset_Handler, %function
Reset_Handler:
```

>I think I'm going to attempt this once this project is done. 

Next, on to NVIC. I jumped to the processor manual PM0214, which details out NVIC the number of interrupts it supports, the priority you can assign them, and a description of its registers. Section 4.3.1 gave me what I really needed -- it defines how to access the NVIC registers via CMSIS, which you can do using NVIC_EnableIRQ() and passing it the appropriate IRQ, which I found in my CMSIS header file.

So combining the two: I overrode TIM1_BRK_TIM9_IRQHandler, which became my ISR for interval-based encoder reads, and then told the processor to call it using the CMSIS method defined in PM0214.

#### Interrupt Architecture

With that sorted, I mapped out the following flow:

![ISR Diagram](Diagrams/Interrupt%20Architecture%20for%20Encoder%20Readings.png)

Timers 2 and 5 run encoder counts, Timer 9 fires an interrupt on overflow, and my ISR captures the counts and passes them to main.

Even with the architecture mapped out, I still had open questions:

- ISRs should be light and non-blocking, so what should my ISR actually do?
- How does main know an interrupt fired and there's work to do?

For the second one: I could poll TIM9's UIF flag via the SR register directly in main, but I wanted the signal main reacts to to come from the ISR, so I can be sure the data is ready when main acts on it. I created ISRFlags, which the ISR sets, then main polls and resets.

That led to my next question: doesn't polling the ISRFlags make my timing non-deterministic? I started to think about what actually needs to be determinsitc in my program. It came down to two things: timing and encoder counts.

For timing, I don't think a delta approach is needed here. On the Arduino I was calculating interval duration manually inside loop(), so tracking elapsed time made sense. On the STM32, a dedicated timer is handling that job and firing the interrupt at an exact fixed interval, so instead, I just defined a global fixed timing variable, ```uint8_t TimeDT```, in my Main file for the PID calculations to use. For encoder counts, my ISR pulls them directly from Timers 2 and 5 using a delta approach, tracking the last known count and passing ```current - last``` to main.

I do want to acknowledge that main polling a flag may introduce a little jitter. It might, for example, act at 82ms instead of 80ms. That said, I wasn't doing anything fundamentally different on the Arduino. I was calculating timing intervals with millis(), polling them in loop(), and waiting for the interval to elapse, and that worked fine. I think the same applies here. If this were a true industrial automation project with tight control requirements it might not be acceptable. But for a personal project, I think it's ok. Everything that actually drives RPM calculation and PID output is captured deterministically in the ISR. Main just does the math with those values, whenever it sees the flag.

