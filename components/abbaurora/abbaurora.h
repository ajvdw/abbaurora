#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/application.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

// Enums
enum DspValueType {
  GRID_VOLTAGE = 1,
  GRID_CURRENT = 2,
  GRID_POWER = 3,
  FREQUENCY = 4,
  V_BULK = 5,
  I_LEAK_DC_DC = 6,
  I_LEAK_INVERTER = 7,
  POWER_IN_1 = 8,
  POWER_IN_2 = 9,
  TEMPERATURE_INVERTER = 21,
  TEMPERATURE_BOOSTER = 22,
  V_IN_1 = 23,
  I_IN_1 = 25,
  V_IN_2 = 26,
  I_IN_2 = 27,
  DC_DC_GRID_VOLTAGE = 28,
  DC_DC_GRID_FREQUENCY = 29,
  ISOLATION_RESISTANCE = 30,
  DC_DC_V_BULK = 31,
  AVERAGE_GRID_VOLTAGE = 32,
  V_BULK_MID = 33,
  POWER_PEAK = 34,
  POWER_PEAK_TODAY = 35,
  GRID_VOLTAGE_NEUTRAL = 36,
  WIND_GENERATOR_FREQENCY = 37,
  GRID_VOLTAGE_NEUTRAL_PHASE = 38,
  GRID_CURRENT_PHASE_R = 39,
  GRID_CURRENT_PHASE_S = 40,
  GRID_CURRENT_PHASE_T = 41,
  FREQUENCY_PHASE_R = 42,
  FREQUENCY_PHASE_S = 43,
  FREQUENCY_PHASE_T = 44,
  V_BULK_POSITIVE = 45,
  V_BULK_NEGATIVE = 46,
  TEMPERATURE_SUPERVISOR = 47,
  TEMPERATURE_ALIM = 48,
  TEMPERATURE_HEAT_SINK = 49,
  TEMPERATURE_1 = 50,
  TEMPERATURE_2 = 51,
  TEMPERATURE_3 = 52,
  FAN_SPEED_1 = 53,
  FAN_SPEED_2 = 54,
  FAN_SPEED_3 = 55,
  FAN_SPEED_4 = 56,
  FAN_SPEED_5 = 57,
  POWER_SATURATION_LIMIT = 58,
  REFERENCE_RING_BULK = 59,
  V_PANEL_MICRO = 60,
  GRID_VOLTAGE_PHASE_R = 61,
  GRID_VOLTAGE_PHASE_S = 62,
  GRID_VOLTAGE_PHASE_T = 63
};

enum DspGlobal {
  GLOBAL_MEASUREMENT = 1,
  MODULE_MEASUREMENT = 0,
};

enum CumulatedEnergyType {
  CURRENT_DAY = 0,
  CURRENT_WEEK = 0,
  CURRENT_MONTH = 3,
  CURRENT_YEAR = 4,
  TOTAL = 5,
  SINCE_RESET = 6
};

