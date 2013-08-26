//
//  merge.h
//  verification-merge
//
//  Created by Shou-pon Lin on 8/22/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#ifndef MERGE_H
#define MERGE_H

#include <iostream>

#include "statemachine.h"

class MergeSnapshot;

class Merge: public StateMachine
{
public:
    Merge( Lookup* msg, Lookup* mac );
    ~Merge() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                bool& high_prob, int startIdx = 0);
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
                       bool& high_prob, int startIdx = 0);        
private:
    MessageTuple* createOutput(MessageTuple* inMsg, int dest, int destMsg);
};

#endif 
