##############################################################################
# Variables
GCC = gcc
HUAWEI_DIR = ./huawei/
AT_DIR = ./huawei/at
CFLAGS = -I. -I$(HUAWEI_DIR) -I$(AT_DIR)  
# Output
OUTPROG = amachine
# Run variables
CMDS_DEV = /dev/ttyUSB0
AUDIO_DEV = /dev/ttyUSB1
AUDIO_FILE = ./audio/fran.wav
RUN_CMD = ./$(OUTPROG) $(CMDS_DEV) $(AUDIO_DEV) $(AUDIO_FILE)
# Sources files
SRC_FILES = huawei/at/at_a.c  huawei/at/at_chup.c  huawei/at/at_clcc.c  \
            huawei/at/at_cvoice.c  huawei/at/at_d.c huawei/at/at_ddsetex.c \
            util.c huawei/modem.c main.c 
# Generated object files
O_FILES = $(SRC_FILES:%.c=%.o)
# Generated auto-dependency files
D_FILES = $(SRC_FILES:%.c=%.d)

##############################################################################
# Targets

# Make all
all:  $(OUTPROG) 

# Make all and run
run:  $(OUTPROG)
	$(RUN_CMD)

# Object generation
%.o: %.c
	$(GCC) $(CFLAGS) -MMD -MP -o $@ -c $<

# Include dependencies
-include $(D_FILES)

# Output generation
$(OUTPROG): $(O_FILES)
	$(GCC) -o $@ $^ 

.PHONY: clean

# Clean all
clean:
	rm -f *.o $(O_FILES) $(D_FILES) $(OUTPROG) 
