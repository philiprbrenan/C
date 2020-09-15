#!/usr/bin/perl -I/home/phil/perl/cpan/DataTableText/lib/
#-------------------------------------------------------------------------------
# Pango View
# Philip R Brenan at gmail dot com, Appa Apps Ltd Inc., 2020
#-------------------------------------------------------------------------------
use warnings FATAL => qw(all);
use strict;
use Carp;
use Data::Dump qw(dump);
use Data::Table::Text qw(:all);
use feature qw(say current_sub);

# fc-list | grep -e mono -e opentype -eLatin
# pango-view -t 'Hello' --dpi=1000 --font='Latin Modern Mono Sans Bold Oblique 48'
