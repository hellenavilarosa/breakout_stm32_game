/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "atraso.h"
#include "defPrincipais.h"
#include "NOKIA5110_fb.h"
#include "figuras.h"
#include "PRNG_LFSR.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

//osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint32_t ADC_buffer[2];
uint32_t valor_ADC[2];

#define lim_sup_X 80					// Limite superior no eixo X
#define lim_sup_Y 25					// Limite superior no eixo Y
#define lim_inf_X 0						// Limite inferior no eixo X
#define lim_inf_Y 10						// Limite inferior no eixo Y
#define velocidade 400					// Velocidade da plataforma

static uint32_t perdeu;					// Flag usada para indicar se jogador perdeu
static uint32_t venceu;					// Flag usada para indicar se jogador venceu
static uint32_t posicao_base_x1[100] = {};	// Vetor posição no eixo X da barra
static uint32_t posicao_base_x2[100] = {};	// Vetor posição no eixo Y da barra
static uint32_t posicao_bola_x[100] = {};	// Vetor posição no eixo X da barra
static uint32_t posicao_bola_y[100] = {};	// Vetor posição no eixo Y da barra
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
//void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if(hadc->Instance == ADC1)
	{
		valor_ADC[0]=ADC_buffer[0];
		valor_ADC[1]=ADC_buffer[1];
	}
}

//---------------------------------------------------------------------------------------------------
// Tarefa para atualizar periodicamente o LCD
void vTask_LCD_Print(void *pvParameters)
{
	while(1) imprime_LCD();
}
//---------------------------------------------------------------------------------------------------


void vTask_Quadradinhos (void *pvParameters){
	struct pontos_quadrado_t quadrado;
	quadrado.x1 = lim_inf_X;	quadrado.y1 = lim_inf_Y;
	quadrado.x2 = lim_sup_X;	quadrado.y2 = lim_sup_Y;
	while (1){

			desenha_retangulo(&quadrado,3);
	}

}
void vTask_Bola (void *pvParameters){
	uint32_t x=50;
	uint32_t y=50;
	uint32_t i=0,flag=0;
	desenha_circulo(x,y,2,1);
	while (1){
			posicao_bola_x[0]=x;
			posicao_bola_y[0]=y;
			escreve_Nr_Peq(0,0,posicao_base_x2[0],10);
			escreve_Nr_Peq(0,6,posicao_bola_x[0],10);

			for(i=0 ; flag=0 ; i++){
				if(posicao_base_x1[i]==posicao_bola_x[0]){
					desenha_circulo(x,y,2,0);// apagando o anterior PROBLEMA
					x++;
					y--;
					desenha_circulo(x,y,2,1);
					flag=1;
				}
			}
	}

}


/* Juca esta com fome */

void vTask_Base (void *pvParameters){
	struct pontos_t t;
	uint32_t i=0;
	t.x2 =50; //sup_x  largura da linha (fica maior quando aumenta o numero)
	t.y2 =45; //sup_y //altura (fica maior quando diminui o numero)
	t.x1 = 30; //inf_x // comprimento (linha maior quando diminui o numero
	t.y1 = 50; //inf_y //
	desenha_retangulo(&t,3);

	while (1){
		//escreve_Nr_Peq(0,0, valor_ADC[1],10);

		if(valor_ADC[1]>2500){//direita
			desenha_retangulo(&t,2);
			t.x2+=1;
			t.x1+=1;
			desenha_retangulo(&t,3);
		}
		HAL_Delay(100);

		if(valor_ADC[1]<1000){//esquerda
			desenha_retangulo(&t,2);
			t.x2-=1;
			t.x1-=1;

			desenha_retangulo(&t,3);
		}
		posicao_base_x1[0]=t.x1;

		for(i=1 ;t.x1!=t.x2; i++){
			posicao_base_x1[i]=t.x1+1;
		}


	}

}

//---------------------------------------------------------------------------------------------------
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint32_t semente_PRNG=1;

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();

	/* USER CODE BEGIN 2 */
	// inicializa LCD 5110
	inic_LCD();
	limpa_LCD();

	// --------------------------------------------------------------------------------------
	// inicializa tela

	goto_XY(0, 0);
	//string_LCD("Breakout Game");


	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_buffer,2);
	HAL_ADC_Start_IT(&hadc1);

	imprime_LCD();

//	while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)) // enquando nao pressionar joystick fica travado
//	{
//		semente_PRNG++;		// semente para o gerador de n�meros pseudoaleatorios
//							// pode ser empregado o ADC lendo uma entrada flutuante para gerar a semente.
//	}

//	init_LFSR(semente_PRNG);	// inicializacao para geracao de numeros pseudoaleatorios
	//rand_prng = prng_LFSR();	// sempre q a funcao prng() for chamada um novo nr � gerado.

	//limpa_LCD();
	//escreve2fb((unsigned char *)dragon);
	//imprime_LCD();
	HAL_Delay(1000);
//	limpa_LCD();
//	imprime_LCD();

	/* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

	/* Create the thread(s) */
	/* definition and creation of defaultTask */
	// osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
	// defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	xTaskCreate(vTask_LCD_Print, "Task 1", 100, NULL, 1,NULL);
//	xTaskCreate(vTask_Nr_Print, "Task 2", 100, NULL, 1,NULL);
	xTaskCreate(vTask_Quadradinhos,"Quadradinhos",100,NULL,1,NULL);
	xTaskCreate(vTask_Bola,"Bola",100,NULL,1,NULL);

	xTaskCreate(vTask_Base,"Base",100,NULL,1,NULL);

	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */
 

	/* Start scheduler */
	// osKernelStart();
	vTaskStartScheduler();	// apos este comando o RTOS passa a executar as tarefas


  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6 
                          |GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA3 PA4 PA5 PA6 
                           PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6 
                          |GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* StartDefaultTask function */
//void StartDefaultTask(void const * argument)
//{

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
//  for(;;)
//  {
//    osDelay(1);
//  }
  /* USER CODE END 5 */ 
//}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