namespace esphome {
namespace abbaurora {

const char* UNKNOWN_TEXT PROGMEM = "Unknown";

const char* INVERTER_VERSION_LOOKUP[] PROGMEM = {
  "Aurora 2 kW indoor",
  "Aurora 2 kW outdoor",
  "ABB TRIO 5.8kW outdoor",
  "Aurora 3.6 kW indoor",
  "Aurora 3.0 - 3.6 kW outdoor",
  "Aurora 5.0 kW outdoor",
  "Aurora 6 kW outdoor",
  "3 - phase interface (3G74)",
  "Aurora 50kW module",
  "Aurora 4.2kW new",
  "Aurora 3.6kW new",
  "Aurora 3.3kW new",
  "Aurora 3.0kW new",
  "Aurora 12.0kW",
  "Aurora 10.0kW"
};

const char* GRID_TYPE_LOOKUP[] PROGMEM = {
  "UL1741",       
  "NETHERL", 
  "VDE0126", 
  "DR 1663 / 2000",
  "ENEL DK 5950", 
  "UK G83",  
  "AS 4777"
};

const char* TRANSFORMER_TYPE_LOOKUP[] PROGMEM = {
  "Transformerless Version",
  "Transformer Version"
};

const char* GENERATION_TYPE_LOOKUP[] PROGMEM = {
  "Wind Version", 
  "PV Version"
};

const char* ALARM_TEXT_LOOKUP[] PROGMEM = {
  "No Alarm",
  "Sun Low",
  "Input OC",
  "Input UV",
  "Input OV",
  "Sun Low",
  "No Parameters",
  "Bulk OV",
  "Comm.Error",
  "Output OC",
  "IGBT Sat",
  "Bulk UV",
  "Internal error",
  "Grid Fail",
  "Bulk Low",
  "Ramp Fail",
  "Dc / Dc Fail",
  "Wrong Mode",
  "Ground Fault",
  "Over Temp.",
  "Bulk Cap Fail",
  "Inverter Fail",
  "Start Timeout",
  "Ground Fault",
  "Degauss error",
  "Ileak sens.fail",
  "DcDc Fail",
  "Self Test Error 1",
  "Self Test Error 2",
  "Self Test Error 3",
  "Self Test Error 4",
  "DC inj error",
  "Grid OV",
  "Grid UV",
  "Grid OF",
  "Grid UF",
  "Z grid Hi",
  "Internal error",
  "Riso Low",
  "Vref Error",
  "Error Meas V",
  "Error Meas F",
  "Error Meas Z",
  "Error Meas Ileak",
  "Error Read V",
  "Error Read I",
  "Table fail",
  "Fan Fail",
  "UTH",
  "Interlock fail",
  "Remote Off",
  "Vout Avg errror",
  "Battery low",
  "Clk fail",
  "Input UC",
  "Zero Power",
  "Fan Stucked",
  "DC Switch Open",
  "Tras Switch Open",
  "AC Switch Open",
  "Bulk UV",
  "Autoexclusion",
  "Grid df / dt",
  "Den switch Open",
  "Jbox fail"
};

const char* TRANSMISSION_STATE_LOOKUP[] PROGMEM = {
    "Everything is OK.",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "Command is not implemented",
    "Variable does not exist",
    "Variable value is out of range",
    "EEprom not accessible",
    "Not Toggled Service Mode",
    "Can not send the command to internal micro",
    "Command not Executed",
    "The variable is not available, retry"
  };

const char* INVERTER_STATE_LOOKUP[] PROGMEM = {
  "Stand By",
  "Checking Grid",
  "Run",
  "Bulk OV",
  "Out OC",
  "IGBT Sat",
  "Bulk UV",
  "Degauss Error",
  "No Parameters",
  "Bulk Low",
  "Grid OV",
  "Communication Error",
  "Degaussing",
  "Starting",
  "Bulk Cap Fail",
  "Leak Fail",
  "DcDc Fail",
  "Ileak Sensor Fail",
  "SelfTest: relay inverter",
  "SelfTest : wait for sensor test",
  "SelfTest : test relay DcDc + sensor",
  "SelfTest : relay inverter fail",
  "SelfTest timeout fail",
  "SelfTest : relay DcDc fail",
  "Self Test 1",
  "Waiting self test start",
  "Dc Injection",
  "Self Test 2",
  "Self Test 3",
  "Self Test 4",
  "Internal Error",
  "Internal Error",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  "Forbidden State",
  "Input UC",
  "Zero Power",
  "Grid Not Present",
  "Waiting Start",
  "MPPT",
  "Grid Fail",
  "Input OC"
};

const char* DCDC_STATE_LOOKUP[] PROGMEM = {
  "DcDc OFF",
  "Ramp Start",
  "MPPT",
  "Not Used",
  "Input OC",
  "Input UV",
  "Input OV",
  "Input Low",
  "No Parameters",
  "Bulk OV",
  "Communication Error",
  "Ramp Fail",
  "Internal Error",
  "Input mode Error",
  "Ground Fault",
  "Inverter Fail",
  "DcDc IGBT Sat",
  "DcDc ILEAK Fail",
  "DcDc Grid Fail",
  "DcDc Comm.Error"
};

const char* GLOBAL_STATE_LOOKUP[] PROGMEM = {
  "Sending Parameters", //0
  "Wait Sun / Grid",
  "Checking Grid",
  "Measuring Riso",
  "DcDc Start",
  "Inverter Start",
  "Run",
  "Recovery",
  "Pausev",
  "Ground Fault",
  "OTH Fault",  // 10
  "Address Setting",
  "Self Test",
  "Self Test Fail",
  "Sensor Test + Meas.Riso",
  "Leak Fault",
  "Waiting for manual reset",
  "Internal Error E026",
  "Internal Error E027",
  "Internal Error E028",
  "Internal Error E029", // 20
  "Internal Error E030",
  "Sending Wind Table",
  "Failed Sending table",
  "UTH Fault",
  "Remote OFF",
  "Interlock Fail",
  "Executing Autotest",
  "Waiting Sun",
  "Temperature Fault",
  "Fan Staucked", // 30
  "Int.Com.Fault",
  "SLV Insertion",
  "DC Switch Open",
  "TRAS Switch Open",
  "MST Exclusion",
  "Auto Exclusion",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  "Erasing Internal EEprom",
  "Erasing External EEprom",
  "Counting EEprom",
  "Freeze"
};

class ABBAuroraComponent : public uart::UARTDevice, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  void set_address(uint8_t address) { this->address_ = address; }
  void set_flow_control_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
  void set_v_in_1_sensor(sensor::Sensor *sensor) { this->v_in_1_ = sensor; }
  void set_v_in_2_sensor(sensor::Sensor *sensor) { this->v_in_2_ = sensor; }
  void set_i_in_1_sensor(sensor::Sensor *sensor) { this->i_in_1_ = sensor; }
  void set_i_in_2_sensor(sensor::Sensor *sensor) { this->i_in_2_ = sensor; }
  void set_power_in_1_sensor(sensor::Sensor *sensor) { this->power_in_1_ = sensor; }
  void set_power_in_2_sensor(sensor::Sensor *sensor) { this->power_in_2_ = sensor; }
  void set_power_in_total_sensor(sensor::Sensor *sensor) { this->power_in_total_ = sensor; }
  void set_grid_power_sensor(sensor::Sensor *sensor) { this->grid_power_ = sensor; }
  void set_temperature_inverter_sensor(sensor::Sensor *sensor) { this->temperature_inverter_ = sensor; }
  void set_temperature_booster_sensor(sensor::Sensor *sensor) { this->temperature_booster_ = sensor; }
  void set_grid_voltage_sensor(sensor::Sensor *sensor) { this->grid_voltage_ = sensor; }
  void set_cumulated_energy_today_sensor(sensor::Sensor *sensor) { this->cumulated_energy_today_ = sensor; }
  void set_cumulated_energy_total_sensor(sensor::Sensor *sensor) { this->cumulated_energy_total_ = sensor; }
  void set_inverter_version_text_sensor(text_sensor::TextSensor *sensor) { this->inverter_version_ = sensor; }
  void set_connection_status_text_sensor(text_sensor::TextSensor *sensor) { this->connection_status_ = sensor; }
  void set_identification_text_sensor(text_sensor::TextSensor *sensor) { this->identification_ = sensor; }

