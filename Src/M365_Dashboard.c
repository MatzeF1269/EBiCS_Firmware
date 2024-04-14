/*
 * M365_Dashboard.c
 *
 *  Created on: Nov 27, 2021
 *      Author: stancecoke
 */


#include "main.h"
#include "FOC.h"
#include "config.h"
#include "stm32f1xx_hal.h"
#include "print.h"
#include "M365_Dashboard.h"
#include "M365_memory_table.h"
#include "decr_and_flash.h"
#include "stm32f1xx_hal_flash.h"


#include "stm32f1xx.h"

enum { STATE_LOST, STATE_START_DETECTED, STATE_LENGTH_DETECTED };
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
static uint8_t ui8_UART3_rx_buffer[132];
static uint8_t ui8_UART1_rx_buffer[18];
static uint8_t ui8_dashboardmessage[132];
static uint8_t ui8_controllermessage[18];

static uint8_t	ui8_UART3_tx_buffer[96];// = {0x55, 0xAA, 0x08, 0x21, 0x64, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t ui8_oldpointerposition=0;
static uint8_t ui8_recentpointerposition=0;
static uint8_t ui8_oldpointerposition_1=0;
static uint8_t ui8_recentpointerposition_1=0;
//static uint8_t ui8_messagestartpos=255;
static uint8_t ui8_messagelength=0;
static uint8_t ui8_messagelength_1=0;
//static uint8_t ui8_state= STATE_LOST;
//static uint32_t ui32_timeoutcounter=0;

//static uint32_t sysinfoaddress = 0x0800F800;
//static uint32_t proc_ID_address = 0x1FFFF7E8;
char sys_info[512] = {
		0x5C,0x51,0xEE,0x7,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8,0x0,0x8,0x0,0x8,0x31,0x33,0x36,0x37,0x38,0x2F,0x30,0x30,0x31,0x31,0x30,0x30,0x32,0x39,0x30,0x30,0x30,0x30,0x30,0x30,0x34,0x1,0x0,0x0,0x0,0x0,0x0,0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x20,0x4E,0x10,0x27,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x50,0xFF,0x70,0x6,0x83,0x67,0x51,0x56,0x30,0x44,0x9,0x67,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
};

char *target;
char *source;



M365_menory_table_t MT;

enum bytesOfMessage65 {
	Throttle = 7,
	Brake = 8,
} msg65;

enum bytesOfMessage64 {
	Speed = 10,
	Mode = 6,
	SOC = 7,
	Light = 8,
	Beep = 9,
	errorcode = 11
} msg64;

enum bytesOfGeneralMessage {
	msglength = 2,
	receiver = 3,
	command = 4,
	startAddress = 5,
	payloadLength = 6
} gen_msg;


void M365Dashboard_init(void) {
//        CLEAR_BIT(huart3.Instance->CR3, USART_CR3_EIE);
	if (HAL_UART_Receive_DMA(&huart3, (uint8_t*) ui8_UART3_rx_buffer, sizeof(ui8_UART3_rx_buffer)) != HAL_OK) {
		Error_Handler();
	}
	ui8_UART3_tx_buffer[0] = 0x55;
	ui8_UART3_tx_buffer[1] = 0xAA;
	MT.ESC_version = 0x0222;
	MT.internal_battery_version = 0x0289;
	MT.total_riding_time[0]=0xFFFF;
	MT.ESC_status_2= 0x0800;

    if (HAL_UART_Receive_DMA(&huart1, (uint8_t *)ui8_UART1_rx_buffer, sizeof(ui8_UART1_rx_buffer)) != HAL_OK)
     {
 	   Error_Handler();
     }



}

void search_DashboardMessage(MotorState_t *MS, MotorParams_t *MP, UART_HandleTypeDef huart3){

	ui8_recentpointerposition = 132 - (DMA1_Channel3->CNDTR); //Pointer of UART1RX DMA Channel
	if(ui8_recentpointerposition>ui8_oldpointerposition){
				ui8_messagelength=ui8_recentpointerposition-ui8_oldpointerposition;
				memcpy(ui8_dashboardmessage,ui8_UART3_rx_buffer+(ui8_oldpointerposition%132),ui8_messagelength);
				process_DashboardMessage( MS,  MP, (uint8_t*)&ui8_dashboardmessage,ui8_messagelength,huart3);

	}
	ui8_oldpointerposition=ui8_recentpointerposition;
}

