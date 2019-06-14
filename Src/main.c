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
#include "tim.h"
#include "music.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

//osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint32_t ADC_buffer[2];
uint32_t valor_ADC[2];
xTaskHandle Quadradinhos;
xTaskHandle Bola;
xTaskHandle Base;
xTaskHandle Music;
#define lim_sup_X 80					// Limite superior no eixo X
#define lim_sup_Y 25					// Limite superior no eixo Y
#define lim_inf_X 0						// Limite inferior no eixo X
#define lim_inf_Y 15						// Limite inferior no eixo Y
#define velocidade 400					// Velocidade da plataforma

uint32_t perdeu=0,fase=0;					// Flag usada para indicar se jogador perdeu
static uint32_t posicao_base_x[2] = {};	// Vetor posição no eixo X da barra
static uint32_t posicao_bola_x[100] = {};	// Vetor posição no eixo X da barra
static uint32_t posicao_bola_y[100] = {};	// Vetor posição no eixo Y da barra
struct pontos_t posicao_quadrados[30];
static uint32_t posicao_quadrados_y[30] = {};
static uint32_t posicao_quadrados_y1[30] = {};
static uint32_t posicao_quadrados_x[30] = {};
static uint32_t posicao_quadrados_ativo[25] = {};//ativo
static uint32_t detectou_colisao=0,score=0,musica=0,start_song=0;

static uint32_t n_quadrados_linha=4,tam_quadrado=15,n_linhas=2,alt_quadrado=10;// contando que 5 pixels sao de espaco
uint32_t semente=1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
//void StartDefaultTask(void const * argument);

/* DEFINICOES PARA FACILITAR */
#define apagaBola desenha_circulo(x,y,2,0)
#define desenhaBola desenha_circulo(x,y,2,1)
//************************************************************* TASK*********************************************************//
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
	desenha_quadradinhos();


	while (1){
		//musica();
	}

}


