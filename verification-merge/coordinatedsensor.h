//
//  coordinatedsensor.h
//  verification-merge
//
//  Created by Shou-pon Lin on 8/23/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#ifndef COORD_SENSOR_H
#define COORD_SENSOR_H

#include <iostream>

#include "statemachine.h"

class CoordinatedSensorSnapshot;

class CoordinatedSensor: public StateMachine
{
public:
    CoordinatedSensor( Lookup* msg, Lookup* mac );    ~CoordinatedSensor() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                        bool& high_prob, int startIdx = 0);
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
                               bool& high_prob, int startIdx = 0);
    
private:
    void broadcast(vector<MessageTuple*>& outMsgs, string msg);
};

#endif /* defined(COORD_SENSOR_H) */
