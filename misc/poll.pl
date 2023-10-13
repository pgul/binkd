#!/usr/bin/perl
# Author is Stas Mishchenkov 2:460/58.
# Generates polls in BSO
#
use strict;
use warnings;

use Getopt::Long;
use File::Spec::Functions;
use File::Path qw(make_path remove_tree);
use LWP::Simple;
use Cwd 'abs_path';
use Fcntl;

my $vers = 'v.0.0.0.3';

my ( $logfile, $pollnodes, $config, $fromfile, %zonedom,
     $check_updates, $defaultzone, $defaultdomain, $outbound, $needhelp,
     $printver, $whatsnew, $pause, $exportconf, $interval );

my $arg = join( ' ', @ARGV );

GetOptions ( "poll-nodes=s"      => \$pollnodes,
             "config=s"          => \$config,
             "from-file=s"       => \$fromfile,
             "update=s"          => \$check_updates,
             "outbound=s"        => \$outbound,     # string
             "zone-default=i"    => \$defaultzone,  # integer
             "help"              => \$needhelp,
             "export"            => \$exportconf,
             "ver"               => \$printver,
             "interval=i"        => \$interval,
             "whatsnew"          => \$whatsnew,
             "log-file=s"        => \$logfile )     # string
or die("Error in command line arguments\n");

sub usage()
{
	print(
		"Usage: $0 options\n".
		"~~~~~~\n".
		"were options are:\n".
		"                     -c filename\n".
		"                    --config=filename  config filename. May be omitted.\n".
		"                     -e\n".
		"                    --export   Export to STDOUT an example configuration file.\n".
		"                     -f filename\n".
		"                    --from-file=filename with space, tab or carriage return\n".
		"                               separated list of nodes to poll. May be omitted.\n".
		"                     -l filename\n".
		"                    --log-file=filename log file name. If omitted no logfile\n".
		"                               will be created.\n".
		"                     -o path\n".
		"                    --outbound=path path to youre outbound. May be omitted.\n".
		"                               Default \'/home/fido/outbound/fidonet\'.\n".
		"                     -p \"list of node adress\"\n".
		"                    --poll-nodes=\"list of node adress\" if more then one address\n".
		"                               listed it must be in a quters. May be omitted.\n".
		"                     -z zone\n".
		"                    --zone-default=zone youre zone number. May be omitted.\n".
		"                               Default 2.\n".
		"                     -i=sec\n".
		"                    --interval sec Interval in seconds between poll creation.\n".
		"                                   Optional. Default 0.\n".
		"                     -u option\n".
		"                    --update=option How to update the program. Optional.\n".
                "                                  =d - download. Check for a new version and\n".
                "                                       download the update to a new file.\n".
                "                                  =f - Force download poll.pl end exit even\n".
                "                                       if no new version is found.\n".
                "                                  =w - warn. Check for a new version and warn\n".
                "                                       the sysop. Default.\n".
                "                                  =n - no. Do nothing.\n".
                "                     -V\n".
                "                    --ver      show version and exit.\n".
                "                     -w\n".
                "                    --whatsnew show whatsnew.\n".
		"   You can mix any options. At least one node addres to poll must be specified\n in any way. You can mix ways to specify addresses to poll.\n"
	);
	exit;
}

sub exportcfg()
{
print <<CONFEND;
#-----------------------------------------------------------------------------
# Example of config file for poll.pl
#-----------------------------------------------------------------------------
#
# Default Zone is Your zone number.
# Optional. Default is "2".
DefaultZone 2

# Youre domain name. Optional. Default is "fidonet".
DefaultDomain fidonet

# Domain names for other zones. May be a few. Optional.
Zone  10 league10
Zone  17 tormoznet
Zone  21 fsxnet
Zone  25 metronet
Zone  32 gamenet
Zone  39 amiganet
Zone  46 agorenet
Zone  75 bbsnet
Zone 115 pascal-net
Zone 111 stnnet
Zone 169 battle
Zone 618 micronet
Zone 700 spooknet
Zone 888 rsnet
Zone 954 hobbynet

# path to outbound
outbound /home/fido/outbound

# file name with path
logfile /home/fido/logs/poll.log

# address to generate a poll. may be a few. Optional.
#node 2:5020/545.0
#node 4:4/0

# How to update.
#   Auto      Check for a new version, download and update.
#   Download  Check for a new version and download the update to a new file.
#   Force     Download poll.pl end exit even if no new version is found.
#   Warn      Check for a new version and warn the sysop. Default.
#   No        Do nothing.
update warn

#-----------------------------------------------------------------------------
CONFEND

exit;
}

