#include "abbaurora.h"

namespace esphome {
namespace abbaurora {

static const char *TAG = "abbaurora";

void ABBAuroraComponent::setup() 
{
    ESP_LOGD(TAG, "Setup");

    // Set flowcontrolpin
    if (this->flow_control_pin_ != nullptr)
        this->flow_control_pin_->setup();

    for( int i=0; i<8; i++ ) ReceiveData[i]=0;

    connection_status->publish_state("Setup");
}

void ABBAuroraComponent::dump_config() 
{
    if (this->flow_control_pin_ != nullptr) {
        LOG_PIN("  Flow control Pin: ", this->flow_control_pin_);
    }
    ESP_LOGCONFIG(TAG, "  Inverter Address: %d", this->Address );
}

void ABBAuroraComponent::loop()
{
    const uint32_t now = millis();
    static uint32_t last_request = 0;
    static uint32_t rotaterequests = 0;
      
    if( now - last_request > 1000)
    {
        last_request = now;
        rotaterequests++;
        switch( rotaterequests % 30)
        {  
            case 2: if(connection_status && this->ReadState()) // If inverter is connected
                        connection_status->publish_state( ABBAuroraComponent::InverterStateText(State.InverterState) );
                    break;
            case 4: if(identification && this->ReadSystemSerialNumber())
                        identification->publish_state(this->SystemSerialNumber.SerialNumber);
                    break;
            case 6: if(version && this->ReadVersion())
                        version->publish_state( this->Version.Par1 );
                    break;        
            case 8: if(temperature_inverter && this->ReadDSPValue(TEMPERATURE_INVERTER, MODULE_MESSUREMENT))
                        temperature_inverter->publish_state(this->DSP.Value);
                    break;
            case 10:if(power_in_1 && this->ReadDSPValue(POWER_IN_1, MODULE_MESSUREMENT))
                        power_in_1->publish_state(this->DSP.Value);
                    if(power_in_total && power_in_1 && power_in_2)
                        power_in_total->publish_state(power_in_1->get_state() + power_in_2->get_state());
                    break;
            case 12:if(power_in_2 && this->ReadDSPValue(POWER_IN_2, MODULE_MESSUREMENT))
                        power_in_2->publish_state(this->DSP.Value);
                    if(power_in_total && power_in_1 && power_in_2)
                        power_in_total->publish_state(power_in_1->get_state() + power_in_2->get_state());
                    break;
            case 14:if(v_in_1 && this->ReadDSPValue(V_IN_1, MODULE_MESSUREMENT))
                        v_in_1->publish_state(this->DSP.Value);
                    break;
            case 16:if(v_in_2 && this->ReadDSPValue(V_IN_2, MODULE_MESSUREMENT))
                        v_in_2->publish_state(this->DSP.Value);
                    break;
            case 18:if(i_in_1 && this->ReadDSPValue(I_IN_1, MODULE_MESSUREMENT))
                        i_in_1->publish_state(this->DSP.Value);
                    break;
            case 20:if(i_in_2 && this->ReadDSPValue(I_IN_2, MODULE_MESSUREMENT))
                        i_in_2->publish_state(this->DSP.Value);
                    break;
            case 22:if(temperature_booster && this->ReadDSPValue(TEMPERATURE_BOOSTER, MODULE_MESSUREMENT))
                        temperature_booster->publish_state(this->DSP.Value);
                    break;
            case 1:
            case 11:
            case 21:if(grid_power && this->ReadDSPValue(GRID_POWER, MODULE_MESSUREMENT))
                        grid_power->publish_state( this->DSP.Value );
                    break;
            case 3:
            case 13:
            case 23:if(grid_voltage && this->ReadDSPValue(GRID_VOLTAGE, MODULE_MESSUREMENT))
                        grid_voltage->publish_state(this->DSP.Value);
                    break;
            case 5:
            case 15:
            case 25:if(cumulated_energy_total && this->ReadCumulatedEnergy(TOTAL))
                        cumulated_energy_total->publish_state(this->CumulatedEnergy.Energy);
                    break;
            case 7:
            case 17:
            case 27:if(cumulated_energy_today && this->ReadCumulatedEnergy(CURRENT_DAY))
                        cumulated_energy_today->publish_state(this->CumulatedEnergy.Energy);
                    break;
        }            
    }
}

bool ABBAuroraComponent::Send(uint8_t address, uint8_t param0, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4, uint8_t param5, uint8_t param6)
{
    uint8_t SendData[10];
    SendData[0] = address;
    SendData[1] = param0;
    SendData[2] = param1;
    SendData[3] = param2;
    SendData[4] = param3;
    SendData[5] = param4;
    SendData[6] = param5;
    SendData[7] = param6;

    // Calculate CRC16
    uint8_t BccLo = 0xFF; uint8_t BccHi = 0xFF;
    for (int i = 0; i < 8; i++)
    {
        uint8_t New = SendData[i] ^ BccLo;
        uint8_t Tmp = New << 4;
        New = Tmp ^ New; Tmp = New >> 5; BccLo = BccHi; BccHi = New ^ Tmp;
        Tmp = New << 3; BccLo = BccLo ^ Tmp; Tmp = New >> 4; BccLo = BccLo ^ Tmp;
    }
    // CRC bytes
    SendData[8] = (uint8_t)(~BccLo);
    SendData[9] = (uint8_t)(~BccHi);

    // Clear data
    for( int i=0; i<8; i++ ) ReceiveData[i]=0;
    // Empty rx buffer
    while( this->available() )
    {
        uint8_t purge;
        this->read_byte( &purge );
    }

    if (this->flow_control_pin_ != nullptr)
    {
        this->flow_control_pin_->digital_write(true);
        //delay(50);      
    }
    this->write_array( (uint8_t *)SendData, 10 );
    this->flush();            

    if (this->flow_control_pin_ != nullptr)
    {
        this->flow_control_pin_->digital_write(false);
    }

    // Wait for 100ms for data to arrive
    const uint32_t now = millis();
    bool datawaiting = false;
    while( millis() - now < 250 || !datawaiting )
    {
        datawaiting = this->available();
        yield();
    }

    ESP_LOGV( TAG, "Waited for %d ms for data to arrive", millis() - now );

    if( datawaiting )
    {
        if (this->read_array( (uint8_t *)ReceiveData, 8 ) )
        {
            // Calc CRC16
            BccLo = 0xFF; BccHi = 0xFF;
            for (int i = 0; i < 6; i++)
            {
                uint8_t New = ReceiveData[i] ^ BccLo;
                uint8_t Tmp = New << 4;
                New = Tmp ^ New; Tmp = New >> 5; BccLo = BccHi; BccHi = New ^ Tmp; 
                Tmp = New << 3; BccLo = BccLo ^ Tmp; Tmp = New >> 4; BccLo = BccLo ^ Tmp;
            }   
            // Check CRC16 
            if(  ReceiveData[7] == (uint8_t)(~BccHi) &&  ReceiveData[6] == (uint8_t)(~BccLo) )
                return true; // Checksum OK
            else
                ESP_LOGD(TAG, "CRC error in received data");
        }
        else
            ESP_LOGD(TAG, "Failed receiving data");
    }
    else
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
bool ABBAuroraComponent::ReadDSPValue(DSP_VALUE_TYPE type, DSP_GLOBAL global)
{
    if ((((int)type >= 1 && (int)type <= 9) || ((int)type >= 21 && (int)type <= 63)) && ((int)global >= 0 && (int)global <= 1))
    {
        DSP.ReadState = Send(this->Address, (uint8_t)59, type, global, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);

        if (DSP.ReadState == false)
        {
            ReceiveData[0] = 255;
            ReceiveData[1] = 255;
        }
    }
    else
    {
        DSP.ReadState = false;
        ReceiveData[0] = 255; ReceiveData[1] = 255;
        ReceiveData[2] = 0; ReceiveData[3] = 0; ReceiveData[4] = 0; 
        ReceiveData[5] = 0; ReceiveData[6] = 0; ReceiveData[7] = 0;
    }

    DSP.TransmissionState = ReceiveData[0];
    DSP.GlobalState = ReceiveData[1];

    foo.asBytes[0] = ReceiveData[5];
    foo.asBytes[1] = ReceiveData[4];
    foo.asBytes[2] = ReceiveData[3];
    foo.asBytes[3] = ReceiveData[2];
    
    DSP.Value = foo.asFloat;
    if( DSP.Value > 1E10 ||  DSP.Value < 0 ) // this does not make sense
      DSP.Value = 0;

    return DSP.ReadState;
}

bool ABBAuroraComponent::ReadTimeDate()
{
    TimeDate.ReadState = Send(this->Address, (uint8_t)70, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);

    if (TimeDate.ReadState == false)
    {
        ReceiveData[0] = 255;
        ReceiveData[1] = 255;
    }

    TimeDate.TransmissionState = ReceiveData[0];
    TimeDate.GlobalState = ReceiveData[1];
    TimeDate.Seconds = ((unsigned long)ReceiveData[2] << 24) + ((unsigned long)ReceiveData[3] << 16) + ((unsigned long)ReceiveData[4] << 8) + (unsigned long)ReceiveData[5];
    return TimeDate.ReadState;
}

bool ABBAuroraComponent::ReadLastFourAlarms()
{
    LastFourAlarms.ReadState = Send(this->Address, (uint8_t)86, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);

    if (LastFourAlarms.ReadState == false)
    {
        ReceiveData[0] = 255;
        ReceiveData[1] = 255;
        ReceiveData[2] = 255;
        ReceiveData[3] = 255;
        ReceiveData[4] = 255;
        ReceiveData[5] = 255;
    }

    LastFourAlarms.TransmissionState = ReceiveData[0];
    LastFourAlarms.GlobalState = ReceiveData[1];
    LastFourAlarms.Alarms1 = ReceiveData[2];
    LastFourAlarms.Alarms2 = ReceiveData[3];
    LastFourAlarms.Alarms3 = ReceiveData[4];
    LastFourAlarms.Alarms4 = ReceiveData[5];

    return LastFourAlarms.ReadState;
}

bool ABBAuroraComponent::ReadJunctionBoxState(uint8_t nj)
{
    return Send(this->Address, (uint8_t)200, nj, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);
}

bool ABBAuroraComponent::ReadJunctionBoxVal(uint8_t nj, uint8_t par)
{
    return Send(this->Address, (uint8_t)201, nj, par, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);
}

// Inverters
bool ABBAuroraComponent::ReadSystemPN()
{
    SystemPN.ReadState = Send(this->Address, (uint8_t)52, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);

    SystemPN.PN.assign(1, (char)ReceiveData[0] );
    SystemPN.PN += (char)ReceiveData[1];
    SystemPN.PN += (char)ReceiveData[2]; 
    SystemPN.PN += (char)ReceiveData[3];
    SystemPN.PN += (char)ReceiveData[4]; 
    SystemPN.PN += (char)ReceiveData[5]; 
    
    return SystemPN.ReadState;
}

bool ABBAuroraComponent::ReadSystemSerialNumber()
{
    SystemSerialNumber.ReadState = Send(this->Address, (uint8_t)63, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);

    SystemSerialNumber.SerialNumber.assign(1,(char)ReceiveData[0]);
    SystemSerialNumber.SerialNumber += (char)ReceiveData[1];
    SystemSerialNumber.SerialNumber += (char)ReceiveData[2]; 
    SystemSerialNumber.SerialNumber += (char)ReceiveData[3];
    SystemSerialNumber.SerialNumber += (char)ReceiveData[4]; 
    SystemSerialNumber.SerialNumber += (char)ReceiveData[5]; 

    return SystemSerialNumber.ReadState;
}

bool ABBAuroraComponent::ReadManufacturingWeekYear()
{
    ManufacturingWeekYear.ReadState = Send(this->Address, (uint8_t)65, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);

    if (ManufacturingWeekYear.ReadState == false)
    {
        ReceiveData[0] = 255;
        ReceiveData[1] = 255;
    }

    ManufacturingWeekYear.TransmissionState = ReceiveData[0];
    ManufacturingWeekYear.GlobalState = ReceiveData[1];

    ManufacturingWeekYear.Week.assign(1,(char)ReceiveData[2]);  
    ManufacturingWeekYear.Week += (char)ReceiveData[3];
    ManufacturingWeekYear.Year.assign(1,(char)ReceiveData[4]);  
    ManufacturingWeekYear.Year += (char)ReceiveData[5];  

    return ManufacturingWeekYear.ReadState;
}

bool ABBAuroraComponent::ReadFirmwareRelease()
{
    FirmwareRelease.ReadState = Send(this->Address, (uint8_t)72, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);

    if (FirmwareRelease.ReadState == false)
    {
        ReceiveData[0] = 255;
        ReceiveData[1] = 255;
    }

    FirmwareRelease.TransmissionState = ReceiveData[0];
    FirmwareRelease.GlobalState = ReceiveData[1];

    FirmwareRelease.Release.assign(1,(char)ReceiveData[2]);
    FirmwareRelease.Release += '.';
    FirmwareRelease.Release += (char)ReceiveData[3];  
    FirmwareRelease.Release += '.';
    FirmwareRelease.Release += (char)ReceiveData[4];  
    FirmwareRelease.Release += '.';
    FirmwareRelease.Release += (char)ReceiveData[5];  

    return FirmwareRelease.ReadState;
}

bool ABBAuroraComponent::ReadCumulatedEnergy(CUMULATED_ENERGY_TYPE par)
{
    if ((int)par >= 0 && (int)par <= 6)
    {
        CumulatedEnergy.ReadState = Send(this->Address, (uint8_t)78, par, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);

        if (CumulatedEnergy.ReadState == false)
        {
            ReceiveData[0] = 255;
            ReceiveData[1] = 255;
        }
    }
    else
    {
        CumulatedEnergy.ReadState = false;
        ReceiveData[0] = 255; ReceiveData[1] = 255;
        ReceiveData[2] = 0; ReceiveData[3] = 0; ReceiveData[4] = 0; 
        ReceiveData[5] = 0; ReceiveData[6] = 0; ReceiveData[7] = 0;
    }

    CumulatedEnergy.TransmissionState = ReceiveData[0];
    CumulatedEnergy.GlobalState = ReceiveData[1];
    
    if (CumulatedEnergy.ReadState == true)
    {
        ulo.asBytes[0] = ReceiveData[5];
        ulo.asBytes[1] = ReceiveData[4];
        ulo.asBytes[2] = ReceiveData[3];
        ulo.asBytes[3] = ReceiveData[2];

        CumulatedEnergy.Energy = ulo.asUlong;
    }
    return CumulatedEnergy.ReadState;
}

bool ABBAuroraComponent::WriteBaudRateSetting(uint8_t baudcode)
{
    if ((int)baudcode >= 0 && (int)baudcode <= 3)
    {
        return Send(this->Address, (uint8_t)85, baudcode, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);
    }
    else
    {
        for( int i=0; i<8; i++ ) ReceiveData[i]=0;
        return false;
    }
}

// Central
bool ABBAuroraComponent::ReadFlagsSwitchCentral()
{
    return Send(this->Address, (uint8_t)67, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);
}

bool ABBAuroraComponent::ReadCumulatedEnergyCentral(uint8_t var, uint8_t ndays_h, uint8_t ndays_l, uint8_t global)
{
    return Send(this->Address, (uint8_t)68, var, ndays_h, ndays_l, global, (uint8_t)0, (uint8_t)0);
}

bool ABBAuroraComponent::ReadFirmwareReleaseCentral(uint8_t var)
{
    return Send(this->Address, (uint8_t)72, var, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);
}

bool ABBAuroraComponent::ReadBaudRateSettingCentral(uint8_t baudcode, uint8_t serialline)
{
    return Send(this->Address, (uint8_t)85, baudcode, serialline, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);
}

bool ABBAuroraComponent::ReadSystemInfoCentral(uint8_t var)
{
    return Send(this->Address, (uint8_t)101, var, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);
}

bool ABBAuroraComponent::ReadJunctionBoxMonitoringCentral(uint8_t cf, uint8_t rn, uint8_t njt, uint8_t jal, uint8_t jah)
{
    return Send(this->Address, (uint8_t)103, cf, rn, njt, jal, jah, (uint8_t)0);
}

bool ABBAuroraComponent::ReadSystemPNCentral()
{
    return Send(this->Address, (uint8_t)105, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);
}

bool ABBAuroraComponent::ReadSystemSerialNumberCentral()
{
    return Send(this->Address, (uint8_t)107, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);
}

bool ABBAuroraComponent::ReadState()
{
    State.ReadState = Send(this->Address, (uint8_t)50, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);

    if (State.ReadState == false)
    {
        ReceiveData[0] = 255;
        ReceiveData[1] = 255;
        ReceiveData[2] = 255;
        ReceiveData[3] = 255;
        ReceiveData[4] = 255;
        ReceiveData[5] = 255;
    }

    State.TransmissionState = ReceiveData[0];
    State.GlobalState = ReceiveData[1];
    State.InverterState = ReceiveData[2];
    State.Channel1State = ReceiveData[3];
    State.Channel2State = ReceiveData[4];
    State.AlarmState = ReceiveData[5];

    return State.ReadState;
}

bool ABBAuroraComponent::ReadVersion()
{
    Version.ReadState = Send(this->Address, (uint8_t)58, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);

    if(Version.ReadState == false)
    {
        ReceiveData[0] = 255;
        ReceiveData[1] = 255;
    }

    Version.TransmissionState = ReceiveData[0];
    Version.GlobalState = ReceiveData[1];

    switch ((char)ReceiveData[2])
    {
        case 'i': Version.Par1 = std::string("Aurora 2 kW indoor"); break;
        case 'o': Version.Par1 = std::string("Aurora 2 kW outdoor"); break;
        case 'r': Version.Par1 = std::string("ABB TRIO 5.8kW outdoor"); break;
        case 'I': Version.Par1 = std::string("Aurora 3.6 kW indoor"); break;
        case 'O': Version.Par1 = std::string("Aurora 3.0 - 3.6 kW outdoor"); break;
        case '5': Version.Par1 = std::string("Aurora 5.0 kW outdoor"); break;
        case '6': Version.Par1 = std::string("Aurora 6 kW outdoor"); break;
        case 'P': Version.Par1 = std::string("3 - phase interface (3G74)"); break;
        case 'C': Version.Par1 = std::string("Aurora 50kW module"); break;
        case '4': Version.Par1 = std::string("Aurora 4.2kW new"); break;
        case '3': Version.Par1 = std::string("Aurora 3.6kW new"); break;
        case '2': Version.Par1 = std::string("Aurora 3.3kW new"); break;
        case '1': Version.Par1 = std::string("Aurora 3.0kW new"); break;
        case 'D': Version.Par1 = std::string("Aurora 12.0kW"); break;
        case 'X': Version.Par1 = std::string("Aurora 10.0kW"); break;
        default: ESP_LOGD(TAG, "Version.par1 unknown [%d]", ReceiveData[2] );
                 Version.Par1 = std::string("Unknown");
    }
 
    switch ((char)ReceiveData[3])
    {
        case 'A': Version.Par2 = std::string("UL1741"); break;
        case 'B': Version.Par2 = std::string("NETHERL"); break;
        case 'E': Version.Par2 = std::string("VDE0126"); break;
        case 'S': Version.Par2 = std::string("DR 1663 / 2000"); break;
        case 'I': Version.Par2 = std::string("ENEL DK 5950"); break;
        case 'U': Version.Par2 = std::string("UK G83"); break;
        case 'K': Version.Par2 = std::string("AS 4777"); break;
        default: ESP_LOGD(TAG, "Version.par2 unknown [%d]", ReceiveData[3] );
                 Version.Par2 = std::string("Unknown"); 
    }

    switch ((char)ReceiveData[4])
    {
        case 'N': Version.Par3 = std::string("Transformerless Version"); break;
        case 'T': Version.Par3 = std::string("Transformer Version"); break;
        default: ESP_LOGD(TAG, "Version.par3 unknown [%d]", ReceiveData[4] );
                 Version.Par3 = std::string("Unknown");
    }

    switch ((char)ReceiveData[5])
    {
        case 'W': Version.Par4 = std::string("Wind version"); break;
        case 'N': Version.Par4 = std::string("PV version"); break;
        default: ESP_LOGD(TAG, "Version.par4 unknown [%d]", ReceiveData[5] );
                 Version.Par4 = std::string("Unknown");
    }
    return Version.ReadState;
};

std::string ABBAuroraComponent::AlarmStateText(uint8_t id)
{
    switch (id)
    {
        case 0: return std::string("No Alarm");
        case 1: return std::string("Sun Low");
        case 2: return std::string("Input OC");
        case 3: return std::string("Input UV");
        case 4: return std::string("Input OV");
        case 5: return std::string("Sun Low");
        case 6: return std::string("No Parameters");
        case 7: return std::string("Bulk OV");
        case 8: return std::string("Comm.Error");
        case 9: return std::string("Output OC");
        case 10: return std::string("IGBT Sat");
        case 11: return std::string("Bulk UV");
        case 12: return std::string("Internal error");
        case 13: return std::string("Grid Fail");
        case 14: return std::string("Bulk Low");
        case 15: return std::string("Ramp Fail");
        case 16: return std::string("Dc / Dc Fail");
        case 17: return std::string("Wrong Mode");
        case 18: return std::string("Ground Fault");
        case 19: return std::string("Over Temp.");
        case 20: return std::string("Bulk Cap Fail");
        case 21: return std::string("Inverter Fail");
        case 22: return std::string("Start Timeout");
        case 23: return std::string("Ground Fault");
        case 24: return std::string("Degauss error");
        case 25: return std::string("Ileak sens.fail");
        case 26: return std::string("DcDc Fail");
        case 27: return std::string("Self Test Error 1");
        case 28: return std::string("Self Test Error 2");
        case 29: return std::string("Self Test Error 3");
        case 30: return std::string("Self Test Error 4");
        case 31: return std::string("DC inj error");
        case 32: return std::string("Grid OV");
        case 33: return std::string("Grid UV");
        case 34: return std::string("Grid OF");
        case 35: return std::string("Grid UF");
        case 36: return std::string("Z grid Hi");
        case 37: return std::string("Internal error");
        case 38: return std::string("Riso Low");
        case 39: return std::string("Vref Error");
        case 40: return std::string("Error Meas V");
        case 41: return std::string("Error Meas F");
        case 42: return std::string("Error Meas Z");
        case 43: return std::string("Error Meas Ileak");
        case 44: return std::string("Error Read V");
        case 45: return std::string("Error Read I");
        case 46: return std::string("Table fail");
        case 47: return std::string("Fan Fail");
        case 48: return std::string("UTH");
        case 49: return std::string("Interlock fail");
        case 50: return std::string("Remote Off");
        case 51: return std::string("Vout Avg errror");
        case 52: return std::string("Battery low");
        case 53: return std::string("Clk fail");
        case 54: return std::string("Input UC");
        case 55: return std::string("Zero Power");
        case 56: return std::string("Fan Stucked");
        case 57: return std::string("DC Switch Open");
        case 58: return std::string("Tras Switch Open");
        case 59: return std::string("AC Switch Open");
        case 60: return std::string("Bulk UV");
        case 61: return std::string("Autoexclusion");
        case 62: return std::string("Grid df / dt");
        case 63: return std::string("Den switch Open");
        case 64: return std::string("Jbox fail");
        default: ESP_LOGD(TAG, "Alarm Text unknown [%d]", id );
                 return std::string("Unknown");
    }
}

std::string ABBAuroraComponent::TransmissionText(uint8_t id)
{
    switch (id)
    {
        case 0:  return std::string("Everything is OK.");
        case 51: return std::string("Command is not implemented");
        case 52: return std::string("Variable does not exist");
        case 53: return std::string("Variable value is out of range");
        case 54: return std::string("EEprom not accessible");
        case 55: return std::string("Not Toggled Service Mode");
        case 56: return std::string("Can not send the command to internal micro");
        case 57: return std::string("Command not Executed");
        case 58: return std::string("The variable is not available, retry");
        default: ESP_LOGD(TAG, "Transmission Text unknown [%d]", id );
                 return std::string("Unknown");
    }
}

std::string ABBAuroraComponent::GlobalStateText(uint8_t id)
{
    switch (id)
    {
        case 0: return std::string("Sending Parameters");
        case 1: return std::string("Wait Sun / Grid");
        case 2: return std::string("Checking Grid");
        case 3: return std::string("Measuring Riso");
        case 4: return std::string("DcDc Start");
        case 5: return std::string("Inverter Start");
        case 6: return std::string("Run");
        case 7: return std::string("Recovery");
        case 8: return std::string("Pausev");
        case 9: return std::string("Ground Fault");
        case 10: return std::string("OTH Fault");
        case 11: return std::string("Address Setting");
        case 12: return std::string("Self Test");
        case 13: return std::string("Self Test Fail");
        case 14: return std::string("Sensor Test + Meas.Riso");
        case 15: return std::string("Leak Fault");
        case 16: return std::string("Waiting for manual reset");
        case 17: return std::string("Internal Error E026");
        case 18: return std::string("Internal Error E027");
        case 19: return std::string("Internal Error E028");
        case 20: return std::string("Internal Error E029");
        case 21: return std::string("Internal Error E030");
        case 22: return std::string("Sending Wind Table");
        case 23: return std::string("Failed Sending table");
        case 24: return std::string("UTH Fault");
        case 25: return std::string("Remote OFF");
        case 26: return std::string("Interlock Fail");
        case 27: return std::string("Executing Autotest");
        case 30: return std::string("Waiting Sun");
        case 31: return std::string("Temperature Fault");
        case 32: return std::string("Fan Staucked");
        case 33: return std::string("Int.Com.Fault");
        case 34: return std::string("Slave Insertion");
        case 35: return std::string("DC Switch Open");
        case 36: return std::string("TRAS Switch Open");
        case 37: return std::string("MASTER Exclusion");
        case 38: return std::string("Auto Exclusion");
        case 98: return std::string("Erasing Internal EEprom");
        case 99: return std::string("Erasing External EEprom");
        case 100: return std::string("Counting EEprom");
        case 101: return std::string("Freeze");
        default: ESP_LOGD(TAG, "GlobalState Text unknown [%d]", id );
                 return std::string("Unknown");
    }
}

std::string ABBAuroraComponent::DcDcStateText(uint8_t id)
{
    switch (id)
    {
        case 0: return std::string("DcDc OFF");
        case 1: return std::string("Ramp Start");
        case 2: return std::string("MPPT");
        case 3: return std::string("Not Used");
        case 4: return std::string("Input OC");
        case 5: return std::string("Input UV");
        case 6: return std::string("Input OV");
        case 7: return std::string("Input Low");
        case 8: return std::string("No Parameters");
        case 9: return std::string("Bulk OV");
        case 10: return std::string("Communication Error");
        case 11: return std::string("Ramp Fail");
        case 12: return std::string("Internal Error");
        case 13: return std::string("Input mode Error");
        case 14: return std::string("Ground Fault");
        case 15: return std::string("Inverter Fail");
        case 16: return std::string("DcDc IGBT Sat");
        case 17: return std::string("DcDc ILEAK Fail");
        case 18: return std::string("DcDc Grid Fail");
        case 19: return std::string("DcDc Comm.Error");
        default: ESP_LOGD(TAG, "DcDcState Text unknown [%d]", id );
                 return std::string("Unknown");
    }
}

std::string ABBAuroraComponent::InverterStateText(uint8_t id)
{
    switch (id)
    {
        case 0: return std::string("Stand By");
        case 1: return std::string("Checking Grid");
        case 2: return std::string("Run");
        case 3: return std::string("Bulk OV");
        case 4: return std::string("Out OC");
        case 5: return std::string("IGBT Sat");
        case 6: return std::string("Bulk UV");
        case 7: return std::string("Degauss Error");
        case 8: return std::string("No Parameters");
        case 9: return std::string("Bulk Low");
        case 10: return std::string("Grid OV");
        case 11: return std::string("Communication Error");
        case 12: return std::string("Degaussing");
        case 13: return std::string("Starting");
        case 14: return std::string("Bulk Cap Fail");
        case 15: return std::string("Leak Fail");
        case 16: return std::string("DcDc Fail");
        case 17: return std::string("Ileak Sensor Fail");
        case 18: return std::string("SelfTest: relay inverter");
        case 19: return std::string("SelfTest : wait for sensor test");
        case 20: return std::string("SelfTest : test relay DcDc + sensor");
        case 21: return std::string("SelfTest : relay inverter fail");
        case 22: return std::string("SelfTest timeout fail");
        case 23: return std::string("SelfTest : relay DcDc fail");
        case 24: return std::string("Self Test 1");
        case 25: return std::string("Waiting self test start");
        case 26: return std::string("Dc Injection");
        case 27: return std::string("Self Test 2");
        case 28: return std::string("Self Test 3");
        case 29: return std::string("Self Test 4");
        case 30: return std::string("Internal Error");
        case 31: return std::string("Internal Error");
        case 40: return std::string("Forbidden State");
        case 41: return std::string("Input UC");
        case 42: return std::string("Zero Power");
        case 43: return std::string("Grid Not Present");
        case 44: return std::string("Waiting Start");
        case 45: return std::string("MPPT");
        case 46: return std::string("Grid Fail");
        case 47: return std::string("Input OC");
        default: ESP_LOGD(TAG, "InverterState Text unknown [%d]", id );
                 return std::string("Unknown");       
    }
}

} // abbaurora namespace
} // esphome namespace
