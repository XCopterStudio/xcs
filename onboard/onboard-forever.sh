#!/bin/bash
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

echo $SCRIPTPATH
. $SCRIPTPATH/env

while true ; do
	echo `date` "Starting onboard..." >>/tmp/xcs-onboard-forever.log
	urbi-launch -s -- onboard.u
	sleep 3
done

