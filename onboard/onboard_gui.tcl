#!/usr/bin/tclsh
package require Tk

button .takeoff -text "TakeOff" -command { setParam "xci.command" "\"TakeOff\"" }
pack .takeoff

button .land -text "Land" -command { setParam "xci.command" "\"Land\"" }
pack .land

button .startall -text "Start ALL" -command { cmd "a.adjustGaz(),a.adjustRoll(),a.adjustYaw(),a.adjustPitch()," }
pack .startall

button .stopall -text "Stop ALL" -command { cmd "a.tpitch.stop();a.tyaw.stop();a.troll.stop();a.tgaz.stop();" }
pack .stopall

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

scale .roll.int -label "roll interval" \
-length 400 -from 0 -to 2 \
-command { setParam "a.rollInterval" } \
-resolution 0.05 \
-digits 3 \
-variable rollint \
-showvalue 1 -orient horizontal
pack .roll.int

scale .roll.sleep -label "roll sleep" \
-length 400 -from 0 -to 1 \
-command { setParam "a.rollSleep" } \
-resolution 0.01 \
-digits 3 \
-variable rollsleep \
-showvalue 1 -orient horizontal
pack .roll.sleep

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

### MIscellaneous ###

scale .distaging -label "Distance Aging" \
-length 300 -from 0 -to 1 \
-command { setParam "f.distanceAging" } \
-resolution 0.05 \
-digits 3 \
-variable distagingval \
-showvalue 1 -orient horizontal
pack .distaging

scale .devaging -label "Deviation Aging" \
-length 300 -from 0 -to 1 \
-command { setParam "f.deviationAging" } \
-resolution 0.05 \
-digits 3 \
-variable devaging \
-showvalue 1 -orient horizontal
pack .devaging

scale .hsvratio -label "Auto HSV Range Ratio" \
-length 300 -from 0 -to 0.5 \
-command { setParam "f.autoHsvValueRangeRatio" } \
-resolution 0.005 \
-digits 3 \
-variable hsvratio \
-showvalue 1 -orient horizontal
pack .hsvratio

scale .houghminlenght -label "Hough Min Length" \
-length 300 -from 0 -to 200 \
-command { setParam "f.houghMinLength" } \
-resolution 1 \
-digits 3 \
-variable houghminlenght \
-showvalue 1 -orient horizontal
pack .houghminlenght

scale .houghmaxgap -label "Hough Max Gap" \
-length 300 -from 0 -to 200 \
-command { setParam "f.houghMaxGap" } \
-resolution 1 \
-digits 3 \
-variable houghmaxgap \
-showvalue 1 -orient horizontal
pack .houghmaxgap

scale .houghrho -label "Hough Rho" \
-length 300 -from 0 -to 10 \
-command { setParam "f.houghRho" } \
-resolution 0.5 \
-digits 3 \
-variable houghrho \
-showvalue 1 -orient horizontal
pack .houghrho

scale .hystcenterthresh -label "Hystersis Center Threshhold" \
-length 300 -from 0 -to 1 \
-command { setParam "f.hystCenterThreshold" } \
-resolution 0.05 \
-digits 3 \
-variable hystcenterthresh \
-showvalue 1 -orient horizontal
pack .hystcenterthresh


#label .lbl -textvariable val 
#pack .lbl

##################
# Default values #
##################
set rollpval 1
set rollival 0
set rolldval 0
set rollint 1
set rollsleep 0.4

set yawpval 0.9
set yawival 0
set yawdval 0

set pitchpval 0.1
set pitchival 0.1
set pitchdval -0.03

set gazpval 0.5
set gazival 0
set gazdval 0

set distagingval 0.1
set devaging 0.2
set hsvratio 0.0
set houghminlenght 50
set houghmaxgap 40
set houghrho 3
set hystcenterthresh 0.2

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