sub readconf()
{
	my ( $line, $CF );

	unless( open( $CF, "<", $config ) ) {
	    printf( STDERR "Cannot open $config: $!\r");
	    return '';
	}

	while ( defined( $line = <$CF>) ) {
		next if $line =~ /^[ 	]*\#.*/;
		$line =~ s/\r//g;
		$line =~ s/\n//g;
		$line =~ s/	/ /g;

		if ( $line =~ /^[ ]*DefaultZone[ ]+(\d+)/i ) {
			$defaultzone = $1 unless defined $defaultzone;
		} elsif( $line =~ /^[ ]*outbound[ ]+([^ \r\n]+)/i ) {
			$outbound = $1 unless defined $outbound;
		} elsif( $line =~ /^[ ]*logfile[ ]+([^ \r\n]+)/i ){
			$logfile = $1 unless defined $logfile;
		} elsif( $line =~ /^[ ]*node[ ]+([^ \r\n]+)/i ){
			$pollnodes .= " $1";
		} elsif( $line =~ /^[ ]*zone[ ]+([^ \r\n]+)[ ]+([^ \r\n]+)/i ){
			$zonedom{$1} = $2;
		} elsif( $line =~ /^[ ]*defaultdomain[ ]+([^ \r\n]+)/i ){
			$defaultdomain = $1;
		} elsif( $line =~ /^[ ]*update[ ]+([nwdaf]{1})/i ){
			$check_updates = lc($1) unless defined $check_updates;
		}
	}
	close($CF);
}

sub readfromfile()
{
	my ( $fb, $f, $F );
	if ( open( $F, "<", $fromfile) ) {
		read( $F, $fb, -s $fromfile );
		close($F);
	}
#	$fb =~ s/[^\d\:\/\.]/ /sg;
	$f = '';
	while ( $fb =~ /(\d+\:\d+\/\d+\.?\d*)/ ) {
	    $f .= " $1";
	    $fb =~ s/(\d+\:\d+\/\d+\.?\d*)//;
	}
#	$fb =~ s/\r/ /sg;
#	$fb =~ s/\n/ /sg;
#	$fb =~ s/	/ /sg;
	$f =~ s/^ //;
	$f =~ s/ $//sg;
return $f;
}

sub dolog($)
{
    my ( $logstr ) = @_;
    return if !defined ( $logfile );
    my @ltime = localtime();
    my $logtime = sprintf( "%04d-%02d-%02d %02d:%02d:%02d", 1900+$ltime[5], 1+$ltime[4], $ltime[3], $ltime[2], $ltime[1], $ltime[0] );

    if( open( my $LF, ">>", $logfile ) ) {
	print( $LF "$logtime $logstr\n" );
	close( $LF );
    } else { print STDERR "Cant open \'$logfile\'. ($!)\n"; }
}

#$outbound =~ /([\\\/])/;
#my $slash = $1;

