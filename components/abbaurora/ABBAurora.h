#pragma once

#include "ABBAuroraEnums.h"
#include "ABBAuroraStrings.h"

#include "esphome/core/component.h"

#include "esphome/components/sensor/sensor.h"

#include "esphome/components/text_sensor/text_sensor.h"

#include "esphome/components/uart/uart.h"

#include "esphome/core/log.h"

#include "esphome/core/defines.h"

#define CONNECTED "CONNECTED"
#define DISCONNECTED "DISCONNECTED"

#define TAG "ABBAurora"

namespace esphome {
namespace abbaurora { 

class ABBAurora : public uart::UARTDevice, public PollingComponent
{

protected:
    ABBAurora() : PollingComponent(15000) {}

private:

    uint8_t connection = 0;
    int MaxAttempt = 1;

    void clearData(byte *data, byte len);

    int Crc16(byte *data, int offset, int count);

    bool Send(byte address, byte param0, byte param1, byte param2, byte param3, byte param4, byte param5, byte param6);

    union {
        byte asBytes[4];
        float asFloat;
    } foo;

    union {
        byte asBytes[4];
        unsigned long asUlong;
    } ulo;


public:
    TextSensor *connection_status = new TextSensor();
    TextSensor *version = new TextSensor();

    Sensor *v_in_1 = new Sensor();
    Sensor *v_in_2 = new Sensor();
    Sensor *i_in_1 = new Sensor();
    Sensor *i_in_2 = new Sensor();
    Sensor *power_in_1 = new Sensor();
    Sensor *power_in_2 = new Sensor();
    Sensor *power_in_total = new Sensor();
    Sensor *power_out_total = new Sensor();
    Sensor *temperature_inverter = new Sensor();
    Sensor *temperature_booster = new Sensor();
    Sensor *grid_voltage = new Sensor();
    Sensor *cumulated_energy_today = new Sensor();
    Sensor *cumulated_energy_total = new Sensor();

    void set_address(uint8_t address);

    void setup() override;
    void update() override;

    bool SendStatus = false;
    bool ReceiveStatus = false;
    byte Address = 0; // Default 2 ??
    byte ReceiveData[8];

    void clearReceiveData();

    typedef struct
    {
        byte TransmissionState;
        byte GlobalState;
        byte InverterState;
        byte Channel1State;
        byte Channel2State;
        byte AlarmState;
        bool ReadState;
    } DataState;

    DataState State;

    bool ReadState();

    typedef struct
    {
        byte TransmissionState;
        byte GlobalState;
        String Par1;
        String Par2;
        String Par3;
        String Par4;
        bool ReadState;
    } DataVersion;

    DataVersion Version;

    bool ReadVersion();

    typedef struct
    {
        byte TransmissionState;
        byte GlobalState;
        float Value;
        bool ReadState;
    } DataDSP;

    DataDSP DSP;

    bool ReadDSPValue(DSP_VALUE_TYPE type, DSP_GLOBAL global);

    typedef struct
    {
        byte TransmissionState;
        byte GlobalState;
        unsigned long Seconds;
        bool ReadState;
    } DataTimeDate;

    DataTimeDate TimeDate;

    bool ReadTimeDate();

    typedef struct
    {
        byte TransmissionState;
        byte GlobalState;
        byte Alarms1;
        byte Alarms2;
        byte Alarms3;
        byte Alarms4;
        bool ReadState;
    } DataLastFourAlarms;

    DataLastFourAlarms LastFourAlarms;

    bool ReadLastFourAlarms();

    bool ReadJunctionBoxState(byte nj);

    bool ReadJunctionBoxVal(byte nj, byte par);

    // Inverters
    typedef struct
    {
        String PN;
        bool ReadState;
    } DataSystemPN;

    DataSystemPN SystemPN;

    bool ReadSystemPN();

    typedef struct
    {
        String SerialNumber;
        bool ReadState;
    } DataSystemSerialNumber;

    DataSystemSerialNumber SystemSerialNumber;

    bool ReadSystemSerialNumber();

    typedef struct
    {
        byte TransmissionState;
        byte GlobalState;
        String Week;
        String Year;
        bool ReadState;
    } DataManufacturingWeekYear;

    DataManufacturingWeekYear ManufacturingWeekYear;

    bool ReadManufacturingWeekYear();

    typedef struct
    {
        byte TransmissionState;
        byte GlobalState;
        String Release;
        bool ReadState;
    } DataFirmwareRelease;

    DataFirmwareRelease FirmwareRelease;

    bool ReadFirmwareRelease();

    typedef struct
    {
        byte TransmissionState;
        byte GlobalState;
        unsigned long Energy;
        bool ReadState;
    } DataCumulatedEnergy;

    DataCumulatedEnergy CumulatedEnergy;

    bool ReadCumulatedEnergy(CUMULATED_ENERGY_TYPE par);

    bool WriteBaudRateSetting(byte baudcode);

    // Central
    bool ReadFlagsSwitchCentral();

    bool ReadCumulatedEnergyCentral(byte var, byte ndays_h, byte ndays_l, byte global);

    bool ReadFirmwareReleaseCentral(byte var);

    bool ReadBaudRateSettingCentral(byte baudcode, byte serialline);

    bool ReadSystemInfoCentral(byte var);

    bool ReadJunctionBoxMonitoringCentral(byte cf, byte rn, byte njt, byte jal, byte jah);

    bool ReadSystemPNCentral();

    bool ReadSystemSerialNumberCentral();
};

} // namespace abbaurora
} // namespace esphome
