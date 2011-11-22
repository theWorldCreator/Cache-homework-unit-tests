CC = gcc
CFLAGS =  -lcunit -O2 -Wall -Werror -Wformat-security -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wempty-body -Wlogical-op -Wstrict-prototypes -Wold-style-declaration -Wold-style-definition -Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs -Wno-pointer-sign -std=gnu99

all : proj test_parse_cfg

test_parse_cfg : test_parse_cfg.c ../parse_config.o ../common.o
	$(CC) test_parse_cfg.c ../parse_config.o ../common.o -o test_parse_cfg $(CFLAGS)

proj : 
	cd ../; make
