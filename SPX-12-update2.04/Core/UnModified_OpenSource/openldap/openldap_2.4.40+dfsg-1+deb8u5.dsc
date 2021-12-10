-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 1.0
Source: openldap
Binary: slapd, slapd-smbk5pwd, ldap-utils, libldap-2.4-2, libldap-2.4-2-dbg, libldap2-dev, slapd-dbg
Architecture: any
Version: 2.4.40+dfsg-1+deb8u5
Maintainer: Debian OpenLDAP Maintainers <pkg-openldap-devel@lists.alioth.debian.org>
Uploaders: Roland Bauerschmidt <rb@debian.org>, Steve Langasek <vorlon@debian.org>, Torsten Landschoff <torsten@debian.org>, Matthijs Möhlmann <matthijs@cacholong.nl>, Timo Aaltonen <tjaalton@ubuntu.com>, Ryan Tandy <ryan@nardis.ca>
Homepage: http://www.openldap.org/
Standards-Version: 3.9.1
Vcs-Browser: http://anonscm.debian.org/gitweb/?p=pkg-openldap/openldap.git
Vcs-Git: git://anonscm.debian.org/pkg-openldap/openldap.git
Build-Depends: debhelper (>= 8.9.0~), dpkg-dev (>= 1.16.1), libdb5.3-dev, nettle-dev, libgnutls28-dev, unixodbc-dev, libncurses5-dev, libperl-dev (>= 5.8.0), libsasl2-dev, libslp-dev, libltdl-dev | libltdl3-dev (>= 1.4.3), libwrap0-dev, perl, po-debconf, quilt (>= 0.46-7), groff-base, time, heimdal-multidev, dh-autoreconf
Build-Conflicts: autoconf2.13, bind-dev, libbind-dev, libicu-dev
Package-List:
 ldap-utils deb net optional arch=any
 libldap-2.4-2 deb libs standard arch=any
 libldap-2.4-2-dbg deb debug extra arch=any
 libldap2-dev deb libdevel extra arch=any
 slapd deb net optional arch=any
 slapd-dbg deb debug extra arch=any
 slapd-smbk5pwd deb net extra arch=any
Checksums-Sha1:
 b80c48f2b7cbf634a3d463b7eb4ca38f081ce2eb 4797667 openldap_2.4.40+dfsg.orig.tar.gz
 668b2c65c4b109d2118bad0444321bd4affeef0d 184219 openldap_2.4.40+dfsg-1+deb8u5.diff.gz
Checksums-Sha256:
 86c0326dc3dc5f1a9b3c25f7106b96f3eafcdf5da090b1fc586dec57d56e0e7f 4797667 openldap_2.4.40+dfsg.orig.tar.gz
 a9836c1dbd548a6525964c3a128267834b004d1c467e27a2b63dd79dc4625072 184219 openldap_2.4.40+dfsg-1+deb8u5.diff.gz
Files:
 8d84a916e2312aade2a3d7b2308a9a69 4797667 openldap_2.4.40+dfsg.orig.tar.gz
 c594b2aa7ffddbbeb74540327d5ac4c2 184219 openldap_2.4.40+dfsg-1+deb8u5.diff.gz

-----BEGIN PGP SIGNATURE-----

iQKjBAEBCgCNFiEErPPQiO8y7e9qGoNf2a0UuVE7UeQFAl1ZnytfFIAAAAAALgAo
aXNzdWVyLWZwckBub3RhdGlvbnMub3BlbnBncC5maWZ0aGhvcnNlbWFuLm5ldEFD
RjNEMDg4RUYzMkVERUY2QTFBODM1RkQ5QUQxNEI5NTEzQjUxRTQPHGFwb0BkZWJp
YW4ub3JnAAoJENmtFLlRO1HkUVQP/RoH6VzPs8X/F/DExuQHozsKGLKPX2bJokgj
zPOmjMp2pKIEWJbbjBbQnCOx1ybapDirE1ZjbGMMamCXKl+m7Z2nfuewDwjBHCIm
FC582DVrDqFkg8ZJnnK5pbVwSNI5mfZ6I5le6x4OicHNqHbduXpZRK0gi/nE/m1J
vqjj8xdqeRNJyNZVUjyi5XUxxIhf0xR1Ra0gKK9swM9V2IjMrFF0AbtSKeZ9aeYl
mXREBjOZj6eGFNtIUUwi52QwHtX06E74v4GByuclUx8fyHJUME3iJYaIIIahGTSv
vuiUgLCL7BwHrTRi1cmj7UImTtI+7fdcZbT2LWojF9mUQpImXO2nenMhX4TRbK/y
hs/szifSCpscoauE0gEOykB1gOcieBTnZwxNUw+cMd11eTMfE3c+g7No8mNgWvYk
9jhi9rDbzwusrTax2BVZs+nF7lWIoxN+vsuXwgx/sJgqrX5mH+DmcWhbEpTdmbka
b85RQwGl4C2NJG0cvErzc2pdRiGOfsRLAGGOOK8zgJ1gCTg9f5BdMaqdcKmAea3l
CqtmkiP6QKkD4NgInH0ORbA5hrrCsCYT7h7GU7DcgFUL45So1gkmAXct8i1ZXuZh
773PvloSKUu3IDbq8qYDfU3vttRRqKrKRh5Ps85WdgiN0cm7yyBsSX6np1z3GJ1g
xxiM1IkT
=PU8U
-----END PGP SIGNATURE-----
