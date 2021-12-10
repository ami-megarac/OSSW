-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 1.0
Source: gcc-4.9
Binary: gcc-4.9-base, libgcc1, libgcc1-dbg, libgcc2, libgcc2-dbg, libgcc-4.9-dev, libgcc4, libgcc4-dbg, lib64gcc1, lib64gcc1-dbg, lib64gcc-4.9-dev, lib32gcc1, lib32gcc1-dbg, lib32gcc-4.9-dev, libn32gcc1, libn32gcc1-dbg, libn32gcc-4.9-dev, libx32gcc1, libx32gcc1-dbg, libx32gcc-4.9-dev, gcc-4.9, gcc-4.9-multilib, gcc-4.9-plugin-dev, gcc-4.9-hppa64, cpp-4.9, gcc-4.9-locales, g++-4.9, g++-4.9-multilib, libgomp1, libgomp1-dbg, lib32gomp1, lib32gomp1-dbg, lib64gomp1, lib64gomp1-dbg, libn32gomp1, libn32gomp1-dbg, libx32gomp1, libx32gomp1-dbg, libitm1, libitm1-dbg, lib32itm1, lib32itm1-dbg, lib64itm1, lib64itm1-dbg, libx32itm1, libx32itm1-dbg, libatomic1, libatomic1-dbg, lib32atomic1, lib32atomic1-dbg, lib64atomic1, lib64atomic1-dbg, libn32atomic1, libn32atomic1-dbg, libx32atomic1, libx32atomic1-dbg, libasan1, libasan1-dbg, lib32asan1, lib32asan1-dbg, lib64asan1, lib64asan1-dbg, libx32asan1, libx32asan1-dbg, liblsan0, liblsan0-dbg, lib32lsan0, lib32lsan0-dbg, libx32lsan0,
 libx32lsan0-dbg, libtsan0, libtsan0-dbg, libubsan0, libubsan0-dbg, lib32ubsan0, lib32ubsan0-dbg, lib64ubsan0, lib64ubsan0-dbg, libx32ubsan0, libx32ubsan0-dbg, libcilkrts5, libcilkrts5-dbg, lib32cilkrts5, lib32cilkrts5-dbg, lib64cilkrts5, lib64cilkrts5-dbg, libx32cilkrts5, libx32cilkrts5-dbg, libquadmath0, libquadmath0-dbg, lib32quadmath0, lib32quadmath0-dbg, lib64quadmath0, lib64quadmath0-dbg, libx32quadmath0, libx32quadmath0-dbg, gobjc++-4.9, gobjc++-4.9-multilib, gobjc-4.9, gobjc-4.9-multilib, libobjc-4.9-dev, lib64objc-4.9-dev, lib32objc-4.9-dev, libn32objc-4.9-dev, libx32objc-4.9-dev, libobjc4, libobjc4-dbg, lib64objc4, lib64objc4-dbg, lib32objc4, lib32objc4-dbg, libn32objc4, libn32objc4-dbg, libx32objc4, libx32objc4-dbg, gfortran-4.9, gfortran-4.9-multilib, libgfortran-4.9-dev, lib64gfortran-4.9-dev, lib32gfortran-4.9-dev, libn32gfortran-4.9-dev, libx32gfortran-4.9-dev, libgfortran3, libgfortran3-dbg, lib64gfortran3, lib64gfortran3-dbg, lib32gfortran3,
 lib32gfortran3-dbg, libn32gfortran3, libn32gfortran3-dbg, libx32gfortran3, libx32gfortran3-dbg, gccgo-4.9, gccgo-4.9-multilib, libgo5, libgo5-dbg, lib64go5, lib64go5-dbg, lib32go5, lib32go5-dbg, libn32go5, libn32go5-dbg, libx32go5, libx32go5-dbg, gcj-4.9, gcj-4.9-jdk, gcj-4.9-jre-headless, gcj-4.9-jre, libgcj15, gcj-4.9-jre-lib, libgcj15-awt, libgcj15-dev, libgcj15-dbg, gcj-4.9-source, libgcj-doc, libstdc++6, lib32stdc++6, lib64stdc++6, libn32stdc++6, libx32stdc++6, libstdc++-4.9-dev, libstdc++-4.9-pic, libstdc++6-4.9-dbg, lib32stdc++-4.9-dev, lib32stdc++6-4.9-dbg, lib64stdc++-4.9-dev, lib64stdc++6-4.9-dbg, libn32stdc++-4.9-dev, libn32stdc++6-4.9-dbg, libx32stdc++-4.9-dev, libx32stdc++6-4.9-dbg, libstdc++-4.9-doc, gdc-4.9, libphobos-4.9-dev, fixincludes,
 gcc-4.9-source
