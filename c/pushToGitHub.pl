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

my $home    = q(/home/phil/);                                                   # Local files
my $user    = q(philiprbrenan);                                                 # User
my $repo    = q(C);                                                             # Repo
my $wf      = q(.github/workflows/main.yml);                                    # Work flow
my $compile = q(/home/phil/perl/makeWithPerl/makeWithPerl.pl);                  # Compile
my @dir     = qw(/home/phil/c/);                                                # Directories to upload

my @cz      =  grep {!/\A#/} split /\s+/, <<END;                                # z files to run
arenaRedBlackTree
arenaTree
stringBuffer
xml
utilities
END

my @cg      =  grep {!/\A#/} split /\s+/, <<END;                                # g files to run
pangoText
END

if (1)                                                                          # Upload files
 {my @files = $compile;

  push @files, grep {-T $_ and !m(/backup/|/z/z/)}                              # Select files ignoring backups and tests
    searchDirectoryTreesForMatchingFiles(@dir, qw(.h .c .pl .md));

  my %files = map {$_=>1} grep {1 or /makeWithPerl/} @files;                    # Filter files

  for my $f(sort keys %files)                                                   # Upload each selected file
   {my $t = swapFilePrefix($f, $home);
       $t = swapFilePrefix($t, q(c/)) if $t =~ m(readme)i and $t !~ m(samples);
    lll "$f to $t ",
      GitHub::Crud::writeFileUsingSavedToken($user, $repo, $t, readFile($f));
   }
 }

sub test($$)                                                                    # Write one test
 {my ($dir, $c) = @_;                                                           # Folder, Program to be tested
  my $path = qq(c/$dir/$c/);
  my $file = fpe($home, $path, $c, q(c));
  -e $path or confess "No such file: $file";
  <<END;
    - name: Run $c
      if: always()
      run: |
        (cd $path; perl perl/makeWithPerl/makeWithPerl.pl --c --run $c.c --cIncludes c/includes)
        (cd $path; perl perl/makeWithPerl/makeWithPerl.pl --c --run $c.c --cIncludes c/includes --valgrind)
END
   }


my $tests = sub                                                                 # Commands to run tests
 {my @t;
  push @t, test q(z), $_ for @cz;
  push @t, test q(g), $_ for @cg;
  join "\n", @t;
 }->();

my $y = <<END;                                                                  # Workflow
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
        sudo apt -y install build-essential gdb tree gtk+-3.0-dev
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

lll GitHub::Crud::writeFileUsingSavedToken($user, $repo, $wf, $y);              # Upload workflow
