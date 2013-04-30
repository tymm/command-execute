# Readme
## What?
Command-execute is a plugin for Pidgin and Finch which lets you execute a command on either every new IM received or on every conversation update.

A conversation update is triggered everytime a message is received which you did not see yet. For example that applies for messages which occur in a window which is in the background in Finch.
## How?
Just put the command-execute.so into your Pidgin/Finch plugins folder.
Normally this is `~/.purple/plugins/`.

If this does not work for you, you maybe have to compile the plugin yourself:

`gcc command-execute.c -O2 -Wall -fpic `pkg-config --cflags glib-2.0` -I/path/to/your/libpurple/headers -shared -o command-execute.so`
## Why?
I wrote this plugin because non of the existing command execution plugins worked for Pidgin _and_ Finch.
