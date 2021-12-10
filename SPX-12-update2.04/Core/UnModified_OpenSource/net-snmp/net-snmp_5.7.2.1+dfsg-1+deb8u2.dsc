-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: net-snmp
Binary: snmpd, snmptrapd, snmp, libsnmp-base, libsnmp30, libsnmp30-dbg, libsnmp-dev, libsnmp-perl, python-netsnmp, tkmib
Architecture: any all
Version: 5.7.2.1+dfsg-1+deb8u2
Maintainer: Net-SNMP Packaging Team <pkg-net-snmp-devel@lists.alioth.debian.org>
Uploaders: Jochen Friedrich <jochen@scram.de>, Thomas Anders <tanders@users.sourceforge.net>, Noah Meyerhans <noahm@debian.org>, Hideki Yamane <henrich@debian.org>
Homepage: http://net-snmp.sourceforge.net/
Standards-Version: 3.9.5
Vcs-Browser: http://anonscm.debian.org/gitweb/?p=pkg-net-snmp/pkg-net-snmp.git
Vcs-Git: git://anonscm.debian.org/pkg-net-snmp/pkg-net-snmp.git
Build-Depends: debhelper (>= 9), libtool, libwrap0-dev, libssl-dev (>> 0.9.8), perl (>= 5.8), libperl-dev, python-all (>= 2.6.6-3~), python-setuptools (>= 0.6b3), python2.7-dev, autoconf, automake, autotools-dev, debianutils (>= 1.13.1), dh-autoreconf, bash (>= 2.05), findutils (>= 4.1.20), procps, libbsd-dev [kfreebsd-i386 kfreebsd-amd64], libkvm-dev [kfreebsd-i386 kfreebsd-amd64], libsensors4-dev [!hurd-i386 !kfreebsd-i386 !kfreebsd-amd64], libmysqld-dev, libpci-dev
Build-Conflicts: libsnmp-dev
Package-List:
 libsnmp-base deb libs optional arch=all
 libsnmp-dev deb libdevel optional arch=any
 libsnmp-perl deb perl optional arch=any
 libsnmp30 deb libs optional arch=any
 libsnmp30-dbg deb debug extra arch=any
 python-netsnmp deb python optional arch=any
 snmp deb net optional arch=any
 snmpd deb net optional arch=any
 snmptrapd deb net optional arch=any
 tkmib deb net optional arch=all
Checksums-Sha1:
 887dfc3c27770ba53740f9fe70907b3ab77b1cfe 3300532 net-snmp_5.7.2.1+dfsg.orig.tar.xz
 3f56310121760b0b633724b9d1fc81ca175bfb7e 64860 net-snmp_5.7.2.1+dfsg-1+deb8u2.debian.tar.xz
Checksums-Sha256:
 0cea52558582a8e104d58c47e49cd967e6b78d8b086adc52bd0d195a8b5733e9 3300532 net-snmp_5.7.2.1+dfsg.orig.tar.xz
 4b67959a12846a386e0e0e720792b6aedc6dea4cfe9be9d82bc4e5917098c606 64860 net-snmp_5.7.2.1+dfsg-1+deb8u2.debian.tar.xz
Files:
 4f878c027f36aeab0118d10606fac8f1 3300532 net-snmp_5.7.2.1+dfsg.orig.tar.xz
 bad247fbe11ae4bd63a439f3a00838a6 64860 net-snmp_5.7.2.1+dfsg-1+deb8u2.debian.tar.xz
Python-Version: 2.7

-----BEGIN PGP SIGNATURE-----

iQKjBAEBCgCNFiEErPPQiO8y7e9qGoNf2a0UuVE7UeQFAlu9DrVfFIAAAAAALgAo
aXNzdWVyLWZwckBub3RhdGlvbnMub3BlbnBncC5maWZ0aGhvcnNlbWFuLm5ldEFD
RjNEMDg4RUYzMkVERUY2QTFBODM1RkQ5QUQxNEI5NTEzQjUxRTQPHGFwb0BkZWJp
YW4ub3JnAAoJENmtFLlRO1HkBjAP/3VicV20ilxr8ZXPr8rtnAawBVuyXndoWii1
4JEAZk1PQCd+6G5a+q0KGoX4K0B2yQ/Jxxw9158yeYe3gqjU4t2KM+tGGC8Le1dw
XWDbCu6Y1XeyYX1jkp19iLWf7Z69fSbnyJS8ta98lsqd5s3ZyJSzPKe45ZKl8aJe
v35os4AChxfYuCaRaBPNVzoY4s1G8XkGKLSXfJ1VnIKriNmWXNUH+6i0YRDxDY6V
HWofcmcxJROyvI/9fzU5sSwslOUzscxUrJl2iIK+ERwOi1UFdNcgUFeCmvXsFVYT
rmXr5qNjgfwP8AwSkHEMIaZOrnGzEVMJ/szMHURWtVyB6BFJ2x6UWS8O/zkGpguV
aeZuV9Zj6nrJW1uqkKFgPMV4/DoZOM3bFiB6q7aX/VF8xRojOxyipjTqz1e2uCzh
JIRzz9lLTUZCY5SwDlxjzyk/qqiHTpjO57hlYaeAZLWP+BQsGueTm0ypQ/X7SHcT
A0wHoul29JpUrUO9r/AU92M9jrpKM2vWX05v2L/G4tJLWxP3QML715nSV0dkQI6p
k1hxnjOE7w+V6nFFH0xtcCNLj140F/PxDuBFZ3T5RJ+OjyWiBsib6X+xzUNDexP4
0fdkA4YsOfKMPFL7HrmzJtLHPpxxggNl623VKKWBPDQMaz40hA2lx+JLqB3HIqvY
uIYBLE8t
=bpz0
-----END PGP SIGNATURE-----
