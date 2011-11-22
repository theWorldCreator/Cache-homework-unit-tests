#include "CUnit/Basic.h"
#include "test_parse_cfg.h"
#include "test_statistics.h"
#include "test_trace.h"

#include <stdio.h>
#include <unistd.h>

int
main(void)
{
    CU_initialize_registry();
    make_cfg_suite();
    make_statistics_suite();
    make_trace_suite();
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
