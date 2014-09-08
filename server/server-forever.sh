#!/bin/bash
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"


while true ; do
	echo `date` "Starting server..." >>/tmp/xcs-server-forever.log
	nodejs $SCRIPTPATH/server.js
	sleep 3
done

