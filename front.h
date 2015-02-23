//
//  front.h
//  verification-merge
//
//  Created by Shou-pon Lin on 8/26/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#ifndef FRONT_H
#define FRONT_H

#include <iostream>
#include <string>

#include "../prob_verify/pverify.h"
#include "../prob_verify/statemachine.h"
#include "../prob_verify/sync.h"
#include "identifiers.h"

class Front: public StateMachine
{
public:
    Front( Lookup* msg, Lookup* mac );
    ~Front() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                bool& high_prob, int startIdx = 0);
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
        bool& high_prob, int startIdx = 0) {return -1;}
private:
    bool isEmergency(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs);
};

#endif /* defined(FRONT_H) */
