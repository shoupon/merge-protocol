//
//  sensorf.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/26/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "sensorf.h"

SensorF::SensorF(Lookup* msg, Lookup* mac ):StateMachine(msg, mac)
{
    setId(machineToInt("sensor(f)"));
    reset();
}

int SensorF::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs, bool &high_prob,
                     int startIdx)
{
    if( startIdx == 0 )
        return 3;
    else
        return -1;
}

int SensorF::nullInputTrans(vector<MessageTuple *> &outMsgs, bool &high_prob, int startIdx)
{
    if( startIdx == 0 && _state == 0 ) {
        MessageTuple* msg = new MessageTuple(0, machineToInt("front"),
                                             0, messageToInt("EMERGENCY"), macId());
        outMsgs.push_back(msg);
        high_prob = false ;
        _state = 1;
        return 3;
    }
    else
        return -1;
}