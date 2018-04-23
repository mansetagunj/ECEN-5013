/**
* @file - nordic_driver.c
* @brief - Implementation file for the driver functions of the NRF240L
*
* @author Gunj/Ashish University of Colorado Boulder
* @date - 8 Dec 2017
**/


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "my_uart.h"

#include "nordic_driver.h"
#include "spi.h"

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

extern uint32_t g_sysClock;

void NRF_IntHandler();

void NRF_gpioInit()
{
    //Enabling the GPIO PC4 for Nordic CE pin
    MAP_SysCtlPeripheralEnable(NORDIC_CE_SYSCTL_PORT);
    GPIOPinTypeGPIOOutput(NORDIC_CE_PORT, NORDIC_CE_PIN);
    GPIOPinWrite(NORDIC_CE_PORT, NORDIC_CE_PIN, 0);

    //Enabling the GPIO PC5 for Nordic CSN pin
    MAP_SysCtlPeripheralEnable(NORDIC_CSN_SYSCTL_PORT);
    GPIOPinTypeGPIOOutput(NORDIC_CSN_PORT, NORDIC_CSN_PIN);
    GPIOPinWrite(NORDIC_CSN_PORT, NORDIC_CSN_PIN, NORDIC_CSN_PIN);

    //Enabling the GPIO PC6 for Nordic IRQ pin
    MAP_SysCtlPeripheralEnable(NORDIC_IRQ_SYSCTL_PORT);

    GPIOPinTypeGPIOInput(NORDIC_IRQ_PORT,NORDIC_IRQ_PIN);
    GPIOIntUnregister(NORDIC_IRQ_PORT);
    GPIOIntClear(NORDIC_IRQ_PORT,0xFFFF);
    GPIOIntTypeSet(NORDIC_IRQ_PORT, NORDIC_IRQ_PIN, GPIO_LOW_LEVEL);
    GPIOIntRegister(NORDIC_IRQ_PORT, NRF_IntHandler);
    GPIOIntDisable(NORDIC_IRQ_PORT,0xFFFF);
    GPIOIntEnable(NORDIC_IRQ_PORT,NORDIC_IRQ_PIN);
}

void NRF_moduleInit()
{
    NRF_gpioInit();
    SPI_clock_init(SPI_1, g_sysClock);
    SPI_init(SPI_1);
    DelayMs(1);
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
    uint8_t config = NRF_read_config();
    NRF_write_config(config & ~NORDIC_CONFIG_PWR_UP(1));
	SPI_disable(SPI_1);
	GPIOIntClear(NORDIC_IRQ_PORT,NORDIC_IRQ_PIN);
	GPIOIntUnregister(NORDIC_IRQ_PORT);
}

uint8_t NRF_read_register(uint8_t regAdd)
{
	//SPI_clear_RXbuffer(SPI_1);	//used to clear the previously value in the RX FIFO
	uint8_t readValue = 0;

	//CSN High to low for new command
	NRF_chip_disable();
	NRF_chip_enable();

	SPI_write_byte(SPI_1,regAdd);
	SPI_read_byte(SPI_1);	//used to clear the previously value in the RX FIFO
	SPI_write_byte(SPI_1,0xFF);
	readValue = SPI_read_byte(SPI_1);

	//Marking the end of transaction by CSN high
	NRF_chip_disable();

	return readValue;
}

void NRF_write_command(uint8_t command)
{
	//CSN High to low for new command
	NRF_chip_disable();
	NRF_chip_enable();

	SPI_write_byte(SPI_1,command);
	//SPI_clear_RXbuffer(SPI_1);	//used to clear the previously value in the RX FIFO
	SPI_read_byte(SPI_1);

	//Marking the end of transaction by CSN high
	NRF_chip_disable();
}

void NRF_write_register(uint8_t regAdd, uint8_t value)
{
	//SPI_clear_RXbuffer(SPI_1);	//used to clear the previously value in the RX FIFO

	//CSN High to low for new command
	NRF_chip_disable();
	NRF_chip_enable();

	SPI_write_byte(SPI_1,regAdd | 0x20);
	SPI_read_byte(SPI_1);	//used to clear the previously value in the RX FIFO
	SPI_write_byte(SPI_1,value);
	SPI_read_byte(SPI_1);	//used to clear the previously value in the RX FIFO

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

    SPI_write_byte(SPI_1,NORDIC_NOP);
    readValue = SPI_read_byte(SPI_1);   //used to clear the previously value in the RX FIFO

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

	SPI_write_byte(SPI_1,NORDIC_TX_ADDR_REG);
	SPI_read_byte(SPI_1);	//used to clear the previously value in the RX FIFO
	//SPI_read_byte(SPI_1);	//used to clear the previously value in the RX FIFO
	while(i < NORDIC_TX_ADDR_LEN)
	{
		SPI_write_byte(SPI_1, 0xFF);	//Dummy to get the data
		*(address+i) = SPI_read_byte(SPI_1);
		i++;
	}

	NRF_chip_disable();
}

