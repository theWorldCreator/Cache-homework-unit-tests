#include "CUnit/Basic.h"
#include "../trace.h"

#include <stdio.h>
#include <unistd.h>


void trace_wrong_data(void) 
{
	Trace *t;
	char file_name[] = "test_trace_file.txt";
	FILE *cfg_f = NULL;
	cfg_f = fopen(file_name, "w");
	fprintf(cfg_f, "Alas lasd lajmda.\nskdj #aaaaaaaaaaaaaaaaa\
	s csdc \r\ndk a\n\rs fasaod ;");
	fclose(cfg_f);
	t = trace_open(file_name, stderr);
	
	CU_ASSERT_NOT_EQUAL(t, NULL);
	CU_ASSERT_EQUAL(trace_next(t), -1);
	CU_ASSERT_EQUAL(trace_next(t), -1);
	CU_ASSERT_EQUAL(trace_next(t), -1);
	CU_ASSERT_EQUAL(trace_next(t), -1);
	CU_ASSERT_EQUAL(trace_next(t), 0);
	CU_ASSERT_EQUAL(trace_close(t), NULL);
	
	
	cfg_f = fopen(file_name, "w");
	fprintf(cfg_f, "TD 0 \n");
	fclose(cfg_f);
	t = trace_open(file_name, stderr);
	
	CU_ASSERT_NOT_EQUAL(t, NULL);
	CU_ASSERT_EQUAL(trace_next(t), -1);
	CU_ASSERT_EQUAL(trace_close(t), NULL);
	
	cfg_f = fopen(file_name, "w");
	fprintf(cfg_f, "WD 0 1\n");
	fclose(cfg_f);
	t = trace_open(file_name, stderr);
	
	CU_ASSERT_NOT_EQUAL(t, NULL);
	CU_ASSERT_EQUAL(trace_next(t), -1);
	CU_ASSERT_EQUAL(trace_close(t), NULL);
	
	cfg_f = fopen(file_name, "w");
	fprintf(cfg_f, "WD 0 3");
	fclose(cfg_f);
	t = trace_open(file_name, stderr);
	
	CU_ASSERT_NOT_EQUAL(t, NULL);
	CU_ASSERT_EQUAL(trace_next(t), -1);
	CU_ASSERT_EQUAL(trace_close(t), NULL);
	
	unlink(file_name);
}



void trace_typical(void) 
{
	Trace *t;
	TraceStep *step;
	char file_name[] = "test_trace_file.txt";
	FILE *cfg_f = NULL;
	cfg_f = fopen(file_name, "w");
	fprintf(cfg_f, "WD F #first line\n\nBAD LINE\r\nRI     	5 4 -2\n    #comment line\r\n\nRD 2");
	fclose(cfg_f);
	t = trace_open(file_name, stderr);
	
	CU_ASSERT_NOT_EQUAL(t, NULL);
	
	CU_ASSERT_EQUAL(trace_next(t), 1);
	step = trace_get(t);
	printf("'%d'\n", step->addr);
	CU_ASSERT_EQUAL(step->op, 'W');
	CU_ASSERT_EQUAL(step->mem, 'D');
	CU_ASSERT_EQUAL(step->addr, 0xF);
	CU_ASSERT_EQUAL(step->size, 1);
	CU_ASSERT_EQUAL(step->value[0].value, 0);
	CU_ASSERT_EQUAL(step->value[0].flags, 1);
	
	CU_ASSERT_EQUAL(trace_next(t), -1);
	
	CU_ASSERT_EQUAL(trace_next(t), 1);
	step = trace_get(t);
	CU_ASSERT_EQUAL(step->op, 'R');
	CU_ASSERT_EQUAL(step->mem, 'I');
	CU_ASSERT_EQUAL(step->addr, 5);
	CU_ASSERT_EQUAL(step->size, 4);
	CU_ASSERT_EQUAL(step->value[0].value, 0xFF);
	CU_ASSERT_EQUAL(step->value[0].flags, 1);
	CU_ASSERT_EQUAL(step->value[1].value, 0xFF);
	CU_ASSERT_EQUAL(step->value[1].flags, 1);
	CU_ASSERT_EQUAL(step->value[2].value, 0xFF);
	CU_ASSERT_EQUAL(step->value[2].flags, 1);
	CU_ASSERT_EQUAL(step->value[3].value, 0xFF - 1);
	CU_ASSERT_EQUAL(step->value[3].flags, 1);
	
	CU_ASSERT_EQUAL(trace_next(t), 1);
	step = trace_get(t);
	CU_ASSERT_EQUAL(step->op, 'R');
	CU_ASSERT_EQUAL(step->mem, 'D');
	CU_ASSERT_EQUAL(step->addr, 2);
	CU_ASSERT_EQUAL(step->size, 1);
	CU_ASSERT_EQUAL(step->value[0].value, 0);
	CU_ASSERT_EQUAL(step->value[0].flags, 1);
	
	CU_ASSERT_EQUAL(trace_next(t), 0);
	
	CU_ASSERT_EQUAL(trace_close(t), NULL);
	
	
	unlink(file_name);
}

void
make_trace_suite(void)
{
	CU_pSuite suite;
    suite = CU_add_suite("trace_suite", NULL, NULL);
    CU_ADD_TEST(suite, trace_wrong_data);
    CU_ADD_TEST(suite, trace_typical);
}
