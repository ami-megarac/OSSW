-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: systemd
Binary: systemd, systemd-sysv, libpam-systemd, libsystemd0, libsystemd-dev, libsystemd-login0, libsystemd-login-dev, libsystemd-daemon0, libsystemd-daemon-dev, libsystemd-journal0, libsystemd-journal-dev, libsystemd-id128-0, libsystemd-id128-dev, udev, libudev1, libudev-dev, udev-udeb, libudev1-udeb, libgudev-1.0-0, gir1.2-gudev-1.0, libgudev-1.0-dev, python3-systemd, systemd-dbg
Architecture: linux-any
Version: 215-17+deb8u13
Maintainer: Debian systemd Maintainers <pkg-systemd-maintainers@lists.alioth.debian.org>
Uploaders: Michael Biebl <biebl@debian.org>, Marco d'Itri <md@linux.it>, Michael Stapelberg <stapelberg@debian.org>, Sjoerd Simons <sjoerd@debian.org>, Martin Pitt <mpitt@debian.org>
Homepage: http://www.freedesktop.org/wiki/Software/systemd
Standards-Version: 3.9.6
Vcs-Browser: http://anonscm.debian.org/gitweb/?p=pkg-systemd/systemd.git;a=summary
Vcs-Git: git://anonscm.debian.org/pkg-systemd/systemd.git
Testsuite: autopkgtest
Build-Depends: debhelper (>= 9), pkg-config, xsltproc, docbook-xsl, docbook-xml, gtk-doc-tools, m4, dh-autoreconf, automake (>= 1.11), autoconf (>= 2.63), intltool, gperf, libcap-dev, libpam0g-dev, libaudit-dev, libdbus-1-dev (>= 1.3.2), libglib2.0-dev (>= 2.22.0), libcryptsetup-dev (>= 2:1.6.0), libselinux1-dev (>= 2.1.9), libacl1-dev, liblzma-dev, libgcrypt11-dev, libkmod-dev (>= 15), libblkid-dev (>= 2.20), libgirepository1.0-dev (>= 1.31.1), gobject-introspection (>= 1.31.1), python3-all-dev, python3-lxml, libglib2.0-doc
Package-List:
 gir1.2-gudev-1.0 deb introspection optional arch=linux-any
 libgudev-1.0-0 deb libs optional arch=linux-any
 libgudev-1.0-dev deb libdevel optional arch=linux-any
 libpam-systemd deb admin optional arch=linux-any
 libsystemd-daemon-dev deb oldlibs extra arch=linux-any
 libsystemd-daemon0 deb oldlibs extra arch=linux-any
 libsystemd-dev deb libdevel optional arch=linux-any
 libsystemd-id128-0 deb oldlibs extra arch=linux-any
 libsystemd-id128-dev deb oldlibs extra arch=linux-any
 libsystemd-journal-dev deb oldlibs extra arch=linux-any
 libsystemd-journal0 deb oldlibs extra arch=linux-any
 libsystemd-login-dev deb oldlibs extra arch=linux-any
 libsystemd-login0 deb oldlibs extra arch=linux-any
 libsystemd0 deb libs optional arch=linux-any
 libudev-dev deb libdevel optional arch=linux-any
 libudev1 deb libs important arch=linux-any
 libudev1-udeb udeb debian-installer optional arch=linux-any
 python3-systemd deb python optional arch=linux-any
 systemd deb admin optional arch=linux-any
 systemd-dbg deb debug extra arch=linux-any
 systemd-sysv deb admin extra arch=linux-any
 udev deb admin important arch=linux-any
 udev-udeb udeb debian-installer optional arch=linux-any
Checksums-Sha1:
 7a592f90c0c1ac05c43de45b8fde1f23b5268cb4 2888652 systemd_215.orig.tar.xz
 c8f8c61166e2a0905ece78de72750de5861d8d8d 248816 systemd_215-17+deb8u13.debian.tar.xz
Checksums-Sha256:
 ce76a3c05e7d4adc806a3446a5510c0c9b76a33f19adc32754b69a0945124505 2888652 systemd_215.orig.tar.xz
 7274d5e33a526b06d37558999325b15f8dd773ad9ddd61cc7f5e12f1bca839db 248816 systemd_215-17+deb8u13.debian.tar.xz
Files:
 d2603e9fffd8b18d242543e36f2e7d31 2888652 systemd_215.orig.tar.xz
 148f0ebd4ab73efe3367a0bf58221348 248816 systemd_215-17+deb8u13.debian.tar.xz

-----BEGIN PGP SIGNATURE-----

iQJJBAEBCAAzFiEEm/uu6GwKpf+/IgeCmvRrMCV3GzEFAlzBnPMVHHN1bndlYXZl
ckBkZWJpYW4ub3JnAAoJEJr0azAldxsxapsQAIFQRwFqWAOlUw6haqs4ib4ShWYc
ZDi3h5iptRMM8SxMpQrRz4LoujXZvFKix0iPuQu1u3QGR3kor+pPZ/xCuFvlldxT
XtO/sNA86Kd/W0Cu+T4zgx+e7YJbS2/3otN0qL6mfmrJjWezp/qPikzxVxp8Mq1G
04/8U6v5fev0bv4ZSxYcGsG8X9yma9jPXJZf9UKWNjL8WmKOduIIdzmEPre/8ZH6
MGC76vwjm1l3KwnfFOlQzfDAIRuLrWmV/evOMaMuAWuhRTGObTBzrn4EPY0YZclk
FOqWexorKLPd6fLSx8w4y3UQIhxW1TsFG/8ocV6HQIu/MqPud8vMsjiHyHmCfRrT
9EpWNoD4vIwcrc/uz4yxK9swYBZuZ6Y+23o7fnpppapL2i0pTMC2qBhjY640Ey6m
qAK8zl4Cal1JaeYYhRgzjTSFd89UVK3twrgTPz065IPhRD7NlnK9eWo5TP8Tyn+/
Z3So+ryL/Lbr1Ly6rMa/zLUO7sR0l7rd8fJrKBKeZLnnarw+uE0kakW7y17Z3wfe
pJo82giR5+e0HUP9eu/svN9s9P1s8J4KYuLsWghUVGsWv7SrWvlCiv/MFLZMykVu
/w9W0tnQTjz9+IAz0igKVFJH+RAzJXH8vYg6jqMfquJxmTtemlhdz+Xo0cas+i4Z
fDsquSY5zoC284JB
=lugz
-----END PGP SIGNATURE-----