void vTask_Bola (void *pvParameters){
	uint32_t x=0;
	uint32_t y=25;
	uint32_t flag_pos_y=0;
	uint32_t flag_=0,flag_pos_x=0,flag_b=0;
	uint32_t perdeu_=0,aleatorio=0,trocouSentido=0;


	srand(semente); // numeros aleatorios
	x=rand()%82;

	inicializa_bola(&x,&y);
	flag_pos_x=x;

	while (1){
		trocouSentido = 0;
		while(!perdeu_jogo()){
//QUANDO BATE NA PARTE ESQUERDA DA BARRA (vindo sempre de cima)
			if((posicao_base_x[0] <= posicao_bola_x[0] && posicao_bola_x[0] <= (posicao_base_x[0]+10)) ){
				// se a bola estiver vindo da direita
				posicao_bola_x[0]=x;
				posicao_bola_y[0]=y;
				if(flag_pos_x>40){
					aleatorio=rand()%4;
					apagaBola;
					x -= aleatorio;
					while((!detecta_colisao_quadrados()) && trocouSentido == 0){ ///enquanto nao bater na barra de cima
						if (checa_limites_tela(x,y)){
							trocouSentido = 1;
						}
		/*
						if(posicao_bola_x[0]+2 >= 84){//limites do display
							trocouSentido = 1;
							do{
								movimentaBola(&x,&y,'E','D');
							}while((!checa_bateu_barra()) && (!perdeu_jogo()));
						}
		*/				else{
							movimentaBola(&x,&y,'E','S');
						}
					}
				}
				//se a bola estiver vindo da esquerda
				else if(flag_pos_x<=40){
					aleatorio=rand()%4;
					apagaBola;
					x += aleatorio;
					while((!detecta_colisao_quadrados()) && trocouSentido == 0){ //enquanto nao bater no quadrado de cima
						if ( checa_limites_tela(x,y) ){
							trocouSentido = 1;
						}

					/*	if(posicao_bola_x[0]-2 <= 1){
							trocouSentido = 1;
							do{
								movimentaBola(&x,&y,'E','D');
							}while((!checa_bateu_barra()) && (!perdeu_jogo()));
						}*/
						else{
							movimentaBola(&x,&y,'D','S'); //movimenta bola para direita
						}
					}
				}

			//BOLA BATENDO NOS QUADRADINOS DE CIMA
				if(detectou_colisao==1){ // caso bata nos quadrados ativa a flag
					flag_b=1;
					posicao_bola_x[0]=x;
					posicao_bola_y[0]=y;
					flag_pos_x=x;

					while(flag_b==1){// caso bateu nos quadrados ele vai voltar na diagonal até que a flag volte para 0
						if(flag_pos_x>40){ // vindo da direita
							movimentaBola(&x,&y,'E','D');
						}
						if(flag_pos_x<=40){//vindo da esquerda
							movimentaBola(&x,&y,'D','D');
						}
						//checa se bateu na barra
						if(checa_bateu_barra()){ // quando bater na barra flag =0 sai do while
							flag_b=0;
							detectou_colisao=0;
						}
						//se nao tocou na barra-> perdeu
						if(perdeu_jogo()){
							flag_b=0;
							detectou_colisao=0;
						}
						if(checa_limites_tela(x,y)){
							flag_b = 0;
							trocouSentido = 1;
						}

					}
					flag_pos_x=x;
					posicao_bola_x[0]=x;
					posicao_bola_y[0]=y;
				}
			}
//PARTE DIREITA DA BARRA
			else if((posicao_base_x[1] >= posicao_bola_x[0] && posicao_bola_x[0] > (posicao_base_x[1]-10))){
				// se a bola estiver vindo da direita
				posicao_bola_x[0]=x;
				posicao_bola_y[0]=y;
				if(flag_pos_x>40){
					aleatorio=rand()%4;
					apagaBola;
					x += aleatorio;
					while((!detecta_colisao_quadrados()) && trocouSentido == 0){ ///enquanto nao bater na barra de cima
						if ( checa_limites_tela(x,y) ){
							trocouSentido = 1;
						}
						/*if(posicao_bola_x[0]+2 >= 84){
							trocouSentido = 1;
							do{
								movimentaBola(&x,&y,'E','D'); //movimenta bola para esquerda
							  }while((!checa_bateu_barra()) && (!perdeu_jogo()));
						}*/
						else{
							movimentaBola(&x,&y,'D','S');
						}
					}
				}
				//se a bola estiver vindo da esquerda
				else if(flag_pos_x<=40){
					aleatorio=rand()%4;
					apagaBola;
					x -= aleatorio;
					while((!detecta_colisao_quadrados()) && (trocouSentido == 0)){ //enquanto nao bater no quadrado de cima
						if (checa_limites_tela(x,y)){
							trocouSentido = 1;
						}

/*						if(posicao_bola_x[0]-2 <= 1){
							trocouSentido = 1;
							do{
								movimentaBola(&x,&y,'E','D'); //movimenta bola para esquerda
							}while((!checa_bateu_barra()) && (!perdeu_jogo()));
						}*/
						else{
							movimentaBola(&x,&y,'E','S');
						}
					}
				}
				flag_pos_x=x;
				posicao_bola_x[0]=x;
				posicao_bola_y[0]=y;

				//BOLA BATENDO NOS QUADRADINOS DE CIMA
				if(detectou_colisao==1 ){ // caso bata nos quadrados ativa a flag
					flag_=1;
					posicao_bola_x[0]=x;
					posicao_bola_y[0]=y;
					while(flag_==1){// caso bateu nos quadrados ele vai voltar na diagonal até que a flag volte para 0
						if(flag_pos_x>40){ // vindo da direita
							movimentaBola(&x,&y,'D','D');
						}
						else if(flag_pos_x<=40){//vindo da esquerda
							movimentaBola(&x,&y,'E','D');
						}
						//checa se bateu na barra
						else if(checa_bateu_barra()){ // quando bater na barra flag =0 sai do while
							flag_=0;
							detectou_colisao=0;

						}
						//se nao tocou na barra-> perdeu
						else if(perdeu_jogo()){
							flag_=0;
							detectou_colisao=0;
						}
						if(checa_limites_tela(x,y)){
							flag_ = 0;
							trocouSentido = 1;
						}
					}
					flag_pos_x=x;
					posicao_bola_x[0]=x;
					posicao_bola_y[0]=y;
				}
			}
			if(trocouSentido == 1){
				flag_= 1;
				flag_pos_x=x;
				flag_pos_y=y;
				posicao_bola_x[0]=x;
				posicao_bola_y[0]=y;
				trocouSentido = 0;
				while(flag_==1){// caso bateu nos quadrados ele vai voltar na diagonal até que a flag volte para 0
					//checa se bateu na barra
					if(checa_bateu_barra()){ // quando bater na barra flag =0 sai do while
						flag_=0;
						detectou_colisao=0;
					}
					//se nao tocou na barra-> perdeu
					else if(perdeu_jogo()){
						flag_=0;
						detectou_colisao=0;
					}
					else if (checa_borda_cim(flag_pos_x,flag_pos_y)){
						if(flag_pos_x>40){
							movimentaBola(&x,&y,'E','D');
						}
						else{
							movimentaBola(&x,&y,'D','D');
						}
					}
					else if (checa_borda_dir(flag_pos_x,flag_pos_y)){
						movimentaBola(&x,&y,'E','D');
					}
					else if (checa_borda_esq(flag_pos_x,flag_pos_y)){
						movimentaBola(&x,&y,'D','D');
					}
					if(checa_limites_tela(x,y)){
						flag_ = 0;
						trocouSentido = 1;
					}
				}

			}
			//proxima fase
			if (score==70){
				limpa_LCD();
				HAL_Delay(100);
				fase=1;
				srand(semente); // numeros aleatorios
				x=rand()%82;
				y=35;
				posicao_bola_x[0]=x;
				posicao_bola_y[0]=y;
				HAL_Delay(500);
				restart(&x,&y);
			}
			if (score >= 160){
				goto_XY(0,0);
				string_LCD("             YOU WIN! YOU ARE THE BEST                   ");
				HAL_Delay(5000);


			}
		}

		srand(semente); // numeros aleatorios
		x=rand()%82;
		y=25;
		score=0;
		perdeu=1;
		n_linhas=2;
		restart(&x,&y);

	}
}

