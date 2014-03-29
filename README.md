# Readme
## What?
Command-execute is a plugin for Pidgin and Finch which lets you execute a command on either every new IM received or on every conversation update.
It can also act on new chat messages.

A conversation update is triggered everytime a message is received which you did not see yet. For example that applies for messages which occur in a window which is in the background of Finch.

You can pass the name of the sender and the received message by using $sender and $msg in the command. Before that you have to activate this feature in the configuration menu. This feature is untested and probably not secure. I wouldn't recommend using it at the moment.
## How?
Just put the `command-execute.so` file into your Pidgin/Finch plugin directory.
Normally this is `~/.purple/plugins/` or `/usr/lib/pidgin/`.

If this does not work for you, you maybe have to compile the plugin yourself:
    
    gcc command-execute.c -O2 -Wall -fpic `pkg-config --cflags glib-2.0` -I/path/to/your/libpurple/headers -shared -o command-execute.so
## Why?
I wrote this plugin because non of the existing command execution plugins worked for Pidgin _and_ Finch.

## TODO

- Provide .so files for every platform.
