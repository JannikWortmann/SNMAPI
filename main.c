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
int SNMAPI_STOP(uint64_t);

SYS_MODULE_INFO(SNMAPI, 0, 1, 0);
SYS_MODULE_START(SNMAPI_MAIN);
SYS_MODULE_STOP(SNMAPI_STOP);

sys_ppu_thread_t _mainThread, _ftpThread, _udpThread, _stopThread;
int run = 1;

static inline void _sys_ppu_thread_exit(uint64_t val)
{
	system_call_1(41, val);
}

static inline sys_prx_id_t prx_get_module_id_by_address(void *addr)
{
	system_call_1(461, (uint64_t)(uint32_t)addr);
	return (int32_t)p1;
}
/*
static void finalize_module(void)
{
	uint64_t meminfo[5];

	sys_prx_id_t prx = prx_get_module_id_by_address(finalize_module);

	meminfo[0] = 0x28;
	meminfo[1] = 2;
	meminfo[3] = 0;

	system_call_3(482, prx, 0, (uint64_t)(uint32_t)meminfo);
}

static void SNMAPI_Stop_thread(uint64_t arg)
{
	if (_mainThread != (sys_ppu_thread_t)-1)
	{
		uint64_t exit;
		sys_ppu_thread_join(_mainThread, &exit);
	}
	if (_ftpThread != (sys_ppu_thread_t)-1)
	{
		uint64_t exit;
		sys_ppu_thread_join(_ftpThread, &exit);
	}
	if (_udpThread != (sys_ppu_thread_t)-1)
	{
		uint64_t exit;
		sys_ppu_thread_join(_udpThread, &exit);
	}
	sys_ppu_thread_exit(0);
}
*/
int SNMAPI_STOP(uint64_t args)
{/*
	uint64_t exitcode;
	sys_ppu_thread_t t;
	sys_ppu_thread_create(&t, SNMAPI_Stop_thread, 0, 0, 0x2000, SYS_PPU_THREAD_CREATE_JOINABLE, "SNMAPI_Stop_Thread");
	sys_ppu_thread_join(t, &exitcode);
*/
	//finalize_module();
	//_sys_ppu_thread_exit(0);
	//exit(0);//
	snmapi_ps3_shutdown();
	return SYS_PRX_STOP_OK;
}

