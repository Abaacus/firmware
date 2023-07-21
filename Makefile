CUSTOM_COMMANDS = all clean autogen init lint load connect gdb test
BOARDS = bmu pdu dcu vcu wsb wsbfl wsbfr wsbrr wsbrl beaglebone

.PHONY: $(CUSTOM_COMMANDS) $(BOARDS) 

all: bmu dcu pdu vcu wsb
 
beaglebone:;
	make -C beaglebone/os/

dashboard:;
	# Added this here as the dashboard uses qmake to generated its Makefile
	mkdir -p Bin/dashboard/Src
	qmake -makefile -o beaglebone/app/wfe/dashboard/Makefile beaglebone/app/wfe/dashboard/dashboard.pro
	make -C beaglebone/app/wfe/dashboard/

include bmu/board.mk
include dcu/board.mk
include pdu/board.mk
include vcu/board.mk
include wsb/board.mk
