#!/bin/sh

DRK_ROOT=/home/rafaz10/Desktop/parrot/dronerk

cd $DRK_ROOT/src/libs/drk_api/
make VERBOSE=y #LOGDATA=y

cd /home/rafaz10/git/Drone-AR2.0/
make VERBOSE=y #LOGDATA=y
RED='\033[0;31m'
BLUE='\033[;34m'
NC='\033[0m' # No Color
printf "${BLUE}Project built!${NC}\n"

HOST=192.168.1.$1
FILE0=libdrk_ARM.so
FILE1=main 

cp $DRK_ROOT/lib/$FILE0 .

printf "Sending binary ${RED}$FILE0${NC} and ${RED}$FILE1${NC} to Drone $1..."
ftp -n $HOST <<END_SCRIPT
quote USER 'asd'
binary
put $FILE0
put $FILE1

quit
END_SCRIPT
rm $FILE0

printf "${BLUE}Sent${NC}\n"

printf "${BLUE}Setting executable permissions to both${NC}\n"

telnet $HOST <<END_SCRIPT
chmod +x /data/video/$FILE0
chmod +x /data/video/$FILE1
mv /data/video/$FILE0 /usr/lib/$FILE0
rm /data/video/$FILE0
quit
END_SCRIPT

printf "${BLUE}Done${NC}\n"
exit 0
