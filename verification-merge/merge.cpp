//
//  merge.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/22/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "merge.h"
#include "sync.h"
#include "statemachine.h"

Merge::Merge( Lookup* msg, Lookup* mac ): StateMachine(msg, mac)
{
    setId(machineToInt("merge")) ;
    reset() ;
}

int Merge::transit(MessageTuple *inMsg, vector<MessageTuple*> &outMsgs, bool &high_prob,
                   int startIdx)
{
    outMsgs.clear();
    high_prob = true;
    
    if( startIdx != 0 )
        return -1;
    
    string msg = IntToMessage(inMsg->destMsgId()) ;
    string src = IntToMachine(inMsg->subjectId()) ;
    switch (_state) {
        case 0:
            if( msg == "SIGNAL" ) {
                assert(src == "driver") ;
                // Set D0
                SyncMessage* d0 = new SyncMessage(inMsg->srcID(),
                                                  machineToInt("sync"),
                                                  inMsg->srcMsgId(),
                                                  messageToInt("SET"),
                                                  macId(), true, 0);
                outMsgs.push_back(d0) ;
                MessageTuple* out = new MessageTuple(inMsg->srcID(),
                                                     machineToInt("lock"),
                                                     inMsg->srcMsgId(),
                                                     messageToInt("ATTEMPT"),
                                                     macId()) ;
                outMsgs.push_back(out) ;
                _state = 1;
                return 3;
            }
            else
                return 3;
            break ;
        case 1:
            if( msg == "SUCCESS" ) {
                assert(src == "lock") ;
                SyncMessage* d1 = new SyncMessage(inMsg->srcID(),
                                                  machineToInt("sync"),
                                                  inMsg->srcMsgId(),
                                                  messageToInt("SET"),
                                                  macId(), true, 1) ;
                outMsgs.push_back(d1) ;
                MessageTuple* out = createOutput(inMsg, machineToInt("periodic"),
                                                 messageToInt("START")) ;
                outMsgs.push_back(out) ;
                _state = 2;
                return 3;
            }
            else if( msg == "CANCEL") {
                assert(src == "driver") ;
                MessageTuple* out = createOutput(inMsg, machineToInt("lock"),
                                                 messageToInt("UNLOCK")) ;
                outMsgs.push_back(out);
                _state = 0 ;
                return 3;
            }
            else
                return 3;
            break;
        case 2:
            if( msg == "ENGAGE" ) {
                assert(src == "periodic") ;
                MessageTuple* out = createOutput(inMsg, machineToInt("cruise"),
                                                 messageToInt("ALIGN"));
                outMsgs.push_back(out);
                _state = 3;
                return 3;
            }
            else if( msg == "STOP" ) {
                assert(src == "periodic");
                outMsgs.push_back(createOutput(inMsg, machineToInt("lock"),
                                               messageToInt("UNLOCK")));
                outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                               messageToInt("ABORT"))) ;
                _state = 0 ;
                return 3;
            }
            else if( msg == "CANCEL") {
                assert(src == "driver") ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("lock"),
                                               messageToInt("UNLOCK"))) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("periodic"),
                                               messageToInt("END")));
                _state = 0 ;
                return 3;
            }
            else if( msg == "DEADLINE" ) {
                int did = inMsg->getParam(1) ;
                assert(did == 1);
                outMsgs.push_back(createOutput(inMsg, machineToInt("lock"),
                                               messageToInt("UNLOCK")));
                outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                               messageToInt("ABORT"))) ;
                _state = 0 ;
                return 3;
            }
            else
                return 3;
            break;
        case 3:
            if( msg == "READY" ) {
                assert(src == "sensor") ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                               messageToInt("CLEARTOMOVE"))) ;
                _state = 4;
                return 3;
            }
            else if( msg == "STOP" ) {
                assert(src == "periodic");
                outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                               messageToInt("ABORT"))) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise"),
                                               messageToInt("RESET"))) ;
                _state = 5;
                return 3;
            }
            else if( msg == "DEADLINE") {
                int did = inMsg->getParam(1);
                assert(did == 1);
                outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                               messageToInt("ABORT"))) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise"),
                                               messageToInt("RESET"))) ;
                _state = 5;
                return 3;
            }
            else
                return 3;
            break;
        case 4:
            if( msg == "COMPLETE" ) {
                assert(src == "driver") ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise"),
                                               messageToInt("RESET"))) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("lock"),
                                               messageToInt("UNLOCK"))) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("periodic"),
                                               messageToInt("END"))) ;
                _state = 0 ;
                return 3;
            }
            else if( msg == "FREE") {
                assert(src == "lock" );
                outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                               messageToInt("ABORT")));
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise"),
                                               messageToInt("RESET"))) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("periodic"),
                                               messageToInt("END")));
                _state = 0 ;
                return 3;
            }
            else if( msg == "STOP") {
                assert(src == "periodic");
                outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                               messageToInt("ABORT"))) ;
                _state = 5;
                return 3 ;
            }
            else
                return 3;
            break;
        case 5:
            if( msg == "FREE") {
                assert(src == "lock") ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise"),
                                               messageToInt("RESET"))) ;
                _state = 0 ;
                return 3;
            }
            else
                return 3;
            break ;
        default:
            return -1;
            break;
    }
    return -1;
}

int Merge::nullInputTrans(vector<MessageTuple *> &outMsgs, bool &high_prob, int startIdx)
{
    return -1;
}

MessageTuple* Merge::createOutput(MessageTuple* inMsg, int dest, int destMsg)
{
    return new MessageTuple(inMsg->srcID(), dest, inMsg->srcMsgId(), destMsg, macId());
}
