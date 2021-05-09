BEGIN {
	i=0;
	user[i];
	proc[i];
	RSSCount[i];
	maxPID=0;
	maxCommand="";
	maxVSZ=0;
	print "UID     ProcCount     RSSCount    "
}

{
	if(NR>1) {
		j = 0;
		for(; j <= i; j++) {
			if( $1 == user[j]) {
				proc[j] = proc[j] + 1;
				RSSCount[j] = RSSCount[j] + $9;
				break;
			}
		}
		if( j > i) {
			i++;
			user[i] = $1;
			proc[i] = 1;
			RSSCount[i] = $9;
		}	
		if(i == 0) {
			user[i] = $1;
			proc[i] = 1;
			RSSCount[i] = $9; 
		}

		if($8 > maxVSZ) {
			maxVSZ = $8;
			maxPID = $2;
			maxCommand = $NF;
	}
}
}
END {
	for(j = 0; j <= i; j++) {
		print user[j] "       " proc[j] "            " RSSCount[j];

	}
	print "-------------------------------------------------------------";

	print "maxVSZ: " maxVSZ"  PID: " maxPID "  Command: " maxCommand;
}

	

			
