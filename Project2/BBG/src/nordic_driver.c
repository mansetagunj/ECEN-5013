/**
 * @brief Implementation file for the driver functions of the NRF240L
 * 
 * @file nordic_driver.c
 * @author Gunj Manseta
 * @date 2018-04-28
 */

#if 1

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "my_uart.h"

#include "nordic_driver.h"
#include "mraa/spi.h"
#include "mraa/gpio.h"

//Commands Byte
#define NORDIC_TXFIFO_FLUSH_CMD	(0xE1)
#define NORDIC_RXFIFO_FLUSH_CMD	(0xE2)
#define NORDIC_W_TXPAYLD_CMD	(0xA0)
#define NORDIC_R_RXPAYLD_CMD	(0x61)
#define NORDIC_ACTIVATE_CMD		(0x50)
#define NORDIC_ACTIVATE_DATA	(0x73)
#define NORDIC_RXPAYLD_W_CMD	(0x60)
#define NORDIC_NOP				(0xFF)
//Register Addresses
#define NORDIC_CONFIG_REG		(0x00)
#define NORDIC_EN_AA_REG        (0x01)
#define NORDIC_EN_RXADDR_REG    (0x02)
#define NODIC_SETUP_RETR_REG    (0x04)
#define NORDIC_STATUS_REG 		(0x07)
#define NORDIC_RF_SETUP_REG		(0x06)
#define NORDIC_RF_CH_REG		(0x05)
#define NORDIC_TX_ADDR_REG		(0x10)
#define NORDIC_TX_ADDR_LEN		(5)

#define NORDIC_RX_ADDR_P0_REG   (0x0A)
#define NORDIC_RX_ADDR_P1_REG   (0x0B)
#define NORDIC_RX_ADDR_P2_REG   (0x0C)
#define NORDIC_RX_ADDR_P3_REG   (0x0D)
#define NORDIC_RX_ADDR_P4_REG   (0x0E)
#define NORDIC_RX_ADDR_P5_REG   (0x0F)

#define NORDIC_FIFO_STATUS_REG	(0x17)
#define NORDIC_RX_PW_P0_REG		(0x11)

#define DEFAULT_TX_ADDRESS_1B	(0xE7)
#define DEFAULT_TX_ADDRESS_2B	(0xE7)
#define DEFAULT_TX_ADDRESS_3B	(0xE7)
#define DEFAULT_TX_ADDRESS_4B	(0xE7)
#define DEFAULT_TX_ADDRESS_5B	(0xE7)

//Masks
#define NORDIC_CONFIG_MAX_RT_MASK       4
#define NORDIC_CONFIG_MAX_RT_INT(x)     ((((uint8_t)x)<<NORDIC_CONFIG_MAX_RT_MASK)&(1<<NORDIC_CONFIG_MAX_RT_MASK))

#define NORDIC_CONFIG_RX_DR_MASK		6
#define NORDIC_CONFIG_RX_DR_INT(x)		((((uint8_t)x)<<NORDIC_CONFIG_RX_DR_MASK)&(1<<NORDIC_CONFIG_RX_DR_MASK))

#define NORDIC_CONFIG_TX_DS_MASK		5
#define NORDIC_CONFIG_TX_DS_INT(x)		((((uint8_t)x)<<NORDIC_CONFIG_TX_DS_MASK)&(1<<NORDIC_CONFIG_TX_DS_MASK))

#define NORDIC_CONFIG_PWR_UP_MASK		1
#define NORDIC_CONFIG_PWR_UP(x)			((((uint8_t)x)<<NORDIC_CONFIG_PWR_UP_MASK)&(1<<NORDIC_CONFIG_PWR_UP_MASK))

#define NORDIC_CONFIG_PRIM_RX_MASK		0
#define NORDIC_CONFIG_PRIM_RX(x)		((((uint8_t)x)<<NORDIC_CONFIG_PRIM_RX_MASK)&(1<<NORDIC_CONFIG_PRIM_RX_MASK))


#define NORDIC_STATUS_TX_FULL_MASK			(1<<0)
#define NORDIC_FIFO_STATUS_TX_FULL_MASK		(1<<5)
#define NORDIC_FIFO_STATUS_RX_FULL_MASK		(1<<1)
#define NORDIC_FIFO_STATUS_TX_EMPTY_MASK	(1<<4)
#define NORDIC_FIFO_STATUS_RX_EMPTY_MASK	(0<<5)

