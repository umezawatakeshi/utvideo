#!/bin/sh

cd `dirname $0`
REPOROOT=../../..

VERSION=$(cat ${REPOROOT}/utv_core/version.h | perl -ne '$_{$1}=$2 if /^\#define\s+UTVIDEO_VERSION_(\w+)\s+(\w+)/;END{print "$_{MAJOR}.$_{MINOR}.$_{BUGFIX}\n"}')

TAG=utvideo-${VERSION}
ZIPFNAME=${TAG}-win.zip
LOGFNAME=${TAG}-log-win.zip

if [ -e "output/${ZIPFNAME}" ]; then
    exec echo "output/${ZIPFNAME} already exists. Not overwritten."
fi
if [ -e "output/${LOGFNAME}" ]; then
    exec echo "output/${LOGFNAME} already exists. Not overwritten."
fi

rm -rf zip/${TAG}
mkdir -p zip/${TAG}
mkdir zip/${TAG}/x86
mkdir zip/${TAG}/x64
cp ${REPOROOT}/Release/{utv_core.dll,utv_vcm.dll,utv_dmo.dll} zip/${TAG}/x86/
cp ${REPOROOT}/x64/Release/{utv_core.dll,utv_vcm.dll,utv_dmo.dll} zip/${TAG}/x64/
cp ${REPOROOT}/readme.{en,ja}.html zip/${TAG}/
cp ${REPOROOT}/{style.css,gplv2.rtf,gplv2.txt,gplv2.ja.sjis.txt} zip/${TAG}/
cp install.bat uninstall.bat globalconfig.bat zip/${TAG}/
cp readme-zip.{en,ja}.txt zip/${TAG}/
cp utvideo-{x86,x64}{,-unreg}.reg zip/${TAG}/

rm -rf log/${TAG}-log
mkdir -p log/${TAG}-log
cp ${REPOROOT}/Release/utv_logc.exe log/${TAG}-log/
cp ${REPOROOT}/readme.{en,ja}.html log/${TAG}-log/

mkdir -p output
(cd zip; exec powershell Compress-Archive -CompressionLevel Optimal -DestinationPath ../output/${ZIPFNAME} ${TAG})
(cd log; exec powershell Compress-Archive -CompressionLevel Optimal -DestinationPath ../output/${LOGFNAME} ${TAG}-log)
