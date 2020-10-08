#!/usr/bin/perl -I/home/phil/perl/cpan/DataTableText/lib/
#-------------------------------------------------------------------------------
# Create derived C packages.
# Philip R Brenan at gmail dot com, Appa Apps Ltd Inc., 2020
#-------------------------------------------------------------------------------
=pod

 A derived C package is obtained by copying another C package with a
 preprocessor definition uncommented to enable the extra functionality.

=cut

use warnings FATAL => qw(all);
use strict;
use Carp;
use Data::Dump qw(dump);
use Data::Table::Text qw(:all);
use feature qw(say current_sub);

my $home     = qq(/home/phil/c/z/);                                             # Home

sub dp($$$)                                                                     # Derive one package from another by uncommenting the specified preprocessor definition in the specified source package and writing it to the specified target package name.
 {my ($source, $target, $variable) = @_;                                        # Source package name, target package name, variable to uncomment to enable the extra functionality
  my $s = fpe($home, $source, $source, q(c));                                   # Source file
  my $d = readFile($s);                                                         # Read source
     $d =~ s(//(\#define\s+\$$variable\s)) ($1);                                # Uncomment
  my $S = ucfirst $source;
  my $T = ucfirst $target;
     $d =~ s($S) ($T)g;                                                         # Change hard coded references to the base file which typically occur in macros
  my $t = fpe($home, $target, $target, q(c));                                   # Target file name
  owf($t, $d);                                                                  # Write target
 }

dp(q(arenaList), q(arenaListEditable), q(Editable));
