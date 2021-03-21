3/21/2021 Lab 11

https://q.utoronto.ca/courses/68725/assignments/1 13168 1 / 2

# Lab 11

```
Due Mar 29, 2019 by 6:30pm Points 1
```
# Lab 11: Select

```
Due : Friday 29 March before 6:30pm
```
# Introduction

```
The purpose of this lab is to practice using select to read from multiple inputs. This system call is used in assignment 4. Like last week, you are
welcome to reuse code from this lab in the assignment if it would be useful.
```
# Setup

```
Find the starter code in the Lab11 folder in your repository.
Like last week, you should change the port in the Makefile before doing anything else. Take the last four digits of your student number, and add a 5
in front. For example, if your student number is 998123456, your port would be 53456. Using this base port, you may add 1 to it as necessary in
order to use new ports (for example, the fictitious student here could also use 53457, 53458, 53459, 53460). Sometimes, when you shutdown your
server (e.g., to compile and run it again), the OS will not release the old port immediately, so you may have to cycle through ports a bit.
```
# An Echo System

```
Start by carefully reading through the starter code carefully making sure you understand what it already does.
Start with the chat_client.c code. This program creates a connection to a server. Then, it reads input from the user, sends it to the server, and waits
for a response which it displays back to the user on standard out. It strictly alternates these reads (from the keyboard and from the server).
What happens in chat_server.c? It accepts a connection from a client. Then, it waits for input from the user and then echos it (it sends what is
received right back). It keeps information about multiple clients in an array and does this echoing operation independently with each of them. In
other words, input from one particular client is echoed only to that same client - not to all of the others. The server uses select in a loop to process
input from whichever client is talking without waiting for others.
The goal of the lab is to create a functioning chat system where the messages sent by one client are delivered to all the other clients and
participants are not required to talk in some pre-defined artificial turns. To accomplish this, you will need to change both the client and the server.
```
# 1. Identify Yourself

```
Once we start broadcasting messages from the server, it will get confusing if we can’t identify who the message comes from. Your first task is to
modify the client so that the first thing it does is to ask the user for a username, to read the username from stdin, and to write that username to the
socket. The server should be modified so that it reads the username immediately after accepting the connection. The username should be added to
the table of struct socknames it is managing. Then, whenever the server echoes a message, it should prefix the message with Username:, where
“Username” is that connection’s name.
```
# 2. Start Echoing

```
This is a quick task. Change the read_from function so that it broadcasts any message received to all connected clients. Then, test by connecting
more than one client to the server and making sure that every client receives any message that the other clients send. It’ll be a bit awkward since
our clients are reading from stdin before reading from the socket. You’ll have to hit Enter on the client that didn’t send the message to see what
the other client has sent, and that will lead to the other clients receiving “empty” messages.
```
# Task 3: Monitor stdin and the Socket in the Client

```
Take a look at how the server uses select. In particular, look at the fd_set variables it is managing and how it checks which file descriptor is ready
for reading using FD_ISSET. Pay special attention to how the fdset used in the select call is reset for each new call. Make sure you understand
how the chat_server.c starter code works before you try to use select in the client.
Your task is to update the client so it monitors just two file descriptors: the socket with the server and stdin. Whenever a message is received on
either, your program should read it and pass it to the correct output stream.
```
# Submission

```
Submit your final chat_client.c and chat_server.c files to MarkUs under the Lab11 folder in your repository.
```

3/21/2021 Lab 1 1

https://q.utoronto.ca/courses/68725/assignments/1 13168 2 / 2

```
Congratulations! You’ve finished the last lab of the course. Good luck with the last assignment!
```


