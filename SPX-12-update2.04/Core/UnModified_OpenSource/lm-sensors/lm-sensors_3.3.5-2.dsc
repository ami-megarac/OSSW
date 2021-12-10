-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: lm-sensors
Binary: lm-sensors, libsensors4, libsensors4-dev, fancontrol, sensord
Architecture: any all
Version: 1:3.3.5-2
Maintainer: Aurelien Jarno <aurel32@debian.org>
Homepage: http://www.lm-sensors.org
Standards-Version: 3.9.5
Build-Depends: debhelper (>= 8.1.3), bison, flex, librrd2-dev, dh-systemd (>= 1.3)
Package-List: 
 fancontrol deb utils extra arch=all
 libsensors4 deb libs optional arch=any
 libsensors4-dev deb libdevel extra arch=any
 lm-sensors deb utils extra arch=any
 sensord deb utils extra arch=any
Checksums-Sha1: 
 179989a52bae8f01ba8ed77e8a15af6ec653d89e 173609 lm-sensors_3.3.5.orig.tar.bz2
 727978ac8238404649521752f96826db443b7ecc 28312 lm-sensors_3.3.5-2.debian.tar.xz
Checksums-Sha256: 
 5dae6a665e1150159a93743c4ff1943a7efe02cd9d3bb12c4805e7d7adcf4fcf 173609 lm-sensors_3.3.5.orig.tar.bz2
 bedd632a169189764383727bbf7cb718c6543e0afa4d368ba0c1bb202593512c 28312 lm-sensors_3.3.5-2.debian.tar.xz
Files: 
 da506dedceb41822e64865f6ba34828a 173609 lm-sensors_3.3.5.orig.tar.bz2
 e612a67c32fef7ca9f6acf214eb1890a 28312 lm-sensors_3.3.5-2.debian.tar.xz

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1

iQIVAwUBU2t9a7qceAYd3YybAQiKzg//Y0psxRUjdf/dniT51BIrTMNNQYF4Nstv
+cmC/NUWWeuOSvQyPl5AKWApHObymHGOtvun1CtYmRUFXSXSGT+h/cYErGYColJB
feMxbfYZqTVojYwA2uK5+3lQB50jL79lI5bMe6jw2LfwlmB+bcleE4cKiGC1bibb
bPADCj5sOixLhnGCTxgdCjglIPmikPOAekabgMmcMVhnwEMYK65SqLBYTGTeA6t3
v6aoz/LfI/EWq5dc3hy3pnRtuVY/7nWrYiQ76UVcfkRyh3MELi7+JnRvckRb7b1s
gbFqKHsgbuQNF3uiUVGwqh+HM1P3n6zl9axVuEfB34gqk0mLBJcc2OFbL8GlqluM
3EJ5gvZwrICFof1UqSk8sUGJkB9lnPkDz/j2sFmJuzcMfGwy08mdXUIcfm04Ch5u
QXWWt+a09q0FwM7NTIpRJcs5bU218JTDdXqqCiTM+8lX+To3QUD/Z5904SbDLM14
GEdEGvwEELopQYuFwjMo4f45zM/ecc7yOlF7hn1avNcOqL7+Z/eNFrsIe81vN1dH
DwyawratB05aF0tpPS3QyvsIhc+yRwUb9NI5OEgbLmJihy1lDcYdIuYByZkuA5dD
ORyXuHEFeFQbGxM5Wd2AdXrrjV4w1GPM+pZi815cHYxWVVFVkSPuK3FRhYpyH+dT
y448qlrt/ZU=
=CFav
-----END PGP SIGNATURE-----
