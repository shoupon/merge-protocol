//
//  cruise.h
//  verification-merge
//
//  Created by Shou-pon Lin on 8/26/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#ifndef CRUISE_H
#define CRUISE_H

#include <iostream>

#include "statemachine.h"

class Cruise: public StateMachine
{
public:
    Cruise( Lookup* msg, Lookup* mac, string name, string abbv, string inst);
    ~Cruise() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                bool& high_prob, int startIdx = 0);
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
                       bool& high_prob, int startIdx = 0) {return -1;}
private:
    string _name;
    string _abbv;
    string _instruction;
};

#endif /* defined(CRUISE_H) */