static void Client_Thread(uint64_t _sock)
{
	int sock = (int)_sock;
	int privateKey = 0;
#if EncryptConnection
	snmapi_establish_safe_connection(sock, &privateKey);
#endif
	char buffer[2048];
	int8_t command = 0;
	uint32_t data_ptr = (uint32_t)buffer + 1;
	while (snmapi_recv(sock, buffer, 2048) == SUCCESS)
	{
		command = *(int8_t*)buffer;
		switch (command)
		{
		case SNMAPI_GetVersion:
			snmapi_client_get_version(sock, privateKey);
			break;

		case SNMAPI_Notify:
			snmapi_client_notify(sock, (snmapi_notify_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_RingBuzzer:
			snmapi_client_ring_buzzer(sock, (snmapi_ring_buzzer_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_GetTemp:
			snmapi_client_get_temperature(sock, (snmapi_get_temperature_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_Shutdown:
			snmapi_client_shutdown(sock, (snmapi_shutdown_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_GetFirmware:
			snmapi_client_get_firmware(sock, privateKey);
			break;

		case SNMAPI_PlaySound:
			snmapi_client_play_sound(sock, (snmapi_play_sound_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_PeekLV2:
			snmapi_client_peek_lv2(sock, (snmapi_peek_lv2_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_PokeLV2:
			snmapi_client_poke_lv2(sock, (snmapi_poke_lv2_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_SetCID:
			snmapi_client_set_consoleid(sock, (snmapi_set_consoleid_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_GetMemory:
			snmapi_client_get_memory(sock, (snmapi_get_memory_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_SetMemory:
			snmapi_client_set_memory(sock, (snmapi_set_memory_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_GetProcesses:
			snmapi_client_get_processes(sock, privateKey);
			break;

		case SNMAPI_GetProcName:
			snmapi_client_get_processname(sock, (snmapi_get_processname_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_LoadSPRX:
			snmapi_client_load_sprx(sock, (snmapi_load_sprx_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_UnloadSPRX:
			snmapi_client_unload_sprx(sock, (snmapi_unload_sprx_struct*)data_ptr, privateKey);
			break;

		case SNMAPI_GetGamePID:
			snmapi_client_get_game_process(sock, privateKey);
			break;
		}
		sys_timer_usleep(1668);
		sys_ppu_thread_yield();
	}
	shutdown(sock, SHUT_RDWR);
	socketclose(sock);
	snmapi_notify("tex_notification_settings", "system_plugin", L"Client thread ended!");
	sys_ppu_thread_exit(0);
}

static void Global_Thread(uint64_t args)
{
	int payload_err = 0;
	sys_timer_sleep(20);
	//install payload
	if (snmapi_install_payload() == -1)
		payload_err = 1;
	InitConsoleID();//setup the console id for the firmware

	while (!snmapi_findPlugin("system_plugin"))
		sys_timer_usleep(500);
	sys_timer_sleep(3);
	/*
	if (payload_err)
	{
		snmapi_notify("", "", L"SNMAPI Payloads not found! ERROR!");
		return;
	}*/

	snmapi_notify("tex_notification_settings", "system_plugin", "SNMAPI v2.50 loaded!");
	sys_timer_sleep(2);
	snmapi_notify("tex_notification_mediasever", "system_plugin", "FTP Server started!");

	int list_sMain, conn_TCP;
	list_sMain = slisten(SNMAPI_Port, 2);

	if (list_sMain > 0)
	{
		while ((conn_TCP = accept(list_sMain, NULL, NULL)))
		{
			sys_ppu_thread_t id1;
			sys_ppu_thread_create(&id1, Client_Thread, conn_TCP, -0x1d0, 0x2000, 0, "SNMAPI_Client");
		}
	}
	sys_ppu_thread_exit(0);
}

static void UDP_Thread(uint64_t args)
{
	sys_timer_sleep(20);
	int sockUDP, n;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len = sizeof(cliaddr);
	char servermsg[] = "SNMAPI-CONSOLE";
	char c_msg[100];

	sockUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(1338);
	bind(sockUDP, (struct sockaddr *)&servaddr, sizeof(servaddr));

	while (1)
	{
		n = recvfrom(sockUDP, c_msg, 100, 0, (struct sockaddr *)&cliaddr, &len);

		c_msg[n] = 0;
		if (!strcmp(c_msg, "SNMAPI-CLIENT"))
		{
			sendto(sockUDP, servermsg, strlen(servermsg), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
		}
	}
	sys_ppu_thread_exit(0);
}

static void FTP_Thread(uint64_t args)
{
	sys_timer_sleep(20);
	int list_sFTP = slisten(21, 4);
	int conn_FTP;
	if (list_sFTP)
	{
		while((conn_FTP = accept(list_sFTP, NULL, NULL)))
		{
			sys_ppu_thread_t id;
			sys_ppu_thread_create(&id, handleclient, (uint64_t)conn_FTP, -0x1d0, 0x2000, 0, "ftpd");
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

	//load the main thread and exit
	sys_ppu_thread_create(&_mainThread, Global_Thread, NULL, -0x1d8, 0x2000, 0, "SNMAPI_Main_Thread");

	///load the ftp thread
	sys_ppu_thread_create(&_ftpThread, FTP_Thread, NULL, -0x1d8, 0x2000, 0, "FTP_Main_Thread");

	//load the UDP Thread
	sys_ppu_thread_create(&_udpThread, UDP_Thread, NULL, -0x1d8, 0x2000, 0, "UDP_Main_Thread");

	return SYS_PRX_RESIDENT;
}