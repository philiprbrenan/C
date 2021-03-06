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
use GitHub::Crud qw(:all);
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
arenaArray
arenaList
stringBuffer
arenaTree
colour
rectangle
vector2d
xml
cairoText
#mimagem
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
   {my $d = fpd($dir, qw(z), $f);
    my $c = fpe($d, $f, q(c));
    lll $f;
    my $r = qx(cd $d; makeWithPerl --c --run "$c" 2>&1);
    if ($r !~ m(SUCCESS:))
     {say STDERR $r;
      exit;
     }
   }
 }

# C files

my $lastModificationTimeC =                                                     # Get last modification time if possible of C repo
  readFileUsingSavedToken($user, $repo, q(lastModificationTime))
  // 0;

if (1)                                                                          # Upload C files
 {my @files = ($compile, $licence);

  push @files, grep {-T $_ and !m(/backup/|/z/z/)}                              # Select files ignoring backups and tests
    searchDirectoryTreesForMatchingFiles($dir, qw(.h .c .pl .md));

  my %files = map {$_=>1}                                                       # Select files by date
    grep {fileModTime($_) > $lastModificationTimeC}                             # Changed since last modification time
    @files;

  if (keys %files)
   {deleteFileUsingSavedToken($user, $repo, $wf);                               # Delete this file to prevent each upload triggering an action - it will be added at the end.

    for my $f(sort keys %files)                                                 # Upload each selected file
     {my $t = swapFilePrefix($f, $home);
      if (($t =~ m(readme)i and $t !~ m(samples)) or $t =~ m(license)i)
       {$t = swapFilePrefix($t, q(c/));
       }
      my $r = ppp  8, writeFileUsingSavedToken $user, $repo, $t, readFile($f);
      my $F = ppp 30, $f;
      my $T = ppp 20, $t;
      lll "$r $F to $T";
     }
   }
 }

sub test($$)                                                                    # Write one C test
 {my ($d, $c) = @_;                                                             # Folder, Program to be tested
  my $file = fpe($dir, $d, $c, $c, q(c));
  -e $file or confess "No such file: $file";

  my $path = fpd($git, q(c), $d, $c);
  my $valg = $c =~ m(cairoText|mimagem)i ? '' : q(--valgrind);                  # No valgrind on GTK+ as it leaks

  my @r = <<END;
    - name: Run $c
      if: always()
      run: |
        export PERL5LIB=\${PERL5LIB}:perl/lib/
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
    - uses: actions/checkout\@v2
      with:
        repository: 'philiprbrenan/perl'
        path: perl

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
        sudo cpan install Data::Table::Text Data::Dump Dita::PCD Digest::SHA1 Getopt::Long File::Basename Java::Doc Preprocess::Ops

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

  lll "Work flow for $repo ", writeFileUsingSavedToken($user, $repo, $wf, $y);
 }

writeFileUsingSavedToken($user, $repo, "lastModificationTime", time);

# Cpan module

my $lastModificationTimePreprocess =                                            # Get last modification time if possible or Perl repo
  readFileUsingSavedToken($user, $cpanRepo, q(lastModificationTime))
  // 0;

if (1)                                                                          # Upload Cpan module
 {my @files =                                                                   # Select files
   grep {fileModTime($_) > $lastModificationTimePreprocess}                     # Changed since last modification time
   grep { m((.gitignore|Build.PL|CHANGES|CONTRIBUTING|MANIFEST|README.*|test\.pl|lib/.*)\Z)i}
   grep {!m(backup|zzz)}                                                        # Ignore work files
   searchDirectoryTreesForMatchingFiles($cpan);

  if (@files)                                                                   # Upload changed files
   {deleteFileUsingSavedToken($user, $cpanRepo, $wf);                           # Delete this file to prevent each upload triggering an action - it will be added at the end.
    for my $f(@files)                                                           # Upload each file in module
     {my $t = swapFilePrefix($f, $cpan);                                        # Target on GitHub
      my $s = readFile($f);
      lll "$f to $t ", writeFileUsingSavedToken($user, $cpanRepo, $t, $s);
     }
   }
 }

if (1)
 {my $y = <<END;                                                                # Workflow for Perl
# Build and test CPAN module: Preprocess::Ops

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

    - name: InstallDTT
      run: |
        sudo cpan install Data::Table::Text

    - name: Test
      run: |
        tree
        perl Build.PL
        perl Build test
END
  lll "Work flow for $cpanRepo ", writeFileUsingSavedToken($user, $cpanRepo, $wf, $y);
 }

#writeFileUsingSavedToken($user, $cpanRepo, "lastModificationTime", time);
