#!/bin/sh


entry(){

while [ "1" ]; do
	menu
done
}

menu(){
 
echo "	MENU
	1. Generate SSH KEYGEN
	2. Copy SSH KEYGEN to clipboard

	0. EXIT
	"

read option

if [ $option = "0" ]; then
	exit 0;

elif [ $option = "1" ]; then
	generateSSH		

elif [ $option = "2" ]; then
	copySSH
else 
	echo "Invalid Option"
	#exit 0;
	
fi

}


generateSSH(){

	echo "Enter your Github email id: "
	read emailId
	echo "Entered Email id is $emailId"
	ssh-keygen -t rsa -b 4096 -C $emailId
}

copySSH(){
	if [ -f xclip ]; then
		xclip < ~/.ssh/id_rsa.pub
		echo "The SSH Key is copied to clipboard"
	else
		#sudo apt-get install xclip
		#xclip < ~/.ssh/id_rsa.pub
		echo "Copy the key manually"
		echo ""
		cat /home/gunj/.ssh/id_rsa.pub
		echo ""
	fi
}

#calling the entry function
entry

