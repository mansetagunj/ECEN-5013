/**
 * @brief 
 * 
 * @file tmp102_sensor.h
 * @author Gunj Manseta
 * @date 2018-03-13
 */
#ifndef TMP102SENSOR_H
#define TMP102SENSOR_H


#define TMP102_SLAVE_ADDR           (0x48)


/* Register address */
#define TMP102_REG_TEMPERATURE      (0x00) 
#define TMP102_REG_CONFIGURATION    (0x01)  
#define TMP102_REG_TLOW             (0x02)
#define TMP102_REG_THIGH            (0x03)  

   
#define EXTENDED_MODE(x)        uint8_t((x)<<4)        /*0:12 bit temp values (-55C to +128C)*/
#define CONVERSION_RATE(x)      (0x03)        /*4Hz Conversion rate*/ 
#define WAKEUP                  (0x00)        /*device maintains a continuous conversion state*/ 
#define SHUTDOWN                (0x01)        /*device is able to go into low power state between one shot conversions*/
#define THERMOSTAT_MODE         (0x00)        /*device is in comparator mode*/
#define POLARITY_MODE           (0x00)        /*Makes AL pin active low*/ 
#define FAULTBITS               (0x02)        /*generates alert after 4 consecutive faults*/
#define CONVERTER_RESOLUTION    (0x02)        /*Read only bits, sets temperature to a 12 bit resolution*/ 
#define ONESHOT_CR              (0x00)        /*saves power between conversions when 1*/ 

typedef enum temperature_unit
{
    CELCIUS = 0,
    FAHREN,
    KELVIN

}TEMPERATURE_UNIT_T;



int TMP102_getTemp(double *temp, TEMPERATURE_UNIT_T unit);

#endif