void vTask_Base (void *pvParameters){
	struct pontos_t t;
	t.x2 =50; //sup_x  largura da linha (fica maior quando aumenta o numero)
	t.y2 =45; //sup_y //altura (fica maior quando diminui o numero)
	t.x1 = 30; //inf_x // comprimento (linha maior quando diminui o numero
	t.y1 = 50; //inf_y //
	desenha_retangulo(&t,3);

	while (1){
		posicao_base_x[0]=t.x1;
		posicao_base_x[1]=t.x2;

		if(valor_ADC[1]>2500){//direita
			if(posicao_base_x[1]>=84){//delimitando a barra da tela
				desenha_retangulo(&t,2);
				t.x2=84;
				t.x1=64;

			}
			else{
				desenha_retangulo(&t,2);
				t.x2+=1;
				t.x1+=1;

			}
			desenha_retangulo(&t,3);
			HAL_Delay(50);
		}
		posicao_base_x[0]=t.x1;
		posicao_base_x[1]=t.x2;

		if(valor_ADC[1]<1000){//esquerda
			if(posicao_base_x[0]<=0){//delimitando a barra na tela
				desenha_retangulo(&t,2);
				t.x2=20;
				t.x1=0;
			}
			else{
				desenha_retangulo(&t,2);
				t.x2-=1;
				t.x1-=1;
			}

			desenha_retangulo(&t,3);
			HAL_Delay(50);
		}


	}
}

void vTask_Music(void *pvParameters){
	uint32_t i = 0;

	while(1){

			while(musica==0){//enquanto não perde o jogo toca musica
				HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
				if(music[0][i] != 0){
					__HAL_TIM_SET_PRESCALER(&htim1,music[0][i]);
					HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
				}
				vTaskDelay(music[1][i]);
				i++;

				if(i>=132){
					i = 0;
				}
				/*if(detecta_colisao_quadrados()){
					__HAL_TIM_SET_PRESCALER(&htim1,music[0][20]);
					HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
					vTaskDelay(music[1][5]);
					HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
				}*/
			}
			//perdeu  o jogo faz esse barulhinho
			__HAL_TIM_SET_PRESCALER(&htim1,music[0][100]);
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			vTaskDelay(music[1][10]);
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		}

}

