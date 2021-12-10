-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 1.0
Source: pam
Binary: libpam0g, libpam-modules, libpam-modules-bin, libpam-runtime, libpam0g-dev, libpam-cracklib, libpam-doc
Architecture: any all
Version: 1.1.8-3.1+deb8u2
Maintainer: Steve Langasek <vorlon@debian.org>
Uploaders: Sam Hartman <hartmans@debian.org>, Roger Leigh <rleigh@debian.org>
Homepage: http://pam.sourceforge.net/
Standards-Version: 3.9.1
Vcs-Bzr: http://bzr.debian.org/bzr/pkg-pam/debian/sid/
Build-Depends: libcrack2-dev (>= 2.8), bzip2, debhelper (>= 8.9.4), quilt (>= 0.48-1), flex, libdb-dev, libselinux1-dev [linux-any], po-debconf, dh-autoreconf, autopoint, libaudit-dev [linux-any], pkg-config
Build-Depends-Indep: xsltproc, libxml2-utils, docbook-xml, docbook-xsl, w3m
Build-Conflicts: libdb4.2-dev, libxcrypt-dev
Build-Conflicts-Indep: fop
Package-List:
 libpam-cracklib deb admin optional arch=any
 libpam-doc deb doc optional arch=all
 libpam-modules deb admin required arch=any
 libpam-modules-bin deb admin required arch=any
 libpam-runtime deb admin required arch=all
 libpam0g deb libs required arch=any
 libpam0g-dev deb libdevel optional arch=any
Checksums-Sha1:
 d055ed81373280a372468c2434cea6313f8d57a8 1892765 pam_1.1.8.orig.tar.gz
 9b58a0bf486a5e7807b4a489042cce1c101d35c8 134897 pam_1.1.8-3.1+deb8u2.diff.gz
Checksums-Sha256:
 4183409a450708a976eca5af561dbf4f0490141a08e86e4a1e649c7c1b094876 1892765 pam_1.1.8.orig.tar.gz
 84a455ce53c7701d2a536fc33909a6ce19c1a6b7c18e6be8d2d0fc2294260610 134897 pam_1.1.8-3.1+deb8u2.diff.gz
Files:
 5107bbf54042400b6200e8367cc7feef 1892765 pam_1.1.8.orig.tar.gz
 dc839cfaf11bc2ccc3075b3b02f89bad 134897 pam_1.1.8-3.1+deb8u2.diff.gz

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCgAdFiEExXWpV+gZuhi/B+dmobCbQjM5YegFAlgmyRAACgkQobCbQjM5
Yejffg/7B0hHaKEq9z0d8DiK5PclKpiNjXR+aBGC8rivn0g9XKrLRaGKrgJ6khal
Y6PdzIhAifJiVDI0tnECtwA5f2Ld/sSE9dErQBebVKZS/g2JI1ahvjSL08hzTB78
MUu7wLOAc+0WbWMUnaIvF7YkgVUUldc3loZxkE2l2Is5Cfuukexg1Q/N1cc8+Vrq
d+VKwnYe/LjEgwkP+nXxIFEJImyOr/CmihNjWmybdEnzEWXBhPlJriXo8LuqPtGb
P/E2iDWaC2RKM29oveG0rQWKDOFTVnvGEKntexPyPbBV2IjgKlalLXkcwcsdJlpt
VxFHB5eOv2cnguS/sTwwv9RM1eCD6BayA5mgfzEZDZCilPxGlr2qu5B+rLoVlA/n
WJrY7nVXDy+3C2Y/5NLzmDY960Vwbw29utuepjZ4vOaauhaTnyiLjp+rer0R9o8J
Ux89L+LTXY7xB7Rlgtd+64PyMR/SzYvtTqcZBUtGXdGrOLAwIca8XI53qEn1JNkc
hYJ8VjqQgH85LYyOeayxgfkTAoHRsXFdvlF7uixzyDgEqvNbo3fDGM4cwXBHhUBn
DATfv0dDPlxF9x9eUjooPeLzjLdnToQY9httWzTrFjSPud0C7Ch5PvrGUHbv1zK1
/IkqGVMlCEHSHuDxSO4ZHvYuLXS8VEFvMS8cMUVa2uS4c85cR3g=
=HO/T
-----END PGP SIGNATURE-----
