-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA1

Format: 3.0 (quilt)
Source: zlib
Binary: zlib1g, zlib1g-dev, zlib1g-dbg, zlib1g-udeb, lib64z1, lib64z1-dev, lib32z1, lib32z1-dev, libn32z1, libn32z1-dev
Architecture: any
Version: 1:1.2.8.dfsg-2
Maintainer: Mark Brown <broonie@debian.org>
Homepage: http://zlib.net/
Standards-Version: 3.9.4
Build-Depends: debhelper (>= 8.1.3~), binutils (>= 2.18.1~cvs20080103-2) [mips mipsel], gcc-multilib [amd64 i386 kfreebsd-amd64 mips mipsel powerpc ppc64 s390 sparc s390x], dpkg-dev (>= 1.16.1)
Package-List:
 lib32z1 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x
 lib32z1-dev deb libdevel optional arch=amd64,ppc64,kfreebsd-amd64,s390x
 lib64z1 deb libs optional arch=sparc,s390,i386,powerpc,mips,mipsel
 lib64z1-dev deb libdevel optional arch=sparc,s390,i386,powerpc,mips,mipsel
 libn32z1 deb libs optional arch=mips,mipsel
 libn32z1-dev deb libdevel optional arch=mips,mipsel
 zlib1g deb libs required arch=any
 zlib1g-dbg deb debug extra arch=any
 zlib1g-dev deb libdevel optional arch=any
 zlib1g-udeb udeb debian-installer optional arch=any
Checksums-Sha1:
 c7c723de179c0d1f3c9d59646a398649b16206b7 361943 zlib_1.2.8.dfsg.orig.tar.gz
 1025717923817210046e1b661443490d1296546d 14768 zlib_1.2.8.dfsg-2.debian.tar.xz
Checksums-Sha256:
 2caecc2c3f1ef8b87b8f72b128a03e61c307e8c14f5ec9b422ef7914ba75cf9f 361943 zlib_1.2.8.dfsg.orig.tar.gz
 39af7ea4b264c229f856ed342bb316a796cb2f1e1278a059f2dc5a4f3ffc9f31 14768 zlib_1.2.8.dfsg-2.debian.tar.xz
Files:
 b752e88a9717131354bd07aa1e1c505d 361943 zlib_1.2.8.dfsg.orig.tar.gz
 33acd96a3311d6fe60d94b64427a296e 14768 zlib_1.2.8.dfsg-2.debian.tar.xz

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v2

iQIcBAEBAgAGBQJT79HqAAoJELSic+t+oim9bM4QAJUbXwmKu2KFjNXI64PUbEeQ
Xz0bh5CgMObWaqDgEeHzU1ImXnHGtwDWwmkl3DYA03w3z4RI9LgSj6PH5EONFOa7
AQc6Tbuz5afDa5fiABJnoe+iOZ4VvvFHDEe47J4EtWppz5Fm9sSu/ElHhsfwYlMv
1CS2W0L3FED6tw2i6hdxbgQlDPMS35DKaO6jF1veWYAXp3eN7r2jU92yXKsLtKx4
4i6fXm2zFPrFgP1ihzvRV4Ikm5dUgLPGr8WLyVnei7hxZkgyVI3vz3wjaIhvSkNb
/PpTx/LBrPLPFp7ymQMfa+SteqIMEe1UGYhkLN3SBaXzOnI2aKAeN/2a4yae84sC
nfcNa2UcnFzCq53jq2GDTdxevVUEjIHbSJ7ggd8Yu/StYaIy+92zdlxzV44fvZhZ
mDR+G7CCVCn+07LLBoQojv/v2Uor0hPQgtqjf5rhBsIxObG3Zy47pz1pmDvdmVhq
cy7s3v/c1SenE+LXvF+PRalDmPogmsY90oGoQ0dIgY2Da1ypKsOGAfjS8wTdh3k4
HJW5yYHVYkymRMVRfRXA3cus3ilsgkVp58guspBIn0BCnio4r2kOUZ77ROVlT+N7
V/tvMnLcDfa4j6cKWrJHC/F2clNWgKKfBZkPhlrJnGBFm328f9TaNZEvnfKRDBvZ
wDG4aHMLsptS209CESCV
=+gA5
-----END PGP SIGNATURE-----