sub getbsoname($)
{
	my ( $address ) = @_;
	my ( $bsoname );

	$address =~ /(\d+)\:(\d+)\/(\d+)\.?(\d*)/;
	my ( $destzone, $destnet, $destnode, $destpnt ) = ( $1, $2, $3, $4 );
	$destpnt = 0 if !defined( $destpnt ) || $destpnt eq "";
	if ( $destzone == $defaultzone ) {
	    $bsoname = catfile( $outbound, $defaultdomain );
	    unless( -e $bsoname ) {
		print STDERR "Can't create $bsoname ($!).\r" unless make_path( $bsoname );
	    }
	} elsif ( defined($zonedom{$destzone}) ) {
	    $bsoname = catfile( $outbound, $zonedom{$destzone} );
	    unless( -e $bsoname ) {
		print STDERR "Can't create $bsoname ($!).\r" unless make_path( $bsoname );
	    }
	} else {
	    $bsoname = catfile( $outbound, sprintf( "$defaultdomain.%03x", $destzone ) );
	    unless( -e $bsoname ) {
		print STDERR "Can't create $bsoname ($!).\r" unless make_path( $bsoname );
	    }
	}
	$bsoname = catfile( $bsoname, sprintf("%04x%04x", $destnet, $destnode) );
	if ( defined ( $destpnt ) && $destpnt != 0 ) {
	    $bsoname .= '.pnt';
	    unless( -e $bsoname ) {
		print STDERR "Can't create $bsoname ($!).\r" unless make_path( $bsoname );
	    }
	    $bsoname = catfile( $bsoname, sprintf( "%08x", $destpnt ) );
	}

return $bsoname;
}

my $url = 'https://brorabbit.g0x.ru/files/perl/';
sub update()
{
#    return if $check_updates eq 'n';
    return unless $check_updates =~ /^[wdf]$/i;

    my ( $ver_s, $upd, $of );
#    my $url = 'http://brorabbit.g0x.ru/files/perl/';

    my $curpath = abs_path($0);
    $curpath =~ /([^\/\\]+)\.pl$/i;
    my $p_name = $1;
#    $curpath = Cwd::realpath($0) unless defined $curpath;
#    $curpath = Cwd::realpath('./') unless defined $curpath;

    $ver_s = get( "$url${p_name}\.v");
    if (defined ($ver_s) ) {
    if ( $check_updates eq 'f' ) {
	if ( $curpath =~ /^(.*?)\.pl$/ ) {
	    $of = "$1_$ver_s\.pl";
	} elsif ( $curpath =~ /^(.*?[\/\\])[^\/\\]+$/ ) {
	    $of = $1 . "${p_name}_${ver_s}\.pl";
	} else {
	    $of = "${curpath}_${ver_s}\.pl";
	}
	print "Latest version is $ver_s\!\n";
	dolog("Latest version is $ver_s\! Downloaded filename is \'$of\'.");
    } elsif ( $vers lt $ver_s ) {
        if ( $check_updates eq 'w' ) {
	    print " \*\*\* You should update to $ver_s\! \*\*\* \n";
	    dolog(" \*\*\* You should update to $ver_s\! \*\*\* ");
	    return;
        } elsif ( $check_updates eq 'd' || $check_updates eq 'f' ) {
#		$curpath =~ /^(.*?)\.pl$/;
#		$of = "$1_$ver_s\.pl";
	    if ( $curpath =~ /^(.*?)\.pl$/ ) {
		$of = "$1_$ver_s\.pl";
	    } elsif ( $curpath =~ /^(.*?[\/\\])[^\/\\]+$/ ) {
		$of = $1 . "${p_name}_${ver_s}\.pl";
	    } else {
	        $of = "${curpath}_${ver_s}\.pl";
	    }
	    print "You should update to $ver_s\!\n";
	    dolog(" \*\*\* You should update to $ver_s\! Update filename is \'$of\'.");
        }

    } else {
        print "You have actual version.\n";
        return;
    }
    $upd = get( $url . 'callip.pl' );
    unless( defined $upd ) {
        print STDERR "Can't get update.\n";
        dolog("Can't get update. ${url}callip.pl");
        return;
    }
    if ( open ( my $OF, ">$of") ) {
        binmode($OF);
        print( $OF $upd );
        close($OF);
        chmod 0755, $of if $^O eq 'linux';
        print "$of saved.\n\n";
        exit if $check_updates eq 'f';
    } else {
        print STDERR "Can't open $of ($!).\n";
    }
    } else {
	print STDERR "Can't connect to $url\n";
	dolog("Can't connect to $url");
    }
}

exportcfg() if defined $exportconf;

