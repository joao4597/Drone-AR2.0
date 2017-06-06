# the name of this project
DRK_ROOT=/home/joao/Documents/dronerk-master-59b5de053886b088f84c6177b3f04be0a9e63812

ifeq ($(X86),YES)
	PROJECT=video_tutorial_X86
else
	PROJECT=video_tutorial_ARM
endif

PROJ_DIR=/home/joao/Documents/dronerk-master-59b5de053886b088f84c6177b3f04be0a9e63812/src/projects/experimental/video_tutorial/Drone-AR2.0
# the makefile is useless without the next line!
include $(DRK_ROOT)/src/projects/projects.mk