void NRF_write_TX_ADDR(uint8_t * tx_addr)
{
	NRF_chip_disable();
	NRF_chip_enable();

	SPI_write_byte(SPI_1,NORDIC_TX_ADDR_REG | 0x20);
	SPI_read_byte(SPI_1);	//used to clear the previously value in the RX FIFO
	SPI_write_packet(SPI_1,tx_addr,NORDIC_TX_ADDR_LEN);
	SPI_flushRXFIFO(SPI_1);

	NRF_chip_disable();
}

void NRF_read_RX_PIPE_ADDR(uint8_t pipe_num, uint8_t *address)
{
    if(pipe_num > 5)
        return;
//    uint8_t i = 0;

    NRF_chip_disable();
    NRF_chip_enable();

    SPI_write_byte(SPI_1,(NORDIC_RX_ADDR_P0_REG + pipe_num));
    SPI_read_byte(SPI_1);   //used to clear the previously value in the RX FIFO
    //SPI_read_byte(SPI_1); //used to clear the previously value in the RX FIFO
    size_t ADDR_LEN = NORDIC_TX_ADDR_LEN;
    pipe_num > 2 ? ADDR_LEN = 1: 0;
//    while(i < ADDR_LEN)
//    {
//        SPI_write_byte(SPI_1, 0xFF);    //Dummy to get the data
//        *(address+i) = SPI_read_byte(SPI_1);
//        i++;
//    }
    SPI_read_packet(SPI_1, address, ADDR_LEN);

    NRF_chip_disable();
}

