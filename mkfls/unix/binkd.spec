Summary: Binkd - a binkp daemon
Name: binkd
Version: 1.1a.112
Release: 2
License: GPL
Source: %name.tar.gz
URL: ftp://happy.kiev.ua/pub/fidosoft/mailer/binkd/
Provides: binkd
BuildRequires: perl-devel >= 5.8.3
%if "%_vendor" == "redhat"
BuildRequires: perl(ExtUtils::Embed)
%endif
BuildRequires: zlib-devel >= 1.2.3
BuildRequires: bzip2-devel >= 1.0.3
%if "%_vendor" == "alt"
Group: Networking/FTN
%endif

%description
Binkd is a daemon for FTN communications over reliable links.

%package doc
BuildArch: noarch
Summary: Sample config and FAQ for %name

%description doc
%summary

%prep
%setup -q -n %name
cp -p mkfls/unix/{Makefile*,configure*,install-sh,mkinstalldirs} .

%build
%configure \
        --prefix=%_prefix \
        --sysconfdir=%_datarootdir/doc/%name \
        --with-proxy --with-ntlm --with-bwlim \
        --with-perl --with-zlib --with-bzip2
%make_build

%install
%make_install

%files
%defattr(-,root,root)
%_sbindir/*
%_mandir/man8/*

%files doc
%_docdir/%name/%name.conf-dist
%_docdir/%name/*.txt
