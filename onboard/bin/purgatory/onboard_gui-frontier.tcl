#!/usr/bin/tclsh
package require Tk

frame .buttons -borderwidth 0 -relief flat
pack .buttons

button .buttons.takeoff -text "TakeOff" -command { setParam "xci.command" "\"TakeOff\"" }
pack .buttons.takeoff -side left

button .buttons.land -text "Land" -command { setParam "xci.command" "\"Land\"" }
pack .buttons.land -side left

#button .buttons.startall -state disabled -text "Start ALL" -command { cmd "a.adjustGaz(),a.adjustRoll(),a.adjustYaw(),a.adjustPitch()," }
#pack .buttons.startall -side left

button .buttons.stopall -text "Stop ALL" -command { cmd "a.trollpitch.stop();a.tyaw.stop();a.tgaz.stop();a.resetAll()" }
pack .buttons.stopall -side left

button .buttons.startrollpitch -text "Start Roll&Pitch" -command { cmd "a.adjustRollPitch()," }
pack .buttons.startrollpitch -side left

button .buttons.stoprollpitch -text "Stop Roll&Pitch" -command { cmd "a.trollpitch.stop();a.resetRollPitch()" }
pack .buttons.stoprollpitch -side left

button .buttons.resetimu -text "Reset IMU" -command { cmd "k.reset(0,0);" }
pack .buttons.resetimu -side left

button .buttons.resetimuvis -text "Reset IMU (visual)" -command { cmd "k.reset(f.distance, f.deviation);" }
pack .buttons.resetimuvis -side left

frame .common -borderwidth 3 -relief raised
pack .common

scale .common.frontierradius  -label "Frontier Radius" \
-length 160 -from 0 -to 3 \
-command { setParam "a.frontierRadius" } \
-resolution 0.02 \
-digits 3 \
-variable frontierradius \
-showvalue 1 -orient horizontal
pack .common.frontierradius -side left

scale .common.rollpitchint -label "rollpitch interval" \
-length 160 -from 0 -to 2 \
-command { setParam "a.rollPitchInterval" } \
-resolution 0.05 \
-digits 3 \
-variable rollpitchint \
-showvalue 1 -orient horizontal
pack .common.rollpitchint -side left

scale .common.rollpitchsleep -label "rollpitch sleep" \
-length 160 -from 0 -to 2 \
-command { setParam "a.rollPitchInterval" } \
-resolution 0.01 \
-digits 3 \
-variable rollpitchsleep \
-showvalue 1 -orient horizontal
pack .common.rollpitchsleep -side left


### GAZ ###

frame .gaz -borderwidth 3 -relief raised
pack .gaz -side left

label .gaz.lbl -text "GAZ"
pack .gaz.lbl

scale .gaz.p  -label "gaz P" \
-length 160 -from 0 -to 2 \
-command { setParam "a.gazPidParam.p" } \
-resolution 0.01 \
-digits 3 \
-variable gazpval \
-showvalue 1 -orient horizontal
pack .gaz.p

scale .gaz.i  -label "gaz I" \
-length 160 -from 0 -to 2 \
-command { setParam "a.gazPidParam.i" } \
-resolution 0.01 \
-digits 3 \
-variable gazival \
-showvalue 1 -orient horizontal
pack .gaz.i

scale .gaz.d -label "gaz D" \
-length 160 -from -2 -to 2 \
-command { setParam "a.gazPidParam.d" } \
-resolution 0.01 \
-digits 3 \
-variable gazdval \
-showvalue 1 -orient horizontal
pack .gaz.d

button .gaz.start -text "Start GY" -command { cmd "a.adjustGazYaw()," }
pack .gaz.start
button .gaz.stop -text "Stop GY" -command { cmd "a.tgazyaw.stop();a.resetGazYaw()" }
pack .gaz.stop

### YAW ###

frame .yaw -borderwidth 3 -relief raised
pack .yaw -side left

label .yaw.lbl -text "YAW"
pack .yaw.lbl

scale .yaw.p  -label "yaw P" \
-length 160 -from 0 -to 2 \
-command { setParam "a.yawPidParam.p" } \
-resolution 0.01 \
-digits 3 \
-variable yawpval \
-showvalue 1 -orient horizontal
pack .yaw.p

