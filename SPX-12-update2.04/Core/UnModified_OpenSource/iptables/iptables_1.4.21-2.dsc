-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA1

Format: 3.0 (quilt)
Source: iptables
Binary: iptables, libxtables10, iptables-dev
Architecture: linux-any
Version: 1.4.21-2
Maintainer: Laurence J. Lane <ljlane@debian.org>
Homepage: http://www.netfilter.org/
Standards-Version: 3.9.5
Build-Depends: debhelper (>= 9), autoconf, automake, libtool (>= 2.2.6), libnfnetlink-dev, libnetfilter-conntrack-dev, libnetfilter-conntrack3, dh-autoreconf
Package-List: 
 iptables deb net important arch=linux-any
 iptables-dev deb devel optional arch=linux-any
 libxtables10 deb net important arch=linux-any
Checksums-Sha1: 
 85d4160537546a23a7e42bc26dd7ee62a0ede4c8 547439 iptables_1.4.21.orig.tar.bz2
 88f049d0e1144135e2cbbcaf7dd69241b26ec053 51696 iptables_1.4.21-2.debian.tar.xz
Checksums-Sha256: 
 52004c68021da9a599feed27f65defcfb22128f7da2c0531c0f75de0f479d3e0 547439 iptables_1.4.21.orig.tar.bz2
 24989003f67257f254c028731918ee445f2dd40fb6e7e5c55a77cc918212ea31 51696 iptables_1.4.21-2.debian.tar.xz
Files: 
 536d048c8e8eeebcd9757d0863ebb0c0 547439 iptables_1.4.21.orig.tar.bz2
 058b55fd2b5a5a6f561db953ccac30fc 51696 iptables_1.4.21-2.debian.tar.xz

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1

iEYEARECAAYFAlN6p5MACgkQxJBkNlXToelY5QCeMWyKTTREDLia5kNojlObbte2
MS0AoI6mLW2i0fXg2Y1xs3Yor4x+SsVo
=7XkK
-----END PGP SIGNATURE-----
