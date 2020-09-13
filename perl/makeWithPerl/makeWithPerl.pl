#!/usr/bin/perl -I/home/phil/perl/cpan/DataTableText/lib  -I/home/phil/perl/cpan/JavaDoc/lib -I/home/phil/perl/cpan/DitaPCD/lib/ -I/home/phil/perl/cpan/DataEditXml/lib/ -I/home/phil/perl/cpan/GitHubCrud/lib/  -I/home/phil/perl/cpan/DataDFA/lib/ -I/home/phil/perl/cpan/DataNFA/lib/ -I//home/phil/perl/cpan/PreprocessOps/lib/
#-------------------------------------------------------------------------------
# Make with Perl
# Philip R Brenan at gmail dot com, Appa Apps Ltd, 2017
#-------------------------------------------------------------------------------
=pod

Integrated development environment for Geany or similar editor for compiling
testing, running Perl, Javascript or Java programs.

Installed as ~/.local/bin/makeWithPerl if you execute this file with this file

Compiled java classes will be placed in and read from a folder named $javaClasses
which you should create higher up the path from the java source files, possibly
in a folder named 'java'.

  makeWithPerl --perl  --compile "%f"
  makeWithPerl --perl  --run "%f"
  makeWithPerl --perl  --doc "%f"

Replace --perl with --java in the above for java compilation. Use

  perl /home/phil/z/java/documentation.pl "%f"

to extract and format Java documentation

=cut

use warnings FATAL => qw(all);
use strict;
use Carp qw(confess);
use Data::Dump qw(dump);
use Data::Table::Text qw(:all);
use Dita::PCD qw(:all);
use Getopt::Long;
use File::Basename;
use Java::Doc;
use Preprocess::Ops;
use utf8;

sub mwpl {qq(makeWithPerlLocally.pl)}                                           # Make with Perl locally

my $reversedDomain = qq(com.appaapps);                                          # Our domain name written in reverse order
my $javaHome    = fpd(qw(/home phil java));                                     # Location of java files
my $javaClasses = fpd($javaHome, qw(Classes));                                  # Folder containing java classes
my $androidJar  = fpe(qw(/home phil Android sdk platforms android-25 android jar));
my $swtJar      = fpe(qw(/home phil java    swt jars swt jar));
my $classPath   = join ':', $javaClasses, $androidJar, $swtJar;                 # Class path to classes created locally and well know jars
my $cIncludes   = fpd(qw(/home phil c includes));                               # C includes folder
my $compile;                                                                    # Compile
my $run;                                                                        # Run
my $doc;                                                                        # Documentation
my $c;                                                                          # C
my $cpp;                                                                        # C++
my $html;                                                                       # Html
my $perl;                                                                       # Perl
my $python;                                                                     # Python
my $java;                                                                       # Java
my $javascript;                                                                 # Javascript
my $bash;                                                                       # Bash
my $vala;                                                                       # Vala
my $valgrind;                                                                   # Run with valgrind to check for memory leaks
my $androidLog;                                                                 # Android log
my $xmlCatalog  = q(/home/phil/z/r/dita/dita-ot-3.1/catalog-dita.xml);          # Standard xml catalog
my $xmlHPE      = q(/home/phil/r/hp/dtd/Dtd_2016_07_12/catalog-hpe.xml);        # HPE catalog
my $xmlSF       = q(/home/phil/r/dita/salesForce/catalog.xml);                  # SF catalog

GetOptions
 ('compile'     =>\$compile,
  'run'         =>\$run,
  'c'           =>\$c,
  'cpp'         =>\$cpp,
  'doc'         =>\$doc,
  'html'        =>\$html,
  'perl'        =>\$perl,
  'python'      =>\$python,
  'java'        =>\$java,
  'bash'        =>\$bash,
  'vala'        =>\$vala,
  'javascript'  =>\$javascript,
  'androidLog'  =>\$androidLog,
  'cIncludes=s' =>\$cIncludes,
  'valgrind'    =>\$valgrind,
 );

unless($compile or $run or $doc or $html or $androidLog)                        # Check action
 {confess "Specify --compile or --run or --doc or --androidLog";
 }

my $file = shift @ARGV;                                                         # File to process

unless($file)
 {confess "Use %f to specify the file to process";
 }

if ($run and $file =~ m(((make|run)WithPerl).pl\Z)s)                            # Install this file
 {my $c = qq(cp $file ~/.local/bin/$1; chmod ugo+x ~/.local/bin/$1; hash -r);
  lll qq($c);
  xxx ($c);
  confess "Installed";
 }

