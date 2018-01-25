#!/bin/bash

numOfCPU=
path=
buildSuccess=0
kernelVersion=

if [ $# -gt 0 ]; then
	if [ $# -lt 2 ] && [ $1 = "-h" ] || [ $1 = "--help" ] ; then
	cat <<- _EOF_
	$0 [options] [dir path]
        Options:
        -h --help : help menu
        -f --file : to mention the path
        e.g. $0 [-f][--file] [dir path]
	_EOF_
	
	exit 0
        
	elif [ $# -eq 2 ] && [ $1 = "--file" ]  || [ $1 =  "-f"  ]; then
        	path=$2
        else
		echo "Invalid Option. Try -h or --help option for details."
		exit 0               
        fi
fi


entrypoint(){
	
	echo "The current kernel version: $(uname -r)"
	goToPath	
	startBuild
	startInstall
	createInitrdFile
	
	echo "Do you want to reboot your system to boot into the new kernel? [YyNn]"
	read -p "-> " ans

	if [ $ans = "Yes" ] || [ $ans = "yes" ] || [ $ans = "Y" ] || [ $ans = "y" ]; then
		echo "REBOOTING........"
		reboot
	else
		echo "EXIT"
	fi
	
	exit 0
}


goToPath(){
	
	if [ -z $path ]; then
		echo "Enter the correct path to the Linux source code Dir."
		read -p "-> " path

		echo "Entered path: $path"
	fi

	if [ -z $path ]; then
		echo "No path entered"
		exit 1
	elif [ -d $path ]; then
		echo "Contents of Dir:"
		cd $path
		ls
		echo ""
	elif [ -d $path != 1]; then
		echo "The file $path is not a directory."	
	else
		echo "Unknown Error"
		exit 1
	fi
}

startBuild(){

	#this is used to store the num of cores available
	numOfCPU=$(nproc)
	#multipying it with 2 as the number of threads per core can be 2
	numOfCPU=$(($numOfCPU * 2))
	
	#gives the kernelversion in the linux source tree
	kernelVersion=$(make -s kernelversion)

	timeStart=$(date +"%x %r %Z")
	echo "Starting Build of Kernel v$kernelVersion. Time: $timeStart"
	echo "Do you want to continue? [YyNn]"
	read -p "-> " ans
	if [ $ans = "N" ] || [ $ans = "n" ]; then
		return
	fi

	#copy the current config file to the linux kernel build tree
        cp /boot/config-$(uname -r) .config
        #this gives a ui to alter i.e. add or remove kernel modules
        make menuconfig


	START=$(date +%s);

	make -j $numOfCPU && make modules -j $numOfCPU && buildSuccess=1
	
	END=$(date +%s);
	echo $((END-START)) | awk '{print "Build Completed, took "int($1/60)" min and "int($1%60)" sec."}'

}

startInstall(){
	
	echo ""
	#echo "Build done? $buildSuccess"
	#the varible buildSuccess is set to 1 only if the make commands for the kernel and modules build.
	if [ $buildSuccess != 1 ]; then
		echo "Kernel did not Build in this transaction."
		echo "Do you still want to continue with the Install? [YyNn]"
		read -p "-> " continue
	fi

	if [ $continue != "Y" ] && [ $continue != "y" ]; then
		exit 0
	fi

	timeStart=$(date +"%x %r %Z")
        echo "Starting Install. Time: $timeStart"
        echo "Do you want to continue? [YyNn]"
        read -p "-> " ans

        if [ $ans = "N" ] || [ $ans = "n" ]; then
                #exit 0
		return
	fi
	
	START=$(date +%s);

	sudo make modules_install -j $numOfCPU && sudo make install -j $numOfCPU
	
	END=$(date +%si);
        echo $((END-START)) | awk '{print "Install Completed, took "int($1/60)" min and "int($1%60)" sec."}'
}

#gives the kernelversion in the linux source tree
#kernelVersion=$(make -s kernelversion)

createInitrdFile(){

	echo ""

	#creating the initrd file with the version $kernelVersion
	sudo update-initramfs -c -k $kernelVersion

	#updating the grub file to include this kernel in the grub menu which is done by changing the grub.cfg file using the below command
	sudo update-grub
}

#starting point of the bash script
entrypoint
