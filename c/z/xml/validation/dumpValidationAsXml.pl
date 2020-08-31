#!/usr/bin/perl -I/home/phil/perl/cpan/DataTableText/lib/ -I/home/phil/perl/cpan/DitaValidate/lib
#-------------------------------------------------------------------------------
# _
# Philip R Brenan at gmail dot com, Appa Apps Ltd Inc., 2020
#-------------------------------------------------------------------------------
use warnings FATAL => qw(all);
use strict;
use Carp;
use Data::Dump qw(dump);
use Data::Table::Text qw(:all);
use Dita::Validate::SSV;
use Test::More qw(no_plan);
use feature qw(say current_sub);

my $x = printPerlDataAsXml(Dita::Validate::SSV::ssv);
owf(q(/home/phil/c/z/xml/validation/validation.xml), $x);


