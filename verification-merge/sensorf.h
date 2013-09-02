//
//  sensorf.h
//  verification-merge
//
//  Created by Shou-pon Lin on 8/26/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#ifndef SENSOR_F_H
#define SENSOR_F_H

#include <iostream>

#include "../prob_verify/statemachine.h"

class SensorF: public StateMachine
{
public:
    SensorF( Lookup* msg, Lookup* mac );
    ~SensorF() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                bool& high_prob, int startIdx = 0);
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
                       bool& high_prob, int startIdx = 0);
};

#endif /* defined(SENSOR_F_H) */