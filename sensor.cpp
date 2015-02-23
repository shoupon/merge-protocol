//
//  sensor.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 5/1/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#include "sensor.h"

Sensor::Sensor() {
  setId(machineToInt(SENSOR_NAME)) ;
  machine_name_ = SENSOR_NAME;
  reset();
}

int Sensor::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs,
                    bool &high_prob, int startIdx) {
  outMsgs.clear();
  high_prob = true;
  
  if (startIdx) {
    return -1;
  } else {
    return 3;
  }
}

int Sensor::nullInputTrans(vector<MessageTuple *> &outMsgs, int& prob_level,
                           int startIdx) {
  outMsgs.clear() ;
  prob_level = 0;
  auto m_ptr = dynamic_cast<Merge*>(ProbVerifier::getMachine(MERGE_NAME));
  auto f_ptr = dynamic_cast<Front*>(ProbVerifier::getMachine(FRONT_NAME));
  auto b_ptr = dynamic_cast<Back*>(ProbVerifier::getMachine(BACK_NAME));
  int m_state = m_ptr->getState();
  int f_state = f_ptr->getState();
  int b_state = b_ptr->getState();
  switch (_state) {
    case 0:
      if (!startIdx) {
        if (m_state == 3) {
          outMsgs.push_back(createMsg(0, MERGE_NAME, GAPREADY));
          return 1;
        } else {
          goto events;
        }
      } else if (startIdx == 1) {
events:
        _state = 2;
        prob_level = 4;
        return 2;
      } else if (startIdx == 2) {
        if (m_state >= 3 && m_state <= 5)
          outMsgs.push_back(gapTaken(MERGE_NAME));
        if (f_state >= 2 && f_state <= 3)
          outMsgs.push_back(gapTaken(FRONT_NAME));
        if (b_state >= 2 && b_state <= 3)
          outMsgs.push_back(gapTaken(BACK_NAME));
        _state = 3;
        return 3;
      } else if (startIdx == 3) {
        if (m_state >= 3 && m_state <= 5)
          outMsgs.push_back(inconsistent(MERGE_NAME));
        if (f_state >= 1 && f_state <= 3)
          outMsgs.push_back(inconsistent(FRONT_NAME));
        if (b_state >= 1 && b_state <= 3)
          outMsgs.push_back(inconsistent(BACK_NAME));
        if (outMsgs.empty())
          return -1;
        else
          _state = 1;
        prob_level = 2;
        return 4;  
      } else if (startIdx == 4) {
        if (m_state >= 3 && m_state <= 5)
          outMsgs.push_back(emergency(MERGE_NAME));
        if (f_state >= 1 && f_state <= 3)
          outMsgs.push_back(emergency(FRONT_NAME));
        if (b_state >= 1 && b_state <= 3)
          outMsgs.push_back(emergency(BACK_NAME));
        return 5;
      } else {
        return -1;
      }
      break;
    default:
      return -1;
      break;
  }
  return -1;
}

MessageTuple* Sensor::emergency(const string& dest) {
  return createMsg(0, dest, EMERGENCY);
}

MessageTuple* Sensor::gapTaken(const string& dest) {
  return createMsg(0, dest, GAPTAKEN);
}

MessageTuple* Sensor::inconsistent(const string& dest) {
  return createMsg(0, dest, INCONSISTENT);
}

MessageTuple* Sensor::createMsg(MessageTuple *inMsg, const string& dest,
                                string msg) {
  if( inMsg == 0 )
    return new MessageTuple(0, machineToInt(dest),
                            0, messageToInt(msg), macId()) ;
  else
    return new MessageTuple(inMsg->srcID(), machineToInt(dest),
                            inMsg->srcMsgId(), messageToInt(msg), macId()) ;
}
