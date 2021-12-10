-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: avahi
Binary: avahi-daemon, avahi-dnsconfd, avahi-autoipd, python-avahi, avahi-utils, avahi-discover, libavahi-common3, libavahi-common-data, libavahi-common-dev, libavahi-core7, libavahi-core-dev, libavahi-client3, libavahi-client-dev, libavahi-glib1, libavahi-glib-dev, libavahi-gobject0, libavahi-gobject-dev, libavahi-qt4-1, libavahi-qt4-dev, libavahi-compat-libdnssd1, libavahi-compat-libdnssd-dev, libavahi-ui0, libavahi-ui-dev, libavahi-ui-gtk3-0, libavahi-ui-gtk3-dev, avahi-ui-utils, avahi-dbg
Architecture: any all
Version: 0.6.31-5
Maintainer: Utopia Maintenance Team <pkg-utopia-maintainers@lists.alioth.debian.org>
Uploaders: Sjoerd Simons <sjoerd@debian.org>, Sebastian Dröge <slomo@debian.org>, Loic Minier <lool@dooz.org>, Michael Biebl <biebl@debian.org>
Homepage: http://avahi.org/
Standards-Version: 3.9.5
Vcs-Browser: http://anonscm.debian.org/gitweb/?p=pkg-utopia/avahi.git;a=summary
Vcs-Git: git://anonscm.debian.org/pkg-utopia/avahi.git
Build-Depends: debhelper (>= 9), dh-systemd (>= 1.5), autotools-dev, pkg-config, dh-autoreconf, libcap-dev (>= 1:2.16) [linux-any], libgdbm-dev, libglib2.0-dev (>= 2.4), libgtk2.0-dev (>= 2.14.0), libgtk-3-dev, libexpat-dev, libdaemon-dev (>= 0.11), libdbus-1-dev (>= 0.60), python-all-dev (>= 2.6.6-3~), python-gdbm (>= 2.4.3), python-dbus, python-gtk2 (>= 2.8.6-2), libqt4-dev, xmltoman, intltool (>= 0.35.0)
Package-List:
 avahi-autoipd deb net optional arch=linux-any
 avahi-daemon deb net optional arch=any
 avahi-dbg deb debug extra arch=any
 avahi-discover deb net optional arch=all
 avahi-dnsconfd deb net optional arch=any
 avahi-ui-utils deb utils optional arch=any
 avahi-utils deb net optional arch=any
 libavahi-client-dev deb libdevel optional arch=any
 libavahi-client3 deb libs optional arch=any
 libavahi-common-data deb libs optional arch=any
 libavahi-common-dev deb libdevel optional arch=any
 libavahi-common3 deb libs optional arch=any
 libavahi-compat-libdnssd-dev deb libdevel optional arch=any
 libavahi-compat-libdnssd1 deb libs optional arch=any
 libavahi-core-dev deb libdevel optional arch=any
 libavahi-core7 deb libs optional arch=any
 libavahi-glib-dev deb libdevel optional arch=any
 libavahi-glib1 deb libs optional arch=any
 libavahi-gobject-dev deb libdevel optional arch=any
 libavahi-gobject0 deb libs optional arch=any
 libavahi-qt4-1 deb libs optional arch=any
 libavahi-qt4-dev deb libdevel optional arch=any
 libavahi-ui-dev deb libdevel optional arch=any
 libavahi-ui-gtk3-0 deb libs optional arch=any
 libavahi-ui-gtk3-dev deb libdevel optional arch=any
 libavahi-ui0 deb libs optional arch=any
 python-avahi deb python optional arch=any
Checksums-Sha1:
 7e05bd78572c9088b03b1207a0ad5aba38490684 1268686 avahi_0.6.31.orig.tar.gz
 184eef9b5273ccd9ff0acc5902684756203c59eb 31300 avahi_0.6.31-5.debian.tar.xz
Checksums-Sha256:
 8372719b24e2dd75de6f59bb1315e600db4fd092805bd1201ed0cb651a2dab48 1268686 avahi_0.6.31.orig.tar.gz
 3fd413d85ab8650d448adbdf82fddbff688d159d19a3f2c8ba26c1a49ee7605d 31300 avahi_0.6.31-5.debian.tar.xz
Files:
 2f22745b8f7368ad5a0a3fddac343f2d 1268686 avahi_0.6.31.orig.tar.gz
 320c4405f9950e588cb84aa666e7da18 31300 avahi_0.6.31-5.debian.tar.xz

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1

iQIcBAEBCAAGBQJVLCclAAoJEGrh3w1gjyLcX38P/3DKfbvvq3qCQGPRs0l9qyWd
x7pNFr/hIWc8EJm208BeKRwOoXfNVW2l1TWvYZgyhKLjY7A609U6hKqolRKEY8LV
XKzXcxiNCq8kjy7eTzIds4Z4neSlbyOxPx/twkfT0ag2rr2oFzeR/yA+xeQ8kSrR
VRIkmUW/+pQQb9SU9uym3/aKb4iY3mFVF4WH9VYCqvUTPRFfIKg5m5XUDvAVn0wP
IAb+rRTG566qpPUZV4FBsk3VW3sOPdwRhXxFYiGRZUMXRdkSU7g7r1RQMUEA1iFL
tIvVl3o6BNGrQRe78mRhgnuIISFQ1Qg1DttNEscMIIJjC7bbkE3j5ZWthapPcPdO
9ImRovx1Q9j32+U0KGWVbavI22icE/Jimw1Y7OGfBIHaFdhwNb40vcZLypxtTe/u
kr6lJT9/cD06/feyFusb0fN8OoWpXkm4o0Y01UKJjbMy6FWpQZ3E8Ce1MnaLx4cW
jG6dYRCvcPJ0hNt1voidgQca6IAhMBXvbspXC9VFy8Jcgyyqe27g0e+EhIQnauqN
5+MTV0KNr69ixYpsgMhS4mrSNzZoRKUc8chIK2AlL1T9o7NxXWQ8yqb3MjMNYW4r
oXw1m2cd3nHGpNJOtBmQ8II448sPR9XeIy6G8yIELFjhBzcyYPSx+99iNlEx8cqJ
/hvkl7eTWIxNvQ6L57v+
=wVPY
-----END PGP SIGNATURE-----
