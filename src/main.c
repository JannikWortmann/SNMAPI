#include <sdk_version.h>
#include <cellstatus.h>
#include <cell/cell_fs.h>
#include <cell/rtc.h>
#include <cell/gcm.h>
#include <cell/pad.h>
#include <sys/vm.h>
#include <sysutil/sysutil_common.h>
#include <float.h>
#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/event.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/memory.h>
#include <sys/timer.h>
#include <sys/process.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <errno.h>

#include "ftp.h"
#include "SNMAPI_Lib.h"
#include "printf.h"

int SNMAPI_MAIN(uint64_t args);
int SNMAPI_STOP(void);

SYS_MODULE_INFO(SNMAPI, 0, 1, 0);
SYS_MODULE_START(SNMAPI_MAIN);
SYS_MODULE_STOP(SNMAPI_STOP);

sys_ppu_thread_t _mainThread = 0, _ftpThread = 0, _udpThread = 0;

static inline void _sys_ppu_thread_exit(uint64_t val)
{
	system_call_1(41, val);
}

static inline sys_prx_id_t prx_get_module_id_by_address(void *addr)
{
	system_call_1(461, (uint64_t)(uint32_t)addr);
	return (int32_t)p1;
}

static void finalize_module(void)
{
	uint64_t meminfo[5];

	sys_prx_id_t prx = prx_get_module_id_by_address(finalize_module);

	meminfo[0] = 0x28;
	meminfo[1] = 2;
	meminfo[3] = 0;

	system_call_3(483, prx, 0, NULL);
}

static void SNMAPI_Stop_thread(uint64_t arg)
{
	uint64_t exit = 0;
	if (_ftpThread != (sys_ppu_thread_t)0)
		sys_ppu_thread_join(_ftpThread, &exit);
	if (_udpThread != (sys_ppu_thread_t)0)
		sys_ppu_thread_join(_udpThread, &exit);
	if (_mainThread != (sys_ppu_thread_t)0)
		sys_ppu_thread_join(_mainThread, &exit);
	sys_ppu_thread_exit(0);
}

int SNMAPI_STOP(void)
{
	uint64_t exitcode;
	sys_ppu_thread_t t;
	sys_ppu_thread_create(&t, SNMAPI_Stop_thread, 0, 0, 0x2000, 1, "SNMAPI_Stop_Thread");
	sys_ppu_thread_join(t, &exitcode);
	finalize_module();

	_sys_ppu_thread_exit(0);
	return SYS_PRX_STOP_OK;
}

static void Client_Thread(uint64_t _sock)
{
	int sock = (int)_sock;

	sys_timer_sleep(2);

	char buffer[2048];

	uint8_t* data_ptr = (uint8_t*)buffer + 1;

	while (snmapi_recv(sock, buffer, 2048) == SUCCESS)
	{
		switch (*(uint8_t*)(buffer))
		{
		case SNMAPI_GetVersion:
			snmapi_client_get_version(sock);
			break;

		case SNMAPI_Notify:
			snmapi_client_notify(sock, (snmapi_notify_struct*)data_ptr);
			break;

		case SNMAPI_RingBuzzer:
			snmapi_client_ring_buzzer(sock, (snmapi_ring_buzzer_struct*)data_ptr);
			break;

		case SNMAPI_GetTemp:
			snmapi_client_get_temperature(sock, (snmapi_get_temperature_struct*)data_ptr);
			break;

		case SNMAPI_Shutdown:
			snmapi_client_shutdown(sock, (snmapi_shutdown_struct*)data_ptr);
			break;

		case SNMAPI_GetFirmware:
			snmapi_client_get_firmware(sock);
			break;

		case SNMAPI_PlaySound:
			snmapi_client_play_sound(sock, (snmapi_play_sound_struct*)data_ptr);
			break;

		/*case SNMAPI_PeekLV2:
			snmapi_client_peek_lv2(sock, (snmapi_peek_lv2_struct*)data_ptr);
			break;

		case SNMAPI_PokeLV2:
			snmapi_client_poke_lv2(sock, (snmapi_poke_lv2_struct*)data_ptr);
			break;*/

		case SNMAPI_SetCID:
			snmapi_client_set_consoleid(sock, (snmapi_set_consoleid_struct*)data_ptr);
			break;

		case SNMAPI_GetMemory:
			snmapi_client_get_memory(sock, (snmapi_get_memory_struct*)data_ptr);
			break;

		case SNMAPI_SetMemory:
			snmapi_client_set_memory(sock, (snmapi_set_memory_struct*)data_ptr);
			break;

		case SNMAPI_GetProcesses:
			snmapi_client_get_processes(sock);
			break;

		case SNMAPI_GetProcName:
			snmapi_client_get_processname(sock, (snmapi_get_processname_struct*)data_ptr);
			break;

		case SNMAPI_LoadSPRX:
			snmapi_client_load_sprx(sock, (snmapi_load_sprx_struct*)data_ptr);
			snmapi_notify_char("SPRX loaded !");
			break;

		/*case SNMAPI_UnloadSPRX:
			snmapi_client_unload_sprx(sock, (snmapi_unload_sprx_struct*)data_ptr);
			break;*/

		case SNMAPI_GetGamePID:
			snmapi_client_get_game_process(sock);
			break;

		case SNMAPI_GetFreeMemory:
			snmapi_client_get_free_memory(sock);
			break;

		case SNMAPI_NotifySmall:
			snmapi_client_notify_small(sock, (snmapi_notify_small_struct*)data_ptr);
			break;

		case SNMAPI_GetFreeSpace:
			snmapi_client_get_free_space(sock);
			break;

		case SNMAPI_GetConType:
				snmapi_client_get_connection_type(sock);
			break;

		/*case SNMAPI_Handle_Game_CMD:
		snmapi_notify_char("Break 1");
			snmapi_client_handle_game_cmd(sock, (snmapi_handle_game_cmd_struct*)data_ptr);
			break;

		case Make_XML(SNMAPI_GetVersion):
				snmapi_client_get_version_xml(sock);
			break;*/
		}
		sys_timer_usleep(5);
		sys_ppu_thread_yield();
	}
	//snmapi_notify_char("Client Thread ended!");
	shutdown(sock, SHUT_RDWR);
	socketclose(sock);
	sys_ppu_thread_exit(0);
}

