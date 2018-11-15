#! /usr/bin/perl
#
# (C) Stas Mishchenkov, 2:460/58 ;)
#
#@CHRS: CP866
#
#*****************************************************************************

$binkdconf="D:\\Fido\\binkd\\binkd.cfg"; # Полное имя конфига бинкд.

$r46tbl="D:\\fido\\logs\\r46tab.tpl"; # имя файла графика по региону.
$alltbl="D:\\fido\\logs\\all.tpl"; # имя файла общего графика.
$sesstbl="D:\\fido\\logs\\sess.tpl"; # имя файла таблички статистики по сесиям/траффику.

#*****************************************************************************
#
#*****************************************************************************

	setvar();
	readconf();
	readbinkdlog();
	qsortp();

	unless (open (Fr, ">$r46tbl")) {
		printf( "Cannot open $r46tbl: $!\r");
		exit;
	}
	unless (open (Fa, ">$alltbl")) {
		printf( "Cannot open $alltbl: $!\r");
		exit;
	}
	unless (open (Fss, ">$sesstbl")) {
		printf( "Cannot open $sesstbl: $!\r");
		exit;
	}

  $n=1;
  $r46n=0;
  $totalSent=0;
  $totalRcvd=0;
  $totalSess=0;

  print(Fr "CHRS: CP866 2\nRealName: Evil Robot\n\n * From $startperiod till $endperiod *\n\n                        0 1 2 3 4 5 6 7 8 9 1011121314151617181920212223\n");
  print(Fa "CHRS: CP866 2\nRealName: Evil Robot\n\n * From $startperiod till $endperiod *\n\n                        0 1 2 3 4 5 6 7 8 9 1011121314151617181920212223\n");
  print(Fss "CHRS: CP866 2\nRealName: Evil Robot\n\n * From $startperiod till $endperiod *\n┌────────────────────────┬──────────────┬──────────────┬─────────────────────┐\n│     Node address       │  Sent bytes  │Received bytes│   In     Out  Sess. │\n├────────────────────────┼──────────────┼──────────────┼─────────────────────┤\n");
  until( $n > $nodes ) {
	$h=0;
	$strout="│";

	$strout=$strout . sprintf("%-20s", $list{$n});
	until( $h == 24) {
		$hh=sprintf("%02d", $h);
		$a=$list{$n};
		if ( defined($tab{$a}{$hh})) {
		$c=$tab{$a}{$hh}; } else {
                $c=0;}
		$strout=$strout . "│$char{$c}";
		$h++;
	}
	$strout=$strout . "│\n";
# r46 table
        if ( $list{$n} =~ /$2\:46[^\.]+\d+$/) {
		$r46n++;
		print(Fr sprintf("%3s", $r46n) . $strout);
	}
# r46 table
	print(Fa sprintf("%3s", $n) . $strout);

	$sessFrom{$a} = 0 if !defined($sessFrom{$a});
	$sessTo{$a} = 0 if !defined($sessTo{$a});
	print(Fss "│" . sprintf("%3s", $n) . "│" . sprintf("%-20s", $a) . "│" . sprintf("%14s", $addrS{$a}) . "│" . sprintf("%14s", $addrR{$a}) . "│" . sprintf("%6s", $sessFrom{$a})  . "│" . sprintf("%6s", $sessTo{$a})  . "│"  . sprintf("%7s", $sessTo{$a} + $sessFrom{$a})  . "│" ."\n" );

	$totalSent+=$addrS{$a};
	$totalRcvd+=$addrR{$a};
	$totalSess+=$sessTo{$a} + $sessFrom{$a};

	$n++;
  }

  print(Fr "\n\" \" - 0 sessions, ░ - 1 session, ▒ - 2 sessions,\n▓ - 3 sessions, █ - 4 or more sessions.\n\n");
  print(Fa "\n\" \" - 0 sessions, ░ - 1 session, ▒ - 2 sessions,\n▓ - 3 sessions, █ - 4 or more sessions.\n\n");
  print(Fss "├────────────────────────┼──────────────┼──────────────┼─────────────────────┤\n│                Total:  │" . sprintf("%14s", $totalSent) ."│" . sprintf("%14s", $totalRcvd) ."│" . sprintf("%21s", $totalSess) ."│\n");

  close(Fr);
  close(Fa);
  close(Fss);

exit(0);

