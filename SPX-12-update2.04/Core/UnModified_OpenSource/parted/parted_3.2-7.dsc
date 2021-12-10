-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: parted
Binary: parted, parted-udeb, libparted2, libparted-fs-resize0, libparted2-udeb, libparted-fs-resize0-udeb, libparted-i18n, libparted-dev, libparted0-dev, libparted2-dbg, libparted-fs-resize0-dbg, parted-doc
Architecture: any all
Version: 3.2-7
Maintainer: Parted Maintainer Team <parted-maintainers@lists.alioth.debian.org>
Uploaders: Otavio Salvador <otavio@debian.org>, Bastian Blank <waldi@debian.org>, Xavier Oswald <xoswald@debian.org>, Colin Watson <cjwatson@debian.org>
Homepage: http://www.gnu.org/software/parted
Standards-Version: 3.8.4
Vcs-Browser: http://anonscm.debian.org/gitweb/?p=parted/debian/parted.git
Vcs-Git: git://anonscm.debian.org/parted/debian/parted.git
Build-Depends: dpkg-dev (>= 1.15.7~), debhelper (>= 9~), dh-exec, libncurses-dev | libncurses5-dev, libreadline-dev | libreadline6-dev, libdevmapper-dev (>= 2:1.02.39) [linux-any], uuid-dev, gettext, texinfo (>= 4.2), debianutils (>= 1.13.1), libblkid-dev, pkg-config, check, dh-autoreconf, autoconf (>= 2.63), automake (>= 1:1.11.6), autopoint
Package-List:
 libparted-dev deb libdevel optional arch=any
 libparted-fs-resize0 deb libs optional arch=any
 libparted-fs-resize0-dbg deb debug extra arch=any
 libparted-fs-resize0-udeb udeb debian-installer extra arch=any
 libparted-i18n deb localization optional arch=all
 libparted0-dev deb oldlibs extra arch=all
 libparted2 deb libs optional arch=any
 libparted2-dbg deb debug extra arch=any
 libparted2-udeb udeb debian-installer extra arch=any
 parted deb admin optional arch=any
 parted-doc deb doc optional arch=all
 parted-udeb udeb debian-installer extra arch=any
Checksums-Sha1:
 78db6ca8dd6082c5367c8446bf6f7ae044091959 1655244 parted_3.2.orig.tar.xz
 89f3ec6e15335a26dccf0193b271a7a714978cbe 55408 parted_3.2-7.debian.tar.xz
Checksums-Sha256:
 858b589c22297cacdf437f3baff6f04b333087521ab274f7ab677cb8c6bb78e4 1655244 parted_3.2.orig.tar.xz
 7567a11af568a595ebd56860fce2817e4f743a1fff650045bd446f5e20fb533e 55408 parted_3.2-7.debian.tar.xz
Files:
 0247b6a7b314f8edeb618159fa95f9cb 1655244 parted_3.2.orig.tar.xz
 4db9e9866fe41ed018f1d941f4096144 55408 parted_3.2-7.debian.tar.xz

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1
Comment: Colin Watson <cjwatson@debian.org> -- Debian developer

iQIVAwUBVQqtbDk1h9l9hlALAQhljg/7BakzPuGFRZeAEYObWjpGnfLyWTKPPR39
CAn9ivnpD9VUltx+kDpx6igpTdBgI97Yj2zaai0A1p7Of0nAZTKkS1y/7NacEam5
2rizY0GmdmAXl+Z4hFdIE7vFxR9FCTQxfk4YQajpQxx3H4hldGF5wAyEKPmT09IL
amfEbaqh/8VPZo0vfk0wd3ozYhIDPrZyE7L4Qr73dDPGXbHea3+p9HFgOpd/SJxX
V3lEy/pyv5TUKdwePFiaZRRuyg/qvdYxr4Y22J64gqablYI5y9efLHiF4oSsFaW4
nz+5WQlcRmer45VZItCPTYcbWsUxXu82AtWUas2QppGAnacwxhFnk9kHEM3kbeML
eLbt1/QcVECpY1MXmk9EkZhBqFNhfd62egKdRXx5ukDWY8ppKIbwt/dazKR8LCuY
KHre+V7CYIFVqoU21hr4+qJ5edudVKusopVhpDyT3qmYb9JDlm3gWb92viHalfDW
Jp1fi3InH4zEbPwnNcumpdeXoNC5dNopN/moJ+mpNigELeplL+eVUkb0W3Dq0Ldh
Ozo8+uWxaUVSW/EmMUBiWcr/H61d0BswMmCpCUj0Op3xURphqghg11DM4jsWDkYq
cu858VG1wwYnrHhQXStXi6RySDEfz179nsb/nLZgh0J9RoS3QpbY/7Zd5SpX4cX6
kGYcGXOMF9I=
=XVVi
-----END PGP SIGNATURE-----
