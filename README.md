# BME680-RP2040 

This repository consits of example for interfacing _Rasberry pi pico board(RP2040)_ with _DFRobot BME680_ using _I2C_ interface. BME680 driver has been used to get all the functionality of the sensor. 

### Pin connections

| PICO PIN | BME680 PIN |
| ------------- | ------------- |
| 3.3V  | VCC  |
| GND   | GND  |
| SDA   | 4    |
| SDA   | 5    |

### Files to make this project work

![image](https://github.com/Thunderbolt2-o/bme680-rp2040/assets/68735588/57d1ef51-6c79-4916-9c3c-518cc5b3f96c)

Driver files can be taken from Bosch [BME68x sensor API](https://github.com/boschsensortec/BME68x_SensorAPI) <br />

[BME68x.h](bme68x/bme68x.h) <br />
[BME68x_defs.h](bme68x/bme68x_defs.h) <br />
[BME68x.c](bme68x/bme68x.c) <br />
[main.c](main.c) <br />
[CMakeLists.txt](CMakeLists.txt) <br />

### References

[RP2040 PINOUT](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html) <br />
[RP2040 C SDK](https://www.raspberrypi.com/documentation/pico-sdk/hardware.html) <br />
[BME680 DATASHEET](https://cdn-shop.adafruit.com/product-files/3660/BME680.pdf) <br />

### Output

![pico with bme680](https://github.com/Thunderbolt2-o/bme680-rp2040/assets/68735588/e2c8144a-2145-43d1-b0d1-51200b51f4a6)
![pico with bme680_output](https://github.com/Thunderbolt2-o/bme680-rp2040/assets/68735588/b688d001-8278-465f-95b2-bb851201eee0)
