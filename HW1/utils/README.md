# BuildLinuxKernel.sh

### Before using the BuildLinuxKernel.sh script. ###
All the dependencies required for the kernel build should be installed correctly beforehand.
This can be done by: 
```
sudo apt-get update
sudo apt-get install build-essential kernel-package fakeroot libncurses5-dev libssl-dev ccache
```
One can clone the Linux source code tree from Linus Trovalds github:
```
#Creating a folder to clone the Linux repo
git clone https://github.com/torvalds/linux.git

#one can fetch a stable release by these commands
#to fetch the v14.4 stable release on a different branch
git fetch --all --tags
git checkout tags/v4.14 -b linux_v4.14

```

To purge the previous kernels one can use
```
sudo apt-get purge
```
This keeps the latest three kernels. This can be override using `--keep n` option
However, if you want to purge a specific kernel, one can use
```
sudo apt-get purge [linux-kernel-image-name]
```
If the above commands doesnot work, one can try using
```
sudo rm -rf /boot/*[kernel-image-version]* { e.g. sudo rm -rf /boot/*4.14.0* }
sudo apt-get -f install
sudo apt-get autoremove
sudo update-grub
sudo apt-get update
```
### For using the BuildLinuxKernel.sh
One should change the path to the log file, where ever one wants to place the log file. The log file used the current date and time in its name.
BUILD_LOG_FILE=~/PATH/TO/FOLDER/WHERE/YOU/WANT/YOUR/LOG/FILE  
```
  $ ./BuildLinuxKernel.sh [options] [path_To_Linux_Source_Tree]  
  Options:  
  -h --help : help menu  
  e.g. ./BuildLinuxKernel.sh -h
  
  -f --file : to mention the path  
  e.g. ./BuildLinuxKernel.sh -f /path/to/linux/source/tree  
```  
Example of the [Kernel Build Log](https://github.com/mansetagunj/ECEN-5013/blob/master/HW1/info_logs/build_log_01_24_2018_20_40_46.log) output.
  
# info.sh

Logs the current system information to the file provided as the input parameter with -f option  
  
```
  $ ./info.sh [options] [filename]    
  -h : Help    
  -f : File [filename] e.g. ./info.sh -f filename 
```  
Example of the [Output log](https://github.com/mansetagunj/ECEN-5013/blob/master/HW1/info_logs/new_sys_info.txt) for info.sh script

# sshKeyGen.sh

Gives you a menu of functionality. 
1. Generates a SSH key
2. Displays the generated SSH public key.

This can be used for Github repos.
