1. The virtual machine manager I used was virtual box. I did not really consult any tutorials. I had a friend help me with some of the setup, and I was familiar with setting up the virtual machine from previous classes. I had an issue where it said I had an inaccessible disk. I fixed this by seeing what disk it was using and realizing it was using an older version of ubuntu that I had installed. 
uname output - Linux aj-VirtualBox 5.15.0-46-generic #49-Ubuntu SMP Thu Aug 4 18:03:25 UTC 2022 x86_64 x86_64 x86_64 GNU/Linux
3.
strncpy -> #include <string.h>
printf -> #include <stdio.h>
exit(); -> #include <stdlib.h>
4. The reason for the error is a segmentation fault occuring with memory inside the program. I believe that strncpy copies more than the requried memory space. It could also be an issue with the pointer where buffer pointer is not big enough to hold the entire copied string. 
