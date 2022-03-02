# Air Pollution Monitoring System
This system aims to monitor the air quality index using sensors that are deployed to measure this index. By supposing these sensors are equipped with LEDs, proper LED light is turned on considering the value of index that is commanded by a controller. System is composed of 3 main parts:
* MQTT Network 
* CoAP Network
* Collector (that is connected to a database)

## Deployment
System must be deployed on different environments. The MQTT network is simulated with real sensors on an IoT testbed. The CoAP network instead is on local machine using Cooja Simulator. Finally, the collector is running on the local machine.
* NOTE: 
### MQTT Network
Connect to the testbed (with the correct address instead of XX) via a terminal:
```
ssh -i ./key -p 20XX user@iot.dii.unipi.it
```
Go inside rpl-border-router folder and flash the program:
```
make TARGET=nrf52840 BOARD=dongle border-router.dfu-upload PORT=/dev/ttyACMXX
```
Then get the log:
```
make TARGET=nrf52840 BOARD=dongle connect-router PORT=/dev/ttyACMXX
```
After the border router is deployed, you need to lunch the Mosquitto broker in order to connect the MQTT client inside the collector with the MQTT client inside the testbed. Open another terminal and connect to the testbed by forwarding the port 1883:
```
ssh -L 1883:127.0.0.1:1883 -i ./key -p 20XX user@iot.dii.unipi.it
```
Run Mosquitto:
```
sudo mosquitto -c /etc/mosquitto/mosquitto.conf
```
Open the third terminal and connect to the testbed:
```
ssh -i ./key -p 20XX user@iot.dii.unipi.it
```
Go inside the folder for MQTT network and flash the program:
```
make TARGET=nrf52840 BOARD=dongle mqtt-client.dfu-upload PORT=/dev/ttyACMXX
```
Get the log:
```
make TARGET=nrf52840 BOARD=dongle login PORT=/dev/ttyACMXX
```
### CoAP Network
First you need to log into contikier and launch Cooja:
```
contikier
cd tools/cooja
ant run
```
Now start a new simulation on Cooja and deploy the border router and connect the serial server socket. Then, Go into rpl-border-router folder and lunch tunslip6:
```
make TARGET=cooja connect-router-cooja
```
After deploying the CoAP sensors on the simulation you can start the simulation.
### Collector
Simply run the collector with:
```
Python main.py
```
