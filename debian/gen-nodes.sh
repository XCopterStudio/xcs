#!/bin/bash

self=`dirname ${BASH_SOURCE[0]}`
DIR=`cd "$PWD/$self" && pwd`

for node in `find $DIR/../onboard/src/xcs/nodes -type d -name "*.xob" | sort` ; do
	node=`basename $node`
	libname=${node%.xob}
	node=${libname/_/-}
	pkg_name=xcs-node-${node}
	packages="$packages,
  $pkg_name"
	cat <<EOD
Package: $pkg_name
Architecture: any
Depends: \${shlibs-node-$node:Depends}
Description: XCS node ${node}...

EOD

	echo "lib/xcs/xobjects/x${libname}.so usr/lib/xcs/xobjects" > ${pkg_name}.install
done

cat <<EOD
Package: xcs-nodes-all
Architecture: all
Depends: ${packages#,
  }
Description: Metapackage of all XCS nodes

EOD
