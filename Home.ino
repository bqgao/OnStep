// -----------------------------------------------------------------------------------
// functions related to Homing the mount

// moves telescope to the home position, then stops tracking
boolean goHome() {
  if (trackingState!=TrackingMoveTo) {
    cli();
    startHA =posHA;
    startDec=posDec;
    if (pierSide==PierSideWest) targetHA=-celestialPoleHA*StepsPerDegreeHA; else targetHA=celestialPoleHA*StepsPerDegreeHA;
    fTargetHA=longToFixed(targetHA);
    targetDec=celestialPoleDec*StepsPerDegreeDec;
    fTargetDec=longToFixed(targetDec);
    sei();
    
//    pierSide         = PierSideNone;
    lastTrackingState= TrackingNone;
    
    trackingState    = TrackingMoveTo;
    SetSiderealClockRate(siderealInterval);
    
    homeMount        = true;
  
    return true;
  } else return false;
}

// sets telescope home position; user manually moves to Hour Angle 90 and Declination 90 (CWD position),
// then the first gotoEqu will set the pier side and turn on tracking
boolean setHome() {
 if (trackingState==TrackingMoveTo) return false;  // fail, forcing home not allowed during a move

  // default values for state variables
  pierSide            = PierSideNone;
  dirDec              = 1;
  DecDir              = DecDirEInit;
  if (celestialPoleDec>0) HADir = HADirNCPInit; else HADir = HADirSCPInit;
  dirHA               = 1;
  newTargetRA         = 0;        
  newTargetDec        = 0;
  newTargetAlt        = 0;
  newTargetAzm        = 0;
  origTargetHA        = 0;
  origTargetDec       = 0;
  
  // reset pointing model
  alignMode           = AlignNone;
  altCor              = 0;
  azmCor              = 0;
  doCor               = 0;
  pdCor               = 0;
  IH                  = 0;
  ID                  = 0;
  
  // reset Meridian Flip control
  #ifdef MOUNT_TYPE_GEM
  meridianFlip = MeridianFlipAlways;
  #endif
  #ifdef MOUNT_TYPE_FORK
  meridianFlip = MeridianFlipAlign;
  #endif
  #ifdef MOUNT_TYPE_FORK_ALT
  meridianFlip = MeridianFlipNever;
  #endif
  
  // where we are
  homeMount           = false;
  atHome              = true;

  // reset tracking and rates
  trackingState       = TrackingNone;
  lastTrackingState   = TrackingNone;
  timerRateHA         = SiderealRate;
  #ifdef DEC_RATIO_ON
  timerRateDec        =SiderealRate*timerRateRatio;
  #else
  timerRateDec        =SiderealRate;
  #endif

  // not parked, but don't wipe the park position if it's saved - we can still use it
  parkStatus          = NotParked;
  EEPROM.write(EE_parkStatus,parkStatus);
  
  // reset PEC, unless we have an index to recover from this
  #ifdef PEC_SENSE_OFF
    PECstatus           = IgnorePEC;
    PECrecorded         = false;
    EEPROM.write(EE_PECstatus,PECstatus);
    EEPROM.write(EE_PECrecorded,PECrecorded);
  #else
    PECstatus           = IgnorePEC;
    PECstatus  =EEPROM.read(EE_PECstatus);
  #endif

  // the polar home position
  startHA             = celestialPoleHA*StepsPerDegreeHA;
  startDec            = celestialPoleDec*StepsPerDegreeDec;

  // clear pulse-guiding state
  guideDurationHA     = 0;
  guideDurationLastHA = 0;
  guideDurationDec    = 0;
  guideDurationLastDec= 0;

  cli();
  targetHA            = startHA;
  fTargetHA=longToFixed(targetHA);
  posHA               = startHA;
  PEC_HA              = 0;
  targetDec           = startDec;
  fTargetDec=longToFixed(targetDec);
  posDec              = startDec;
  sei();

  return true;
}


