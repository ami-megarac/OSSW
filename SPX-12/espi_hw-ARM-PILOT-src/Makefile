DEBUG := n
TARGET := espi_hw

OBJS := espimain_hw.o 

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/espi
EXTRA_CFLAGS += -I${SPXINC}/reset

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/espi/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers

