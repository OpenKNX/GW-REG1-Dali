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
	rtt.begin(115200);
	rtt.println("Setup Dali");

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
                rtt.println("RandomizeWait finished");
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
                    rtt.print("Found Ballast at ");
					rtt.println(_adrLow);
                    if(_adrAssign)
                    {
                        uint8_t addr = 0;
                        while(addresses[addr]){
                            addr++;
                        }
                        addresses[addr] = true;
                        rtt.print("Assinging Address ");
						rtt.println(addr);
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
                    //rtt.println("Range has ballast");
                    _adrHighLast = _adrHigh;
                    _adrHigh = (_adrLow + _adrHigh) / 2;
                    _adrNoRespCounter = 0;
                    _adrState = AddressingState::Search;
                }
            } else {
                rtt.print("Dali Error ");
				rtt.print(response);
				rtt.println(" aborting addressing");
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
                    rtt.println("Found ballast not answering");
                    _adrState = AddressingState::Finish;
                    ballasts[_adrFound].address = 255;
                    _adrFound++;
                }
            } else if(response >= 0) {
                rtt.print("Ballast has Short Address ");
				rtt.println(response >> 1);

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
                rtt.print("Dali Error ");
				rtt.print(response);
				rtt.println(", aborting addressing");
                _adrState = AddressingState::Finish;
            }
            break;
        }

        case AddressingState::Finish:
        {
            rtt.print("Found ");
			rtt.print(_adrFound);
			rtt.println(" ballasts");
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
                rtt.println("Adresse wird nicht verwendet");
                return;
            }

            if(resp == -200) return;

            if(resp >= 0)
            {
                rtt.println("Adresse wird bereits verwendet");
                _assState = AssigningState::Failed_Exists;
                _adrState = AddressingState::None;
                return;
            } else {
                rtt.print("Bus Error ");
				rtt.println(resp);
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
                rtt.println("Verwerfe alle mit niedrigerer Long Address");
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
                rtt.println("Gerät antwortet nicht");
                _assState = AssigningState::Failed_No_Answer;
                _adrState = AddressingState::None;
                return;
            }

            int16_t resp = _adrResp;

            if(resp == -200) return;

            if(resp >= 0)
            {
                rtt.println("Long Address exists");
                rtt.print("CMD_PROGRAMSHORT to ");
				rtt.println(_adrLow);
                sendCmdSpecial(DaliClass::CMD_PROGRAMSHORT, _adrLow);
                _adrState = AddressingState::Confirm_Address;
                _adrResp = sendCmdSpecial(DaliClass::CMD_QUERY_SHORT, 0, true);
                _adrTime = millis();
                rtt.println("Frage Short Address ab");
                return;
            } else if(resp == -1) {
                rtt.println("Long Address dont exists");
                _assState = AssigningState::Failed_Exists_Not;
                _adrState = AddressingState::None;
            } else {
                rtt.print("Bus Error ");
				rtt.println(resp);
                _assState = AssigningState::Failed_Bus;
                _adrState = AddressingState::None;
            }
            break;
        }

        case AddressingState::Confirm_Address:
        {
            if(millis() - _adrTime > DALI_WAIT_SEARCH)
            {
                rtt.println("Gerät antwortet nicht");
                _assState = AssigningState::Failed_No_Answer;
                _adrState = AddressingState::None;
                return;
            }

            int16_t resp = _adrResp;

            if(resp == -200) return;

            if(resp >= 0)
            {
                rtt.print("Got resp ");
				rtt.println(resp);
                if(resp == _adrLow)
                {
                    rtt.println("Adresse erfolgreich gesetzt");
                    _assState = AssigningState::Success;
                } else {
                    rtt.println("Adresse wurde nicht übernommen");
                    _assState = AssigningState::Failed_Confirm;
                }
                _adrState = AddressingState::None;
                return;
            } else {
                rtt.print("Bus Error ");
				rtt.println(resp);
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
                rtt.print("Address ");
				rtt.print(_adrLow);
				rtt.println("dont exists");
                _adrState = AddressingState::SearchAdr;
                _adrLow++;
                return;
            }

            int16_t resp = _adrResp;

            if(resp == -200) return;

            if(resp >= 0)
            {
                rtt.print("Address ");
				rtt.print(_adrLow);
				rtt.println("exists");
                ballasts[_adrFound].address = _adrLow;
                _adrState = AddressingState::SearchAdr;
                _adrLow++;
            } else if(resp == -1) {
                rtt.print("Address ");
				rtt.print(_adrLow);
				rtt.println(" dont exists");
                _adrState = AddressingState::SearchAdr;
                _adrLow++;
            } else {
                rtt.print("Bus Error ");
				rtt.println(resp);
                _assState = AssigningState::Failed_Bus;
                _adrState = AddressingState::None;
            }
            break;
        }
    }
}