#!/bin/sh 

timeNow=$(date +"%x %r %Z")

#option for help
if [ $# -eq 1 ] && [ $1 = "-h" ]; then
	echo "
	$0 [options] [filename]
	-h : Help
	-f : File [filename] e.g. $0 -f filename
	"
	exit 1

#elif [ $# -eq 1 ] && [ $1 = "-o" ]; then
elif [ $# -eq 2 ] && [ $1 = "-f" ]; then
	filename=$2;

else
	echo "Invalid Parameters."
	echo "Try -h option for help."
	exit 1
fi

echo "Time: $timeNow" >> $filename

echo "User Information" >> $filename
cat /etc/passwd | grep gunj >> $filename

echo "" >> $filename
echo "OS type and Brand" >> $filename
uname -o >> $filename

echo "" >> $filename

echo "OS Distribution:" >> $filename
lsb_release -s -i >> $filename

echo "" >> $filename

echo "OS version:" >> $filename
lsb_release -s -r >> $filename

echo "" >> $filename

echo "Kernel Details:" >> $filename
#uname -r >> $filename
cat /proc/version >> $filename

echo "" >> $filename

echo "System Architecture information:" >> $filename
lscpu >> $filename

echo "" >> $filename

echo "Info on File System Memory" >> $filename
df -h >> $filename

echo "" >> $filename

exit 0
