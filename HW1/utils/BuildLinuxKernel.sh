#!/bin/bash

NUM_OF_CPU=
LINUX_SOURCE_PATH=
BUILD_SUCCESS=0
KERNEL_VERSION=
DATE_TIME_NOW=$(date +"%m_%d_%Y_%H_%M_%S")
BUILD_LOG_FILE=~/repos/ECEN-5013/HW1/info_logs/build_log_$DATE_TIME_NOW.log


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
        	LINUX_SOURCE_PATH=$2
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
	
	if [ -z $LINUX_SOURCE_PATH ]; then
		echo "Enter the correct path to the Linux source code Dir."
		read -p "-> " path

		echo "Entered path: $LINUX_SOURCE_PATH"
	fi

	if [ -z $LINUX_SOURCE_PATH ]; then
		echo "No path entered"
		exit 1
	elif [ -d $LINUX_SOURCE_PATH ]; then
		echo "Contents of Dir:"
		cd $LINUX_SOURCE_PATH
		ls
		echo ""
		echo "Build Log can be found in $BUILD_LOG_FILE"
	elif [ -d $LINUX_SOURCE_PATH != 1]; then
		echo "The file $LINUX_SOURCE_PATH is not a directory."	
	else
		echo "Unknown Error"
		exit 1
	fi
}

startBuild(){

	#this is used to store the num of cores available
	NUM_OF_CPU=$(nproc)
	#multipying it with 2 as the number of threads per core can be 2
	NUM_OF_CPU=$(($NUM_OF_CPU * 2))
	
	#gives the kernelversion in the linux source tree
	KERNEL_VERSION=$(make -s kernelversion)

	timeStart=$(date +"%x %r %Z")
	echo "Starting Build of Kernel v$KERNEL_VERSION. Time: $timeStart" |tee -a $BUILD_LOG_FILE
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

	make -j $NUM_OF_CPU &>> $BUILD_LOG_FILE && make modules -j $NUM_OF_CPU &>> $BUILD_LOG_FILE && BUILD_SUCCESS=1
	
	END=$(date +%s);

	if [ $BUILD_SUCCESS != 1 ];then
		echo "Build Failed. Look for Build Log at: $BUILD_LOG_FILE"
		echo "Build Failed." >> $BUILD_LOG_FILE	
	fi
	echo $((END-START)) | awk '{print "Build Completed, took "int($1/60)" min and "int($1%60)" sec."}' |tee -a $BUILD_LOG_FILE

}

startInstall(){
	
	echo ""
	#the variable BUILD_SUCCESS is set to 1 only if the make commands for the kernel and modules build.
	if [ $BUILD_SUCCESS != 1 ]; then
		echo "Kernel did not Build in this transaction."
		echo "Do you still want to continue with the Install? [YyNn]"
		read -p "-> " continue
	fi

	if [ $continue != "Y" ] && [ $continue != "y" ]; then
		exit 0
	fi

	timeStart=$(date +"%x %r %Z")
        echo "Starting Install. Time: $timeStart" |tee -a $BUILD_LOG_FILE
        echo "Do you want to continue? [YyNn]"
        read -p "-> " ans

        if [ $ans = "N" ] || [ $ans = "n" ]; then
                #exit 0
		return
	fi
	
	START=$(date +%s);

	sudo make modules_install -j $NUM_OF_CPU &>> $BUILD_LOG_FILE && sudo make install -j $NUM_OF_CPU &>> $BUILD_LOG_FILE
	
	END=$(date +%s);
        echo $((END-START)) | awk '{print "Install Completed, took "int($1/60)" min and "int($1%60)" sec."}' |tee -a $BUILD_LOG_FILE
}

#gives the kernelversion in the linux source tree
#KERNEL_VERSION=$(make -s kernelversion)

createInitrdFile(){

	echo ""

	#creating the initrd file with the version $KERNEL_VERSION
	sudo update-initramfs -c -k $KERNEL_VERSION >> $BUILD_LOG_FILE 2>&1

	#updating the grub file to include this kernel in the grub menu which is done by changing the grub.cfg file using the below command
	sudo update-grub >> $BUILD_LOG_FILE 2>&1
}

#starting point of the bash script
entrypoint
