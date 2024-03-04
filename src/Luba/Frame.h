#include "Arduino.h"

class LubaFrame
{
    public:
        LubaFrame(uint8_t *data);
        uint8_t command();
        uint8_t length();

    private:
        uint8_t *_data;
};