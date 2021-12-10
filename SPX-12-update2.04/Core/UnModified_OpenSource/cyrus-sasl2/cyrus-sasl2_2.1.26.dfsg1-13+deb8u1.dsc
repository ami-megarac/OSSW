-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: cyrus-sasl2
Binary: sasl2-bin, cyrus-sasl2-doc, libsasl2-2, libsasl2-modules, libsasl2-modules-db, libsasl2-modules-ldap, libsasl2-modules-otp, libsasl2-modules-sql, libsasl2-modules-gssapi-mit, libsasl2-dev, libsasl2-modules-gssapi-heimdal, cyrus-sasl2-dbg, cyrus-sasl2-mit-dbg, cyrus-sasl2-heimdal-dbg
Architecture: any all
Version: 2.1.26.dfsg1-13+deb8u1
Maintainer: Debian Cyrus SASL Team <pkg-cyrus-sasl2-debian-devel@lists.alioth.debian.org>
Uploaders: Fabian Fagerholm <fabbe@debian.org>, Roberto C. Sanchez <roberto@connexer.com>, Ondřej Surý <ondrej@debian.org>, Adam Conrad <adconrad@0c3.net>
Homepage: http://www.cyrusimap.org/
Standards-Version: 3.9.5
Vcs-Browser: http://anonscm.debian.org/gitweb/?p=pkg-cyrus-sasl2/cyrus-sasl2.git
Vcs-Git: git://anonscm.debian.org/pkg-cyrus-sasl2/cyrus-sasl2.git
Build-Depends: debhelper (>= 9), quilt, automake (>= 1:1.14), autotools-dev, libdb-dev, libpam0g-dev, libssl-dev, libmysqlclient-dev | libmysqlclient15-dev, libpq-dev, heimdal-multidev, krb5-multidev, libkrb5-dev, libsqlite3-dev, libldap2-dev, chrpath, groff-base, po-debconf, docbook-to-man, dh-autoreconf
Build-Conflicts: heimdal-dev
Package-List:
 cyrus-sasl2-dbg deb debug extra arch=any
 cyrus-sasl2-doc deb doc optional arch=all
 cyrus-sasl2-heimdal-dbg deb debug extra arch=any
 cyrus-sasl2-mit-dbg deb debug extra arch=any
 libsasl2-2 deb libs standard arch=any
 libsasl2-dev deb libdevel optional arch=any
 libsasl2-modules deb libs optional arch=any
 libsasl2-modules-db deb libs standard arch=any
 libsasl2-modules-gssapi-heimdal deb libs extra arch=any
 libsasl2-modules-gssapi-mit deb libs extra arch=any
 libsasl2-modules-ldap deb libs extra arch=any
 libsasl2-modules-otp deb libs extra arch=any
 libsasl2-modules-sql deb libs extra arch=any
 sasl2-bin deb utils optional arch=any
Checksums-Sha1:
 2e2e96dfb788974d7ab335f39bde526bc71815e5 1494337 cyrus-sasl2_2.1.26.dfsg1.orig.tar.gz
 f4729c228877e10b8c4502339f3a0e02043a929a 94284 cyrus-sasl2_2.1.26.dfsg1-13+deb8u1.debian.tar.xz
Checksums-Sha256:
 172c39555012f479543ce7305949db75df708771fe8f8b34248027f09e53bb85 1494337 cyrus-sasl2_2.1.26.dfsg1.orig.tar.gz
 14e00798c41b6fae965211f1af8b47a67d22001146d0019f81af0fc7be9f162f 94284 cyrus-sasl2_2.1.26.dfsg1-13+deb8u1.debian.tar.xz
Files:
 45fc09469ca059df56d64acfe06a940d 1494337 cyrus-sasl2_2.1.26.dfsg1.orig.tar.gz
 1eb0997e4dcaa0c331e38d50f138defe 94284 cyrus-sasl2_2.1.26.dfsg1-13+deb8u1.debian.tar.xz

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v2

iQJ8BAEBCgBmBQJWBQwbXxSAAAAAAC4AKGlzc3Vlci1mcHJAbm90YXRpb25zLm9w
ZW5wZ3AuZmlmdGhob3JzZW1hbi5uZXQzMEI5MzNEODBGQ0UzRDk4MUEyRDM4RkIw
Qzk5QjcwRUY0RkNCQjA3AAoJEAyZtw70/LsH760QAN0v+oKjAt+pu37l6SfPvF2y
BkKbuFQZ7gjK/plfJThGNNYMWOmTZWwzrtvUXYMSO2qsRvfC69rrpB2zJT8p5a0t
x9tb66rRpTIrzOUTIvTPD1rTBcuUqbZN51PNmZmDA54cWj6PkrqDpFA7eDlDqZWh
ejivTYlTtOV0bF7Ddqtx1ZcCAbRLoelB2WrqINKIms1QrhqI9OyTcVPL8qXNHoVj
bDgNnPmnrVwdAdW/xBYf76+CUG7HRDRmJLmqmKDzTTptp3/8JxVL6AaKzGZ0lerh
w0K8+3GOoNGq8A9rZD96O+jEJN3MnwGWrEP8O6IzNIB8EaEOUdKhh3LrntcdMeYn
G7jffc9hYS4PY0e481Tg15D1WOhII2fwl/c+FPdTH7PyaGLdZhuLdIJ6hMLUV9Xa
pRS9fkSL7oDDJS8cyhPsI1B3kvn6rndneL9rUGmFz8FnR/2JrqwXlZwW5KtTzi4u
b7EqhJv7iK7Bg3jTt6jXRyVYj4swTgm9OrLo9R480DWtRQ7UgCXwoZ6d+hnw5eMv
NioKVdLSQ4poV2OJtJd+VQj1Y2VcvGNuqIfBJi95n7mwkoo7LyPm3BB68/RsgiJ+
nxKi0b4hFDKgbAhwC1Z0DvZVXyXVRPYUKGCaNlDjVP7LET7fyAzeoJQDj6OMUUbD
Q/mF1yYCeyh6zQ+QLcLN
=AZbY
-----END PGP SIGNATURE-----
