-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: lvm2
Binary: lvm2, lvm2-udeb, clvm, libdevmapper-dev, libdevmapper1.02.1, libdevmapper1.02.1-udeb, dmsetup, dmsetup-udeb, libdevmapper-event1.02.1, dmeventd, liblvm2app2.2, liblvm2cmd2.02, liblvm2-dev
Architecture: any
Version: 2.02.111-2.2+deb8u1
Maintainer: Debian LVM Team <pkg-lvm-maintainers@lists.alioth.debian.org>
Uploaders: Bastian Blank <waldi@debian.org>
Homepage: http://sources.redhat.com/lvm2/
Standards-Version: 3.9.5
Vcs-Browser: http://svn.debian.org/wsvn/pkg-lvm/lvm2/trunk/
Vcs-Svn: svn://svn.debian.org/pkg-lvm/lvm2/trunk/
Build-Depends: dpkg-dev (>= 1.16.1~), debhelper (>= 8.1.3~), dh-systemd, automake, libcman-dev (>> 2), libcorosync-dev, libdlm-dev (>> 2), libreadline-gplv2-dev, libselinux1-dev, libudev-dev, openais-dev, pkg-config
Package-List:
 clvm deb admin extra arch=any
 dmeventd deb admin optional arch=any
 dmsetup deb admin optional arch=any
 dmsetup-udeb udeb debian-installer optional arch=any
 libdevmapper-dev deb libdevel optional arch=any
 libdevmapper-event1.02.1 deb libs optional arch=any
 libdevmapper1.02.1 deb libs optional arch=any
 libdevmapper1.02.1-udeb udeb debian-installer optional arch=any
 liblvm2-dev deb libdevel optional arch=any
 liblvm2app2.2 deb libs optional arch=any
 liblvm2cmd2.02 deb libs optional arch=any
 lvm2 deb admin optional arch=any
 lvm2-udeb udeb debian-installer optional arch=any
Checksums-Sha1:
 ac0a93053ca09f4cd27a1569fad5085a1cce4445 1497626 lvm2_2.02.111.orig.tar.gz
 10dacc7c7ab620c91031fdf0a05f3db71d0ad730 29476 lvm2_2.02.111-2.2+deb8u1.debian.tar.xz
Checksums-Sha256:
 ff358054ee821503ada8a33b327688cd4d64a2fc448c667a85c332c545aae4f6 1497626 lvm2_2.02.111.orig.tar.gz
 df657682e06f9559d04719c1543285e8bcb51fb4647f673b787538fb34ebe26f 29476 lvm2_2.02.111-2.2+deb8u1.debian.tar.xz
Files:
 fb748f698e52a6f5eb8db69ef965824d 1497626 lvm2_2.02.111.orig.tar.gz
 65a94e57bc61b91a2168a5a72234195e 29476 lvm2_2.02.111-2.2+deb8u1.debian.tar.xz

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v2

iQEcBAEBCAAGBQJXSA9xAAoJEG2TiIWKaf5RhLQH/RQSZ6VHiWsRdZg/JrNseLKi
8aY74egEgZtUgQJnfmu0D/MkFrg/g638seeAs2dNMXCAMFX3edd0myrVy8hoGzgl
dOEAzQHABkCLK8UPfoxxpBQc8n3WS19WmVe0UFa2d8LTWs5clYZ6ijUvm8EnJrgR
FMZyNYxptKL9xRg1KaKbua21EZVqnvd06hhggsmTb+z4XXs/ixcbu2LGTntFRQkJ
lrjYiZEkckL+bSukt/1aQjpsheSp6ozzZDQmJKACtRG/oYSL8rPGieLfEZH1bQ16
F2/RvQJqtoDsIJZKBu/XApkSiCBwIOhA2TLYgBBwz0VG6S5rMKoRWIgWnUBb0ps=
=T/Ef
-----END PGP SIGNATURE-----
