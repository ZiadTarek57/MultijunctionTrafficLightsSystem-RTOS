#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define gn (1U)
#define rn (1U << 1)
#define gs (1U << 2)
#define rs (1U << 3)
#define ge (1U << 4)
#define re (1U << 5)
#define gw (1U << 6)
#define rw (1U << 7)
#define sw1 (1U << 4) //sw1 PF0
#define sw2 (1U) //sw2 PF4
#define nssw (1U ) //northsouth sw PE0
#define ewsw (1U << 1) //eastwest sw PE1
#define rst (1U << 3) //reset

xTaskHandle nsTask;
xTaskHandle ewTask;




static _Bool flag1=0;
static _Bool flag2=0;


int TrainArriving(){

		if(!(GPIO_PORTF_DATA_R & sw1 )){
			return 1;
		}
	 return 0;
	}
int TrainPassed(){

		if(!(GPIO_PORTF_DATA_R & sw2 )){
			return 1;
		}
	 return 0;
	}

static void vNorthSouth( void *pvParameters )
{
	for( ;; )
	{
    GPIO_PORTB_DATA_R = (gn | gs | re | rw);       
		vTaskDelay(2500);
		if(flag1 == 1){
					vTaskSuspend(ewTask);
					GPIO_PORTB_DATA_R = (rn | rs | re | rw);
					vTaskDelay(10000);
					flag1=0;
					vTaskResume(ewTask);
	}
}
}

static void vEastWest( void *pvParameters )
{
	for( ;; )
	{
    GPIO_PORTB_DATA_R = (rn | rs | ge | gw);     // LED is red
		vTaskDelay(7500);
		if(flag2 == 1){
					
					GPIO_PORTB_DATA_R = (rn | rs | re | rw);
					vTaskDelay(10000);
					flag2=0;
				  vTaskResume(nsTask);
	}
		
	}
}


static void vTrain( void *pvParameters )
{   
	for( ;; ){
		vTaskDelay(10); //1ms
		if(TrainArriving()){
			  while(TrainPassed()==0){
					GPIO_PORTB_DATA_R  = (rn | rs | re | rw);		
			}
		}
	}
}


static void vPedestrians( void *pvParameters )
{

	for( ;; ){
		vTaskDelay(10); //1ms
		if((GPIO_PORTE_DATA_R & ewsw )){
			flag2 = 1;
			vTaskSuspend(nsTask);	
		}
		if((GPIO_PORTE_DATA_R & nssw )){
			flag1 = 1;
		}	
	}
}

static void vReset( void *pvParameters )
{

	for( ;; ){
		vTaskDelay(10); //1ms
		if((GPIO_PORTE_DATA_R & rst )){
			NVIC_APINT_R = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
		}
			
	}
}

void PortB_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x00000002; // activate clock for port B
  GPIO_PORTB_DIR_R |= 0xFF;  
  GPIO_PORTB_DEN_R |= 0xFF;     // enable digital I/O on PB7-PB0 
  GPIO_PORTB_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTB_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTB_AFSEL_R = 0x00;        // 6) no alternate function
  //GPIO_PORTB_DEN_R = 0xFF;          // 7) enable digital pins PB7-PB0       
}

void PortF_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x00000020; // activate clock for port F
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F; 
  GPIO_PORTF_DIR_R &= ~(0x11);  
  GPIO_PORTF_DEN_R |= (0x11);     // enable digital I/O on PF4-PF0 
	GPIO_PORTF_PUR_R |= 0x11;   
//GPIO_PORTF_DEN_R |= 0x11;          // 7) enable digital pins PF4-PF0       
}
void PortE_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x00000010;
  GPIO_PORTE_DIR_R &= ~(0X0B);
  GPIO_PORTE_PUR_R |= 0X0B;
	GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTE_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTE_AFSEL_R = 0x00;
	GPIO_PORTE_DEN_R |= 0X0B;
}

//void PortA_Init(void){ 
	//volatile unsigned long delay;
  //SYSCTL_RCGC2_R |= 0x00000001;     // 1) activate clock for Port A
  //delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  //GPIO_PORTA_AMSEL_R &= ~0x30;      // 3) disable analog on PA5
  //GPIO_PORTA_PCTL_R &= ~0x00FF0000; // 4) PCTL GPIO on PA5
  //GPIO_PORTA_DIR_R &= ~0x30;        // 5) direction PA5 input
  //GPIO_PORTA_AFSEL_R &= ~0x30;      // 6) PA5 regular port function
  //GPIO_PORTA_DEN_R |= 0x30;         // 7) enable PA5 digital port
//}