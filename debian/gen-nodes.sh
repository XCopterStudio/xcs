#!/bin/bash

self=`dirname ${BASH_SOURCE[0]}`
DIR=`cd "$PWD/$self" && pwd`

for node in `find $DIR/../onboard/src/xcs/nodes -type d -name "*.xob" | sort` ; do
	node=`basename $node`
	node=${node%.xob}
	node=${node/_/-}
	pkg_name=xcs-node-${node}
	cat <<EOD
Package: $pkg_name
Architecture: all
Depends: \${shlibs-node-$node:Depends}
Description: XCS node ${node}...

EOD

	echo "lib/xcs/xobjects/x${node}.so usr/lib/xcs/xobjects" > ${pkg_name}.install
done
