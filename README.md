# PowerSensor-Firmware
Firmware to support the leawood_gateway

# Overview
The leawood_gateway project created a Python based gateway to receive messages via an XBee device and post them to a database using REST APIs. The idea of that project was to provide the simple infrastructure in which to develop and test further sensor units. The PowerSensor is the first such sensor. It is to be simple i.e. it will be solely based on 8bit archtecture using an ATTiny1614. The reason for this, amongst other things, this is what I have on hand with enough GPIO to get the job done.

# Architecture
Development will be using MPLAB from Microsystems and taking advantage of the Code Configurator.
The device will have a button, an RGB LED as an additional indicator. Other external devices, that will communicate via TWI (I2C) will include a temperature sensor, the INA219 for power measurement and a display.

# Operation
The basic operation - as expected by the gateway - is that the device will wake every so often and send a READY. The gateway will respond with either a DATAREQ message for a kown sensor or a DATAINTRO message for a sensor that needs to be introduced to a gateway. Once these requests are processed i.e. responding with some introductory information or collected data, the sensor unit will go to sleep for an amount of time (yet to be determined).

## DATAINTRO
The response to this message means that the sensor unit needs to introduce itself to the gateway. This will tell the gateway that type of device it is and the expected data to receive. It was found that due to the message size constraints of the XBee, the data needed to be encoded. It was decided to maintain a central store of the possible metrics and the senor units will deal with standarised token values for the metrics.
On sucessful receipt of the message, the sensor unit can expect an INTROACK message. If this message is not received in a certian time (to be determined), then the sensor unit will go back to sleep and try again another time - here the next message telegram could contain a metric to indicate the attempt count.

## DATAREQ
For a known sensor unit, on receiving a DATAREQ, the unit can transmit the data it has collected. The metrics collected and transmitted will be tokenised to minimise the size of the data message. There will be a repository of standard metric names, units and multipliers.
On successful receipt of the message, the sensor unit can expect a DATAACK message. If this message is not received in a certian time (to be determined), then the sensor unit will go back to sleep and try again another time - here the next message telegram could contain a metric to indicate the attempt count.

```python
HEADER_GROUP = 0x10
OPERATION_GROUP = 0x20
PROPERTY_GROUP = 0x30
DATA_GROUP = 0x40
METADATA_GROUP = 0x50

telegram_token = {

    "operation": HEADER_GROUP | 0x01,
    "serial_id": HEADER_GROUP | 0x02,
    "domain": HEADER_GROUP | 0x03,
    "class": HEADER_GROUP | 0x04,

    "p": PROPERTY_GROUP | 0x00,
    "bus_voltage": PROPERTY_GROUP | 0x01,
    "shunt_voltage":  PROPERTY_GROUP | 0x02,
    "load_current":  PROPERTY_GROUP | 0x03,

    "READY": OPERATION_GROUP | 0x01,
    "DATAREQ": OPERATION_GROUP | 0x02,
    "DATA": OPERATION_GROUP | 0x03,
    "DATAACK": OPERATION_GROUP | 0x04,
    "NODEINTROREQ": OPERATION_GROUP | 0x05,
    "NODEINTRO": OPERATION_GROUP | 0x06,
    "NODEINTROACK": OPERATION_GROUP | 0x07,

    'power': METADATA_GROUP | 0x01,
    'capacity': METADATA_GROUP | 0x04,
    'sensor': METADATA_GROUP | 0x03,
}
```

## Pseudo code
The pseudo code needs to be fleshed out more to hadle such things like sleepsing, waking from sleep and timeouts when waiting for responses.

```C
while(1) {
    if(onWake){
        send_ready();
        ack = wait_ack();
        switch(ack){
           case data_ack:
              package_and_intro();
              break;
           case data_ack:
              package_and_send_data();
              break;
           default: break;
        }
    }
}
```
