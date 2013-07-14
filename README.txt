Fluid Simulator
Matthew Jee
mcjee@ucsc.edu

Platforms:
    Mac OS X only. Tested on OS X 10.8.3, 10.8.4, and 10.9.
    Executables are in the bin directory.

Build Instructions:

    TLDR: run `make` in the code directory.

    Xcode 4 and the developer command line tools must be installed.

    NOTE:
    This builds perfectly on all Macs I have tested it on, but for
    some strange reason the lab iMacs crash due to improper memory
    access 4 out of 5 times that I try to run it. When it does run,
    though, it runs perfectly. I am compiling to ansi standard with
    all errors an warnings, and I even have -pedantic on. I don't know
    what the issue is. I believe that the issue may be differences
    between drivers for Nvidia and AMD GPUs (the labs have AMD, the
    laptops I have tested on use Nvidia).
