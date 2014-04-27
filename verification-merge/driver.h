//
//  driver.h
//  verification-merge
//
//  Created by Shou-pon Lin on 8/27/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#ifndef DRIVER_H
#define DRIVER_H

#include <iostream>

#include "../prob_verify/statemachine.h"
#include "../prob_verify/sync.h"
#include "identifiers.h"

class Driver: public StateMachine
{
public:
    Driver( Lookup* msg, Lookup* mac);
    ~Driver() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                bool& high_prob, int startIdx = 0);
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
                       bool& high_prob, int startIdx = 0) ;
};

#endif /* defined(DRIVER_H) */