if ($doc)                                                                       # Documentation
 {if ($file =~ m((pl|pm)\Z)s)                                                   # Document perl
   {say STDERR "Document perl $file";
    updatePerlModuleDocumentation($file);
   }
  elsif ($file =~ m((java)\Z)s)                                                 # Document java
   {say STDERR "Document java $file";

    my %files;
    #$files{$file}++;
    for(findFiles($javaHome))
     {next if m/Test\.java\Z/ or m(/java/z/);                                   # Exclude test files and /java/ sub folders
      $files{$_}++ if /\.java\Z/
     }

    my $j = Java::Doc::new;
    $j->source = [sort keys %files];
    $j->target = my $f = filePathExt($javaHome, qw(documentation html));
    $j->indent = 20;
    $j->colors = [map {"#$_"} qw(ccFFFF FFccFF FFFFcc CCccFF FFCCcc ccFFCC)];
    $j->html;
    qx(google-chrome $f);
   }
  else
   {confess "Unable to document file $file";
   }
  exit
 }

if (-e mwpl and $run)                                                           # Make with Perl locally
 {my $p = join ' ', @ARGV;
  if ($p !~ /test/i)
   {my $c = mwpl;
    print STDERR qx(perl -CSDA $c $p);
    exit;
   }
 }

if ($file =~ m(VocabularyActivity.java))                                        # Process special files
 {if ($compile)
   {my $l = '/home/phil/vocabulary/supportingDocumentation/libs/';
    my $a = '/home/phil/Amazon/sdk/Android/GameCircle/GameCircleSDK/libs/';
    my $c = 'javac  -Xlint -Xdiags:verbose -cp '.
     $l.'Translation.jar:'.
     $l.'Colours.jar:'.
     $l.'DejaVu.jar:'.
     $l.'amazon-mobile-associates-1.0.64.0.jar:'.
     $l.'aws-android-sdk-core-2.2.1.jar:'.
     $l.'aws-android-sdk-mobilenalytics-2.2.1.jar:'.
     $l.'aws-android-sdk-s3-2.2.1.jar:'.
     $l.'json-simple-1.1.1.jar:'.
     '/home/phil/Android/sdk/platforms/android-23/android.jar:'.
     $a.'AmazonInsights-android-sdk-2.1.26.jar:'.
     $a.'gamecirclesdk.jar:'.
     $a.'login-with-amazon-sdk.jar:'.
     '/home/phil/vocabulary/src '.
     '-d /home/phil/vocabulary/src/z /home/phil/vocabulary/src/VocabularyActivity.java';
    say   STDERR $c;
    print STDERR for qx($c);
   }
  else
   {my $c = "perl /home/phil/vocabulary/supportingDocumentation/perl/genApp.pm";
    print STDERR for qx($c);
   }
  &removeClasses;
  exit;
 }

if ($file =~ m(\.(dita|ditamap|xml)\Z))                                         # Process xml
 {my $source = readFile($file);
  my $C = $xmlCatalog;
     $C = $xmlHPE if $source =~ m(HPE)is;
     $C = $xmlSF  if $source =~ m(salesforce)is;
  my $c = qq(xmllint --noent --noout "$file" && echo "Parses OK!" && export XML_CATALOG_FILES=$C && xmllint --noent --noout --valid - < "$file" && echo Valid);
  say STDERR $c;
  say STDERR qx($c);
  exit;
 }

if ($file =~ m(\.pcd\Z)i)                                                       # Process Please Change Dita
 {if ($compile)
   {compilePcdFile($file);
   }
  else
   {my $i = fp $file;
    my $o = fpd($i, q(out));
    clearFolder($o, 1e2);
    pleaseChangeDita(in=>$i, out=>$o, trace=>1);
   }
  exit;
 }

#&Preprocess::file($file);                                                      # Vector pre-processing if required

if ($perl)                                                                      # Perl
 {if ($compile)                                                                 # Syntax check perl
   {print STDERR qx(perl -CSDA -cw "$file");
   }
  elsif ($run)                                                                  # Run perl
   {if ($file =~ m(\A/home/phil/r/www/perl/send)s)                              # Run from web server
     {&cgiPerl($file);
     }
    else                                                                        # Run from command line
     {print STDERR qx(timeout 10 perl -CSDA -w  "$file");
     }
   }
  elsif ($doc)                                                                  # Document perl
   {say STDERR "Document perl $file";
    updatePerlModuleDocumentation($file);
   }
 }
