//
//  trbp.h
//  verification-merge
//
//  Created by Shou-pon Lin on 4/11/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//


#ifndef TRBP_H
#define TRBP_H

#include <iostream>
#include <string>

#include "../prob_verify/statemachine.h"
#include "../prob_verify/sync.h"
#include "identifiers.h"
#include "lock.h"

class TRBP: public StateMachine
{
public:
    TRBP( Lookup* msg, Lookup* mac );
    ~TRBP() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                bool& high_prob, int startIdx = 0) ;
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
                       bool& high_prob, int startIdx = 0) ;
    
private:
    MessageTuple* createMsg(MessageTuple* inMsg, const string& dest, string msg);
    MessageTuple* loss(const string& dest);
};

#endif /* defined(TRBP_H) */
