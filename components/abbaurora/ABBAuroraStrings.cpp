#include "ABBAuroraStrings.h"

std::string ABBAuroraStrings::TransmissionState(uint8_t id)
{
    switch (id)
    {
    case 0:
        return std::string("Everything is OK.");
    case 51:
        return std::string("Command is not implemented");
    case 52:
        return std::string("Variable does not exist");
    case 53:
        return std::string("Variable value is out of range");
    case 54:
        return std::string("EEprom not accessible");
    case 55:
        return std::string("Not Toggled Service Mode");
    case 56:
        return std::string("Can not send the command to internal micro");
    case 57:
        return std::string("Command not Executed");
    case 58:
        return std::string("The variable is not available, retry");
    default:
        return std::string("Unknown");
    }
}

std::string ABBAuroraStrings::GlobalState(uint8_t id)
{
    switch (id)
    {
    case 0:
        return std::string("Sending Parameters");
    case 1:
        return std::string("Wait Sun / Grid");
    case 2:
        return std::string("Checking Grid");
    case 3:
        return std::string("Measuring Riso");
    case 4:
        return std::string("DcDc Start");
    case 5:
        return std::string("Inverter Start");
    case 6:
        return std::string("Run");
    case 7:
        return std::string("Recovery");
    case 8:
        return std::string("Pausev");
    case 9:
        return std::string("Ground Fault");
    case 10:
        return std::string("OTH Fault");
    case 11:
        return std::string("Address Setting");
    case 12:
        return std::string("Self Test");
    case 13:
        return std::string("Self Test Fail");
    case 14:
        return std::string("Sensor Test + Meas.Riso");
    case 15:
        return std::string("Leak Fault");
    case 16:
        return std::string("Waiting for manual reset");
    case 17:
        return std::string("Internal Error E026");
    case 18:
        return std::string("Internal Error E027");
    case 19:
        return std::string("Internal Error E028");
    case 20:
        return std::string("Internal Error E029");
    case 21:
        return std::string("Internal Error E030");
    case 22:
        return std::string("Sending Wind Table");
    case 23:
        return std::string("Failed Sending table");
    case 24:
        return std::string("UTH Fault");
    case 25:
        return std::string("Remote OFF");
    case 26:
        return std::string("Interlock Fail");
    case 27:
        return std::string("Executing Autotest");
    case 30:
        return std::string("Waiting Sun");
    case 31:
        return std::string("Temperature Fault");
    case 32:
        return std::string("Fan Staucked");
    case 33:
        return std::string("Int.Com.Fault");
    case 34:
        return std::string("Slave Insertion");
    case 35:
        return std::string("DC Switch Open");
    case 36:
        return std::string("TRAS Switch Open");
    case 37:
        return std::string("MASTER Exclusion");
    case 38:
        return std::string("Auto Exclusion");
    case 98:
        return std::string("Erasing Internal EEprom");
    case 99:
        return std::string("Erasing External EEprom");
    case 100:
        return std::string("Counting EEprom");
    case 101:
        return std::string("Freeze");
    default:
        return std::string("Unknown");
    }
}

std::string ABBAuroraStrings::DcDcState(uint8_t id)
{
    switch (id)
    {
    case 0:
        return std::string("DcDc OFF");
    case 1:
        return std::string("Ramp Start");
    case 2:
        return std::string("MPPT");
    case 3:
        return std::string("Not Used");
    case 4:
        return std::string("Input OC");
    case 5:
        return std::string("Input UV");
    case 6:
        return std::string("Input OV");
    case 7:
        return std::string("Input Low");
    case 8:
        return std::string("No Parameters");
    case 9:
        return std::string("Bulk OV");
    case 10:
        return std::string("Communication Error");
    case 11:
        return std::string("Ramp Fail");
    case 12:
        return std::string("Internal Error");
    case 13:
        return std::string("Input mode Error");
    case 14:
        return std::string("Ground Fault");
    case 15:
        return std::string("Inverter Fail");
    case 16:
        return std::string("DcDc IGBT Sat");
    case 17:
        return std::string("DcDc ILEAK Fail");
    case 18:
        return std::string("DcDc Grid Fail");
    case 19:
        return std::string("DcDc Comm.Error");
    default:
        return std::string("Unknown");
    }
}

