#include <Arduino.h>
#include "Dali.h"
#include "RTTStream.h"
#include "Ballast.hpp"


#define DALI_WAIT_RANDOMIZE 1000
#define DALI_WAIT_SEARCH 300

void loopAddressing();

enum class AddressingState {
	None,
	//addressing
	Init,
	Randomize_Wait,
	Search,
	SearchWait,
	Found,
	Finish,
	//addressing auto
	SearchAdr,
	SearchAdrWait,
	GetHWait,
	GetLWait,
	GetMWait,
	//assigning
	Query_Wait,
	Withdraw_Others,
	Set_Address,
	Check_Address,
	Confirm_Address
};
enum class AssigningState {
	None,
	Working,
	Success,
	Failed_Bus = 10,
	Failed_Exists,
	Failed_Exists_Not,
	Failed_Confirm,
	Failed_No_Answer
};
		
uint32_t _adrLow = 0;
uint32_t _adrHigh = 0xFFFFFF;
uint32_t _adrHighLast = 0xFFFFFF;
int _adrResp = -1;
int _adrNoRespCounter = 0;
unsigned long _adrTime = 0;
AddressingState _adrState = AddressingState::None;
AssigningState _assState = AssigningState::None;
Ballast *ballasts;
bool *addresses;
int _adrFound = 0;
bool _adrAssign = false;

RTTStream rtt;

int16_t sendCmd(byte addr, byte value, byte type, bool wait = false)
{
    return Dali.sendCmdWait(addr, value, type);
}

int16_t sendCmdSpecial(int command, byte value = 0, bool wait = false)
{
    return Dali.sendSpecialCmdWait(command, value);
}

void setup()
{
	SERIAL_DEBUG.begin(115200);
	SERIAL_DEBUG.println("Setup Dali");

	Dali.begin(17,16);
}

void setup1()
{
	
}

void loop()
{
	loopAddressing();
}

void loop1()
{
	
}

