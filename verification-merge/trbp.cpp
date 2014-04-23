//
//  trbp.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 4/11/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#include "trbp.h"


TRBP::TRBP( Lookup* msg, Lookup* mac ):StateMachine(msg, mac)
{
    setId(machineToInt(TRBP_NAME)) ;
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
            if( msg == START ) {
                assert(src == MERGE_NAME);
                _state = 1;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else
                return -1;
            break;
        case 1:
            if( msg == STOP ) {
                assert(src == MERGE_NAME) ;
                _state = 0 ;
                return 3;
            }
            else if (msg == MONITOR) {
                assert(src == MERGE_NAME);
                _state = 2;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else
                return -1;
            break;
        case 2:
            if( msg == STOP ) {
                assert(src == MERGE_NAME) ;
                _state = 0 ;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else
                return -1;
            break;
        case 3:
            if( msg == STOP ) {
                assert(src == MERGE_NAME);
                _state = 0;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
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
    high_prob = true;
    switch (_state) {
        case 0:
            return -1;
        case 2:
            if( startIdx == 1 ) {
                outMsgs.push_back(createMsg(0, MERGE_NAME, GAPREADY));
                _state = 2;
                return 3;
            }
        case 1:
        case 3:
            if( startIdx == 0 ) {
                outMsgs.push_back(emergency(MERGE_NAME));
                outMsgs.push_back(emergency(FRONT_NAME));
                outMsgs.push_back(emergency(BACK_NAME));
                _state = 0;
                high_prob = false;
                return 1;
            }
            /*
            else if( startIdx == 1) {
                outMsgs.push_back(gapTaken("merge"));
                outMsgs.push_back(gapTaken("front"));
                outMsgs.push_back(gapTaken("back"));
                high_prob = false;
                return 2;
            }
            else if( startIdx == 2) {
                outMsgs.push_back(loss("merge"));
                outMsgs.push_back(loss("back"));
                outMsgs.push_back(loss("front"));
            }
             */
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

MessageTuple* TRBP::emergency(const string& dest)
{
    return createMsg(0, dest, EMERGENCY);
}

MessageTuple* TRBP::gapTaken(const string& dest)
{
    return createMsg(0, dest, GAPTAKEN);
}

MessageTuple* TRBP::loss(const string &dest)
{
    return createMsg(0, dest, COMMLOSS);
}