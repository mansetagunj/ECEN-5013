/**
* @file - nordic_driver.h
* @brief - Header file for the driver functions of the NRF240L
*
* @author Gunj University of Colorado Boulder
* @date - 19th April 2017
**/


#ifndef __NORDIC_DRIVER_H__
#define __NORDIC_DRIVER_H__

#if 1

#include <stdbool.h>
#include <stdint.h>

#include "delay.h"

#include "spi.h"
#include "mraa/spi.h"
#include "mraa/gpio.h"


#define NORDIC_STATUS_RX_DR_MASK			(1<<6)
#define NORDIC_STATUS_TX_DS_MASK			(1<<5)
#define NORDIC_STATUS_MAX_RT_MASK           (1<<4)

typedef void (*NRF_INT_HANDLER_T)(void);

typedef enum{

	NRF_Mode_TX = 0,
	NRF_Mode_RX = 1

}NRF_Mode_t;

typedef enum{

    NRF_DR_1Mbps = 0,
    NRF_DR_2Mbps = 1

}NRF_DataRate_t;

typedef enum{

    NRF_PW_LOW = 0,
    NRF_PW_MED = 2,
    NRF_PW_HIGH = 3

}NRF_Power_t;

extern mraa_gpio_context NRF_CSN_GPIO;
extern mraa_gpio_context NRF_CE_GPIO;


/**
* @brief - Enable the chip select connection to Nordic
* @return void
**/
static inline void NRF_chip_enable()
{
	mraa_result_t status = mraa_gpio_write(NRF_CSN_GPIO, 0);
    if (status != MRAA_SUCCESS) 
    {
    }
	DelayUs(50);
}

/**
* @brief - Disable the chip select connection to Nordic
* @return void
**/
static inline void NRF_chip_disable()
{
    mraa_result_t status = mraa_gpio_write(NRF_CSN_GPIO, 1);
    if (status != MRAA_SUCCESS) 
    {
    }
}

/**
* @brief - Enable TX/RX from the Nordic module
* @return void
**/
static inline void NRF_radio_enable()
{
    mraa_result_t status = mraa_gpio_write(NRF_CE_GPIO, 1);
    if (status != MRAA_SUCCESS) 
    {
    }
}

/**
* @brief - Disable TX/RX from the Nordic module
* @return void
**/
static inline void NRF_radio_disable()
{
    mraa_result_t status = mraa_gpio_write(NRF_CE_GPIO, 0);
    if (status != MRAA_SUCCESS) 
    {
    }
}

/**
* @brief - Initialize the NRF module
* Initialized the GPIO connections pertaining to the Nordic module
* @return int8_t
**/
int8_t NRF_moduleInit(uint8_t use_interrupt, NRF_INT_HANDLER_T handler);

/**
* @brief - Disable the GPIO connections set up earlier for the Nordic module
* @return void
**/
void NRF_moduleDisable();

/**
* @brief - Read a register from the NRF module
* @param - regAdd uint8_t
* @return uint8_t
**/
uint8_t NRF_read_register(uint8_t regAdd);

/**
* @brief - Write to a register from the NRF module
* @param - regAdd uint8_t
* @param - value uint8_t
* @return void
**/
void NRF_write_register(uint8_t regAdd, uint8_t value);

/**
* @brief - Write to the NRF module's status register
* @param - statusValue uint8_t
* @return void
**/
void NRF_write_status(uint8_t statusValue);

/**
* @brief - Read the NRF module's status register
* @return uint8_t
**/
uint8_t NRF_read_status();

/**
* @brief - Write to the NRF module's config register
* @param - configValue uint8_t
* @return void
**/
void NRF_write_config(uint8_t configValue);

/**
* @brief - Read the NRF module's config register
* @return uint8_t
**/
uint8_t NRF_read_config();


/**
* @brief - Read the NRF module's RF setup register
* @return uint8_t
**/
uint8_t NRF_read_rf_setup();

/**
* @brief - Write to the NRF module's RF setup register
* @param - rfStatusValue uint8_t
* @return void
**/
void NRF_write_rf_setup(uint8_t rfSetupValue);

/**
* @brief - Read the NRF module's RF CH register
* @return uint8_t
**/
uint8_t NRF_read_rf_ch();

/**
* @brief - Write to the NRF module's RF CH register
* @param - channel uint8_t
* @return void
**/
void NRF_write_rf_ch(uint8_t channel);

/**
* @brief - Reads 5 bytes of the NRF module's TX ADDR register
* @param - address uint8_t *
* @return void
**/
void NRF_read_TX_ADDR(uint8_t * address);

/**
* @brief - Writes 5 bytes of the NRF module's TX ADDR register
* @param - tx_addr uint8_t *
* @return void
**/
void NRF_write_TX_ADDR(uint8_t * tx_addr);

/**
* @brief - Read the NRF module's FIFO status register
* @return address uint8_t
**/
uint8_t NRF_read_fifo_status();

/**
* @brief - Send the command FLUSH_TX to the NRF module
* @return void
**/
void NRF_flush_tx_fifo();

/**
* @brief - Send the command FLUSH_RX to the NRF module
* @return void
**/
void NRF_flush_rx_fifo();

/**
* @brief - Send the activation command to the NRF module
* Activates the features: R_RX_PL_WID, W_ACK_PAYLOAD, W_TX_PAYLOAD_NOACK
* @return void
**/

void NRF_moduleSetup(NRF_DataRate_t DR, NRF_Power_t power);

void NRF_write_status(uint8_t statusValue);

uint8_t NRF_read_status();

void NRF_activate_cmd();

void NRF_read_RX_PIPE_ADDR(uint8_t pipe_num, uint8_t *address);
void NRF_write_RX_PIPE_ADDR(uint8_t pipe_num, uint8_t *rx_addr);

void NRF_write_En_AA(uint8_t data);
uint8_t NRF_read_En_AA();
void NRF_write_setup_retry(uint8_t data);
uint8_t NRF_read_setup_retry();

int32_t NRF_read_data(uint8_t *data, uint8_t len);
int32_t NRF_transmit_data(uint8_t *data, uint8_t len, uint8_t toRXMode);

void NRF_write_TXPayload(uint8_t *data, uint8_t len);
void NRF_TX_pulse();


void NRF_openReadPipe(uint8_t rx_pipe_number, uint8_t rx_addr[5], uint8_t payload_size);

void NRF_openWritePipe(uint8_t tx_addr[5]);

void NRF_closeWritePipe();

void NRF_closeReadPipe(uint8_t rx_pipe_number);



#endif /* __NORDIC_DRIVER_H__ */
#endif
