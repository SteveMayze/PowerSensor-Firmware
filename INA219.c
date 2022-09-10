
#include "mcc_generated_files/system/system.h"
#include "INA219.h"
#include "util/delay.h"

static volatile uint8_t iic_address;

typedef union {
    uint8_t buffer[3];
    struct {
        uint16_t data;
        uint8_t spare;
    };
} i2c_data_t;

i2c_data_t i2c_data; 

// #define CAL_MSBFIRST 0x0010
#define CAL_MSBFIRST_0 0x0E
#define CAL_MSBFIRST_1 0x0F

i2c_host_interface_t i2c;

void INA219_Initialise(uint8_t addr) {
    iic_address = addr;
    
    i2c_data.buffer[0] = INA219_CAL;
    i2c_data.buffer[1] = CAL_MSBFIRST_0;
    i2c_data.buffer[2] = CAL_MSBFIRST_1;
    TWI0_Write(iic_address, i2c_data.buffer, 3 );
    i2c_data.buffer[0] = INA219_CFG;
    i2c_data.buffer[1] = INA219_DEFAULT_CFG_MSBFIRST_0;
    i2c_data.buffer[2] = INA219_DEFAULT_CFG_MSBFIRST_1;
    TWI0_Write(iic_address, i2c_data.buffer, 3 );
}

uint16_t get_shunt_voltage_raw() {
    uint8_t shunt_voltage = INA219_SHUNT_VOLTAGE;
    TWI0_WriteRead(iic_address, &shunt_voltage, 1, i2c_data.buffer, 2);
    
    return i2c_data.data;
}

uint16_t get_bus_voltage_raw() {
    uint8_t shunt_voltage = INA219_BUS_VOLTAGE;
    TWI0_WriteRead(iic_address, &shunt_voltage, 1, i2c_data.buffer, 2);
    return i2c_data.data;
}

uint16_t get_current_raw() {
    uint8_t shunt_voltage = INA219_CURRENT;
    TWI0_WriteRead(iic_address, &shunt_voltage, 1, i2c_data.buffer, 2);
    return i2c_data.data;
}

uint16_t get_power_raw() {
    uint8_t shunt_voltage = INA219_POWER;
    TWI0_WriteRead(iic_address, &shunt_voltage, 1, i2c_data.buffer, 2);
    return i2c_data.data;
}

struct ina219_data INA219_getReadings() {

    struct ina219_data readings;

    readings.bus_voltage = 0.0;
    readings.shunt_voltage = 0.0;
    readings.current = 0.0;
    readings.power = 0.0;
    bool reading_ok = false;
    while (!reading_ok) {
        readings.raw_bus_voltage = get_bus_voltage_raw();
        reading_ok = readings.raw_bus_voltage & 0x02;
        _delay_ms(500);
        LED_BLUE_Toggle();
    }
    readings.bus_voltage = (int16_t) ((readings.raw_bus_voltage >> 3) * 4);
    readings.bus_voltage = readings.bus_voltage * 0.001;

    readings.raw_shunt_voltage = get_shunt_voltage_raw();
    readings.shunt_voltage = readings.raw_shunt_voltage * 0.01;


    readings.raw_current = get_current_raw();
    readings.current = ((float) readings.raw_current / 10.0);

    readings.raw_power = get_power_raw();
    readings.power = ((float) readings.raw_power * 2.0);

    return readings;

}

