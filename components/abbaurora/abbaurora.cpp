#include "abbaurora.h"

namespace esphome {
namespace abbaurora {

static const char *const TAG = "abbaurora";

void ABBAuroraComponent::setup() {
  ESP_LOGD(TAG, "Setup");

  // Set flowcontrolpin
  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->setup();

  memset((uint8_t *) receive_data_, 0, 8);

  connection_status_->publish_state("Setup");
}

void ABBAuroraComponent::dump_config() {
  if (this->flow_control_pin_ != nullptr) {
    LOG_PIN("  Flow control Pin: ", this->flow_control_pin_);
  }
  ESP_LOGCONFIG(TAG, "  Inverter Address: %d", this->address_);
}

void ABBAuroraComponent::loop() {
  const uint32_t now = millis();
  static uint32_t last_request = 0;
  static uint32_t rotaterequests = 0;

  if (now - last_request > 1000) {
    last_request = now;
    rotaterequests++;
    switch (rotaterequests % 30) {
      case 2:
        if (connection_status_ && this->read_state_())  // If inverter is connected
          connection_status_->publish_state(ABBAuroraComponent::inverter_state_text_(state_.InverterState));
        break;
      case 4:
        if (identification_ && this->read_system_serialnumber_())
          identification_->publish_state(this->system_serialnumber_.SerialNumber);
        break;
      case 6:
        if (inverter_version_ && this->read_version_())
          inverter_version_->publish_state(this->data_version_.Par1);
        break;
      case 8:
        if (temperature_inverter_ && this->read_dsp_value_(TEMPERATURE_INVERTER, MODULE_MEASUREMENT))
          temperature_inverter_->publish_state(this->dsp_.Value);
        break;
      case 10:
        if (power_in_1_ && this->read_dsp_value_(POWER_IN_1, MODULE_MEASUREMENT))
          power_in_1_->publish_state(this->dsp_.Value);
        if (power_in_total_ && power_in_1_ && power_in_2_)
          power_in_total_->publish_state(power_in_1_->get_state() + power_in_2_->get_state());
        break;
      case 12:
        if (power_in_2_ && this->read_dsp_value_(POWER_IN_2, MODULE_MEASUREMENT))
          power_in_2_->publish_state(this->dsp_.Value);
        if (power_in_total_ && power_in_1_ && power_in_2_)
          power_in_total_->publish_state(power_in_1_->get_state() + power_in_2_->get_state());
        break;
      case 14:
        if (v_in_1_ && this->read_dsp_value_(V_IN_1, MODULE_MEASUREMENT))
          v_in_1_->publish_state(this->dsp_.Value);
        break;
      case 16:
        if (v_in_2_ && this->read_dsp_value_(V_IN_2, MODULE_MEASUREMENT))
          v_in_2_->publish_state(this->dsp_.Value);
        break;
      case 18:
        if (i_in_1_ && this->read_dsp_value_(I_IN_1, MODULE_MEASUREMENT))
          i_in_1_->publish_state(this->dsp_.Value);
        break;
      case 20:
        if (i_in_2_ && this->read_dsp_value_(I_IN_2, MODULE_MEASUREMENT))
          i_in_2_->publish_state(this->dsp_.Value);
        break;
      case 22:
        if (temperature_booster_ && this->read_dsp_value_(TEMPERATURE_BOOSTER, MODULE_MEASUREMENT))
          temperature_booster_->publish_state(this->dsp_.Value);
        break;
      case 1:
      case 11:
      case 21:
        if (grid_power_ && this->read_dsp_value_(GRID_POWER, MODULE_MEASUREMENT))
          grid_power_->publish_state(this->dsp_.Value);
        break;
      case 3:
      case 13:
      case 23:
        if (grid_voltage_ && this->read_dsp_value_(GRID_VOLTAGE, MODULE_MEASUREMENT))
          grid_voltage_->publish_state(this->dsp_.Value);
        break;
      case 5:
      case 15:
      case 25:
        if (cumulated_energy_total_ && this->read_cumulated_energy_(TOTAL))
          cumulated_energy_total_->publish_state(this->cumulated_energy_.Energy);
        break;
      case 7:
      case 17:
      case 27:
        if (cumulated_energy_today_ && this->read_cumulated_energy_(CURRENT_DAY))
          cumulated_energy_today_->publish_state(this->cumulated_energy_.Energy);
        break;
      case 28:
        ESP_LOGV(TAG, "Memory FreeHeap: %d", ESP.getFreeHeap() );
        break;
    }
  }
}

bool ABBAuroraComponent::send_(uint8_t address, uint8_t param0, uint8_t param1, uint8_t param2, uint8_t param3,
                               uint8_t param4, uint8_t param5, uint8_t param6) {
  uint8_t send_data[10];
  send_data[0] = address;
  send_data[1] = param0;
  send_data[2] = param1;
  send_data[3] = param2;
  send_data[4] = param3;
  send_data[5] = param4;
  send_data[6] = param5;
  send_data[7] = param6;

  // Calculate CRC16
  uint8_t bcc_lo = 0xFF;
  uint8_t bcc_hi = 0xFF;
  for (int i = 0; i < 8; i++) {
    uint8_t new_b = send_data[i] ^ bcc_lo;
    uint8_t tmp_b = new_b << 4;
    new_b = tmp_b ^ new_b;
    tmp_b = new_b >> 5;
    bcc_lo = bcc_hi;
    bcc_hi = new_b ^ tmp_b;
    tmp_b = new_b << 3;
    bcc_lo = bcc_lo ^ tmp_b;
    tmp_b = new_b >> 4;
    bcc_lo = bcc_lo ^ tmp_b;
  }
  // CRC bytes
  send_data[8] = (uint8_t)(~bcc_lo);
  send_data[9] = (uint8_t)(~bcc_hi);

  // Clear data
  memset((uint8_t *) receive_data_, 0, 8);
  // Empty RX buffer
  while (this->available()) {
    uint8_t purge;
    this->read_byte(&purge);
  }

  // Flow control to TX
  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(true);

  // Send data
  this->write_array((uint8_t *) send_data, 10);
  // Wait until complete
  this->flush();

  // Flow control to RX
  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(false);

  // Wait for 100ms for data to arrive
  const uint32_t now = millis();
  bool datawaiting = false;
  while (millis() - now < 100 && !datawaiting)
    datawaiting = this->available();

  ESP_LOGV(TAG, "Waited for %d ms for data to arrive", millis() - now);

  // Data waiting?
  if (datawaiting) {
    // Receive data
    if (this->read_array((uint8_t *) receive_data_, 8)) {
      // Calc CRC16
      bcc_lo = 0xFF;
      bcc_hi = 0xFF;
      for (int i = 0; i < 6; i++) {
        uint8_t new_b = receive_data_[i] ^ bcc_lo;
        uint8_t tmp_b = new_b << 4;
        new_b = tmp_b ^ new_b;
        tmp_b = new_b >> 5;
        bcc_lo = bcc_hi;
        bcc_hi = new_b ^ tmp_b;
        tmp_b = new_b << 3;
        bcc_lo = bcc_lo ^ tmp_b;
        tmp_b = new_b >> 4;
        bcc_lo = bcc_lo ^ tmp_b;
      }
      // Check CRC16
      if (receive_data_[7] == (uint8_t)(~bcc_hi) && receive_data_[6] == (uint8_t)(~bcc_lo))
        return true;  // Checksum OK
      else
        ESP_LOGD(TAG, "CRC error in received data");
    } else
      ESP_LOGD(TAG, "Failed receiving data");
  } else
    ESP_LOGD(TAG, "No data available");

  return false;
}

/**
 * Reads a single value of the digital signal procesor.
 * Not all values are supported by all models.
 * Read values are in following Units:
 * Voltage V
 * Current A
 * Power W
 * Temperature °C
 *
 **/
bool ABBAuroraComponent::read_dsp_value_(DspValueType type, DspGlobal global) {
  if ((((int) type >= 1 && (int) type <= 9) || ((int) type >= 21 && (int) type <= 63)) &&
      ((int) global >= 0 && (int) global <= 1)) {
    dsp_.ReadState =
        send_(this->address_, (uint8_t) 59, type, global, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);

    if (!dsp_.ReadState) {
      receive_data_[0] = 255;
      receive_data_[1] = 255;
    }
  } else {
    dsp_.ReadState = false;
    receive_data_[0] = 255;
    receive_data_[1] = 255;
    receive_data_[2] = 0;
    receive_data_[3] = 0;
    receive_data_[4] = 0;
    receive_data_[5] = 0;
    receive_data_[6] = 0;
    receive_data_[7] = 0;
  }

  dsp_.TransmissionState = receive_data_[0];
  dsp_.GlobalState = receive_data_[1];

  float_bytes_.asBytes[0] = receive_data_[5];
  float_bytes_.asBytes[1] = receive_data_[4];
  float_bytes_.asBytes[2] = receive_data_[3];
  float_bytes_.asBytes[3] = receive_data_[2];

  dsp_.Value = float_bytes_.asFloat;
  if (dsp_.Value > 1E10 || dsp_.Value < 0)  // this does not make sense
    dsp_.Value = 0;

  return dsp_.ReadState;
}

bool ABBAuroraComponent::read_timedate_() {
  timedate_.ReadState =
      send_(this->address_, (uint8_t) 70, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);

  if (!timedate_.ReadState) {
    receive_data_[0] = 255;
    receive_data_[1] = 255;
  }

  timedate_.TransmissionState = receive_data_[0];
  timedate_.GlobalState = receive_data_[1];
  timedate_.Seconds = ((uint32_t) receive_data_[2] << 24) + ((uint32_t) receive_data_[3] << 16) +
                      ((uint32_t) receive_data_[4] << 8) + (uint32_t) receive_data_[5];
  return timedate_.ReadState;
}

bool ABBAuroraComponent::read_last_four_alarms_() {
  last_four_alarms_.ReadState =
      send_(this->address_, (uint8_t) 86, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);

  if (!last_four_alarms_.ReadState) {
    receive_data_[0] = 255;
    receive_data_[1] = 255;
    receive_data_[2] = 255;
    receive_data_[3] = 255;
    receive_data_[4] = 255;
    receive_data_[5] = 255;
  }

  last_four_alarms_.TransmissionState = receive_data_[0];
  last_four_alarms_.GlobalState = receive_data_[1];
  last_four_alarms_.Alarms1 = receive_data_[2];
  last_four_alarms_.Alarms2 = receive_data_[3];
  last_four_alarms_.Alarms3 = receive_data_[4];
  last_four_alarms_.Alarms4 = receive_data_[5];

  return last_four_alarms_.ReadState;
}

bool ABBAuroraComponent::read_junctionbox_state_(uint8_t nj) {
  return send_(this->address_, (uint8_t) 200, nj, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);
  /*
  // ToDo not yet implemented
  junctionbox_state_.state_.transmissionState = receive_data_[0];
  junctionbox_state_.state_.globalState = -1;

  junctionbox_state_.jBoxstate_.burnFuseOnJBox      = receive_data_[1] & 0x01;
  junctionbox_state_.jBoxstate_.jBoxNotCalibrated   = receive_data_[1] & 0x80;
  junctionbox_state_.jBoxstate_.jBoxOvercurrent     = receive_data_[1] & 0x10;
  junctionbox_state_.jBoxstate_.jBoxOvertemperature = receive_data_[1] & 0x02;
  junctionbox_state_.jBoxstate_.jBoxOvervoltage     = receive_data_[1] & 0x04;
  junctionbox_state_.jBoxstate_.noCommunication     = receive_data_[1] & 0x40;
  junctionbox_state_.jBoxstate_.powerOff            = receive_data_[1] & 0x20;
  junctionbox_state_.jBoxstate_.unbalancedStringCurrent = receive_data_[1] &
  0x08;

  junctionbox_state_.fuseBurnt[1] = receive_data_[4] & 0x04;
  junctionbox_state_.fuseBurnt[2] = receive_data_[4] & 0x01;
  junctionbox_state_.fuseBurnt[3] = receive_data_[3] & 0x40;
  junctionbox_state_.fuseBurnt[4] = receive_data_[3] & 0x10;
  junctionbox_state_.fuseBurnt[5] = receive_data_[3] & 0x04;
  junctionbox_state_.fuseBurnt[6] = receive_data_[3] & 0x01;
  junctionbox_state_.fuseBurnt[7] = receive_data_[2] & 0x40;
  junctionbox_state_.fuseBurnt[8] = receive_data_[2] & 0x10;
  junctionbox_state_.fuseBurnt[9] = receive_data_[2] & 0x04;
  junctionbox_state_.fuseBurnt[10] = receive_data_[2] & 0x01;
  junctionbox_state_.fuseBurnt[11] = receive_data_[4] & 0x08;
  junctionbox_state_.fuseBurnt[12] = receive_data_[4] & 0x02;
  junctionbox_state_.fuseBurnt[13] = receive_data_[3] & 0x80;
  junctionbox_state_.fuseBurnt[14] = receive_data_[3] & 0x20;
  junctionbox_state_.fuseBurnt[15] = receive_data_[3] & 0x08;
  junctionbox_state_.fuseBurnt[16] = receive_data_[3] & 0x02;
  junctionbox_state_.fuseBurnt[17] = receive_data_[2] & 0x80;
  junctionbox_state_.fuseBurnt[18] = receive_data_[2] & 0x20;
  junctionbox_state_.fuseBurnt[19] = receive_data_[2] & 0x08;
  junctionbox_state_.fuseBurnt[20] = receive_data_[2] & 0x02;
  junctionbox_state_.stringCurrentUnbalanced[0] = receive_data_[5] & 0x80;
  junctionbox_state_.stringCurrentUnbalanced[1] = receive_data_[5] & 0x40;
  junctionbox_state_.stringCurrentUnbalanced[2] = receive_data_[5] & 0x20;
  junctionbox_state_.stringCurrentUnbalanced[3] = receive_data_[5] & 0x10;
  junctionbox_state_.stringCurrentUnbalanced[4] = receive_data_[5] & 0x08;
  junctionbox_state_.stringCurrentUnbalanced[5] = receive_data_[5] & 0x04;
  junctionbox_state_.stringCurrentUnbalanced[6] = receive_data_[5] & 0x02;
  junctionbox_state_.stringCurrentUnbalanced[7] = receive_data_[5] & 0x01;
  junctionbox_state_.stringCurrentUnbalanced[8] = receive_data_[4] & 0x80;
  junctionbox_state_.stringCurrentUnbalanced[9] = receive_data_[4] & 0x40;
  */
}

bool ABBAuroraComponent::read_junctionbox_value_(uint8_t nj, uint8_t par) {
  return send_(this->address_, (uint8_t) 201, nj, par, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);
}

// Inverters
bool ABBAuroraComponent::read_system_partnumber_() {
  system_partnumber_.ReadState =
      send_(this->address_, (uint8_t) 52, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);

  system_partnumber_.PartNumber[0] = receive_data_[0];
  system_partnumber_.PartNumber[1] = receive_data_[1];
  system_partnumber_.PartNumber[2] = receive_data_[2];
  system_partnumber_.PartNumber[3] = receive_data_[3];
  system_partnumber_.PartNumber[4] = receive_data_[4];
  system_partnumber_.PartNumber[5] = receive_data_[5];
  system_partnumber_.PartNumber[6] = '\0';

  return system_partnumber_.ReadState;
}

bool ABBAuroraComponent::read_system_serialnumber_() {
  system_serialnumber_.ReadState =
      send_(this->address_, (uint8_t) 63, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);

  system_serialnumber_.SerialNumber[0] = receive_data_[0];
  system_serialnumber_.SerialNumber[1] = receive_data_[1];
  system_serialnumber_.SerialNumber[2] = receive_data_[2];
  system_serialnumber_.SerialNumber[3] = receive_data_[3];
  system_serialnumber_.SerialNumber[4] = receive_data_[4];
  system_serialnumber_.SerialNumber[5] = receive_data_[5];
  system_serialnumber_.SerialNumber[6] = '\0';

  return system_serialnumber_.ReadState;
}

bool ABBAuroraComponent::read_manufacturing_week_year_() {
  manufacturing_week_year_.ReadState =
      send_(this->address_, (uint8_t) 65, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);

  if (!manufacturing_week_year_.ReadState) {
    receive_data_[0] = 255;
    receive_data_[1] = 255;
  }

  manufacturing_week_year_.TransmissionState = receive_data_[0];
  manufacturing_week_year_.GlobalState = receive_data_[1];

  manufacturing_week_year_.Week[0] = receive_data_[2];
  manufacturing_week_year_.Week[1] = receive_data_[3];
  manufacturing_week_year_.Week[2] = '\0';

  manufacturing_week_year_.Year[0] = receive_data_[4];
  manufacturing_week_year_.Year[1] = receive_data_[5];
  manufacturing_week_year_.Year[2] = '\0';

  return manufacturing_week_year_.ReadState;
}

bool ABBAuroraComponent::read_firmware_release_() {
  firmware_release_.ReadState =
      send_(this->address_, (uint8_t) 72, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);

  if (!firmware_release_.ReadState) {
    receive_data_[0] = 255;
    receive_data_[1] = 255;
  }

  firmware_release_.TransmissionState = receive_data_[0];
  firmware_release_.GlobalState = receive_data_[1];

  firmware_release_.Release[0] = receive_data_[2];
  firmware_release_.Release[1] = '.';
  firmware_release_.Release[2] = receive_data_[3];
  firmware_release_.Release[3] = '.';
  firmware_release_.Release[4] = receive_data_[4];
  firmware_release_.Release[5] = '.';
  firmware_release_.Release[6] = receive_data_[5];
  firmware_release_.Release[7] = '\0'; 

  return firmware_release_.ReadState;
}

bool ABBAuroraComponent::read_cumulated_energy_(CumulatedEnergyType par) {
  if ((int) par >= 0 && (int) par <= 6) {
    cumulated_energy_.ReadState =
        send_(this->address_, (uint8_t) 78, par, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);

    if (!cumulated_energy_.ReadState) {
      receive_data_[0] = 255;
      receive_data_[1] = 255;
    }
  } else {
    cumulated_energy_.ReadState = false;
    receive_data_[0] = 255;
    receive_data_[1] = 255;
    receive_data_[2] = 0;
    receive_data_[3] = 0;
    receive_data_[4] = 0;
    receive_data_[5] = 0;
    receive_data_[6] = 0;
    receive_data_[7] = 0;
  }

  cumulated_energy_.TransmissionState = receive_data_[0];
  cumulated_energy_.GlobalState = receive_data_[1];

  if (cumulated_energy_.ReadState) {
    long_bytes_.asBytes[0] = receive_data_[5];
    long_bytes_.asBytes[1] = receive_data_[4];
    long_bytes_.asBytes[2] = receive_data_[3];
    long_bytes_.asBytes[3] = receive_data_[2];

    cumulated_energy_.Energy = long_bytes_.asUlong;
  }
  return cumulated_energy_.ReadState;
}

bool ABBAuroraComponent::write_baudrate_setting_(uint8_t baudcode) {
  if ((int) baudcode >= 0 && (int) baudcode <= 3) {
    return send_(this->address_, (uint8_t) 85, baudcode, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0,
                 (uint8_t) 0);
  } else {
    memset((uint8_t *) receive_data_, 0, 8);
    return false;
  }
}

// Central
bool ABBAuroraComponent::read_flags_switch_central_() {
  return send_(this->address_, (uint8_t) 67, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0,
               (uint8_t) 0);
}

bool ABBAuroraComponent::read_cumulated_energy_central_(uint8_t var, uint8_t ndays_h, uint8_t ndays_l, uint8_t global) {
  return send_(this->address_, (uint8_t) 68, var, ndays_h, ndays_l, global, (uint8_t) 0, (uint8_t) 0);
}

bool ABBAuroraComponent::read_firmware_release_central_(uint8_t var) {
  return send_(this->address_, (uint8_t) 72, var, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);
}

bool ABBAuroraComponent::read_baudrate_setting_central_(uint8_t baudcode, uint8_t serialline) {
  return send_(this->address_, (uint8_t) 85, baudcode, serialline, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);
}

bool ABBAuroraComponent::read_system_info_central_(uint8_t var) {
  return send_(this->address_, (uint8_t) 101, var, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);
}

bool ABBAuroraComponent::read_junctionbox_monitoring_central_(uint8_t cf, uint8_t rn, uint8_t njt, uint8_t jal,
                                                              uint8_t jah) {
  return send_(this->address_, (uint8_t) 103, cf, rn, njt, jal, jah, (uint8_t) 0);
}

bool ABBAuroraComponent::read_system_partnumber_central_() {
  return send_(this->address_, (uint8_t) 105, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0,
               (uint8_t) 0);
}

bool ABBAuroraComponent::read_system_serialnumber_central_() {
  return send_(this->address_, (uint8_t) 107, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0,
               (uint8_t) 0);
}

bool ABBAuroraComponent::read_state_() {
  state_.ReadState =
      send_(this->address_, (uint8_t) 50, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);

  if (!state_.ReadState) {
    receive_data_[0] = 255;
    receive_data_[1] = 255;
    receive_data_[2] = 255;
    receive_data_[3] = 255;
    receive_data_[4] = 255;
    receive_data_[5] = 255;
  }

  state_.TransmissionState = receive_data_[0];
  state_.GlobalState = receive_data_[1];
  state_.InverterState = receive_data_[2];
  state_.Channel1State = receive_data_[3];
  state_.Channel2State = receive_data_[4];
  state_.AlarmState = receive_data_[5];

  ESP_LOGV(TAG, "TransmissionState: %s", ABBAuroraComponent::transmission_state_text_(state_.TransmissionState));
  ESP_LOGV(TAG, "GlobalState: %s", ABBAuroraComponent::global_state_text_(state_.GlobalState));
  ESP_LOGV(TAG, "InverterState: %s", ABBAuroraComponent::inverter_state_text_(state_.InverterState));
  ESP_LOGV(TAG, "Channel1State: %s", ABBAuroraComponent::dcdc_state_text_(state_.Channel1State));
  ESP_LOGV(TAG, "Channel2State: %s", ABBAuroraComponent::dcdc_state_text_(state_.Channel2State));
  ESP_LOGV(TAG, "AlarmState: %s", ABBAuroraComponent::alarm_state_text_(state_.AlarmState));

  return state_.ReadState;
}

bool ABBAuroraComponent::read_version_() {
  data_version_.ReadState =
      send_(this->address_, (uint8_t)58, (uint8_t)0, (uint8_t)0, (uint8_t)0,
            (uint8_t)0, (uint8_t)0, (uint8_t)0);

  if (!data_version_.ReadState) {
    receive_data_[0] = 255;
    receive_data_[1] = 255;
  }

  data_version_.TransmissionState = receive_data_[0];
  data_version_.GlobalState = receive_data_[1];
  data_version_.Par1 = inverter_version_text_(receive_data_[2]);
  data_version_.Par2 = grid_type_text_(receive_data_[3]);
  data_version_.Par3 = transformer_type_text_(receive_data_[4]);
  data_version_.Par4 = generation_type_text_(receive_data_[5]);

  return data_version_.ReadState;
}

char *ABBAuroraComponent::inverter_version_text_(uint8_t id) {
  char *id_sequence = "iorIO56PC4321DX";
  char *occur = strchr(id_sequence, (char)id);

  if (occur) return INVERTER_VERSION_LOOKUP[occur - id_sequence];

  return UNKNOWN_TEXT;
}

char *ABBAuroraComponent::grid_type_text_(uint8_t id) {
  char *id_sequence = "ABESIUK";
  char *occur = strchr(id_sequence, (char)id);

  if (occur) return GRID_TYPE_LOOKUP[occur - id_sequence];

  return UNKNOWN_TEXT;
}

char *ABBAuroraComponent::transfomer_type_text_(uint8_t id) {
  char *id_sequence = "NT";
  char *occur = strchr(id_sequence, (char)id);

  if (occur) return TRANSFORMER_TYPE_LOOKUP[occur - id_sequence];

  return UNKNOWN_TEXT;
}

char *ABBAuroraComponent::generation_type_text_(uint8_t id) {
  char *id_sequence = "WN";
  char *occur = strchr(id_sequence, (char)id);

  if (occur) return GENERATION_TYPE_LOOKUP[occur - id_sequence];

  return UNKNOWN_TEXT;
}

char *ABBAuroraComponent::alarm_state_text_(uint8_t id) {
  if (id >= 0 && id <= 64) return ALARM_TEXT_LOOKUP[id];

  return UNKNOWN_TEXT;
}

char *ABBAuroraComponent::transmission_state_text_(uint8_t id) {
  if (id >= 0 && id <= 58) {
    if (TRANSMISSION_STATE_LOOKUP[id]) return TRANSMISSION_STATE_LOOKUP[id];
  }
  return UNKNOWN_TEXT;
}

char *ABBAuroraComponent::global_state_text_(uint8_t id) {
  if (id >= 0 && id <= 101) {
    if (GLOBAL_STATE_LOOKUP[id]) return GLOBAL_STATE_LOOKUP[id];
  }
  return UNKNOWN_TEXT;
}

char *ABBAuroraComponent::dcdc_state_text_(uint8_t id) {
  if (id >= 0 and id <= 19) {
    if (DCDC_STATE_LOOKUP[id]) return DCDC_STATE_LOOKUP[id];
  }
  return UNKNOWN_TEXT;
}

char *ABBAuroraComponent::inverter_state_text_(uint8_t id) {
  if (id >= 0 && id <= 47) {
    if (INVERTER_STATE_LOOKUP[id]) return INVERTER_STATE_LOOKUP[id];
  }
  return UNKNOWN_TEXT;
}

}  // namespace abbaurora
}  // namespace esphome