#define NORDIC_INT_MAXRT_MASK   (1<<3)
#define NORDIC_INT_TXDS_MASK    (1<<4)
#define NORDIC_INT_TXDR_MASK    (1<<5)

volatile uint8_t txconfigured = 0;
volatile uint8_t rxconfigured = 0;

volatile uint8_t transmitted = 0;
volatile uint8_t received = 0;
volatile uint8_t retry_error = 0;

static uint8_t using_interrupt = 0;



#define NRF_SPI_BUS   0

void NRF_IntHandler(void *args);

static NRF_INT_HANDLER_T user_handler;

#define NORDIC_CE_PIN_MRAA      73
#define NORDIC_CSN_PIN_MRAA     70
#define NORDIC_IRQ_PIN_MRAA     69

mraa_gpio_context NRF_CSN_GPIO = 0;
mraa_gpio_context NRF_CE_GPIO = 0;
static mraa_gpio_context NRF_IRQ_GPIO = 0;


int8_t NRF_gpioInit()
{
    mraa_result_t status = MRAA_SUCCESS;
    NRF_CSN_GPIO = mraa_gpio_init(NORDIC_CSN_PIN_MRAA);
    if(NRF_CSN_GPIO == NULL)
    {
        goto ERR_CSN;
    }

    NRF_CE_GPIO = mraa_gpio_init(NORDIC_CE_PIN_MRAA);
    if(NRF_CE_GPIO == NULL)
    {
        goto ERR_CE;
    }
    NRF_IRQ_GPIO = mraa_gpio_init(NORDIC_IRQ_PIN_MRAA);
    if(NRF_IRQ_GPIO == NULL)
    {
        goto ERR_IRQ;
    }

    status = mraa_gpio_dir(NRF_CSN_GPIO, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) 
    {
        goto ERR;
    }

    status = mraa_gpio_dir(NRF_CE_GPIO, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) 
    {
        goto ERR;
    }

    status = mraa_gpio_dir(NRF_IRQ_GPIO, MRAA_GPIO_IN);
    if (status != MRAA_SUCCESS) 
    {
        goto ERR;
    }
    return 1;

ERR:
    status = mraa_gpio_close(NRF_IRQ_GPIO);
ERR_IRQ:
    status = mraa_gpio_close(NRF_CE_GPIO);
ERR_CE:
    status = mraa_gpio_close(NRF_CSN_GPIO);
ERR_CSN:
    return -1;
}

int8_t NRF_moduleInit(uint8_t use_interrupt, NRF_INT_HANDLER_T handler)
{
    SPI_clock_init(NRF_SPI_BUS, 0);
    if(SPI_init(NRF_SPI_BUS))
        return -1;
        
    DelayMs(1);

    if(NRF_gpioInit())
        return -1;

    if(use_interrupt)
    {
        using_interrupt = 1;
        user_handler = handler;
        mraa_result_t  status = mraa_gpio_isr(NRF_IRQ_GPIO, MRAA_GPIO_EDGE_FALLING, &NRF_IntHandler, NULL);
        if(status != MRAA_SUCCESS) 
        {
            status = mraa_gpio_close(NRF_IRQ_GPIO);
            status = mraa_gpio_close(NRF_CE_GPIO);
            status = mraa_gpio_close(NRF_CSN_GPIO);
            using_interrupt = 0;
            return -1;
        }
    }
    else
    {
        using_interrupt = 0;
    }
    return 1;
}

void NRF_moduleSetup(NRF_DataRate_t DR, NRF_Power_t power)
{
    //Clearing all interrupts
    NRF_write_status(0);
    //Disabling all interrupts and init in power down TX mode
    NRF_write_config(0x78);
    NRF_write_rf_ch(44);
    NRF_write_rf_setup((power<<1) | (DR<<3) | 1);
    //ADDR LEN as 5bytes
    NRF_write_register(0x03, 0x03);
    DelayMs(1);
}

