#ifndef ABBAuroraStrings_h
#define ABBAuroraStrings_h

#include "esphome/core/helpers.h"

class ABBAuroraStrings {
    private:
    
    public:
       static std::string TransmissionState(byte id);
       static std::string GlobalState(byte id);
       static std::string DcDcState(byte id);
       static std::string InverterState(byte id);
       static std::string AlarmState(byte id);
       static std::string VersionPart1(byte id);
       static std::string VersionPart2(byte id);
       static std::string VersionPart3(byte id);
       static std::string VersionPart4(byte id);
};
#endif
