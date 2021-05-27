BEGIN {
	maxSize = 0
	fileName = ""
	fileUser = ""
	fileAuthority = ""
	fileTime = ""
}	
{
	if ($1  !=  "total") {
		if ($5 > maxSize) {
			maxSize = $5
			fileName = $(NF)
			fileUser = $3
			fileAuthority = $1
			fileTime = $8
		}
	}
}
END {
	printf("MaxFileName: /home/%s\n", fileName) 
	printf("FileSize: %d\n", maxSize)
       	printf("FileUser: %s\n", fileUser)
	printf("FileAuthority: %s\n", fileAuthority)
	printf("FileTime: %s\n", fileTime)
	
}


