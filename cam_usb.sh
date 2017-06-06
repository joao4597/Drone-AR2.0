killall -9 program.elf.respawner.sh
killall -9 program.elf
mv /dev/ttyUSB0 /dev/ttyOUTRO
mv /tmp/udev/dev/ttyUSB0 /tmp/udev/dev/ttyOUTRO
insmod v4l2loopback.ko
mv /dev/video1 /dev/videoC
mv /dev/video7 /dev/video1
/bin/program.elf.respawner.sh &