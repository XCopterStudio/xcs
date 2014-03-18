#!/usr/bin/tclsh
package require Tk

button .takeoff -text "TakeOff" -command { setParam "xci.command" "\"TakeOff\"" }
pack .takeoff

button .land -text "Land" -command { setParam "xci.command" "\"Land\"" }
pack .land

### ROLL ###

frame .roll -borderwidth 5 -relief raised
pack .roll -side left

label .roll.lbl -text "ROLL"
pack .roll.lbl

scale .roll.p  -label "roll P" \
-length 400 -from 0 -to 2 \
-command { setParam "a.rollPidParam.p" } \
-resolution 0.01 \
-digits 3 \
-variable rollpval \
-showvalue 1 -orient horizontal
pack .roll.p

scale .roll.i  -label "roll I" \
-length 400 -from 0 -to 2 \
-command { setParam "a.rollPidParam.i" } \
-resolution 0.01 \
-digits 3 \
-variable rollival \
-showvalue 1 -orient horizontal
pack .roll.i

scale .roll.d -label "roll D" \
-length 400 -from -2 -to 2 \
-command { setParam "a.rollPidParam.d" } \
-resolution 0.01 \
-digits 3 \
-variable rolldval \
-showvalue 1 -orient horizontal
pack .roll.d

button .roll.start -text "Start" -command { cmd "a.adjustRoll()," }
pack .roll.start
button .roll.stop -text "Stop" -command { cmd "a.troll.stop()" }
pack .roll.stop

### YAW ###

frame .yaw -borderwidth 5 -relief raised
pack .yaw -side left

label .yaw.lbl -text "YAW"
pack .yaw.lbl

scale .yaw.p  -label "yaw P" \
-length 400 -from 0 -to 2 \
-command { setParam "a.yawPidParam.p" } \
-resolution 0.01 \
-digits 3 \
-variable yawpval \
-showvalue 1 -orient horizontal
pack .yaw.p

scale .yaw.i  -label "yaw I" \
-length 400 -from 0 -to 2 \
-command { setParam "a.yawPidParam.i" } \
-resolution 0.01 \
-digits 3 \
-variable yawival \
-showvalue 1 -orient horizontal
pack .yaw.i

scale .yaw.d -label "yaw D" \
-length 400 -from -2 -to 2 \
-command { setParam "a.yawPidParam.d" } \
-resolution 0.01 \
-digits 3 \
-variable yawdval \
-showvalue 1 -orient horizontal
pack .yaw.d

button .yaw.start -text "Start" -command { cmd "a.adjustYaw()," }
pack .yaw.start
button .yaw.stop -text "Stop" -command { cmd "a.tyaw.stop()" }
pack .yaw.stop

### PITCH ###

frame .pitch -borderwidth 5 -relief raised
pack .pitch -side left

label .pitch.lbl -text "PITCH"
pack .pitch.lbl

scale .pitch.p  -label "pitch P" \
-length 400 -from 0 -to 2 \
-command { setParam "a.pitchPidParam.p" } \
-resolution 0.01 \
-digits 3 \
-variable pitchpval \
-showvalue 1 -orient horizontal
pack .pitch.p

scale .pitch.i  -label "pitch I" \
-length 400 -from 0 -to 2 \
-command { setParam "a.pitchPidParam.i" } \
-resolution 0.01 \
-digits 3 \
-variable pitchival \
-showvalue 1 -orient horizontal
pack .pitch.i

scale .pitch.d -label "pitch D" \
-length 400 -from -2 -to 2 \
-command { setParam "a.pitchPidParam.d" } \
-resolution 0.01 \
-digits 3 \
-variable pitchdval \
-showvalue 1 -orient horizontal
pack .pitch.d

button .pitch.start -text "Start" -command { cmd "a.adjustPitch()," }
pack .pitch.start
button .pitch.stop -text "Stop" -command { cmd "a.tpitch.stop()" }
pack .pitch.stop

### GAZ ###

frame .gaz -borderwidth 5 -relief raised
pack .gaz -side left

label .gaz.lbl -text "GAZ"
pack .gaz.lbl

scale .gaz.p  -label "gaz P" \
-length 400 -from 0 -to 2 \
-command { setParam "a.gazPidParam.p" } \
-resolution 0.01 \
-digits 3 \
-variable gazpval \
-showvalue 1 -orient horizontal
pack .gaz.p

scale .gaz.i  -label "gaz I" \
-length 400 -from 0 -to 2 \
-command { setParam "a.gazPidParam.i" } \
-resolution 0.01 \
-digits 3 \
-variable gazival \
-showvalue 1 -orient horizontal
pack .gaz.i

scale .gaz.d -label "gaz D" \
-length 400 -from -2 -to 2 \
-command { setParam "a.gazPidParam.d" } \
-resolution 0.01 \
-digits 3 \
-variable gazdval \
-showvalue 1 -orient horizontal
pack .gaz.d

button .gaz.start -text "Start" -command { cmd "a.adjustGaz()," }
pack .gaz.start
button .gaz.stop -text "Stop" -command { cmd "a.tgaz.stop()" }
pack .gaz.stop

scale .distaging -label "Distance Aging" \
-length 400 -from 0 -to 1 \
-command { setParam "f.distanceAging" } \
-resolution 0.05 \
-digits 3 \
-variable distagingval \
-showvalue 1 -orient horizontal
pack .distaging -side left

#label .lbl -textvariable val 
#pack .lbl

##################
# Default values #
##################
set rollpval 0.4
set rollival 0
set rolldval 0

set yawpval 0.5
set yawival 0
set yawdval 0

set pitchpval 0.1
set pitchival 0.1
set pitchdval -0.02

set gazpval 0.5
set gazival 0
set gazdval 0

set distagingval 0.5


#############
# functions #
#############
proc cmd {string} {
  puts  "$string;"
}

proc setParam {name value} {
  set cmd "$name = $value"
  cmd $cmd
}



