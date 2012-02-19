# needed for makefile, packages, etc
ROBOTPY_BASE=/WindRiver/workspace/robotpy

PROJECT_TARGET = _basket_tracker.pyd

PROJECT_OBJECTS = cpp/BasketTracker.o

PROJECT_OBJECTS += sip_basket_trackercmodule.o
PROJECT_OBJECTS += sip_basket_trackerBasketTracker.o
PROJECT_OBJECTS += sip_basket_trackerTrackingData.o

ADDED_INCLUDES = 	-I. \
					-Icpp \
					-I$(ROBOTPY_BASE)/Packages/sip \
					-I$(ROBOTPY_BASE)/Packages/wpilib \
					-I$(ROBOTPY_BASE)/Packages/wpilib/WPILib \
					-I$(ROBOTPY_BASE)/Packages/Vision2009 \
					-I$(ROBOTPY_BASE)/Packages/nivision \
					-I$(ROBOTPY_BASE)/Packages/vision \
					-I$(ROBOTPY_BASE)/Packages/vision/Vision

ADDED_CFLAGS = -D'SVN_REV="2993"'
ADDED_C++FLAGS = -D'SVN_REV="2993"'

include $(ROBOTPY_BASE)/mk/module.mk
