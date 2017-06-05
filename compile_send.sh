#DRK_ROOT=/home/rafaz10/teste/dronerk

RED='\033[0;31m'
BLUE='\033[;34m'
NC='\033[0m' # No Color



#!/bin/sh
HERE=$(pwd)
HOST=192.168.1.$1
FILE0='libdrk_ARM.so' 
#FILE1=$(pwd | awk -F "/" '{print $NF}') #use folder name
FILE1=video_tutorial
FILE1=${FILE1}_ARM

# compile drk lib
godrk
make VERBOSE=y
echo " --- " ${FILE0} " done --- "

#compile and link this
cd ${HERE}
make VERBOSE=y
echo " --- " ${FILE1} " app done --- "


cp ${DRK_ROOT}/lib/${FILE0} .
#echo id $1 > myid.txt
#printf "${BLUE}${FILE0} copied to cur folder${NC}\n"
printf "${BLUE}Sending both...${NC}"
ftp -n $HOST <<END_SCRIPT
quote USER 'asd'
binary
put ${FILE0}
put ${FILE1}
quit
END_SCRIPT
printf "${BLUE}Sent!${NC}\n"
rm ${FILE0}
#printf "${BLUE}${FILE0} removed from cur folder${NC}\n"


printf "${BLUE}Setting executable permissions to both${NC}\n"

telnet $HOST <<END_SCRIPT
chmod +x /data/video/$FILE0
chmod +x /data/video/$FILE1
mv /data/video/$FILE0 /usr/lib/$FILE0
rm /data/video/$FILE0
quit
END_SCRIPT

printf "${BLUE}Installed in the drone${NC}\n"