Architecture: any all
Version: 4.9.2-10+deb8u2
Maintainer: Debian GCC Maintainers <debian-gcc@lists.debian.org>
Uploaders: Matthias Klose <doko@debian.org>
Homepage: http://gcc.gnu.org/
Standards-Version: 3.9.6
Vcs-Browser: http://svn.debian.org/viewsvn/gcccvs/branches/sid/gcc-4.9/
Vcs-Svn: svn://anonscm.debian.org/gcccvs/branches/sid/gcc-4.9
Build-Depends: debhelper (>= 5.0.62), dpkg-dev (>= 1.17.11), g++-multilib [amd64 i386 kfreebsd-amd64 mips mips64 mips64el mipsel mipsn32 mipsn32el powerpc ppc64 s390 s390x sparc sparc64 x32], libc6.1-dev (>= 2.13-5) [alpha ia64] | libc0.3-dev (>= 2.13-5) [hurd-i386] | libc0.1-dev (>= 2.13-5) [kfreebsd-i386 kfreebsd-amd64] | libc6-dev (>= 2.13-5), libc6-dev (>= 2.13-31) [armel armhf], libc6-dev-amd64 [i386 x32], libc6-dev-sparc64 [sparc], libc6-dev-sparc [sparc64], libc6-dev-s390 [s390x], libc6-dev-s390x [s390], libc6-dev-i386 [amd64 x32], libc6-dev-powerpc [ppc64], libc6-dev-ppc64 [powerpc], libc0.1-dev-i386 [kfreebsd-amd64], lib32gcc1 [amd64 ppc64 kfreebsd-amd64 mipsn32 mipsn32el mips64 mips64el s390x sparc64 x32], libn32gcc1 [mips mipsel mips64 mips64el], lib64gcc1 [i386 mips mipsel mipsn32 mipsn32el powerpc sparc s390 x32], libc6-dev-mips64 [mips mipsel mipsn32 mipsn32el], libc6-dev-mipsn32 [mips mipsel mips64 mips64el], libc6-dev-mips32 [mipsn32 mipsn32el mips64 mips64el], libc6-dev-x32 [amd64 i386], libx32gcc1 [amd64 i386], libc6.1-dbg [alpha ia64] | libc0.3-dbg [hurd-i386] | libc0.1-dbg [kfreebsd-i386 kfreebsd-amd64] | libc6-dbg, kfreebsd-kernel-headers (>= 0.84) [kfreebsd-any], m4, libtool, autoconf2.64, libunwind7-dev (>= 0.98.5-6) [ia64], libatomic-ops-dev [ia64], autogen, gawk, lzma, xz-utils, patchutils, zlib1g-dev, systemtap-sdt-dev [linux-any kfreebsd-any hurd-any], binutils (>= 2.25-3~) | binutils-multiarch (>= 2.25-3~), binutils-hppa64 (>= 2.25-3~) [hppa], gperf (>= 3.0.1), bison (>= 1:2.3), flex, gettext, gdb, texinfo (>= 4.3), locales, sharutils, procps, libantlr-java, python, libffi-dev, fastjar, libmagic-dev, libecj-java (>= 3.3.0-2), zip, libasound2-dev [!hurd-any !kfreebsd-any], libxtst-dev, libxt-dev, libgtk2.0-dev (>= 2.4.4-2), libart-2.0-dev, libcairo2-dev, netbase, libcloog-isl-dev (>= 0.18), libmpc-dev (>= 1.0), libmpfr-dev (>= 3.0.0-9~), libgmp-dev (>= 2:5.0.1~), dejagnu [!m68k], realpath (>= 1.9.12), chrpath, lsb-release, quilt
Build-Depends-Indep: doxygen (>= 1.7.2), graphviz (>= 2.2), ghostscript, texlive-latex-base, xsltproc, libxml2-utils, docbook-xsl-ns
Package-List:
 cpp-4.9 deb interpreters optional arch=any
 fixincludes deb devel optional arch=any
 g++-4.9 deb devel optional arch=any
 g++-4.9-multilib deb devel optional arch=amd64,i386,kfreebsd-amd64,mips,mips64,mips64el,mipsel,mipsn32,mipsn32el,powerpc,ppc64,s390,s390x,sparc,sparc64,x32
 gcc-4.9 deb devel optional arch=any
 gcc-4.9-base deb libs required arch=any
 gcc-4.9-hppa64 deb devel optional arch=hppa
 gcc-4.9-locales deb devel optional arch=all
 gcc-4.9-multilib deb devel optional arch=amd64,i386,kfreebsd-amd64,mips,mips64,mips64el,mipsel,mipsn32,mipsn32el,powerpc,ppc64,s390,s390x,sparc,sparc64,x32
 gcc-4.9-plugin-dev deb devel optional arch=any
 gcc-4.9-source deb devel optional arch=all
 gccgo-4.9 deb devel optional arch=any
 gccgo-4.9-multilib deb devel optional arch=amd64,i386,kfreebsd-amd64,mips,mips64,mips64el,mipsel,mipsn32,mipsn32el,powerpc,ppc64,s390,s390x,sparc,sparc64,x32
 gcj-4.9 deb java optional arch=any
 gcj-4.9-jdk deb java optional arch=any
 gcj-4.9-jre deb java optional arch=any
 gcj-4.9-jre-headless deb java optional arch=any
 gcj-4.9-jre-lib deb java optional arch=all
 gcj-4.9-source deb java optional arch=all
 gdc-4.9 deb devel optional arch=any
 gfortran-4.9 deb devel optional arch=any
 gfortran-4.9-multilib deb devel optional arch=amd64,i386,kfreebsd-amd64,mips,mips64,mips64el,mipsel,mipsn32,mipsn32el,powerpc,ppc64,s390,s390x,sparc,sparc64,x32
 gobjc++-4.9 deb devel optional arch=any
 gobjc++-4.9-multilib deb devel optional arch=amd64,i386,kfreebsd-amd64,mips,mips64,mips64el,mipsel,mipsn32,mipsn32el,powerpc,ppc64,s390,s390x,sparc,sparc64,x32
 gobjc-4.9 deb devel optional arch=any
 gobjc-4.9-multilib deb devel optional arch=amd64,i386,kfreebsd-amd64,mips,mips64,mips64el,mipsel,mipsn32,mipsn32el,powerpc,ppc64,s390,s390x,sparc,sparc64,x32
 lib32asan1 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32asan1-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32atomic1 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32atomic1-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32cilkrts5 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32cilkrts5-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32gcc-4.9-dev deb libdevel optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32gcc1 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32gcc1-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32gfortran-4.9-dev deb libdevel optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32gfortran3 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32gfortran3-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32go5 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32go5-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32gomp1 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32gomp1-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32itm1 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32itm1-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32lsan0 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32lsan0-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32objc-4.9-dev deb libdevel optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32objc4 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32objc4-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32quadmath0 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32quadmath0-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32stdc++-4.9-dev deb libdevel optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32stdc++6 deb libs extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32stdc++6-4.9-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32ubsan0 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib32ubsan0-dbg deb debug extra arch=amd64,ppc64,kfreebsd-amd64,s390x,sparc64,x32,mipsn32,mipsn32el,mips64,mips64el
 lib64asan1 deb libs optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64asan1-dbg deb debug extra arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64atomic1 deb libs optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64atomic1-dbg deb debug extra arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64cilkrts5 deb libs optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64cilkrts5-dbg deb debug extra arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64gcc-4.9-dev deb libdevel optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64gcc1 deb libs optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64gcc1-dbg deb debug extra arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64gfortran-4.9-dev deb libdevel optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64gfortran3 deb libs optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64gfortran3-dbg deb debug extra arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64go5 deb libs optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64go5-dbg deb debug extra arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64gomp1 deb libs optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64gomp1-dbg deb debug extra arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64itm1 deb libs optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64itm1-dbg deb debug extra arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64objc-4.9-dev deb libdevel optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64objc4 deb libs optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64objc4-dbg deb debug extra arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64quadmath0 deb libs optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64quadmath0-dbg deb debug extra arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64stdc++-4.9-dev deb libdevel optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64stdc++6 deb libs optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64stdc++6-4.9-dbg deb debug extra arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64ubsan0 deb libs optional arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 lib64ubsan0-dbg deb debug extra arch=i386,powerpc,sparc,s390,mips,mipsel,mipsn32,mipsn32el,x32
 libasan1 deb libs optional arch=any
 libasan1-dbg deb debug extra arch=any
 libatomic1 deb libs optional arch=any
 libatomic1-dbg deb debug extra arch=any
 libcilkrts5 deb libs optional arch=any
 libcilkrts5-dbg deb debug extra arch=any
 libgcc-4.9-dev deb libdevel optional arch=any
 libgcc1 deb libs required arch=any
 libgcc1-dbg deb debug extra arch=any
 libgcc2 deb libs required arch=m68k
 libgcc2-dbg deb debug extra arch=m68k
 libgcc4 deb libs required arch=hppa
 libgcc4-dbg deb debug extra arch=hppa
 libgcj-doc deb doc optional arch=all
 libgcj15 deb libs optional arch=any
 libgcj15-awt deb libs optional arch=any
 libgcj15-dbg deb debug extra arch=any
 libgcj15-dev deb libdevel optional arch=any
 libgfortran-4.9-dev deb libdevel optional arch=any
 libgfortran3 deb libs optional arch=any
 libgfortran3-dbg deb debug extra arch=any
 libgo5 deb libs optional arch=any
 libgo5-dbg deb debug extra arch=any
 libgomp1 deb libs optional arch=any
 libgomp1-dbg deb debug extra arch=any
 libitm1 deb libs optional arch=any
 libitm1-dbg deb debug extra arch=any
 liblsan0 deb libs optional arch=any
 liblsan0-dbg deb debug extra arch=any
 libn32atomic1 deb libs optional arch=mips,mipsel,mips64,mips64el
 libn32atomic1-dbg deb debug extra arch=mips,mipsel,mips64,mips64el
 libn32gcc-4.9-dev deb libdevel optional arch=mips,mipsel,mips64,mips64el
 libn32gcc1 deb libs optional arch=mips,mipsel,mips64,mips64el
 libn32gcc1-dbg deb debug extra arch=mips,mipsel,mips64,mips64el
 libn32gfortran-4.9-dev deb libdevel optional arch=mips,mipsel,mips64,mips64el
 libn32gfortran3 deb libs optional arch=mips,mipsel,mips64,mips64el
 libn32gfortran3-dbg deb debug extra arch=mips,mipsel,mips64,mips64el
 libn32go5 deb libs optional arch=mips,mipsel,mips64,mips64el
 libn32go5-dbg deb debug extra arch=mips,mipsel,mips64,mips64el
 libn32gomp1 deb libs optional arch=mips,mipsel,mips64,mips64el
 libn32gomp1-dbg deb debug extra arch=mips,mipsel,mips64,mips64el
 libn32objc-4.9-dev deb libdevel optional arch=mips,mipsel,mips64,mips64el
 libn32objc4 deb libs optional arch=mips,mipsel,mips64,mips64el
 libn32objc4-dbg deb debug extra arch=mips,mipsel,mips64,mips64el
 libn32stdc++-4.9-dev deb libdevel optional arch=mips,mipsel,mips64,mips64el
 libn32stdc++6 deb libs optional arch=mips,mipsel,mips64,mips64el
 libn32stdc++6-4.9-dbg deb debug extra arch=mips,mipsel,mips64,mips64el
 libobjc-4.9-dev deb libdevel optional arch=any
 libobjc4 deb libs optional arch=any
 libobjc4-dbg deb debug extra arch=any
 libphobos-4.9-dev deb libdevel optional arch=amd64,armel,armhf,i386,x32,kfreebsd-amd64,kfreebsd-i386
 libquadmath0 deb libs optional arch=any
 libquadmath0-dbg deb debug extra arch=any
 libstdc++-4.9-dev deb libdevel optional arch=any
 libstdc++-4.9-doc deb doc optional arch=all
 libstdc++-4.9-pic deb libdevel extra arch=any
 libstdc++6 deb libs important arch=any
 libstdc++6-4.9-dbg deb debug extra arch=any
 libtsan0 deb libs optional arch=any
 libtsan0-dbg deb debug extra arch=any
 libubsan0 deb libs optional arch=any
 libubsan0-dbg deb debug extra arch=any
 libx32asan1 deb libs optional arch=amd64,i386
 libx32asan1-dbg deb debug extra arch=amd64,i386
 libx32atomic1 deb libs optional arch=amd64,i386
 libx32atomic1-dbg deb debug extra arch=amd64,i386
 libx32cilkrts5 deb libs optional arch=amd64,i386
 libx32cilkrts5-dbg deb debug extra arch=amd64,i386
 libx32gcc-4.9-dev deb libdevel optional arch=amd64,i386
 libx32gcc1 deb libs optional arch=amd64,i386
 libx32gcc1-dbg deb debug extra arch=amd64,i386
 libx32gfortran-4.9-dev deb libdevel optional arch=amd64,i386
 libx32gfortran3 deb libs optional arch=amd64,i386
 libx32gfortran3-dbg deb debug extra arch=amd64,i386
 libx32go5 deb libs optional arch=amd64,i386
 libx32go5-dbg deb debug extra arch=amd64,i386
 libx32gomp1 deb libs optional arch=amd64,i386
 libx32gomp1-dbg deb debug extra arch=amd64,i386
 libx32itm1 deb libs optional arch=amd64,i386
 libx32itm1-dbg deb debug extra arch=amd64,i386
 libx32lsan0 deb libs optional arch=amd64,i386
 libx32lsan0-dbg deb debug extra arch=amd64,i386
 libx32objc-4.9-dev deb libdevel optional arch=amd64,i386
 libx32objc4 deb libs optional arch=amd64,i386
 libx32objc4-dbg deb debug extra arch=amd64,i386
 libx32quadmath0 deb libs optional arch=amd64,i386
 libx32quadmath0-dbg deb debug extra arch=amd64,i386
 libx32stdc++-4.9-dev deb libdevel optional arch=amd64,i386
 libx32stdc++6 deb libs optional arch=amd64,i386
 libx32stdc++6-4.9-dbg deb debug extra arch=amd64,i386
 libx32ubsan0 deb libs optional arch=amd64,i386
 libx32ubsan0-dbg deb debug extra arch=amd64,i386
