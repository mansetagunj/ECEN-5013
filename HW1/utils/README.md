# BuildLinuxKernel.sh

One should change the path to the log file, where ever one wants to place the log file.
BUILD_LOG_FILE=~/PATH/TO/FOLDER/WHERE/YOU/WANT/YOUR/LOG/FILE  
```
  $ ./BuildLinuxKernel.sh [options] [path_To_Linux_Source_Tree]  
  Options:  
  -h --help : help menu  
  e.g. ./BuildLinuxKernel.sh -h
  
  -f --file : to mention the path  
  e.g. ./BuildLinuxKernel.sh -f /path/to/linux/source/tree  
```  
  
  
# info.sh

Logs the current system information to the file provided as the input parameter with -f option  
  
```
  $ ./info.sh [options] [filename]    
  -h : Help    
  -f : File [filename] e.g. ./info.sh -f filename 
```   

# sshKeyGen.sh

Gives you a menu of functionality. 
1. Generates a SSH key
2. Displays the generated SSH public key.

This can be used for Github repos.
