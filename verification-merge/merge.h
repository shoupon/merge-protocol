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
#include <string>
#include <sstream>
using namespace std;

#include "../prob_verify/statemachine.h"
#include "../prob_verify/sync.h"
#include "lock.h"
#include "identifiers.h"

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
    bool isEmergency(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs);
    void abortSeq(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs);
    void cancelSeq(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs);
    MessageTuple* createOutput(MessageTuple* inMsg, int dest, int destMsg);
    SyncMessage* createSetMsg(MessageTuple* inMsg, int did);
    LockMessage* createLockMsg(MessageTuple* inMsg, string purpose, int did) ;
};

#endif 
