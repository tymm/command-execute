/*
 * Command-execute
 * Copyright (C) 2010 tymm
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */


#define PURPLE_PLUGINS

#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "notify.h"
#include "plugin.h"
#include "version.h"
#include "debug.h"
#include "prefs.h"

#define PLUGIN_ID "core-tymm-command-execute"

#ifndef _PIDGIN_CONVERSATION_H_
typedef enum
{
	PIDGIN_UNSEEN_NONE,
	PIDGIN_UNSEEN_EVENT,
	PIDGIN_UNSEEN_NO_LOG,
	PIDGIN_UNSEEN_TEXT,
	PIDGIN_UNSEEN_NICK
} PidginUnseenState;
#endif

void execute(const char *cmd, char *sender, char *message) {
	if(strcmp(cmd,"") != 0) {
		pid_t pid = fork();
		if (pid == -1) {
		    // error, failed to fork()
		} else if (pid > 0) {
			purple_debug_info(PLUGIN_ID, "Command executed\n");
			purple_debug_info(PLUGIN_ID, cmd);
		} else {
			// we are the child
			execl(cmd,cmd,sender,message,(char *) NULL);
			purple_debug_warning(PLUGIN_ID, "There was a problem executing the command\n");
			_exit(EXIT_FAILURE);   // exec never returns
		}
	} else {
		// There is no command
		purple_debug_warning(PLUGIN_ID, "No command found\n");
	}
}


static void cmdexe_conversation_updated(PurpleConversation *conv, PurpleConvUpdateType type) {
	/* Check if the user wants to execute the command only when a conversation is updated */
	if(!purple_prefs_get_bool("/plugins/core/tymm-command-execute/execute_always")) {
		gboolean has_unseen_state=FALSE, has_unseen_count;
		if( type != PURPLE_CONV_UPDATE_UNSEEN) return;

		//has_unseen_state = (GPOINTER_TO_INT(purple_conversation_get_data(conv, "unseen-state")) >= PIDGIN_UNSEEN_TEXT);
		has_unseen_count = (GPOINTER_TO_INT(purple_conversation_get_data(conv, "unseen-count")) > 0);

		/* Check if the conversation_updated signal has been emitted by a new message or something else */
		if(has_unseen_state || has_unseen_count) {
			//const char *cmd = purple_prefs_get_string("/plugins/core/tymm-command-execute/command");
			//execute(cmd, sender, message);
		}
	}
}

static void cmdexe_received_im_msg(PurpleAccount *account, char *sender, char *message, PurpleConversation *conv, PurpleMessageFlags flags) {
	/* Check if the user wants to execute the command on _every_ received IM */
	if(purple_prefs_get_bool("/plugins/core/tymm-command-execute/execute_always")) {
		const char *cmd = purple_prefs_get_string("/plugins/core/tymm-command-execute/command");
		execute(cmd, sender, message);
	}
}

static void cmdexe_received_chat_msg(PurpleAccount *account, char *sender, char *message, PurpleConversation *conv, PurpleMessageFlags flags) {
	/* Check if the user wants to execute the command _everytime_ the user receives a chat message */
	if(purple_prefs_get_bool("/plugins/core/tymm-command-execute/execute_chat")) {
		const char *cmd = purple_prefs_get_string("/plugins/core/tymm-command-execute/command");
		execute(cmd, sender, message);
	}
}

static gboolean plugin_load(PurplePlugin *plugin) {
	/* Connect the conversation-updated signal to the callback function cmdexe_conversation_updated.
	 * The conversation-updated signal is emitted when a conversation is updated.
	 * I discovered that the conversation-updated signal is also triggered if the focus of a chat window is being triggered */
	purple_signal_connect(purple_conversations_get_handle(), "conversation-updated", plugin, PURPLE_CALLBACK(cmdexe_conversation_updated), NULL);

	/* Connect the received-im-msg signal to the callback function cmdexe_received_im_msg.
	 * This signal is emitted everytime an IM message is received */
	purple_signal_connect(purple_conversations_get_handle(), "received-im-msg", plugin, PURPLE_CALLBACK(cmdexe_received_im_msg), NULL);

	/* Connect the received-chat-msg signal to the callback function cmdexe_received_chat_msg.
	 * This signal is emitted everytime a Chat message is received */
	purple_signal_connect(purple_conversations_get_handle(), "received-chat-msg", plugin, PURPLE_CALLBACK(cmdexe_received_chat_msg), NULL);
	return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin) {
	/* Disconnect the signal if the plugin is getting unloaded */
	purple_signal_disconnect(purple_conversations_get_handle(), "conversation-updated", plugin, PURPLE_CALLBACK(cmdexe_conversation_updated));
	purple_signal_disconnect(purple_conversations_get_handle(), "received-im-msg", plugin, PURPLE_CALLBACK(cmdexe_received_im_msg));
	purple_signal_disconnect(purple_conversations_get_handle(), "received-chat-msg", plugin, PURPLE_CALLBACK(cmdexe_received_chat_msg));
	return TRUE;
}

/* UI */
static PurplePluginPrefFrame *plugin_config_frame(PurplePlugin *plugin) {
	PurplePluginPrefFrame *frame;
	PurplePluginPref *ppref;

	frame = purple_plugin_pref_frame_new();

	ppref = purple_plugin_pref_new_with_label("Insert Command:");
	purple_plugin_pref_frame_add(frame, ppref);

	ppref = purple_plugin_pref_new_with_name_and_label("/plugins/core/tymm-command-execute/command", "Command");
	purple_plugin_pref_frame_add(frame, ppref);

	ppref = purple_plugin_pref_new_with_name_and_label("/plugins/core/tymm-command-execute/execute_always", "Execute command on every new IM (not only on updated conversations)");
	purple_plugin_pref_frame_add(frame, ppref);

	ppref = purple_plugin_pref_new_with_name_and_label("/plugins/core/tymm-command-execute/execute_chat", "Execute command on new chat messages");
	purple_plugin_pref_frame_add(frame, ppref);

	ppref = purple_plugin_pref_new_with_name_and_label("/plugins/core/tymm-command-execute/arguments", "Enable arguments (%s sender, %m message)");
	purple_plugin_pref_frame_add(frame, ppref);

	return frame;
}

static PurplePluginUiInfo prefs_info = {
	plugin_config_frame,
	0,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,
	NULL,
	0,
	NULL,
	PURPLE_PRIORITY_DEFAULT,
	PLUGIN_ID,
	"Command execute",
	"1.1",
	"Command execution for pidgin and finch",
	"Takes a command which will be executed either on every new IM or on every conversation update. It can also act on new chat messages.",
	"tymm <tymmm1@gmail.com>",
	"https://github.com/tymm/command-execute/",
	plugin_load,
	plugin_unload,
	NULL, // plugin_destroy
	NULL,
	NULL,
	&prefs_info,
	NULL, // plugin_actions function here
	NULL,
	NULL,
	NULL,
	NULL
};

static void init_plugin(PurplePlugin *plugin) {
	purple_prefs_add_none("/plugins/core/tymm-command-execute");
	purple_prefs_add_string("/plugins/core/tymm-command-execute/command", "");
	purple_prefs_add_bool("/plugins/core/tymm-command-execute/execute_always", FALSE);
	purple_prefs_add_bool("/plugins/core/tymm-command-execute/execute_chat", FALSE);
	purple_prefs_add_bool("/plugins/core/tymm-command-execute/arguments", FALSE);
}

PURPLE_INIT_PLUGIN(command-execute, init_plugin, info)
