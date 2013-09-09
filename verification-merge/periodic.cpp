//
//  periodic.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/23/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "periodic.h"
#include "../prob_verify/sync.h"

Periodic::Periodic( Lookup* msg, Lookup* mac ):StateMachine(msg, mac)
{
    setId(machineToInt("periodic")) ;
    reset();
}

int Periodic::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs, bool &high_prob,
                      int startIdx)
{
    outMsgs.clear();
    high_prob = true;
    string msg = IntToMessage(inMsg->destMsgId());
    string src = IntToMachine(inMsg->subjectId());
    
    if( startIdx != 0 )
        return -1;
    switch (_state) {
        case 0:
            if( msg == "START" ) {
                assert(src == "merge");
                outMsgs.push_back(toMerge(inMsg, "ENGAGE"));
                outMsgs.push_back(toFront(inMsg, "ENGAGE"));
                outMsgs.push_back(toBack(inMsg, "ENGAGE"));
                _state = 1;
                return 3;
            }
            else
                return 3;
            break;
        case 1:
            if( msg == "DISENGAGE" ) {
                assert(src == "front") ;
                outMsgs.push_back(toMerge(inMsg, "STOP"));
                outMsgs.push_back(toFront(inMsg, "STOP"));
                outMsgs.push_back(toBack(inMsg, "STOP"));
                _state = 0 ;
                return 3;
            }
            else if( msg == "END" ) {
                assert(src == "merge");
                outMsgs.push_back(toFront(inMsg, "STOP"));
                outMsgs.push_back(toBack(inMsg, "STOP"));
                _state = 0;
                return 3;
            }
            else
                return 3;
            break;
        default:
            return -1;
            break;
    }
}

MessageTuple* Periodic::toMerge(MessageTuple *inMsg, string msg)
{
    return createMsg(inMsg, "merge", msg);
}

MessageTuple* Periodic::toFront(MessageTuple* inMsg, string msg)
{
    return createMsg(inMsg, "front", msg);
}

MessageTuple* Periodic::toBack(MessageTuple *inMsg, string msg)
{
    return createMsg(inMsg, "back", msg);
}

MessageTuple* Periodic::createMsg(MessageTuple *inMsg, string dest, string msg)
{
    return new MessageTuple(inMsg->srcID(), machineToInt(dest),
                            inMsg->destId(), messageToInt(msg), macId()) ;
}