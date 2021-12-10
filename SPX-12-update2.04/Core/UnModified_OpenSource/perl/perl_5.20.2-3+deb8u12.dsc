-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: perl
Binary: perl-base, perl-doc, perl-debug, libperl5.20, libperl-dev, perl-modules, perl
Architecture: any all
Version: 5.20.2-3+deb8u12
Maintainer: Niko Tyni <ntyni@debian.org>
Uploaders: Dominic Hargreaves <dom@earth.li>
Homepage: http://dev.perl.org/perl5/
Standards-Version: 3.9.6
Vcs-Browser: http://anonscm.debian.org/gitweb/?p=perl/perl.git
Vcs-Git: git://anonscm.debian.org/perl/perl.git -b debian-5.20
Build-Depends: file, cpio, libdb-dev, libgdbm-dev, netbase [!hurd-any], procps [!hurd-any], zlib1g-dev | libz-dev, libbz2-dev, dpkg-dev (>= 1.16.0), libc6-dev (>= 2.19-9) [s390x]
Package-List:
 libperl-dev deb libdevel optional arch=any
 libperl5.20 deb libs optional arch=any
 perl deb perl standard arch=any
 perl-base deb perl required arch=any essential=yes
 perl-debug deb debug extra arch=any
 perl-doc deb doc optional arch=all
 perl-modules deb perl standard arch=all
Checksums-Sha1:
 63126c683b4c79c35008a47d56f7beae876c569f 13717128 perl_5.20.2.orig.tar.bz2
 17289f40ec9aa0dd86ec4f5c619ad8252a652236 158872 perl_5.20.2-3+deb8u12.debian.tar.xz
Checksums-Sha256:
 e5a4713bc65e1da98ebd833dce425c000768bfe84d17ec5183ec5ca249db71ab 13717128 perl_5.20.2.orig.tar.bz2
 b93b828b4ebd8171ca7ef5f8f195d529c368e83cd86f276d4a25470a6b7aaa6d 158872 perl_5.20.2-3+deb8u12.debian.tar.xz
Files:
 21062666f1c627aeb6dbff3c6952738b 13717128 perl_5.20.2.orig.tar.bz2
 2e063c43030e24ec686d3e673f0dfd46 158872 perl_5.20.2-3+deb8u12.debian.tar.xz

-----BEGIN PGP SIGNATURE-----

iQJBBAEBCAArFiEEy0llJ/kAnyscGnbawAV+cU1pT7IFAlwBPD0NHGRvbUBlYXJ0
aC5saQAKCRDABX5xTWlPsgpmEADnGRYJ8HVZYqzK8NZ2im2hD2ZDAnx+wRcy7pK1
Zx0fakEwdU7exJ9uwgkO1LSvX3AsTDeQWAJRzZ2v0+hTo3bcq/bTtCMCT7hMr1D5
We15BTZcO2IegvZ6Sxk6JeN27VLqh0jisFl0lprqcCbNRztKPSq4c7zZ94bCJYLU
XsOgt5yaYbmHvCKj0Dg9g/TXkJ0//9BqjblcKn8jRHuTm+n1iWCGp4LxNgFgjGkb
Qgxxny/PPCdj7uqm3l9tBZML38x57eBWkiLSGrAIREiH9/+zJdFsH/O5aoXG1zLS
xjB0a3sU/KNYW10PPPbLjYkR/U6IYku6eFe73zcIuEGNG8XUQBBQH95Olf7pv+c4
F+Kisly53f1JnC8aYW+L+r2OlkNn56zCqPSBHh0iIqCOdVDZcFubsXgJV71n4twJ
6qc0wUthDSzjmgdba9vuiWzPQkSOSHXUJBTansr7zE7XnIm7lJeihdbml+c9vSFA
A4hva06b8lDz/cWp6OSRtYcFHIEmCwn0XXve5Xi5zPgUMbCRJ/eY1stMiEloiDvL
pEIcTZvmLf4RwDUYoDYHNZDcWQCTDEOtdJkkF9vYur8UStIHz6CurIgxJNKb8UF4
bp0w3poReTd8aTI45ojsTUxLm+uw8v+WGMH3Lb4iqJsJnlnCBvIAX9TxUlueqGoj
fsUziA==
=tE7Q
-----END PGP SIGNATURE-----