elsif ($c)                                                                      # GCC
 {my $lp = '-L/usr/lib/x86_64-linux-gnu/libperl.so.5.26 ';
  my $cp = qq(-fstack-protector-strong -finput-charset=UTF-8);

#  for my $lib(qw(gtk+-3.0 glib-2.0))
#   {$cp .= ' '.trim(qx(pkg-config --cflags $lib));
#    $lp .= ' '.trim(qx(pkg-config --libs $lib));
#   }

  my $optimize = 0;                                                             # Whether to optimize or not
# my $opt = 0 ? '-O3' : '-fprofile-arcs -ftest-coverage -aux-info /tmp/aux-info.data';
  my $opt = $compile ? '' : $optimize ? '-O3' : '--coverage -Wno-unused-function -ggdb';


  my $I    = $cIncludes;                                                        # Includes folders
  my $gcc  = "gcc $opt  -fmax-errors=132 -rdynamic -Wall -Wextra $cp";          # Options: rdynamic required for printStackBackTrace
     $gcc .= " -I$I";                                                           # Includes

  my $cFile = fpe($I, fn($file), q(c));                                         # Preprocess output
  my $hFile = fpe($I, fn($file).q(_prototypes), q(h));                          # Prototypes

  Preprocess::Ops::c($file, $cFile, $hFile);                                    # Preprocess

  if ($compile)
   {my $cmd = qq($gcc -c "$cFile" -o /dev/null);                                # Syntax check
    say STDERR $cmd;
    print STDERR $_ for qx($cmd);
   }
  else
   {my $e = $file =~ s(\.cp?p?\Z) ()gsr;                                        # Execute
    my $o = fpe($e, q(o));                                                      # Object file
    my $a = fpe($e, q(gcda));                                                   # Coverage file
    my $g = owf(undef, "run\nbacktrace\n");                                     # Gdb command file
#   my $c = qq(rm $e $o $a 2>/dev/null; $gcc -o "$e" "$cFile" $lp; gdb -batch-silent -x $g $e 2>&1);
    my $c = qq(rm $e $o $a 2>/dev/null; $gcc -o "$e" "$cFile" $lp && $e 2>&1);
    lll qq($c);
    my $result = qx($c);
    say STDERR $result;
    unlink $o, $g;
    exit(1);# unless $result =~ m(SUCCESS);                                       # Confirm successful
    if (!$optimize)                                                             # Place coverage files in a sub folder
     {my $c = qq(gcov $a);
      lll qq($c);
      say STDERR qx($c);
      my $d = fp($file);
      my $g = fpd($d, q(gcov));
      makePath($g);
      my @files = searchDirectoryTreesForMatchingFiles(fp($file), qw(.gcda .gcno .gcov));
      moveFileWithClobber $_, swapFilePrefix($_, $d, $g) for @files;
     }
    if ($valgrind)                                                              # Valgrind requested
     {my $c = qq(valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes $e 2>&1);
      lll qq($c);
      my $result = qx($c);
      exit(1) unless $result =~ m(ERROR SUMMARY: 0 errors from 0 contexts)
     }
   }
 }
elsif ($javascript)                                                             # Javascript
 {if ($compile)
   {say STDERR "Compile javascript $file";
    print STDERR qx(nodejs -c "$file");                                         # Syntax check javascript
   }
  else
   {my $c = qq(nodejs  --max_old_space_size=4096  "$file");                     # Run javascript
    say STDERR $c;
    print STDERR qx($c);
    say STDERR q();
   }
 }
elsif ($bash)                                                                   # Bash script
 {if ($compile)
   {say STDERR "Test bash $file";
    print STDERR qx(bash -x "$file");                                           # Debug bash
   }
  else
   {print STDERR qx(bash "$file");                                              # Bash
   }
 }
elsif ($androidLog)                                                             # Android log
 {my $adb = q(/home/phil/Android/sdk/platform-tools/adb);
  my $c = qq($adb -e logcat -d > $file && $adb -e logcat -c);
  say STDERR "Android log\n$c";
  print STDERR qx($adb -e logcat -d > $file && $adb -e logcat -c);
 }
elsif ($java)                                                                   # Java
 {my ($name, undef, $ext) = fileparse($file, qw(.java));                        # Parse file name
  my $package  = &getPackageNameFromFile($file);                                # Get package name
  my $standAloneTest;                                                           # Testing an Android class
  $standAloneTest = $package && $package =~ /src/ && $file =~ /\ATest/;
  my $buildApp = "../perl/makeWithPerl.pl";                                     # Optional app build file

  if (!-e $buildApp or $standAloneTest)                                         # Standard Java compile/run also used for stand alone tests of Android classes
   {my $a  = $javaClasses;                                                      # Folder for java classes
    my $cp = $classPath;                                                        # Class path
    if ($compile)                                                               # Compile
     {my $c =                                                                   # Syntax check Java
        "javac -g -d $a -cp $cp -Xlint -Xdiags:verbose $file -Xmaxerrs 99";
      say STDERR $c;
      print STDERR qx($c);
     }
    else                                                                        # Compile and run
     {my $class = "$package.$name";                                             # Class location
      my $p = join ' ', @ARGV;                                                  # Collect the remaining parameters and pass them to the java application
      my $c = "javac -g -d $a -cp $cp $file && java -ea -cp $cp $class $p";     # Run java
      say STDERR $c;
      print STDERR qx($c);
     }
   }
  else                                                                          # Optional Java app build file available to control creation of Java app
   {if ($compile)                                                               # Syntax check Java
     {my $c = "perl $buildApp --lint";
      print STDERR qx($c);
     }
    else                                                                        # Compile and run
     {my $c = "perl $buildApp --run";
      print STDERR qx($c);
     }
   }
  &removeClasses;
 }
elsif ($html)                                                                   # html
 {if (-e $file)
   {my $s = expandWellKnownUrlsInHtmlFormat readFile $file;
    my $F = temporaryFolder;
    my $f = owf(fpf($F, fne $file), $s);
    say STDERR qx(opera $f &);
   }
 }
