#include "abbaurora.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/defines.h"

namespace esphome {
namespace abbaurora {

static const char *TAG = "abbaurora";

void ABBAuroraComponent::setup() 
{
    ESP_LOGD(TAG, "Setup");

    // Set flowcontrolpin
    if (this->flow_control_pin_ != nullptr)
        this->flow_control_pin_->setup();

    SendStatus = false;
    ReceiveStatus = false;
    clearReceiveData();

    connection_status->publish_state("Disconnected");
}

void ABBAuroraComponent::dump_config() 
{
  if (this->flow_control_pin_ != nullptr) {
    LOG_PIN("  Flow control Pin: ", this->flow_control_pin_);
  }
}

void ABBAuroraComponent::update()
{
    static int rotaterequests = 0;

    rotaterequests++;

    //If inverter is connected
    if (this->ReadState())
    {
        if (!connection)
        {
            connection = 1;
            connection_status->publish_state("Connected");
        }

        switch( rotaterequests % 26)
        {  
            case 0,10:
                if(power_in_1)
                {
                    if(this->ReadDSPValue(POWER_IN_1, MODULE_MESSUREMENT))
                        power_in_1->publish_state(this->DSP.Value);
                    if(power_in_total && power_in_2)
                        power_in_total->publish_state(power_in_1->get_state() + power_in_2->get_state());
                }
                break;
            case 2,12:
                if(power_in_2)
                {
                    if(this->ReadDSPValue(POWER_IN_2, MODULE_MESSUREMENT))
                        power_in_2->publish_state(this->DSP.Value);
                    if(power_in_total && power_in_1 )
                        power_in_total->publish_state(power_in_1->get_state() + power_in_2->get_state());
                }
                break;
            case 4:
                if(temperature_inverter)
                    if(this->ReadDSPValue(TEMPERATURE_INVERTER, MODULE_MESSUREMENT))
                        temperature_inverter->publish_state(this->DSP.Value);
                break;
            case 6:
                if(v_in_1)
                    if(this->ReadDSPValue(V_IN_1, MODULE_MESSUREMENT))
                        v_in_1->publish_state(this->DSP.Value);
                break;
            case 8:
                if(v_in_2)
                    if(this->ReadDSPValue(V_IN_2, MODULE_MESSUREMENT))
                        v_in_2->publish_state(this->DSP.Value);
                break;
            case 14:
                if(i_in_1) 
                    if(this->ReadDSPValue(I_IN_1, MODULE_MESSUREMENT))
                        i_in_1->publish_state(this->DSP.Value);
                break;
            case 16:
                if(i_in_2) 
                    if(this->ReadDSPValue(I_IN_2, MODULE_MESSUREMENT))
                        i_in_2->publish_state(this->DSP.Value);
               break;
            case 18:
                if(temperature_booster)
                    if(this->ReadDSPValue(TEMPERATURE_BOOSTER, MODULE_MESSUREMENT))
                        temperature_booster->publish_state(this->DSP.Value);
                break;
            case 20:
                if(identification)
                    if(this->ReadSystemSerialNumber() )
                        identification->publish_state(this->SystemSerialNumber.SerialNumber);
            case 22:           
                if(version)
                    if( this->ReadVersion() )
                        version->publish_state( this->Version.Par1 );
                break;
            case 1,9,17:
                if(grid_power)
                    if(this->ReadDSPValue(GRID_POWER, MODULE_MESSUREMENT))
                        grid_power->publish_state( this->DSP.Value );
                break;
            case 3,11,19:
                if(grid_voltage)
                    if(this->ReadDSPValue(GRID_VOLTAGE, MODULE_MESSUREMENT))
                        grid_voltage->publish_state(this->DSP.Value);
                break;
            case 5,13,21:
                if(cumulated_energy_total) 
                    if( this->ReadCumulatedEnergy(TOTAL))
                        cumulated_energy_total->publish_state(this->CumulatedEnergy.Energy);
                break;
            case 7,15,23:
                if(cumulated_energy_today)
                    if(this->ReadCumulatedEnergy(CURRENT_DAY))
                        cumulated_energy_today->publish_state(this->CumulatedEnergy.Energy);
                break;
         
            
        }
    }
    else
    {
        if (connection)
        {
            connection = 0;
            connection_status->publish_state("Disconnected");
        }
        ESP_LOGD(TAG, "Inverter not conntected");
    }
}
 
void ABBAuroraComponent::clearData(uint8_t *data, uint8_t len)
{
    for (int i = 0; i < len; i++)
    {
        data[i] = 0;
    }
}

bool ABBAuroraComponent::Send(uint8_t address, uint8_t param0, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4, uint8_t param5, uint8_t param6)
{
    SendStatus = false;
    ReceiveStatus = false;

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
    uint8_t BccLo = 0xFF;
    uint8_t BccHi = 0xFF;

    for (int i = 0; i < 8; i++)
    {
        uint8_t New = SendData[i] ^ BccLo;
        uint8_t Tmp = New << 4;
        New = Tmp ^ New;
        Tmp = New >> 5;
        BccLo = BccHi;
        BccHi = New ^ Tmp;
        Tmp = New << 3;
        BccLo = BccLo ^ Tmp;
        Tmp = New >> 4;
        BccLo = BccLo ^ Tmp;
    }

    SendData[8] = (uint8_t)(~BccLo);
    SendData[9] = (uint8_t)(~BccHi);

    clearReceiveData();

    for (int i = 0; i < this->MaxAttempt; i++)
    {
        if (this->flow_control_pin_ != nullptr)
	    {
            this->flow_control_pin_->digital_write(true);
            delay(5);      
        }
        //ESP_LOGV(TAG, "> %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x",SendData[0], SendData[1], SendData[2], SendData[3],
          //SendData[4], SendData[5], SendData[6], SendData[7] , SendData[8], SendData[9] );     

        this->write_array( (uint8_t *) SendData, sizeof(SendData));
        
        this->flush();            
        SendStatus = true;

        if (this->flow_control_pin_ != nullptr)
            this->flow_control_pin_->digital_write(false);

        if (this->read_array( (uint8_t *)ReceiveData, sizeof(ReceiveData)) )
        {
            BccLo = 0xFF;
            BccHi = 0xFF;

            for (int i = 0; i < 6; i++)
            {
                uint8_t New = ReceiveData[i] ^ BccLo;
                uint8_t Tmp = New << 4;
                New = Tmp ^ New;
                Tmp = New >> 5;
                BccLo = BccHi;
                BccHi = New ^ Tmp;
                Tmp = New << 3;
                BccLo = BccLo ^ Tmp;
                Tmp = New >> 4;
                BccLo = BccLo ^ Tmp;
            }    
            //ESP_LOGV(TAG, "< %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x", 
                //   ReceiveData[0], ReceiveData[1], ReceiveData[2], ReceiveData[3],
                //   ReceiveData[4], ReceiveData[5], ReceiveData[6], ReceiveData[7] ,(uint8_t)(~BccHi), (uint8_t)(~BccLo) );      
    
            if(  ReceiveData[7] == (uint8_t)(~BccHi) &&  ReceiveData[6] == (uint8_t)(~BccLo) )
            {
                ReceiveStatus = true;
                break;
            }
            else
                ESP_LOGD(TAG, "CRC error in received data");
        }
        yield();
    }
    return ReceiveStatus;
}

void ABBAuroraComponent::clearReceiveData()
{
    clearData(ReceiveData, 8);
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
        clearReceiveData();
        ReceiveData[0] = 255;
        ReceiveData[1] = 255;
    }