scale .yaw.i  -label "yaw I" \
-length 160 -from 0 -to 2 \
-command { setParam "a.yawPidParam.i" } \
-resolution 0.01 \
-digits 3 \
-variable yawival \
-showvalue 1 -orient horizontal
pack .yaw.i

scale .yaw.d -label "yaw D" \
-length 160 -from -2 -to 2 \
-command { setParam "a.yawPidParam.d" } \
-resolution 0.01 \
-digits 3 \
-variable yawdval \
-showvalue 1 -orient horizontal
pack .yaw.d

### ROLL ###

frame .roll -borderwidth 3 -relief raised
pack .roll -side left

label .roll.lbl -text "ROLL"
pack .roll.lbl

scale .roll.p  -label "roll P" \
-length 160 -from 0 -to 1 \
-command { setParam "a.rollPidParam.p" } \
-resolution 0.005 \
-digits 4 \
-variable rollpval \
-showvalue 1 -orient horizontal
pack .roll.p

scale .roll.i  -label "roll I" \
-length 160 -from 0 -to 1 \
-command { setParam "a.rollPidParam.i" } \
-resolution 0.005 \
-digits 4 \
-variable rollival \
-showvalue 1 -orient horizontal
pack .roll.i

scale .roll.d -label "roll D" \
-length 160 -from -1 -to 1 \
-command { setParam "a.rollPidParam.d" } \
-resolution 0.005 \
-digits 4 \
-variable rolldval \
-showvalue 1 -orient horizontal
pack .roll.d

scale .roll.int -state disabled -label "roll interval" \
-length 160 -from 0 -to 2 \
-command { setParam "a.rollInterval" } \
-resolution 0.05 \
-digits 3 \
-variable rollint \
-showvalue 1 -orient horizontal
pack .roll.int

scale .roll.sleep -state disabled -label "roll sleep" \
-length 160 -from 0 -to 1 \
-command { setParam "a.rollSleep" } \
-resolution 0.01 \
-digits 3 \
-variable rollsleep \
-showvalue 1 -orient horizontal
pack .roll.sleep


### PITCH ###

frame .pitch -borderwidth 3 -relief raised
pack .pitch -side left

label .pitch.lbl -text "PITCH"
pack .pitch.lbl

scale .pitch.p  -label "pitch P" \
-length 160 -from 0 -to 1 \
-command { setParam "a.pitchPidParam.p" } \
-resolution 0.005 \
-digits 4 \
-variable pitchpval \
-showvalue 1 -orient horizontal
pack .pitch.p

scale .pitch.i  -label "pitch I" \
-length 160 -from 0 -to 1 \
-command { setParam "a.pitchPidParam.i" } \
-resolution 0.005 \
-digits 4 \
-variable pitchival \
-showvalue 1 -orient horizontal
pack .pitch.i

scale .pitch.d -label "pitch D" \
-length 160 -from -1 -to 1 \
-command { setParam "a.pitchPidParam.d" } \
-resolution 0.005 \
-digits 4 \
-variable pitchdval \
-showvalue 1 -orient horizontal
pack .pitch.d

scale .pitch.int -state disabled -label "pitch interval" \
-length 160 -from 0 -to 2 \
-command { setParam "a.pitchInterval" } \
-resolution 0.05 \
-digits 3 \
-variable pitchint \
-showvalue 1 -orient horizontal
pack .pitch.int

scale .pitch.sleep -state disabled -label "pitch sleep" \
-length 160 -from 0 -to 1 \
-command { setParam "a.pitchSleep" } \
-resolution 0.01 \
-digits 3 \
-variable pitchsleep \
-showvalue 1 -orient horizontal
pack .pitch.sleep

### MIscellaneous ###

frame .misc -borderwidth 3 -relief raised
pack .misc -side right

scale .misc.distaging -label "Distance Aging" \
-length 150 -from 0 -to 1 \
-command { setParam "s.distanceAging" } \
-resolution 0.05 \
-digits 3 \
-variable distagingval \
-showvalue 1 -orient horizontal
pack .misc.distaging

scale .misc.devaging -label "Deviation Aging" \
-length 150 -from 0 -to 1 \
-command { setParam "s.deviationAging" } \
-resolution 0.05 \
-digits 3 \
-variable devaging \
-showvalue 1 -orient horizontal
pack .misc.devaging

scale .misc.curvaging -label "Curvature aging" \
-length 150 -from 0 -to 1 \
-command { setParam "s.curvatureAging" } \
-resolution 0.05 \
-digits 3 \
-variable curvaging \
-showvalue 1 -orient horizontal
pack .misc.curvaging

