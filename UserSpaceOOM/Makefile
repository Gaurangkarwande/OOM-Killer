.PHONY: all clean

CC = gcc
INCLUDES = -I .
TARGET = userOOM
MAP_FILE = userOOM.map

CC_OPTIONS = -Wall -O2 -g
LD_FLAGS = -Xlinker -Map=$(MAP_FILE)

OBJ_FILES = main.o get_process_statistics.o parse_oom_notifier.o user_space_kill.o

.SILENT: clean all %.o

all: ${OBJ_FILES}
	@echo "Building ${TARGET} ..."
	${CC} ${CC_OPTIONS} ${LD_FLAGS} ${OBJ_FILES} -o ${TARGET}

%.o: %.c
	@echo "Compiling $*.o ..."
	${CC} ${CC_OPTIONS} $(LD_FLAGS) -c $*.c ${INCLUDES}

clean:
	@echo "Cleaning $(OBJ_FILES) $(TARGET) $(MAP_FILE)..."
	-rm -rf	 *.o
	-rm -rf	 userOOM.map
	-rm -rf	 userOOM