void NRF_moduleDisable()
{
    using_interrupt = 0;
    uint8_t config = NRF_read_config();
    NRF_write_config(config & ~NORDIC_CONFIG_PWR_UP(1));
	SPI_disable(NRF_SPI_BUS);
    mraa_result_t status = mraa_gpio_close(NRF_IRQ_GPIO);
    status = mraa_gpio_close(NRF_CE_GPIO);
    status = mraa_gpio_close(NRF_CSN_GPIO);
}

uint8_t NRF_read_register(uint8_t regAdd)
{
	//SPI_clear_RXbuffer(NRF_SPI_BUS);	//used to clear the previously value in the RX FIFO
	uint8_t readValue = 0;

	//CSN High to low for new command
	NRF_chip_disable();
	NRF_chip_enable();

	SPI_write_byte(NRF_SPI_BUS,regAdd);
	readValue = SPI_read_byte(NRF_SPI_BUS);

	//Marking the end of transaction by CSN high
	NRF_chip_disable();

	return readValue;
}

void NRF_write_command(uint8_t command)
{
	//CSN High to low for new command
	NRF_chip_disable();
	NRF_chip_enable();

	SPI_write_byte(NRF_SPI_BUS,command);
	//SPI_read_byte(NRF_SPI_BUS);

	//Marking the end of transaction by CSN high
	NRF_chip_disable();
}

void NRF_write_register(uint8_t regAdd, uint8_t value)
{
	//SPI_clear_RXbuffer(NRF_SPI_BUS);	//used to clear the previously value in the RX FIFO

	//CSN High to low for new command
	NRF_chip_disable();
	NRF_chip_enable();

	uint8_t ret = SPI_write_byte(NRF_SPI_BUS,regAdd | 0x20);
	//SPI_read_byte(NRF_SPI_BUS);	//used to clear the previously value in the RX FIFO
	ret = SPI_write_byte(NRF_SPI_BUS,value);
	//SPI_read_byte(NRF_SPI_BUS);	//used to clear the previously value in the RX FIFO

	//Marking the end of transaction by CSN high
	NRF_chip_disable();
}

void NRF_write_status(uint8_t statusValue)
{
	NRF_write_register(NORDIC_STATUS_REG, statusValue);
}

uint8_t NRF_read_status()
{
    uint8_t readValue = 0;

    //CSN High to low for new command
    NRF_chip_disable();
    NRF_chip_enable();

    readValue = SPI_write_byte(NRF_SPI_BUS,NORDIC_NOP);
    //readValue = SPI_read_byte(NRF_SPI_BUS);   //used to clear the previously value in the RX FIFO

    //Marking the end of transaction by CSN high
    NRF_chip_disable();

    return readValue;
}

void NRF_write_config(uint8_t configValue)
{
	NRF_write_register(NORDIC_CONFIG_REG, configValue);
}

uint8_t NRF_read_config()
{
	return NRF_read_register(NORDIC_CONFIG_REG);
}

uint8_t NRF_read_rf_setup()
{
	return NRF_read_register(NORDIC_RF_SETUP_REG);
}

void NRF_write_rf_setup(uint8_t rfSetupValue)
{
	NRF_write_register(NORDIC_RF_SETUP_REG, rfSetupValue);
}

uint8_t NRF_read_rf_ch()
{
	return NRF_read_register(NORDIC_RF_CH_REG);
}

void NRF_write_rf_ch(uint8_t channel)
{
	NRF_write_register(NORDIC_RF_CH_REG, channel);
}

void NRF_write_En_AA(uint8_t data)
{
    NRF_write_register(NORDIC_EN_AA_REG, data);
}

uint8_t NRF_read_En_AA()
{
    return NRF_read_register(NORDIC_EN_AA_REG);
}

void NRF_write_setup_retry(uint8_t data)
{
    NRF_write_register(NODIC_SETUP_RETR_REG, data);
}

uint8_t NRF_read_setup_retry()
{
    return NRF_read_register(NODIC_SETUP_RETR_REG);
}

void NRF_read_TX_ADDR(uint8_t *address)
{
	uint8_t i = 0;

	NRF_chip_disable();
	NRF_chip_enable();

	SPI_write_byte(NRF_SPI_BUS,NORDIC_TX_ADDR_REG);
	//SPI_read_byte(NRF_SPI_BUS);	//used to clear the previously value in the RX FIFO
	//SPI_read_byte(NRF_SPI_BUS);	//used to clear the previously value in the RX FIFO
	while(i < NORDIC_TX_ADDR_LEN)
	{
		SPI_write_byte(NRF_SPI_BUS, 0xFF);	//Dummy to get the data
		*(address+i) = SPI_read_byte(NRF_SPI_BUS);
		i++;
	}

	NRF_chip_disable();
}

