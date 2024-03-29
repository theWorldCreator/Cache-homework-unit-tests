#include "CUnit/Basic.h"
#include "../parse_config.h"
#include "../random.h"
#include "../statistics.h"
#include "../memory.h"
#include "../cache.h"

#include <stdio.h>
#include <unistd.h>

void direct_cache_wt_typicall(void) 
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
	info->hit_counter_needed = 1;
	
	mem = memory_create(cfg, "", info);
	
	cache = cache_create(cfg, "", info, mem, NULL);
	CU_ASSERT_NOT_EQUAL(cache, NULL);
	cache->ops->read(cache, 15, 1, cells);
	
	CU_ASSERT_EQUAL(cells[0].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 17);
	
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
	CU_ASSERT_EQUAL(info->hit_counter, 1);
	
	cache->ops->read(cache, 2, 1, cells);
	CU_ASSERT_EQUAL(cells[0].value, 183);
	CU_ASSERT_EQUAL(cells[0].flags, 1);
	CU_ASSERT_EQUAL(info->clock_counter, 29);
	CU_ASSERT_EQUAL(info->hit_counter, 2);
	
	cache->ops->read(cache, 5, 1, cells);
	CU_ASSERT_EQUAL(cells[0].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 30);
	CU_ASSERT_EQUAL(info->hit_counter, 3);
	
	cache->ops->read(cache, 64, 1, cells);
	CU_ASSERT_EQUAL(cells[0].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 47);
	
	cache->ops->read(cache, 64, 1, cells);
	CU_ASSERT_EQUAL(cells[0].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 48);
	
	
	cache->ops->read(cache, 1024, 1, cells);
	CU_ASSERT_EQUAL(cells[0].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 65);
	
	cells[0].flags = 1;
	cells[0].value = 0xFF;
	cache->ops->write(cache, 5, 2, cells);
	CU_ASSERT_EQUAL(info->clock_counter, 73);
	
	// FIXME: add more tests
	
	
	CU_ASSERT_EQUAL(cache->ops->free(cache), NULL);
	mem->ops->free(mem);
	statistics_free(info);
	config_file_free(cfg);
}

void direct_cache_wb_typicall(void) 
{
	// Most of the write-back cache code is simular to write through,
	// exept cache->ops->write func, so we will test only it
	
	AbstractMemory *cache;
	AbstractMemory *mem;
	StatisticsInfo *info;
	ConfigFile *cfg;
	MemoryCell cells[8]; 
	char conf_file[] = "test_config_file.txt";
	FILE *cfg_f = NULL;
	cfg_f = fopen(conf_file, "w");
	fprintf(cfg_f, "write_strategy = write-back\n block_size = 64\n\
	cache_size = %d\n cache_read_time = 1\n cache_write_time = 2\n\
	memory_size = %d\n memory_read_time = 4\n memory_write_time = 8\n\
	memory_width = 16\n associativity = direct\n", KiB, 16 * KiB);
	fclose(cfg_f);
	
	cfg = config_file_parse(conf_file, stderr);
	unlink(conf_file);
	
	info = statistics_create(cfg);
	info->hit_counter_needed = 1;
	info->write_back_needed = 1;
	
	mem = memory_create(cfg, "", info);
	
	cache = cache_create(cfg, "", info, mem, NULL);
	CU_ASSERT_NOT_EQUAL(cache, NULL);
	
	cells[0].flags = 1;
	cells[0].value = 0xFF;
	cells[1].flags = 1;
	cells[1].value = 183;
	cells[2].flags = 0;
	cache->ops->write(cache, 1, 2, cells);
	CU_ASSERT_EQUAL(info->clock_counter, 18);
	CU_ASSERT_EQUAL(info->write_back_counter, 0);
	
	memset(cells, 0, sizeof(cells));
	cache->ops->read(cache, 1, 2, cells);
	CU_ASSERT_EQUAL(cells[0].value, 0xFF);
	CU_ASSERT_EQUAL(cells[0].flags, 1);
	CU_ASSERT_EQUAL(cells[1].value, 183);
	CU_ASSERT_EQUAL(cells[1].flags, 1);
	CU_ASSERT_EQUAL(cells[2].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 19);
	CU_ASSERT_EQUAL(info->hit_counter, 1);
	
	memset(cells, 0, sizeof(cells));
	mem->ops->read(mem, 1, 2, cells);
	CU_ASSERT_EQUAL(cells[0].flags, 0);
	CU_ASSERT_EQUAL(cells[1].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 23);
	CU_ASSERT_EQUAL(info->hit_counter, 1);
	
	
	cells[0].flags = 1;
	cells[0].value = 5;
	cache->ops->write(cache, 2048, 1, cells);
	CU_ASSERT_EQUAL(info->clock_counter, 73);
	CU_ASSERT_EQUAL(info->hit_counter, 1);
	CU_ASSERT_EQUAL(info->write_back_counter, 1);
	
	memset(cells, 0, sizeof(cells));
	mem->ops->read(mem, 1, 2, cells);
	CU_ASSERT_EQUAL(cells[0].value, 0xFF);
	CU_ASSERT_EQUAL(cells[0].flags, 1);
	CU_ASSERT_EQUAL(cells[1].value, 183);
	CU_ASSERT_EQUAL(cells[1].flags, 1);
	CU_ASSERT_EQUAL(info->clock_counter, 77);
	CU_ASSERT_EQUAL(info->hit_counter, 1);
	
	cache->ops->read(cache, 0, 2, cells);
	CU_ASSERT_EQUAL(cells[0].flags, 0);
	CU_ASSERT_EQUAL(cells[1].value, 0xFF);
	CU_ASSERT_EQUAL(cells[1].flags, 1);
	CU_ASSERT_EQUAL(info->clock_counter, 126);
	CU_ASSERT_EQUAL(info->hit_counter, 1);
	CU_ASSERT_EQUAL(info->write_back_counter, 2);
	
	
	
	CU_ASSERT_EQUAL(cache->ops->free(cache), NULL);
	mem->ops->free(mem);
	statistics_free(info);
	config_file_free(cfg);
}

