#include "CUnit/Basic.h"
#include "../statistics.h"

#include <stdio.h>
#include <unistd.h>

void tets(void) 
{
	StatisticsInfo *info = statistics_create(NULL);
	CU_ASSERT_NOT_EQUAL(info, NULL);
	CU_ASSERT_EQUAL(info->clock_counter, 0);
	CU_ASSERT_EQUAL(info->write_counter, 0);
	statistics_add_counter(info, 5);
	CU_ASSERT_EQUAL(info->clock_counter, 5);
	statistics_add_counter(info, 6);
	CU_ASSERT_EQUAL(info->clock_counter, 11);
	statistics_add_write(info);
	CU_ASSERT_EQUAL(info->write_counter, 1);
	CU_ASSERT_EQUAL(statistics_free(info), NULL);
}

void
make_statistics_suite(void)
{
	CU_pSuite suite;
    suite = CU_add_suite("statistics_suite", NULL, NULL);
    CU_ADD_TEST(suite, tets);
}
