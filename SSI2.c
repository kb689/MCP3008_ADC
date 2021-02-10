// SSI2.c
// Runs on TM4C123.
// Enables SSI module 2.
// SSI operates in Freescale mode.
// SPO=0
// SPH=0
// TM4C123 pins:
// PB4: SSI2Clk
// PB5: SSI2Fss (manually control Fss)
// PB6: SSI2Rx
// PB7: SSI2Tx
//
// Kirabo Nsereko
// 1/12/2021


#include <stdint.h>
#include "tm4c123gh6pm.h"


// Initializes SSI2.
// Config:
// PB4: SSI2Clk
// PB5: SSI2Fss (manually control Fss)
// PB6: SSI2Rx
// PB7: SSI2Tx
// TM4C123 master
// SSIclk @ 2Mz
// Freescale mode (0,0)
// SPO=0
// SPH=0
// 8-bit data size 
void SSI2_Init(void){
	volatile uint32_t delay;
  //	To enable and initialize the SSI, the following steps are necessary:
  //  1. Enable the SSI module using the RCGCSSI register (see page 346).
	SYSCTL_RCGCSSI_R |= 0x04; // SSI2
  //  2. Enable the clock to the appropriate GPIO module via the RCGCGPIO register (see page 340).
  //  To find out which GPIO port to enable, refer to Table 23-5 on page 1351.
	SYSCTL_RCGCGPIO_R |= 0x02; // PortB
	delay = SYSCTL_RCGCGPIO_R; // wait for clock to stabilize
  //  3. Set the GPIO AFSEL bits for the appropriate pins (see page 671). To determine which GPIOs to
  //  configure, see Table 23-4 on page 1344.
	GPIO_PORTB_AFSEL_R |= (1<<7)|(1<<6)|(1<<4); // AFSEL on PB4,6,7
	GPIO_PORTB_AFSEL_R &= ~(1<<5); // disable alternate function on PB5
  //  4. Configure the PMCn fields in the GPIOPCTL register to assign the SSI signals to the appropriate
  //  pins. See page 688 and Table 23-5 on page 1351.
	GPIO_PORTB_PCTL_R |= (2<<28)|(2<<24)|(2<<20)|(2<<16);
  //  5. Program the GPIODEN register to enable the pin's digital function. In addition, the drive strength,
  //  drain select and pull-up/pull-down functions must be configured. Refer to “General-Purpose
  //  Input/Outputs (GPIOs)” on page 649 for more information.
  //  Note: Pull-ups can be used to avoid unnecessary toggles on the SSI pins, which can take the
  //  slave to a wrong state. In addition, if the SSIClk signal is programmed to steady state
  //  High through the SPO bit in the SSICR0 register, then software must also configure the
  //  GPIO port pin corresponding to the SSInClk signal as a pull-up in the GPIO Pull-Up
  //  Select (GPIOPUR) register.
	GPIO_PORTB_DEN_R |= (1<<7)|(1<<6)|(1<<5)|(1<<4);
	GPIO_PORTB_DIR_R |= (1<<5); // PB5 output 
  //  For each of the frame formats, the SSI is configured using the following steps:
  //  1. Ensure that the SSE bit in the SSICR1 register is clear before making any configuration changes.
	SSI2_CR1_R &= ~SSI_CR1_SSE;
  //  2. Select whether the SSI is a master or slave:
  //    a. For master operations, set the SSICR1 register to 0x0000.0000.
	SSI2_CR1_R &= ~SSI_CR1_MS;
  //	  b. For slave mode (output enabled), set the SSICR1 register to 0x0000.0004.
  //    c. For slave mode (output disabled), set the SSICR1 register to 0x0000.000C.
  //  3. Configure the SSI clock source by writing to the SSICC register.
	//SSInClk = SysClk / (CPSDVSR * (1 + SCR))
  //2x106 = 16x106 / (CPSDVSR * (1 + SCR))
	SSI2_CC_R &= ~0x0F; // system clock
  //  4. Configure the clock prescale divisor by writing the SSICPSR register.
	SSI2_CPSR_R |= 0x04; // CPSDVSR = 0x04
  //  5. Write the SSICR0 register with the following configuration:
  //    ¦ Serial clock rate (SCR)
	SSI2_CR0_R |= (1<<8); // SCR = 1
  //    ¦ Desired clock phase/polarity, if using Freescale SPI mode (SPH and SPO)
	SSI2_CR0_R &= ~(1<<6); // SPO=0
	SSI2_CR0_R &= ~(1<<7); // SPH=0
  //    ¦ The protocol mode: Freescale SPI, TI SSF, MICROWIRE (FRF)
	SSI2_CR0_R &= ~(1<<4); // Freescale mode
  //    ¦ The data size (DSS)
	SSI2_CR0_R |= SSI_CR0_DSS_8;
  //  6. Optionally, configure the SSI module for µDMA use with the following steps:
  //    a. Configure a µDMA for SSI use. See “Micro Direct Memory Access (µDMA)” on page 585 for
  //       more information.
  //    b. Enable the SSI Module's TX FIFO or RX FIFO by setting the TXDMAE or RXDMAE bit in the
  //       SSIDMACTL register.
  //  7. Enable the SSI by setting the SSE bit in the SSICR1 register.
	SSI2_CR1_R |= SSI_CR1_SSE;
}

// Writes byte of data to SSI2 data register.
// Inputs: data to transmit
// Outputs: none
void Write_Byte(uint8_t data){
  SSI2_DR_R = data;
	while ((SSI2_SR_R&SSI_SR_BSY) != 0){}; // wait until SSI idle
}

// Reads byte of data from SSI2 data register.
// Inputs: address of variable to store receives data
// Outputs: none
void Read_Byte(uint8_t *data){
  while ((SSI2_SR_R&SSI_SR_BSY) != 0){}; // wait until SSI idle
	(*data) = SSI2_DR_R&0xFF;
}
