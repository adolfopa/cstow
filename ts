#!/usr/bin/env ksh

function tsrun
{
	typeset ret

	sh -c "$@" >"$out" 2>"$err"
	ret=$?
	while IFS= read -r ln; do print "| ${ln}"; done <"$out"
	while IFS= read -r ln; do print "@ ${ln}"; done <"$err"
	(( ret != 0 )) && print "? $ret"
}

function tseval
{
	typeset ln

	while IFS= read -r ln
	do
		case ${ln%% *} in
		$)
			print "$ln"
			tsrun "${ln#$}"
			;;
		!(\?|@|\|))
			print "$ln"
			;;
		esac
	done
}

function tsexpand
{
	typeset ln

	while IFS= read -r ln
	do
		case ${ln%% *}
		in
			\?|@|\|)
				eval print \""$ln"\"
				;;
			*)
				print "$ln"
				;;
		esac
	done
}

(( $# == 0 )) && exit 64

err=$(mktemp /tmp/XXXXXXXXXX) || exit 1
exp=$(mktemp /tmp/XXXXXXXXXX) || exit 1
out=$(mktemp /tmp/XXXXXXXXXX) || exit 1
trap 'rc=$?; rm -f "$err" "$exp" "$out"; exit $rc' EXIT HUP INT QUIT TERM

typeset -i rc=0
if [[ $1 = -b ]]
then
	(( $# == 1 )) && exit 64
	shift
	for fn
	do
		cp "$fn" "$fn~"
		(rm "$fn" && tseval >"$fn") <"$fn"
	done
else
	for fn
	do
		tsexpand <"$fn" >"$exp"
		tseval <"$fn" | diff -u "$exp" - || rc=1
	done
fi
exit $rc
