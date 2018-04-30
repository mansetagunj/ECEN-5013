/*
 * cam_header.h
 *
 *  Created on: 29-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef CAM_HEADER_H_
#define CAM_HEADER_H_


#define MAX_FIFO_SIZE       0x5FFFF         //384KByte

#define RWBIT                                   0x80  //READ AND WRITE BIT IS BIT[7]

#define ARDUCHIP_TEST1          0x00  //TEST register


#define ARDUCHIP_MODE           0x02  //Mode register
#define MCU2LCD_MODE            0x00
#define CAM2LCD_MODE            0x01
#define LCD2MCU_MODE            0x02

#define ARDUCHIP_TIM            0x03  //Timming control

//#define FIFO_PWRDN_MASK           0x20    //0 = Normal operation, 1 = FIFO power down
//#define LOW_POWER_MODE              0x40      //0 = Normal mode,          1 = Low power mode

#define ARDUCHIP_FIFO           0x04  //FIFO and I2C control
#define FIFO_CLEAR_MASK         0x01
#define FIFO_START_MASK         0x02
#define FIFO_RDPTR_RST_MASK     0x10
#define FIFO_WRPTR_RST_MASK     0x20

#define ARDUCHIP_GPIO             0x06  //GPIO Write Register
#if !(defined (ARDUCAM_SHIELD_V2) || defined (ARDUCAM_SHIELD_REVC))
#define GPIO_RESET_MASK         0x01  //0 = Sensor reset,                           1 =  Sensor normal operation
#define GPIO_PWDN_MASK          0x02  //0 = Sensor normal operation,    1 = Sensor standby
#define GPIO_PWREN_MASK         0x04    //0 = Sensor LDO disable,           1 = sensor LDO enable
#endif

#define BURST_FIFO_READ         0x3C  //Burst FIFO read operation
#define SINGLE_FIFO_READ        0x3D  //Single FIFO read operation

#define ARDUCHIP_REV            0x40  //ArduCHIP revision
#define VER_LOW_MASK            0x3F
#define VER_HIGH_MASK           0xC0

#define ARDUCHIP_TRIG           0x41  //Trigger source
#define VSYNC_MASK              0x01
#define SHUTTER_MASK            0x02
#define CAP_DONE_MASK           0x08

#define FIFO_SIZE1              0x42  //Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2              0x43  //Camera write FIFO size[15:8]
#define FIFO_SIZE3              0x44  //Camera write FIFO size[18:16]

#endif /* CAM_HEADER_H_ */
