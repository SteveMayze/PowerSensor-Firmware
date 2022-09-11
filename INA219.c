
#include "mcc_generated_files/system/system.h"
#include "INA219.h"
#include "util/delay.h"

static volatile uint8_t iic_address;

uint8_t write_buffer[3];
union read_buffer_t {
    uint16_t bit16;
    uint8_t bit8[2];

};

union read_buffer_t read_buffer;

#define INA219_DEFAULT_CFG 0x199F
#define INA219_DEFAULT_CFG_0 0x19
#define INA219_DEFAULT_CFG_1 0x9F

#define CAL 0x1000
#define CAL_0 0x10
#define CAL_1 0x00

i2c_host_interface_t i2c;

void INA219_set_calibration(){
    uint8_t data[3];
    data[0] = INA219_CAL;
    data[1] = CAL >> 8;
    data[2] = 0xFF & CAL;
    TWI0_Write(iic_address, data, 2);    
}

void INA219_Initialise(uint8_t addr) {
    iic_address = addr;
    
    INA219_set_calibration();

    uint8_t data[3];
    data[0] = INA219_CFG;
    data[1] = INA219_DEFAULT_CFG >> 8;
    data[2] = 0xFF & INA219_DEFAULT_CFG;
    TWI0_Write(iic_address, data, 2);

}


uint16_t get_regsiter_value(uint8_t reg, bool calibrate){
    if( calibrate )
        INA219_set_calibration();
    write_buffer[0] = reg;
    TWI0_WriteRead(iic_address, write_buffer, 1, read_buffer.bit8, 2);
    uint16_t result = ((uint16_t) read_buffer.bit8[0]<<8)|read_buffer.bit8[1];
    return result;
}


struct ina219_data INA219_getReadings() {

    struct ina219_data readings;
    uint8_t timeout = 0;

    readings.bus_voltage = 0.0;
    readings.shunt_voltage = 0.0;
    readings.current = 0.0;
    readings.power = 0.0;
    bool reading_ok = false;
    while (!reading_ok) {
        readings.raw_bus_voltage = get_regsiter_value(INA219_BUS_VOLTAGE, false);
        reading_ok = readings.raw_bus_voltage & 0x02;
        _delay_ms(500);
        timeout++;
        if(timeout > 10){
            LED_BLUE_Toggle();
            break;
        }
    }
    readings.bus_voltage = (int16_t) ((readings.raw_bus_voltage >> 3) * 4);
    readings.bus_voltage = readings.bus_voltage * 0.001;

    readings.raw_shunt_voltage = get_regsiter_value(INA219_SHUNT_VOLTAGE, false);
    readings.shunt_voltage = readings.raw_shunt_voltage * 0.01;


    readings.raw_current = get_regsiter_value(INA219_CURRENT, true);
    readings.current = ((float) readings.raw_current / 10.0);

    readings.raw_power = get_regsiter_value(INA219_POWER, true);
    readings.power = ((float) readings.raw_power * 2.0);

    return readings;

}

