#include "CUnit/Basic.h"
#include "../common.h"
#include "../parse_config.h"
#include "../abstract_memory.h"
#include "../memory.h"

#include <stdio.h>
#include <unistd.h>

void memory_wrong_data(void) 
{
	// FIXME: how test wrong data if programm will terminanted on it?
}

void memory_typicall(void) 
{
	AbstractMemory *mem;
	StatisticsInfo *info;
	ConfigFile *cfg;
	MemoryCell cells[8]; 
	char conf_file[] = "test_config_file.txt";
	FILE *cfg_f = NULL;
	cfg_f = fopen(conf_file, "w");
	fprintf(cfg_f, "memory_size = %d\n memory_read_time = 4\n memory_write_time = 8\n memory_width = 4\n", 16 * KiB);
	fclose(cfg_f);
	
	cfg = config_file_parse(conf_file, stderr);
	unlink(conf_file);
	
	info = statistics_create(cfg);
	
	mem = memory_create(cfg, "", info);
	CU_ASSERT_NOT_EQUAL(mem, NULL);
	
	mem->ops->read(mem, 15, 1, cells);
	
	CU_ASSERT_EQUAL(cells[0].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 4);
	
	mem->ops->read(mem, 13, 4, cells);
	
	CU_ASSERT_EQUAL(cells[0].flags, 0);
	CU_ASSERT_EQUAL(cells[1].flags, 0);
	CU_ASSERT_EQUAL(cells[2].flags, 0);
	CU_ASSERT_EQUAL(cells[3].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 8);
	
	mem->ops->read(mem, 13, 8, cells);
	
	CU_ASSERT_EQUAL(info->clock_counter, 16);
	
	
	cells[0].flags = 1;
	cells[0].value = 0xFF;
	cells[1].flags = 1;
	cells[1].value = 183;
	cells[2].flags = 0;
	mem->ops->write(mem, 1, 2, cells);
	CU_ASSERT_EQUAL(info->clock_counter, 24);
	
	memset(cells, 0, sizeof(cells));
	
	mem->ops->read(mem, 1, 2, cells);
	CU_ASSERT_EQUAL(cells[0].value, 0xFF);
	CU_ASSERT_EQUAL(cells[0].flags, 1);
	CU_ASSERT_EQUAL(cells[1].value, 183);
	CU_ASSERT_EQUAL(cells[1].flags, 1);
	CU_ASSERT_EQUAL(cells[2].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 28);
	
	
	
	cells[0].flags = 1;
	cells[0].value = 0;
	cells[1].flags = 1;
	cells[1].value = 73;
	cells[2].flags = 0;
	mem->ops->reveal(mem, 0, 2, cells);
	CU_ASSERT_EQUAL(info->clock_counter, 28);
	
	memset(cells, 0, sizeof(cells));
	
	mem->ops->read(mem, 0, 4, cells);
	CU_ASSERT_EQUAL(cells[0].value, 0);
	CU_ASSERT_EQUAL(cells[0].flags, 1);
	CU_ASSERT_EQUAL(cells[1].value, 73);
	CU_ASSERT_EQUAL(cells[1].flags, 1);
	CU_ASSERT_EQUAL(cells[2].value, 183);
	CU_ASSERT_EQUAL(cells[2].flags, 1);
	CU_ASSERT_EQUAL(cells[3].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 32);
	
	
	// FIXME: add tests
	
	
	CU_ASSERT_EQUAL(mem->ops->free(mem), NULL);
}

void
make_memory_suite(void)
{
	CU_pSuite suite;
    suite = CU_add_suite("memory_suite", NULL, NULL);
    CU_ADD_TEST(suite, memory_wrong_data);
    CU_ADD_TEST(suite, memory_typicall);
}
