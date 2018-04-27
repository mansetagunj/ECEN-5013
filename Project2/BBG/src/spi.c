/*
 * spi.c
 *
 *  Created on: Dec 1, 2017
 *      Author: Gunj Manseta
 */

#if 0
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "spi.h"

static SPI_Type SPI[4] = {NULL,NULL,NULL,NULL};

void SPI_GPIO_init(SPI_t spi)
{

    if(spi==SPI_0)
    {
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
        GPIOPinConfigure(GPIO_PA3_SSI0FSS);
        GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
        GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);

        // The pins are assigned as follows:
        //      PA5 - SSI0Tx
        //      PA4 - SSI0Rx
        //      PA3 - SSI0Fss
        //      PA2 - SSI0CLK
        GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
                       GPIO_PIN_2);
	}
    else if(spi==SPI_1)
    {
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
        GPIOPinConfigure(GPIO_PB5_SSI1CLK);
        GPIOPinConfigure(GPIO_PB4_SSI1FSS);
        GPIOPinConfigure(GPIO_PE4_SSI1XDAT0);
        GPIOPinConfigure(GPIO_PE5_SSI1XDAT1);

        // The pins are assigned as follows:
        //      PE4 - SSI0Tx
        //      PE5 - SSI0Rx
        //      PB4 - SSI0Fss
        //      PB5 - SSI0CLK
        GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_5 | GPIO_PIN_4);
        GPIOPinTypeSSI(GPIO_PORTE_BASE, GPIO_PIN_5 | GPIO_PIN_4);
    }
}


void SPI_write_packet(SPI_t spi, uint8_t* p, size_t length)
{
	uint8_t i=0;
	while (i<length)
	{
		SPI_write_byte(spi, *(p+i));
		++i;
	}
}

void SPI_read_packet(SPI_t spi, uint8_t* p, size_t length)
{
	uint8_t i=0;
	while (i<length)
	{
	    SPI_write_byte(spi,0xFF);
		*(p+i) = SPI_read_byte(spi);
		++i;
	}
}

void SPI0_IRQHandler()
{

}

#endif