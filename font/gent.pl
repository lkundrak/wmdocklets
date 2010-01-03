# Create a template for font drawing
# perl gent.pl 8 <cp437.xpm >8pxfont.xpm

my $line;
my @result;

my $width = (shift @ARGV or 8);

while (<>) {
	/".(.{400})",?/ or next;
	unless (defined $line)
	{
		$line = 0;
		next;
		warn;
	}
	my @chars = split (//, $1);

	if ($line > 3 && $line < 20) {
		# glue each character in
		for (my $i = 0; $i < 16; $i++) {
			@result[$line - 4] .= join ("", @chars[($i * 25 + 12)..($i * 25 + 12 + $width - 1)]);
		}
	}

	# wrap aroud
	$line = 0 if (++$line > 24);
}

printf <<'EOF', length($result[0]), (scalar @result) * 3;
/* XPM */
static char *charset[] = {
/* columns rows colors chars-per-pixel */
"%i %i 2 1",
"  c #000000",
". c #FFFFFF",
/* pixels */
EOF

print join ",\n", (map { "\"$_\"" }
	@result,
	(map { scalar ("." . (" " x ($width - 1))) x 256 } @result),
	(map { " " x $width x 256 } @result),
);

print "};\n";
