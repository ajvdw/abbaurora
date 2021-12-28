#pragma once

#include "esphome/core/application.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

#include "ABBAuroraEnums.h"
#include "ABBAuroraStrings.h"

namespace esphome {
namespace abbaurora {

class ABBAuroraComponent : public uart:: UARTDevice, Component
{
public:
//    ABBAuroraComponent() : PollingComponent(2500) {}
    ABBAuroraComponent() : Component() {}
protected:
    GPIOPin *flow_control_pin_{nullptr};

public:
    void setup() override;
    void loop() override;
//    void update() override;
    void dump_config() override;
    void set_address(uint8_t address) {  Address = address; }
    void set_flow_control_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
    void set_cumulated_energy_total( sensor::Sensor *sensor ) { this->cumulated_energy_total = sensor; }
    void set_v_in_1( sensor::Sensor *sensor ) { this->v_in_1 = sensor; }
    void set_v_in_2( sensor::Sensor *sensor ) { this->v_in_2 = sensor; }
    void set_i_in_1( sensor::Sensor *sensor ) { this->i_in_1 = sensor; }
    void set_i_in_2( sensor::Sensor *sensor ) { this->i_in_2 = sensor; }
    void set_power_in_1( sensor::Sensor *sensor ) { this->power_in_1 = sensor; }
    void set_power_in_2( sensor::Sensor *sensor ) { this->power_in_2 = sensor; }
    void set_power_in_total( sensor::Sensor *sensor ) { this->power_in_total = sensor; }
    void set_grid_power( sensor::Sensor *sensor ) { this->grid_power = sensor; }
    void set_temperature_inverter( sensor::Sensor *sensor ) { this->temperature_inverter = sensor; }
    void set_temperature_booster( sensor::Sensor *sensor ) { this->temperature_booster = sensor; }
    void set_grid_voltage( sensor::Sensor *sensor ) { this->grid_voltage = sensor; }
    void set_cumulated_energy_today( sensor::Sensor *sensor ) { this->cumulated_energy_today = sensor; }
    void set_version( text_sensor::TextSensor *sensor ) { this->version = sensor; }
    void set_connection_status( text_sensor::TextSensor *sensor ) { this->connection_status = sensor; }
    void set_identification( text_sensor::TextSensor *sensor ) { this->identification = sensor; }

private:
    uint8_t connection = 0;
    int MaxAttempt = 1;
    bool SendStatus = false;
    bool ReceiveStatus = false;
    uint8_t Address = 0; // Default 2 ??
    uint8_t ReceiveData[8];

public:
    bool ReadVersion();
    bool ReadState();
    bool ReadDSPValue(DSP_VALUE_TYPE type, DSP_GLOBAL global);
    bool ReadSystemSerialNumber();
    bool ReadSystemPN();    
    bool ReadCumulatedEnergy(CUMULATED_ENERGY_TYPE par);
    bool WriteBaudRateSetting(uint8_t baudcode);
    bool ReadFlagsSwitchCentral();
    bool ReadCumulatedEnergyCentral(uint8_t var, uint8_t ndays_h, uint8_t ndays_l, uint8_t global);
    bool ReadFirmwareReleaseCentral(uint8_t var);
    bool ReadBaudRateSettingCentral(uint8_t baudcode, uint8_t serialline);
    bool ReadSystemInfoCentral(uint8_t var);
    bool ReadJunctionBoxMonitoringCentral(uint8_t cf, uint8_t rn, uint8_t njt, uint8_t jal, uint8_t jah);
    bool ReadSystemPNCentral();
    bool ReadSystemSerialNumberCentral();
    void clearData(uint8_t *data, uint8_t len);
    void clearReceiveData();
    bool Send(uint8_t address, uint8_t param0, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4, uint8_t param5, uint8_t param6);
    bool ReadLastFourAlarms();
    bool ReadJunctionBoxState(uint8_t nj);
    bool ReadJunctionBoxVal(uint8_t nj, uint8_t par);

   union {
        uint8_t asBytes[4];
        float asFloat;
    } foo;

    union {
        uint8_t asBytes[4];
        unsigned long asUlong;
    } ulo;


    typedef struct
    {
        uint8_t TransmissionState;
        uint8_t GlobalState;
        uint8_t InverterState;
        uint8_t Channel1State;
        uint8_t Channel2State;
        uint8_t AlarmState;
        bool ReadState;
    } DataState;

    DataState State;

    typedef struct
    {
        uint8_t TransmissionState;
        uint8_t GlobalState;
        std::string Par1;
        std::string Par2;
        std::string Par3;
        std::string Par4;
        bool ReadState;
    } DataVersion;

    DataVersion Version;

    typedef struct
    {
        uint8_t TransmissionState;
        uint8_t GlobalState;
        float Value;
        bool ReadState;
    } DataDSP;

    DataDSP DSP;

    typedef struct
    {
        uint8_t TransmissionState;
        uint8_t GlobalState;
        unsigned long Seconds;
        bool ReadState;
    } DataTimeDate;

    DataTimeDate TimeDate;

    bool ReadTimeDate();

    typedef struct
    {
        uint8_t TransmissionState;
        uint8_t GlobalState;
        uint8_t Alarms1;
        uint8_t Alarms2;
        uint8_t Alarms3;
        uint8_t Alarms4;
        bool ReadState;
    } DataLastFourAlarms;

    DataLastFourAlarms LastFourAlarms;

    // Inverters
    typedef struct
    {
        std::string PN;
        bool ReadState;
    } DataSystemPN;

    DataSystemPN SystemPN;

    typedef struct
    {
        std::string SerialNumber;
        bool ReadState;
    } DataSystemSerialNumber;

    DataSystemSerialNumber SystemSerialNumber;

    typedef struct
    {
        uint8_t TransmissionState;
        uint8_t GlobalState;
        std::string Week;
        std::string Year;
        bool ReadState;
    } DataManufacturingWeekYear;

    DataManufacturingWeekYear ManufacturingWeekYear;

    bool ReadManufacturingWeekYear();

    typedef struct
    {
        uint8_t TransmissionState;
        uint8_t GlobalState;
        std::string Release;
        bool ReadState;
    } DataFirmwareRelease;

    DataFirmwareRelease FirmwareRelease;

    bool ReadFirmwareRelease();

    typedef struct
    {
        uint8_t TransmissionState;
        uint8_t GlobalState;
        unsigned long Energy;
        bool ReadState;
    } DataCumulatedEnergy;

    DataCumulatedEnergy CumulatedEnergy;

protected:
    text_sensor::TextSensor *connection_status{nullptr};
    text_sensor::TextSensor *version{nullptr};
    text_sensor::TextSensor *identification{nullptr};
    sensor::Sensor *cumulated_energy_total{nullptr};   
    sensor::Sensor *v_in_1{nullptr}; 
    sensor::Sensor *v_in_2{nullptr}; 
    sensor::Sensor *i_in_1{nullptr}; 
    sensor::Sensor *i_in_2{nullptr}; 
    sensor::Sensor *power_in_1{nullptr}; 
    sensor::Sensor *power_in_2{nullptr}; 
    sensor::Sensor *power_in_total{nullptr}; 
    sensor::Sensor *grid_power{nullptr}; 
    sensor::Sensor *temperature_inverter{nullptr}; 
    sensor::Sensor *temperature_booster{nullptr}; 
    sensor::Sensor *grid_voltage{nullptr}; 
    sensor::Sensor *cumulated_energy_today{nullptr};  
};

} // abbaurora namespace
} // esphome namespace