std::string ABBAuroraStrings::InverterState(uint8_t id)
{
    switch (id)
    {
    case 0:
        return std::string("Stand By");
    case 1:
        return std::string("Checking Grid");
    case 2:
        return std::string("Run");
    case 3:
        return std::string("Bulk OV");
    case 4:
        return std::string("Out OC");
    case 5:
        return std::string("IGBT Sat");
    case 6:
        return std::string("Bulk UV");
    case 7:
        return std::string("Degauss Error");
    case 8:
        return std::string("No Parameters");
    case 9:
        return std::string("Bulk Low");
    case 10:
        return std::string("Grid OV");
    case 11:
        return std::string("Communication Error");
    case 12:
        return std::string("Degaussing");
    case 13:
        return std::string("Starting");
    case 14:
        return std::string("Bulk Cap Fail");
    case 15:
        return std::string("Leak Fail");
    case 16:
        return std::string("DcDc Fail");
    case 17:
        return std::string("Ileak Sensor Fail");
    case 18:
        return std::string("SelfTest: relay inverter");
    case 19:
        return std::string("SelfTest : wait for sensor test");
    case 20:
        return std::string("SelfTest : test relay DcDc + sensor");
    case 21:
        return std::string("SelfTest : relay inverter fail");
    case 22:
        return std::string("SelfTest timeout fail");
    case 23:
        return std::string("SelfTest : relay DcDc fail");
    case 24:
        return std::string("Self Test 1");
    case 25:
        return std::string("Waiting self test start");
    case 26:
        return std::string("Dc Injection");
    case 27:
        return std::string("Self Test 2");
    case 28:
        return std::string("Self Test 3");
    case 29:
        return std::string("Self Test 4");
    case 30:
        return std::string("Internal Error");
    case 31:
        return std::string("Internal Error");
    case 40:
        return std::string("Forbidden State");
    case 41:
        return std::string("Input UC");
    case 42:
        return std::string("Zero Power");
    case 43:
        return std::string("Grid Not Present");
    case 44:
        return std::string("Waiting Start");
    case 45:
        return std::string("MPPT");
    case 46:
        return std::string("Grid Fail");
    case 47:
        return std::string("Input OC");
    default:
        return std::string("Unknown");
    }
}

