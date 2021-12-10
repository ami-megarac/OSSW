-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA1

Format: 3.0 (quilt)
Source: readline6
Binary: libreadline6, lib64readline6, readline-common, libreadline-dev, libreadline6-dev, lib64readline6-dev, libreadline6-dbg, rlfe, lib32readline6, lib32readline6-dev, readline-doc
Architecture: any all
Version: 6.3-8
Maintainer: Matthias Klose <doko@debian.org>
Standards-Version: 3.9.5
Build-Depends: debhelper (>= 8.1.3), libtinfo-dev, lib32tinfo-dev [amd64 ppc64], libncursesw5-dev (>= 5.6), lib32ncursesw5-dev [amd64 ppc64], lib64ncurses5-dev [i386 powerpc sparc s390], mawk | awk, texinfo, autotools-dev, gcc-multilib [amd64 i386 kfreebsd-amd64 powerpc ppc64 s390 sparc]
Package-List:
 lib32readline6 deb libs optional arch=amd64,ppc64
 lib32readline6-dev deb libdevel optional arch=amd64,ppc64
 lib64readline6 deb libs optional arch=i386,powerpc,s390,sparc
 lib64readline6-dev deb libdevel optional arch=i386,powerpc,s390,sparc
 libreadline-dev deb libdevel optional arch=any
 libreadline6 deb libs important arch=any
 libreadline6-dbg deb debug extra arch=any
 libreadline6-dev deb libdevel optional arch=any
 readline-common deb utils important arch=all
 readline-doc deb doc optional arch=all
 rlfe deb utils optional arch=any
Checksums-Sha1:
 017b92dc7fd4e636a2b5c9265a77ccc05798c9e1 2468560 readline6_6.3.orig.tar.gz
 d787876bb2f0cad2ec1081c9acc5610ee41d9ead 30576 readline6_6.3-8.debian.tar.xz
Checksums-Sha256:
 56ba6071b9462f980c5a72ab0023893b65ba6debb4eeb475d7a563dc65cafd43 2468560 readline6_6.3.orig.tar.gz
 c2b55fdd221136f46fa1e0cbf0bf2e37b70ddf97929312fbab6032e9129d58b5 30576 readline6_6.3-8.debian.tar.xz
Files:
 33c8fb279e981274f485fd91da77e94a 2468560 readline6_6.3.orig.tar.gz
 76d4feca5974a35e6c82bcadeba851af 30576 readline6_6.3-8.debian.tar.xz

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1

iEYEARECAAYFAlPec3AACgkQStlRaw+TLJz0HACfeG69C450RqvUkb30EHgVBLEl
1yoAn3ENnqk1EttuE5zg2yk8kh4fQ7H1
=VUA8
-----END PGP SIGNATURE-----
