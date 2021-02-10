// SSI2.h
// Runs on TM4C123.
// Enables SSI module 2.
// SSI operates in Freescale mode.
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

#ifndef SSI2_H
#define SSI2_H

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
void SSI2_Init(void);

// Writes byte of data to SSI2 data register.
// Inputs: data to transmit
// Outputs: none
void Write_Byte(uint8_t data);

// Reads byte of data from SSI2 data register.
// Inputs: address of variable to store receives data
// Outputs: none
void Read_Byte(uint8_t *data);

#endif 
