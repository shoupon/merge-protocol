//
//  cruise.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/26/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include <sstream>

#include "cruise.h"

Cruise::Cruise( Lookup* msg, Lookup* mac, string name, string abbv, string inst)
:StateMachine(msg, mac), _name(name), _abbv(abbv), _instruction(inst)
{
    stringstream ss;
    ss << "cruise(" << abbv << ")" ;
    setId(machineToInt(ss.str()));
    reset();
}

int Cruise::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs, bool &high_prob,
                    int startIdx)
{
    outMsgs.clear();
    high_prob = true;
    string msg = IntToMessage(inMsg->destMsgId());
    string src = IntToMachine(inMsg->subjectId());
    
    if( startIdx != 0 )
        return -1;
    
    switch (_state) {
        case 0:
            if( msg == _instruction ) {
                assert(src == _name);
                _state = 1;
                return 3;
            }
            else
                return -1;
            break;
        case 1:
            if( msg == "RESET" ) {
                assert(src == _name) ;
                _state = 0 ;
                return 3;
            }
            else if( msg == "PILOT" ) {
                assert(src == _name);
                _state = 2;
                return 3;
            }
            else
                return -1;
            break;
        default:
            return -1;
            break;
    }
}

int Cruise::nullInputTrans(vector<MessageTuple *> &outMsgs, bool &high_prob, int startIdx)
{
    outMsgs.clear();
    high_prob = true ;
    if( startIdx != 0 )
        return -1;
    
    switch (_state) {
        case 2:
            // models the driver deactivating collision avoidance mode
            _state = 0;
            outMsgs.push_back(new MessageTuple(0, machineToInt(_name),
                                               0, messageToInt("DISENGAGE"), macId()));
            return 3;
            break;
            
        default:
            return -1;
            break;
    }
}