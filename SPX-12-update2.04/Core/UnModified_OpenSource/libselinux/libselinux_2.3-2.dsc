-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: libselinux
Binary: selinux-utils, libselinux1, libselinux1-dev, ruby-selinux, python-selinux
Architecture: linux-any
Version: 2.3-2
Maintainer: Debian SELinux maintainers <selinux-devel@lists.alioth.debian.org>
Uploaders: Manoj Srivastava <srivasta@debian.org>, Russell Coker <russell@coker.com.au>
Homepage: http://userspace.selinuxproject.org/
Standards-Version: 3.9.5
Vcs-Browser: http://anonscm.debian.org/gitweb/?p=selinux/libselinux.git;a=summary
Vcs-Git: git://anonscm.debian.org/selinux/libselinux.git
Build-Depends: debhelper (>= 9), file, gem2deb (>= 0.5.0~), libsepol1-dev (>= 2.3), libpcre3-dev, pkg-config, python-all-dev (>= 2.6.6-3~), swig
Package-List:
 libselinux1 deb libs required arch=linux-any
 libselinux1-dev deb libdevel optional arch=linux-any
 python-selinux deb python optional arch=linux-any
 ruby-selinux deb ruby optional arch=linux-any
 selinux-utils deb admin optional arch=linux-any
Checksums-Sha1:
 14669a7119a314c91754e7c1c6ae2ff1103aba9f 171254 libselinux_2.3.orig.tar.gz
 905a6ad71a14b7043e956868ccac94864eebdaec 24384 libselinux_2.3-2.debian.tar.xz
Checksums-Sha256:
 0b1e0b43ecd84a812713d09564019b08e7c205d89072b5cbcd07b052cd8e77b2 171254 libselinux_2.3.orig.tar.gz
 8ec4bdb5acc066d1b369877e9a94ec1a723e4d31691753e0e1861d0884b3fd1a 24384 libselinux_2.3-2.debian.tar.xz
Files:
 b11d4d95ef4bde732dbc8462df57a1e5 171254 libselinux_2.3.orig.tar.gz
 f5b29653930819a24155b42e78063e4e 24384 libselinux_2.3-2.debian.tar.xz
Ruby-Versions: all

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1

iQEcBAEBCAAGBQJUA3F5AAoJEB/FiR66sEPVcsEIAKLxHBXcB5mxgZ46ioxmXGBP
x7z5boauF/AFTzF9EuAw+wLM60mrrwithVV08f8QX1NFlSZlTRZ1TYPq0FflPm5n
5wGeVeHhcUTSiF/JrrWzOurY2pp4FmFAvKhp7vFTD8bCq1XQTS5twJ8TdqznjW6V
n6P7ApdijJG0gXlcD8b9rnh21QLbJb5RHA5cz90oR26QGhP4yZPHErL52a9h2PUq
KfKrUq9rUjh8Mx7a2k5vSjv4xQJv1Nw9ryQ19XophqpR6S9J9U3PYqnyOv3/+RRl
GA2MuhcCs3uS4JnMayPkFb+KylC0DbuKKqhvwi5tqwyqBGv0d0uxFo0wS+6UnVw=
=JspZ
-----END PGP SIGNATURE-----
