#include "stm32f4xx.h"

static uint8_t BUTTON_TopState = 0;
static uint8_t BUTTON_BottomState = 0;
static uint8_t BUTTON_CenterState = 0;
static uint8_t BUTTON_LeftState = 0;

// Function to initialize GPIO
static void GPIO_Init(void) 
{
	// Enable RCC for GPIOA, GPIOC, and GPIOB
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOBEN;

	// Configure PA9, PC7, and PB4 as output mode
	GPIOA->MODER |= GPIO_MODER_MODER9_0;
	GPIOC->MODER |= GPIO_MODER_MODER7_0;
	GPIOB->MODER |= GPIO_MODER_MODER4_0;

	// Activate GPIOs PA9, PC7, and PB4
	GPIOA->BSRR = GPIO_BSRR_BS_9; 
	GPIOC->BSRR = GPIO_BSRR_BS_7; 
	GPIOB->BSRR = GPIO_BSRR_BS_4; 
}

static void SwitchInit(void)
{
	// Enable clock for SYSCFG and EXTI
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable clock for SYSCFG unit

	// Initialize the top button
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable clock for GPIO A
	GPIOA->MODER &= ~GPIO_MODER_MODER4; // Set pin PA4 to input mode
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PA; // Select PA4 for EXTI4
	EXTI->RTSR |= EXTI_RTSR_TR4; // Enable rising edge detection for EXTI4
	EXTI->IMR |= EXTI_IMR_MR4; // Enable interrupt for EXTI4

	// Initialize the bottom button
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable clock for GPIO B
	GPIOB->MODER &= ~GPIO_MODER_MODER0; // Set pin PB0 to input mode
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB; // Select PB0 for EXTI0
	EXTI->RTSR |= EXTI_RTSR_TR0; // Enable rising edge detection for EXTI0
	EXTI->IMR |= EXTI_IMR_MR0; // Enable interrupt for EXTI0

	// Initialize the center button
	GPIOB->MODER &= ~GPIO_MODER_MODER5; // Set pin PB5 to input mode
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PB; // Select PB5 for EXTI5
	EXTI->RTSR |= EXTI_RTSR_TR5; // Enable rising edge detection for EXTI5
	EXTI->IMR |= EXTI_IMR_MR5; // Enable interrupt for EXTI5

	// Initialize the left button
	GPIOC->MODER &= ~GPIO_MODER_MODER1; // Set pin PB1 to input mode
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PC; // Select PB1 for EXTI1
	EXTI->RTSR |= EXTI_RTSR_TR1; // Enable rising edge detection for EXTI1
	EXTI->IMR |= EXTI_IMR_MR1; // Enable interrupt for EXTI1

	NVIC_SetPriority(EXTI4_IRQn, 0); // Set priority for EXTI4 interrupt
	NVIC_EnableIRQ(EXTI4_IRQn); // Enable EXTI4 interrupt in NVIC

	NVIC_SetPriority(EXTI0_IRQn, 1); // Set priority for EXTI0 interrupt
	NVIC_EnableIRQ(EXTI0_IRQn); // Enable EXTI0 interrupt in NVIC

	NVIC_SetPriority(EXTI9_5_IRQn, 2); // Set priority for EXTI9_5 interrupt
	NVIC_EnableIRQ(EXTI9_5_IRQn); // Enable EXTI9_5 interrupt in NVIC

	NVIC_SetPriority(EXTI1_IRQn, 3); // Set priority for EXTI1 interrupt
	NVIC_EnableIRQ(EXTI1_IRQn); // Enable EXTI1 interrupt in NVIC
}

void EXTI4_IRQHandler(void)
{
	if (EXTI->PR & EXTI_PR_PR4) // Check if it is EXTI4 interrupt
	{
		BUTTON_TopState = 1; // Update top button state first
		EXTI->PR |= EXTI_PR_PR4; // Clear interrupt flag after updating state
	}
}

void EXTI0_IRQHandler(void)
{
	if (EXTI->PR & EXTI_PR_PR0) // Check if it is EXTI0 interrupt
	{
		BUTTON_BottomState = 1; // Update bottom button state first
		EXTI->PR |= EXTI_PR_PR0; // Clear interrupt flag after updating state
	}
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI->PR & EXTI_PR_PR5) // Check if it is EXTI5 interrupt
	{
		BUTTON_CenterState = 1; // Update center button state first
		EXTI->PR |= EXTI_PR_PR5; // Clear interrupt flag after updating state
	}
}

void EXTI1_IRQHandler(void)
{
	if (EXTI->PR & EXTI_PR_PR1) // Check if it is EXTI1 interrupt
	{
		BUTTON_LeftState = 1; // Update left button state first
		EXTI->PR |= EXTI_PR_PR1; // Clear interrupt flag after updating state
	}
}

int main(void) 
{
	// Initialization
	GPIO_Init();
	SwitchInit();

	while (1) 
	{
		// Turn on LEDs
		if (BUTTON_TopState) 
		{
			GPIOC->BSRR = GPIO_BSRR_BR_7; // Turn on LED connected to PC7
			BUTTON_TopState = 0; // Reset top button state
		}
		if (BUTTON_BottomState) 
		{
			GPIOC->BSRR = GPIO_BSRR_BS_7; // Turn off LED connected to PC7
			BUTTON_BottomState = 0; // Reset bottom button state
		}
		if (BUTTON_CenterState) 
		{
			GPIOA->BSRR = GPIO_BSRR_BS_9; // Turn on LED connected to PA9
			BUTTON_CenterState = 0; // Reset center button state
		}
		if (BUTTON_LeftState) 
		{
			GPIOA->BSRR = GPIO_BSRR_BR_9; // Turn off LED connected to PA9
			BUTTON_LeftState = 0; // Reset left button state
		}
	} 
}