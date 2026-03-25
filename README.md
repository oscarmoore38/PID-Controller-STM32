# PID-Controller-STM32
## Overview

As mentioned in the conclusion of my [Arduino Motor Controller](https://github.com/oscarmoore38/PID-Controller) repo, this is a continuation of that project. If you're evaluating my embedded background and landed here first, I'd recommend going back and checking that out. It was my true entry point into embedded work and will give you more context on what I've built and why, what I've learned, and how I think. It includes test data and a fairly detailed write-up.

So what are my goals here? Well, I like to go deeper on one thing rather than jump around. I still feel there's a lot of ground to cover with this project and I'm not done with it. I've genuinely enjoyed getting my hands on hardware, and I realized from a career perspective, I need work that involves that. This project has also reinforced that I have no interest in web or app development whatsoever. I want something real in front of me that I can probe at and figure out. Honestly, regardless of whether I actually make the pivot into an embedded role or not, I'll keep building circuits in my own time — it's just plain fun.

With that said, here's what I want from this project:

---

### 1. Go Deeper With the Hardware

Arduino was a great entry point. It let me understand important concepts and get a circuit running quickly. But it also abstracts away a lot of the details I actually want to wrestle with, and often left me feeling "but how does that actually work?" I want to be trawling through datasheets and manuals, scratching my head trying to figure out what on earth is going on at the hardware level. That may sound strange to some, but I genuinely love that state. To accomplish this goal, I'm upgrading my circuit with an STM32 board. More discussion on the specfic one I chose, and why, later. 

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

I actually added the second motor while still on the Nano, and the limitations showed up fast. The Nano has only two hardware interrupt-capable pins, D2 and D3. Reliable encoder reading depends on hardware interrupts on both the A and B channels to accurately track direction and speed, which means two pins per encoder. I'm also planning to incorporate RTOS down the line, which the Nano couldn't accommodate. Sure, upgrading to a better Arduino board would have solved the flash and pin limitations well enough, and for a while I considered it.

So why didn't I do that? Well, as mentioned above, I'd started to notice something. I was reading embedded systems books in my own time, spending evenings and weekends just digging into how things actually work at the hardware level, genuinely enjoying it. That kind of pull told me something about where I wanted to go: closer to the hardware, down not up, below the HALs to what's actually happening. Arduino's abstraction layer was exactly what I needed early on. It got the project off the ground quickly and let me build foundational concepts without getting buried. But I found myself wanting to push further, so I chose a board that would give me that. 

Finally, my goal with this project isn't just to learn, but to simulate what I may actually face as an embedded engineer: here's a board you've never touched, here's the datasheet and manuals, figure it out and get it built. I'm trying to get comfortable in that space.

