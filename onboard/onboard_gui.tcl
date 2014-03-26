#!/usr/bin/tclsh
package require Tk

frame .buttons -borderwidth 0 -relief flat
pack .buttons

button .buttons.takeoff -text "TakeOff" -command { setParam "xci.command" "\"TakeOff\"" }
pack .buttons.takeoff -side left

button .buttons.land -text "Land" -command { cmd "a.tpitch.stop();a.tyaw.stop();a.troll.stop();a.tgaz.stop();xci.command=\"Land\";" }
pack .buttons.land -side left

button .buttons.startall -text "Start ALL" -command { cmd "a.adjustGaz(),a.adjustRoll(),a.adjustYaw(),a.adjustPitch()," }
pack .buttons.startall -side left

button .buttons.stopall -text "Stop ALL" -command { cmd "a.tpitch.stop();a.tyaw.stop();a.troll.stop();a.tgaz.stop();" }
pack .buttons.stopall -side left

button .buttons.resetimu -text "Reset IMU" -command { cmd "k.reset(0,0);" }
pack .buttons.resetimu -side left

button .buttons.resetimuvis -text "Reset IMU (visual)" -command { cmd "k.reset(f.distance, f.deviation);" }
pack .buttons.resetimuvis -side left

### ROLL ###

frame .roll -borderwidth 3 -relief raised
pack .roll -side left

label .roll.lbl -text "ROLL"
pack .roll.lbl

scale .roll.p  -label "roll P" \
-length 150 -from 0 -to 2 \
-command { setParam "a.rollPidParam.p" } \
-resolution 0.01 \
-digits 3 \
-variable rollpval \
-showvalue 1 -orient horizontal
pack .roll.p

scale .roll.i  -label "roll I" \
-length 150 -from 0 -to 2 \
-command { setParam "a.rollPidParam.i" } \
-resolution 0.01 \
-digits 3 \
-variable rollival \
-showvalue 1 -orient horizontal
pack .roll.i

scale .roll.d -label "roll D" \
-length 150 -from -2 -to 2 \
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
-length 150 -from 0 -to 2 \
-command { setParam "a.rollInterval" } \
-resolution 0.05 \
-digits 3 \
-variable rollint \
-showvalue 1 -orient horizontal
pack .roll.int

scale .roll.sleep -label "roll sleep" \
-length 150 -from 0 -to 1 \
-command { setParam "a.rollSleep" } \
-resolution 0.01 \
-digits 3 \
-variable rollsleep \
-showvalue 1 -orient horizontal
pack .roll.sleep

scale .roll.threshold -label "roll threshold" \
-length 150 -from 0 -to 0.5 \
-command { setParam "a.rollThreshold" } \
-resolution 0.05 \
-digits 3 \
-variable rollthreshold \
-showvalue 1 -orient horizontal
pack .roll.threshold

### YAW ###

frame .yaw -borderwidth 3 -relief raised
pack .yaw -side left

label .yaw.lbl -text "YAW"
pack .yaw.lbl

scale .yaw.p  -label "yaw P" \
-length 150 -from 0 -to 2 \
-command { setParam "a.yawPidParam.p" } \
-resolution 0.01 \
-digits 3 \
-variable yawpval \
-showvalue 1 -orient horizontal
pack .yaw.p

scale .yaw.i  -label "yaw I" \
-length 150 -from 0 -to 2 \
-command { setParam "a.yawPidParam.i" } \
-resolution 0.01 \
-digits 3 \
-variable yawival \
-showvalue 1 -orient horizontal
pack .yaw.i

scale .yaw.d -label "yaw D" \
-length 150 -from -2 -to 2 \
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

frame .pitch -borderwidth 3 -relief raised
pack .pitch -side left

label .pitch.lbl -text "PITCH"
pack .pitch.lbl

