/* 
 * File:   common_error.h
 * Author: Steven
 *
 * Created on 09 October 2022, 11:49
 */

#ifndef COMMON_ERROR_H
#define	COMMON_ERROR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/system/system.h"
    

typedef enum
{
    INA219_INITIALIZE,
    INA219_INITIALIZE_CALIBRATE,
    INA219_READ_CALIBRATE,
    INA219_READ_SHUNT,
    INA219_READ_BUS,
    INA219_READ_CURRENT,
    INA219_READ_POWER,

} ina219_operation_t;

typedef enum
{
    INA219_ERROR_NONE,             /* No Error */
    INA219_ERROR,
    INA219_SHUNT_ERROR,
    INA219_BUS_ERROR,
    INA219_BUS_NOT_READY_ERROR,
    INA219_CURRENT_ERROR,
    INA219_POWER_ERROR,
    INA219_IC2_BUSY,
    INA219_I2C_ERROR_ADDR_NACK,
    INA219_I2C_ERROR_DATA_NACK,
    INA219_I2C_ERROR_BUS_COLLISION,
    INA219_CALIBRATE_FAIL,/* Bus Collision Error */
    INA219_READ_CALIBRATE_TIMEOUT,
    INA219_UNKOWN,

} ina219_status_t;

extern char *ina219_error_name[];
extern char *ina219_operation_name[];

void INA219_ERROR_set(ina219_status_t error);

ina219_status_t INA219_ERROR_get();

char *INA219_ERROR_get_message();

void INA219_ERROR_set_operation(ina219_operation_t operation);

char *INA219_ERROR_get_operation();


#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_ERROR_H */

