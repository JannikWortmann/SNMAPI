

int snmapi_client_get_version(int socket)
{
	return snmapi_send(socket, (char*)SNMAPI_Lib_Version"\0", strlen((char*)SNMAPI_Lib_Version) + 1);
}

int snmapi_client_get_version_xml(int socket)
{
	char buff[150];
	sprintf(buff, 150, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<status>\n<version>%s</version>\n</status>", (char*)SNMAPI_Lib_Version);
	return snmapi_send(socket, buff, 150);
}

int snmapi_client_notify(int socket, snmapi_notify_struct* args)
{
	char* plugin = "system_plugin";
	char* tex = "";
	switch (args->texture)
	{
	case SNMAPI_Notify_Caution:
		tex = "tex_notification_caution";
		break;

	case SNMAPI_Notify_Friend:
		tex = "tex_notification_friend";
		break;

	case SNMAPI_Notify_Headset:
		tex = "tex_notification_headset";
		break;

	case SNMAPI_Notify_Info:
		tex = "tex_notification_info";
		break;

	case SNMAPI_Notify_Keyboard:
		tex = "tex_notification_keypad";
		break;

	case SNMAPI_Notify_Mediaserver:
		tex = "tex_notification_mediasever";
		break;

	case SNMAPI_Notify_Music:
		tex = "tex_notification_music";
		break;

	case SNMAPI_Notify_Error:
		tex = "tex_notification_psbutton_insensitive";
		break;

	case SNMAPI_Notify_Settings:
		tex = "tex_notification_settings";
		break;

	case SNMAPI_Notify_Trophy_Bronze:
		tex = "tex_notification_trophy_bronze";
		break;

	case SNMAPI_Notify_Trophy_Gold:
		tex = "tex_notification_trophy_gold";
		break;

	case SNMAPI_Notify_Trophy_Platinum:
		tex = "tex_notification_trophy_platinum";
		break;

	case SNMAPI_Notify_Trophy_Silver:
		tex = "tex_notification_trophy_silver";
		break;

	case SNMAPI_Notify_Arrow:
		tex = "tex_pointer_arrow";
		break;

	case SNMAPI_Notify_Pen:
		tex = "tex_pointer_pen";
		break;

	case SNMAPI_Notify_Hand:
		tex = "tex_pointer_hand";
		break;

	case SNMAPI_Notify_Grab:
		tex = "tex_pointer_grab";
		break;

	case SNMAPI_Notify_Arrow_Right:
		tex = "tex_arrow_right";
		break;

	case SNMAPI_Notify_PSN:
		tex = "tex_psn";
		plugin = "xmb_plugin";
		break;

	case SNMAPI_Notify_PSPlus:
		tex = "tex_indi_plus";
		plugin = "xmb_plugin";
		break;

	case SNMAPI_Notify_SignIn:
		tex = "tex_Signing_In";
		plugin = "explore_plugin";
		break;

	case SNMAPI_Notify_New:
		tex = "tex_new_ws";
		plugin = "explore_plugin";
		break;

	case SNMAPI_Notify_Check:
		tex = "tex_check_ws";
		plugin = "explore_plugin";
		break;

	case SNMAPI_Notify_Attention:
		tex = "tex_urgent_ws";
		plugin = "explore_plugin";
		break;

	case SNMAPI_Notify_Facebook:
		tex = "item_tex_cam_facebook";
		plugin = "explore_plugin";
		break;

	case SNMAPI_Notify_TrophyRank:
		tex = "item_tex_Profile_LevelIcon";
		plugin = "explore_plugin";
		break;

	case SNMAPI_Notify_PSNStore:
		tex = "item_tex_ps_store";
		plugin = "explore_plugin";
		break;

	default:
		tex = "tex_notification_info";
		break;
	}
	return snmapi_send_int(socket, snmapi_notify((char*)tex, (char*)plugin,(char*)args->message));
}

int snmapi_client_notify_xml(int socket, snmapi_notify_struct* args)
{
    char* plugin = "system_plugin";
    char* tex = "";
    switch (args->texture)
    {
        case SNMAPI_Notify_Caution:
            tex = "tex_notification_caution";
            break;

        case SNMAPI_Notify_Friend:
            tex = "tex_notification_friend";
            break;

        case SNMAPI_Notify_Headset:
            tex = "tex_notification_headset";
            break;

        case SNMAPI_Notify_Info:
            tex = "tex_notification_info";
            break;

        case SNMAPI_Notify_Keyboard:
            tex = "tex_notification_keypad";
            break;

        case SNMAPI_Notify_Mediaserver:
            tex = "tex_notification_mediasever";
            break;

        case SNMAPI_Notify_Music:
            tex = "tex_notification_music";
            break;

        case SNMAPI_Notify_Error:
            tex = "tex_notification_psbutton_insensitive";
            break;

        case SNMAPI_Notify_Settings:
            tex = "tex_notification_settings";
            break;

        case SNMAPI_Notify_Trophy_Bronze:
            tex = "tex_notification_trophy_bronze";
            break;

        case SNMAPI_Notify_Trophy_Gold:
            tex = "tex_notification_trophy_gold";
            break;

        case SNMAPI_Notify_Trophy_Platinum:
            tex = "tex_notification_trophy_platinum";
            break;

        case SNMAPI_Notify_Trophy_Silver:
            tex = "tex_notification_trophy_silver";
            break;

        case SNMAPI_Notify_Arrow:
            tex = "tex_pointer_arrow";
            break;

        case SNMAPI_Notify_Pen:
            tex = "tex_pointer_pen";
            break;

        case SNMAPI_Notify_Hand:
            tex = "tex_pointer_hand";
            break;

        case SNMAPI_Notify_Grab:
            tex = "tex_pointer_grab";
            break;

        case SNMAPI_Notify_Arrow_Right:
            tex = "tex_arrow_right";
            break;

        case SNMAPI_Notify_PSN:
            tex = "tex_psn";
            plugin = "xmb_plugin";
            break;

        case SNMAPI_Notify_PSPlus:
            tex = "tex_indi_plus";
            plugin = "xmb_plugin";
            break;

        case SNMAPI_Notify_SignIn:
            tex = "tex_Signing_In";
            plugin = "explore_plugin";
            break;

        case SNMAPI_Notify_New:
            tex = "tex_new_ws";
            plugin = "explore_plugin";
            break;

        case SNMAPI_Notify_Check:
            tex = "tex_check_ws";
            plugin = "explore_plugin";
            break;

        case SNMAPI_Notify_Attention:
            tex = "tex_urgent_ws";
            plugin = "explore_plugin";
            break;

        case SNMAPI_Notify_Facebook:
            tex = "item_tex_cam_facebook";
            plugin = "explore_plugin";
            break;

        case SNMAPI_Notify_TrophyRank:
            tex = "item_tex_Profile_LevelIcon";
            plugin = "explore_plugin";
            break;

        case SNMAPI_Notify_PSNStore:
            tex = "item_tex_ps_store";
            plugin = "explore_plugin";
            break;

        default:
            tex = "tex_notification_info";
            break;
    }

    char buff[150];
    sprintf(buff, 150, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<errorcode>\n<version>%i</errorcode>\n</status>", snmapi_notify((char*)tex, (char*)plugin,(char*)args->message));
    return snmapi_send(socket, buff, 150);
}

int snmapi_client_ring_buzzer(int socket, snmapi_ring_buzzer_struct* args)
{
    int ret = -1;
    switch (args->buzzermode)
	{
	case 0:
		//ring once
		ret = snmapi_ring_buzzer_once();
		break;

	case 1:
		//ring twice
		ret = snmapi_ring_buzzer_twice();
		break;

	case 2:
		//ring tripple
		ret = snmapi_ring_buzzer_tripple();
		break;
	}
	return snmapi_send_int(socket, ret);
}



int snmapi_client_shutdown(int socket, snmapi_shutdown_struct* args)
{
	switch (args->shutdown_flag)
	{
	case 0:
		//shutdown
		snmapi_send_int(socket, 0);
		snmapi_ps3_shutdown();
		break;

	case 1:
		//quick reboot
		snmapi_send_int(socket, 0);
		snmapi_ps3_quick_reboot();
		break;

	case 2:
		//soft reboot
		snmapi_send_int(socket, 0);
		snmapi_ps3_soft_reboot();
		break;

	case 3:
		//hard reboot
		snmapi_send_int(socket, 0);
		snmapi_ps3_hard_reboot();
		break;
	}
	return 0;
}

int snmapi_client_get_temperature(int socket, snmapi_get_temperature_struct* args)
{
	int tmp, ret = -1;
	ret = snmapi_get_temperature_celsius((int)args->kernel, &tmp);
	return snmapi_send_int(socket, tmp);
}

int snmapi_client_get_firmware(int socket)
{
	char fw[5];
	char type[5];
	char buffer[15];
	int ret = snmapi_get_firmware_string(fw);
	ret = snmapi_get_ps3_type_string(type);
	sprintf(buffer, "%s %s", fw, type);
	return snmapi_send(socket, buffer, 15);
}

int snmapi_client_play_sound(int socket, snmapi_play_sound_struct* args)
{
	switch(args->soundNum)
	{
		case 0:
			return snmapi_send_int(socket, snmapi_playSound((const char*)"snd_trophy", (const char*)"system_plugin"));

		case 1:
			return 0;
	}
	return -1;
}

int snmapi_client_peek_lv2(int socket, snmapi_peek_lv2_struct* args)
{
	uint64_t value = snmapi_peek_lv2_(args->address);
	value = reverse_long(value);
	return snmapi_send(socket, &value, sizeof(value));
}

int snmapi_client_poke_lv2(int socket, snmapi_poke_lv2_struct* args)
{
	return snmapi_send_int(socket, snmapi_poke_lv2(args->address, args->value));
}

int snmapi_client_set_consoleid(int socket, snmapi_set_consoleid_struct* args)
{
	return snmapi_send_int(socket, snmapi_set_cid(args->cid));
}

int snmapi_client_get_memory(int socket, snmapi_get_memory_struct* args)
{
	char* buffer = snmapi_malloc(args->length);
	int ret = snmapi_getmemory(args->processID, args->address, args->length, buffer);
	ret = snmapi_send(socket, buffer, args->length);
	snmapi_free(buffer);
	return ret;
}

int snmapi_client_set_memory(int socket, snmapi_set_memory_struct* args)
{
	int ret = snmapi_setmemory(args->processID, args->address, args->length, args->buffer);
	return snmapi_send_int(socket, ret);
}

int snmapi_client_get_processes(int socket)
{
	uint32_t* buffer = (uint32_t*)snmapi_malloc(4 * 16);
	snmapi_getprocesslist((process_id_t*)buffer);
	int i;
	for (i = 0; i < 16; i++)
		buffer[i] = byte_reverse_32(buffer[i]);
	int ret = snmapi_send(socket, buffer, 64);
	snmapi_free(buffer);
	return ret;
}

int snmapi_client_get_processname(int socket, snmapi_get_processname_struct* args)
{
	char* buffer = snmapi_malloc(MAX_CHAR_LENGTH);
	int ret;
	snmapi_getprocessname(args->processID, buffer);
	if (strlen(buffer) == 0)
	{
		char* n = "NULL";
		int x = strlen(n);
		strncpy(buffer, n, x);
		buffer[x] = 0;
		ret = snmapi_send(socket, buffer, strlen(buffer) + 1);
	}
	else
	{
		int n = strlen(buffer);
		buffer[n] = 0;
		ret = snmapi_send(socket, buffer, n + 1);
	}
	snmapi_free(buffer);
	return ret;
}

int snmapi_client_load_sprx(int socket, snmapi_load_sprx_struct* args)
{
	sys_prx_id_t prxID;
	snmapi_loadsprx(args->processID, args->path, &prxID);
	return snmapi_send_int(socket, prxID);
}

int snmapi_client_unload_sprx(int socket, snmapi_unload_sprx_struct* args)
{
	return snmapi_send_int(socket, snmapi_unloadsprx(args->processID, args->prxID));
}

int snmapi_client_get_game_process(int socket)
{
	process_id_t* buff = (process_id_t*)snmapi_malloc(64);
	snmapi_getprocesslist(buff);
	process_id_t pid = buff[2];
	snmapi_free(buff);
	return snmapi_send_int(socket, pid);
}

int snmapi_client_get_free_memory(int socket)
{
	memoryInfo info;
	snmapi_get_free_memory(&info);
	return snmapi_send_uint(socket, (uint32_t)info.available>>10);//from bytes to KBytes :2^10
}

int snmapi_client_notify_small(int socket, snmapi_notify_small_struct* args)
{
	return snmapi_send_int(socket, snmapi_display_message(args->msg));
}

int snmapi_client_get_free_space(int socket)
{
	uint32_t blocksize;
	uint64_t size;
	cellFsGetFreeSize((char*)"/dev_hdd0", &blocksize, &size);
	return snmapi_send_int(socket, (int)((blocksize*size)>>20));//from bytes to MBytes : 2^10 * 2^10
}

int snmapi_client_get_connection_type(int socket)
{
	net_info info;
	memset(&info, 0, sizeof(net_info));
	xsetting_F48C0548()->sub_44A47C(&info);
	return snmapi_send_int(socket, info.device);//0 = LAN; 1 = WLAN
}