sub qsortp
{
  $n=1;
  $a=0;
  until( $n == $nodes ) {
	$nn=$n+1;
	if ( $nn <= $nodes ) {


	$list{$n} =~ /(\d+)\:(\d+)\/(\d+)\.?(\d*)/;
	($z, $ne, $f, $p) = ($1, $2, $3, $4);
	if ($p !~ /\d+/) { $p = 0; }

	$list{$nn} =~ /(\d+)\:(\d+)\/(\d+)\.?(\d*)/;
	($zn, $nen, $fn, $pn) = ($1, $2, $3, $4);
	if ($pn !~ /\d+/) { $pn = 0; }

		if (($z <=> $zn || $ne <=> $nen || $f <=> $fn || $p <=> $pn) == 1) {
	       		$a=$list{$n};
			$list{$n}=$list{$nn};
			$list{$nn}=$a;
		}
        }
	$n++;
  }
  qsortp() if $a != 0;
}

sub readconf
{

$point="0";
$nodes=0;

	unless (open (F, "<$binkdconf")) {
		printf( "Cannot open $binkdconf: $!\r");
		exit(1);
	}
	
	while (defined($line = <F>)) {
		$line =~ s/	/ /g;
		next unless $line !~ /^\#.*/i;
		
		if ( $line =~ /^[ ]*address[ ]+(\d+)\:(\d+)\/(\d+)\.*\d*\@.*/i && !defined ($node)) {
			($zone, $net, $node)=($1, $2, $3);
			next;
		}
		
		if ($line =~ /^[ ]*log[ ]+([^ ]+).*/i) {
			$binkdlog = $1;
			next;
		}

		if ( $line =~ /^[ ]*node[ ]+([^ ]+)[ ]+([^ ]+)[ ]+([^ ]+).*/i ) {
			($_address, $domain, $password) = ($1, $2, $3);

			$nodes++;

			if ($_address =~ /^(\d+)\:\d+.*/) {
				$z=$1;
			} else {
				$z=$zone;
			}
			if ($_address =~ /^\d+\:(\d+)\/.*/ || $_address =~ /^(\d+)\/.*/) {
				$n=$1;
			} else {
				$n=$net;
			}

			if ($_address =~ /.*\/(\d+)\..*/ || $_address =~ /.*\/(\d+)$/ || $_address =~ /^(\d+)\..*/ || $_address =~ /^(\d+)$/) {
				$f=$1;
			} else {
				$f=$node;
			}
			if ($_address =~ /.*\.(\d+)$/) {
				$p=$1;
			} else {
				$p=$point;
			}
			
			if ($p != 0) {
				$aa="$z\:$n\/$f\.$p";
			} else {
				$aa="$z\:$n\/$f";
			}
			
			$addr{$aa}=1;
			$list{$nodes}=$aa;
			$sessFrom{$aa}=0;
			$sessTo{$aa}=0;
			$addrR{$aa}=0;
			$addrS{$aa}=0;
		}
	}
	
	close(F);
}

sub setvar
{
$char{0}=" ";
$char{1}="░";
$char{2}="▒";
$char{3}="▓";
$char{4}="█";
}

sub readbinkdlog
{
	unless (open (F, "<$binkdlog")) {
		printf( "Cannot open $binkdlog: $!\r");
		exit(1);
	}

  while (defined($line = <F>)) {
	$line =~ s/\r?\n$//s;

	next unless $line =~ /^. (\d\d [a-z]{3}) (\d\d):(\d\d:\d\d) \[\d+\] done \((from|to) (\d+\:\d+\/\d+\.?\d*)\@[^,]+, ([^,]+), S\/R\: \d+\/\d+ \((\d+)\/(\d+) bytes\)\)$/i;
	($day, $hour, $minsec, $direction, $address, $result, $sent, $rcvd) = ($1, $2, $3, $4, $5, $6, $7, $8);
	
	if ( !defined($startperiod) ) {
		$startperiod="$day $hour:$minsec";
	}

	if ( $direction eq "from" ) {
		$sessFrom{$address}++;
	}
	if ( $direction eq "to" ) {
		$sessTo{$address}++;
	}
	
	if ( !defined( $addr{$address} ) ) {
		$nodes++;
		$addr{$address}=1;
		$addrS{$address}=$sent;
		$addrR{$address}=$rcvd;
		$list{$nodes}=$address;
	} else {
		$addrS{$address}+=$sent;
		$addrR{$address}+=$rcvd;
        }
	if ( !defined($addr{$address}{$direction})) {
		$addr{$address}{$direction}=1;
	} else {
		$addr{$address}{$direction}++;
	}
	if ( !defined($tab{$address}{$hour}) ) {
		$tab{$address}{$hour}=1;
	} else {
		if ($tab{$address}{$hour} < 4) {
			$tab{$address}{$hour}++;
		}
	}
  }     
  $endperiod="$day $hour:$minsec";
  close(F);

}
