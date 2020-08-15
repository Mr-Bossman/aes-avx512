CC           = /opt/intel/bin/icc
LD           = /opt/intel/bin/icc
AR           = ar
ARFLAGS      = rcs
CFLAGS       = -std=c11 -Wall -Wextra -Wpedantic -Os -mmic
SRCS=main.c haraka.c aes.c
OUTFILE="main"
# include path to AVR library

default: main

.SILENT:
.PHONY:  clean


all: main

main : $(SRCS)
	$(CC) $(CFLAGS)  $(SRCS) -o $(OUTFILE)

clean:
	

test:
	make clean && make && sudo rm /mnt/main & sudo cp ./main /mnt/main && echo "cd / && ./micNfs/main" | ssh phi@192.168.0.111

