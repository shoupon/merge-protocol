//
//  driver.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/27/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "driver.h"

Driver::Driver( Lookup* msg, Lookup* mac)
:StateMachine(msg, mac)
{
    setId(machineToInt(DRIVER_NAME));
    reset();
}

int Driver::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs,
                    bool &high_prob, int startIdx)
{
    outMsgs.clear();
    high_prob = true ;
    
    if( startIdx != 0 )
        return -1;
    
    string msg = IntToMessage(inMsg->destMsgId());
    string src = IntToMachine(inMsg->subjectId());
    if (msg == CLOCKFAIL) {
        _state = 4;
        return 3;
    }
    switch (_state) {
        case 0:
            if( msg == DEADLINE )
                return 3;
            else
                return -1;
        case 1:
            if( msg == ABORT ) {
                assert(src == MERGE_NAME);
                _state = 0 ;
                return 3;
            }
            else if( msg == GREENLIGHT ) {
                assert(src == MERGE_NAME);
                _state = 2;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else
                return 3;
            break;
        case 2:
            if( msg == ABORT ) {
                assert(src == MERGE_NAME);
                _state = 0 ;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else
                return 3;
        case 4:
            return 3;
        default:
            return -1;
            break;
    }
    
    return -1;
}

int Driver::nullInputTrans(vector<MessageTuple *> &outMsgs, bool &high_prob, int startIdx)
{
    outMsgs.clear();
    high_prob = true;
    
    if( startIdx != 0 )
        return -1;
    
    switch (_state) {
        case 0:
            outMsgs.push_back(new MessageTuple(0, machineToInt(MERGE_NAME),
                                               0, messageToInt(SIGNAL), macId()));
            outMsgs.push_back(new MessageTuple(0, machineToInt(CRUISE_MERGE_NAME),
                                               0, messageToInt(REQUIRE), macId()));
            _state = 1;
            return 3;
            break;
        case 1:
            high_prob = false ;
            outMsgs.push_back(new MessageTuple(0, machineToInt(MERGE_NAME),
                                               0, messageToInt(CANCEL), macId()));
            _state = 0 ;
            return 3;
            break;
        case 2:
            outMsgs.push_back(new MessageTuple(0, machineToInt(MERGE_NAME),
                                               0, messageToInt(FINISH), macId()));
            _state = 0 ;
            return 3;
            break;
        default:
            return -1;
            break;
    }
    return -1;
}