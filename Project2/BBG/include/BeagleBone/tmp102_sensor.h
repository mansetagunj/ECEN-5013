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
#define TMP102_REG_TEMPERATURE          (0x00) 
#define TMP102_REG_CONFIGURATION        (0x01)  
#define TMP102_REG_TLOW                 (0x02)
#define TMP102_REG_THIGH                (0x03)  



#define TMP102_CONFIG_SD                (1)
#define TMP102_CONFIG_TM                (1<<1)
#define TMP102_CONFIG_POL               (1<<2) 
#define TMP102_CONFIG_EM                (1<<12)
#define TMP102_CONFIG_AL                (1<<13)
#define TMP102_CONFIG_CR(x)             (x<<14)
            
#define TMP102_CONFIG_FAULTBITS                (3<<3)              /*generates alert after 4 consecutive faults*/
#define TMP102_CONFIG_ONESHOT_CR               (1<7)              /*saves power between conversions when 1*/ 

typedef enum temperature_unit
{
    CELCIUS = 0,
    FAHREN,
    KELVIN

}TEMPERATURE_UNIT_T;

typedef struct
{
    ///* (D)0 = Continuous conversion; 1 = Can sleep*/
    uint16_t SD_MODE:1;
    ///* (D)0 = Comparatore mode; 1 = Interrupt mode*/
    uint16_t TM_MODE:1;
    ///* (D)0 = ALERT pin becomes active low; 1 = ALERT pin becomes active high and the state of the ALERT pin is inverted. */
    uint16_t POL:1;
    ///* (D)0 = 1Fault; 1= 2Faults; 3 = 4Faults; 4=6Faults */
    uint16_t FAULT:2;
    const uint16_t RES0:2;
    ///* (D)0 =  During the conversion, the OS bit reads '0'; 1 = writing a 1 to the OS bit starts a single temperature conversion */
    uint16_t OS:1;
    const uint16_t RES1:4;
    ///* (D)0 = Normal mode(12bit); 1= Extended mode(13 bit) */
    uint16_t EM_MODE:1;
    ///*  Reads the AL bit*/
    const uint16_t RO_AL_MODE:1;
    ///* 0 = 0.25Hz ; 1 = 1Hz ; (D)2 = 4Hz ; 3 = 8Hz */
    uint16_t CR:2;

}TMP102_CONFIG_REG_SETTINGS_T;

#define TMP102_CONFIG_DEFAULT_ASSIGN \
{\
    .SD_MODE = 0,\
    .TM_MODE = 0,\
    .POL     = 0,\
    .OS      = 0,\
    .EM_MODE = 0,\
    .CR      = 2\
}

extern const TMP102_CONFIG_REG_SETTINGS_T TMP102_CONFIG_DEFAULT;

int TMP102_setMode(TMP102_CONFIG_REG_SETTINGS_T mode);

/**
 * @brief Brings back to default
 * 
 * @return int 
 */
int TMP102_setmode_allDefault();

/**
 * @brief Gives a memdump of 4 len. 
 *  **IMP** must free the address using return pointer 
 * @return uint8_t* 
 */
uint16_t* TMP102_memDump();

/**
 * @brief Abstracted macros for different units
 * 
 */
#define TMP102_getTemp_Celcius(p_temp)  TMP102_getTemp(p_temp, CELCIUS)
#define TMP102_getTemp_Kelvin(p_temp)   TMP102_getTemp(p_temp, KELVIN)
#define TMP102_getTemp_Fahren(p_temp)   TMP102_getTemp(p_temp, FAHREN)


/**
 * @brief Gets the temperature value
 * 
 * @param temp 
 * @param unit 
 * @return int 
 */
int TMP102_getTemp(float *temp, TEMPERATURE_UNIT_T unit);

/**
 * @brief 
 * 
 * @return int 
 */
int TMP102_setMode_SD_PowerSaving();

/**
 * @brief 
 * 
 * @return int 
 */
int TMP102_setMode_SD_Continuous_default();

/**
 * @brief 
 * 
 * @return int 
 */
int TMP102_setMode_TM_ComparatorMode_default();

/**
 * @brief 
 * 
 * @return int 
 */
int TMP102_setMode_TM_InterruptMode();

/**
 * @brief 
 * 
 * @return int 
 */
int TMP102_setMode_ALERT_ActiveLow_default();

/**
 * @brief 
 * 
 * @return int 
 */
int TMP102_setMode_ALERT_ActiveHigh();

/**
 * @brief 
 * 
 * @return int 
 */
int TMP102_setMode_EM_NormalMode_default();

/**
 * @brief 
 * 
 * @return int 
 */
int TMP102_setMode_EM_ExtendedMode();

/**
 * @brief 
 * 
 * @return int 
 */
int TMP102_setMode_CR_250mHZ();

/**
 * @brief 
 * 
 * @return int 
 */
int TMP102_setMode_CR_1HZ();

/**
 * @brief 
 * 
 * @return int 
 */
int TMP102_setMode_CR_4HZ_default();

/**
 * @brief 
 * 
 * @return int 
 */
int TMP102_setMode_CR_8HZ();

/**
 * @brief 
 * 
 * @param al_bit 
 * @return int 
 */
int TMP102_readMode_ALERT(uint8_t *al_bit);

/**
 * @brief 
 * 
 * @param tlow_C 
 * @return int 
 */
int TMP102_write_Tlow(float tlow_C);

/**
 * @brief 
 * 
 * @param thigh_C 
 * @return int 
 */
int TMP102_write_Thigh(float thigh_C);

/**
 * @brief 
 * 
 * @param tlow_C 
 * @return int 
 */
int TMP102_read_Tlow(float *tlow_C);

/**
 * @brief 
 * 
 * @param thigh_C 
 * @return int 
 */
int TMP102_read_Thigh(float *thigh_C);


#endif