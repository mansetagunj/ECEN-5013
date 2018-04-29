/*
 * spi.c
 *
 *  Created on: Dec 1, 2017
 *      Author: Gunj Manseta
 */

#if 1
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "spi.h"


#define CHECK_SPI_NUM(spi)       ({if(spi > NUM_SPI_BUS-1)   \
                                return 1;})

static uint32_t opened[NUM_SPI_BUS] = {0};

SPI_Type SPI[NUM_SPI_BUS] = {NULL,NULL,NULL,NULL};

static SPI_t SPI_release(SPI_t spi)
{
    if(spi > NUM_SPI_BUS-1)   
        return -1;
    
    mraa_result_t status = mraa_spi_stop(SPI[spi]);
    if (status != MRAA_SUCCESS)
        return -1;
    return spi;
}

SPI_t SPI_init(SPI_t spi)
{
    if(spi > NUM_SPI_BUS-1)   
        return -1;

    if(opened[spi] && SPI[spi] != NULL)
    {
        opened[spi]++;
        return spi;   
    }
    mraa_result_t status = MRAA_SUCCESS;

    mraa_spi_context spi_context = mraa_spi_init(spi);
    if(spi_context == NULL)
    {
        return 0;
    }
    status = mraa_spi_frequency(SPI[spi], SPI_2MZ);
    if (status != MRAA_SUCCESS)
    {
        return SPI_release(spi);
    }
    SPI[spi] = spi_context;
    opened[spi]++;
    return spi;
}

void SPI_GPIO_init(SPI_t spi)
{
}

SPI_t SPI_disable(SPI_t spi)
{
    if(spi > NUM_SPI_BUS-1)   
        return -1;

    opened[spi]--;
    if(opened[spi])
    {
        return spi;
    }
    else
    {
        return SPI_release(spi);
    }
}

int8_t SPI_write_packet(SPI_t spi, uint8_t* p, size_t length)
{
    CHECK_SPI_NUM(spi);

	uint8_t i=0;
	while (i<length)
	{
		SPI_write_byte(spi, *(p+i));
		++i;
	}
    return length;
}

int32_t  SPI_read_packet(SPI_t spi, uint8_t* p, size_t length)
{
    CHECK_SPI_NUM(spi);
	uint8_t i=0;
	while (i<length)
	{
		*(p+i) = SPI_read_byte(spi);
		++i;
	}
    return length;
}

void SPI0_IRQHandler()
{

}

#endif