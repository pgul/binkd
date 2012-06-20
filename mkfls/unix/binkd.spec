Summary: Binkd - the binkp daemon
Name: binkd
Version: 1.0a.613
Release: 1
License: GPL
Source: %{name}.tar.gz
URL: ftp://happy.kiev.ua/pub/fidosoft/mailer/binkd/
Provides: binkd
Requires: perl >= 5.8.3, zlib >= 1.2.3, bzip2 >= 1.0.3
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Group: Applications/Internet

%description
Binkd is the daemon for FTN communications over reliable links.

%prep
%setup -q -n %{name}
cp -p mkfls/unix/{Makefile*,configure*,install-sh,mkinstalldirs} .

%build
%configure \
        --prefix=%{_prefix} \
        --sysconfdir=%{_sysconfdir} \
        --localstatedir=%{_localstatedir} \
        --with-proxy --with-ntlm --with-bwlim \
        --with-perl --with-zlib --with-bzip2
make

%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} install
rm -rf %{buildroot}%{_sysconfdir}

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%{_prefix}/*

