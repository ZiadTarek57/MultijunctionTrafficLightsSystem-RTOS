#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "main.h"

void PortB_Init(void);
void PortF_Init(void);
void PortE_Init(void);

//static void vTask3( void *pvParameters );
int main(){
  PortF_Init();
	PortB_Init();
	PortE_Init();
	xTaskCreate( vNorthSouth, (const portCHAR *)"NorthSouth", configMINIMAL_STACK_SIZE, NULL, 2, &nsTask );
	xTaskCreate( vEastWest, (const portCHAR *)"EastWest", configMINIMAL_STACK_SIZE, NULL, 1, &ewTask );
	xTaskCreate( vPedestrians, (const portCHAR *)"Pedestrians", configMINIMAL_STACK_SIZE, NULL, 3, NULL );	
	xTaskCreate( vTrain, (const portCHAR *)"Train", configMINIMAL_STACK_SIZE, NULL, 4, NULL );
	xTaskCreate( vReset, (const portCHAR *)"Reset", configMINIMAL_STACK_SIZE, NULL, 5, NULL );
	
	/* Start the scheduler. */
	vTaskStartScheduler();
}