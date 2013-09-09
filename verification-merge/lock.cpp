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
                _state = 0 ;
                return 3;
            }
            else if( msg == "DEADLINE") {
                int did = inMsg->getParam(1);
                if(did != 3){
                    return 3;
                }
                MessageTuple* msg2merge = new MessageTuple(inMsg->srcID(),
                                                           machineToInt("merge"),
                                                           inMsg->srcMsgId(),
                                                           messageToInt("FREE"),
                                                           macId());
                outMsgs.push_back(msg2merge);
                _state = 0 ;
                high_prob = false;
                return 3;
            }
            return 3;
            break ;
        case 2:
            if( msg == "DEADLINE" ) {
                int did = inMsg->getParam(1);
                if( did == 3 ) {
                    MessageTuple* msg2merge = new MessageTuple(inMsg->srcID(),
                                                               machineToInt("merge"),
                                                               inMsg->srcMsgId(),
                                                               messageToInt("FREE"),
                                                               macId()) ;
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
                    outMsgs.push_back(msg2merge) ;
                    outMsgs.push_back(msg2front);
                    outMsgs.push_back(msg2back);
                    _state = 0 ;
                    return 3;
                }
                else
                    return 3;
            }
            else if( msg == "UNLOCK" ) {
                assert(src == "merge");
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
                _state = 0 ;
                return 3;
            }
            else
                return 3;
                break ;
        default:
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
                MessageTuple* msg2merge = new MessageTuple(0, machineToInt("merge"),
                                                           0, messageToInt("SUCCESS"),
                                                           macId()) ;
                MessageTuple* msg2front = new MessageTuple(0, machineToInt("front"),
                                                           0, messageToInt("COOPERATE"),
                                                           macId()) ;
                MessageTuple* msg2back = new MessageTuple(0, machineToInt("back"),
                                                           0, messageToInt("COOPERATE"),
                                                           macId()) ;
                outMsgs.push_back(msg2merge);
                outMsgs.push_back(msg2front);
                outMsgs.push_back(msg2back);
                _state = 2 ;
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