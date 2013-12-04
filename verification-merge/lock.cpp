//
//  lock.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/22/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "lock.h"
#include "../prob_verify/statemachine.h"


Lock::Lock( Lookup* msg, Lookup* mac ): StateMachine(msg, mac)
{
    setId(machineToInt("lock"));
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
    
    switch (_state) {
        case 0:
            if( msg == "ATTEMPT" ) {
                assert(src == "merge");
                _state = 1;
                return 3;
            }
            else
                return 3;
            break;
        case 1:
            if( msg == "UNLOCK" ) {
                assert(src == "merge");
                //_state = 0 ;
                return 3;
            }
            else if( msg == "DEADLINE") {
                int did = inMsg->getParam(1);
                if(did == 3){
                    outMsgs.push_back(createFreeMsg(inMsg, Lock::Merge));
                    _state = 0 ;
                    high_prob = false;
                    return 3;
                }
                else
                    return 3;
            }
            return 3;
            break ;
        case 2:
            if( msg == "DEADLINE" ) {
                int did = inMsg->getParam(1);
                if( did == 3 ) {
                    outMsgs.push_back(createFreeMsg(inMsg, Lock::Front));
                    outMsgs.push_back(createFreeMsg(inMsg, Lock::Merge));
                    _state = 0 ;
                    high_prob = false;
                    return 3;
                }
                else
                    return 3;
            }
            else if( msg == "UNLOCK" ) {
                assert(src == "merge");
                /*
                MessageTuple* msg2front = new MessageTuple(inMsg->srcID(),
                                                           machineToInt("front"),
                                                           inMsg->srcMsgId(),
                                                           messageToInt("FREE"),
                                                           macId()) ;
                MessageTuple* msg2back = new MessageTuple(inMsg->srcID(),
                                                          machineToInt("back"),
                                                          inMsg->srcMsgId(),
                                                          messageToInt("FREE"),
                                                          macId()) ;
                outMsgs.push_back(msg2front);
                outMsgs.push_back(msg2back);
                _state = 0 ;*/
                return 3;
            }
            else
                return 3;
            break ;
        case 3:
            if (msg == "DEADLINE") {
                int did = inMsg->getParam(1);
                if (did == 3) {
                    outMsgs.push_back(createFreeMsg(inMsg, Lock::Merge));
                    outMsgs.push_back(createFreeMsg(inMsg, Lock::Back));
                    _state = 0;
                    high_prob = false;
                    return 3;
                }
                else
                    return 3;
            }
            else
                return 3;
            break;
        case 4:
            if (msg == "DEADLINE") {
                int did = inMsg->getParam(1);
                if (did == 3) {
                    outMsgs.push_back(createFreeMsg(inMsg, Lock::Merge));
                    outMsgs.push_back(createFreeMsg(inMsg, Lock::Front));
                    outMsgs.push_back(createFreeMsg(inMsg, Lock::Back));
                    _state = 0;
                    high_prob = false;
                    return 3;
                }
                else
                    return 3;
            }
            else
                return 3;
        case 5:
            if (msg == "DEADLINE") {
                int did = inMsg->getParam(1);
                if (did == 3) {
                    outMsgs.push_back(createFreeMsg(inMsg, Lock::Merge));
                    outMsgs.push_back(createFreeMsg(inMsg, Lock::Front));
                    outMsgs.push_back(createFreeMsg(inMsg, Lock::Back));
                    _state = 0;
                    high_prob = false;
                    return 3;
                }
                else
                    return 3;
            }
            break;
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

                MessageTuple* msg2front = new MessageTuple(0, machineToInt("front"),
                                                           0, messageToInt("COOPERATE"),
                                                           macId()) ;
                outMsgs.push_back(msg2front);
                _state = 2 ;
                return 3;
            }
            else if (startIdx == 1) {
                MessageTuple* msg2back = new MessageTuple(0, machineToInt("back"),
                                                          0, messageToInt("COOPERATE"),
                                                          macId()) ;
                outMsgs.push_back(msg2back);
                _state = 3;
                return 3;
            }
            else
                return -1;
            break;
        case 2:
            if (startIdx == 0) {
                MessageTuple* msg2back = new MessageTuple(0, machineToInt("back"),
                                                          0, messageToInt("COOPERATE"),
                                                          macId()) ;
                outMsgs.push_back(msg2back);
                _state = 4;
                return 3;
            }
            else
                return -1;
            break;
        case 3:
            if (startIdx == 0) {
                MessageTuple* msg2front = new MessageTuple(0, machineToInt("front"),
                                                           0, messageToInt("COOPERATE"),
                                                           macId()) ;
                outMsgs.push_back(msg2front);
                _state = 4;
                return 3;
            }
            else
                return -1;
            break;
        case 4:
            if (startIdx == 0) {
                MessageTuple* msg2merge = new MessageTuple(0, machineToInt("merge"),
                                                           0, messageToInt("SUCCESS"),
                                                           macId()) ;
                outMsgs.push_back(msg2merge);
                _state = 5;
                return 3;
            }
            else
                return -1;
            
        default:
            return -1;
            break;
    }
}

MessageTuple* Lock::createFreeMsg(MessageTuple *inMsg, Lock::Role r)
{
    int destId ;
    switch (r) {
        case Merge:
            destId = machineToInt("merge");
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
    
    MessageTuple* msg = new MessageTuple(inMsg->srcID(),
                                         destId,
                                         inMsg->srcMsgId(),
                                         messageToInt("FREE"), macId()) ;
    return msg;
}