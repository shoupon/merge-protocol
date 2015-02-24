//
//  emergency-checker.cpp
//  merge-protocol
//
//  Created by Shou-pon Lin on 2/23/15.
//  Copyright (c) 2015 Shou-pon Lin. All rights reserved.
//

#include "emergency-checker.h"
int getPosition(const string& name) {
  return ProbVerifier::getMachine(name)->macId() - 1;
}

EmergencyChecker::EmergencyChecker()
      : trbp_pos_(getPosition(TRBP_NAME)),
        sensor_pos_(getPosition(SENSOR_NAME)),
        driver_pos_(getPosition(DRIVER_NAME)),
        merge_pos_(getPosition(MERGE_NAME)),
        front_pos_(getPosition(FRONT_NAME)),
        back_pos_(getPosition(BACK_NAME)),
        cc_merge_pos_(getPosition(CRUISE_MERGE_NAME)),
        cc_front_pos_(getPosition(CRUISE_FRONT_NAME)),
        cc_back_pos_(getPosition(CRUISE_BACK_NAME)) {
  ;
}

bool EmergencyChecker::check(CheckerState* checker_state,
                             const vector<StateSnapshot*>& machine_states) {
  const auto trbp_ptr = machine_states[trbp_pos_];
  const auto sensor_ptr = machine_states[sensor_pos_];
  const auto m_ptr = machine_states[merge_pos_];
  const auto f_ptr = machine_states[front_pos_];
  const auto b_ptr = machine_states[back_pos_];
  int trbp_state = trbp_ptr->curStateId();
  int sensor_state = sensor_ptr->curStateId();
  int m_state = m_ptr->curStateId();
  int f_state = f_ptr->curStateId();
  int b_state = b_ptr->curStateId();
  // If trbp fails, merge shouldn't be in state 2, 3, 4, 5, 6
  //                front shouldn't be in state 1, 2, 3
  //                back  ...
  if (trbp_state == 1) {
    if (m_state >= 2 && m_state <= 6)
      return false;
    else if (f_state >= 1 && f_state <= 3)
      return false;
    else if (b_state >= 1 && b_state <= 3)
      return false;
  }

  // If sensor fails, merge shouldn't be in state 3, 4, 5
  //                  front shouldn't be in state 2, 3
  //                  back  ...
  if (sensor_state == 1 || sensor_state == 2) {
    if (m_state >= 3 && m_state <= 5)
      return false;
    else if (f_state >= 2 && f_state <= 3)
      return false;
    else if (b_state >= 2 && b_state <= 3)
      return false;
  }

  return true;
}   
