//
//  trbp.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 4/11/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#include "trbp.h"


//
//  periodic.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/23/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "trbp.h"
#include "../prob_verify/sync.h"



TRBP::TRBP( Lookup* msg, Lookup* mac ):StateMachine(msg, mac)
{
    setId(machineToInt("periodic")) ;
    reset();
}

int TRBP::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs, bool &high_prob,
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
                _state = 1;
                return 3;
            }
            else
                return -1;
            break;
        case 1:
            if( msg == "STOP" ) {
                assert(src == "merge") ;
                _state = 0 ;
                return 3;
            }
            else
                return -1;
            break;
        case 2:
            if( msg == "STOP" ) {
                assert(src == "merge");
                _state = 0;
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

int TRBP::nullInputTrans(vector<MessageTuple *> &outMsgs, bool &high_prob, int startIdx)
{
    outMsgs.clear() ;
    if( _state != 1 )
        return -1;
    high_prob = false;
    switch (_state) {
        case 0:
            return -1;
        case 1:
            if( startIdx == 2 ) {
                outMsgs.push_back(createMsg(0, "merge", "READY"));
                return 3;
            }
        case 2:
            if( startIdx == 0 ) {
                outMsgs.push_back(emergency("merge"));
                outMsgs.push_back(emergency("front"));
                outMsgs.push_back(emergency("back"));
                high_prob = false;
                return 1;
            }
            else if( startIdx == 1) {
                outMsgs.push_back(gapTaken("merge"));
                outMsgs.push_back(gapTaken("front"));
                outMsgs.push_back(gapTaken("back"));
                high_prob = false;
                return 2;
            }
            else {
                return -1;
            }
            break;
            
        default:
            return -1;
            break;
    }
}

MessageTuple* TRBP::createMsg(MessageTuple *inMsg, const string& dest, string msg)
{
    if( inMsg == 0 )
        return new MessageTuple(0, machineToInt(dest),
                                0, messageToInt(msg), macId()) ;
    else
        return new MessageTuple(inMsg->srcID(), machineToInt(dest),
                                inMsg->srcMsgId(), messageToInt(msg), macId()) ;
}

MessageTuple* TRBP::emergency(const string& dest) {
    return createMsg(0, dest, "EMERGENCY");
}

MessageTuple* TRBP::gapTaken(const string& dest) {
    return createMsg(0, dest, "GAPTAKEN");
}