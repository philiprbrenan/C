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

my $home     = q(/home/phil/);                                                  # Local files
my $git      = q(/home/runner/work/C/C);                                        # Corresponding location on Github
my $user     = q(philiprbrenan);                                                # User
my $repo     = q(C);                                                            # Repo
my $wf       = q(.github/workflows/main.yml);                                   # Work flow
my $compile  = q(/home/phil/perl/makeWithPerl/makeWithPerl.pl);                 # Compile
my $licence  = q(/home/phil/c/LICENSE.md);                                      # License
my $dir      = fpd($home, q(c/));                                               # Directories to upload

my $cpan     = q(/home/phil/perl/cpan/PreprocessOps/);                          # Associated CPAN module
my $cpanRepo = containingFolderName(fpf($cpan, q(aaa)));                        # GitHub repo is the last component of the folder containing the CPAN module

my @cz       =  grep {!/\A#/} split /\s+/, <<END;                               # C files in /z/ to upload and run
arenaList
arenaTree
cairoText
colour
mimagem
rectangle
stringBuffer
utilities
vector2d
xml
END

# C files

if (1)                                                                          # Upload C files
 {my @files = ($compile, $licence);

  push @files, grep {-T $_ and !m(/backup/|/z/z/)}                              # Select files ignoring backups and tests
    searchDirectoryTreesForMatchingFiles($dir, qw(.h .c .pl .md));

  my %files = map {$_=>1} grep {1 or /makeWithPerl/} @files;                    # Filter files

  for my $f(sort keys %files)                                                   # Upload each selected file
   {my $t = swapFilePrefix($f, $home);
    if (($t =~ m(readme)i and $t !~ m(samples)) or $t =~ m(license)i)
     {$t = swapFilePrefix($t, q(c/));
     }
    lll "$f to $t ",
      GitHub::Crud::writeFileUsingSavedToken($user, $repo, $t, readFile($f));
   }
 }

sub test($$)                                                                    # Write one C test
 {my ($d, $c) = @_;                                                             # Folder, Program to be tested
  my $file = fpe($dir, $d, $c, $c, q(c));
  -e $file or confess "No such file: $file";

  my $path = fpd($git, q(c), $d, $c);
  my $valg = $c =~ m(cairoText|mimagem)i ? '' : q(--valgrind);                  # Nop valgrind on GTK+ as it leaks

  my @r = <<END;
    - name: Run $c
      if: always()
      run: |
        (cd $path; perl $git/perl/makeWithPerl/makeWithPerl.pl --c --run $path/$c.c --cIncludes $git/c/includes $valg --gccVersion gcc-10)
END
    join '', @r;
 }

my $tests = join "\n", map{test q(z), $_} @cz;                                  # Commands to run tests for C

if (1)
 {my $y = <<END;                                                                # Workflow for C
# Test various C programs written with the help of: https://metacpan.org/pod/Preprocess::Ops

name: Test

on:
  push

jobs:
  test:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout\@v2

    - name: Env
      run: |
        echo \$HOME
        pwd
        ls -la fonts/NotoSansMono-Regular.ttf
        md5sum fonts/NotoSansMono-Regular.ttf

    - name: Install
      run: |
        sudo apt update
        sudo apt -y install build-essential gdb tree gtk+-3.0-dev
        sudo cpan install Data::Table::Text Data::Dump Dita::PCD Getopt::Long File::Basename Java::Doc Preprocess::Ops

    - name: Install Valgrind
      run: |
        sudo apt -y install valgrind

    - name: GCC10
      run: |
         sudo add-apt-repository ppa:ubuntu-toolchain-r/test
         sudo apt-get update
         sudo apt install gcc-10

    - name: Check
      run: |
        perl -v
        gcc-10 -v

    - name: Tree
      run: |
        tree

$tests
END

  lll "C Work flow to $repo ",
    GitHub::Crud::writeFileUsingSavedToken($user, $repo, $wf, $y);
 }

# Cpan module

if (1)                                                                          # Upload Cpan module
 {my %files = map {$_=>1} searchDirectoryTreesForMatchingFiles($cpan);          # Possible files

  for my $f(sort keys %files)                                                   # Upload each file
   {my $t = swapFilePrefix($f, $cpan);
    next if     $t =~ m(backup|zzz);
    next unless $t =~ m(\A(Build.PL|CHANGES|CONTRIBUTING|MANIFEST|README.*|test\.pl|lib/.*)\Z);
    lll "$f to $t ",
      GitHub::Crud::writeFileUsingSavedToken($user, $cpanRepo, $t, readFile($f));
   }
 }

if (1)
 {my $y = <<END;                                                                # Workflow for Perl
# Build and text CPAN module: Preprocess::Ops

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
        sudo apt -y install build-essential tree
        sudo cpan install Data::Table::Text

    - name: Test
      run: |
        tree
        perl Build.PL
        perl Build test
END
  lll "Perl Work flow to $cpanRepo ",
    GitHub::Crud::writeFileUsingSavedToken($user, $cpanRepo, $wf, $y);
 }
