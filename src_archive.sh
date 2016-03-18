#!/bin/sh

VERSION=$(cat utv_core/version.h | perl -ne '$_{$1}=$2 if /^\#define\s+UTVIDEO_VERSION_(\w+)\s+(\w+)/;END{print "$_{MAJOR}.$_{MINOR}.$_{BUGFIX}\n"}')
git archive --format=zip --prefix=utvideo-${VERSION}/ -o utvideo-${VERSION}-src.zip utvideo-${VERSION}
