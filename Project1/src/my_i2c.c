/**
 * @brief 
 * 
 * @file my_i2c.c
 * @author Gunj Manseta
 * @date 2018-03-13
 */

#include <errno.h>
#include "my_i2c.h"


static I2C_MASTER_HANDLE_T *internal_master_handle = NULL;
static pthread_mutex_t init_destroy_lock = PTHREAD_MUTEX_INITIALIZER;

void printErrorCode(int errorCode)
{
    #define VERBOSE
    #ifdef VERBOSE
    mraa_result_print(errorCode);
    #endif
}

int I2Cmaster_Init(I2C_MASTER_HANDLE_T *handle)
{
    int ret = 0;
    if(pthread_mutex_lock(&init_destroy_lock))
        return -1;

    if(NULL != internal_master_handle)
    {
        *handle = *internal_master_handle;
        ret =  0;
    }
    else
    {
        handle->i2c_context = mraa_i2c_init_raw(BB_I2C_BUS_2);
        if(NULL == handle->i2c_context)
        {
            internal_master_handle = NULL;
            ret = -1;
        }
        else if( -1 == pthread_spin_init(&handle->handle_lock, PTHREAD_PROCESS_PRIVATE))
        {
            internal_master_handle = NULL;
            mraa_i2c_stop(handle->i2c_context);
            ret = -1;
        }
        else
        {
            internal_master_handle = handle;
            ret = 0;
        }
    }

    if(pthread_mutex_unlock(&init_destroy_lock))
        return -1;

    return ret;
}

int I2Cmaster_Destroy(I2C_MASTER_HANDLE_T *handle)
{
    int ret;
    if(pthread_mutex_lock(&init_destroy_lock))
        return -1;

    /* If the input handle is not null and the input initialized handle should match the internal handle */    
    if(NULL != handle && internal_master_handle == handle)
    {
        ret = mraa_i2c_stop(handle->i2c_context);
        if(ret == 0)
        {
            static int timeout = 5000;
            do{
                ret = pthread_spin_destroy(&handle->handle_lock);
                timeout--;
            }while(EBUSY == ret && timeout > 0);

            if(ret == 0)
                internal_master_handle = NULL;
        }
    }
    else
        ret = -1;

    if(pthread_mutex_unlock(&init_destroy_lock))
        return -1;

    return ret;
} 

I2C_MASTER_HANDLE_T getMasterI2C_handle()
{
    return *internal_master_handle;
}

int I2Cmaster_write_byte(uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{
    if(NULL == internal_master_handle)
    {
        return -1;
    }

    pthread_spin_lock(&internal_master_handle->handle_lock);

    mraa_result_t ret =  mraa_i2c_address(internal_master_handle->i2c_context, slave_addr);
    if(0 == ret)
    {   
        ret = mraa_i2c_write_byte_data(internal_master_handle->i2c_context, data, reg_addr);
    }

    pthread_spin_unlock(&internal_master_handle->handle_lock);

    return ret;
}

#if 0
int I2Cmaster_write_bytes(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, size_t len)
{
    if(NULL == internal_master_handle)
    {
        return -1;
    }

    pthread_spin_lock(&internal_master_handle->handle_lock);

    mraa_result_t ret =  mraa_i2c_address(internal_master_handle->i2c_context, slave_addr);
    if(0 == ret)
    {   
        ret = mraa_i2c_write_byte_data(internal_master_handle->i2c_context, data, reg_addr);
    }

    pthread_spin_unlock(&internal_master_handle->handle_lock);

    return ret;

}
#endif

int I2Cmaster_read_byte(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data)
{
    if(NULL == internal_master_handle)
    {
        return -1;
    }

    pthread_spin_lock(&internal_master_handle->handle_lock);

    mraa_result_t ret =  mraa_i2c_address(internal_master_handle->i2c_context, slave_addr);
    if(0 == ret)
    {   
        ret = mraa_i2c_read_byte_data(internal_master_handle->i2c_context, reg_addr);
        (ret != -1) ? *data  = ret, ret = 0 : 0; 
    }

    pthread_spin_unlock(&internal_master_handle->handle_lock);

    return ret;

}

int I2Cmaster_read_bytes(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, size_t len)
{
    if(NULL == internal_master_handle)
    {
        return -1;
    }

    pthread_spin_lock(&internal_master_handle->handle_lock);

    mraa_result_t ret =  mraa_i2c_address(internal_master_handle->i2c_context, slave_addr);
    if(0 == ret)
    {   
        ret = mraa_i2c_read_bytes_data(internal_master_handle->i2c_context, reg_addr, data , len);
        //(ret == len) ? ret = 0 : ret = -1; 
    }

    pthread_spin_unlock(&internal_master_handle->handle_lock);

    return ret;
}

