# test

Testing ChatGPT Codex

## STM32F446 Hello World Example

This repository includes a simple example for the STM32F446 using the STM32 LL libraries. The program prints `HEllo World` on USART2 and blinks the on-board LED (PA5). It demonstrates a minimal `printf` implementation that writes characters through USART2.

Build with:

```bash
make -C stm32f446_hello
```

A cross compiler such as `arm-none-eabi-gcc` and the STM32F4 LL headers are required.

