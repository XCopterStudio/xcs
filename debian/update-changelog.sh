#!/bin/bash

VERSION=`git describe --tags`
DATE=`date -R`

cat - changelog >changelog.tmp <<EOD
xcs ($VERSION) unstable; urgency=low

  * Automatic changelog entry.

 -- Michal Koutný  <xm.koutny@gmail.com> $DATE

EOD

mv changelog.tmp changelog

