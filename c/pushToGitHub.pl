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
use GitHub::Crud qw(writeFileUsingSavedToken);
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
my $includes = fpd($dir, q(includes));                                          # Includes folder

my $cpan     = q(/home/phil/perl/cpan/PreprocessOps/);                          # Associated CPAN module
my $cpanRepo = containingFolderName(fpf($cpan, q(aaa)));                        # GitHub repo is the last component of the folder containing the CPAN module

my ($positional, $keywords) =                                                   # Parse command line
  parseCommandLineArguments {@_} [@ARGV],
  {clean => 'Clean up github',
   test  => 'Run tests before uploading'};

my @cz       =  grep {!/\A#/} split /\s+/, <<END;                               # C files in /z/ to upload and run
utilities
arenaList
stringBuffer
arenaTree
colour
rectangle
vector2d
xml
cairoText
mimagem
END

if (exists $$keywords{clean})                                                   # Clean up github
 {my $g = GitHub::Crud::new(userid => $user, repository => $repo);
  my @files = $g->list;
  for my $f(@files)
   {if ($f =~ m(\Ac/))
     {say STDERR "Delete: $f";
      $g->gitFile = $f;
      $g->delete;
     }
   }
 }

if (exists $$keywords{test})                                                    # Test all C files
 {clearFolder($includes, 20);
  for my $f(@cz)
   {my $c = fpe($dir, qw(z), $f, $f, q(c));
    lll $c;
    say STDERR qx(makeWithPerl --c  --run "$c");
   }
 }

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
    lll "$f to $t ", writeFileUsingSavedToken($user, $repo, $t, readFile($f));
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

  lll "Work flow to $repo ", writeFileUsingSavedToken($user, $repo, $wf, $y);
 }

# Cpan module

if (1)                                                                          # Upload Cpan module
 {for my $f(searchDirectoryTreesForMatchingFiles($cpan))                        # Upload each file in module
   {my $t = swapFilePrefix($f, $cpan);                                          # Target on GitHub
    next if $t =~ m(backup|zzz);                                                # Ignore work files
    if ($t =~ m(\A(.gitignore|Build.PL|CHANGES|CONTRIBUTING|MANIFEST|README.*|test\.pl|lib/.*)\Z))
     {my $s = readFile($f);
      lll "$f to $t ", writeFileUsingSavedToken($user, $cpanRepo, $t, $s);
     }
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
  lll "Work flow to $cpanRepo ", writeFileUsingSavedToken($user, $cpanRepo, $wf, $y);
 }