void NRF_write_TX_ADDR(uint8_t * tx_addr)
{
	NRF_chip_disable();
	NRF_chip_enable();

	SPI_write_byte(NRF_SPI_BUS,NORDIC_TX_ADDR_REG | 0x20);
	//SPI_read_byte(NRF_SPI_BUS);	//used to clear the previously value in the RX FIFO
	SPI_write_packet(NRF_SPI_BUS,tx_addr,NORDIC_TX_ADDR_LEN);
	//SPI_flushRXFIFO(NRF_SPI_BUS);

	NRF_chip_disable();
}

void NRF_read_RX_PIPE_ADDR(uint8_t pipe_num, uint8_t *address)
{
    if(pipe_num > 5)
        return;
//    uint8_t i = 0;

    NRF_chip_disable();
    NRF_chip_enable();

    SPI_write_byte(NRF_SPI_BUS,(NORDIC_RX_ADDR_P0_REG + pipe_num));
    size_t ADDR_LEN = NORDIC_TX_ADDR_LEN;
    pipe_num > 2 ? ADDR_LEN = 1: 0;
    SPI_read_packet(NRF_SPI_BUS, address, ADDR_LEN);

    NRF_chip_disable();
}

void NRF_write_RX_PIPE_ADDR(uint8_t pipe_num, uint8_t *rx_addr)
{
    if(pipe_num > 5)
        return;

    NRF_chip_disable();
    NRF_chip_enable();

    SPI_write_byte(NRF_SPI_BUS,(NORDIC_RX_ADDR_P0_REG + pipe_num) | 0x20);
    //SPI_read_byte(NRF_SPI_BUS);   //used to clear the previously value in the RX FIFO
    size_t ADDR_LEN = NORDIC_TX_ADDR_LEN;
    pipe_num > 1 ? ADDR_LEN = 1: 0;
    SPI_write_packet(NRF_SPI_BUS,rx_addr,ADDR_LEN);
    //SPI_flushRXFIFO(NRF_SPI_BUS);

    NRF_chip_disable();
}


uint8_t NRF_read_fifo_status()
{
	return NRF_read_register(NORDIC_FIFO_STATUS_REG);
}

void NRF_flush_tx_fifo()
{
	NRF_write_command(NORDIC_TXFIFO_FLUSH_CMD);
}

void NRF_flush_rx_fifo()
{
	NRF_write_command(NORDIC_RXFIFO_FLUSH_CMD);
}

void NRF_activate_cmd()
{
	NRF_write_register(NORDIC_ACTIVATE_CMD, NORDIC_ACTIVATE_DATA);
}

void NRF_enable_RX_PIPE(uint8_t rx_pipe_number)
{
    if(rx_pipe_number > 5)
        return;
    uint8_t ret = NRF_read_register(NORDIC_EN_RXADDR_REG);
    NRF_write_register(NORDIC_EN_RXADDR_REG, ret | (1<<rx_pipe_number));

}
void NRF_disable_RX_PIPE(uint8_t rx_pipe_number)
{
    if(rx_pipe_number > 5)
        return;
    uint8_t ret = NRF_read_register(NORDIC_EN_RXADDR_REG);
    NRF_write_register(NORDIC_EN_RXADDR_REG, ret & (~(1<<rx_pipe_number)));
}