scale .pitch.p  -label "pitch P" \
-length 150 -from 0 -to 2 \
-command { setParam "a.pitchPidParam.p" } \
-resolution 0.01 \
-digits 3 \
-variable pitchpval \
-showvalue 1 -orient horizontal
pack .pitch.p

scale .pitch.i  -label "pitch I" \
-length 150 -from 0 -to 2 \
-command { setParam "a.pitchPidParam.i" } \
-resolution 0.01 \
-digits 3 \
-variable pitchival \
-showvalue 1 -orient horizontal
pack .pitch.i

scale .pitch.d -label "pitch D" \
-length 150 -from -2 -to 2 \
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

scale .pitch.int -label "pitch interval" \
-length 150 -from 0 -to 2 \
-command { setParam "a.pitchInterval" } \
-resolution 0.05 \
-digits 3 \
-variable pitchint \
-showvalue 1 -orient horizontal
pack .pitch.int

scale .pitch.sleep -label "pitch sleep" \
-length 150 -from 0 -to 1 \
-command { setParam "a.pitchSleep" } \
-resolution 0.01 \
-digits 3 \
-variable pitchsleep \
-showvalue 1 -orient horizontal
pack .pitch.sleep

scale .pitch.distance -label "pitch distance" \
-length 150 -from 0 -to 0.5 \
-command { setParam "a.pitchDistance" } \
-resolution 0.05 \
-digits 3 \
-variable pitchdistance \
-showvalue 1 -orient horizontal
pack .pitch.distance

scale .pitch.deviation -label "pitch deviation" \
-length 150 -from 0 -to 0.5 \
-command { setParam "a.pitchDeviation" } \
-resolution 0.05 \
-digits 3 \
-variable pitchdeviation \
-showvalue 1 -orient horizontal
pack .pitch.deviation

### GAZ ###

frame .gaz -borderwidth 3 -relief raised
pack .gaz -side left

label .gaz.lbl -text "GAZ"
pack .gaz.lbl

scale .gaz.p  -label "gaz P" \
-length 150 -from 0 -to 2 \
-command { setParam "a.gazPidParam.p" } \
-resolution 0.01 \
-digits 3 \
-variable gazpval \
-showvalue 1 -orient horizontal
pack .gaz.p

scale .gaz.i  -label "gaz I" \
-length 150 -from 0 -to 2 \
-command { setParam "a.gazPidParam.i" } \
-resolution 0.01 \
-digits 3 \
-variable gazival \
-showvalue 1 -orient horizontal
pack .gaz.i

scale .gaz.d -label "gaz D" \
-length 150 -from -2 -to 2 \
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

scale .gaz.altitude -label "Altitude" \
-length 150 -from 1 -to 2 \
-command { setParam "a.gazAltitude" } \
-resolution 0.05 \
-digits 3 \
-variable gazaltitude \
-showvalue 1 -orient horizontal
pack .gaz.altitude



### MIscellaneous ###

scale .distaging -label "Distance Aging" \
-length 150 -from 0 -to 1 \
-command { setParam "s.distanceAging" } \
-resolution 0.05 \
-digits 3 \
-variable distagingval \
-showvalue 1 -orient horizontal
pack .distaging

scale .devaging -label "Deviation Aging" \
-length 150 -from 0 -to 1 \
-command { setParam "s.deviationAging" } \
-resolution 0.05 \
-digits 3 \
-variable devaging \
-showvalue 1 -orient horizontal
pack .devaging

scale .curvaging -label "Curvature aging" \
-length 150 -from 0 -to 1 \
-command { setParam "m.curvatureAging" } \
-resolution 0.05 \
-digits 3 \
-variable curvaging \
-showvalue 1 -orient horizontal
pack .curvaging


scale .hsvratio -label "Auto HSV Range Ratio" \
-length 150 -from 0 -to 0.5 \
-command { setParam "f.autoHsvValueRangeRatio" } \
-resolution 0.005 \
-digits 3 \
-variable hsvratio \
-showvalue 1 -orient horizontal
pack .hsvratio

