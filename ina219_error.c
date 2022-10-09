
#include "ina219_error.h"

char *ina219_error_name[] = {
    "INA219_ERROR_NONE",
    "INA219_ERROR",
    "INA219_SHUNT_ERROR",
    "INA219_BUS_ERROR",
    "INA219_BUS_NOT_READY_ERROR",
    "INA219_CURRENT_ERROR",
    "INA219_POWER_ERROR",
    "INA219_IC2_BUSY",
    "INA219_I2C_ERROR_ADDR_NACK",
    "INA219_I2C_ERROR_DATA_NACK",
    "INA219_I2C_ERROR_BUS_COLLISION",
    "INA219_CALIBRATE_FAIL",
    "INA219_READ_CALIBRATE_TIMEOUT",
    "INA219_UNKNOWN",
};

char *ina219_operation_name[] = {
    "INA219_INITIALIZE",
    "INA219_INITIALIZE_CALIBRATE",
    "INA219_READ_CALIBRATE",
    "INA219_CALEBRATE",
    "INA219_READ_SHUNT",
    "INA219_READ_BUS",
    "INA219_READ_CURRENT",
    "INA219_READ_POWER",
};

volatile uint8_t ina219_errno = 0;
uint8_t ina219_operation;

void INA219_ERROR_set(ina219_status_t error){
    ina219_errno = error;
};

ina219_status_t INA219_ERROR_get() {
    return ina219_errno;
}

char *INA219_ERROR_get_message() {
    return ina219_error_name[ina219_errno];
}

void INA219_ERROR_set_operation(ina219_operation_t operation){
    ina219_operation = operation;
}

char *INA219_ERROR_get_operation() {
    return ina219_operation_name[ina219_operation];
}
