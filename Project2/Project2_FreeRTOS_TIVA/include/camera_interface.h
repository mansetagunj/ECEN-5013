/*
 * camera_interface.h
 *
 *  Created on: 29-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef INCLUDE_CAMERA_INTERFACE_H_
#define INCLUDE_CAMERA_INTERFACE_H_



void CameraInit();
uint32_t SendFrame();
void I2C_init();

/* Ported functions from arducam arduino library*/
void wrSensorReg8_8(uint8_t reg, uint8_t data);
uint8_t rdSensorReg8_8(uint8_t reg);
void wrSensorRegs8_8(uint8_t **array_reg_value_pair);
uint8_t get_bit(uint8_t addr, uint8_t bit);
uint32_t read_fifo_length();
uint8_t read_reg(uint8_t reg);
void write_reg(uint8_t reg, uint8_t value);
uint8_t read_fifo_burst();
uint8_t transfer(uint8_t val);


static inline void write(uint8_t data)
{
//    UARTCharPut(UART0_BASE, data);
    UARTCharPut(UART3_BASE, data);
}

static inline void CS_LOW()
{
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
}

static inline void CS_HIGH()
{
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2);
}



#endif /* INCLUDE_CAMERA_INTERFACE_H_ */
