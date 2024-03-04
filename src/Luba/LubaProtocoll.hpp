#include "Arduino.h"
#include "OpenKNX.h"

uint8_t buffer[260];
uint8_t position = 0;

enum class LubaState {
    Idle,
    Receiving,
    End
};
LubaState state = LubaState::Idle;

void loopLubaProtocoll()
{
    if(Serial.available())
    {
        while(Serial.available())
        {
            uint8_t data = Serial.read();
            logDebug("raw", "%.2X", data);

            switch(state)
            {
                case LubaState::Idle:
                {
                    if(data == 0x59)
                    {
                        state = LubaState::Receiving;
                        buffer[0] = data;
                        position = 1;
                    }
                    break;
                }

                case LubaState::Receiving:
                {
                    buffer[position] = data;
                    if(position == 1)
                        logDebug("LUBA", "Command: %.2X", data);
                    
                    if(position == 2)
                        logDebug("LUBA", "Length: %.2X", data);

                    if(position > 2 && position == buffer[2] + 3)
                    {
                        state = LubaState::End;
                        logHexDebug("LUBA", buffer, buffer[2] + 4);
                    }
                    position++;
                    break;
                }

                case LubaState::End:
                {
                    //TODO process frame
                    state = LubaState::Idle;
                    break;
                }
            }
        }
    }
}