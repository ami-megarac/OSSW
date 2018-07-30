DEBUG := n
TARGET := mctppcie_hw

OBJS := ast_mctppcie.o 

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/mctppcie

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/mctppcie/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers

