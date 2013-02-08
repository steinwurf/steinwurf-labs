Running a C/C++ application from Visual Studio with WAF
=======================================================
This document describes the steps needed to build and run a C/C++ application with WAF from Visual Studio as a Makefile project.

Disclaimer
----------
These instructions have been tested for:
- Visual Studio 2010 C++ Express

Prerequisites
-------------------------
Visual Studio changes some of the environment variables, therefore waf must be configured from within Visual Studio.
Do not configure waf from the Command Prompt, otherwise the build process will fail due to an 'invalid lock file'. 

1. In Visual Studio, go to Tools->External Tools. Add a new tool called 'WAF CONFIGURE'.
Enter the following command:
::
 python waf configure --bundle=ALL --bundle-path="../deps" --options=cxx_mkspec=cxx_msvc11_x64
Set the Initial Directory to:
::
 $(SolutionDir)
