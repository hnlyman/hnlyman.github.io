find **/_pr.* -exec ./copy_to_zipfolder.sh {}  \;
tar -czf private.tar.gz _pr.ZIPFOLDER

gpg --passphrase $(cat _pr.PASSWORD) --batch --yes --symmetric private.tar.gz

rm -r _pr.ZIPFOLDER
rm private.tar.gz
