#
# Perl nodelist compiler for binkd ver 0.3
# Copyright (C) Pavel Gulchouck 2:463/68  2009-2013
#
# Add folowing lines to your binkd.cfg (with correct pathes):
#
#perl-hooks /etc/fido/nodelist.pl
#perl-var nodelist fidonet:/home/fnet/nodelist/nodelist.[0-9][0-9][0-9]
#perl-var magichost *
#perl-dll perl58.dll # only for win32 and os/2 versions with runtime DLL load
#
# and then "*" in hosts list will be substituted to INA value from nodelist.
# If set magichost to another value (i.e. '!' or '+'), you can preserve
# traditional meaning of '*'.
# Note that strictIP (-ip or -sip flags) will not check by nodelist info.
#
# Binkd version must be 1.0a-534 or later and compiled with perl support
#
# Parse flags IBN, INA, DOM, IRD; fetch information from phone and from 
# system name if any.
#

#     ToDo:
# - many nodelist files and domains

#     History:
# ver 0.1 (28 May 2009): first
# ver 0.2 ( 2 Jun 2009): avoid using glob() - File::Glob module needed, 
#         and no all systems have fully installed perl
# ver 0.3 (19 Jan 2012): support multiple IBN flags (ports)

my ($curnodelist, %nodelist, $glob);

sub nodelist  { return ($config{"nodelist"} =~ /:/ ? $' : ""); }
sub domain    { return ($config{"nodelist"} =~ /:/ ? $` : ""); }
sub magichost { return ($config{"magichost"} || "*"); }

sub find_nodelist
{
	my (@listdir);

	if (!defined($glob)) {
		$diehandler = $SIG{"__DIE__"};
		$SIG{"__DIE__"} = sub {};
		eval "use File::Glob;";
		$SIG{"__DIE__"} = $diehandler;
		if ($@) {
			Log(2, "No Autoloader in perl");
			$glob = 0;
		} else {
			$glob = 1;
		}
	}
	if ($glob) {
		eval "\@listdir = glob(nodelist());";
	} else {
		my ($dir, $name);
		if (nodelist() =~ m@[/\\]([^/\\]+)$@) {
			($dir, $name) = ($`, $1);
		} else {
			return undef;
		}
		$name =~ s/\\/\\\\/g;	# no chance
		$name =~ s/\./\\./g;
		$name =~ s/\?/./g;
		$name =~ s/\*/.*/g;
		opendir(DIR, $dir) || return undef;
		@listdir = ();
		while (($_=readdir(DIR))) {
			push(@listdir, "$dir/$_") if /^$name$/i;
		}
		closedir(DIR);
	}
	foreach (sort { $b cmp $a } @listdir) {
		return $_ if -r $_;
	}
	return undef;
}

sub need_reload
{
	my ($nlist) = find_nodelist();
	if ($nlist && $nlist ne $curnodelist) {
		$curnodelist = $nlist;
		return 1;
	}
	return undef;
}

sub config_loaded
{
	my ($nlist) = find_nodelist();
	my ($magichost) = magichost();
	my ($subst);

	if ($nlist ne $curnodelist || !%nodelist) {
		compile_nodelist($curnodelist = $nlist);
	}
	foreach (keys %node) {
		if ($node{$_}->{"hosts"} =~ /(^|;)$magichost(;|$)/ && $node{$_}->{"IP"}) {
			Log(1, "WARNING: restrict IP will not work by nodelist for $_!");
		}
	}
}

sub on_call
{
	my ($subst);
	$hosts = $subst if defined($subst = subst($addr, $hosts));
	return 1;
}

sub subst
{
	my ($addr, $hosts) = @_;
	my (@hosts, %hosts, $n);

	foreach (split(/;/, $hosts)) {
		if ($_ eq magichost()) {
			if ($nodelist{$addr}) {
				Log(4, "Substituted $_ to $nodelist{$addr} for $addr by nodelist");
				$_ = $nodelist{$addr};
			} elsif ($_ ne "*") {
				next;
			}
		}
		$n = $_;
		s/\.$//;
		next if $hosts{$_};
		$hosts{$_} = 1;
		push(@hosts, $n);
	}
	$n = join(';', @hosts);
	$n = "" if !defined($n);
	return ($n eq $hosts ? undef : $n);
}

