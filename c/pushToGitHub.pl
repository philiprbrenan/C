#!/usr/bin/perl -I/home/phil/perl/cpan/DataTableText/lib/ -I/home/phil/perl/cpan/GitHubCrud/lib/
#-------------------------------------------------------------------------------
# Push C, perl/cpan and perl/makeWithPerl to GitHub C
# Philip R Brenan at gmail dot com, Appa Apps Ltd Inc., 2020
#-------------------------------------------------------------------------------

=pod

Does not work under Geany - use from the command line only.

=cut

use warnings FATAL => qw(all);
use strict;
use Carp;
use Data::Dump qw(dump);
use Data::Table::Text qw(:all);
use GitHub::Crud;
use YAML::Loader;
use feature qw(say current_sub);

my $home = q(/home/phil/);                                                      # Local files
my $user = q(philiprbrenan);                                                    # Repo
my $repo = q(C);
my $wf   = q(.github/workflows/main.yml);

my @c    =  grep {!/\A#/} split /\s+/, <<END;                                   # C files
arenaRedBlackTree
arenaTree
fileName
#redBlackTree
readOnlyBytes
stringBuffer
xml
utilities
END

if (1)                                                                          # Upload files
 {my @files;
  push @files, grep {!-d $_ and !m(/backup/|/z/z/)}
    searchDirectoryTreesForMatchingFiles(qw(/home/phil/c/ .h .c .pl .md)),
    q(/home/phil/perl/makeWithPerl/makeWithPerl.pl);

  my %files = map {$_=>1} grep {1 or /makeWithPerl/} @files;

  for my $f(sort keys %files)
   {my $t = swapFilePrefix($f, $home);
       $t = swapFilePrefix($t, q(c/)) if $t =~ m(readme)i and $t !~ m(samples);
    lll "$f to $t ",
      GitHub::Crud::writeFileUsingSavedToken($user, $repo, $t, readFile($f));
   }
 }

my $tests = sub                                                                 # Commands to run tests
 {my @t;
  for my $c(@c)
   {push @t, <<END;
    - name: Run $c
      continue-on-error: true
      run: |
        perl perl/makeWithPerl/makeWithPerl.pl --c --run c/z/$c/$c.c --cIncludes c/includes
        perl perl/makeWithPerl/makeWithPerl.pl --c --run c/z/$c/$c.c --cIncludes c/includes --valgrind
END
   }
  join "\n", @t;
 }->();

my $y = <<END;
# Test various C programs written with the help of: https://metacpan.org/pod/Preprocess::Ops

name: Test

on:
  push

jobs:
  test:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout\@v2

    - name: Install
      run: |
        sudo apt update
        sudo apt -y install build-essential gdb tree
        sudo cpan install Data::Table::Text Data::Dump Dita::PCD Getopt::Long File::Basename Java::Doc Preprocess::Ops

    - name: Install Valgrind
      run: |
        sudo apt -y install valgrind

    - name: Home
      run: |
        echo \$HOME
        tree

$tests
END
#        sudo apt -y install build-essential libgtk-3-dev gdb tree

lll GitHub::Crud::writeFileUsingSavedToken($user, $repo, $wf, $y);
