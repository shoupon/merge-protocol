//
//  lock.h
//  verification-merge
//
//  Created by Shou-pon Lin on 8/22/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#ifndef LOCK_H
#define LOCK_H

#include <iostream>
#include <sstream>

#include "merge.h"
#include "../prob_verify/statemachine.h"


class LockSnapshot;

class Lock: public StateMachine
{
public:
    Lock( Lookup* msg, Lookup* mac );
    ~Lock() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                bool& high_prob, int startIdx = 0);
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
                       bool& high_prob, int startIdx = 0);
};

#endif /* defined(LOCK_H) */
