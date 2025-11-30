#!/usr/bin/env ksh

function tsrun
{
	typeset ret

	sh -c "$@" >$out 2>$err
	ret=$?
	sed 's/^/| /' $out
	sed 's/^/@ /' $err
	(( ret != 0 )) && print "? $ret"
}

function tseval
{
	typeset ln

	while read ln
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

	while read ln
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

err=$(mktemp /tmp/XXXXXXXXXX)
out=$(mktemp /tmp/XXXXXXXXXX)
trap "rm $err $out" HUP INT QUIT TERM

if [[ $1 = -b ]]
then
	(( $# == 1 )) && exit 64
	shift
	for fn
	do
		cp $fn $fn~
		(rm $fn && tseval >$fn) <$fn
	done
else
	exp=$(mktemp /tmp/XXXXXXXXXX)
	trap "rm $exp" HUP INT QUIT TERM
	for fn
	do
		tsexpand <$fn >$exp
		tseval <$fn | diff -u $exp -
	done
	rm $exp
fi
rm $err $out