//****************************************************FUNCOES****************************************************//
void desenha_quadradinhos(){
	uint32_t k=0,t=0,aux=0;// contando que 5 pixels sao de espaco
		struct pontos_t quadrado;
		quadrado.ativo = 1;

		quadrado.x1 = 5;
		quadrado.x2 = 15;
		quadrado.y1 = 5;
		quadrado.y2 = 10;
		posicao_quadrados_x[aux]=quadrado.x1;
		posicao_quadrados_y[aux]=quadrado.y2;
		posicao_quadrados_y1[aux]=quadrado.y1;
		posicao_quadrados[aux] = quadrado;
		posicao_quadrados_ativo[aux] =quadrado.ativo;

		for(t=0;t<n_linhas;t++){
			desenha_retangulo(&quadrado,3);
			for(k=0;k<n_quadrados_linha;k++){
				quadrado.x1 = (quadrado.x1 + tam_quadrado);
				quadrado.x2 = (quadrado.x2 + tam_quadrado);
				quadrado.y1 = quadrado.y1;
				quadrado.y2 = quadrado.y2;
				aux++;
				desenha_retangulo(&quadrado,3);
				posicao_quadrados[aux] = quadrado;
				posicao_quadrados_ativo[aux] = quadrado.ativo;
				posicao_quadrados_x[aux]=quadrado.x1;
				posicao_quadrados_y[aux]=quadrado.y2;
				posicao_quadrados_y1[aux]=quadrado.y1;

			}
			posicao_quadrados[aux] = quadrado;
			quadrado.x1 = 5;
			quadrado.x2 = 15;
			quadrado.y1 = (quadrado.y1+alt_quadrado);
			quadrado.y2 = (quadrado.y2+alt_quadrado);
			aux++;
			posicao_quadrados_x[aux]=quadrado.x1;
			posicao_quadrados_y[aux]=quadrado.y2;
			posicao_quadrados_y1[aux]=quadrado.y1;
			posicao_quadrados[aux] = quadrado;
			posicao_quadrados_ativo[aux] = quadrado.ativo;
		}

}


int detecta_colisao_quadrados(){
	uint32_t i=0, detecta=0;

	for(i=0;i<((n_linhas*n_quadrados_linha)+2);i++){ //10 quadrados iniciais
			if(((posicao_bola_y[0]-2 <= posicao_quadrados_y[i] && posicao_bola_y[0]-2 >= posicao_quadrados_y1[i]) || (posicao_bola_y[0]+2 <= posicao_quadrados_y[i] && posicao_bola_y[0]+2 >= posicao_quadrados_y1[i])) && (posicao_bola_x[0]>=posicao_quadrados_x[i]) && (posicao_bola_x[0]<=(posicao_quadrados_x[i]+tam_quadrado)) && (posicao_quadrados_ativo[i] == 1) ){
				detecta=1;
				desenha_retangulo(&posicao_quadrados[i],2);
				posicao_quadrados_ativo[i] = 0;
				detectou_colisao=1;
				score=score+10;
				escreve_Nr_Peq(0,0,score,0);
				//musica();
				return 1;
			}
	}

	if(detecta==0)
		return 0;
}

int checa_bateu_barra(){
	if((posicao_bola_y[0]+2 == 45) && (posicao_bola_x[0]>=posicao_base_x[0] && posicao_bola_x[0]<=posicao_base_x[1]) ){ // quando bater na barra flag =0 sai do while
		return 1;
	}
	else{
		return 0;
	}
}

void movimentaBola (int * x, int * y, char direcao, char altura){
	desenha_circulo(*x,*y,2,0);// apagando o anterior
	if(direcao == 'E'){
		*x -= 1;//diagonal esquerda
	}
	else if(direcao == 'D'){
		*x +=1;//diagonal direita
	}
	if(altura == 'S'){
		*y -= 1;
	}
	else if(altura == 'D'){
		*y += 1;
	}
	desenha_circulo(*x,*y,2,1);//desenha a nova bola
	posicao_bola_y[0]=*y;//atualiza posicao bola
	posicao_bola_x[0]=*x;
	HAL_Delay(100);
}


