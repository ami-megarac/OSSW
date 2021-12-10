-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: audit
Binary: auditd, libauparse0, libauparse-dev, libaudit1, libaudit-common, libaudit-dev, python-audit, audispd-plugins
Architecture: linux-any all
Version: 1:2.4-1
Maintainer: Laurent Bigonville <bigon@debian.org>
Homepage: http://people.redhat.com/sgrubb/audit/
Standards-Version: 3.9.5
Vcs-Browser: http://anonscm.debian.org/gitweb/?p=collab-maint/audit.git
Vcs-Git: git://anonscm.debian.org/collab-maint/audit.git
Build-Depends: debhelper (>= 9), dh-autoreconf, dh-systemd (>= 1.4), dpkg-dev (>= 1.16.1~), intltool, libcap-ng-dev, libkrb5-dev, libldap2-dev, libprelude-dev, libwrap0-dev, python-all-dev (>= 2.6.6-3~), swig
Package-List:
 audispd-plugins deb admin extra arch=linux-any
 auditd deb admin extra arch=linux-any
 libaudit-common deb libs optional arch=all
 libaudit-dev deb libdevel extra arch=linux-any
 libaudit1 deb libs optional arch=linux-any
 libauparse-dev deb libdevel extra arch=linux-any
 libauparse0 deb libs optional arch=linux-any
 python-audit deb python extra arch=linux-any
Checksums-Sha1:
 4cddd98eff5d6fc53dccaa845d87ba560b77cf82 937809 audit_2.4.orig.tar.gz
 4ab20ac469bc9c94b141cb14ffac6567b50428e9 15808 audit_2.4-1.debian.tar.xz
Checksums-Sha256:
 6e5d39e7af9d00477ef60f824be8c93bd23a227869d6993ff36b7e7fa28fe99b 937809 audit_2.4.orig.tar.gz
 c18c1b88c41f3b8be9e59d3041563599f822994cb10574ffc17f00f0a157c12c 15808 audit_2.4-1.debian.tar.xz
Files:
 7d35d9db17c2c5e155bc1fe42b55ec48 937809 audit_2.4.orig.tar.gz
 65d4ae40d7aac72f5c7691f9127be4cf 15808 audit_2.4-1.debian.tar.xz

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1

iQEcBAEBCAAGBQJUDHQ4AAoJEB/FiR66sEPVnScH/j3uUebXMeSQzBtToiiOHBqV
5FHDOaa5OhgyMWQdz11WY2NSjNaLHFgXXfsWri4tlPo/TEW24YF+sUW+W3YY/f3H
979zrIhIm68RNMBif0byaKfMEqpyjR5wyd7TfaFFs1Yyl6tUw83dPcKv6uB1O/pm
bmt+P5dzgzbEWChEeYZj7ub6j8xlp+D+rnit2bBrY1CNxGOuszE0//e7MpjxWIHY
8ccK15NB4JhDjwfF+5/4m5sx5QOxQrEMVHrLiWAPpqfxPWzWQu8yvQ8oxfOiD0k6
/8+8T5Lit+bzixqCjHDsrRBv8R1mSfC5zhjXlJ+KtjKEiL2SzxeeA4tJIYnIioE=
=eEN7
-----END PGP SIGNATURE-----
