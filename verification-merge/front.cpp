//
//  front.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/26/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "front.h"


Front::Front( Lookup* msg, Lookup* mac ):StateMachine(msg, mac)
{
    setId(machineToInt("front"));
    reset();
}

int Front::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs, bool &high_prob,
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
            if( msg == "COOPERATE" ) {
                assert(src == "lock") ;
                _state = 1;
                return 3;
            }
            else
                return 3;
            break;
        case 1:
            if( msg == "ENGAGE" ){
                assert(src == "periodic") ;
                outMsgs.push_back(new MessageTuple(inMsg->srcID(),
                                                   machineToInt("cruise(f)"),
                                                   inMsg->srcMsgId(),
                                                   messageToInt("MAINTAIN"),
                                                   macId()));
                _state = 2;
                return 3;
            }
            else if( msg == "FREE" ) {
                assert(src == "lock") ;
                _state = 0 ;
                return 3;
            }
            else
                return 3;
            break;
        case 2:
            if( msg == "READY" ) {
                assert(src == "coordsensor");
                _state = 3;
                return 3;
            }
            else if( msg == "STOP" ) {
                assert(src == "periodic");
                outMsgs.push_back(new MessageTuple(inMsg->srcID(),
                                                   machineToInt("cruise(f)"),
                                                   inMsg->srcMsgId(),
                                                   messageToInt("RESET"),
                                                   macId()));
                _state = 4 ;
                return 3;
            }
            else
                return 3;
            break;
        case 3:
            if( msg == "FREE" )  {
                assert(src == "lock");
                outMsgs.push_back(new MessageTuple(inMsg->srcID(),
                                                   machineToInt("cruise(f)"),
                                                   inMsg->srcMsgId(),
                                                   messageToInt("RESET"),
                                                   macId()));
                _state = 0 ;
                return 3;
            }
            else if( msg == "EMERGENCY" ) {
                assert(src == "sensor(f)") ;
                outMsgs.push_back(new MessageTuple(inMsg->srcID(),
                                                   machineToInt("periodic"),
                                                   inMsg->srcMsgId(),
                                                   messageToInt("DISENGAGE"),
                                                   macId()));
                _state = 5;
            }
            else
                return 3;
            break;
        case 4:
            if( msg == "FREE" ) {
                assert(src == "lock");
                _state = 0 ;
                return 3;
            }
            else
                return 3;
            break;
        case 5:
            if( msg == "STOP" ) {
                assert(src == "periodic");
                outMsgs.push_back(new MessageTuple(inMsg->srcID(),
                                                   machineToInt("cruise(f)"),
                                                   inMsg->srcMsgId(),
                                                   messageToInt("RESET"),
                                                   macId()));
                _state = 4;
                return 3;
            }
            else
                return 3;
            break;
        default:
            return -1;
            break;
    }
    return -1;
}