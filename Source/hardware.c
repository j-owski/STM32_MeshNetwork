// hardware.c
// Here you can find the link from the hardware to software

//Usart 1: PA9  (TX) and PA10 (RX)
//Usart 2: PA2  (TX) and PA3  (RX)
//Usart 6: PA11 (TX) and PA12 (RX)

#include "hardware.h"

void init_hardware (void)
{
	GPIO_InitTypeDef GPIO_IS;
	USART_InitTypeDef USART_IS;
	NVIC_InitTypeDef NVIC_InitStructure;

  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
	NVIC_SetPriority (SysTick_IRQn, 0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Add your application code here */
  /* Insert 50 ms delay */
  Delay(50);
	
  /* Enable the GPIOC peripheral */ 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  /* Configure MCO1 pin(PA5) in output */
	GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_IS.GPIO_OType = GPIO_OType_PP;
	GPIO_IS.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_IS.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_SetBits (GPIOC, GPIO_Pin_1| GPIO_Pin_0 | GPIO_Pin_3);
	
  GPIO_Init(GPIOC, &GPIO_IS);
  
  /* Output HSE clock on MCO1 pin(PA8) ****************************************/ 
  /* Enable the GPIOA peripheral */ 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /* Configure MCO1 pin(PA5) in output */
	GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_IS.GPIO_OType = GPIO_OType_PP;
	GPIO_IS.GPIO_Pin = GPIO_Pin_5;
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_IS.GPIO_Speed = GPIO_Speed_2MHz;
	
  GPIO_Init(GPIOA, &GPIO_IS);
	
	
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXX initialise USART1 with Interrupt XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	/* Enable the GPIOA peripheral */ 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//Set Pins PA9 (TX) and PA10 (RX) as alternate function
	GPIO_IS.GPIO_Mode = GPIO_Mode_AF;
	GPIO_IS.GPIO_OType = GPIO_OType_PP;
	GPIO_IS.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_IS.GPIO_Speed = GPIO_Speed_2MHz;
	
  GPIO_Init(GPIOA, &GPIO_IS);
	
	GPIO_PinAFConfig (GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig (GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
/* Enable the USART1 peripheral */ 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	USART_IS.USART_BaudRate = USART_SPEED;
	USART_IS.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_IS.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_IS.USART_Parity = USART_Parity_No;
	USART_IS.USART_StopBits = USART_StopBits_1;
	USART_IS.USART_WordLength = USART_WordLength_8b;		
	USART_Init (USART1, &USART_IS);
	USART1->CR1  |= USART_CR1_RXNEIE;		//enable RXNE Interrupt
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
	//activate USART1
	USART_Cmd (USART1, ENABLE);
	
	
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXX initialise USART2 with Interrupt XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	/* Enable the GPIOA peripheral */ 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
	GPIO_IS.GPIO_Mode = GPIO_Mode_AF;
	GPIO_IS.GPIO_OType = GPIO_OType_PP;
	GPIO_IS.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_IS.GPIO_Speed = GPIO_Speed_2MHz;
	
  GPIO_Init(GPIOA, &GPIO_IS);
	
	//Set Pins PA2 (TX) and PA3 (RX) as alternate function
	GPIO_PinAFConfig (GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig (GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

/* Enable the USART2 peripheral */ 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	USART_IS.USART_BaudRate = USART_SPEED;
	USART_IS.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_IS.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_IS.USART_Parity = USART_Parity_No;
	USART_IS.USART_StopBits = USART_StopBits_1;
	USART_IS.USART_WordLength = USART_WordLength_8b;
	USART_Init (USART2, &USART_IS);
	USART2->CR1  |= USART_CR1_RXNEIE;		//enable RXNE Interrupt
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
	//activate USART2
	USART_Cmd (USART2, ENABLE);

	
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXX initialise USART6 with Interrupt XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	/* Enable the GPIOA peripheral */ 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
	//Set Pins PA11 (TX) and PA12 (RX) as alternate function
	GPIO_IS.GPIO_Mode = GPIO_Mode_AF;
	GPIO_IS.GPIO_OType = GPIO_OType_PP;
	GPIO_IS.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_IS.GPIO_Speed = GPIO_Speed_2MHz;
	
  GPIO_Init(GPIOA, &GPIO_IS);
	
	GPIO_PinAFConfig (GPIOA, GPIO_PinSource11, GPIO_AF_USART6);
	GPIO_PinAFConfig (GPIOA, GPIO_PinSource12, GPIO_AF_USART6);

 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	
	USART_IS.USART_BaudRate = USART_SPEED;
	USART_IS.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_IS.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_IS.USART_Parity = USART_Parity_No;
	USART_IS.USART_StopBits = USART_StopBits_1;
	USART_IS.USART_WordLength = USART_WordLength_8b;		
	USART_Init (USART6, &USART_IS);
	USART6->CR1  |= USART_CR1_RXNEIE;		//enable RXNE interrupt
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
		
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd (USART6, ENABLE);
	
	//CONFIGURE LED
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Configure MCO1 pin(PA5) Output */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}
