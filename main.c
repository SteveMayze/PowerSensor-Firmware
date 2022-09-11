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
/*
    Main application
*/
#define INA219_ADDRESS INA219_ADDR_GND_GND

volatile bool tock;

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
    INA219_Initialise(INA219_ADDRESS);
    RTC_SetOVFIsrCallback(tick);
    RTC_Start();  
    
    uint8_t data;
    
    LED_BLUE_SetHigh();
    
    struct ina219_data readings;
//    uint8_t vshunt[2];
//    uint8_t vbus[2];
//    uint8_t current[2];
//    uint8_t power[2];
    
    while(1)
    {
       if(tock){
            if( USART0_IsRxReady() ) {
                data = USART0_Read();
                LED_GREEN_Toggle();
                LED_BLUE_SetLow();
                
                while(!USART0_IsTxReady()){}
                USART0_Write(data|0x80);

                char strbuf[20];
                uint8_t buflen;
                readings = INA219_getReadings();
#ifdef IN_HEX
                sprintf(strbuf, "%04X", readings.raw_shunt_voltage);
                buflen = strlen((char *)strbuf);
                USART0_Write(0x01);
                write_data((uint8_t *)strbuf, buflen);
                USART0_Write(0x00);

                sprintf(strbuf, "%04X", readings.raw_bus_voltage);
                buflen = strlen((char *)strbuf);
                USART0_Write(0x02);
                write_data((uint8_t *)strbuf, buflen);
                USART0_Write(0x00);

                sprintf(strbuf, "%04X", readings.raw_current);
                buflen = strlen((char *)strbuf);
                USART0_Write(0x03);
                write_data((uint8_t *)strbuf, buflen);
                USART0_Write(0x00);
#else
                sprintf(strbuf, "%5.2f", readings.shunt_voltage);
                buflen = strlen((char *)strbuf);
                USART0_Write(0x01);
                write_data((uint8_t *)strbuf, buflen);
                USART0_Write(0x00);

                sprintf(strbuf, "%5.2f", readings.bus_voltage);
                buflen = strlen((char *)strbuf);
                USART0_Write(0x02);
                write_data((uint8_t *)strbuf, buflen);
                USART0_Write(0x00);

                sprintf(strbuf, "%5.2f", readings.current);
                buflen = strlen((char *)strbuf);
                USART0_Write(0x03);
                write_data((uint8_t *)strbuf, buflen);
                USART0_Write(0x00);

                sprintf(strbuf, "%5.2f", readings.power);
                buflen = strlen((char *)strbuf);
                USART0_Write(0x04);
                write_data((uint8_t *)strbuf, buflen);
                USART0_Write(0x00);
#endif                
                USART0_Write(0xFE);
                
                
            }
            LED_RED_Toggle();                
            tock = false;
            
       }
    }    
}