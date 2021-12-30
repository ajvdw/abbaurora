# ABB Aurora (ESPHOME)
 
This is an ESPHome component for the ABB Aurora SOLAR inverter.
It is connected via RS485 to an ESP8266 (Wemos D1 mini).

The code is based on https://github.com/jrbenito/ABBAurora 
that again was based on the work of Davide Rosa 
https://www.drhack.it/arduino/32-lettura-inverte-power-one-aurora.html

The original work was made available under the LGPL v2.1 license.
I published the modifications under GPL v3.

An example of a configuration for ESPHome:

```
substitutions:
  device_name: zonnepanelen
  device_description: "ABB Aurora Solar Inverter"
     
esphome:
  name: ${device_name}
  comment: "${device_description}"
  platform: ESP8266
  esp8266_restore_from_flash: true
  board: d1_mini
  name_add_mac_suffix: false

external_components:
  #use from main branch in GitHub
  - source: "github://ajvdw/abbaurora"
    components: [abbaurora]
    refresh: 0s
    
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  manual_ip:
    static_ip: 192.168.2.114
    gateway: 192.168.2.254
    subnet: 255.255.255.0  
 
  # Enable fallback hotspot (captive portal) in case wifi connection fails
  ap:
    ssid: ${device_name}
 
captive_portal:
 
# Enable logging
logger:
  level: info
 
# Enable Home Assistant API
api:

ota:
  password: !secret ota_password
 
web_server:
  port: 80
 
uart:
  rx_pin: GPIO12
  tx_pin: GPIO14
  baud_rate: 19200

abbaurora:
  id: inverter1
  flow_control_pin: GPIO13
  address: 2

sensor:
  - platform: abbaurora
    abbaurora_id: inverter1
    cumulated_energy_total: 
      name: "ElecProductionSolar"
      unit_of_measurement: 'kWh'
      accuracy_decimals: 3
      filters:
      - multiply: 0.001
    v_in_1:
      name: "Voltage IN 1"
    v_in_2:
      name: "Voltage IN 2"
    i_in_1:
      name: "Current IN 1"
    i_in_2:
      name: "Current IN 2"
    power_in_1:
      name: "Power IN 1" 
    power_in_2:
      name: "Power IN 2"  
    power_in_total:
      name: "Power IN Total" 
    grid_power:
      name: "SolarPower"
    temperature_inverter:
      name: "TemperatureInverter"
    temperature_booster:
      name: "TemperatureBooster"
    grid_voltage:
      name: "GridVoltageSolar"
    cumulated_energy_today:
      name: "SolarEnergyToday"

text_sensor:
  - platform: abbaurora
    abbaurora_id: inverter1
    version:
      name: "Inverter type"
    connection_status:
      name: "Inverter RS485 Status"
    identification:
      name: "Inverter identification"
  - platform: version
    name: "ESPHome Version"
    hide_timestamp: true
 ```   