static void Global_Thread(uint64_t args)
{
	sys_timer_sleep(20);

	while (!snmapi_findPlugin("explore_plugin"))
		sys_timer_usleep(500);
	sys_timer_sleep(3);

	if (args == SNMAPI_PAYLOAD_NOT_FOUND)
	{
		snmapi_notify("tex_notification_psbutton_insensitive", "system_plugin", "SNMAPI ERROR. Code: 0x1");
		return;
	}

	snmapi_notify("tex_urgent_ws", "explore_plugin", "SNMAPI v4.81 loaded!");
	sys_timer_sleep(1);
	snmapi_playSound("snd_trophy", "system_plugin");
	sys_timer_sleep(1);
	snmapi_notify("tex_notification_mediasever", "system_plugin", "FTP Server started!");

	int list_sMain, conn_TCP;
	list_sMain = slisten(SNMAPI_Port, 2);

	if (list_sMain)
	{
		for (;;)
		{
			if ((conn_TCP = accept(list_sMain, NULL, NULL)) > 0)
			{
				sys_ppu_thread_t id1;
				sys_ppu_thread_create(&id1, Client_Thread, conn_TCP, -0x1d0, 0x2000, 1, "SNMAPI_Client");
			}
			sys_timer_usleep(1000);
		}
	}
	_sys_ppu_thread_exit(0);
}

static void UDP_Thread(uint64_t args)
{
	sys_timer_sleep(20);
	int sockUDP, enable = 1;
	struct sockaddr_in s;

	char* msg = "SNMAPI_PS3";
	size_t len = strlen(msg);

	sockUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	memset(&s, 0, sizeof(s));
	s.sin_family = AF_INET;
	s.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	s.sin_port = htons(1337);

	int ret = setsockopt(sockUDP, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable));

	for (;;)
	{
		sendto(sockUDP, msg, len, 0, (struct sockaddr*)&s, sizeof(s));
		sys_timer_sleep(2);
	}
	_sys_ppu_thread_exit(0);
}

static void FTP_Thread(uint64_t args)
{
	sys_timer_sleep(20);
	int list_sFTP = slisten(21, 4);
	int conn_FTP;
	if (list_sFTP)
	{
		for (;;)
		{
			if ((conn_FTP = accept(list_sFTP, NULL, NULL)) > 0)
			{
				sys_ppu_thread_t id;
				sys_ppu_thread_create(&id, handleclient, (uint64_t)conn_FTP, -0x1d0, 0x2000, 1, "ftpd");
			}
		}
	}
	sys_ppu_thread_exit(0);
}

int SNMAPI_MAIN(uint64_t args)
{
	//start the original sprx
	snmapi_load_sys_audio();

	//snmapi mount dev_SNMAPI flash
	snmapi_mount_flash();

	//install the payload for SNMAPI functions
	uint64_t payload = (uint64_t)snmapi_install_payload();

	//find the consoleID for the firmware
	InitConsoleID();

	//load the main thread and exit
	sys_ppu_thread_create(&_mainThread, Global_Thread, payload, -0x1d8, 0x2000, 1, "SNMAPI_Main_Thread");

	///load the ftp thread
	sys_ppu_thread_create(&_ftpThread, FTP_Thread, 0, -0x1d8, 0x2000, 1, "FTP_Main_Thread");

	//load the UDP Thread
	sys_ppu_thread_create(&_udpThread, UDP_Thread, 0, -0x1d8, 0x2000, 1, "UDP_Main_Thread");

	return SYS_PRX_RESIDENT;
}
