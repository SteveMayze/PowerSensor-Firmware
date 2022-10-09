
#include "mcc_generated_files/system/system.h"
#include "INA219.h"
#include "util/delay.h"
#include <string.h>
#include "ina219_error.h"

static volatile uint8_t iic_address;

uint8_t write_buffer[10];
uint8_t read_buffer[10];

#define INA219_DEFAULT_CFG 0x199F
#define INA219_DEFAULT_CFG_0 0x19
#define INA219_DEFAULT_CFG_1 0x9F

#define CAL 0x1000
#define CAL_0 0x10
#define CAL_1 0x00

i2c_host_interface_t i2c;


/**
 * Sets the calibration value
 * 
 * @return 
 */
bool INA219_set_calibration(){
    write_buffer[0] = INA219_CAL;
    write_buffer[1] = CAL >> 8;
    write_buffer[2] = 0xFF & CAL;
    if(TWI0_Write(iic_address, write_buffer, 3))
    {        
        return true;
    }
    // Confirm write operation completed and check for error
    if(!TWI0_IsBusy())
    {
        if ( TWI0_ErrorGet() == I2C_ERROR_NONE)
        {
            return true;
        }
        else
        {
            i2c_host_error_t error_state = TWI0_ErrorGet();
            switch(error_state){
                case(I2C_ERROR_NONE):
                    return true;
                    break;
                case(I2C_ERROR_ADDR_NACK):
                    INA219_ERROR_set(INA219_I2C_ERROR_ADDR_NACK);
                    return false;
                    break;
                case(I2C_ERROR_DATA_NACK):
                    INA219_ERROR_set(INA219_I2C_ERROR_DATA_NACK);
                    return false;
                    break;
                case(I2C_ERROR_BUS_COLLISION):
                    INA219_ERROR_set(INA219_I2C_ERROR_BUS_COLLISION);
                    return false;
                    break;
                default:
                    INA219_ERROR_set(INA219_UNKOWN);
                    return false;
            }
        }
    } else {
            INA219_ERROR_set(INA219_IC2_BUSY);
            return false;
    }   
    return true;
}

bool INA219_Initialise(uint8_t addr) {
    INA219_ERROR_set_operation(INA219_INITIALIZE);
    iic_address = addr;

    write_buffer[0] = INA219_CFG;
    write_buffer[1] = INA219_DEFAULT_CFG >> 8;
    write_buffer[2] = 0xFF & INA219_DEFAULT_CFG;
    if(TWI0_Write(iic_address, write_buffer, 3))
    {        
        return true;
    }

    // Confirm write operation completed and check for error
    if(!TWI0_IsBusy())
    {
        if ( TWI0_ErrorGet() == I2C_ERROR_NONE)
        {
            return true;
        }
        else
        {
            i2c_host_error_t error_state = TWI0_ErrorGet();
            switch(error_state){
                case(I2C_ERROR_NONE):
                    return true;
                    break;
                case(I2C_ERROR_ADDR_NACK):
                    INA219_ERROR_set(INA219_I2C_ERROR_ADDR_NACK);
                    return false;
                    break;
                case(I2C_ERROR_DATA_NACK):
                    INA219_ERROR_set(INA219_I2C_ERROR_DATA_NACK);
                    return false;
                    break;
                case(I2C_ERROR_BUS_COLLISION):
                    INA219_ERROR_set(INA219_I2C_ERROR_BUS_COLLISION);
                    return false;
                    break;
                default:
                    INA219_ERROR_set(INA219_UNKOWN);
                    return false;
            }
        }
    } else {
            INA219_ERROR_set(INA219_IC2_BUSY);
            return false;
    }    
    INA219_ERROR_set_operation(INA219_INITIALIZE_CALIBRATE);
    if(!INA219_set_calibration())
        return false;
    return true;
}


