# Readme
## What?
Command-execute is a plugin for Pidgin and Finch which lets you execute a command on either every new IM received or on every conversation update.
It can also act on new chat messages.

A conversation update is triggered everytime a message is received which you did not see yet. For example that applies for messages which occur in a window which is in the background of Finch.
## How?
Just put the `command-execute.so` file into your Pidgin/Finch plugin directory.
Normally this is `~/.purple/plugins/` or `/usr/lib/pidgin/`.

If this does not work for you, you maybe have to compile the plugin yourself:
    
    gcc command-execute.c -O2 -Wall -fpic `pkg-config --cflags glib-2.0` -I/path/to/your/libpurple/headers -shared -o command-execute.so

## TODO

- Provide .so files for every platform.

- Provide a feature which makes passing arguments like $msg for message or $sender for the id of the sender possible. This is already implemented in the develop branch. I wouldn't recommend using it though since I hadn't time yet to test it and make sure the feature doesn't raise security vulnerabilities.
