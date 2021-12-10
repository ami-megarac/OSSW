-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: krb5
Binary: krb5-user, krb5-kdc, krb5-kdc-ldap, krb5-admin-server, krb5-multidev, libkrb5-dev, libkrb5-dbg, krb5-pkinit, krb5-otp, krb5-doc, libkrb5-3, libgssapi-krb5-2, libgssrpc4, libkadm5srv-mit9, libkadm5clnt-mit9, libk5crypto3, libkdb5-7, libkrb5support0, libkrad0, krb5-gss-samples, krb5-locales, libkrad-dev
Architecture: any all
Version: 1.12.1+dfsg-19+deb8u5
Maintainer: Sam Hartman <hartmans@debian.org>
Uploaders: Russ Allbery <rra@debian.org>, Benjamin Kaduk <kaduk@mit.edu>
Homepage: http://web.mit.edu/kerberos/
Standards-Version: 3.9.5
Vcs-Browser: http://git.debian.org/?p=pkg-k5-afs/debian-krb5-2013.git
Vcs-Git: git://git.debian.org/git/pkg-k5-afs/debian-krb5-2013.git
Build-Depends: debhelper (>= 8.1.3), byacc | bison, comerr-dev, docbook-to-man, doxygen, libkeyutils-dev [linux-any], libldap2-dev, libncurses5-dev, libssl-dev, ss-dev, libverto-dev (>= 0.2.4), pkg-config, dh-systemd
Build-Depends-Indep: python-cheetah, python-lxml, python-sphinx, doxygen-latex
Package-List:
 krb5-admin-server deb net optional arch=any
 krb5-doc deb doc optional arch=all
 krb5-gss-samples deb net extra arch=any
 krb5-kdc deb net optional arch=any
 krb5-kdc-ldap deb net extra arch=any
 krb5-locales deb localization standard arch=all
 krb5-multidev deb libdevel optional arch=any
 krb5-otp deb net extra arch=any
 krb5-pkinit deb net extra arch=any
 krb5-user deb net optional arch=any
 libgssapi-krb5-2 deb libs standard arch=any
 libgssrpc4 deb libs standard arch=any
 libk5crypto3 deb libs standard arch=any
 libkadm5clnt-mit9 deb libs standard arch=any
 libkadm5srv-mit9 deb libs standard arch=any
 libkdb5-7 deb libs standard arch=any
 libkrad-dev deb libdevel extra arch=any
 libkrad0 deb libs standard arch=any
 libkrb5-3 deb libs standard arch=any
 libkrb5-dbg deb debug extra arch=any
 libkrb5-dev deb libdevel extra arch=any
 libkrb5support0 deb libs standard arch=any
Checksums-Sha1:
 d211e7d605bd992d33b7cbca1da14d68f0770258 11792370 krb5_1.12.1+dfsg.orig.tar.gz
 033d6e98bf289f19f81c06ed51908d6f11615f2a 129708 krb5_1.12.1+dfsg-19+deb8u5.debian.tar.xz
Checksums-Sha256:
 eb29959f1e9f8d71e7401f5809daefae067296eb5b0da1176366280a16bdd784 11792370 krb5_1.12.1+dfsg.orig.tar.gz
 e4d511b15cf9f812e81ff98076604325f2dfa46e72f713d3c60eaa875e78f380 129708 krb5_1.12.1+dfsg-19+deb8u5.debian.tar.xz
Files:
 dd0367010b3d2385d9f23db25457a0bf 11792370 krb5_1.12.1+dfsg.orig.tar.gz
 242d35e2266014e20e009c869eddcd4a 129708 krb5_1.12.1+dfsg-19+deb8u5.debian.tar.xz

-----BEGIN PGP SIGNATURE-----

iQKnBAEBCgCRFiEEYgH7/9u94Hgi6ruWlvysDTh7WEcFAlxLWbJfFIAAAAAALgAo
aXNzdWVyLWZwckBub3RhdGlvbnMub3BlbnBncC5maWZ0aGhvcnNlbWFuLm5ldDYy
MDFGQkZGREJCREUwNzgyMkVBQkI5Njk2RkNBQzBEMzg3QjU4NDcTHGRlYmlhbkBh
bHRlaG9sei5kZQAKCRCW/KwNOHtYR6bqD/0dZft7znsNCBUr8pPmjJo7Ia89S3WY
kSv2pHDeqXoxTKhghWlcn7PtP5H1nY7WLAXNKrOvUgeNv/oeDXA9btgN4CQPQP/u
2mIttrIUg4JkC02P5Orv1IDtJO05qWTHDmgDsRz3X7hTlfflhjHo3Uhl+ktzSevQ
qRpth40VgDh9HqwPcCv/jWCmGnWpP9pFkGFE4hxd5jHqB8Ql0JDDO4poPNG33Jnf
OVYZGkta/YC85bvSdaJgrETyavxb0WG2ehmHWjwUu4EY7E89db+FRO1zQ409I1Oa
u/KACnY4u0eqYc6b2e953AmWfuwnJOJfwl/Lsbmz9z2idkeI/ZZLCt7cWKKd+8CS
c7F+ygRe7F/SIIQ7009+dW7pwUa1EhZJD+F0WjOUvpfQ5Uo+mzICLE4C+QozcLQz
ZHjbEn/l9rCUe03DYou6ySO6gNx8zqmPlg55eg/kylaOhBJ1KCyBEmbtx/YNfJY6
vihdbyiexOSIRf8YBYWTETKGcSGeC/G/I9xtIqyj2hHvBk2xANeKJZueH4tEkaDs
KrN7WttFF07J31vsC+XRkYDm8Hp3TdBqilbEgIsSLlv5a4MFzvny4a2tMhqOW/nr
WvqYpVep0GTT2DWmrL9lBweAtmU7ckoT4B50rabEyZiNdaqttAi/bjxsbH9YkGlG
xOsrFWLmYOaCFA==
=RwKY
-----END PGP SIGNATURE-----
