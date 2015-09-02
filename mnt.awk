BEGIN{ 
	FS="@@##@@";  t=b=0;
} 
END {
	if(!t)
	print "" > "pp.mnt"; 
	if(!b)
	print "" > "pp.mnb"; 
  } 
NF && length($0)>3 { 
	gsub(/","/,"@@##@@"); gsub(/mm@@##@@/,"@@##@@"); 
	gsub(/,/,"."); gsub(/[ \t][ \t]*/,"_"); 
	sub(/^"/,""); sub(/\r$/,""); sub(/"$/,""); 
	$0=$0;
	if($9=="T") { t++; printf("%s %3.3f %3.3f %d %s %s\n",$1,$3,$4,$10,$11,$2) > "pp.mnt" ; next; }
	if($9=="B") { b++; printf("%s %3.3f %3.3f %d %s %s\n",$1,$3,$4,$10,$11,$2) > "pp.mnb" ; next; }
	if($9=="T") { print $1,$3,$4,$10,$11,$2 > "pp.mnt" ; next; }
	if($9=="B") { print $1,$3,$4,$10,$11,$2 > "pp.mnb"; next; }
  }
/./