scale .misc.hsvratio -label "Auto HSV Range Ratio" \
-length 150 -from 0 -to 0.5 \
-command { setParam "f.autoHsvValueRangeRatio" } \
-resolution 0.005 \
-digits 3 \
-variable hsvratio \
-showvalue 1 -orient horizontal
pack .misc.hsvratio

scale .misc.houghminlenght -label "Hough Min Length" \
-length 150 -from 0 -to 200 \
-command { setParam "f.houghMinLength" } \
-resolution 1 \
-digits 3 \
-variable houghminlenght \
-showvalue 1 -orient horizontal
pack .misc.houghminlenght

scale .misc.houghmaxgap -label "Hough Max Gap" \
-length 150 -from 0 -to 200 \
-command { setParam "f.houghMaxGap" } \
-resolution 1 \
-digits 3 \
-variable houghmaxgap \
-showvalue 1 -orient horizontal
pack .misc.houghmaxgap

scale .misc.houghrho -label "Hough Rho" \
-length 150 -from 0 -to 10 \
-command { setParam "f.houghRho" } \
-resolution 0.5 \
-digits 3 \
-variable houghrho \
-showvalue 1 -orient horizontal
pack .misc.houghrho

# MISC 2

frame .misc2 -borderwidth 3 -relief raised
pack .misc2 -side right

scale .misc2.hystcenterthresh -label "Hystersis Distance Threshhold" \
-length 150 -from 0 -to 1 \
-command { setParam "f.hystDistanceThr" } \
-resolution 0.05 \
-digits 3 \
-variable hystcenterthresh \
-showvalue 1 -orient horizontal
pack .misc2.hystcenterthresh

scale .misc2.hystdirthreshbs -label "Hyst. Dev. Thr. Base" \
-length 150 -from 0 -to 1 \
-command { setParam "m.hystDeviationThrBase" } \
-resolution 0.05 \
-digits 3 \
-variable hystdirthreshbs \
-showvalue 1 -orient horizontal
pack .misc2.hystdirthreshbs

scale .misc2.hystdirthreshrt -label "Hyst. Dev. Thr. Rate" \
-length 150 -from 0 -to 100 \
-command { setParam "m.hystDeviationThrRate" } \
-resolution 5 \
-digits 3 \
-variable hystdirthreshrt \
-showvalue 1 -orient horizontal
pack .misc2.hystdirthreshrt

scale .misc2.imuweight -label "IMU line weight" \
-length 150 -from 0 -to 1 \
-command { setParam "m.imuWeight" } \
-resolution 0.05 \
-digits 3 \
-variable imuweight \
-showvalue 1 -orient horizontal
pack .misc2.imuweight

scale .misc2.curva -label "Curvature tolerance" \
-length 150 -from 1 -to 4 \
-command { setParam "f.curvatureTolerance" } \
-resolution 0.25 \
-digits 3 \
-variable curva \
-showvalue 1 -orient horizontal
pack .misc2.curva

#scale .cameraparam -label "Camera param" \
#-length 150 -from 0.01 -to 2 \
#-command { setParam "f.cameraParam" } \
#-resolution 0.025 \
#-digits 3 \
#-variable cameraparam \
#-showvalue 1 -orient horizontal
#pack .cameraparam
#
scale .misc.camerascale -label "Camera scale" \
-length 150 -from 0.0001 -to 0.0020 \
-command { setParam "k.cameraScale" } \
-resolution 0.00005 \
-digits 4 \
-variable camerascale \
-showvalue 1 -orient horizontal
pack .misc.camerascale

#label .lbl -textvariable val
#pack .lbl

##################
# Default values #
##################


set gazpval 0.5
set gazival 0
set gazdval 0

set yawpval 0.9
set yawival 0.0
set yawdval 0

set rollpval 0.1
set rollival 0.0
set rolldval 0.0
#set rollint 1
#set rollsleep 0.4
#set rollthreshold 0.1

set pitchpval 0.110
set pitchival 0.0
set pitchdval 0.0
#set pitchint 0.9
#set pitchsleep 0.6
#set pitchdistance 0.2
#set pitchdeviation 0.15

set frontierradius 0.7
set rollpitchint 2
set rollpitchsleep 0.54

# MISC
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