 protected:
  GPIOPin *flow_control_pin_{nullptr};
  uint8_t address_ = 0;
  uint8_t receive_data_[8];

  text_sensor::TextSensor *connection_status_{nullptr};
  text_sensor::TextSensor *inverter_version_{nullptr};
  text_sensor::TextSensor *identification_{nullptr};
  sensor::Sensor *cumulated_energy_total_{nullptr};
  sensor::Sensor *v_in_1_{nullptr};
  sensor::Sensor *v_in_2_{nullptr};
  sensor::Sensor *i_in_1_{nullptr};
  sensor::Sensor *i_in_2_{nullptr};
  sensor::Sensor *power_in_1_{nullptr};
  sensor::Sensor *power_in_2_{nullptr};
  sensor::Sensor *power_in_total_{nullptr};
  sensor::Sensor *grid_power_{nullptr};
  sensor::Sensor *temperature_inverter_{nullptr};
  sensor::Sensor *temperature_booster_{nullptr};
  sensor::Sensor *grid_voltage_{nullptr};
  sensor::Sensor *cumulated_energy_today_{nullptr};

  bool read_baudrate_setting_central_(uint8_t baudcode, uint8_t serialline);
  bool read_cumulated_energy_(CumulatedEnergyType par);
  bool read_cumulated_energy_central_(uint8_t var, uint8_t ndays_h, uint8_t ndays_l, uint8_t global);
  bool read_dsp_value_(DspValueType type, DspGlobal global);
  bool read_firmware_release_();
  bool read_firmware_release_central_(uint8_t var);
  bool read_flags_switch_central_();
  bool read_junctionbox_monitoring_central_(uint8_t cf, uint8_t rn, uint8_t njt, uint8_t jal, uint8_t jah);
  bool read_junctionbox_state_(uint8_t nj);
  bool read_junctionbox_value_(uint8_t nj, uint8_t par);
  bool read_last_four_alarms_();
  bool read_manufacturing_week_year_();
  bool read_state_();
  bool read_system_info_central_(uint8_t var);
  bool read_system_partnumber_();
  bool read_system_partnumber_central_();
  bool read_system_serialnumber_();
  bool read_system_serialnumber_central_();
  bool read_timedate_();
  bool read_version_();
  bool send_(uint8_t address, uint8_t param0, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4,
             uint8_t param5, uint8_t param6);
  bool write_baudrate_setting_(uint8_t baudcode);

  const char *ABBAuroraComponent::inverter_version_text_(uint8_t id) {
    char *id_sequence = (char *) "iorIO56PC4321DX";
    char *occur = strchr(id_sequence, (char)id);

    if (occur) return INVERTER_VERSION_LOOKUP[occur - id_sequence];

    return UNKNOWN_TEXT;
  }

