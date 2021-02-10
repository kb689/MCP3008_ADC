// MCP3008_ADC.c
// Runs on TM4C123.
// Connect to MCP38008 ADC using SSI2.
// Variable resistor connected to output channel CH0.
// Red LED on TM4C123 turns on once resistor reaches certain threshold value.
// Kirabo Nsereko
// 1/12/2021

#include "SSI2.h"

#define START_BYTE    0x01 // start byte to init MCP3808
#define CONTROL_BYTE  0x00 // single-ended channel on CH0. see datasheet
#define ADC_OUPUT      512 // ADC output at which LED turns on. Max=1024, Min=0.

//void PortF_Init(void);
void LED_Init(void); // Init PB2 for output to an LED  

uint8_t data1;
uint8_t data2;
uint8_t data3;
uint16_t ADCdata;

int main(void){
	int i;
	//uint8_t data1;
	//uint8_t data2;
	//uint8_t data3;
	//uint16_t ADCdata; // digital value output from ADC
  SSI2_Init();
	LED_Init();
	
	GPIO_PORTB_DATA_R |= 0x20; // set PB5 high
	
	while(1){
		
		// set SSI2Fss low
	  GPIO_PORTB_DATA_R &= ~0x20; // set PB5 low
			
	  Write_Byte(START_BYTE);
		Read_Byte(&data1);
		Write_Byte(CONTROL_BYTE);
		Read_Byte(&data2);
    Write_Byte(0x00);
		Read_Byte(&data3);
		
		GPIO_PORTB_DATA_R |= 0x20; // set PB5 high
		
		data2 = data2&0x03; // keep two bits
		ADCdata = (data2<<8)|data3;
		// turn of red LED if variable resistor at 50% resistance
		if (ADCdata >= ADC_OUPUT){ 
		  GPIO_PORTB_DATA_R |= (1<<2);
		} else{
		  GPIO_PORTB_DATA_R &= ~(1<<2);
		}
	}
}

// Init PB2 for output to an LED
// Input: none
// Output: none
void LED_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;     // 1) activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTB_AMSEL_R &= ~0x04;        // 3) disable analog
  GPIO_PORTB_DIR_R |= 0x04;          // 5) PB2 out
  GPIO_PORTB_AFSEL_R &= ~0x04;        // 6) disable alt funct on PB2
  GPIO_PORTB_DEN_R |= 0x04;          // 7) enable digital I/O on PB2
}
