$file = $ARGV[0] || die;

use Config;
($perl_incl = $Config::Config{libpth})  =~ s/^\"(.*)\"$/$1/;
($perl_lib  = $Config::Config{libperl}) =~ s/^\"(.*)\"$/$1/;

open(FILE, ">$file") || die;
print FILE <<EOT;
!if !defined(PERL_INCL)
PERL_INCL=$perl_incl
!endif

!if !defined(PERL_LIB) && !defined(PERLDL)
PERL_LIB=$perl_lib
!endif
EOT
close(FILE);
