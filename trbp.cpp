//
//  trbp.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 4/11/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#include "trbp.h"


TRBP::TRBP() {
  setId(machineToInt(TRBP_NAME));
  machine_name_ = TRBP_NAME;
  reset();
}

int TRBP::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs,
                  bool &high_prob, int start_idx) {
  outMsgs.clear();
  high_prob = true;
  if (start_idx)
    return -1;

  string msg = IntToMessage(inMsg->destMsgId());
  string src = IntToMachine(inMsg->subjectId());
  auto m_ptr = dynamic_cast<Merge*>(ProbVerifier::getMachine(MERGE_NAME));
  auto f_ptr = dynamic_cast<Front*>(ProbVerifier::getMachine(FRONT_NAME));
  auto b_ptr = dynamic_cast<Back*>(ProbVerifier::getMachine(BACK_NAME));
  int m_state = m_ptr->getState();
  int f_state = f_ptr->getState();
  int b_state = b_ptr->getState();
  if (!_state) {
    if (msg == CLOCKFAIL) {
      if (m_state >= 2 && m_state <= 6)
        outMsgs.push_back(loss(MERGE_NAME));
      if (f_state >= 1 && f_state <= 3)
        outMsgs.push_back(loss(FRONT_NAME));
      if (b_state >= 1 && b_state <= 3)
        outMsgs.push_back(loss(BACK_NAME));
      _state = 1;
      return 3;
    } else {
      return 3;
    }
  } else {
    return 3;
  }
  return -1;
}

int TRBP::nullInputTrans(vector<MessageTuple *> &outMsgs,
                         bool &high_prob, int startIdx) {
  outMsgs.clear();
  high_prob = true;
  auto m_ptr = dynamic_cast<Merge*>(ProbVerifier::getMachine(MERGE_NAME));
  auto f_ptr = dynamic_cast<Front*>(ProbVerifier::getMachine(FRONT_NAME));
  auto b_ptr = dynamic_cast<Back*>(ProbVerifier::getMachine(BACK_NAME));
  int m_state = m_ptr->getState();
  int f_state = f_ptr->getState();
  int b_state = b_ptr->getState();
  if (!_state) {
    if (!startIdx) {
      high_prob = false;
      if (m_state >= 2 && m_state <= 6)
        outMsgs.push_back(loss(MERGE_NAME));
      if (f_state >= 1 && f_state <= 3)
        outMsgs.push_back(loss(FRONT_NAME));
      if (b_state >= 1 && b_state <= 3)
        outMsgs.push_back(loss(BACK_NAME));
      _state = 1;
    } else {
      return -1;
    }
  }
  return -1;
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


MessageTuple* TRBP::loss(const string &dest)
{
    return createMsg(0, dest, COMMLOSS);
}