bool get_regsiter_value(uint8_t reg, bool calibrate, uint16_t *value){
    if( calibrate ){
//        if(!INA219_set_calibration()){
//            INA219_ERROR_set_operation(INA219_READ_CALIBRATE);
//            return false;
//        }
        uint8_t timeout = 0;
        while(!INA219_set_calibration()){
            timeout++;
            if(timeout>10){
                INA219_ERROR_set(INA219_READ_CALIBRATE_TIMEOUT);
                return false;
            }
            _delay_ms(100);
        }
    }
    _delay_ms(500);
        
    write_buffer[0] = reg;
    
    if( TWI0_WriteRead(iic_address, write_buffer, 1, read_buffer , 2)){
        *value = ((uint16_t) read_buffer[0]<<8)|read_buffer[1];
        return true;
    }

    // Confirm write operation completed and check for error
    if(!TWI0_IsBusy()){
        i2c_host_error_t error_state = TWI0_ErrorGet();
        switch(error_state){
            case(I2C_ERROR_NONE):
                *value = ((uint16_t) read_buffer[0]<<8)|read_buffer[1];
                return true;
                break;
            case(I2C_ERROR_ADDR_NACK):
                INA219_ERROR_set(INA219_I2C_ERROR_ADDR_NACK);
                return false;
                break;
            case(I2C_ERROR_DATA_NACK):
                INA219_ERROR_set(INA219_I2C_ERROR_DATA_NACK);
                return false;
                break;
            case(I2C_ERROR_BUS_COLLISION):
                INA219_ERROR_set(INA219_I2C_ERROR_BUS_COLLISION);
                return false;
                break;
            default:
                INA219_ERROR_set(INA219_UNKOWN);
                return false;
        }
    } else {
            INA219_ERROR_set(INA219_IC2_BUSY);
            return false;
    }      
    return true;
}


bool INA219_getReadings(struct ina219_data *readings) {

    uint8_t timeout = 0;
    uint16_t value;

    readings->bus_voltage = 0.0;
    readings->shunt_voltage = 0.0;
    readings->current = 0.0;
    readings->power = 0.0;
    bool reading_ok = false;
    // INA219_ERROR_set(INA219_ERROR_NONE);
    INA219_ERROR_set_operation(INA219_READ_BUS);
    while (!reading_ok) {        
        if(get_regsiter_value(INA219_BUS_VOLTAGE, false, &value)){
            reading_ok = value & 0x02;
            if (reading_ok){
                break;
            } else {
                timeout++;
                if(timeout > 10){
                    INA219_ERROR_set(INA219_BUS_NOT_READY_ERROR);
                    break;
                }
                _delay_ms(100);
            }
        } else {
            return false;
        }
    }
    _delay_ms(500);
    
    readings->raw_bus_voltage = value;
    readings->bus_voltage = (int16_t) ((readings->raw_bus_voltage >> 3) * 4);
    readings->bus_voltage = readings->bus_voltage * 0.001;

    // INA219_ERROR_set(INA219_ERROR_NONE);
    INA219_ERROR_set_operation(INA219_READ_SHUNT);
    reading_ok = false;
    timeout = 0;
    while(!reading_ok){
        reading_ok = get_regsiter_value(INA219_SHUNT_VOLTAGE, false, &value);
        if(reading_ok){
            readings->raw_shunt_voltage = value;
            readings->shunt_voltage = value * 0.01;
            break;
        } else {
            timeout++;
            if(timeout > 10){
                break;
            }
            _delay_ms(100);
        }
    }
    if(!reading_ok)
        return false;
    
    _delay_ms(500);


    // INA219_ERROR_set(INA219_ERROR_NONE);
    INA219_ERROR_set_operation(INA219_READ_CURRENT);
//    if(get_regsiter_value(INA219_CURRENT, true, &value)){
//        readings->raw_current = value;
//        readings->current = ((float) readings->raw_current / 10.0);
//    } else {
//        return false;
//    }

    reading_ok = false;
    timeout = 0;
    while(!reading_ok){
        reading_ok = get_regsiter_value(INA219_CURRENT, true, &value);
        if(reading_ok){
            readings->raw_current = value;
            readings->current = ((float) readings->raw_current / 10.0);
            break;
        } else {
            timeout++;
            if(timeout > 10){
                break;
            }
            _delay_ms(100);
        }
    }
    if(!reading_ok)
        return false;

     _delay_ms(500);

    
   // INA219_ERROR_set(INA219_ERROR_NONE);
   INA219_ERROR_set_operation(INA219_READ_POWER);
//    if(get_regsiter_value(INA219_POWER, true, &value)){
//        readings->raw_power = value;
//        readings->power = ((float) readings->raw_power * 2.0);
//    } else {
//        return false;
//    }

    reading_ok = false;
    timeout = 0;
    while(!reading_ok){
        reading_ok = get_regsiter_value(INA219_POWER, true, &value);
        if(reading_ok){
        readings->raw_power = value;
        readings->power = ((float) readings->raw_power * 2.0);
            break;
        } else {
            timeout++;
            if(timeout > 10){
                break;
            }
            _delay_ms(100);
        }
    }
    if(!reading_ok)
        return false;



    return true;

}

