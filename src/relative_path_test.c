#include <atf-c.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "path.h"

#define BUFFER_SIZE 127
#define ARRAY_LEN(x) (sizeof(x)*sizeof(x[0]))

ATF_TC(relative_path_ignores_duplicated_slashes);
ATF_TC_HEAD(relative_path_ignores_duplicated_slashes, tc)
{
     atf_tc_set_md_var(tc, "descr",
		       "When calculating the relative path between two items, "
		       "ignores any extra slashes.");
}
ATF_TC_BODY(relative_path_ignores_duplicated_slashes, tc)
{
     char *from = strdup("//usr/bin///exec");
     char *to = strdup("/usr///bin/////exec");
     char buffer[BUFFER_SIZE];
     int retval;

     retval = make_relative_path(from, to, buffer, ARRAY_LEN(buffer));

     ATF_CHECK_EQ(retval, 0);
     ATF_CHECK_STREQ(buffer, "");
}

ATF_TC(relative_path_to_self_is_empty);
ATF_TC_HEAD(relative_path_to_self_is_empty, tc)
{
     atf_tc_set_md_var(tc, "descr",
		       "The relative path of an item to itself is the "
		       "empty path.");
}
ATF_TC_BODY(relative_path_to_self_is_empty, tc)
{
     char *from = strdup("/usr/bin/exec");
     char *to = strdup("/usr/bin/exec");
     char buffer[BUFFER_SIZE];
     int retval;

     retval = make_relative_path(from, to, buffer, ARRAY_LEN(buffer));

     ATF_CHECK_EQ(retval, 0);
     ATF_CHECK_STREQ(buffer, "");
}

ATF_TC(relative_path_between_unrelated_items);
ATF_TC_HEAD(relative_path_between_unrelated_items, tc)
{
     atf_tc_set_md_var(tc, "descr",
		       "The relative path between two unrelated items is "
		       "the second one with the parent dir (..) preprended.");
}
ATF_TC_BODY(relative_path_between_unrelated_items, tc)
{
     char *from = strdup("foo");
     char *to = strdup("bar");
     char buffer[BUFFER_SIZE];
     int retval;

     retval = make_relative_path(from, to, buffer, ARRAY_LEN(buffer));

     ATF_CHECK_EQ(retval, 0);
     ATF_CHECK_STREQ(buffer, "../bar");
}

ATF_TC(relative_path_between_items_with_common_parent);
ATF_TC_HEAD(relative_path_between_items_with_common_parent, tc)
{
}
ATF_TC_BODY(relative_path_between_items_with_common_parent, tc)
{
     char *from = strdup("/usr/local/cstow/foo/bin/exec");
     char *to = strdup("/usr/local/bin/exec");
     char buffer[BUFFER_SIZE];
     int retval;

     retval = make_relative_path(from, to, buffer, ARRAY_LEN(buffer));

     ATF_CHECK_EQ(retval, 0);
     ATF_CHECK_STREQ(buffer, "../../../bin/exec");
}

ATF_TC(relative_path_fails_when_buffer_not_big_enough);
ATF_TC_HEAD(relative_path_fails_when_buffer_not_big_enough, tc)
{
}
ATF_TC_BODY(relative_path_fails_when_buffer_not_big_enough, tc)
{
     char *from = strdup("/usr/local/cstow/foo/bin/exec");
     char *to = strdup("/usr/local/bin/exec");
     char buffer[] = { 'a', 'b', 'c', 'd', '\0' };
     int retval;

     retval = make_relative_path(from, to, buffer, 0);

     ATF_CHECK_EQ(retval, 1);
     ATF_CHECK_STREQ(buffer, "abcd");
}

ATF_TC(relative_path_respects_buffer_limits);
ATF_TC_HEAD(relative_path_respects_buffer_limits, tc)
{
}
ATF_TC_BODY(relative_path_respects_buffer_limits, tc)
{
     char *from = strdup("/usr/local/cstow/foo/bin/exec");
     char *to = strdup("/usr/local/bin/exec");
     char buffer[] = { 'a', 'b', 'c', 'd', '\0' };
     int retval;

     retval = make_relative_path(from, to, buffer, 1);

     ATF_CHECK_EQ(retval, 1);
     ATF_CHECK_STREQ(buffer, "");
     ATF_CHECK_STREQ(buffer + 1, "bcd");
}

ATF_TP_ADD_TCS(tp)
{
     ATF_TP_ADD_TC(tp, relative_path_to_self_is_empty);
     ATF_TP_ADD_TC(tp, relative_path_between_unrelated_items);
     ATF_TP_ADD_TC(tp, relative_path_between_items_with_common_parent);
     ATF_TP_ADD_TC(tp, relative_path_ignores_duplicated_slashes);
     ATF_TP_ADD_TC(tp, relative_path_respects_buffer_limits);
     ATF_TP_ADD_TC(tp, relative_path_fails_when_buffer_not_big_enough);

     return atf_no_error();
}
