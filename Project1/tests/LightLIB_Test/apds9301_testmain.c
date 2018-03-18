/**
 * @brief Test for the APDS9301 sensor
 * 
 * @file apds9301_testmain.c
 * @author Gunj Manseta
 * @date 2018-03-14
 */

#include "my_i2c.h"
#include "apds9301_sensor.h"
#include <unistd.h>
#include "cmocka.h"

void testAPDS9301(void **state)
{
    I2C_MASTER_HANDLE_T i2c;
    int ret = 0;
    ret = I2Cmaster_Init(&i2c);
    assert_int_equal(ret, 0);
    assert_non_null((void*)getMasterI2C_handle());
    assert_ptr_equal(&i2c,getMasterI2C_handle());
    
    ret = APDS9301_poweron();
    assert_int_equal(ret, 0);

    ret = APDS9301_test();
    assert_int_equal(ret, 0);

    uint8_t data = 0;
    ret  = APDS9301_readControlReg(&data);
    assert_int_equal(ret, 0);
    assert_int_equal((data & 0x3), 0x03);


    uint16_t tlow = 0xBB11;
    ret = APDS9301_write_ThLow(tlow);
    assert_int_equal(ret, 0);

    tlow = 0;
    ret = APDS9301_read_ThLow(&tlow);
    assert_int_equal(ret, 0);
    assert_int_equal(tlow, 0xBB11);

    uint16_t thigh = 0xA5A5;
    ret = APDS9301_write_ThHigh(thigh);
    assert_int_equal(ret, 0);

    thigh = 0 ; 
    ret = APDS9301_read_ThHigh(&thigh);
    assert_int_equal(ret, 0);
    assert_int_equal(thigh, 0xA5A5);
    
    
    ret = APDS9301_mode_highGain();
    assert_int_equal(ret, 0);

    ret = APDS9301_mode_integrationTime3();
    assert_int_equal(ret, 0);

    ret = APDS9301_mode_interruptEnable();
    assert_int_equal(ret, 0);

    ret = APDS9301_mode_manualcontrolON();
    assert_int_equal(ret, 0);

    uint8_t *memdump = APDS9301_memDump();

    assert_non_null(memdump);

    /* Power up bits */
    assert_int_equal(memdump[0] & 0x3, 0x3);

    /* Timing register */
    assert_int_equal(memdump[1] & 0x1B, 0x1B);
    
    /* Interrupt control reg */
    assert_int_equal(memdump[6] & 0x3F, 0x10);

    free(memdump);

    // ret = APDS9301_mode_lowGain_default();
    // assert_int_equal(ret, 0);

    // ret = APDS9301_mode_integrationTime2_default();
    // assert_int_equal(ret, 0);

    // ret = APDS9301_mode_interruptDisable_default();
    // assert_int_equal(ret, 0);

    // ret = APDS9301_mode_manualcontrolOFF_default();
    // assert_int_equal(ret, 0);

    ret = APDS9301__setmode_allDefault();
    assert_int_equal(ret, 0);

    ret = APDS9301_readID(&data);
    assert_int_equal(data&0xF0, 0x50);

    int i = 0;
    while(i<2)
    {
        float lux = APDS9301_getLux();
        assert_int_not_equal(lux, -1);
        assert_in_range(lux,0,100);

        i++;
    }
    
    ret = APDS9301_powerdown();
    assert_int_equal(ret, 0);

    ret = APDS9301_readControlReg(&data);
    assert_int_equal(ret, 0);
    assert_int_equal((data & 0x3), 0);

    ret = I2Cmaster_Destroy(&i2c);
    assert_int_equal(ret, 0);
    assert_null((void*)getMasterI2C_handle());

}


int main()
{
    const struct CMUnitTest tests[] = {	
	
    cmocka_unit_test(testAPDS9301)
    
	};

	return cmocka_run_group_tests(tests, NULL, NULL);

}