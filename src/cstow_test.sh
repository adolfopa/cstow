#! /usr/bin/env atf-sh

atf_test_case with_no_args_fails
with_no_args_fails_head() {
    atf_set "descr" "With no args, CStow should fail."
    atf_set "require.progs" "cstow"
}
with_no_args_fails_body() {
    atf_check -s not-exit:0 -o empty -e not-empty cstow
}

atf_test_case D_flag_with_no_arg_fails
D_flag_with_no_arg_fails_head() {
    atf_set "descr" "The D flag requires an argument."
    atf_set "require.progs" "cstow"
}
D_flag_with_no_arg_fails_body() {
    atf_check -s not-exit:0 -o empty -e not-empty cstow
}

atf_test_case h_flag_succeeds
h_flag_succeeds_head() {
    atf_set "descr" "The h flag always succeeds."
    atf_set "require.progs" "cstow"
}
h_flag_succeeds_body() {
    atf_check -s exit:0 -o not-empty -e empty cstow -h
}

atf_test_case without_flags_stows_package
without_flags_stows_package_head() {
    atf_set "descr" "Without flags, stows the given package."
    atf_set "require.progs" "cstow" "test"
}
without_flags_stows_package_body() {
    create_test_package

    cd cstow
    atf_check -s exit:0 cstow pkg

    cd ..
    check_directories exist a a/b a/b/c x x/y
    check_links exist a/foo.bin x/y/bar.bin a/b/c/baz.bin
}

atf_test_case D_flag_unstows_package
D_flag_unstows_package_head() {
    atf_set "descr" "With -D flag, it unstows the given package."
    atf_set "require.progs" "cstow" "test"
}
D_flag_unstows_package_body() {
    create_test_package

    cd cstow
    atf_check -s exit:0 cstow pkg

    atf_check -s exit:0 cstow -D pkg

    cd ..
    check_directories dont_exist a a/b a/b/c x x/y
    check_links dont_exist a/foo.bin x/y/bar.bin a/b/c/baz.bin
}

atf_test_case v_flag_outputs_each_action
v_flag_outputs_each_action_head() {
    atf_set "descr" "With the -v flag, each action performed is output to stdout"
    atf_set "require.progs" "cstow"
}
v_flag_outputs_each_action_body() {
    create_test_package

    cd cstow
    atf_check -s exit:0 -o save:stdout.log -e empty cstow -v pkg

    ## 8 actions were executed in total.
    atf_check -s exit:0 test $(wc -l stdout.log | awk '{print $1}') = 8

    ## 5 directories were created.
    atf_check -s exit:0 -o save:grepmk.log -e empty grep mkdir stdout.log
    atf_check -s exit:0 test $(wc -l grepmk.log | awk '{print $1}') = 5

    ## 3 files were linked.
    atf_check -s exit:0 -o save:grepln.log -e empty grep ln stdout.log
    atf_check -s exit:0 test $(wc -l grepln.log | awk '{print $1}') = 3
}

atf_test_case n_flag_only_pretends
n_flag_only_pretends_head() {
    atf_set "descr" "With the -n flag, no actions are executed."
    atf_set "require.progs" "cstow"
}
n_flag_only_pretends_body() {
    create_test_package

    cd cstow
    atf_check -s exit:0 cstow -n pkg

    cd ..
    check_directories dont_exist a a/b a/b/c x x/y
    check_links dont_exist a/foo.bin x/y/bar.bin a/b/c/baz.bin
}

atf_init_test_cases() {
    atf_add_test_case with_no_args_fails
    atf_add_test_case D_flag_with_no_arg_fails
    atf_add_test_case h_flag_succeeds
    atf_add_test_case without_flags_stows_package
    atf_add_test_case D_flag_unstows_package
    atf_add_test_case v_flag_outputs_each_action
    atf_add_test_case n_flag_only_pretends
}

check_file_type() {
    flag=$1
    shift
    if [ $1 = 'exist' ]; then
    	status_check=exit
    else
    	status_check=not-exit
    fi
    shift
    for file in $*; do
    	atf_check -s ${status_check}:0 test $flag $file
    done
}

check_links() {
    check_file_type -L $*
}

check_directories() {
    check_file_type -d $*
}

create_test_package() {
    mkdir -p cstow/pkg/a/b/c
    mkdir -p cstow/pkg/x/y
    touch cstow/pkg/a/foo.bin
    touch cstow/pkg/x/y/bar.bin
    touch cstow/pkg/a/b/c/baz.bin
}
