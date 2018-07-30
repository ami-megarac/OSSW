#--------------------------------------------------------------------------------------------------------------------
#                      Makefile to build ipmi apml
#                 Copyright (c) 2012 American Megatrends Inc 
#---------------------------------------------------------------------------------------------------------------------

#------- Comment/uncomment the following line to enable/disable debugging---------------
DEBUG = n
#---------------------------------------------------------------------------------------------------------------------
#---------always set target as below if you are part of libami----------------------------------------
LIBRARY_NAME 	= libopenapml
#---------------------------------------------------------------------------------------------------------------------
#---------------------- Change according to your files ----------------------------------------------------
SRC   =  Apml_fdk.c User_platform.c

CFLAGS += -I${SPXINC}/unix
CFLAGS += -I${SPXINC}/dbgout
CFLAGS += -I${SPXINC}/global
CFLAGS += -I${SPXINC}/ipmi
CFLAGS += -I${SPXINC}/i2c
CFLAGS += -I${SPXINC}/pdk
CFLAGS += -I${SPXINC}/ipmihalapi
CFLAGS += -I${SPXINC}/featuredefine
CFLAGS += -I${SPXINC}/network
CFLAGS += -I${SPXINC}/hostname

include ${TOOLDIR}/rules/Rules.make.libs
