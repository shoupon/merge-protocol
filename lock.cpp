//
//  lock.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/22/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "lock.h"
#include "../prob_verify/statemachine.h"


Lock::Lock( Lookup* msg, Lookup* mac, int did )
: StateMachine(msg, mac), _did(did)
{
    stringstream ss;
    ss << LOCK_NAME << "(" << did << ")";
    setId(machineToInt(ss.str()));
    reset();
}

int Lock::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs, bool &high_prob,
                  int startIdx)
{
    outMsgs.clear() ;
    high_prob = true;
    string msg = IntToMessage(inMsg->destMsgId()) ;
    string src = IntToMachine(inMsg->subjectId()) ;
    
    if( startIdx != 0 )
        return -1;
    if (msg == CLOCKFAIL)
        return 3;
    switch (_state) {
        case 0:
            if( msg == "ATTEMPT" ) {
                assert(src == "merge");
                assert(typeid(*inMsg) == typeid(LockMessage));
                LockMessage *lMsg = dynamic_cast<LockMessage *>(inMsg);
                _purpose = lMsg->getBody();
                _state = 1;
                return 3;
            }
            else if (msg == DEADLINE)
                return 3;
            else
                return -1;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            if( msg == "DEADLINE") {
                int did = inMsg->getParam(1);
                if(did == _did){
                    switch (_state) {
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                            high_prob = false;
                            break;
                        case 5:
                            high_prob = true;
                            break;
                    }
                    reset();
                    return 3;
                }
                else
                    return 3;
            }
            else
                return -1;
            break ;
        default:
            assert(false);
            break;
    }
    return -1;
}

int Lock::nullInputTrans(vector<MessageTuple *> &outMsgs, bool &high_prob, int startIdx)
{
    outMsgs.clear() ;
    high_prob = true;
    
    switch (_state) {
        case 1:
            if( startIdx == 0 ) {
                outMsgs.push_back(createCoopMsg(Front));
                _state = 2 ;
                return 3;
            }
            else if (startIdx == 1) {
                outMsgs.push_back(createCoopMsg(Back));
                _state = 3;
                return 3;
            }
            else
                return -1;
            break;
        case 2:
            if (startIdx == 0) {
                outMsgs.push_back(createCoopMsg(Back));
                _state = 4;
                return 3;
            }
            else
                return -1;
            break;
        case 3:
            if (startIdx == 0) {
                outMsgs.push_back(createCoopMsg(Front));
                _state = 4;
                return 3;
            }
            else
                return -1;
            break;
        case 4:
            if (startIdx == 0) {
                outMsgs.push_back(createSuccMsg());
                _state = 5;
                return 3;
            }
            else
                return -1;
            break;
        default:
            return -1;
            break;
    }
}

void Lock::restore(const StateSnapshot *snapshot)
{
    assert(typeid(*snapshot) == typeid(LockSnapshot)) ;
    const LockSnapshot *ls = dynamic_cast<const LockSnapshot*>(snapshot) ;
    _did = ls->_ss_did;
    _purpose = ls->_ss_purpose ;
    _state = ls->_ss_state ;
}

StateSnapshot* Lock::curState()
{
    return new LockSnapshot(_state, _did, _purpose);
}


MessageTuple* Lock::createCoopMsg(Role r)
{
    int destId ;
    switch (r) {
        case Merge:
            assert(false);
            break;
        case Front:
            destId = machineToInt("front");
            break;
        case Back:
            destId = machineToInt("back");
            break;
        default:
            assert(false);
            break;
    }
    LockMessage* msg = new LockMessage(0, destId,
                                       0, messageToInt("COOPERATE"),
                                       macId(), _purpose);
    return msg;
}

MessageTuple* Lock::createSuccMsg()
{
    int destId = machineToInt("merge");
    LockMessage* msg = new LockMessage(0, destId,
                                       0, messageToInt("SUCCESS"),
                                       macId(), _purpose);
    return msg;
}

string LockMessage::toString()
{
    stringstream ss;
    ss << MessageTuple::toString() << "(" << _body << ")" ;
    return ss.str();
}

LockMessage* LockMessage::clone() const
{
    return new LockMessage(*this);
}

string LockSnapshot::toString()
{
    stringstream ss;
    ss << _ss_state << "," << _ss_did << "(" << _ss_purpose << ")" ;
    return ss.str();
}