gpg --passphrase $(cat _pr.PASSWORD) --batch --yes -o private.tar.gz -d private.tar.gz.gpg

tar -xzf private.tar.gz

find ./_pr.ZIPFOLDER -type f -exec ./copy_from_zipfolder.sh {} \;

rm -r _pr.ZIPFOLDER
rm private.tar.gz