usage() if defined $needhelp;
if ($printver) {
    print "Youre version is $vers\n";
    exit;
}
if ( $whatsnew ) {
    my $wn = get( $url . 'poll.w');
    if( defined( $wn) ) {
	if ( $wn =~ /^$vers/i ) {
	    print $wn;
	} elsif ( $wn =~ /$vers/i ) {
    	    print $`;
	} else { print $wn; }
    } else {
	print "Can't get what's new.\n";
    }
    exit;
}

if (defined($fromfile)){
	if (defined($pollnodes)) {
		$pollnodes .= ' ' . readfromfile();
	} else {
		$pollnodes = readfromfile();
	}
}

if ( defined( $config ) ){
	readconf();
}

# set some defaults
$defaultzone = 2 unless defined $defaultzone;
$defaultdomain = 'fidonet' unless defined $defaultdomain;
$check_updates = 'w' unless defined $check_updates;
$outbound = '/home/fido/outbound' unless defined $outbound;

update();
usage() unless defined( $pollnodes );

dolog("$0 $arg");

my @nodes = split( / /, $pollnodes );

my ( $filename, $F, $fbsy );

for my $addr ( @nodes ) {

	$filename = getbsoname($addr);
	if ( -e "$filename.bsy" ) {
	    dolog("$addr is busy.");
	    next;
	}
	unless ( sysopen( $fbsy, "$filename.bsy", O_CREAT | O_EXCL ) ) {
	    print STDERR "Can't create $filename.bsy ($!).\n";
	    dolog("Can't create $filename.bsy ($!).");
	    next;
	}else { close($fbsy); }
	if ( -e "$filename.clo" ) {
	    dolog("Poll for $addr already exists.");
	    unless( unlink( "$filename.bsy" ) ) {
		print STDERR "Can't delete file $filename.bsy ($!).\n";
	    	dolog("Can't delete file $filename.bsy ($!).");
	    }
	    next;
	}
	print $filename."\n";
	if( sysopen( $F, "$filename.clo", O_CREAT | O_EXCL ) ) {
		close($F);
		dolog("Poll for $addr created.")
	} else {
		print STDERR "Can't create $filename.clo ($!)\n";
		dolog("Can't create poll for node $addr. Filename $filename.clo (Error: $!)");
	}
	unless( unlink( "$filename.bsy" ) ) {
	    print STDERR "Can't delete file $filename.bsy ($!).\n";
	    dolog("Can't delete file $filename.bsy ($!).");
	}
	if( defined $interval ) {
#	    print "   Poll to $addr created. Waiting $interval sec...\n";
	    sleep $interval;
	}
}

=head1 NAME

    poll.pl - advanced BSO poll program.

=head1 DESCRIPTION

    poll.pl - is designed to create a poll for several nodes at once for
    the BSO. Allows you to significantly reduce the number of event records.
    It is convenient to use together with binkd.

=head1 SYNOPSIS

    Usage: ./poll.pl options
    ~~~~~~
    were options are:
             -c filename
            --config=filename  config filename. May be omitted.

             -e
            --export   Export to STDOUT an example configuration file.

             -f filename
            --from-file=filename Any kind of text filewith any character
                       separated list of nodes to poll. May be omitted.
             -l filename
            --log-file=filename log file name. If omitted no logfile
                       will be created.
             -o path
            --outbound=path path to youre outbound. May be omitted.
                       Default '/home/fido/outbound/fidonet'.
             -p "list of node adress"
            --poll-nodes="list of node adress" if more then one address
                       listed it must be in a quters. May be omitted.
             -z zone
            --zone-default=zone youre zone number. May be omitted.
                       Default 2.
             -i=sec
            --interval sec Interval in seconds between poll creation.
                           Optional. Default 0.
             -u option
            --update=option How to update the program. Optional.
                          =d - download. Check for a new version and
                               download the update to a new file.
                          =f - Force download poll.pl end exit even
                               if no new version is found.
                          =w - warn. Check for a new version and warn
                               the sysop. Default.
                          =n - no. Do nothing.
             -V
            --ver      show version and exit.
             -w
            --whatsnew show whatsnew.
You can mix any options. At least one node addres to poll must be specified
in any way. You can mix ways to specify addresses to poll.

=head1 AUTHOR

   Stas Mishchenkov 2:460/58

=head1 COPYRIGHT AND LICENSE

   This program is free software; you may redistribute it and/or
   modify it under the same terms as Perl itself.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
