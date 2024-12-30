#include "stm32f1xx.h"   // Include STM32F1 header
#include <stdio.h>        // For printf

// Function prototypes
void delay_ms(uint32_t ms);
void UART1_Init(void);
void UART1_SendChar(char c);
void UART1_SendString(const char* str);

int main(void)
{
    // Enable clock for GPIOA (for UART1 TX/RX pins) and USART1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // GPIOA clock enable
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // USART1 clock enable

    // Initialize UART1
    UART1_Init();

    // Print a message to the terminal
    UART1_SendString("STM32F103C8 Serial Output!\r\n");

    // Configure PC13 as an output pin (for LED)
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;    // Enable clock for GPIOC
    GPIOC->CRH &= ~GPIO_CRH_MODE13;        // Clear mode bits
    GPIOC->CRH |= GPIO_CRH_MODE13_1;      // Set mode to 2MHz
    GPIOC->CRH &= ~GPIO_CRH_CNF13;        // Set as general-purpose output

    while (1)
    {
        // Toggle the PC13 LED
        GPIOC->ODR ^= GPIO_ODR_ODR13;       // Toggle PC13 (LED)

        // Print to the terminal when the LED is ON
        if (GPIOC->ODR & GPIO_ODR_ODR13) {
            UART1_SendString("LED is ON\r\n");
        } else {
            UART1_SendString("LED is OFF\r\n");
        }

        // Delay 500ms
        delay_ms(500);
    }
}

// Simple delay function (blocking)
void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 8000; i++) // Adjust based on clock speed
    {
        __NOP(); // No operation (wastes time)
    }
}

// Initialize UART1 for 9600 baud
void UART1_Init(void)
{
    // Configure PA9 (TX) and PA10 (RX) for USART1
    GPIOA->CRH &= ~GPIO_CRH_MODE9; // Clear mode bits for PA9 (TX)
    GPIOA->CRH |= GPIO_CRH_MODE9_1; // Set mode to 2MHz for PA9 (TX)
    GPIOA->CRH &= ~GPIO_CRH_CNF9;  // Set as general-purpose output for PA9 (TX)

    GPIOA->CRH &= ~GPIO_CRH_MODE10; // Clear mode bits for PA10 (RX)
    GPIOA->CRH |= GPIO_CRH_MODE10_0; // Set mode to input for PA10 (RX)

    // Configure USART1: 9600 baud, 8 data bits, 1 stop bit
    USART1->BRR = 0x1D4C; // Set baud rate to 9600 for 72 MHz clock (0x1D4C is the divisor for 9600)
    USART1->CR1 |= USART_CR1_UE;   // Enable USART
    USART1->CR1 |= USART_CR1_TE;   // Enable transmitter
}

// Send a single character over UART
void UART1_SendChar(char c)
{
    while (!(USART1->SR & USART_SR_TXE)); // Wait for the transmit buffer to be empty
    USART1->DR = c; // Send the character
}

// Send a string over UART
void UART1_SendString(const char* str)
{
    while (*str) {
        UART1_SendChar(*str++);
    }
}
