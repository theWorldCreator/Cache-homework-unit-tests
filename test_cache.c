#include "CUnit/Basic.h"
#include "../statistics.h"
#include "../memory.h"
#include "../cache.h"

#include <stdio.h>
#include <unistd.h>

void direct_cache_through_typicall(void) 
{
	AbstractMemory *cache;
	AbstractMemory *mem;
	StatisticsInfo *info;
	ConfigFile *cfg;
	MemoryCell cells[8]; 
	char conf_file[] = "test_config_file.txt";
	FILE *cfg_f = NULL;
	cfg_f = fopen(conf_file, "w");
	fprintf(cfg_f, "write_strategy = write-through\n block_size = 64\n\
	cache_size = %d\n cache_read_time = 1\n cache_write_time = 2\n\
	memory_size = %d\n memory_read_time = 4\n memory_write_time = 8\n\
	memory_width = 16\n associativity = direct\n", KiB, 16 * KiB);
	fclose(cfg_f);
	
	cfg = config_file_parse(conf_file, stderr);
	unlink(conf_file);
	
	info = statistics_create(cfg);
	
	mem = memory_create(cfg, "", info);
	
	cache = cache_create(cfg, "", info, mem);
	CU_ASSERT_NOT_EQUAL(cache, NULL);
	cache->ops->read(cache, 15, 1, cells);
	
	CU_ASSERT_EQUAL(cells[0].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 17); //FIXME: maybe 19??
	//Should I consider cache write time here?
	
	cells[0].flags = 1;
	cells[0].value = 0xFF;
	cells[1].flags = 1;
	cells[1].value = 183;
	cells[2].flags = 0;
	cache->ops->write(cache, 1, 2, cells);
	CU_ASSERT_EQUAL(info->clock_counter, 27);
	
	memset(cells, 0, sizeof(cells));
	
	cache->ops->read(cache, 1, 2, cells);
	CU_ASSERT_EQUAL(cells[0].value, 0xFF);
	CU_ASSERT_EQUAL(cells[0].flags, 1);
	CU_ASSERT_EQUAL(cells[1].value, 183);
	CU_ASSERT_EQUAL(cells[1].flags, 1);
	CU_ASSERT_EQUAL(cells[2].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 28);
	
	// FIXME: add more tests
}

void direct_cache_back_typicall(void) 
{
	
}

void
make_cache_suite(void)
{
	CU_pSuite suite;
    suite = CU_add_suite("cache_suite", NULL, NULL);
    CU_ADD_TEST(suite, direct_cache_through_typicall);
    CU_ADD_TEST(suite, direct_cache_back_typicall);
}
