//
//  sensor.h
//  verification-merge
//
//  Created by Shou-pon Lin on 5/1/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#ifndef SENSOR_H
#define SENSOR_H

#include <iostream>
#include <sstream>
using namespace std;

#include "../prob_verify/pverify.h"
#include "../prob_verify/statemachine.h"
#include "../prob_verify/sync.h"
#include "identifiers.h"
#include "merge.h"
#include "front.h"
#include "back.h"

class Front;
class Back;
class Merge;

class Sensor: public StateMachine
{
public:
    Sensor( Lookup* msg, Lookup* mac );
    ~Sensor() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                bool& high_prob, int startIdx = 0);
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
                       int& prob_level, int startIdx = 0);
private:
    MessageTuple* createMsg(MessageTuple* inMsg, const string& dest, string msg);
    MessageTuple* emergency(const string& dest);
    MessageTuple* gapTaken(const string& dest);
    MessageTuple* inconsistent(const string& dest);
};

#endif // (SENSOR_H)