void full_cache_wt_random_typicall(void) 
{
	AbstractMemory *cache;
	AbstractMemory *mem;
	StatisticsInfo *info;
	ConfigFile *cfg;
	Random *rnd;
	MemoryCell cells[8]; 
	char conf_file[] = "test_config_file.txt";
	FILE *cfg_f = NULL;
	cfg_f = fopen(conf_file, "w");
	fprintf(cfg_f, "write_strategy = write-through\n block_size = 64\n\
	cache_size = %d\n cache_read_time = 1\n cache_write_time = 2\n\
	memory_size = %d\n memory_read_time = 4\n memory_write_time = 8\n\
	memory_width = 16\n associativity = full\n seed = 12481204\n\
	replacement_strategy = random\n", KiB, 16 * KiB);
	fclose(cfg_f);
	
	cfg = config_file_parse(conf_file, stderr);
	unlink(conf_file);
	
	info = statistics_create(cfg);
	info->hit_counter_needed = 1;
	
	mem = memory_create(cfg, "", info);
	
	rnd = random_create(cfg);
	
	cache = cache_create(cfg, "", info, mem, rnd);
	CU_ASSERT_NOT_EQUAL(cache, NULL);
	cache->ops->read(cache, 15, 1, cells);
	
	CU_ASSERT_EQUAL(cells[0].flags, 0);
	CU_ASSERT_EQUAL(info->clock_counter, 17);
	
	cells[0].flags = 1;
	cells[0].value = 0xFF;
	cells[1].flags = 1;
	cells[1].value = 183;
	cells[2].flags = 0;
	cache->ops->write(cache, 1, 2, cells);
	CU_ASSERT_EQUAL(info->clock_counter, 27);
	
	//memset(cells, 0, sizeof(cells));
	//cache->ops->read(cache, 1, 2, cells);
	//CU_ASSERT_EQUAL(cells[0].value, 0xFF);
	//CU_ASSERT_EQUAL(cells[0].flags, 1);
	//CU_ASSERT_EQUAL(cells[1].value, 183);
	//CU_ASSERT_EQUAL(cells[1].flags, 1);
	//CU_ASSERT_EQUAL(cells[2].flags, 0);
	//CU_ASSERT_EQUAL(info->clock_counter, 28);
	//CU_ASSERT_EQUAL(info->read_counter, 2);
	//CU_ASSERT_EQUAL(info->hit_counter, 1);
	
	//cache->ops->read(cache, 2, 1, cells);
	//CU_ASSERT_EQUAL(cells[0].value, 183);
	//CU_ASSERT_EQUAL(cells[0].flags, 1);
	//CU_ASSERT_EQUAL(info->clock_counter, 29);
	//CU_ASSERT_EQUAL(info->read_counter, 3);
	//CU_ASSERT_EQUAL(info->hit_counter, 2);
	
	//cache->ops->read(cache, 5, 1, cells);
	//CU_ASSERT_EQUAL(cells[0].flags, 0);
	//CU_ASSERT_EQUAL(info->clock_counter, 30);
	//CU_ASSERT_EQUAL(info->read_counter, 4);
	//CU_ASSERT_EQUAL(info->hit_counter, 3);
	
	//cache->ops->read(cache, 64, 1, cells);
	//CU_ASSERT_EQUAL(cells[0].flags, 0);
	//CU_ASSERT_EQUAL(info->clock_counter, 47);
	
	//cache->ops->read(cache, 64, 1, cells);
	//CU_ASSERT_EQUAL(cells[0].flags, 0);
	//CU_ASSERT_EQUAL(info->clock_counter, 48);
	
	
	//cache->ops->read(cache, 1024, 1, cells);
	//CU_ASSERT_EQUAL(cells[0].flags, 0);
	//CU_ASSERT_EQUAL(info->clock_counter, 65);
	
	//cells[0].flags = 1;
	//cells[0].value = 0xFF;
	//cache->ops->write(cache, 5, 2, cells);
	//CU_ASSERT_EQUAL(info->clock_counter, 91);
	
	
	CU_ASSERT_EQUAL(cache->ops->free(cache), NULL);
	mem->ops->free(mem);
	statistics_free(info);
	random_free(rnd);
	config_file_free(cfg);
}

void
make_cache_suite(void)
{
	CU_pSuite suite;
    suite = CU_add_suite("cache_suite", NULL, NULL);
    CU_ADD_TEST(suite, direct_cache_wt_typicall);
    CU_ADD_TEST(suite, direct_cache_wb_typicall);
    CU_ADD_TEST(suite, full_cache_wt_random_typicall);
}
