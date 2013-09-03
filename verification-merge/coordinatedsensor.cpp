//
//  coordinatedsensor.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/23/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "coordinatedsensor.h"

CoordinatedSensor::CoordinatedSensor(Lookup* msg, Lookup* mac ):StateMachine(msg, mac)
{
    setId(machineToInt("coordsensor"));
    reset();
}

int CoordinatedSensor::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs,
                               bool &high_prob, int startIdx)
{
    outMsgs.clear();
    high_prob = true;
    string msg = IntToMessage(inMsg->destMsgId());
    string src = IntToMachine(inMsg->subjectId());
    
    if( startIdx != 0 )
        return -1;
    
    switch (_state) {
        case 0:
            if( msg == "MONITOR" ) {
                assert(src == "merge");
                _state = 1;
            }
            return 3;
            break;
        case 1:
            if( msg == "STOP") {
                assert(src == "merge") ;
                _state = 0 ;
            }
            return 3;
            break;
        case 2:
            if( msg == "COMPLETE" ) {
                assert(src == "driver");
                _state = 0 ;
            }
            else if( msg == "STOP") {
                assert(src == "merge") ;
                _state = 0 ;
            }
            return 3;
            break;
            
        default:
            return -1;
            break;
    }
    return -1;
}

int CoordinatedSensor::nullInputTrans(vector<MessageTuple *> &outMsgs, bool &high_prob,
                                      int startIdx)
{
    outMsgs.clear();
    high_prob = true;
    
    switch (_state) {
        case 0:
            return -1;
            break;
        case 1:
            if( startIdx == 0 ) {
                broadcast(outMsgs, "READY");
                _state = 2 ;
                return 1;
            }
            else if( startIdx == 1 ) {
                broadcast(outMsgs, "GAPTAKEN");
                high_prob = false ;
                _state = 0;
                return 2;
            }
            else
                return -1;
            break;
        case 2:
            if( startIdx == 0 ) {
                broadcast(outMsgs, "GAPTAKEN");
                _state = 0;
                high_prob = false;
                return 3;
            }
            else
                return -1;
            break;
        default:
            return -1;
            break;
    }
    return -1;
}

void CoordinatedSensor::broadcast(vector<MessageTuple *> &outMsgs, string msg)
{
    MessageTuple* msg2merge = new MessageTuple(0, machineToInt("merge"),
                                               0, messageToInt(msg),
                                               macId());
    MessageTuple* msg2front = new MessageTuple(0, machineToInt("front"),
                                               0, messageToInt(msg),
                                               macId());
    MessageTuple* msg2back = new MessageTuple(0, machineToInt("back"),
                                              0, messageToInt(msg),
                                              macId());
    outMsgs.push_back(msg2merge);
    outMsgs.push_back(msg2front);
    outMsgs.push_back(msg2back);
}