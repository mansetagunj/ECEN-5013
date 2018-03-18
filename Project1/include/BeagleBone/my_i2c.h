/**
 * @brief 
 * 
 * @file my_i2c.h
 * @author Gunj Manseta
 * @date 2018-03-13
 */
#ifndef MYI2C_H
#define MYI2C_H

#include <pthread.h>
#include "mraa/i2c.h"

#define BB_I2C_BUS_2  (2)

/**
 * @brief This is the handle for I2C mster and each master should have only one handle
 * 
 */
typedef struct i2c_handle
{
    /* This context is a typedef'ed pointer within */
    mraa_i2c_context i2c_context;
    pthread_spinlock_t handle_lock;

}I2C_MASTER_HANDLE_T;

/**
 * @brief Get the MasterI2C handle object
 * 
 * @return I2C_MASTER_HANDLE_T *
 */
I2C_MASTER_HANDLE_T* getMasterI2C_handle();

/**
 * @brief Prints the error code string to stdout
 * 
 * @param errorCode 
 */
void printErrorCode(int errorCode);

/**
 * @brief Inits the I2C master handle
 * There is an internal state of the context which is maintained which gets updated with every init call
 * Internal context goes to NULL is error in init
 * This context points to the new handle that is passed as the parameter
 * @param handle 
 * @return int SUCCESS=0 and ERROR =-1
 */
int I2Cmaster_Init(I2C_MASTER_HANDLE_T *handle);

/**
 * @brief 
 * 
 * @param handle 
 * @return int 
 */
int I2Cmaster_Destroy(I2C_MASTER_HANDLE_T *handle);

/**
 * @brief 
 * 
 * @param slave_addr 
 * @param reg_addr 
 * @param data 
 * @return int 
 */
int I2Cmaster_read_byte(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data);

/**
 * @brief 
 * 
 * @param slave_addr 
 * @param reg_addr 
 * @param data 
 * @param len 
 * @return int 
 */
int I2Cmaster_read_bytes(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, size_t len);


/**
 * @brief Writes a byte/pointer register to the slave
 * 
 * @param slave_addr 
 * @param reg_addr 
 * @return int 
 */
int I2Cmaster_write(uint8_t slave_addr, uint8_t reg_addr);

/**
 * @brief 
 * 
 * @param slave_addr 
 * @param reg_addr 
 * @param data 
 * @return int 
 */
int I2Cmaster_write_byte(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);

/**
 * @brief 
 * 
 * @param slave_addr 
 * @param reg_addr 
 * @param data 
 * @param len 
 * @return int 
 */
int I2Cmaster_write_bytes(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, size_t len);

/**
 * @brief 
 * 
 * @param slave_addr 
 * @param reg_addr 
 * @param data 
 * @param lsb_first 
 * @return int 
 */
int I2Cmaster_write_word(uint8_t slave_addr, uint8_t reg_addr, uint16_t data, uint8_t lsb_first);



#endif