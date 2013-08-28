//
//  periodic.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/23/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "periodic.h"
#include "sync.h"

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
    
    if( startIdx >= 3 )
        return -1;
    switch (_state) {
        case 0:
            if( msg == "START" ) {
                assert(src == "merge");
                SyncMessage* dm = new SyncMessage(inMsg->srcID(),
                                                  machineToInt("sync"),
                                                  inMsg->srcMsgId(),
                                                  messageToInt("SET"),
                                                  macId(), true, 1);
                outMsgs.push_back(dm) ;
                _state = 1;
                return 3;
            }
            else
                return 3;
            break;
        case 1:
            if( msg == "DEADLINE" ) {
                int did = inMsg->getParam(1) ;
                if( did != 0 )
                    return 3;
                
                if( startIdx == 0 ) {
                    outMsgs.push_back(toMerge(inMsg, "ENGAGE"));
                    outMsgs.push_back(toFront(inMsg, "ENGAGE"));
                    outMsgs.push_back(toBack(inMsg, "ENGAGE"));
                    _state = 2;
                    return 1;
                }
                else if( startIdx == 1 ) {
                    outMsgs.push_back(toMerge(inMsg, "STOP")) ;
                    high_prob = false;
                    _state = 0 ;
                    return 3;
                }
                else
                    return -1;
            }
            else
                return 3;
            break;
        case 2:
            if( msg == "END" ) {
                assert(src == "merge");
                outMsgs.push_back(toFront(inMsg, "STOP"));
                outMsgs.push_back(toBack(inMsg, "STOP"));
                _state = 0 ;
                return 3;
            }
            else if( msg == "DISENGAGE" ) {
                assert(src == "front");
                SyncMessage* dmp = new SyncMessage(inMsg->srcID(),
                                                   machineToInt("sync"),
                                                   inMsg->srcMsgId(),
                                                   messageToInt("SET"),
                                                   macId(), true, 2);
                outMsgs.push_back(dmp) ;
                _state = 3;
                return 3;
            }
            else
                return 3;
        case 3:
            if( msg == "DEADLINE" ) {
                int did = inMsg->getParam(1);
                if( did == 1 ) {
                    outMsgs.push_back(toMerge(inMsg, "STOP"));
                    outMsgs.push_back(toFront(inMsg, "STOP"));
                    outMsgs.push_back(toBack(inMsg, "STOP"));
                    _state = 0;
                    return 3;
                }
                else
                    return 3;
            }
            else
                return 3;
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