/**
 *	20-UART 
 * Prof. Rafael Corsi
 *
 *    (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 */

#include "asf.h"
#include "conf_board.h"
#include "conf_clock.h"
#include "string.h"

/************************************************************************/
/* Configurações                                                        */
/************************************************************************/
#define TAMANHO 16
#define STRING_EOL    "\r"
#define STRING_NULL 0x00
#define STRING_VERSAO "-- "BOARD_NAME" --\r\n" \
					  "-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

#define CONF_UART_BAUDRATE 115200		
#define CONF_UART          CONSOLE_UART

/** 
 * LEDs
 */ 
#define PIN_LED_BLUE	19
#define PORT_LED_BLUE	PIOA
#define ID_LED_BLUE		ID_PIOA
#define MASK_LED_BLUE	(1u << PIN_LED_BLUE)

#define PIN_LED_GREEN 20
#define PORT_LED_GREEN PIOA
#define ID_LED_GREEN	ID_PIOA
#define MASK_LED_GREEN	(1u << PIN_LED_GREEN)

#define PIN_LED_RED 20
#define PORT_LED_RED	PIOC
#define ID_LED_RED	ID_PIOC
#define MASK_LED_RED	(1u << PIN_LED_RED)

void limparvetor(void);

/************************************************************************/
/* var globais                                                                     */
/************************************************************************/
int flagB;
int flagG;
int flagR;

uint8_t g_UartStrg[16];
uint8_t g_UartStrgBuffer[16];
uint8_t g_UartStrgi = 0 ;
uint8_t g_UartStrgOk;
uint8_t g_UartStrgAck = 0;

/************************************************************************/
/* HANDLERS                                                                     */
/************************************************************************/

/**
 *  Interrupt handler for TC0 interrupt. 
 */
void TC0_Handler(void)
{
	volatile uint32_t ul_dummy;
	
    /****************************************************************
	* Devemos indicar ao TC que a interrupção foi satisfeita.
    ******************************************************************/
	ul_dummy = tc_get_status(TC0,0);

	if (flagB == 1)
	{
		/* Avoid compiler warning */
		UNUSED(ul_dummy);

		/** Muda o estado do LED */
		if(pio_get(PIOA,PIO_OUTPUT_0,MASK_LED_BLUE))
		{
			pio_clear(PIOA,MASK_LED_BLUE);
		}
		else
		{
			pio_set(PIOA,MASK_LED_BLUE);
		}

	
		if (flagG == 2)
		{

			/** Muda o estado do LED */
			if(pio_get(PIOA,PIO_OUTPUT_0,MASK_LED_GREEN))
			{
				pio_clear(PIOA,MASK_LED_GREEN);
			}
			else
			{
				pio_set(PIOA,MASK_LED_GREEN);
			}
		}
	}

}
	
void UART0_Handler(void){
	g_UartStrg[g_UartStrgi] = UART0->UART_RHR;

	if(g_UartStrg[g_UartStrgi] == '\n'){
			g_UartStrgOk			= 1;
			g_UartStrg[g_UartStrgi] = NULL;
		
	}
	g_UartStrgi++;
}

/************************************************************************/
/* Configura UART                                                       */
/************************************************************************/
void config_uart(void){
	
	/* configura pinos */
	gpio_configure_group(PINS_UART0_PIO, PINS_UART0, PINS_UART0_FLAGS);
	
	/* ativa clock */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	
	/* Configuração UART */
	const usart_serial_options_t uart_serial_options = {
		.baudrate   = CONF_UART_BAUDRATE,
		.paritytype = UART_MR_PAR_NO,
		.stopbits   = 0
	};
	
	stdio_serial_init((Usart *)CONF_UART, &uart_serial_options);

	UART0->UART_IER = (1);
	NVIC_EnableIRQ(ID_UART0);

}

/************************************************************************/
/* Display Menu                                                         */
/************************************************************************/
static void display_menu(void)
{
	puts(" M : exibe novamente esse menu \n\r"
		 " LGON : Ativa o LED  \n\r"
		 " LGOFF : Desliga o LED \n\r ");
}

/************************************************************************/
/* Config PIO                                  */
/************************************************************************/
void functionPIO(int id, int pin, Pio * port) {
	// 29.17.4 PMC Peripheral Clock Enable Register 0
	// 1: Enables the corresponding peripheral clock.
	// ID_PIOA = 11 - TAB 11-1
	
	PMC->PMC_PCER0 = id;
	port-> PIO_PER = (1 << pin );
	port->PIO_WPMR = 0;
	port->PIO_OER |=  (1 << pin );
}

