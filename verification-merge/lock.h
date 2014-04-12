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

enum Role {Merge, Front, Back};

class Lock: public StateMachine
{
public:
    Lock( Lookup* msg, Lookup* mac, int did );
    ~Lock() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                bool& high_prob, int startIdx = 0);
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
                       bool& high_prob, int startIdx = 0);
private:
    string _purpose;
    int _did; // deadline number
    MessageTuple* createCoopMsg(Role r);
    MessageTuple* createSuccMsg();   // default role: Merge
};

class LockMessage : public MessageTuple
{
public:
    // Constructor: src, dest, srcMsg, destMsg, subject all retain the same implication as
    // is ancestor, MessageTuple;
    // k: the ID of the source lock
    // lock: the ID of the destination competitor. (exception: when the message is
    // "complete", which is to notify the controller that the lock is released, the field
    // is used to tell the controller which competitor this lock was associated to
    LockMessage(int src, int dest, int srcMsg, int destMsg, int subject, string body)
    :MessageTuple(src, dest, srcMsg, destMsg, subject), _body(body){}
    
    LockMessage( const LockMessage& msg )
    :MessageTuple(msg._src, msg._dest, msg._srcMsg, msg._destMsg, msg._subject)
    , _body(msg._body) {}
    
    LockMessage(int src, int dest, int srcMsg, int destMsg, int subject,
                const LockMessage& msg)
    :MessageTuple(src, dest, srcMsg, destMsg, subject)
    , _body(msg._body) {}
    
    ~LockMessage() {}
    
    size_t numParams() {return 1; }
    int getParam(size_t arg) { return 0; }
    string toString() ;
    LockMessage* clone() const ;
    
    string getBody() { return _body; }
private:
    const string _body;
};


#endif /* defined(LOCK_H) */
