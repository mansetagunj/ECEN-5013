#!/bin/sh -x 

numOfCPU=
path=
buildSuccess=0

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
	
	echo "Do you want to reboot your system to boot into the new kernel? [Yy]"
	read ans

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
		read path

		echo "Entered path: $path"
	fi

	if [ -z $path ]; then
		echo "Path is empty"
		exit 1
	elif [ -d $path ]; then
		cd $path
		ls
		echo ""
	elif [ -d $path != 1]; then
		echo "The file is not a directory."	
	else
		echo "Unknown Error"
		exit 1
	fi
}

startBuild(){

	cp /boot/config-$(uname -r) .config
	make menuconfig

	numOfCPU=$(nproc)
	
	timeStart=$(date +"%x %r %Z")
	echo "Starting Build. Time: $timeStart"
	echo "Do you want to continue? [YyNn]"
	read ans
	if [ $ans = "N" ] || [ $ans = "n" ]; then
		#exit 0
		return
	fi

	START=$(date +%s);

	make -j $numOfCPU && make modules -j $numOfCPU
set +x
	echo "Build Suc: $buildSucess"
#	sleep 5
#	make modules -j $numOfCPU
	
	END=$(date +%s);
	echo $((END-START)) | awk '{print "Build Completed, took "int($1/60)" min and "int($1%60)" sec."}'

}

startInstall(){
	
	echo ""
	#echo "Build done? $buildSuccess"
	#if [ $buildSuccess != 1 ]; then
	#	echo "Kernel did not Build in this transaction. Cannot Install."
	#	echo "Do you still want to continue? [YyNn]"
	#	read continue
	#fi

	#if [ $continue != "Y" ] || [ $continue != "y" ]; then
	#	exit 0
	#fi

	timeStart=$(date +"%x %r %Z")
        echo "Starting Install. Time: $timeStart"
        echo "Do you want to continue? [YyNn]"
        #read ans
	ans="Y"
        if [ $ans = "N" ] || [ $ans = "n" ]; then
                #exit 0
		return
	fi
	
	START=$(date +%s);

	sudo make modules_install -j $numOfCPU && sudo make install -j $numOfCPU
#	sleep 3
	sudo make install -j $numOfCPU
	
	END=$(date +%si);
        echo $((END-START)) | awk '{print "Install Completed, took "int($1/60)" min and "int($1%60)" sec."}'
}

kernelVersion=4.14.0

createInitrdFile(){

	echo ""

	#creating the initrd file with the version $kernelVersion
	sudo update-initramfs -c -k $kernelVersion

	#updating the grub file to include this kernel in the grub menu which is done by changing the grub.cfg file using the below command
	#
	sudo update-grub
}


entrypoint
