/**
* @file - spi.h
* @brief - Header file for the library functions for SPI
*
* @author Gunj University of Colorado Boulder
* @date - 19th April 2018
**/

#ifndef _SPI_H_
#define _SPI_H_

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/debug.h"

#include "my_uart.h"

#define SPI_1MZ 1000000
#define SPI_2MZ 2000000

/**
* @brief - Enum to allow flexibility of selection between SPI0 and SPI1
**/
typedef enum{
	SPI_0,
	SPI_1,
	SPI_2,
	SPI_3
}SPI_t;

typedef uint32_t SPI_Type;
typedef uint32_t SPI_SYSCTL_Type;

extern const SPI_Type SPI[4];

extern const SPI_SYSCTL_Type SPI_SYSCTL[4];

/**
* @brief - Initialize the GPIO pins associated with SPI
* Configure SPI in 3 wire mode and use a GPIO pin for chip select
* @param - spi SPI_t
* @return void
**/
void SPI_GPIO_init(SPI_t spi);

/**
* @brief - Enable the clock gate control for SPI
* @param - spi SPI_t
* @return void
**/
static inline void SPI_clock_init(SPI_t spi, uint32_t g_sysclock)
{
    MAP_SysCtlPeripheralEnable(SPI_SYSCTL[spi]);
    uint32_t src = SSIClockSourceGet(SPI[spi]);
    if(src == SSI_CLOCK_SYSTEM)
    {
        printf("SSI Using System Clock\n");
    }
    else if(src == SSI_CLOCK_PIOSC)
    {
        printf("SSI Using PIOSC\n");
    }

    //SSIAdvModeSet(SPI[spi], SSI_ADV_MODE_LEGACY);

    SSIConfigSetExpClk(SPI[spi], g_sysclock, SSI_FRF_MOTO_MODE_0,
                           SSI_MODE_MASTER, SPI_1MZ, 8);

}



/**
* @brief - Perform the initialization routine for the SPI module
* @param - spi SPI_t
* @return void
**/
static inline void SPI_init(SPI_t spi /*, uint32_t g_sysclock*/)
{
//    SSIConfigSetExpClk(SPI[spi], g_sysclock, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, SPI_1MZ, 8);
    SPI_GPIO_init(spi);
    SSIEnable(SPI[spi]);
}

/**
* @brief - Disable the GPIO pins earlier initialized for the SPI module
* @return void
**/
static inline void SPI_disable(SPI_t spi)
{
    SSIDisable(SPI[spi]);
    MAP_SysCtlPeripheralDisable(SPI_SYSCTL[spi]);
}

/**
* @brief - Blocks until SPI transmit buffer has completed transmitting
* @param - spi SPI_t
* @return void
**/
static inline void SPI_flush(SPI_t spi)
{
    while(SSIBusy(SPI[spi]));
}

static inline void SPI_flushRXFIFO(SPI_t spi)
{
    uint32_t garbage;
    while(SSIDataGetNonBlocking(SPI[spi], &garbage));
}


/**
* @brief - Read a single byte from the SPI bus
* @param - spi SPI_t
* @return uint8_t
**/
static inline uint8_t SPI_read_byte(SPI_t spi)
{
    uint32_t data;
    SSIDataGet(SPI[spi], &data);
    return ((uint8_t)(data & 0xFF));
}

/**
* @brief - Read a single byte from the SPI bus without waiting
* @param - spi SPI_t
* @return uint8_t
**/
static inline uint8_t SPI_read_byte_NonBlocking(SPI_t spi)
{

    uint32_t data;
    SSIDataGetNonBlocking(SPI[spi], &data);
    return ((uint8_t)(data & 0xFF));
}

/**
* @brief - Write a single byte on to the SPI bus
* @param - spi SPI_t
* @param - byte uint8_t
* @return void
**/
static inline void SPI_write_byte(SPI_t spi, uint8_t byte)
{
    SSIDataPut(SPI[spi],((uint32_t)byte & 0x000000FF));
    SPI_flush(spi);
}

/**
* @brief - Write a single byte on to the SPI bus without blocking
* @param - spi SPI_t
* @param - byte uint8_t
* @return void
**/
static inline void SPI_write_byte_NonBlocking(SPI_t spi, uint8_t byte)
{

    SPI_flush(spi);
    SSIDataPutNonBlocking(SPI[spi],((uint32_t)byte & 0x000000FF));
    SPI_flush(spi);
}

/**
* @brief - Send a packet on to the SPI bus
* Send multiple bytes given a pointer to an array and the number of bytes to be sent
* @param - spi SPI_t
* @param - p uint8_t
* @param - length size_t
* @return void
**/
void SPI_write_packet(SPI_t spi, uint8_t* p, size_t length);

/**
* @brief - Read a packet from the SPI bus
* Read multiple bytes given a pointer to an array for storage and the number of bytes to be read
* @param - spi SPI_t
* @param - p uint8_t *
* @param - length size_t
* @return void
**/
void SPI_read_packet(SPI_t spi, uint8_t* p, size_t length);


#endif /* SOURCES_SPI0_H_ */