    DSP.TransmissionState = ReceiveData[0];
    DSP.GlobalState = ReceiveData[1];

    foo.asBytes[0] = ReceiveData[5];
    foo.asBytes[1] = ReceiveData[4];
    foo.asBytes[2] = ReceiveData[3];
    foo.asBytes[3] = ReceiveData[2];
    
    DSP.Value = foo.asFloat;
    if( DSP.Value > 1E10 ) // this does not make sense
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

    ReceiveData[6]=0; 
    SystemPN.PN = std::string( (char *)ReceiveData );
    
    return SystemPN.ReadState;
}

bool ABBAuroraComponent::ReadSystemSerialNumber()
{
    SystemSerialNumber.ReadState = Send(this->Address, (uint8_t)63, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0);

    ReceiveData[6]=0;
    SystemSerialNumber.SerialNumber = std::string( (char *)ReceiveData );

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

    ReceiveData[6]=0;
    ManufacturingWeekYear.Year = std::string( (char *) &(ReceiveData[4]) );
    ReceiveData[4]=0;
    ManufacturingWeekYear.Week = std::string( (char *) &(ReceiveData[2]) );

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

    ReceiveData[6]=0;
    FirmwareRelease.Release = std::string( (char *) &(ReceiveData[2]) );

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
        clearReceiveData();
        ReceiveData[0] = 255;
        ReceiveData[1] = 255;
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
        clearReceiveData();
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

    Version.Par1 = ABBAuroraStrings::VersionPart1(ReceiveData[2]);
    Version.Par2 = ABBAuroraStrings::VersionPart2(ReceiveData[3]);
    Version.Par3 = ABBAuroraStrings::VersionPart3(ReceiveData[4]);
    Version.Par4 = ABBAuroraStrings::VersionPart4(ReceiveData[5]);

    return Version.ReadState;
};

} // abbaurora namespace
} // esphome namespace