/************************************************************************/
/* Config TC                                                            */
/************************************************************************/
static void configure_tc(void)
{
	uint32_t ul_sysclk = sysclk_get_cpu_hz();
	pmc_enable_periph_clk(ID_TC0);
	tc_init(TC0,0,TC_CMR_TCCLKS_TIMER_CLOCK5 |TC_CMR_CPCTRG);
	tc_write_rc(TC0,0,4000);
	tc_enable_interrupt(TC0,0,TC_IER_CPCS);
	NVIC_EnableIRQ(ID_TC0);
	tc_start(TC0,0);
}

/************************************************************************/
/* Configura leds                                                                     */
/************************************************************************/
void config_led(void){
	/* Configure LED 1 */
	pmc_enable_periph_clk(ID_LED_BLUE);
	pio_set_output(PORT_LED_BLUE  , MASK_LED_BLUE	,1,0,0);
	functionPIO(ID_PIOA, PIN_LED_GREEN, PIOA);
	functionPIO(ID_PIOC, PIN_LED_RED, PIOC);
}

/************************************************************************/
/* Funçao limpar vetor                                                  */
/************************************************************************/
void limparvetor(void)
{
	g_UartStrgi = 0 ;
	g_UartStrgOk = 0 ;
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/
int main(void)
{
	uint8_t vetor[TAMANHO+1];
	uint8_t uc_key;
	uint8_t rtn;

	/* Initialize the system */
	sysclk_init();
	board_init();


	/* Initialize debug console */
	config_uart();

	/* Initialize_tc */
	configure_tc();

	/* Initialize Led */
	config_led();
	
	/* frase de boas vindas */
	puts(" ---------------------------- \n\r"
	 	 " Bem vindo terraquio !		\n\r"
		 " ---------------------------- \n\r");
		 
	/* display main menu */
	display_menu();
	
	/* init var */
	vetor[TAMANHO] = STRING_NULL;
	
	while (1) {

		while(!g_UartStrgOk){ 
			pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
		};

		//rtn = readvec(&vetor[0]);
	
			if (strcmp(g_UartStrg, "LGON") == 0)
			{
				pio_clear(PORT_LED_GREEN, MASK_LED_GREEN);
				puts("Led ON \n\r");
			}
			else if (strcmp(g_UartStrg, "LBPISCA") == 0)
			{
			flagB = 1;	
			puts("Pisca LED azul \n\r");	
			}
			else if (strcmp(g_UartStrg, "LGPISCA") == 0)
			{
				flagG = 2;
			}
			else if (strcmp(g_UartStrg, "LBON") == 0)
			{
				pio_clear(PORT_LED_BLUE, MASK_LED_BLUE);
				puts("Led ON \n\r");
			}
			else if (strcmp(g_UartStrg, "LRON") == 0)
			{
				pio_set(PORT_LED_RED, MASK_LED_RED);
				puts("Led ON \n\r");
			}
			else if (strcmp(g_UartStrg, "LGOFF") == 0)
			{
			pio_set(PORT_LED_GREEN, MASK_LED_GREEN);
			puts("Led OFF \n\r");	
			flagG = 0 ;
			}
			else if (strcmp(g_UartStrg, "LBOFF") == 0)
			{
				pio_set(PORT_LED_BLUE, MASK_LED_BLUE);
				puts("Led OFF \n\r");
				flagB = 0;
			}
			else if (strcmp(g_UartStrg, "LROFF") == 0)
			{
				pio_clear(PORT_LED_RED, MASK_LED_RED);
				puts("Led OFF \n\r");
			}
			else if (strcmp(g_UartStrg, "FREQ1") == 0)
			{
				tc_write_rc(TC0,0,8000);
				puts("Frequencia 8000 \n\r");
			}
			else if (strcmp(g_UartStrg, "FREQ2") == 0)
			{
				tc_write_rc(TC0,0,4000);
				puts("Frequencia 4000 \n\r");
			}
			else if (strcmp(g_UartStrg, "FREQ3") == 0)
			{
				tc_write_rc(TC0,0,2000);
				puts("Frequencia 2000 \n\r");
			}
			
			/* more else if clauses */
			else 
			{
				printf("[INFO] Opcao nao definida: %s \n\r", g_UartStrg);
			}

			//g_UartStrgAck = 1;
			limparvetor();

	}
}



