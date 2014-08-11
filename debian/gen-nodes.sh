#!/bin/bash

self=`dirname ${BASH_SOURCE[0]}`
DIR=`cd "$PWD/$self" && pwd`

for node in `find $DIR/../onboard/src/xcs/nodes -type d -name "*.xob" | sort` ; do
	node=`basename $node`
	node=${node%.xob}
	pkg_name=xcs-node-${node/_/-}
	cat <<EOD
Package: $pkg_name
Architecture: all
Depends: TODO
Description: XCS node ${node}...

EOD

	echo "lib/xcs/xobjects/x${node}.so usr/lib/xcs/xobjects" > ${pkg_name}.install
done
