#!/usr/bin/perl
use strict;
use subs qw/ChkAddr ChkSum DisAsm FixRec GenRec/;
use Getopt::Std;
use vars qw/ $opt_a $opt_c $opt_d $opt_f $opt_g $opt_s $opt_l /;

getopts ('acdfgs:l:');
if ($#ARGV == -1) {
   open FILE, "-";
}
else {
   open FILE, $ARGV[-1] or die "No such file";
}

if ($opt_a) { ChkAddr; }
elsif ($opt_c) { ChkSum; }
elsif ($opt_d) { DisAsm; }
elsif ($opt_f) { FixRec; }
elsif ($opt_g) { GenRec; }
else {
   DisAsm;
}

sub ChkAddr {
   my ($addr, $prev_addr,$type);
   $prev_addr = 0;
   while (<FILE>) {
      s/\s*$//; # remove trailing white space
      $type = unpack ("x A", $_);
      if ($type =~ /[123]/) {
         $addr = hex unpack ("x4a" . (2*($type+1)), $_);
         die "Address is out of order at line $." if ($prev_addr >= $addr);
         $prev_addr = $addr;
      }
   }
}

sub ChkSum {
   my ($len, $cs);
   while (<FILE>) {
      s/^..//;
      s/\s*$//;
      $len = (length $_) - 1;
      $cs = 0;
      for (unpack ("a2" x ($len/2), $_)) {
         $cs += hex $_;
      }
      $cs = ~$cs;
      $cs %= 256;
      die "Checksum error on line $. : ", sprintf ("0x%02X", $cs), "\n" 
      if (hex unpack ("x" x ($len-1) . "a2", $_) != $cs);
   }
}

sub DisAsm {
   my($type, $count, $addr, $addrlen);
   my ($str);
   my ($last_addr);

   $last_addr = 0;
   while (<FILE>) {
      #print;
      s/\s*$//;  # remove trailing white space
      die "Line $. is not an S-record\n" unless unpack ("A", $_) eq "S";
      $type = unpack ("x A", $_);
      $count = (hex unpack ("x2 a2", $_)) - 1; # w/o checksum
      SWITCH: {
         $type =~ /0/ && do {
            $count -= 2;            # ignore address field
            $str = unpack ("x8 a" . (2*$count), $_);
            print "S0: ";
            for (unpack ("a2" x (length ($str)/2), $str)) {
               print chr hex $_;
            }
            print "\n";
            last SWITCH;
         };
         $type =~ /[123]/ && do {
            $addrlen = $type + 1;
            $count -= $addrlen;
            $addr = hex unpack ("x4a" . (2*$addrlen), $_);
            $str = unpack ("x". (6 + 2*$type) . "a". (2*$count), $_);
            #printf ("%d:%8x:%2d:%s\n", $addrlen, $addr, $count, $str);
            #print "$_\n";
            for (unpack ("a8" x (length ($str)/8), $str)) {
               if (hex $_) {
                  print "...\n" if ($last_addr != $addr - 4);
                  printf ("%8x: %s\n", $addr, lc $_);
                  $last_addr = $addr;
               }
               $addr += 4;
            }
            #print "\n";
            last SWITCH;
         };
         $type =~ /[789]/ && do {
            $addr = unpack ("x4a" . ((11 - $type) * 2), $_);
            print ("Starting address is $addr\n");
            #print "$_\n";
            last SWITCH;
         };
      }
   }
}

sub FixRec {
   my ($addr, $type, $count, $format);
   my ($str, $chk);
   while (<FILE>) {
      s/\s*$//; # remove trailing white space
      $type = unpack ("x A", $_);
      $count = hex unpack ("x2 a2", $_); # w/o checksum
      if ($type =~ /[123]/) {
         $addr = unpack ("x4a" . (2*$type+2), $_);
         $format = "x" . (6 + 2*$type) . "a" . (2*($count-$type-2));
         $str = unpack ($format, $_);
         $chk = hex unpack ("x" . ($count*2 + 2) . "a2", $_);
         #if ( $str =~ m/[^0]+/) {
            if ($type == 3) {
               print "$_\n";
            }
            else {
               print "S3", sprintf ("%02X", $count + 3 - $type), 
                  "00" x (3 - $type), $addr, $str, 
                  sprintf ("%02X\n", ($chk + $type - 3) & 0xFF);
            }
         #}
      }
      elsif ($type =~ /8/) {
         printf STDERR "\nWarning: S8 record on line $.\n";
         printf ( "S70500%s%02X\n", unpack ("x4a6", $_),
            (hex unpack ("x10a2", $_)) - 1);
      }
      else {
         print "$_\n";
      }
   }
}

sub GenRec {
   my ($addr, $len, $cs, $byte, $cst);

   $addr = eval $opt_s;
   $opt_l = eval $opt_l;
   printf (STDERR  "Starting address is 0x%x\n", $addr);
   print STDERR "Length is $opt_l\n";
   while ($len < $opt_l) {
      printf ("S309%08x", $addr);
      $cs = 9;
      $cst = $addr;
      while ($cst > 0) {
         $cs += $cst % 256;
         $cst >>= 8;
      }
      for (0..3) {
         $byte = int (256 * rand);
         printf ("%02x", $byte);
         $cs += $byte;
      }
      $cs = ~$cs;
      $cs %= 256;
      printf ("%02x\n", $cs);
      $len += 4;
      $addr += 4;
   }
   print "S70500400000BA\n\n";
}
