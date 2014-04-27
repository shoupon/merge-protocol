//
//  back.h
//  verification-merge
//
//  Created by Shou-pon Lin on 8/26/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#ifndef BACK_H
#define BACK_H

#include <iostream>

#include "../prob_verify/statemachine.h"
#include "../prob_verify/sync.h"
#include "identifiers.h"

class Back: public StateMachine
{
public:
    Back( Lookup* msg, Lookup* mac );
    ~Back() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                bool& high_prob, int startIdx = 0);
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
                       bool& high_prob, int startIdx = 0);
private:
    bool isEmergency(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs);
};

#endif /* defined(BACK_H) */