Checksums-Sha1:
 702e8d3fc681fed9ef11b825b88c7bfbf47ce42e 73565212 gcc-4.9_4.9.2.orig.tar.gz
 be7606848ca75d794f5c7ea53c9f8bcc52191d07 883134 gcc-4.9_4.9.2-10+deb8u2.diff.gz
Checksums-Sha256:
 861aa811d5f9e9ecf32d8195d2346fc434eba7e17330878ed3d876c49a32ec4e 73565212 gcc-4.9_4.9.2.orig.tar.gz
 bd151e4ec61e40a96a49dc4a8275da688380ffd9668670b3cfe78c622e3812ae 883134 gcc-4.9_4.9.2-10+deb8u2.diff.gz
Files:
 a6cb2e9cd59fda1d05ea1d07b29ba5d9 73565212 gcc-4.9_4.9.2.orig.tar.gz
 0c30853f587bdc2d4bc9e53d23ca2b17 883134 gcc-4.9_4.9.2-10+deb8u2.diff.gz

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCAAdFiEEcJymx+vmJZxd92Q+nUbEiOQ2gwIFAlwTXUMACgkQnUbEiOQ2
gwIhzg//cCtEA0VZ1mZVlrBCsbpEnIW9I+ppavm2BjyQ0Gtn3afTAkZ+hD8Nmw65
gcrdk6oUDDZLsUsKcSu+We30S++J8Z/LSsFktuH60/ITjvdFFnqNWq0GpDmAPA+E
F2mMviZ4fAiaAO5S5zTZdyIxFsACco1VyHzG1/8sLlZBndjAKyBoYef3sBwPej9d
TDjdVj50uhXNcpWZe16bpWnfIoQ85CXAGQ8U6WIsM6I1Q0qk3PPYediqIB0iMuK0
sMrMTQdLHAaqE7+I92l2i2NUEOMDBu6P3FITKGpoPF7jSwptg3ODGHLMjwJQZvO6
CbwL2WfpXjzgRBTifYPEPYeJmGZDHehiNA8a3wb8CmdsVRiBJdbgOPuNcqnMfsv/
pTFs88Z7O963O5o41ygCRA8VB7tcUKU34pJjYYmyNFZoJGowCjcN+yCkuglayAN0
e/lsUD1xh9NjTedIXa0uWQxeMDNWsuI69aSJ2grEr2nu/tKLMjAJsZyqZTeyW02K
46lZNTQAawocva507Aa+K4v2Dzlke190gGUzlCUxVoh8jEllEzOttmg2zDoiky/d
Cel8gcKr++xsEsNKYrza2qHqnnRLMNypBFnx9Vftb++KpVSup0fbfc7v5mmnnYT1
ziP++8575kQdap0nwjZvBJQJ40ZiSVm+s5adkzLeCp18aeb9i9E=
=ezDE
-----END PGP SIGNATURE-----
