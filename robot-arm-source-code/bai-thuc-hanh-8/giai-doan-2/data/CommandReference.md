//  ASCII protocol, newline-terminated commands.

//  COMMAND REFERENCE
//  ─────────────────────────────────────────────────────────────
//  Command          Args                 Description
//  ─────────────────────────────────────────────────────────────
//  M <id> <ang>     id=0-5, ang=0-180    Move one servo
//  A <a0..a5>       six space-sep angles Move all servos
//  S <id> <spd>     id=0-5, spd=1-20    Set speed (deg/tick)
//  H                –                    Home all servos
//  H <id>           id=0-5               Home one servo
//  X                –                    Stop all motion
//  G <id>           id=0-5               Get current angle
//  T                –                    Status: all angles
//  I                –                    Info: all config
//  W                –                    Wait; replies DONE when idle
//  ─────────────────────────────────────────────────────────────
//  RESPONSE REFERENCE
//  ─────────────────────────────────────────────────────────────
//  OK                         Command accepted
//  ERR:<reason>               Error (ID, ARGS, RANGE)
//  VAL:<id>:<angle>           Single servo angle
//  STA:<a0>,<a1>,...,<a5>     All servo angles
//  CFG:<id>:<min>,<max>,<home>,<spd>  Per-servo config
//  DONE                       Motion complete (reply to W)
//  ─────────────────────────────────────────────────────────────
// =============================================================    

// JOINT DESCRIPTION
// =================

Joint   Name	Function	Min Angle	Max Angle	Home Angle
J0	Base	Rotates entire arm	0deg	180deg	90deg
J1	Shoulder	Raises/lowers upper arm	70deg	150deg	70deg
J2	Elbow	Extends/retracts forearm	0deg	150deg	90deg
J3	Wrist Pitch	Tilts gripper up/down	0deg	180deg	90deg
J4	Wrist Roll	Rotates gripper	0deg	180deg	90deg
J5	Gripper	Opens/closes gripper	60deg	120deg	90deg