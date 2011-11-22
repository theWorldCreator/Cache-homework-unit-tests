#include "CUnit/Basic.h"
#include "../parse_config.h"

#include <stdio.h>
#include <unistd.h>

void wrong_path(void) 
{
	CU_ASSERT_EQUAL(config_file_parse(NULL, stderr), NULL);
	CU_ASSERT_EQUAL(config_file_parse("unexist__", stderr), NULL);
}

void wrong_data(void) 
{
	char file_name[] = "test_file.txt";
	FILE *cfg_f = NULL;
	cfg_f = fopen(file_name, "w");
	fprintf(cfg_f, "slcknl");
	fclose(cfg_f);
	CU_ASSERT_EQUAL(config_file_parse(file_name, stderr), NULL);
	
	cfg_f = fopen(file_name, "w");
	fprintf(cfg_f, "slcknl\n");
	fclose(cfg_f);
	CU_ASSERT_EQUAL(config_file_parse(file_name, stderr), NULL);
	
	cfg_f = fopen(file_name, "w");
	fprintf(cfg_f, "slck#nl\n");
	fclose(cfg_f);
	CU_ASSERT_EQUAL(config_file_parse(file_name, stderr), NULL);
	
	cfg_f = fopen(file_name, "w");
	fprintf(cfg_f, "\n\nslck#nl\n");
	fclose(cfg_f);
	CU_ASSERT_EQUAL(config_file_parse(file_name, stderr), NULL);
	
	cfg_f = fopen(file_name, "w");
	fprintf(cfg_f, "a = b\n   c=d\n\n\n = val\n");
	fclose(cfg_f);
	CU_ASSERT_EQUAL(config_file_parse(file_name, stderr), NULL);
	
	unlink(file_name);
}



void test_parse_cfg(void) 
{
	ConfigFile *cfg;
	int val;
	char file_name[] = "test_file.txt";
	FILE *cfg_f = NULL;
	cfg_f = fopen(file_name, "w");
	fprintf(cfg_f, "val1 = b\n\n	  val2=g\r\n#\n\r\n###\ncompl=    a + b - c    	hi	# complex line\nval4=    2");
	fclose(cfg_f);
	cfg = config_file_parse(file_name, stderr);
	unlink(file_name);
	CU_ASSERT_NOT_EQUAL(cfg, NULL);
	CU_ASSERT_EQUAL(config_file_get(cfg, "unexist"), NULL);
	CU_ASSERT_EQUAL(config_file_get(cfg, "unexist2"), NULL);
	CU_ASSERT_EQUAL(config_file_get_int(cfg, "unexist3", &val), 0);
	CU_ASSERT_EQUAL(config_file_get_int(cfg, "val1", &val), -1);
	CU_ASSERT_STRING_EQUAL(config_file_get(cfg, "val1"), "b");
	CU_ASSERT_STRING_EQUAL(config_file_get(cfg, "val2"), "g");
	CU_ASSERT_STRING_EQUAL(config_file_get(cfg, "compl"), "a + b - c    	hi");
	CU_ASSERT_STRING_EQUAL(config_file_get(cfg, "compl"), "a + b - c    	hi");
	CU_ASSERT_STRING_EQUAL(config_file_get(cfg, "compl"), "a + b - c    	hi");
	CU_ASSERT_STRING_EQUAL(config_file_get(cfg, "val4"), "2");
	CU_ASSERT_EQUAL(config_file_get_int(cfg, "val4", &val), 1);
	CU_ASSERT_EQUAL(val, 2);
	CU_ASSERT_EQUAL(config_file_free(cfg), NULL);
}

int
main(void)
{
	CU_pSuite suite;
    CU_initialize_registry();
    suite = CU_add_suite("cfg_suite", NULL, NULL);
    CU_ADD_TEST(suite, wrong_path);
    CU_ADD_TEST(suite, wrong_data);
    CU_ADD_TEST(suite, test_parse_cfg);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