scale .houghminlenght -label "Hough Min Length" \
-length 150 -from 0 -to 200 \
-command { setParam "f.houghMinLength" } \
-resolution 1 \
-digits 3 \
-variable houghminlenght \
-showvalue 1 -orient horizontal
pack .houghminlenght

scale .houghmaxgap -label "Hough Max Gap" \
-length 150 -from 0 -to 200 \
-command { setParam "f.houghMaxGap" } \
-resolution 1 \
-digits 3 \
-variable houghmaxgap \
-showvalue 1 -orient horizontal
pack .houghmaxgap

scale .houghrho -label "Hough Rho" \
-length 150 -from 0 -to 10 \
-command { setParam "f.houghRho" } \
-resolution 0.5 \
-digits 3 \
-variable houghrho \
-showvalue 1 -orient horizontal
pack .houghrho

scale .hystcenterthresh -label "Hystersis Distance Threshhold" \
-length 150 -from 0 -to 1 \
-command { setParam "f.hystDistanceThr" } \
-resolution 0.05 \
-digits 3 \
-variable hystcenterthresh \
-showvalue 1 -orient horizontal
pack .hystcenterthresh

scale .hystdirthreshbs -label "Hyst. Dev. Thr. Base" \
-length 150 -from 0 -to 1 \
-command { setParam "m.hystDeviationThrBase" } \
-resolution 0.05 \
-digits 3 \
-variable hystdirthreshbs \
-showvalue 1 -orient horizontal
pack .hystdirthreshbs

scale .hystdirthreshrt -label "Hyst. Dev. Thr. Rate" \
-length 150 -from 0 -to 100 \
-command { setParam "m.hystDeviationThrRate" } \
-resolution 5 \
-digits 3 \
-variable hystdirthreshrt \
-showvalue 1 -orient horizontal
pack .hystdirthreshrt

scale .imuweight -label "IMU line weight" \
-length 150 -from 0 -to 1 \
-command { setParam "m.imuWeight" } \
-resolution 0.05 \
-digits 3 \
-variable imuweight \
-showvalue 1 -orient horizontal
pack .imuweight

scale .curva -label "Curvature tolerance" \
-length 150 -from 1 -to 4 \
-command { setParam "f.curvatureTolerance" } \
-resolution 0.25 \
-digits 3 \
-variable curva \
-showvalue 1 -orient horizontal
pack .curva

#scale .cameraparam -label "Camera param" \
#-length 150 -from 0.01 -to 2 \
#-command { setParam "f.cameraParam" } \
#-resolution 0.025 \
#-digits 3 \
#-variable cameraparam \
#-showvalue 1 -orient horizontal
#pack .cameraparam
#
#scale .camerascale -label "Camera scale" \
#-length 150 -from 0.000 -to 0.03 \
#-command { setParam "k.cameraScale" } \
#-resolution 0.0003 \
#-digits 4 \
#-variable camerascale \
#-showvalue 1 -orient horizontal
#pack .camerascale

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
set rollthreshold 0.1

set yawpval 0.9
set yawival 0.0
set yawdval 0

set pitchpval 0.35
set pitchival 0
set pitchdval 0
set pitchint 0.9
set pitchsleep 0.5
set pitchdistance 0.2
set pitchdeviation 0.15

set gazpval 0.5
set gazival 0
set gazdval 0

set distagingval 0.1
set devaging 0.8
set curvaging 0.85
set hsvratio 0.0
set houghminlenght 50
set houghmaxgap 40
set houghrho 3
set hystcenterthresh 0.2
set hystdirthreshbs 0.30
set hystdirthreshrt 40
set imuweight 0.65
set curva 2.0
set gazaltitude 1.5

# value determined by measurement (more precise experiment)
set cameraparam 0.74
# value determined by experiment
set camerascale 0.0012

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