std::string ABBAuroraStrings::AlarmState(uint8_t id)
{
    switch (id)
    {
    case 0:
        return std::string("No Alarm");
    case 1:
        return std::string("Sun Low");
    case 2:
        return std::string("Input OC");
    case 3:
        return std::string("Input UV");
    case 4:
        return std::string("Input OV");
    case 5:
        return std::string("Sun Low");
    case 6:
        return std::string("No Parameters");
    case 7:
        return std::string("Bulk OV");
    case 8:
        return std::string("Comm.Error");
    case 9:
        return std::string("Output OC");
    case 10:
        return std::string("IGBT Sat");
    case 11:
        return std::string("Bulk UV");
    case 12:
        return std::string("Internal error");
    case 13:
        return std::string("Grid Fail");
    case 14:
        return std::string("Bulk Low");
    case 15:
        return std::string("Ramp Fail");
    case 16:
        return std::string("Dc / Dc Fail");
    case 17:
        return std::string("Wrong Mode");
    case 18:
        return std::string("Ground Fault");
    case 19:
        return std::string("Over Temp.");
    case 20:
        return std::string("Bulk Cap Fail");
    case 21:
        return std::string("Inverter Fail");
    case 22:
        return std::string("Start Timeout");
    case 23:
        return std::string("Ground Fault");
    case 24:
        return std::string("Degauss error");
    case 25:
        return std::string("Ileak sens.fail");
    case 26:
        return std::string("DcDc Fail");
    case 27:
        return std::string("Self Test Error 1");
    case 28:
        return std::string("Self Test Error 2");
    case 29:
        return std::string("Self Test Error 3");
    case 30:
        return std::string("Self Test Error 4");
    case 31:
        return std::string("DC inj error");
    case 32:
        return std::string("Grid OV");
    case 33:
        return std::string("Grid UV");
    case 34:
        return std::string("Grid OF");
    case 35:
        return std::string("Grid UF");
    case 36:
        return std::string("Z grid Hi");
    case 37:
        return std::string("Internal error");
    case 38:
        return std::string("Riso Low");
    case 39:
        return std::string("Vref Error");
    case 40:
        return std::string("Error Meas V");
    case 41:
        return std::string("Error Meas F");
    case 42:
        return std::string("Error Meas Z");
    case 43:
        return std::string("Error Meas Ileak");
    case 44:
        return std::string("Error Read V");
    case 45:
        return std::string("Error Read I");
    case 46:
        return std::string("Table fail");
    case 47:
        return std::string("Fan Fail");
    case 48:
        return std::string("UTH");
    case 49:
        return std::string("Interlock fail");
    case 50:
        return std::string("Remote Off");
    case 51:
        return std::string("Vout Avg errror");
    case 52:
        return std::string("Battery low");
    case 53:
        return std::string("Clk fail");
    case 54:
        return std::string("Input UC");
    case 55:
        return std::string("Zero Power");
    case 56:
        return std::string("Fan Stucked");
    case 57:
        return std::string("DC Switch Open");
    case 58:
        return std::string("Tras Switch Open");
    case 59:
        return std::string("AC Switch Open");
    case 60:
        return std::string("Bulk UV");
    case 61:
        return std::string("Autoexclusion");
    case 62:
        return std::string("Grid df / dt");
    case 63:
        return std::string("Den switch Open");
    case 64:
        return std::string("Jbox fail");
    default:
        return std::string("Unknown");
    }
}

std::string ABBAuroraStrings::VersionPart1(uint8_t id)
{
    switch ((char)id)
    {
    case 'i':
        return std::string("Aurora 2 kW indoor");
    case 'o':
        return std::string("Aurora 2 kW outdoor");
    case 'r':
        return std::string("ABB TRIO 5.8kW outdoor");
    case 'I':
        return std::string("Aurora 3.6 kW indoor");
    case 'O':
        return std::string("Aurora 3.0 - 3.6 kW outdoor");
    case '5':
        return std::string("Aurora 5.0 kW outdoor");
    case '6':
        return std::string("Aurora 6 kW outdoor");
    case 'P':
        return std::string("3 - phase interface (3G74)");
    case 'C':
        return std::string("Aurora 50kW module");
    case '4':
        return std::string("Aurora 4.2kW new");
    case '3':
        return std::string("Aurora 3.6kW new");
    case '2':
        return std::string("Aurora 3.3kW new");
    case '1':
        return std::string("Aurora 3.0kW new");
    case 'D':
        return std::string("Aurora 12.0kW");
    case 'X':
        return std::string("Aurora 10.0kW");
    default:
        return std::string("Unknown");
    }
}

std::string ABBAuroraStrings::VersionPart2(uint8_t id)
{
    switch ((char)id)
    {
    case 'A':
        return std::string("UL1741");
    case 'E':
        return std::string("VDE0126");
    case 'S':
        return std::string("DR 1663 / 2000");
    case 'I':
        return std::string("ENEL DK 5950");
    case 'U':
        return std::string("UK G83");
    case 'K':
        return std::string("AS 4777");
    default:
        return std::string("Unknown");
    }
}

std::string ABBAuroraStrings::VersionPart3(uint8_t id)
{

    switch ((char)id)
    {
    case 'N':
        return std::string("Transformerless Version");
    case 'T':
        return std::string("Transformer Version");
    default:
        return std::string("Unknown");
    }
}

std::string ABBAuroraStrings::VersionPart4(uint8_t id)
{
    switch ((char)id)
    {
    case 'W':
        return std::string("Wind version");
    case 'N':
        return std::string("PV version");
    default:
        return std::string("Unknown");
    }
}
