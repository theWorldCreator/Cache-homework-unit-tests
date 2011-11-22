CC = gcc
CFLAGS =  -lcunit -O2 -Wall -Werror -Wformat-security -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wempty-body -Wlogical-op -Wstrict-prototypes -Wold-style-declaration -Wold-style-definition -Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs -Wno-pointer-sign -std=gnu99

CFILES = test_parse_cfg.c
HFILES = test_parse_cfg.h

all : proj main

main : main.c test_statistics.o ../statistics.o test_parse_cfg.o ../parse_config.o ../common.o
	$(CC) $^ -o main $(CFLAGS)
	
deps.make : $(CFILES) $(HFILES)
	$(CC) -MM $(CFILES) > deps.make


-include deps.make

proj : 
	cd ../; make
