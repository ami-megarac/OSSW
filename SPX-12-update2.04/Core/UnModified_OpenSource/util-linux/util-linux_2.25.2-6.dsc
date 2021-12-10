-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: util-linux
Binary: util-linux, util-linux-locales, mount, bsdutils, fdisk-udeb, cfdisk-udeb, libblkid1, libblkid1-udeb, libblkid-dev, libmount1, libmount-dev, libsmartcols1, libsmartcols1-udeb, libsmartcols-dev, libuuid1, uuid-runtime, libuuid1-udeb, uuid-dev, util-linux-udeb
Architecture: any all
Version: 2.25.2-6
Maintainer: Debian util-linux Maintainers <ah-util-linux@debian.org>
Uploaders: Scott James Remnant <scott@ubuntu.com>, LaMont Jones <lamont@debian.org>, Andreas Henriksson <andreas@fatal.se>, Adam Conrad <adconrad@0c3.net>
Standards-Version: 3.9.5
Vcs-Browser: https://anonscm.debian.org/gitweb/?p=collab-maint/pkg-util-linux.git
Vcs-Git: git://git.debian.org/git/collab-maint/pkg-util-linux.git
Build-Depends: autoconf, automake, autopoint, debhelper (>= 9), dh-autoreconf, dh-exec (>= 0.13), dh-systemd (>= 1.5), dpkg-dev (>= 1.16.0), gettext, libncurses5-dev, libpam0g-dev, libselinux1-dev [linux-any], libslang2-dev (>= 2.0.4), libsystemd-dev [linux-any], libtool, lsb-release, pkg-config, po-debconf, systemd [linux-any], zlib1g-dev
Package-List:
 bsdutils deb utils required arch=any
 cfdisk-udeb udeb debian-installer extra arch=hurd-any,linux-any
 fdisk-udeb udeb debian-installer extra arch=hurd-any,linux-any
 libblkid-dev deb libdevel optional arch=any
 libblkid1 deb libs required arch=any
 libblkid1-udeb udeb debian-installer optional arch=any
 libmount-dev deb libdevel optional arch=linux-any
 libmount1 deb libs required arch=any
 libsmartcols-dev deb libdevel optional arch=any
 libsmartcols1 deb libs required arch=any
 libsmartcols1-udeb udeb debian-installer optional arch=any
 libuuid1 deb libs required arch=any
 libuuid1-udeb udeb debian-installer optional arch=any
 mount deb admin required arch=linux-any
 util-linux deb utils required arch=any
 util-linux-locales deb localization optional arch=all
 util-linux-udeb udeb debian-installer optional arch=any
 uuid-dev deb libdevel optional arch=any
 uuid-runtime deb utils optional arch=any
Checksums-Sha1:
 854f37255ca8098e88fb4220c244b727f5dd6d58 3703644 util-linux_2.25.2.orig.tar.xz
 920b0402d31389190393ba928f293fd056ff2988 304292 util-linux_2.25.2-6.debian.tar.xz
Checksums-Sha256:
 e0457f715b73f4a349e1acb08cb410bf0edc9a74a3f75c357070f31f70e33cd6 3703644 util-linux_2.25.2.orig.tar.xz
 b500d70a60f2814d6552492cee5f40c27063029ef74ddea53bc713503680527b 304292 util-linux_2.25.2-6.debian.tar.xz
Files:
 cab3d7be354000f629bc601238b629b3 3703644 util-linux_2.25.2.orig.tar.xz
 d9590bd25529004b547acb1bc4e7b6ee 304292 util-linux_2.25.2-6.debian.tar.xz

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1

iQIcBAEBCgAGBQJVGH4NAAoJEAvEfcZNE1MGytYP/Rs4dluQKPVxhJKF9N4SunhP
kscHlPQsyAnpIDxz7tof4Oy5QI5Au41k/Wi0sBE4VUP7y1dec9YJgVk+lSFHDuK0
HAtItBjwiHrE6leSMhS1NnR+6uEGxQ6j8/PsgtLDyrE2lKVQ076blIJ1O3FjoU3N
XqTWBJPTkg93YhdzYknbhnTxp5dEC4nXxlloQzqJ8EfiRJLD7BPfY7ffdWVzzWRR
v1mVCFhi1tucdWoGHZqMM9D73xffEcewNp2o1I3JwoF9b076jor9y6IuRnxCaMqQ
klMktAKp4PlrrECQDnfhI7fcqHF6PIDZwaPTCxJNcDbF+T+8zmuDMmcJRRhz0UZZ
RcnlwGFxMjhXdMns0dEMKw6U+ivdyITSnBRDH49uuqo4ZGgWXRFR5z1VSzZ/neY8
CyGMws0jF1bGlFwuQtHtWVAx/aSiJu1TskEyBGs++UR9qxhesX2gKa+jAIVv8hGD
PZ38Pib1VWsdBxqelm+M4pzJhYtsR4MnuKzkal/cATbXkruUVulVYs3yHIwZThPf
+WldRr4v/5H1+LORGB7QD8JLyIBuBa/kuDCXM5EB8RF++PYandW/bN4urqUDXfg9
ND3paFTspFfOTzyY0IC+DYk+76M+HcBzJ/UQITPSAOVRb7Uz5NlDU5Ji4wSutr8A
bqVgkaBVKlNXbm+2YjcM
=zQ3a
-----END PGP SIGNATURE-----
