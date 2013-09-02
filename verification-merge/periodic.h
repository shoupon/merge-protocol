//
//  periodic.h
//  verification-merge
//
//  Created by Shou-pon Lin on 8/23/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#ifndef PERIODIC_H
#define PERIODIC_H

#include <iostream>

#include "statemachine.h"
#include "sync.h"

class PeriodicSnapshot;

class Periodic: public StateMachine
{
public:
    Periodic( Lookup* msg, Lookup* mac );
    ~Periodic() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                        bool& high_prob, int startIdx = 0) ;
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
                       bool& high_prob, int startIdx = 0) {return -1;}
    
private:
    MessageTuple* toMerge(MessageTuple* inMsg, string msg);
    MessageTuple* toFront(MessageTuple* inMsg, string msg);
    MessageTuple* toBack(MessageTuple* inMsg, string msg);
    MessageTuple* createMsg(MessageTuple* inMsg, string dest, string msg);
};

#endif /* defined(PERIODIC_H) */