sub compile_nodelist
{
	my ($nlist) = @_;
	my ($zone, $net, $node, $dom, $ird, $domain, $start);
	my ($line, $keyword, $name, $phone, $flags, $port, $lport, $nodes);
	my (%flags, $uflag, %addr, @addr, $domzone, $domreg, $domnet);

	%nodelist = ();
	unless ($nlist) {
		Log(1, "No nodelist found!");
		return;
	}
	unless (open (F, "<$nlist")) {
		Log(1, "Cannot read nodelist $nlist: $!");
		return;
	}
	Log(6, "Parsing nodelist file $nlist");
	$start = time();
	$zone = $net = $node = 0;
	$domzone = $domreg = $domnet = $ird = "";
	$domain = domain();
	$nodes = 0;
	while (defined($line = <F>)) {
		$line =~ s/\r?\n$//s;
		next unless $line =~ /^([a-z]*),(\d+),([^,]*),[^,]*,[^,]*,([^,]*),\d+(?:,(.*))?\s*$/i;
		($keyword, $node, $name, $phone, $flags) = ($1, $2, $3, $4, $5);
		$uflag = "";
		%flags = ();
		%addr = ();
		@addr = ();
		foreach (split(/,/, $flags)) {
			if (/^U/) {
				$uflag = "U";
				next if /^U$/;
			} else {
				$_ = "$uflag$_";
			}
			if (/:/) {
				$flags{$`} .= "," if defined($flags{$`});
				$flags{$`} .= $';
			} else {
				$flags{$_} .= "," if defined($flags{$_});
				$flags{$_} .= "";
			}
		}
		next if $keyword eq "Down";
		next if $keyword eq "Hold";
		if ($keyword eq "Zone") {
			$zone = $region = $net = $node;
			$node = 0;
			$domzone = $domreg = $domnet = "";
			foreach $i (qw(M 1 2 3 4)) {
				$domzone = $domreg = $domnet = "DO$i:" . $flags{"UDO$i"} if $flags{"UDO$i"};
			}
			$ird = $flags{"IRD"};
		} elsif ($keyword eq "Region") {
			$region = $net = $node;
			$node = 0;
			$domreg = $domnet = "";
			foreach $i (qw(M 1 2 3 4)) {
				$domreg = $domnet = "DO$i:" . $flags{"UDO$i"} if $flags{"UDO$i"};
			}
			$ird = $flags{"IRD"};
		} elsif ($keyword eq "Host") {
			$net = $node;
			$node = 0;
			$domnet = "";
			foreach $i (qw(M 1 2 3 4)) {
				$domnet = "DO$i:" . $flags{"UDO$i"} if $flags{"UDO$i"};
			}
			$ird = $flags{"IRD"};
		}
		next unless defined($flags{"IBN"});
		%port = ();
		foreach (split(/,/, $flags{"IBN"})) {
			if (/^\d*$/) {
				$port{/\d/ ? ":$_" : ""} = 1;
				next;
			}
			$lport = "";
			($_, $lport) = ($`, ":$'") if /:/;
			$_ .= "." unless /^\d+\.\d+\.\d+\.\d+$|\.$/;
			%lport = ($lport ? ( ":$lport" => 1 ) : %port);
			$lport{""} = 1 unless %lport;
			foreach $lport (keys %lport) {
				next if $addr{"$_$lport"};
				$addr{"$_$lport"} = 1;
				push(@addr, "$_$lport");
			}
		}
		if (@addr) {
			$nodelist{"$zone:$net/$node\@$domain"} = join(';', @addr);
			$nodes++;
			Log(8, "Fetch addr for $zone:$net/$node\@$domain: " . $nodelist{"$zone:$net/$node\@$domain"} . " (IBN flag)");
			next;
		}
		$port{""} = 1 unless %port;
		if ($_ = $flags{"INA"}) {
			foreach (split(/,/, $flags{"INA"})) {
				$_ .= "." unless /^\d+\.\d+\.\d+\.\d+$|\.$/;
				foreach $port (keys %port) {
					next if $addr{"$_$port"};
					$addr{"$_$port"} = 1;
					push(@addr, "$_$port");
				}
			}
			$nodelist{"$zone:$net/$node\@$domain"} = join(';', @addr);
			$nodes++;
			Log(8, "Fetch addr for $zone:$net/$node\@$domain: " . $nodelist{"$zone:$net/$node\@$domain"} . " (INA flag)");
			next;
		}
		if ($phone =~ /000-([1-9]\d*)-(\d+)-(\d+)-(\d+)$/) {
			$addr{"$1.$2.$3.$4"} = 1;
			push(@addr, "$1.$2.$3.$4");
			Log(8, "Fetch addr for $zone:$net/$node\@$domain: $1.$2.$3.$4 (phone)");
		}
		if ($name =~ /^(\d+\.\d+\.\d+\.\d+|[a-z0-9][-a-z0-9.]*\.(net|org|com|biz|info|name|[a-z][a-z]))$/) {
			$name .= "." if $name =~ /[a-z]/;
			unless ($addr{$name}) {
				$addr{$name} = 1;
				push(@addr, $name);
				Log(8, "Fetch addr for $zone:$net/$node\@$domain: $name (system name)");
			}
		}
		unless (@addr) {
			$domflag = ($domnet || $domreg || $domzone);
			foreach $i (qw(M 1 2 3 4)) {
				$domflag = "DO$i:" . $flags{"UDO$i"} if $flags{"UDO$i"};
			}
			if ($domflag =~ /^DO(.):/) {
				($i, $dom) = ($1, $');
				if ($i eq 'M') {
					$_ = "f$node.n$net.z$zone.$domain.$dom.";
				} elsif ($i eq '4') {
					$_ = "f$node.n$net.z$zone.$dom.";
				} elsif ($i eq '3') {
					$_ = "f$node.n$net.$dom.";
				} elsif ($i eq '2') {
					$_ = "f$node.$dom.";
				} elsif ($i eq '1') {
					$_ = "$dom.";
				}
				unless ($addr{$_}) {
					$addr{$_} = 1;
					push(@addr, $_);
					Log(8, "Fetch addr for $zone:$net/$node\@$domain: $_ (DO$i flag)");
				}
			}
			if ($ird) {
				$_ = "f$node.n$net.z$zone.$ird.";
				unless ($addr{$_}) {
					$addr{$_} = 1;
					push(@addr, $_);
					Log(8, "Fetch addr for $zone:$net/$node\@$domain: $_ (IRD flag)");
				}
			}
		}
		next unless @addr;
		%addr = ();
		foreach $addr (@addr) {
			foreach $port (keys %port) {
				$addr{"$addr$port"} = 1;
			}
		}
		$_ .= $port foreach @addr;
		$nodelist{"$zone:$net/$node\@$domain"} = join(';', keys %addr);
		$nodes++;
	}
	close(F);
	Log(3, "Nodelist $nlist parsed, $nodes IP-nodes processed (" . (time() - $start) . " sec)");
}

