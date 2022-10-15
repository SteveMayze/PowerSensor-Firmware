 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
© [2022] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"
#include "INA219.h"
#include <math.h>
#include <string.h>
#include "util/delay.h"
/*
    Main application
*/
#define INA219_ADDRESS INA219_ADDR_GND_GND

volatile bool tock;
bool ina219_ok = false;
ina219_status_t ina219_state;
char strbuf[128];
uint8_t buflen;

void tick(){
    tock = true;
}

void splitFloat(uint8_t *result, float value) {
    result[0] = trunc(value);
    result[1] = trunc((value - result[0]) * 10);
}

void write_data(uint8_t *data, uint8_t size){
    for(uint8_t i=0; i<size; i++) {
        while(!USART0_IsTxReady()){}
        USART0_Write(data[i]);
    }    
}

#define IN_HEX
int main(void)
{

    SYSTEM_Initialize();
    LED_BLUE_SetLow();    
    LED_GREEN_SetLow();   
    
    _delay_ms(1000);
    
    INA219_ERROR_set(INA219_ERROR_NONE);
    if(INA219_Initialise(INA219_ADDRESS)){
        ina219_state = INA219_ERROR_get();        
        LED_BLUE_SetLow();
        sprintf(strbuf, "INIT OK %0X %s %s\n", ina219_state, INA219_ERROR_get_message(), INA219_ERROR_get_operation());        
    } else {
        ina219_state = INA219_ERROR_get();        
        LED_BLUE_SetHigh();
        sprintf(strbuf, "INIT FAIL %0X %s %s\n", ina219_state, INA219_ERROR_get_message(), INA219_ERROR_get_operation());
    }
    buflen = strlen((char *)strbuf);
    write_data((uint8_t *)strbuf, buflen);
    
    RTC_SetOVFIsrCallback(tick);
    RTC_Start();      
    
    uint8_t data;
    struct ina219_data readings;
    
    while(1)
    {
       if(tock){
            if( USART0_IsRxReady() ) {
                data = USART0_Read();
                
                while(!USART0_IsTxReady()){}
                USART0_Write(data|0x80);

                INA219_ERROR_set(INA219_ERROR_NONE);
               if (INA219_getReadings( &readings )){    
                    LED_GREEN_SetHigh();
                    LED_BLUE_SetLow();

                    sprintf(strbuf, "SHUNT %04X %5.2f ", readings.raw_shunt_voltage, readings.shunt_voltage);
                    buflen = strlen((char *)strbuf);
                    write_data((uint8_t *)strbuf, buflen);
                    sprintf(strbuf, "BUS %04X %5.2f ", readings.raw_bus_voltage, readings.bus_voltage);
                    buflen = strlen((char *)strbuf);
                    write_data((uint8_t *)strbuf, buflen);

                    sprintf(strbuf, "CURRENT %04X %5.2f ", readings.raw_current, readings.current);
                    buflen = strlen((char *)strbuf);
                    write_data((uint8_t *)strbuf, buflen);

                    sprintf(strbuf, "POWER %04X %5.2f\n", readings.raw_power, readings.power);
                    buflen = strlen((char *)strbuf);
                    write_data((uint8_t *)strbuf, buflen);
                } else {
                    LED_GREEN_SetLow();
                    LED_BLUE_SetHigh();
                    ina219_state = INA219_ERROR_get();        
                    sprintf(strbuf, "ERROR %0X %s %s\n", ina219_state, INA219_ERROR_get_operation(), INA219_ERROR_get_message());
                    buflen = strlen((char *)strbuf);
                    write_data((uint8_t *)strbuf, buflen);
                }
            }
            LED_RED_Toggle();                
            tock = false;
            
       }
    }    
}