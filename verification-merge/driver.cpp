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
    setId(machineToInt("driver"));
    reset();
}

int Driver::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs, bool &high_prob,
                    int startIdx)
{
    outMsgs.clear();
    high_prob = true ;
    
    if( startIdx != 0 )
        return -1;
    
    string msg = IntToMessage(inMsg->destMsgId());
    string src = IntToMachine(inMsg->subjectId());
    switch (_state) {
        case 0:
            return 3;
        case 1:
            if( msg == "ABORT" ) {
                assert(src == "merge");
                _state = 0 ;
                return 3;
            }
            else if( msg == "CLEARTOMOVE" ) {
                assert(src == "merge");
                _state = 2;
                return 3;
            }
            else
                return 3;
            break;
        case 2:
            if( msg == "ABORT" ) {
                assert(src == "merge");
                _state = 0 ;
                return 3;
            }
            else
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
            outMsgs.push_back(new MessageTuple(0, machineToInt("merge"),
                                               0, messageToInt("SIGNAL"), macId()));
            _state = 1;
            return 3;
            break;
        case 1:
            high_prob = false ;
            outMsgs.push_back(new MessageTuple(0, machineToInt("merge"),
                                               0, messageToInt("CANCEL"), macId()));
            _state = 0 ;
            return 3;
            break;
        case 2:
            outMsgs.push_back(new MessageTuple(0, machineToInt("driver"),
                                               0, messageToInt("COMPLETE"), macId()));
            _state = 0 ;
            return 3;
            break;
        default:
            return -1;
            break;
    }
    return -1;
}