#! /usr/bin/env atf-sh

atf_test_case D_flag_with_no_arg_fails
D_flag_with_no_arg_fails_head() {
    atf_set "descr" "The -D flag requires an argument."
    atf_set "require.progs" "cstow"
}
D_flag_with_no_arg_fails_body() {
    atf_check -s not-exit:0 -o empty -e not-empty cstow
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

atf_test_case R_flag_restows_package
R_flag_restows_package_head() {
    atf_set "descr" "With the -R flag, the package is unstowed and restowed."
    atf_set "require.progs" "cstow"
}
R_flag_restows_package_body() {
    create_test_package

    cd cstow
    atf_check -s exit:0 cstow pkg

    touch pkg/quux.bin
    atf_check -s exit:0 cstow -R pkg

    cd ..
    check_directories exist a a/b a/b/c x x/y
    check_links exist a/foo.bin x/y/bar.bin a/b/c/baz.bin quux.bin
}

atf_test_case d_flag_changes_stow_dir
d_flag_changes_stow_dir_head() {
    atf_set "descr" "With the -d flag, the package is read from the given directory."
    atf_set "require.progs" "cstow"
}
d_flag_changes_stow_dir_body() {
    create_test_package

    atf_check -s exit:0 cstow -d cstow pkg

    check_directories exist a a/b a/b/c x x/y
    check_links exist a/foo.bin x/y/bar.bin a/b/c/baz.bin
}

atf_test_case last_d_supplied_takes_effect
last_d_supplied_takes_effect_head() {
    atf_set "descr" "When more than one -d flag is given, the last one takes effect."
    atf_set "require.progs" "cstow"
}
last_d_supplied_takes_effect_body() {
    create_test_package

    atf_check -s exit:0 cstow -d i_dont_exits_dir -d cstow pkg

    check_directories exist a a/b a/b/c x x/y
    check_links exist a/foo.bin x/y/bar.bin a/b/c/baz.bin
}

atf_test_case t_flag_changes_target_dir
t_flag_changes_target_dir_head() {
    atf_set "descr" "With the -t flag, the package is installed into the given directory."
    atf_set "require.progs" "cstow"
}
t_flag_changes_target_dir_body() {
    create_test_package
    
    mkdir other_dir && cd other_dir
    cd ..

    cd cstow
    atf_check -s exit:0 cstow -t ../other_dir pkg

    cd ../other_dir
    check_directories exist a a/b a/b/c x x/y
    check_links exist a/foo.bin x/y/bar.bin a/b/c/baz.bin
}

atf_test_case last_t_supplied_takes_effect
last_t_supplied_takes_effect_head() {
    atf_set "descr" "When more than one -t is given, the last one takes effect."
    atf_set "require.progs" "cstow"
}
last_t_supplied_takes_effect_body() {
    create_test_package

    mkdir not_used_dir
    mkdir other_dir

    cd cstow
    atf_check -s exit:0 cstow -t ../not_used_dir -t ../other_dir pkg

    cd ../other_dir
    check_directories exist a a/b a/b/c x x/y
    check_links exist a/foo.bin x/y/bar.bin a/b/c/baz.bin
}

atf_test_case t_and_d_flags_work_together
t_and_d_flags_work_together_head() {
    atf_set "descr" "-t and -d flags work well together"
    atf_set "require.progs" "cstow"
}
t_and_d_flags_work_together_body() {
    create_test_package

    mkdir target_dir

    atf_check -s exit:0 cstow -d cstow -t target_dir pkg

    cd target_dir
    check_directories exist a a/b a/b/c x x/y
    check_links exist a/foo.bin x/y/bar.bin a/b/c/baz.bin
}

atf_init_test_cases() {
    atf_add_test_case with_no_args_fails
    atf_add_test_case D_flag_with_no_arg_fails
    atf_add_test_case h_flag_succeeds
    atf_add_test_case without_flags_stows_package
    atf_add_test_case D_flag_unstows_package
    atf_add_test_case v_flag_outputs_each_action
    atf_add_test_case n_flag_only_pretends
    atf_add_test_case R_flag_restows_package
    atf_add_test_case d_flag_changes_stow_dir
    atf_add_test_case last_d_supplied_takes_effect
    atf_add_test_case t_flag_changes_target_dir
    atf_add_test_case last_t_supplied_takes_effect
    atf_add_test_case t_and_d_flags_work_together
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