elsif ($python)                                                                 # Python
 {if ($compile)                                                                 # Syntax check
   {print STDERR qx(python3 -m py_compile "$file");
   }
  elsif ($run)                                                                  # Run
   {print STDERR qx(python3 "$file");
   }
  elsif ($doc)                                                                  # Document
   {say STDERR "Document perl $file";
    updatePerlModuleDocumentation($file);
   }
 }
elsif ($vala)                                                                   # Vala
 {my $lib = "--pkg gtk+-3.0";                                                   # Libraries
   if ($compile)                                                                # Syntax check
   {print STDERR qx(valac -c "$file" $lib);
   }
  elsif ($run)                                                                  # Run
   {print STDERR qx(vala "$file" $lib);
   }
  elsif ($doc)                                                                  # Document
   {say STDERR "Document perl $file";
    updatePerlModuleDocumentation($file);
   }
 }

sub removeClasses
 {unlink for fileList("*.class")
 }

sub getPackageNameFromFile($)                                                   # Get package name from java file
 {my ($file) = @_;                                                              # File to read
  my $s = readFile($file);
  my ($p) = $s =~ m/package\s+(\S+)\s*;/;
  $p
 }

sub cgiPerl($)                                                                  # Run perl on web server
 {my ($file) = @_;                                                              # File to read

  my $r = qx(perl -CSDA -cw "$file" 2>&1);
  if ($r !~ m(syntax OK))
   {say STDERR $r;
   }
  else
   {my $base = fne $file;
    my $target = fpf(q(/usr/lib/cgi-bin), $base);
    lll qx(echo 121212 | sudo -S cp $file $target);
    lll qx(echo 121212 | sudo chmod ugo+rx $target);
    lll qx(opera http://localhost/cgi-bin/$base &);
   }
 }

__DATA__
sub getJavaCompiledClassesFolder2222                                            # Directory to contain classes compiled by javac -d
 {my $path = qx(pwd);                                                           # Present working directory
  chomp($path);
  my @path = split /\//, $path;                                                 # Path components
  my @Path = @path;
  while(@path)                                                                  # Walk up the path until we meet java or a folder containing 'Classes'
   {my $p = join '/', @path, $javaClasses;
    last if -d $p;                                                              # A folder containing compile java classes
    last if $path[-1] =~ /java/;                                                # A java folder
    pop @path;                                                                  # Try higher up
   }
  my $f = join '/', (@path ? @path : @Path), $javaClasses;                      # Reuse existing directory or create a new one if no existing directory
  makePath($f);
  $f
 }

=pod

If we have a java file in which any line starts with either of:

 //vector
 /*vector

then expand the vector statements and replace the original file so that the
vector operation are all inline and local; thus we can avoid the method
overhead, the allocation overhead and the garbage collection overhead that Java
would otherwise foist upon us.

=cut
my $temporaryIdentifier = 0;                                                    # Temporary identifiers

sub Preprocess                                                                  # Vector commands
 {package Preprocess;
  use Carp;
  use Data::Table::Text qw(:all);
  use Data::Dump qw(dump);
  use utf8;

  sub tab  {10}                                                                 # Comment tab width
  sub col  {80}                                                                 # Comment column

  sub file($)                                                                   # Preprocess a file
   {my ($file) = @_;
    return unless $java;                                                        # Vectorization only applied to java files
    my @gen;
    my $source = readFile($file);                                               # Read source file
       $source =~ s/\s+\Z//gs;

    for(split /\n/, readFile($file))                                            # Check each source line
     {my @p = m(\A/[*/](\w+));                                                  # Package name
      if (@p)                                                                   # Preprocess request label either /* or // followed by package
       {my @parms = split /\s+/, $_;                                            # Parse command
        my ($label, $command) = @parms;                                         # Label, command
        my $p = bless [@parms], $p[0];                                          # Use package name to bless request
        if ($command)
         {push @gen, $p->$command;                                              # Save generated code
         }
        else
         {warn "No command on line:\n$_";
         }
       }
      else                                                                      # Normal code
       {push @gen, $_;
       }
     }
    if (1)
     {my $s = join("\n", @gen);
         $s =~ s/\s+\Z//gs;

      if ($s ne $source)                                                        # Update the source file if we have created a newer version
       {writeFile($file, $s);
        say STDERR "Source $file updated";
       }
     }
   }

  sub parmCount($$)                                                             # Complain if there are too few parameters
   {my ($parms, $count) = @_;                                                   # Vector command parameters, relevant parameter count

    confess "Wrong parameter count, expected at least ".($count+2).             # Two few parameters
     " got ".scalar(@$parms).
     " see:\n". join(' ', @$parms)."\n" unless @$parms >= $count+2;

    confess "Undefined parameter ", dump($parms) if grep {!defined} @$parms;    # Undefined parameters
   }

  sub lay($$$$)                                                                 # Layout a line of code so things line up
   {my ($parms, $count, $code, $comment) = @_;                                  # Vector command parameters, relevant parameters count, code, comment
    parmCount($parms, $count);
    my $c = col;
    my $t = tab;
    $#$parms = $count + 1;                                                      # Relevant parameters
    my $name = \$parms->[0];                                                    # Address label
    $$name = '/*'.$$name =~ s/\A[\/*]+//r;                                      # Format label
    $parms->[1] = pad($parms->[1], $t);                                         # Format command name
    my $p = pad(join(' ', @$parms), $t);                                        # Format parameters
    my $s = join ' ', $p, '*/ '.$code;
    my $l = length($s);
    my $n = $l < $c ? $c - $l : $t - $l % $t;                                   # Space text length
    my $T = ' ' x $n;
    $s.$T.'/* '.$comment.' */';                                                 # Layout line of code
   }

  sub subParms($$$)                                                             # Substitute arguments for operands in template and description
   {my ($parms, $count, $string) = @_;                                          # Vector command parameters, relevant parameter count, command, parameters
    parmCount($parms, $count);
    my ($label, $command, @parms) = @$parms;                                    # Arguments to vector operation
    my @chars = split //, "𝗮𝗯𝗰𝗱𝗲𝗳𝗴𝗵𝗶𝗷𝗸𝗹𝗺𝗻𝗼𝗽𝗾𝗿𝘀𝘁𝘂𝘃𝘄𝘅𝘆𝘇";                         # Parameter names
    $#chars   = $count;                                                         # Number of parameters
    for(keys @chars)                                                            # Substitute each parameter
     {my $p = $parms[$_];
      my $c = $chars[$_];
      $string =~ s/$c/$p/g if defined($p);
     }
    $string
   }

  sub gen($$$$)                                                                 # Generate code for a generic vector operation
   {my ($parms, $count, $description, $template) = @_;                          # Vector command parameters, relevant parameter count, description of command, code template
    parmCount($parms, $count);
    $#$parms = $count+1;                                                        # Relevant parameters
    my ($label, $command, $a, $b, $c, $d) = @$parms;                            # Arguments to vector operation
    $$_ = subParms($parms, $count, $$_) for \$template, \$description;          # Substitute arguments for operands in template and description
    if ($template =~ /\.[𝘅𝘆]/)                                                  # x to x and x to y
     {my $x = $template =~ s/\.𝘅/_x/gr;
      my $y = $template =~ s/\.𝘅/_y/gr;
      lay($parms, $count, "$x $y", $description);
     }
    else
     {lay($parms, $count,  $template, $description);
     }
   }

  sub wrap($$$@)                                                                # Generate code for a generic vector operation
   {my ($parms, $count, $description, @statements) = @_;                        # Vector command parameters, relevant parameter count, description of command, code template
    parmCount($parms, $count);
    my $s = nws(join(' ', @statements) =~ s/\/\*.+?\*\///gr);                   # Put statements on one line and remove comments
    lay($parms, $count,  $s, subParms($parms, $count, $description));           # Layout command
   }

  sub temporaryIdentifier {'t_'.(++$temporaryIdentifier)}                       # Generate a temporary identifier name
 }

sub Vector                                                                      # Vector commands
 {package Vector;
  use Data::Dump qw(dump);
  use Data::Table::Text qw(:all);
  use utf8;

  sub gen {&Preprocess::gen(@_)}                                                # Generate source code for a command
  sub wrap{&Preprocess::wrap(@_)}                                               # Wrap the generated commands so that the source can be regenerated as needed
  sub tmp {&Preprocess::temporaryIdentifier(@_)}                                # Temporary identifier
  sub vector {my ($command) = @_; bless([__PACKAGE__, @_])->$command}           # Request source code
  sub closeEnough{1e-6}                                                         # Close enough

  sub x      {gen($_[0], 2, 'place x component of vector 𝗮 in scalar 𝗯',                         'final float 𝗯 = 𝗮_x;')}
  sub y      {gen($_[0], 2, 'place y component of vector 𝗮 in scalar 𝗯',                         'final float 𝗯 = 𝗮_y;')}
  sub xy     {gen($_[0], 3, 'place (x, y) components of vector 𝗮 in scalars 𝗯 and 𝗰',            'final float 𝗯 = 𝗮_x, 𝗰 = 𝗮_y;')}
  sub eq     {gen($_[0], 3, 'set scalar 𝗰 to show if vectors 𝗮 and 𝗯 are equal',                  'final boolean 𝗰 = Math.hypot(𝗮_x - 𝗯_x, 𝗮_y - 𝗯_y) < '.closeEnough.';')}
  sub eqFloat{gen($_[0], 3, 'set scalar 𝗰 to show if floats 𝗮 and 𝗯 are equal',                   'final boolean 𝗰 = Math.abs(𝗮 - 𝗯)'.               ' < '.closeEnough.';')}
  sub declare{gen($_[0], 1, 'declare vector 𝗮',                                                  'float 𝗮.𝘅 = 0;')}
  sub assign {gen($_[0], 2, 'assign vector 𝗮 to non final vector 𝗯',                             '𝗯.𝘅 = 𝗮.𝘅;')}
  sub X      {gen($_[0], 2, 'assign to the x component of vector 𝗯 from scalar 𝗮',               '𝗯_x = 𝗮;')}
  sub Y      {gen($_[0], 2, 'assign to the y component of vector 𝗯 from scalar 𝗮',               '𝗯_y = 𝗮;')}
  sub XY     {gen($_[0], 3, 'assign to vector 𝗰 from two scalar operands 𝗮 and 𝗯',               '𝗰_x = 𝗮; 𝗰_y = 𝗯;')}
  sub new    {gen($_[0], 3, 'new vector 𝗰 from two scalar operands 𝗮 and 𝗯',                     'final float 𝗰_x = 𝗮, 𝗰_y = 𝗯;')}
  sub plus   {gen($_[0], 3, 'vector 𝗮 plus '      .'vector 𝗯 giving new vector 𝗰',               'final float 𝗰.𝘅 = 𝗮.𝘅 + 𝗯.𝘅;')}
  sub minus  {gen($_[0], 3, 'vector 𝗮 minus '     .'vector 𝗯 giving new vector 𝗰',               'final float 𝗰.𝘅 = 𝗮.𝘅 - 𝗯.𝘅;')}
  sub times  {gen($_[0], 3, 'vector 𝗮 times '     .'scalar 𝗯 giving new vector 𝗰',               'final float 𝗰.𝘅 = 𝗮.𝘅 * 𝗯;')}
  sub over   {gen($_[0], 3, 'vector 𝗮 divided by '.'scalar 𝗯 giving new vector 𝗰',               'final float 𝗰.𝘅 = 𝗮.𝘅 / 𝗯;')}
  sub mid    {gen($_[0], 3, 'vector 𝗰 is mid way between vector 𝗮 and vector 𝗯',                 'final float 𝗰.𝘅 = (𝗮.𝘅 + 𝗯.𝘅) / 2f;')}

  sub dot    {gen($_[0], 3, 'vector 𝗮 dot vector 𝗯 giving scalar 𝗰',                             'final float 𝗰 = 𝗮_x * 𝗯_x + 𝗮_y * 𝗯_y;')}
  sub det    {gen($_[0], 3, 'vector 𝗮 det vector 𝗯 giving scalar 𝗰',                             'final float 𝗰 = 𝗮_x * 𝗯_y - 𝗮_y * 𝗯_x;')}
  sub detVec {gen($_[0], 5, 'vector 𝗲 = (det of vectors 𝗮 𝗯, det of vectors 𝗰, 𝗱)',              'final float 𝗲_x = 𝗮_x * 𝗯_y - 𝗮_y * 𝗯_x,  𝗲_y = 𝗰_x * 𝗱_y - 𝗰_y * 𝗱_x;')}

  sub length2{gen($_[0], 2, 'squared length of vector 𝗮 giving scalar 𝗯',                        'final float 𝗯 = 𝗮_x * 𝗮_x + 𝗮_y * 𝗮_y;')}
  sub length {gen($_[0], 2,         'length of vector 𝗮 giving scalar 𝗯',                        'final float 𝗯 = (float)Math.hypot(𝗮_x, 𝗮_y);')}

  sub clone  {gen($_[0], 2, 'copy vector 𝗮 to new vector 𝗯',                                     'final float 𝗯_x = 𝗮_x, 𝗯_y = 𝗮_y;')}
  sub rot1   {gen($_[0], 2, 'copy vector 𝗮 to new vector 𝗯 and rotate  90 anti clockwise',       'final float 𝗯_x = -𝗮_y, 𝗯_y =  𝗮_x;')}
  sub rot2   {gen($_[0], 2, 'copy vector 𝗮 to new vector 𝗯 and rotate 180 degrees',              'final float 𝗯_x = -𝗮_x, 𝗯_y = -𝗮_y;')}
  sub rot3   {gen($_[0], 2, 'copy vector 𝗮 to new vector 𝗯 and rotate  90 clockwise',            'final float 𝗯_x =  𝗮_y, 𝗯_y = -𝗮_x;')}
  sub about1 {gen($_[0], 3, 'rotate vector 𝗯 around vector 𝗮  90 anti clockwise giving vector 𝗰', 'final float 𝗰_x = 𝗮_x-(𝗯_y-𝗮_y), 𝗰_y = 𝗮_y+(𝗯_x-𝗮_x);')}
  sub about2 {gen($_[0], 3, 'rotate vector 𝗯 around vector 𝗮 180 degrees '.    'giving vector 𝗰', 'final float 𝗰_x = 𝗮_x-(𝗯_x-𝗮_x), 𝗰_y = 𝗮_y-(𝗯_y-𝗮_y);')}
  sub about3 {gen($_[0], 3, 'rotate vector 𝗯 around vector 𝗮   90 clockwise '.  'giving vector 𝗰', 'final float 𝗰_x = 𝗮_x+(𝗯_y-𝗮_y), 𝗰_y = 𝗮_y-(𝗯_x-𝗮_x);')}

  sub distanceTo
   {my ($parms) = @_;
    my ($label, $command, $a, $b, $c) = @$parms;
    my $t = tmp;
    wrap($parms, 3, "Put the distance from vector 𝗮 to vector 𝗯 in scalar 𝗰",
      vector(qw(minus), $b, $a, $t).
      vector(qw(length), $t, $c));
   }

  sub norm
   {my ($parms) = @_;
    my ($label, $command, $a, $b, $c) = @$parms;
    my ($t1, $t2) = (tmp, tmp);
    wrap($parms, 2, "copy vector 𝗮 to new vector 𝗯 and normalize",
      vector(qw(clone),  $a,  $t1).
      vector(qw(length), $a,  $t2).
      vector(qw(over),   $t1, $t2, $b));
   }

  sub angle                                                                     # sin(angle abc)
   {my ($parms) = @_;
    my ($label, $command, $a, $b, $c, $d) = @$parms;
    my ($ab, $cb, $abn, $cbn, $angle) = (tmp,tmp,tmp,tmp);
    wrap($parms, 4, "scalar 𝗱 = sin(angle between points 𝗮, 𝗯, 𝗰)",
      vector(qw(minus),   $a,  $b,  $ab).
      vector(qw(minus),   $c,  $b,  $cb).
      vector(qw(norm),    $ab, $abn).
      vector(qw(norm),    $cb, $cbn).
      vector(qw(det),    $abn, $cbn, $d));
   }
# https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
  sub intersectionOfTwoLines                                                    # The intersection of lines through points (𝗮, 𝗰) and (𝗯, 𝗱)
   {my ($parms) = @_;
    my ($label, $command, $a, $b, $c, $d, $e, $f) = @$parms;
    my ($ab, $cd, $x, $y, $D, $E, $F, $R) = map {tmp} 1..8;
    wrap($parms, 6, "The intersection of the lines through points (𝗮, 𝗯) and (𝗰, 𝗱) is placed in vector 𝗳, boolean 𝗲 is set if they are parallel or co-incident",
      vector(qw(minus),   $a,  $b,  $ab).                                       # along line 1
      vector(qw(minus),   $c,  $d,  $cd).                                       # along line 2
      vector(qw(det),     $ab, $cd, $D).                                        # super determinant zero if lines are parallel
      vector(qw(eqFloat), $D,  0,   $e).                                        # set e if parallel
      vector(qw(declare), $f).                                                  # declare result area
      "if (!$e) {".                                                             # If not parallel lines
        vector(qw(new),   $ab."_x", $cd."_x", $x).                              # x stack
        vector(qw(new),   $ab."_y", $cd."_y", $y).                              # y stack
        vector(qw(detVec), $a, $b, $c, $d, $E).                                 # det vector
        vector(qw(detVec), $E, $x, $E, $y, $F).                                 # det vector
        vector(qw(over),   $F, $D, $R).                                         # intersection
        vector(qw(assign), $R, $f).                                             # assign result
      "}");                                                                     # end if not parallel lines
   }

  sub pointOnLineClosestToPoint
   {my ($parms) = @_;
    my ($label, $command, $a, $b, $c, $d) = @$parms;
    my ($A, $B, $parallel, $R) = map {tmp} 1..4;
    wrap($parms, 4, "The point 𝗱 on the line through points (𝗯, 𝗰) nearest to point 𝗮",
      vector(qw(minus),   $c,  $b,  $A).                                        # Vector along line
      vector(qw(rot1),    $A,  $R).                                             # Right angles to line
      vector(qw(plus),    $a,  $R, $B).                                         # to point B
      vector(qw(intersectionOfTwoLines), $a, $B, $b, $c, $parallel, $d).        # Intersection of line and normal to line through vector a
     '');
   }

  sub distanceToLine
   {my ($parms) = @_;
    my ($label, $command, $a, $b, $c, $d) = @$parms;
    my ($D) = map {tmp} 1..1;
    wrap($parms, 4, "The shortest distance 𝗱 from the point 𝗮 to the line through the points (𝗯, 𝗰)",
      vector(qw(pointOnLineClosestToPoint), $a, $b, $c, $D). # Vector to line
      vector(qw(distanceTo),                $a, $D, $d).     # Distance
     '');
   }
 }

sub Utilities                                                                   # Some useful utilities that it is just too annoying to have as classes
 {package Utilities;
  use Data::Dump qw(dump);
  use utf8;

  sub here
   {my ($parms) = @_;
    &Preprocess::wrap($parms, 0, "Utilities: say(...)",
      'static void say(Object...O) {final StringBuilder b = new StringBuilder(); for(Object o: O) b.append(o.toString()); System.err.print(b.toString()+"\n");}'
     )
   }
 }

sub List                                                                        # Doubly linked lists
 {package List;
  use Data::Dump qw(dump);
  use utf8;

  sub gen {&Preprocess::gen(@_)}                                                # Generate source code for a command
  sub wrap{&Preprocess::wrap(@_)}                                               # Wrap the generated commands so that the source can be regenerated as needed
  sub tmp {&Preprocess::temporaryIdentifier(@_)}                                # Temporary identifier

  sub getLastTemplate                                                           # Get the last item in a list template
   {("Get the 𝗹ast item in list 𝗮 in object 𝗯 and place it in 𝗰 of type 𝗱",
     'final 𝗰  𝗱 = 𝗯.𝗮_𝗹ast ;'
   )}

  sub getNextTemplate                                                           # Get the item after an item in a list template
   {("Get the 𝗻ext item in list 𝗮 in object 𝗯 and place it in 𝗱 of type 𝗰",
     'final 𝗰 𝗱 = 𝗯.𝗮_𝗻ext;'
   )}

  sub putLastTemplate                                                           # Put item 𝗮 last in list 𝗯 template
   {("Put list item 𝗯 𝗹ast in list named 𝗮 in object 𝗰",
     '𝗯.𝗮_parent = 𝗰; 𝗯.𝗮_𝗽rev = 𝗰.𝗮_𝗹ast; if (𝗰.𝗮_𝗹ast != null) 𝗰.𝗮_𝗹ast.𝗮_𝗻ext = 𝗯; 𝗰.𝗮_𝗹ast = 𝗯; 𝗯.𝗮_𝗻ext = null; if (𝗰.𝗮_𝗳irst == null) 𝗰.𝗮_𝗳irst = 𝗯;  𝗰.𝗮_size++;'
   )}

  sub putNextTemplate                                                           # Put item 𝗮 last in list 𝗯 template
   {("Put list item 𝗯 𝗻ext in list named 𝗮 𝗮fter list item 𝗰",
     '𝗯.𝗮_parent = 𝗰.𝗮_parent; 𝗯.𝗮_𝗽rev = 𝗰; 𝗯.𝗮_𝗻ext = 𝗰.𝗮_𝗻ext; 𝗰.𝗮_𝗻ext = 𝗯; if (𝗯.𝗮_𝗻ext == null) {𝗯.𝗮_parent.𝗮_last = 𝗯;} else{ 𝗯.𝗮_𝗻ext.𝗮_𝗽rev = 𝗯;} 𝗯.𝗮_parent.𝗮_size++;'
   )}

  sub replaceNormal {qw(𝗳irst first 𝗹ast last  𝗽rev prev 𝗻ext next 𝗮fter after  𝗯efore before)}
  sub replaceReverse{qw(𝗳irst last  𝗹ast first 𝗽rev next 𝗻ext prev 𝗮fter before 𝗯efore after)}

  sub replace($@)                                                               # Replace items in a string
   {my ($s, %r) = @_;
    for(sort keys %r)
     {my ($a, $b) = ($_, $r{$_});
      $s =~ s/$a/$b/gs;
     }
    $s
   }

  sub list{gen($_[0], 2, "Declare List header for list called 𝗮 of elements of type 𝗯", '𝗯 𝗮_first = null, 𝗮_last = null; int 𝗮_size = 0;')}
  sub item{gen($_[0], 3, "Declare a list item for a list called 𝗮 in object 𝗰 that will be added to list 𝗮 in object 𝗯", '𝗰 𝗮_parent = null; 𝗯 𝗮_prev = null, 𝗮_next = null;')} # Declare list named 𝗮 with header in 𝗯 in class 𝗰
  sub size{gen($_[0], 3, "Size of list 𝗮 in object 𝗯 is placed in 𝗰", 'final int 𝗰 = 𝗯.𝗮_size;')} # Declare list named 𝗮 with header in 𝗯 in class 𝗰

  sub last   {gen($_[0], 4, map {replace($_, replaceNormal)}  getLastTemplate)}
  sub first  {gen($_[0], 4, map {replace($_, replaceReverse)} getLastTemplate)}
  sub next   {gen($_[0], 4, map {replace($_, replaceNormal)}  getNextTemplate)}
  sub prev   {gen($_[0], 4, map {replace($_, replaceReverse)} getNextTemplate)}
  sub parent {gen($_[0], 4, 'Find parent in list 𝗮 of item 𝗯 in object of type 𝗰, put it in 𝗱', "final 𝗰  𝗱 = 𝗯.𝗮_parent;")}

  sub push   {gen($_[0], 3, map {replace($_, replaceNormal)}  putLastTemplate)}
  sub unshift{gen($_[0], 3, map {replace($_, replaceReverse)} putLastTemplate)}
  sub putNext{gen($_[0], 3, map {replace($_, replaceNormal)}  putNextTemplate)}
  sub putPrev{gen($_[0], 3, map {replace($_, replaceReverse)} putNextTemplate)}

  sub for    {gen($_[0], 4, "Iterate objects in list name 𝗮 in object 𝗯 placing results in object type 𝗰 in object 𝗱",
                            'for(𝗰 𝗱 = 𝗯.𝗮_first; 𝗱 != null; 𝗱 = 𝗱.𝗮_next)')}
 }

# cp /home/phil/perl/makeWithPerl/makeWithPerl.pl /home/phil/.local/bin/makeWithPerl; chmod ugo+x /home/phil/.local/bin/makeWithPerl; hash -r
