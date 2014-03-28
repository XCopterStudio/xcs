#!/usr/bin/tclsh
package require Tk

frame .buttons -borderwidth 0 -relief flat
pack .buttons

button .buttons.takeoff -text "TakeOff" -command { setParam "xci.command" "\"TakeOff\"" }
pack .buttons.takeoff -side left

button .buttons.land -text "Land" -command { cmd "a.trp.stop();a.tyg.stop();xci.command=\"Land\";" }
pack .buttons.land -side left

button .buttons.stopall -text "Stop ALL" -command { cmd "a.trp.stop();a.tyg.stop();" }
pack .buttons.stopall -side left

button .buttons.resetimu -text "Reset IMU" -command { cmd "k.reset(0,0);" }
pack .buttons.resetimu -side left

button .buttons.resetimuvis -text "Reset IMU (visual)" -command { cmd "k.reset(f.distance, f.deviation);" }
pack .buttons.resetimuvis -side left

button .buttons.autostart -text "Autostart" -command { cmd "autostart(a, xci, m),;" }
pack .buttons.autostart -side left

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

scale .gaz.altitude -label "Altitude" \
-length 150 -from 1 -to 2 \
-command { setParam "a.gazAltitude" } \
-resolution 0.05 \
-digits 3 \
-variable gazaltitude \
-showvalue 1 -orient horizontal
pack .gaz.altitude

scale .gaz.enabled -label "Enabled" \
-length 100 -from 0 -to 1 \
-command { setParam "a.gazEnabled" } \
-resolution 1 \
-digits 1 \
-variable gazenabled \
-showvalue 1 -orient horizontal
pack .gaz.enabled




### YAW (& GAZ) ###

frame .yaw -borderwidth 3 -relief raised
pack .yaw -side left

label .yaw.lbl -text "YAW (& GAZ)"
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

scale .yaw.enabled -label "Enabled" \
-length 100 -from 0 -to 1 \
-command { setParam "a.yawEnabled" } \
-resolution 1 \
-digits 1 \
-variable yawenabled \
-showvalue 1 -orient horizontal
pack .yaw.enabled

button .yaw.start -text "Start YG" -command { cmd "a.adjustYG()," }
pack .yaw.start
button .yaw.stop -text "Stop YG" -command { cmd "a.tyg.stop()" }
pack .yaw.stop




### ROLL (& PICTH) ###

frame .roll -borderwidth 3 -relief raised
pack .roll -side left

label .roll.lbl -text "ROLL"
pack .roll.lbl

scale .roll.p  -label "roll P" \
-length 150 -from 0 -to 4 \
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

scale .roll.enabled -label "Enabled" \
-length 100 -from 0 -to 1 \
-command { setParam "a.rollEnabled" } \
-resolution 1 \
-digits 1 \
-variable rollenabled \
-showvalue 1 -orient horizontal
pack .roll.enabled


button .roll.start -text "Start RP" -command { cmd "a.adjustRP()," }
pack .roll.start
button .roll.stop -text "Stop RP" -command { cmd "a.trp.stop()" }
pack .roll.stop

scale .roll.int -label "RP interval" \
-length 150 -from 0 -to 2 \
-command { setParam "a.rpInterval" } \
-resolution 0.05 \
-digits 3 \
-variable rollint \
-showvalue 1 -orient horizontal
pack .roll.int

scale .roll.sleep -label "RP sleep" \
-length 150 -from 0 -to 1 \
-command { setParam "a.rpSleep" } \
-resolution 0.01 \
-digits 3 \
-variable rollsleep \
-showvalue 1 -orient horizontal
pack .roll.sleep

scale .roll.sleepmax -label "RP sleep max" \
-length 150 -from 0 -to 1 \
-command { setParam "a.rpSleepMax" } \
-resolution 0.01 \
-digits 3 \
-variable rollsleepmax \
-showvalue 1 -orient horizontal
pack .roll.sleepmax

### PITCH ###

frame .pitch -borderwidth 3 -relief raised
pack .pitch -side left

label .pitch.lbl -text "PITCH"
pack .pitch.lbl

scale .pitch.p  -label "pitch P" \
-length 150 -from 0 -to 4 \
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


scale .pitch.enabled -label "Enabled" \
-length 100 -from 0 -to 1 \
-command { setParam "a.pitchEnabled" } \
-resolution 1 \
-digits 1 \
-variable pitchenabled \
-showvalue 1 -orient horizontal
pack .pitch.enabled



scale .pitch.forward -label "Forward" \
-length 150 -from 0 -to 1 \
-command { setParam "a.forward" } \
-resolution 0.01 \
-digits 3 \
-variable pitchforward \
-showvalue 1 -orient horizontal
pack .pitch.forward

scale .pitch.forwardmax -label "Forward max" \
-length 150 -from 0 -to 1 \
-command { setParam "a.forwardMax" } \
-resolution 0.01 \
-digits 3 \
-variable pitchforwardmax \
-showvalue 1 -orient horizontal
pack .pitch.forwardmax

scale .pitch.safecurv -label "Safe curv" \
-length 150 -from 0 -to 0.0100 \
-command { setParam "a.safeCurv" } \
-resolution 0.0001 \
-digits 4 \
-variable pitchsafecurv \
-showvalue 1 -orient horizontal
pack .pitch.safecurv

scale .pitch.fullcurv -label "Full curv" \
-length 150 -from 0 -to 0.0080 \
-command { setParam "a.fullCurv" } \
-resolution 0.0001 \
-digits 4 \
-variable pitchfullcurv \
-showvalue 1 -orient horizontal
pack .pitch.fullcurv


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
-command { setParam "s.curvatureAging" } \
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
scale .camerascale -label "Camera scale" \
-length 150 -from 0.0001 -to 0.0020 \
-command { setParam "k.cameraScale" } \
-resolution 0.00005 \
-digits 4 \
-variable camerascale \
-showvalue 1 -orient horizontal
pack .camerascale

#label .lbl -textvariable val 
#pack .lbl

##################
# Default values #
##################
set rollpval 1.1
set rollival 0
set rolldval 0
set rollint 0.6
set rollsleep 0.20
set rollsleepmax 0.20

set yawpval 0.9
set yawival 0.0
set yawdval 0

set pitchpval 2.4
set pitchival 0
set pitchdval 0
set pitchforward 0.09
set pitchforwardmax 0.09
set pitchfullcurv 0.0004
set pitchsafecurv 0.0080

set gazpval 0.5
set gazival 0
set gazdval 0

set distagingval 0.1
set devaging 0.7
set curvaging 0.85
set hsvratio 0.0
set houghminlenght 50
set houghmaxgap 40
set houghrho 3
set hystcenterthresh 0.2
set hystdirthreshbs 0.30
set hystdirthreshrt 40
set imuweight 0.75
set curva 2.0
set gazaltitude 1.5

set rollenabled 1
set pitchenabled 1
set yawenabled 1
set gazenabled 1

# value determined by measurement (more precise experiment)
set cameraparam 0.74
# value determined by experiment
set camerascale 0.001

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



