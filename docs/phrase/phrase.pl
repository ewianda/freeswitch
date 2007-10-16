#!/usr/bin/perl
#
# Phrase verification and generation script.
#

use XML::Simple;
use Data::Dumper;

my $ref = XMLin("phrase.xml");

foreach $language ( sort keys %{$ref}) {
  foreach $item ( sort keys %{$ref->{$language}}) {
    foreach $element ( sort keys %{$ref->{$language}->{$item}}) {
      print "Language: $language, $item, $element\n";
      #system("mkdir -p $language/$item");
      foreach $foo (@{$ref->{$language}->{$item}->{$element}}) {
	print "filename: $language/$item/$foo->{filename} contains phrase \"$foo->{phrase}\"\n";
	# insert command to verify or generate files here
	# system("say -o $language/$item/$foo->{filename} \"$foo->{phrase}\"");
      }
    }
  }
}


