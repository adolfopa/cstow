BEGIN {
    FS = "·"
    "uname" | getline os
}

{ sub(/ *#.*$/, "") }
$1 == h && $2 == os  { print $3; exit }
$1 == h && $2 == "*" { fallback = $3 }

END {
    print(fallback)
}
