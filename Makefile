# the name of this project

DRK_ROOT=/home/rafaz10/Desktop/parrot/dronerk

ifeq ($(X86),YES)
	PROJECT=video_tutorial_X86
else
	PROJECT=video_tutorial_ARM
endif

PROJ_DIR=/home/rafaz10/git/Drone-AR2.0/
# the makefile is useless without the next line!
include $(DRK_ROOT)/src/projects/projects.mk