void NRF_write_RX_PIPE_ADDR(uint8_t pipe_num, uint8_t *rx_addr)
{
    if(pipe_num > 5)
        return;

    NRF_chip_disable();
    NRF_chip_enable();

    SPI_write_byte(SPI_1,(NORDIC_RX_ADDR_P0_REG + pipe_num) | 0x20);
    SPI_read_byte(SPI_1);   //used to clear the previously value in the RX FIFO
    size_t ADDR_LEN = NORDIC_TX_ADDR_LEN;
    pipe_num > 2 ? ADDR_LEN = 1: 0;
    SPI_write_packet(SPI_1,rx_addr,ADDR_LEN);
    SPI_flushRXFIFO(SPI_1);

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

static void NRF_mode_configure(NRF_Mode_t mode, uint8_t rx_pipe_number, uint8_t addr[5], uint8_t payload_size)
{
	if(mode < 2)
	{
	    NRF_radio_disable();
		uint8_t configureRead = NRF_read_config();

		if(mode == NRF_Mode_TX)
		{
			txconfigured = 1;
			configureRead &= ~(NORDIC_CONFIG_TX_DS_INT(1) | NORDIC_CONFIG_MAX_RT_INT(1));
			NRF_flush_tx_fifo();
	        NRF_write_En_AA(0);
	        NRF_write_setup_retry(0);
	        NRF_write_TX_ADDR(addr);
	        NRF_write_RX_PIPE_ADDR(rx_pipe_number, addr);
	        NRF_write_register((NORDIC_RX_PW_P0_REG), payload_size);
	        NRF_write_config(configureRead | NORDIC_CONFIG_PWR_UP(1));
	        //This line should called when transmitting the data
//	        NRF_write_config(configureRead | NORDIC_CONFIG_PRIM_RX(mode) | NORDIC_CONFIG_PWR_UP(1));
		}
		else
		{
			rxconfigured = 1;
			configureRead |= NORDIC_CONFIG_PWR_UP(1) | NORDIC_CONFIG_PRIM_RX(1);
			configureRead &= ~(NORDIC_CONFIG_RX_DR_INT(1));
			NRF_flush_rx_fifo();
			NRF_write_RX_PIPE_ADDR(rx_pipe_number, addr);
			NRF_write_config(configureRead);
			NRF_write_register((NORDIC_RX_PW_P0_REG + rx_pipe_number), payload_size);
			NRF_radio_enable();
		}

		DelayMs(2);
		printf("NORDIC Configured in %s mode\n", ((mode)?"RX MODE":"TX MODE"));

	}
	else
		printf("INVALID MODE\n");
}

void NRF_openReadPipe(uint8_t rx_pipe_number, uint8_t rx_addr[5], uint8_t payload_size)
{
    NRF_mode_configure(NRF_Mode_RX, rx_pipe_number, rx_addr, payload_size);
}

void NRF_openWritePipe(uint8_t tx_addr[5])
{
    NRF_mode_configure(NRF_Mode_TX, 0, tx_addr, 5);
}

void NRF_closeWritePipe()
{
    txconfigured = 0;
    uint8_t configureRead = NRF_read_config();
    configureRead |= (NORDIC_CONFIG_TX_DS_INT(1)  | NORDIC_CONFIG_MAX_RT_INT(1));
    NRF_write_config(configureRead);
}

void NRF_closeReadPipe()
{
    NRF_radio_disable();
    rxconfigured = 0;
    uint8_t configureRead = NRF_read_config();
    configureRead |= NORDIC_CONFIG_RX_DR_INT(1);
    NRF_write_config(configureRead);
}

void NRF_write_TXPayload(uint8_t *data, uint8_t len)
{
    NRF_chip_disable();
    NRF_chip_enable();
    SPI_write_byte(SPI_1, NORDIC_W_TXPAYLD_CMD);
    SPI_read_byte(SPI_1); //used to clear the previously value in the RX FIFO

    SPI_write_packet(SPI_1,data, len);  //loading the FIFO with data  before enabling the CE pin
    SPI_flushRXFIFO(SPI_1);
    NRF_chip_disable();
}

void NRF_TX_pulse()
{
    NRF_radio_enable();
    //Delay of min 10us
    DelayUs(20);
    NRF_radio_disable();
}

void NRF_transmit_data(uint8_t *data, uint8_t len, uint8_t toRXMode)
{
	if(txconfigured)
	{
	    uint8_t configureRead = NRF_read_config();
	    configureRead &= ~NORDIC_CONFIG_PRIM_RX(1);
	    NRF_write_config(configureRead);
	    configureRead = NRF_read_config();
	    DelayUs(130);

		NRF_radio_disable();

		NRF_write_TXPayload(data, len);

		NRF_TX_pulse();

		printf("Data written");

		while(transmitted == 0);	//wait till TX data is transmitted from FIFO
		transmitted = 0;
//		uint8_t status = 0;
//		do
//		{
//		    status = NRF_read_status();
//		}while(!((NORDIC_STATUS_TX_DS_MASK | NORDIC_STATUS_MAX_RT_MASK) & status));

		printf("Data Transmitted");

		if(toRXMode)
		{
            configureRead &= ~(NORDIC_CONFIG_PRIM_RX(1));
            NRF_write_config(configureRead);
            NRF_radio_enable();
		}

	}
	else
		printf("TX mode not configured");
}

NRF_read_RXPayload(uint8_t *data, uint8_t len)
{
    NRF_chip_enable();

    SPI_write_byte(SPI_1, NORDIC_R_RXPAYLD_CMD);
    SPI_read_byte(SPI_1);   //used to clear the previously value in the RX FIFO
    SPI_read_packet(SPI_1,data,len);
    SPI_flush(SPI_1);

    NRF_chip_disable();
}

void NRF_read_data(uint8_t *data, uint8_t len)
{
	if(rxconfigured)
	{
	    //TODO: Check how to move forward with this? Call this function after we know that the data is avail or check with the
	    //Status reg if data is available
		while(received == 0);	//wait till RX data in FIFO
		received = 0;
//		while(!(NORDIC_STATUS_RX_DR_MASK & NRF_read_status()));
		printf("Data received");

		NRF_read_RXPayload(data, len);

		printf("Data read");
	}
	else
		printf("RX mode not configured");
}

#define SELF_TEST
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

void NRF_IntHandler()
{
    MAP_IntMasterDisable();
    uint32_t int_status = GPIOIntStatus(NORDIC_IRQ_PORT, false);
    if(int_status & NORDIC_IRQ_PIN)
    {
        GPIOIntClear(NORDIC_IRQ_PORT, NORDIC_IRQ_PIN);
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
            //TODO: Notification to the handler for the Nordic Data recv task
            received = 1;
            printf("NRF TX Complete\n");
        }
    }
    MAP_IntMasterEnable();
}



