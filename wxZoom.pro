# this is the project file for the wxZoom application

# we generate the VC++ IDE project file, comment this line
# to generate a makefile for (n)make
TEMPLATE = vcapp

# wx is mandatory for wxWindows projects
CONFIG  = wx warn_on

# project files
SOURCES = src/wxZoom.cpp
RC_FILE = wxZoom.rc
TARGET = wxZoom

