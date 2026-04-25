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
	typeset evout=$1
	typeset exout=$2
	typeset ln

	while IFS= read -r ln
	do
		case ${ln%% *} in
		$)
			print "$ln" >>"$evout"
			[[ -n $exout ]] && print "$ln" >>"$exout"
			tsrun "${ln#$}" >>"$evout"
			;;
		\?|@|\|)
			[[ -n $exout ]] && eval print \""$ln"\" >>"$exout"
			;;
		*)
			print "$ln" >>"$evout"
			[[ -n $exout ]] && print "$ln" >>"$exout"
			;;
		esac
	done
}

(( $# == 0 )) && exit 64

err=$(mktemp /tmp/XXXXXXXXXX) || exit 1
evl=$(mktemp /tmp/XXXXXXXXXX) || exit 1
exp=$(mktemp /tmp/XXXXXXXXXX) || exit 1
out=$(mktemp /tmp/XXXXXXXXXX) || exit 1
trap 'rc=$?; rm -f "$err" "$evl" "$exp" "$out"; exit $rc' EXIT HUP INT QUIT TERM

typeset -i rc=0

if [[ $1 = -b ]]
then
	(( $# == 1 )) && exit 64
	shift
	for fn
	do
		cp "$fn" "$fn~"
		tseval "$evl" "" <"$fn"
		cp "$evl" "$fn"
	done
else
	for fn
	do
		tseval "$evl" "$exp" <"$fn"
		diff -u "$exp" "$evl" || rc=1
	done
fi
exit $rc