void loopAddressing()
{
	switch(_adrState)
    {
        case AddressingState::Randomize_Wait:
        {
            if(millis() - _adrTime > DALI_WAIT_RANDOMIZE)
            {
                _adrState = AddressingState::Search;
                SERIAL_DEBUG.println("RandomizeWait finished");
            }
            break;
        }

        case AddressingState::Search:
        {
            if(_adrNoRespCounter == 2)
            {
                _adrState = AddressingState::Finish;
                break;
            }

            byte high = _adrHigh >> 16;
            byte middle = (_adrHigh >> 8) & 0xFF;
            byte low = _adrHigh & 0xFF;
            sendCmdSpecial(DaliClass::CMD_SEARCHADDRH, high);
            sendCmdSpecial(DaliClass::CMD_SEARCHADDRM, middle);
            sendCmdSpecial(DaliClass::CMD_SEARCHADDRL, low);
            _adrResp = sendCmdSpecial(DaliClass::CMD_COMPARE, 0x00, true);
            _adrTime = millis();
            _adrState = AddressingState::SearchWait;
            break;
        }

        case AddressingState::SearchWait:
        {
            int16_t response = _adrResp;
            
            if(response == -200 || response == -1)
            {
                if(response == -1 || millis() - _adrTime > DALI_WAIT_SEARCH)
                {
                    _adrLow = _adrHigh + 1;
                    _adrHigh = _adrHighLast;
                    _adrState = AddressingState::Search;
                    _adrNoRespCounter++;
                }
            } else if(response >= 0) {
                if(_adrLow == _adrHigh)
                {
                    SERIAL_DEBUG.print("Found Ballast at ");
					SERIAL_DEBUG.println(_adrLow);
                    if(_adrAssign)
                    {
                        uint8_t addr = 0;
                        while(addresses[addr]){
                            addr++;
                        }
                        addresses[addr] = true;
                        SERIAL_DEBUG.print("Assinging Address ");
						SERIAL_DEBUG.println(addr);
                        sendCmdSpecial(DaliClass::CMD_PROGRAMSHORT, _adrLow);
                        sendCmdSpecial(DaliClass::CMD_WITHDRAW);
                        ballasts[_adrFound].high = (_adrLow >> 16) & 0xFF;
                        ballasts[_adrFound].middle = (_adrLow >> 8) & 0xFF;
                        ballasts[_adrFound].low = _adrLow & 0xFF;
                        ballasts[_adrFound].address = response >> 1;
                        _adrFound++;

                        _adrLow = 0;
                        _adrHigh = 0xFFFFFF;
                        _adrHighLast = 0xFFFFFF;
                        _adrNoRespCounter = 1;
                        sendCmdSpecial(DaliClass::CMD_WITHDRAW);
                        _adrState = AddressingState::Search;
                    } else {
                        _adrResp = sendCmdSpecial(DaliClass::CMD_QUERY_SHORT, 0, true);
                        _adrTime = millis();
                        _adrState = AddressingState::Found;
                    }
                } else {
                    //SERIAL_DEBUG.println("Range has ballast");
                    _adrHighLast = _adrHigh;
                    _adrHigh = (_adrLow + _adrHigh) / 2;
                    _adrNoRespCounter = 0;
                    _adrState = AddressingState::Search;
                }
            } else {
                SERIAL_DEBUG.print("Dali Error ");
				SERIAL_DEBUG.print(response);
				SERIAL_DEBUG.println(" aborting addressing");
                _adrState = AddressingState::Finish;
            }
            break;
        }

        case AddressingState::Found:
        {
            int16_t response = _adrResp;
            if(response == -200 || response == -1)
            {
                if(response == -1 || millis() - _adrTime > DALI_WAIT_SEARCH)
                {
                    SERIAL_DEBUG.println("Found ballast not answering");
                    _adrState = AddressingState::Finish;
                    ballasts[_adrFound].address = 255;
                    _adrFound++;
                }
            } else if(response >= 0) {
                SERIAL_DEBUG.print("Ballast has Short Address ");
				SERIAL_DEBUG.println(response >> 1);

                ballasts[_adrFound].high = (_adrLow >> 16) & 0xFF;
                ballasts[_adrFound].middle = (_adrLow >> 8) & 0xFF;
                ballasts[_adrFound].low = _adrLow & 0xFF;
                ballasts[_adrFound].address = response >> 1;
                _adrFound++;

                _adrLow = 0;
                _adrHigh = 0xFFFFFF;
                _adrHighLast = 0xFFFFFF;
                _adrNoRespCounter = 1;
                sendCmdSpecial(DaliClass::CMD_WITHDRAW);
                _adrState = AddressingState::Search;
            } else {
                SERIAL_DEBUG.print("Dali Error ");
				SERIAL_DEBUG.print(response);
				SERIAL_DEBUG.println(", aborting addressing");
                _adrState = AddressingState::Finish;
            }
            break;
        }

        case AddressingState::Finish:
        {
            SERIAL_DEBUG.print("Found ");
			SERIAL_DEBUG.print(_adrFound);
			SERIAL_DEBUG.println(" ballasts");
            sendCmdSpecial(DaliClass::CMD_TERMINATE);
            _adrState = AddressingState::None;
            break;
        }

        case AddressingState::Query_Wait:
        {
            int16_t resp = _adrResp;

            if(resp == -1 || millis() - _adrTime > DALI_WAIT_SEARCH)
            {
                _adrState = AddressingState::Withdraw_Others;
                SERIAL_DEBUG.println("Adresse wird nicht verwendet");
                return;
            }

            if(resp == -200) return;

            if(resp >= 0)
            {
                SERIAL_DEBUG.println("Adresse wird bereits verwendet");
                _assState = AssigningState::Failed_Exists;
                _adrState = AddressingState::None;
                return;
            } else {
                SERIAL_DEBUG.print("Bus Error ");
				SERIAL_DEBUG.println(resp);
                _assState = AssigningState::Failed_Bus;
                _adrState = AddressingState::None;
                return;
            }
            break;
        }

        case AddressingState::Withdraw_Others:
        {
            sendCmdSpecial(DaliClass::CMD_INITIALISE, 0);
            sendCmdSpecial(DaliClass::CMD_INITIALISE, 0);
            if(_adrHigh > 0)
            {
                SERIAL_DEBUG.println("Verwerfe alle mit niedrigerer Long Address");
                _adrHigh--;
                byte high = _adrHigh >> 16;
                byte middle = (_adrHigh >> 8) & 0xFF;
                byte low = _adrHigh & 0xFF;
                sendCmdSpecial(DaliClass::CMD_SEARCHADDRH, high);
                sendCmdSpecial(DaliClass::CMD_SEARCHADDRM, middle);
                sendCmdSpecial(DaliClass::CMD_SEARCHADDRL, low);
                sendCmdSpecial(DaliClass::CMD_WITHDRAW);
                _adrHigh++;
            }
            byte high = _adrHigh >> 16;
            byte middle = (_adrHigh >> 8) & 0xFF;
            byte low = _adrHigh & 0xFF;
            sendCmdSpecial(DaliClass::CMD_SEARCHADDRH, high);
            sendCmdSpecial(DaliClass::CMD_SEARCHADDRM, middle);
            sendCmdSpecial(DaliClass::CMD_SEARCHADDRL, low);
            _adrResp =  sendCmdSpecial(DaliClass::CMD_COMPARE, 0, true);
            _adrTime = millis();
            _adrState = AddressingState::Check_Address;
            break;
        }

        case AddressingState::Check_Address:
        {
            if(millis() - _adrTime > DALI_WAIT_SEARCH)
            {
                SERIAL_DEBUG.println("Gerät antwortet nicht");
                _assState = AssigningState::Failed_No_Answer;
                _adrState = AddressingState::None;
                return;
            }

            int16_t resp = _adrResp;

            if(resp == -200) return;

            if(resp >= 0)
            {
                SERIAL_DEBUG.println("Long Address exists");
                SERIAL_DEBUG.print("CMD_PROGRAMSHORT to ");
				SERIAL_DEBUG.println(_adrLow);
                sendCmdSpecial(DaliClass::CMD_PROGRAMSHORT, _adrLow);
                _adrState = AddressingState::Confirm_Address;
                _adrResp = sendCmdSpecial(DaliClass::CMD_QUERY_SHORT, 0, true);
                _adrTime = millis();
                SERIAL_DEBUG.println("Frage Short Address ab");
                return;
            } else if(resp == -1) {
                SERIAL_DEBUG.println("Long Address dont exists");
                _assState = AssigningState::Failed_Exists_Not;
                _adrState = AddressingState::None;
            } else {
                SERIAL_DEBUG.print("Bus Error ");
				SERIAL_DEBUG.println(resp);
                _assState = AssigningState::Failed_Bus;
                _adrState = AddressingState::None;
            }
            break;
        }

        case AddressingState::Confirm_Address:
        {
            if(millis() - _adrTime > DALI_WAIT_SEARCH)
            {
                SERIAL_DEBUG.println("Gerät antwortet nicht");
                _assState = AssigningState::Failed_No_Answer;
                _adrState = AddressingState::None;
                return;
            }

            int16_t resp = _adrResp;

            if(resp == -200) return;

            if(resp >= 0)
            {
                SERIAL_DEBUG.print("Got resp ");
				SERIAL_DEBUG.println(resp);
                if(resp == _adrLow)
                {
                    SERIAL_DEBUG.println("Adresse erfolgreich gesetzt");
                    _assState = AssigningState::Success;
                } else {
                    SERIAL_DEBUG.println("Adresse wurde nicht übernommen");
                    _assState = AssigningState::Failed_Confirm;
                }
                _adrState = AddressingState::None;
                return;
            } else {
                SERIAL_DEBUG.print("Bus Error ");
				SERIAL_DEBUG.println(resp);
                _assState = AssigningState::Failed_Bus;
                _adrState = AddressingState::None;
                return;
            }
            break;
        }

        case AddressingState::SearchAdr:
        {
            if(_adrLow == 64)
            {
                sendCmdSpecial(DaliClass::CMD_INITIALISE, 255);
                sendCmdSpecial(DaliClass::CMD_INITIALISE, 255);
                sendCmdSpecial(DaliClass::CMD_RANDOMISE);
                sendCmdSpecial(DaliClass::CMD_RANDOMISE);
                _adrState = AddressingState::Randomize_Wait;
                _adrLow = 0;
                _adrHigh = 0xFFFFFF;
                _adrHighLast = 0xFFFFFF;
                break;
            }
                
            _adrState = AddressingState::SearchAdrWait;
            _assState = AssigningState::Working;
            _adrResp = sendCmd(_adrLow, DaliClass::CMD_QUERY_STATUS, DaliClass::DALI_SHORT_ADDRESS, true);
            _adrTime = millis();
            break;
        }
        
        case AddressingState::SearchAdrWait:
        {
            if(millis() - _adrTime > DALI_WAIT_SEARCH)
            {
                SERIAL_DEBUG.print("Address ");
				SERIAL_DEBUG.print(_adrLow);
				SERIAL_DEBUG.println("dont exists");
                _adrState = AddressingState::SearchAdr;
                _adrLow++;
                return;
            }

            int16_t resp = _adrResp;

            if(resp == -200) return;

            if(resp >= 0)
            {
                SERIAL_DEBUG.print("Address ");
				SERIAL_DEBUG.print(_adrLow);
				SERIAL_DEBUG.println("exists");
                ballasts[_adrFound].address = _adrLow;
                _adrState = AddressingState::SearchAdr;
                _adrLow++;
            } else if(resp == -1) {
                SERIAL_DEBUG.print("Address ");
				SERIAL_DEBUG.print(_adrLow);
				SERIAL_DEBUG.println(" dont exists");
                _adrState = AddressingState::SearchAdr;
                _adrLow++;
            } else {
                SERIAL_DEBUG.print("Bus Error ");
				SERIAL_DEBUG.println(resp);
                _assState = AssigningState::Failed_Bus;
                _adrState = AddressingState::None;
            }
            break;
        }
    }
}