
# CPP CMake NamedPipe Assignment

- [Introduction](#introduction)
- [Requirements](#requirements)
- [Required specifications](#required-specifications)
- [The Project](#the-project)
  - [The Goal](#the-goal)
    - [Requests and Storage](#requests-and-storage)
  - [Application structure](#application-structure)
    -[CommonLib](#commonlib)
    -[Client](#client)
    -[Server](#server)
    -[Implementation changes on Client](#implementation-changes-on-client)
  - [Source Code](#source-code)

  

## Introduction

CPP Cmake NamedPipe Assignment created to build a client/server desktop application targetted for Windows platform.

Using CMake we create:

* ServerApp - A NamedPipe, multithreaded server application

* ClientApp - A NamedPipe, threaded client application

* CommonLib - A library common to ServerApp and ClientApp with some util functions and classes.

* nlohmann_json - A third party Json library.

  

The basis of the [ServerApp](https://docs.microsoft.com/en-us/windows/desktop/ipc/multithreaded-pipe-server) and [ClientApp](https://docs.microsoft.com/en-us/windows/desktop/ipc/transactions-on-named-pipes) are from [Microsoft's Named Pipe documentation](https://docs.microsoft.com/en-us/windows/desktop/ipc/pipes).

  

[Niels Lohmann's Json Library](https://github.com/nlohmann/json) is under MIT License.

  

## Requirements

The basic requirements for this project are:

* CMake v3.8+

* A Visual Studio compiler

  

## Required specifications

1. The client should be able to connect to the server through a NamedPipe

2. The client should be able to make both sync and async calls to the server

3. The client should be able to send trivial data (strings, numbers) to the server

4. The client should be able to create objects on the server (based on req-7 below), retrieve them, their attributes and call methods on them

5. The server should be able to receive both sync/async connection requests from clients

6. The server should be able to store data provided by the client via NamedPipe in a reasonable data structure

7. The server should be able to register a custom class (w/ related functions, attributes) which can be used by the client (see req-4)

8. The server should be able to store the custom objects created by the client for the custom class created in req-7

  

## The project

Following the specifications we created a CMake Project for a server/client application using Named Pipes.

  

### The Goal

#### Requests and Storage

The goal of our application is to be simple and efficient. From our Web Development knowledge, we believe that when talking about simple and efficient servers, one type of service that comes to mind is that of [RESTful](https://searchmicroservices.techtarget.com/definition/REST-representational-state-transfer) applications, in simple terms we will make a stateless server, that will treat each request as it's own contained task.

Regarding storage, we decided to have a json file to store our server data, so it persists between sessions.

  

### Application structure

After researching around, mostly at Microsoft's documentation, about how to use Named Pipes and the usual server and client applications. We decided to take the following approach:

#### CommonLib

Create a Common Library for both Client and Server applications to have some util functions and to store all the possible commands(requests).
Also contain a Address and a UserProfile classes.

Address has Street, City, Country and Postal Code as attributes, all std::strings.

UserProfile has a std::string Name, an int Age, an Address UserAddress and a std::size_t Id.

These classes will be used as the custom object requirement from the task.

We created the following commands, which can be called by the command name or index.

0. **AsyncHello** - A simple Asynchronous command sent. Receives a simple string as response, this is string is changed once the user sends his name.
1. **SyncedHello** - A simple Synchronous command sent. Receives a simple string as response, this is string is changed once the user sends his name.
2. **SendName** - An Asynchronous command sent with an string arg. Receives a response that contains his name.
3. **SendAge** - A Synchronous command sent with an integer arg. Receives a response that contains his age.
4. **CreateUserProfile** - An Asynchronous command that asks the server to create a UserProfile, given all it's parameters. Receives the user in a JSON formated matter as a response.
5. **UserBirthday** - An Asynchronous command that asks the server to run a function on a given UserProfile, UserProfile is selected by it's ID. Receives a response with the new user's age.
6. **FindUserProfile** - An Asynchronous command that asks the server to find a UserProfile by part of his name(Case sensitive). Receives a list of all UserProfiles with the sent part contained in their name.
7. **GetUserProfile** - A Synchronous command that asks the server to return all data of a given UserProfile. Receives the user desired in a JSON formated matter as a response. 

  

#### Client

```
Client Main Loop
    Wait for user's command
    IF Command is "Exit"
        Close program
    ELSE
        IF command has arguments
            Wait for user's arguments
        // Define Message as command appended with it's arguments separated by '@'
        Switch on user's command
            // Each command will handle the things differently
            // And some are Synchronous, others are Asynchronous
            IF command is Synchronous
                Connect Pipe
                Send Message
                Wait for response
                Deal with response
                Close Pipe
            ELSE
                Connect Pipe
                Send Message
                Create Thread
                    Wait for response
                    Deal with response
                    Close Pipe
```

  

#### Server

```
Server Main Loop
    While program isn't terminated
        Create a Named Pipe Connection
        Wait for a client to connect (blocks program)
        When client connected
            Create thread to deal with client

Thread Loop
    Read clent's request //composed of a command plus a series of arguments separated by '@'
    If command exists
        Deal with command
        Send response
    Disconnect Pipe
    Close thread
```

  

#### Implementation changes on Client

While implementating I realized that I could create a synchronous call (SyncedMessage) to the server using CallNamedPipe, which automatically handles CreateFile, WaitNamedpipe, TransactNamedPipe and CloseHandle in one single call, so instead of making at least 4 calls we do one to CallNamedPipe passing our parameters, like a timeout of 20 seconds, and it handles everything for us.

Once we had a synchronous call, we decided to make the ASynchronous call be a thread that calls our SyncedMessage function. Since it's a thread it will run independently of our client main loop and it won't have problems of handling with Asynchronous Server I/O, because it's a synchronous call to the essence.

  

### Source Code

```
.
├── CMakeLists.txt
├── .gitignore
├── README.md
├── Binaries
│   ├── bin
│   |   ├── ClientApp.exe
│   │   ├── Common.lib
│   │   └── ServerApp.exe
│   ├── include
│   └── lib
├── ClientApp
│   ├── CMakeLists.txt
│   ├── include
│   └── src
│       └── main.cpp
├── Common
│   ├── CMakeLists.txt
│   ├── include
│   |   └── common
│   |       ├── address.h
│   |       ├── common.h
│   |       └── userProfile.h
│   └── src
│       ├── address.cpp
│       ├── common.cpp
│       └── userProfile.cpp
├── Server
│   ├── CMakeLists.txt
│   ├── include
│   └── src
│       └── main.cpp
```