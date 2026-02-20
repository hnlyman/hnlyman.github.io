#!/usr/bin/env bash

search_dir=.
prefix=_pr.
tmp_dir="./${prefix}TEMPDIR"
pass_file="${prefix}PASSWORD"
dest=private.tar.gz

zip() {
    find $search_dir -type f -name "$prefix*" -exec $0 copy_to_archive {}  \;
    tar -czf $dest $tmp_dir

    gpg --passphrase $(cat $pass_file) --batch --yes --symmetric $dest

    rm -r $tmp_dir
    rm $dest
}

unzip() {
    gpg --passphrase $(cat $pass_file) --batch --yes -o $dest -d $dest.gpg

    tar -xzf $dest

    find $tmp_dir -type f -exec $0 copy_from_archive {} \;

    rm -r $tmp_dir
    rm $dest
}

copy_to_archive() {
    dn=$(dirname $1)
    mkdir -p $tmp_dir/$dn
    cp $1 $tmp_dir/$1
}

copy_from_archive() {
    x=$1
    cp $1 .${x#$tmp_dir}
}

case "$1" in
    zip)
        zip
        ;;
    unzip)
        unzip
        ;;
    copy_to_archive)
        copy_to_archive $2
        ;;
    copy_from_archive)
        copy_from_archive $2
        ;;
    *)
        exit 1
        ;;
esac