void search_ControllerMessage(void){

	int start=0;

	for(int m=0; m<18; m++){
		if(ui8_UART1_rx_buffer[m]==0x55&&ui8_UART1_rx_buffer[(m+1)%18]==0xAA)start=m;
		}
	int laenge = ui8_UART1_rx_buffer[(start+2)%18]+6;
	for(int m=0; m<laenge; m++){
		ui8_dashboardmessage[m]=ui8_UART1_rx_buffer[(start+m)%18];
			}
	int command = ui8_dashboardmessage[4];
	//push through to Dashboard
	//HAL_HalfDuplex_EnableTransmitter(&huart3);
	//HAL_UART_Transmit_DMA(&huart3, (uint8_t*)ui8_controllermessage, laenge);
}

void process_DashboardMessage(MotorState_t *MS, MotorParams_t *MP, uint8_t *message, uint8_t length, UART_HandleTypeDef huart3 ){
	//while(HAL_UART_GetState(&huart1)!=HAL_UART_STATE_READY){}
	//HAL_Delay(2); // bad style, but wait for characters coming in, if message is longer than expected
	if(!checkCRC(message, length)){
	//55 AA 06 21 64 00 00 00 00 00 74 FF
	//55	AA	8	21	64	0	20	0	0	1	0	12	3F	FF

		switch (message[command]) {

		case 0x64: {
			memcpy(MS->dashboardmessage64,message,length);
			//push message through to Controller
			//HAL_UART_Transmit_DMA(&huart1, (uint8_t*)message, length);
			}
			break;

		case 0x65: {
			temp4 = message[Brake];
			temp5 = message[Throttle];
			temp6 = ui8_messagelength;
			if(message[Brake]<BRAKEOFFSET>>1)MS->error_state=brake;
			else if(MS->error_state==brake)MS->error_state=none;
			if(map(message[Brake],BRAKEOFFSET,BRAKEMAX,0,MP->regen_current)>0){

				if(MS->Speed>2){
					MS->i_q_setpoint_temp =map(message[Brake],BRAKEOFFSET,BRAKEMAX,0,MP->regen_current);
					// ramp down regen strength at the max voltage to avoid the BMS shutting down the battery.
					MS->i_q_setpoint_temp =-map(MS->Voltage,BATTERYVOLTAGE_MAX-1000,BATTERYVOLTAGE_MAX,MS->i_q_setpoint_temp,0);
					MS->brake_active=true;
				}
				else {
					MS->i_q_setpoint_temp =0;
					MS->brake_active=false;
					}
				}
			else{
				MS->i_q_setpoint_temp = map(message[Throttle],THROTTLEOFFSET,THROTTLEMAX,0,MP->phase_current_limit);
				MS->brake_active=false;
				}

			if(MS->i_q_setpoint_temp>-1){
				message[Throttle]=map(MS->i_q_setpoint,0,MP->phase_current_limit,THROTTLEOFFSET,THROTTLEMAX);
				addCRC((uint8_t*)message, length);
			}
			memcpy(MS->dashboardmessage65,message,length);
			//push message to Controller
			//HAL_UART_Transmit_DMA(&huart1, (uint8_t*)message, length);
			}
			break;


		default: {
		//	MS->i_q_setpoint = 0; // stop motor for safety reason
			}
			break;
		}//end switch


	}

}

void addCRC(uint8_t * message, uint8_t size){
    unsigned long cksm = 0;
    for(int i = 2; i < size - 2; i++) cksm += message[i];
    cksm ^= 0xFFFF;
    message[size - 2] = (uint8_t)(cksm&0xFF);
    message[size - 1] = (uint8_t)((cksm&0xFF00) >> 8);
    message[size] = '\0';
}

int16_t checkCRC(uint8_t * message, uint8_t size){
    unsigned long cksm = 0;
    for(int i = 2; i < size - 2; i++) cksm += message[i];
    cksm ^= 0xFFFF;
    return cksm-(message[size - 2]+(message[size - 1]<<8));
}

