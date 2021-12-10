-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: gnutls28
Binary: libgnutls28-dev, libgnutls-deb0-28, libgnutls28-dbg, gnutls-bin, gnutls-doc, guile-gnutls, libgnutlsxx28, libgnutls-openssl27
Architecture: any all
Version: 3.3.30-0+deb8u1
Maintainer: Debian GnuTLS Maintainers <pkg-gnutls-maint@lists.alioth.debian.org>
Uploaders: Andreas Metzler <ametzler@debian.org>, Eric Dorland <eric@debian.org>, James Westby <jw+debian@jameswestby.net>, Simon Josefsson <simon@josefsson.org>
Homepage: http://www.gnutls.org/
Standards-Version: 3.9.5
Vcs-Browser: http://anonscm.debian.org/gitweb/?p=pkg-gnutls/gnutls.git
Vcs-Git: git://anonscm.debian.org/pkg-gnutls/gnutls.git
Build-Depends: debhelper (>= 9), nettle-dev (>= 2.7), zlib1g-dev, libtasn1-6-dev (>= 3.9), autotools-dev, guile-2.0-dev [!ia64 !m68k], datefudge, libp11-kit-dev (>= 0.20.7), pkg-config, chrpath, libidn11-dev, autogen (>= 1:5.16-0), bison, dh-autoreconf, libgmp-dev (>= 2:6), libopts25-dev, net-tools
Build-Depends-Indep: gtk-doc-tools, texinfo (>= 4.8)
Build-Conflicts: libgnutls-dev, libp11-kit-dev (= 0.21.2-1)
Package-List:
 gnutls-bin deb net optional arch=any
 gnutls-doc deb doc optional arch=all
 guile-gnutls deb lisp optional arch=amd64,arm64,armel,armhf,i386,kfreebsd-amd64,kfreebsd-i386,mips,mipsel,powerpc,ppc64el,s390,s390x,sparc,hurd-i386
 libgnutls-deb0-28 deb libs standard arch=any
 libgnutls-openssl27 deb libs standard arch=any
 libgnutls28-dbg deb debug extra arch=any
 libgnutls28-dev deb libdevel optional arch=any
 libgnutlsxx28 deb libs extra arch=any
Checksums-Sha1:
 05d7e38e1b386be9683a23f873b7e049d49db332 6392748 gnutls28_3.3.30.orig.tar.xz
 5e38dfe5ea73d6339251599d1f7983c724976bca 46352 gnutls28_3.3.30-0+deb8u1.debian.tar.xz
Checksums-Sha256:
 41d70107ead3de2f12390909a05eefc9a88def6cd1f0d90ea82a7dac8b8effee 6392748 gnutls28_3.3.30.orig.tar.xz
 f3055451c76ba5c805f558b676bc5b83fbbc5cce9332d2fc0bece2c180165d6f 46352 gnutls28_3.3.30-0+deb8u1.debian.tar.xz
Files:
 748f4c194a51ca9f2c02d9b7735262c2 6392748 gnutls28_3.3.30.orig.tar.xz
 3be1fde48a35320850ba08de5fadb167 46352 gnutls28_3.3.30-0+deb8u1.debian.tar.xz

-----BEGIN PGP SIGNATURE-----

iQEzBAEBCAAdFiEEexZCBNCWcjsBljWrPqHd3bJh2XsFAlvYeOcACgkQPqHd3bJh
2XuqhAf+OJHTGoxWU3tEn3ib0K4RSQplcz8kOG0YL/33kzVyThLD0e1hl5dgc+OZ
OWnmpKfHR0Cfo+m2b0dNgTqvCWlUlGX3Xt2HoNZzfUcfwRnFb4M5UJ2MUsUrzczX
crKi5dm41C9XN0Pw08ZxIRT2FG0sFwqD+1oUTJZAjdQdKbmRqKIFWqFr2EiF75IO
Fu0jSvvxuxwObO78cqo8GyxrNpGvjj8x9Wr/+aNe5f8Z6eFJwhR647qY/7OyNXTe
pDBZwa9sXJSA2YbtPhNjr4QZBDIadhyrVSsw6bxefF6hRbNGcZ0cziodkfEGFzkt
NFUwTKsfj5AARIfaVUPzXs5KWtTc/A==
=g9oP
-----END PGP SIGNATURE-----