  const char *ABBAuroraComponent::grid_type_text_(uint8_t id) {
    char *id_sequence = (char *) "ABESIUK";
    char *occur = strchr(id_sequence, (char)id);

    if (occur) return GRID_TYPE_LOOKUP[occur - id_sequence];

    return UNKNOWN_TEXT;
  }

  const char *ABBAuroraComponent::transformer_type_text_(uint8_t id) {
    char *id_sequence = (char *) "NT";
    char *occur = strchr(id_sequence, (char)id);

    if (occur) return TRANSFORMER_TYPE_LOOKUP[occur - id_sequence];

    return UNKNOWN_TEXT;
  }

  const char *ABBAuroraComponent::generation_type_text_(uint8_t id) {
    char *id_sequence = (char *) "WN";
    char *occur = strchr(id_sequence, (char)id);

    if (occur) return GENERATION_TYPE_LOOKUP[occur - id_sequence];

    return UNKNOWN_TEXT;
  }

  const char *ABBAuroraComponent::alarm_state_text_(uint8_t id) {
    if (id >= 0 && id <= 64) return ALARM_TEXT_LOOKUP[id];

    return UNKNOWN_TEXT;
  }

  const char *ABBAuroraComponent::transmission_state_text_(uint8_t id) {
    if (id >= 0 && id <= 58) {
      if (TRANSMISSION_STATE_LOOKUP[id]) return TRANSMISSION_STATE_LOOKUP[id];
    }
    return UNKNOWN_TEXT;
  }

  const char *ABBAuroraComponent::global_state_text_(uint8_t id) {
    if (id >= 0 && id <= 101) {
      if (GLOBAL_STATE_LOOKUP[id]) return GLOBAL_STATE_LOOKUP[id];
    }
    return UNKNOWN_TEXT;
  }

  const char *ABBAuroraComponent::dcdc_state_text_(uint8_t id) {
    if (id >= 0 and id <= 19) {
      if (DCDC_STATE_LOOKUP[id]) return DCDC_STATE_LOOKUP[id];
    }
    return UNKNOWN_TEXT;
  }

  const char *ABBAuroraComponent::inverter_state_text_(uint8_t id) {
    if (id >= 0 && id <= 47) {
      if (INVERTER_STATE_LOOKUP[id]) return INVERTER_STATE_LOOKUP[id];
    }
    return UNKNOWN_TEXT;
  }

  union {
    uint8_t asBytes[4];
    float asFloat;
  } float_bytes_;

  union {
    uint8_t asBytes[4];
    uint32_t asUlong;
  } long_bytes_;

  using DataState = struct {
    uint8_t TransmissionState;
    uint8_t GlobalState;
    uint8_t InverterState;
    uint8_t Channel1State;
    uint8_t Channel2State;
    uint8_t AlarmState;
    bool ReadState;
  };
  DataState state_;

  using DataVersion = struct {
    uint8_t TransmissionState;
    uint8_t GlobalState;
    char *Par1;
    char *Par2;
    char *Par3;
    char *Par4;
    bool ReadState;
  };
  DataVersion data_version_;

  using DataDSP = struct {
    uint8_t TransmissionState;
    uint8_t GlobalState;
    float Value;
    bool ReadState;
  };
  DataDSP dsp_;

  using DataTimeDate = struct {
    uint8_t TransmissionState;
    uint8_t GlobalState;
    uint32_t Seconds;
    bool ReadState;
  };
  DataTimeDate timedate_;

  using DataLastFourAlarms = struct {
    uint8_t TransmissionState;
    uint8_t GlobalState;
    uint8_t Alarms1;
    uint8_t Alarms2;
    uint8_t Alarms3;
    uint8_t Alarms4;
    bool ReadState;
  };
  DataLastFourAlarms last_four_alarms_;

  // Inverters
  using DataSystemPartNumber = struct {
    char PartNumber[7];
    bool ReadState;
  };
  DataSystemPartNumber system_partnumber_;

  using DataSystemSerialNumber = struct {
    char SerialNumber[7];
    bool ReadState;
  };
  DataSystemSerialNumber system_serialnumber_;

  using DataManufacturingWeekYear = struct {
    uint8_t TransmissionState;
    uint8_t GlobalState;
    char Week[3];
    char Year[3];
    bool ReadState;
  };
  DataManufacturingWeekYear manufacturing_week_year_;

  using DataFirmwareRelease = struct {
    uint8_t TransmissionState;
    uint8_t GlobalState;
    char Release[8];
    bool ReadState;
  };
  DataFirmwareRelease firmware_release_;

  using DataCumulatedEnergy = struct {
    uint8_t TransmissionState;
    uint8_t GlobalState;
    uint32_t Energy;
    bool ReadState;
  };
  DataCumulatedEnergy cumulated_energy_;
};

}  // namespace abbaurora
}  // namespace esphome