static void NRF_mode_configure(NRF_Mode_t mode, uint8_t rx_pipe_number, uint8_t addr[5], uint8_t payload_size)
{
	if(mode < 2)
	{
	    NRF_radio_disable();
		uint8_t configureRead = NRF_read_config();

		if(mode == NRF_Mode_TX)
		{
			txconfigured = 1;
			configureRead &= ~(NORDIC_CONFIG_TX_DS_INT(1));// | NORDIC_CONFIG_MAX_RT_INT(1));
			NRF_flush_tx_fifo();
	        NRF_write_En_AA(0);
	        NRF_write_setup_retry(0);
	        NRF_write_TX_ADDR(addr);
	        NRF_write_RX_PIPE_ADDR(rx_pipe_number, addr);
	        NRF_enable_RX_PIPE(rx_pipe_number);
	        NRF_write_register((NORDIC_RX_PW_P0_REG), payload_size);
	        NRF_write_config(configureRead | NORDIC_CONFIG_PWR_UP(1));
	        DelayMs(2);
		}
		else
		{
			rxconfigured = 1;
			configureRead |= NORDIC_CONFIG_PWR_UP(1) | NORDIC_CONFIG_PRIM_RX(1);
			configureRead &= ~(NORDIC_CONFIG_RX_DR_INT(1));
			NRF_flush_rx_fifo();
			NRF_enable_RX_PIPE(rx_pipe_number);
			NRF_write_RX_PIPE_ADDR(rx_pipe_number, addr);
			NRF_write_register((NORDIC_RX_PW_P0_REG + rx_pipe_number), payload_size);
			NRF_write_config(configureRead);
			NRF_radio_enable();
		}

		DelayMs(2);
		printf("NORDIC Configured in %s mode\n", ((mode)?"RX MODE":"TX MODE"));

	}
	else
	{
		printf("INVALID MODE\n");
	}
}

void NRF_openReadPipe(uint8_t rx_pipe_number, uint8_t rx_addr[5], uint8_t payload_size)
{
    NRF_mode_configure(NRF_Mode_RX, rx_pipe_number, rx_addr, payload_size);
}

void NRF_openWritePipe(uint8_t tx_addr[5])
{
    //NRF_mode_configure(NRF_Mode_TX, 0, tx_addr, 5);
    NRF_mode_configure(NRF_Mode_TX, 0, tx_addr, 32);
}

void NRF_closeWritePipe()
{
    txconfigured = 0;
    uint8_t configureRead = NRF_read_config();
    configureRead |= (NORDIC_CONFIG_TX_DS_INT(1)  | NORDIC_CONFIG_MAX_RT_INT(1));
    NRF_write_config(configureRead);
    NRF_disable_RX_PIPE(0);
}

void NRF_closeReadPipe(uint8_t rx_pipe_number)
{
    NRF_radio_disable();
    rxconfigured = 0;
    uint8_t configureRead = NRF_read_config();
    configureRead |= NORDIC_CONFIG_RX_DR_INT(1);
    NRF_write_config(configureRead);
    NRF_disable_RX_PIPE(rx_pipe_number);
}

void NRF_write_TXPayload(uint8_t *data, uint8_t len)
{
    NRF_chip_disable();
    NRF_chip_enable();
    SPI_write_byte(NRF_SPI_BUS, NORDIC_W_TXPAYLD_CMD);
    SPI_read_byte(NRF_SPI_BUS); //used to clear the previously value in the RX FIFO

    SPI_write_packet(NRF_SPI_BUS,data, len);  //loading the FIFO with data  before enabling the CE pin
    SPI_flushRXFIFO(NRF_SPI_BUS);
    NRF_chip_disable();
}

void NRF_TX_pulse()
{
    NRF_radio_enable();
    //Delay of min 10us
    DelayUs(20);
    NRF_radio_disable();
}

int32_t NRF_transmit_data(uint8_t *data, uint8_t len, uint8_t toRXMode)
{
    int32_t ret = 0;
	if(txconfigured)
	{
	    uint8_t configureRead = NRF_read_config();
	    configureRead &= ~NORDIC_CONFIG_PRIM_RX(1);
	    NRF_write_config(configureRead);
	    //configureRead = NRF_read_config();
	    DelayUs(130);

		NRF_radio_disable();

		NRF_write_TXPayload(data, len);

		NRF_TX_pulse();

		printf("Data written");

        uint32_t retry_count = 0;
		if(using_interrupt)
		{
            while(transmitted == 0 && retry_count < 1024)	//wait till TX data is transmitted from FIFO
            {
                retry_count++;
            }
            if(retry_count == 1024)
            {
                ret = 0;
                printf("Data Retry Error\n");
            }
            else
            {
                ret = len;
                transmitted = 0; printf("Data Transmitted\n");
            }
		}
		else
		{
            uint8_t status = 0;
            do
            {
                status = NRF_read_status();
            }while((!((NORDIC_STATUS_TX_DS_MASK | NORDIC_STATUS_MAX_RT_MASK) & status)) && ++retry_count < 1024);
            if(retry_count > 1023)
            {
                ret = 0;
                printf("Data Retry Error\n");
            }
            else
            {
                NRF_write_status(NORDIC_STATUS_TX_DS_MASK | NORDIC_STATUS_MAX_RT_MASK);
                ret = len;
            }
		}
		if(toRXMode)
		{
            configureRead &= ~(NORDIC_CONFIG_PRIM_RX(1));
            NRF_write_config(configureRead);
            NRF_flush_rx_fifo();
            NRF_radio_enable();
            DelayUs(130);
		}
	}
	else
	{
		printf("TX mode not configured");
        ret = -1;
	}
    return ret;
}

