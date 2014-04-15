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
using namespace std;

#include "../prob_verify/statemachine.h"
#include "identifiers.h"


class LockSnapshot;

class Lock: public StateMachine
{
    enum Role {Merge, Front, Back};
public:
    Lock( Lookup* msg, Lookup* mac, int did );
    ~Lock() {}
    int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                bool& high_prob, int startIdx = 0);
    int nullInputTrans(vector<MessageTuple*>& outMsgs,
                       bool& high_prob, int startIdx = 0);
    void restore(const StateSnapshot* snapshot);
    StateSnapshot* curState();
    void reset() { _purpose = "" ; _state = 0; }
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

class LockSnapshot : public StateSnapshot
{
public:
    friend class Lock;
    
    LockSnapshot(int state, int did, string purpose)
    :StateSnapshot(state), _ss_did(did), _ss_purpose(purpose) {}
    
    ~LockSnapshot() {}
    // Returns the name of current state as specified in the input file
    string toString() ;
    // Cast the Snapshot into a integer. Used in HashTable
    int toInt() { return (_ss_state << 8) + (_ss_did << 4) + _ss_purpose.c_str()[0]; }
    LockSnapshot* clone() const { return new LockSnapshot(_ss_state, _ss_did, _ss_purpose); }
    
private:
    string _ss_purpose;
    int _ss_did;
};


#endif /* defined(LOCK_H) */
