#!C:/Perl/bin/perl

$PIXELMARK   = 'Pixel';
$DIR_FILES   = './files'; 
$PATH_SERVER = './files/server.txt'; 

AnalyzeForm();


( $sec, $min, $hour, $mday, $month, $year ) = localtime time; $year -= 100; ++$month;

# output log.

$PATH_LOG = sprintf( "%s/log%02d%02d%02d.txt", $DIR_FILES, $year, $month, $mday );
if(   open( hFile, "+<$PATH_LOG" ) ){ }
else{ open( hFile, ">$PATH_LOG"  ) or exit; }
eval{ flock( hFile, 2 ) };
seek( hFile, 0, 2 );
if( $FORM{'status'} ){ $str = "\t"."$FORM{'status'}";}
else               { $str = "";}
print hFile sprintf( "%02d%02d%02d\t%s%s\n", $hour, $min, $sec, $ENV{'REMOTE_ADDR'}, $str );
close hFile;

#status
#ip
#port
#protocol 通信バージョン 
#quota    定員
#comment

if( $FORM{'status'} )
{
	open( hFile, ">$PATH_SERVER"  ) or exit;
	eval{ flock( hFile, 2 ) };

	$d   = sprintf( "%02d%02d%02d-%02d%02d%02d", $year, $month, $mday, $hour, $min, $sec );
	$rec = "$d\t"."$FORM{'status'}\t"."$FORM{'ip'}\t"."$FORM{'port'}\t"."$FORM{'protocol'}\t"."$FORM{'version'}\t"."$FORM{'comment0'}\t"."$FORM{'comment1'}\t"."$FORM{'comment2'}\t"."$FORM{'comment3'}\t"."$FORM{'comment4'}\t"."$FORM{'comment5'}\t"."<br>\n";

	print hFile $rec ;
	close hFile;
}
else
{
	if( open( hFile, "<$PATH_SERVER"  ) ){ $rec = <hFile>; close hFile; }
	else{ $rec = "empty\t<br>\n"; }
}

Output();


# sub functions ----

sub AnalyzeForm
{
	my ( $query, $pair, $key, $value );

	if( $ENV{'REQUEST_METHOD'} eq 'POST' ){ read( STDIN, $query,  $ENV{'CONTENT_LENGTH'} ); }
	else								  {				 $query = $ENV{'QUERY_STRING'};		}

	foreach $pair ( split(/&/, $query) )
	{
		($key, $value) = split(/=/, $pair);
		$FORM{$key} = $value;
	}
}

sub Output
{
	print "Content-type: text/html\n\n";
	print "<html><body>\n";
	print $PIXELMARK."<br>\n";
	print $rec;
	print "</body></html>";
}
