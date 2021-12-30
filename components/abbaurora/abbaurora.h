#pragma once

#include "esphome/core/application.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

// Enums
enum DspValueType
{
    GRID_VOLTAGE = 1, GRID_CURRENT = 2, GRID_POWER = 3, FREQUENCY = 4, V_BULK = 5, I_LEAK_DC_DC = 6, I_LEAK_INVERTER = 7,
    POWER_IN_1 = 8, POWER_IN_2 = 9, TEMPERATURE_INVERTER = 21, TEMPERATURE_BOOSTER = 22, V_IN_1 = 23, I_IN_1 = 25, 
    V_IN_2 = 26, I_IN_2 = 27, DC_DC_GRID_VOLTAGE = 28, DC_DC_GRID_FREQUENCY = 29, ISOLATION_RESISTANCE = 30,
    DC_DC_V_BULK = 31, AVERAGE_GRID_VOLTAGE = 32, V_BULK_MID = 33, POWER_PEAK = 34, POWER_PEAK_TODAY = 35,
    GRID_VOLTAGE_NEUTRAL = 36, WIND_GENERATOR_FREQENCY = 37, GRID_VOLTAGE_NEUTRAL_PHASE = 38,
    GRID_CURRENT_PHASE_R = 39, GRID_CURRENT_PHASE_S = 40, GRID_CURRENT_PHASE_T = 41,
    FREQUENCY_PHASE_R = 42, FREQUENCY_PHASE_S = 43, FREQUENCY_PHASE_T = 44,
    V_BULK_POSITIVE = 45, V_BULK_NEGATIVE = 46, TEMPERATURE_SUPERVISOR = 47, TEMPERATURE_ALIM = 48, 
    TEMPERATURE_HEAT_SINK = 49, TEMPERATURE_1 = 50, TEMPERATURE_2 = 51, TEMPERATURE_3 = 52,
    FAN_SPEED_1 = 53, FAN_SPEED_2 = 54, FAN_SPEED_3 = 55, FAN_SPEED_4 = 56, FAN_SPEED_5 = 57,
    POWER_SATURATION_LIMIT = 58, REFERENCE_RING_BULK = 59, V_PANEL_MICRO = 60,
    GRID_VOLTAGE_PHASE_R = 61, GRID_VOLTAGE_PHASE_S = 62, GRID_VOLTAGE_PHASE_T = 63
};

enum DspGlobal
{
    GLOBAL_MEASUREMENT = 1,
    MODULE_MEASUREMENT = 0,
};

enum CumulatedEnergyType
{
    CURRENT_DAY = 0,
    CURRENT_WEEK =0,
    CURRENT_MONTH = 3,
    CURRENT_YEAR = 4,
    TOTAL = 5,
    SINCE_RESET = 6
};

namespace esphome {
namespace abbaurora {

class ABBAuroraComponent : public uart:: UARTDevice, public Component
{
public:
    void setup() override;
    void loop() override;
    void dump_config() override;
    void set_address(uint8_t address) {  this->address_ = address; }
    void set_flow_control_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
    void set_cumulated_energy_total( sensor::Sensor *sensor ) { this->cumulated_energy_total = sensor; }
    void set_v_in_1_sensor( sensor::Sensor *sensor ) { this->v_in_1 = sensor; }
    void set_v_in_2_sensor( sensor::Sensor *sensor ) { this->v_in_2 = sensor; }
    void set_i_in_1_sensor( sensor::Sensor *sensor ) { this->i_in_1 = sensor; }
    void set_i_in_2_sensor( sensor::Sensor *sensor ) { this->i_in_2 = sensor; }
    void set_power_in_1_sensor( sensor::Sensor *sensor ) { this->power_in_1 = sensor; }
    void set_power_in_2_sensor( sensor::Sensor *sensor ) { this->power_in_2 = sensor; }
    void set_power_in_total_sensor( sensor::Sensor *sensor ) { this->power_in_total = sensor; }
    void set_grid_power_sensor( sensor::Sensor *sensor ) { this->grid_power = sensor; }
    void set_temperature_inverter_sensor( sensor::Sensor *sensor ) { this->temperature_inverter = sensor; }
    void set_temperature_booster_sensor( sensor::Sensor *sensor ) { this->temperature_booster = sensor; }
    void set_grid_voltage_sensor( sensor::Sensor *sensor ) { this->grid_voltage = sensor; }
    void set_cumulated_energy_today_sensor( sensor::Sensor *sensor ) { this->cumulated_energy_today = sensor; }
    void set_version_text_sensor( text_sensor::TextSensor *sensor ) { this->version = sensor; }
    void set_connection_status_text_sensor( text_sensor::TextSensor *sensor ) { this->connection_status = sensor; }
    void set_identification_text_sensor( text_sensor::TextSensor *sensor ) { this->identification = sensor; }

protected:
    GPIOPin *flow_control_pin_{nullptr};
    uint8_t address_ = 0; // Default 2 ??
    uint8_t ReceiveData[8];

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
    
    bool send(uint8_t address, uint8_t param0, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4, uint8_t param5, uint8_t param6);
    bool read_version();
    bool read_state();
    bool read_dsp_value(DspValueType type, DspGlobal global);
    bool read_system_serialnumber();
    bool read_system_partnumber();    
    bool read_cumulated_energy(CumulatedEnergyType par);
    bool write_baudrate_setting(uint8_t baudcode);
    bool read_flags_switch_central();
    bool read_cumulated_energy_central(uint8_t var, uint8_t ndays_h, uint8_t ndays_l, uint8_t global);
    bool read_firmware_release_central(uint8_t var);
    bool read_baudrate_setting_central(uint8_t baudcode, uint8_t serialline);
    bool read_system_info_central(uint8_t var);
    bool read_junctionbox_monitoring_central(uint8_t cf, uint8_t rn, uint8_t njt, uint8_t jal, uint8_t jah);
    bool read_system_partnumber_central();
    bool read_system_serialnumber_central();
    bool read_last_four_alarms();
    bool read_junctionbox_state(uint8_t nj);
    bool read_junctionbox_value(uint8_t nj, uint8_t par);

    static std::string transmission_state_text(uint8_t id);
    static std::string global_state_text(uint8_t id);
    static std::string dcdc_state_text(uint8_t id);
    static std::string inverter_state_text(uint8_t id);
    static std::string alarm_state_text(uint8_t id);

    union {
        uint8_t asBytes[4];
        float asFloat;
    } FloatBytes;

    union {
        uint8_t asBytes[4];
        unsigned long asUlong;
    } LongBytes;


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
        std::string PartNumber;
        bool ReadState;
    } DataSystemPN;

    DataSystemPN SystemPartNumber;

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
};

} // abbaurora namespace
} // esphome namespace