int perdeu_jogo(){
	if(posicao_bola_y[0]+2>45){
		limpa_LCD();
		musica=1;
	/*	goto_XY(0,0);
		string_LCD("              sorry, but you LOST :(           ");
		HAL_Delay(200);*/
		return 1;
		}
	else
		return 0;
}

int checa_borda_dir(int x, int y){
	if(x+2 >= 84){//limites do display direito
		return 1;
	}
	return 0;

}

int checa_borda_esq(int x, int y){
	if(x-2 <= 0){//limites do display esquerdo
		return 1;
	}
	return 0;
}

int checa_borda_cim(int x, int y){
	if(y-2 <= 0){//limite do display superior
		return 1;
	}
	return 0;
}

int checa_limites_tela(int x, int y){
	if((checa_borda_esq(x,y)==1 || checa_borda_dir(x,y)==1 || checa_borda_cim(x,y)==1 ))
		return 1;
	else
		return 0;
}

void inicializa_bola(int *x,int *y){
	uint32_t init=0;
	posicao_bola_x[0]=*x;
	posicao_bola_y[0]=*y;
	//flag_pos_x=x;
	desenha_circulo(*x,*y,2,1);//desenha a nova bola
	while(init== 0){
		desenha_circulo(*x,*y,2,0);//apaga
		*y=(*y)+1;
		desenha_circulo(*x,*y,2,1);//desenha a nova bola
		posicao_bola_x[0]=*x;
		posicao_bola_y[0]=*y;
		HAL_Delay(200);
		//checando se a bola esta batendo na barra
		if(checa_bateu_barra()){
				init=1;
		}
		else if (perdeu_jogo()){
			init=1;
			perdeu=1; //variavel na fucao restart
			srand(semente); // numeros aleatorios
			x=rand()%82;
			y=25;
			score=0;
			perdeu=1;
			n_linhas=2;
			restart(&x,&y);
		}
	}
}

void restart(int *x, int *y){
	limpa_LCD();
	posicao_bola_x[0]=*x;
	posicao_bola_y[0]=*y;

	if(score == 70){
		goto_XY(0,0);
		string_LCD("              FASE 2                    ");
		HAL_Delay(1000);
		n_linhas=3;
		score=81;
	}

	else if (perdeu==1){
		//musica();
		goto_XY(0,0);
		string_LCD("              LOSER         Press the Joystick to restart                      ");

		while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)){ // enquando nao pressionar joystick fica travado
					semente++;		// semente para o gerador de n�meros pseudoaleatorios
									// pode ser empregado o ADC lendo uma entrada flutuante para gerar a semente.
		}
	}

	limpa_LCD();
	perdeu=0;
	musica=0;
	//HAL_Delay(100);
	desenha_quadradinhos();
	//HAL_Delay(100);
	inicializa_bola(&(*x),&(*y));
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
	  MX_TIM1_Init();
	/* USER CODE BEGIN 2 */
	// inicializa LCD 5110
	inic_LCD();
	limpa_LCD();

	// --------------------------------------------------------------------------------------
	// inicializa tela

	goto_XY(0,0);
	string_LCD("               Press the Joystick to start THE BREAKOUT GAME :)                     ");

	//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	//TIM1->PSC = 200;

	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_buffer,2);
	//HAL_ADC_Start_IT(&hadc1);

	imprime_LCD();




	while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)){ // enquando nao pressionar joystick fica travado
				semente++;		// semente para o gerador de n�meros pseudoaleatorios
								// pode ser empregado o ADC lendo uma entrada flutuante para gerar a semente.
	}





//	init_LFSR(semente_PRNG);	// inicializacao para geracao de numeros pseudoaleatorios
	//rand_prng = prng_LFSR();	// sempre q a funcao prng() for chamada um novo nr � gerado.


	//escreve2fb((unsigned char *)dragon);
	HAL_Delay(1000);
	limpa_LCD();
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
	xTaskCreate(vTask_Quadradinhos,"Task 2",100,NULL,1,NULL);
	xTaskCreate(vTask_Bola,"Task 3",100,NULL,1,NULL);
	xTaskCreate(vTask_Base,"Task 4",50,NULL,1,NULL);
	xTaskCreate(vTask_Music,"Task 5",100,NULL,1,NULL);
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