void NRF_read_RXPayload(uint8_t *data, uint8_t len)
{
    NRF_chip_enable();

    SPI_write_byte(NRF_SPI_BUS, NORDIC_R_RXPAYLD_CMD);
    SPI_read_byte(NRF_SPI_BUS);   //used to clear the previously value in the RX FIFO
    SPI_read_packet(NRF_SPI_BUS,data,len);
    SPI_flush(NRF_SPI_BUS);

    NRF_chip_disable();
}

int32_t NRF_read_data(uint8_t *data, uint8_t len)
{
    int32_t ret = 0;
	if(rxconfigured)
	{
	    NRF_radio_enable();
	    uint8_t val = NRF_read_fifo_status();
	    val = NRF_read_config();
	    //TODO: Check how to move forward with this? Call this function after we know that the data is avail or check with the
	    //Status reg if data is available
        uint32_t retry_count = 0;
	    if(using_interrupt)
	    {
	        while(received == 0 && retry_count < 1024)	//wait till RX data in FIFO
	        {
	            //val = NRF_read_fifo_status();//Not needed
                retry_count++;
	        }
            if(retry_count > 1023)
            {
                ret = 0;
                printf("Data Retry Error\n");
            }
            else
            {
	            received = 0;
                ret = len;
            }
	    }
	    else
	    {
	        uint8_t status = 0;
	        do
	        {
	            status = NRF_read_status();
	        }while((!(NORDIC_STATUS_RX_DR_MASK & status))&& ++retry_count < 1024);
            if(retry_count > 1023)
            {
                ret = 0;
                printf("Data Retry Error\n");
            }
            else
            {
                NRF_write_status(NORDIC_STATUS_RX_DR_MASK);
                ret = len;
            }
	    }

		printf("Data received");

		NRF_read_RXPayload(data, len);

		printf("Data read");
	}
	else
	{
		printf("RX mode not configured");
        ret = -1;
	}
    return ret;
}

//#define SELF_TEST
#ifdef SELF_TEST

