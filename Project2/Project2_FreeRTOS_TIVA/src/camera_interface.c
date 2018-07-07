/*
 * camera_interface.c
 *
 *  Created on: 29-Apr-2018
 *      Author: Gunj Manseta
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"


#include "my_uart.h"
#include "delay.h"
#include "spi.h"
#include "ov2640_regs.h"
#include "cam_header.h"
#include "camera_interface.h"

#define SLAVE_ADDRESS (0x60)

static uint8_t camera_error_flag = 0;

uint8_t read_fifo_burst()
{
    uint8_t temp = 0, temp_last = 0;
    uint32_t length = 0;
    length = read_fifo_length();
    uint8_t is_header = 0;

    if (length >= MAX_FIFO_SIZE) //512 kb
    {
        printf("Camera frame oversize.\n");
        return 0;
    }
    if (length == 0 ) //0 kb
    {
        printf("Camera Frame size is 0.\n");
        return 0;
    }
    CS_LOW();

//    set_fifo_burst();//Set fifo burst mode
    transfer(BURST_FIFO_READ);
    temp =  transfer(0x00);
    length--;
    while ( length-- )
    {
        temp_last = temp;
        temp =  transfer(0x00);
        if (is_header == 1)
        {
            write(temp);
        }
        else
        if ((temp == 0xD8) & (temp_last == 0xFF))
        {
            is_header = 1;
            //printACKIMG();
            write(temp_last);
            write(temp);
        }
        if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
        {
            break;
        }
        DelayUs(15);
    }

    CS_HIGH();

    write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);

    return length;
}

uint32_t SendFrame()
{
    if(camera_error_flag)
    {
        printf(FAIL "Camera interface error. Cannot send frame\n");
        return 0;
    }
    //    myCAM.flush_fifo();
    write_reg(ARDUCHIP_FIFO, FIFO_RDPTR_RST_MASK);
     //Clear the capture done flag
    //     myCAM.clear_fifo_flag();
    write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
     //Start capture
    //     myCAM.start_capture();
    write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);

    uint32_t retryCount = (1<<24);
    while ((!get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK)) && retryCount) { DelayUs(10); retryCount--;}
    write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);

    if(retryCount == 0)
    {
        printf(FAIL "Failed to get frame done flag\n");
        return 0;
    }

    uint8_t ret = read_fifo_burst();

    DelayMs(20);

    return ret;

}


void CameraInit()
{
    SPI_clock_init(SPI_2,g_sysClock);
    SPI_init(SPI_2);
    I2C_init();

    uint32_t retryCount = 0;
    while (retryCount < 2048) {
      //Check if the ArduCAM SPI bus is OK
      write_reg(ARDUCHIP_TEST1, 0x55);
      uint8_t temp = read_reg(ARDUCHIP_TEST1);

      if (temp != 0x55) {
        //Serial.println(F("SPI interface Error!"));
        DelayUs(1000); continue;
      } else {
          printf(OK "Camera SPI Interface working. \n");
          break;
      }
      retryCount++;
    }
    if(retryCount > 2047)
    {
        camera_error_flag = 1;
        printf(FAIL "Camera SPI interface error\n");
        return;
    }
    retryCount = 0;

    uint8_t vid = 0, pid = 0;
    wrSensorReg8_8(0xff, 0x01);
    DelayUs(1000);
    while(retryCount < 2048)
    {
        vid = rdSensorReg8_8(OV2640_CHIPID_HIGH);
        pid = rdSensorReg8_8(OV2640_CHIPID_LOW);
        if ((vid == 0x26 ) && ( pid == 0x42 ))
        {
             printf(OK "Camera I2C interface Done. ModuleID: 0x%x%x\n",vid,pid);
             break;
        }
        retryCount++;
    }
    if(retryCount > 2047)
    {
         camera_error_flag = 1;
         printf(FAIL "Camera I2C interface error\n");
         return;
    }

    retryCount = 0;
    printf(OK "Found OV2640 module!\n");
    camera_error_flag = 0;

    //CAM INIT on jpeg
    wrSensorReg8_8(0xff, 0x01);
    wrSensorReg8_8(0x12, 0x80);
    DelayUs(1000);
    wrSensorRegs8_8(OV2640_JPEG_INIT);
    DelayUs(1000);
    wrSensorRegs8_8(OV2640_YUV422);
    DelayUs(1000);
    wrSensorRegs8_8(OV2640_JPEG);
    DelayUs(1000);
    wrSensorReg8_8(0xff, 0x01);
    wrSensorReg8_8(0x15, 0x00);
//    wrSensorRegs8_8(OV2640_160x120_JPEG);
//    wrSensorRegs8_8(OV2640_320x240_JPEG);
    wrSensorRegs8_8(OV2640_640x480_JPEG);
    DelayUs(1000);
    wrSensorReg8_8(0xff, 0x00);
    wrSensorReg8_8(0x44, 0x32);

    printf(OK "Camera module initialized. Format:JPEG, RES:640x480!\n");
}

void I2C_init()
{
    //enable GPIO peripheral that contains I2C 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C0_BASE, g_sysClock, true);
//    I2CMasterInitExpClk(I2C0_BASE, 40000000, true);

    I2CTxFIFOFlush(I2C0_BASE);
    I2CRxFIFOFlush(I2C0_BASE);
    //clear I2C FIFOs
    //HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;

}




uint8_t get_bit(uint8_t addr, uint8_t bit)
{
  uint8_t temp;
  temp = read_reg(addr);
  temp = temp & bit;
  return temp;
}


uint32_t read_fifo_length()
{
    uint8_t len1,len2,len3;
    uint32_t length=0;
    len1 = read_reg(FIFO_SIZE1);
    len2 = read_reg(FIFO_SIZE2);
    len3 = read_reg(FIFO_SIZE3) & 0x7f;
    length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
    return length;
}

uint8_t transfer(uint8_t val)
{
    SPI_write_byte(SPI_2, val);
    uint8_t value = SPI_read_byte(SPI_2);
    return value;
}

uint8_t read_reg(uint8_t reg)
{
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
    SPI_write_byte(SPI_2, reg);
    SPI_read_byte(SPI_2);
    SPI_write_byte(SPI_2, 0xFF);
    uint8_t value = SPI_read_byte(SPI_2);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2);
    return value;
}

void write_reg(uint8_t reg, uint8_t value)
{
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
    SPI_write_byte(SPI_2, reg | 0x80);
    SPI_read_byte(SPI_2);
    SPI_write_byte(SPI_2, value);
    SPI_read_byte(SPI_2);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2);
}

void wrSensorRegs8_8(uint8_t **array_reg_value_pair)
{
    uint8_t reg_addr = 0;
    uint8_t reg_val = 0;
    uint8_t *next = array_reg_value_pair;
    uint32_t count = 0;
    while ((reg_addr != 0xff) | (reg_val != 0xff))
    {
      reg_addr = *(next);
      reg_val = *(next+1);
      wrSensorReg8_8(reg_addr, reg_val);
      next+=2;
      count++;
    }
//    printf("ACK CMD Count %u, NEXT: %u REG: 0x%x, Dat: 0x%x\n", count, next, reg_addr, reg_val);
}

uint8_t rdSensorReg8_8(uint8_t reg)
{
    I2CMasterSlaveAddrSet(I2C0_BASE, (SLAVE_ADDRESS)>>1, false);
    //specify register to be read
    I2CMasterDataPut(I2C0_BASE, reg);

    //send control byte and register address byte to slave device
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C0_BASE));

    //specify that we are going to read from slave device
    I2CMasterSlaveAddrSet(I2C0_BASE, (SLAVE_ADDRESS| 0x01) >>1, true);

    //send control byte and read from the register we
    //specified
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C0_BASE));

    //return data pulled from the specified register
    uint8_t val =  I2CMasterDataGet(I2C0_BASE);

    while(I2CMasterBusy(I2C0_BASE));
    return val;
}


void wrSensorReg8_8(uint8_t reg, uint8_t data)
{
    I2CMasterSlaveAddrSet(I2C0_BASE, (SLAVE_ADDRESS>>1), false);
    I2CMasterDataPut(I2C0_BASE, reg);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C0_BASE));

    I2CMasterDataPut(I2C0_BASE, data);
    //send next data that was just placed into FIFO
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    // Wait until MCU is done transferring.
    while(I2CMasterBusy(I2C0_BASE));
}
