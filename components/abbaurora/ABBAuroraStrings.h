#ifndef ABBAuroraStrings_h
#define ABBAuroraStrings_h

#include "esphome/core/helpers.h"

class ABBAuroraStrings {
    private:
    
    public:
       static std::string TransmissionState(uint8_t id);
       static std::string GlobalState(uint8_t id);
       static std::string DcDcState(uint8_t id);
       static std::string InverterState(uint8_t id);
       static std::string AlarmState(uint8_t id);
       static std::string VersionPart1(uint8_t id);
       static std::string VersionPart2(uint8_t id);
       static std::string VersionPart3(uint8_t id);
       static std::string VersionPart4(uint8_t id);
};
#endif