void Nordic_Test()
{
    NRF_moduleInit();
    NRF_moduleSetup(NRF_DR_1Mbps, NRF_PW_LOW);
    DelayMs(100);

    printf( "SPI Initialized\n");
    printf("Nordic Initialized\n");
    printf("Nordic Test\n");
//    NRF_write_status(0);
//    uint8_t sendValue = 0x08;
//    uint8_t readValue = 0;
//    NRF_write_config(sendValue);
//    readValue = NRF_read_config();
//    printf("Recv: 0x%x\n",readValue);
//    if(readValue == sendValue)
//    {
//        printf("Write/Read Config Value Matched\n");
//        printf("Sent: 0x%x\n",sendValue);
//        printf("Recv: 0x%x\n",readValue);
//    }
//
//    DelayMs(5);
//
//    NRF_write_register(NORDIC_STATUS_REG,0);
//    sendValue = 44;
//    NRF_write_rf_ch(sendValue);
//    readValue = NRF_read_rf_ch();
//    if(readValue == sendValue)
//    {
//        printf("Write/Read RF CH Value Matched\n");
//        printf("Sent: 0x%x\n",sendValue);
//        printf("Recv: 0x%x\n",readValue);
//    }
//
//    //sendValue = 0x0F;
//    sendValue = 0x07 ;
//    NRF_write_rf_setup(sendValue);
//    readValue = NRF_read_rf_setup();
//    if(readValue == sendValue)
//    {
//        printf("Write/Read RF Setup Value Matched\n");
//        printf("Sent: 0x%x\n",sendValue);
//        printf("Recv: 0x%x\n",readValue);
//    }
//
//    NRF_write_register(0x03, 3);
//
////    uint8_t sendAddr[5] = {0xBA,0x56,0xBA,0x56,0xBA};
//    uint8_t sendAddr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
//    printf("TX ADDRESSES SET: 0x%x%x%x%x%x\n",sendAddr[0],sendAddr[1],sendAddr[2],sendAddr[3],sendAddr[4]);
//    NRF_write_TX_ADDR(sendAddr);
//    uint8_t readAddr[5];
//    NRF_read_TX_ADDR(readAddr);
//    printf("TX ADDRESSES GET: 0x%x%x%x%x%x\n",readAddr[0],readAddr[1],readAddr[2],readAddr[3],readAddr[4]);
//
//    NRF_read_RX_P0_ADDR(readAddr);
//    printf("RX ADDRESSES GET: 0x%x%x%x%x%x\n",readAddr[0],readAddr[1],readAddr[2],readAddr[3],readAddr[4]);
//
//    NRF_write_RX_P0_ADDR(sendAddr);
//    NRF_read_RX_P0_ADDR(readAddr);
//    printf("RX ADDRESSES GET: 0x%x%x%x%x%x\n",readAddr[0],readAddr[1],readAddr[2],readAddr[3],readAddr[4]);




//    NRF_Mode_t mode = NRF_Mode_RX;
//    printf("Configuring NRF in %d mode",mode);
//    NRF_mode_configure(mode);
//    uint8_t Data[2] = {0};
//    NRF_read_data(Data,2);
//    printf("Nordic Data Recvd: 0x%x, 0x%x", Data[0],Data[1]);

    uint8_t sendAddr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
    NRF_openWritePipe(sendAddr);
    printf("Configuring NRF in TX mode");
    uint8_t readAddr[5];
    NRF_read_TX_ADDR(readAddr);
    logger_log(INFO,"TX ADDRESSES GET: 0x%x%x%x%x%x\n",readAddr[0],readAddr[1],readAddr[2],readAddr[3],readAddr[4]);

    //NRF_read_RX_P0_ADDR(readAddr);
    logger_log(INFO,"RX ADDRESSES GET: 0x%x%x%x%x%x\n",readAddr[0],readAddr[1],readAddr[2],readAddr[3],readAddr[4]);

    NRF_read_RX_PIPE_ADDR(0, readAddr);
    logger_log(INFO,"RX ADDRESSES GET: 0x%x%x%x%x%x\n",readAddr[0],readAddr[1],readAddr[2],readAddr[3],readAddr[4]);

    uint8_t Data[5] = {0x55,0xBB,0xBB,0xBB,0xBB};
    NRF_transmit_data(Data,5, false);
    printf("Nordic Data Sent: 0x%x, 0x%x", Data[0],Data[1]);

    printf("Nordic Test End\n");

    NRF_moduleDisable();
}
#endif

void NRF_IntHandler(void *args)
{
    uint8_t NRF_int_reason = NRF_read_status();
    if(NRF_int_reason & NORDIC_STATUS_TX_DS_MASK)
    {
        NRF_write_status(NRF_int_reason | NORDIC_STATUS_TX_DS_MASK);
        transmitted = 1;
        printf("NRF TX Complete\n");
    }
    if(NRF_int_reason & NORDIC_STATUS_RX_DR_MASK)
    {
        NRF_write_status(NRF_int_reason | NORDIC_STATUS_RX_DR_MASK);
        NRF_flush_rx_fifo();
        //TODO: Notification to the handler for the Nordic Data recv task
        user_handler();
        received = 1;
        printf("NRF RX Complete\n");
    }
    if(NRF_int_reason & NORDIC_STATUS_MAX_RT_MASK)
    {
        NRF_write_status(NRF_int_reason | NORDIC_STATUS_MAX_RT_MASK);
        NRF_flush_tx_fifo();
        //TODO: Notification to the handler for the Nordic Data recv task
        user_handler();
        retry_error = 1;
        printf("NRF TX RETRY ERROR\n");
    }
}

#endif


