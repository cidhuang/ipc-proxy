# ipc-proxy

Visual Studio 2008 does not support modern C++, so it is very difficult to develop new features with new libraries. This repo is for those suffering from the limitation of Visual Studio 2008.

Through IPC(Inter-process Communication) WM_COPYDATA, Visual Studio 2008 projects can communicate with the projects of latest Visual Studio, so an IPC Proxy built with latest Visual Studio can extend the ability of Visual Studio 2008 projects. This repo is a library for a simple IPC Proxy which provides fixed-length and mixed-length packets.

The files in the folder vs2008 are the common codes for both projects Visual Studio 2008 and lates Visual Studio. The files in the folder src are for IPC Proxy only. The packet definitions and handlers are not included and needed to be further defined.

In the folder example, Client in the folder vs2008 is a Visual Studio 2008 project, and it communicates with IpcProxy through IPC WM_COPYDATA. The files in the folder common are the packet definitions and handlers only for this example. IpcProxy is a Visual Studio 2022 project. It receives the request from Client, accesses Frankfurter open-source API for current and historical foreign exchange rates published by the European Central Bank (https://www.frankfurter.app/) through C++11 RESTful library cpp-httplib (https://github.com/yhirose/cpp-httplib) and then parse the result with the C++11 JSON library jsoncpp (https://github.com/open-source-parsers/jsoncpp), and then reply Client. Both Client and IpcProxy write log files.

