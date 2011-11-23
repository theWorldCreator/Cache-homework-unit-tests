CC = gcc
CFLAGS =  -lcunit -O2 -Wall -Werror -Wformat-security -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wempty-body -Wlogical-op -Wstrict-prototypes -Wold-style-declaration -Wold-style-definition -Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs -Wno-pointer-sign -std=gnu99

CFILES = test_parse_cfg.c test_statistics.c test_trace.c test_memory.c test_cache.c
HFILES = test_parse_cfg.h test_statistics.h test_trace.h test_memory.h test_cache.h

all : proj main

main : main.c test_cache.o ../cache.o ../direct_cache.o test_memory.o ../memory.o test_trace.o ../trace.o test_statistics.o ../statistics.o test_parse_cfg.o ../parse_config.o ../common.o
	$(CC) $^ -o main $(CFLAGS)
	
deps.make : $(CFILES) $(HFILES)
	$(CC) -MM $(CFILES) > deps.make


-include deps.make

proj : 
	cd ../; make
