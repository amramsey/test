# test

Testing ChatGPT Codex

## STM32F446 Hello World Example

This repository includes a simple example for the STM32F446 using the STM32 LL libraries. The program prints `Hello World` on USART2 and blinks the on-board LED (PA5).

It also drives a hobby servo on PB6. A UART command interface allows controlling the servo pulse width and the LED state.

Commands:

- `SERVO <us>`: set servo pulse width between 500 and 2500 microseconds
- `LED ON`: turn the LED on
- `LED OFF`: turn the LED off

Build with:

```bash
make -C stm32f446_hello
```

A cross compiler such as `arm-none-eabi-gcc` and the STM32F4 LL headers are required.
