#!/bin/sh

VERSION=$(cat utv_core/version.h | perl -ne '$_{$1}=$2 if /^\#define\s+UTVIDEO_VERSION_(\w+)\s+(\w+)/;END{print "$_{MAJOR}.$_{MINOR}.$_{BUGFIX}\n"}')

TAG=utvideo-${VERSION}
FNAME=${TAG}-src.zip

if [ -e "${FNAME}" ]; then
    echo "${FNAME} already exists. Not overwritten."
    exit 1
fi

echo "Archiving to ${FNAME}..."
git archive --format=zip --prefix=${TAG}/ -o ${FNAME} ${TAG}
