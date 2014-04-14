//
//  identifiers.h
//  verification-merge
//
//  Created by Shou-pon Lin on 4/14/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#ifndef IDENTIFIERS_H
#define IDENTIFIERS_H

// List of machine names
#define LOCK_NAME "lock"
#define CRUISE_FRONT_NAME "cruise(f)"
#define CRUISE_BACK_NAME "cruise(b)"
#define CRUISE_MERGE_NAME "cruise(m)"
#define TRBP_NAME "trbp"
#define FRONT_NAME "front"
#define BACK_NAME "back"
#define MERGE_NAME "merge"
#define DRIVER_NAME "driver"
#define SYNC_NAME "sync"

// List of message names
// Lock
#define ATTEMPT "ATTEMPT"
#define SUCCESS "SUCCESS"
#define COOPERATE "COOPERATE"
// Cruise control
#define RESET "RESET"
#define PILOT "PILOT"
#define DISENGAGE "DISENGAGE"
#define MAINTAIN "MAINTAIN"
#define MAKEGAP "MAKE-GAP"
#define ALIGN "ALIGN"
// TRBP
#define EMERGENCY "EMERGENCY"
#define GAPTAKEN "GAP-TAKEN"
#define COMMLOSS "COMM-LOSS"
#define GAPREADY "GAP-READY"
#define START "START"
#define STOP "STOP"
// Timing
#define DEADLINE "DEADLINE"
#define SET "SET"
// Driver
#define SIGNAL "SIGNAL"
#define CANCEL "CANCEL"
#define ABORT "ABORT"
#define FINISH "FINISH"
#define GREENLIGHT "GREENLIGHT"
// Merge
#define REQUEST "REQUEST"
#define CREATE "CREATE"
#define MOVE "MOVE"


#endif // endif(IDENTIFIERS_H)
