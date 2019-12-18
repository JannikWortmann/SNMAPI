#include <cellstatus.h>
#include <sys/prx.h>
#include <stdio.h>
#include <sys/syscall.h>
/*
	SNMAPI Payload Functions
*/

int snmapi_getmemory(process_id_t processID, uint64_t address, uint32_t size, char* buffer)
{
	system_call_5(SNMAPI_SYSCALL_NUMBER, SNMAPI_GetMemory, (uint64_t)processID, address, (uint64_t)buffer, (uint64_t)size);
	return_to_user_prog(int);
}

int snmapi_setmemory(process_id_t processID, uint64_t address, uint32_t size, const char* buffer)
{
	system_call_5(SNMAPI_SYSCALL_NUMBER, SNMAPI_SetMemory, (uint64_t)processID, address, (uint64_t)buffer, (uint64_t)size);
	return_to_user_prog(int);
}

int snmapi_getprocesslist(process_id_t* processlist)
{
	system_call_2(SNMAPI_SYSCALL_NUMBER, SNMAPI_GetProcesses, (uint64_t)processlist);
	return_to_user_prog(int);
}

int snmapi_getprocessname(process_id_t processID, char* name)
{
	system_call_3(SNMAPI_SYSCALL_NUMBER, SNMAPI_GetProcName, (uint64_t)processID, (uint64_t)name);
	return_to_user_prog(int);
}

int snmapi_loadsprx(process_id_t processID, char* sprxPath, sys_prx_id_t* prxID)
{
	system_call_4(SNMAPI_SYSCALL_NUMBER, SNMAPI_LoadSPRX, (uint64_t)processID, (uint64_t)sprxPath, (uint64_t)prxID);
	return_to_user_prog(int);
}

int snmapi_unloadsprx(process_id_t processID, sys_prx_id_t prxID)
{
	system_call_3(SNMAPI_SYSCALL_NUMBER, SNMAPI_UnloadSPRX, (uint64_t)processID, (uint64_t)prxID);
	return_to_user_prog(int);
}

/*
	SNMAPI Syscall Functions
*/

int snmapi_ring_buzzer_once(void)
{
	system_call_3(392, 0x1004, 0x4, 0x6);
	return_to_user_prog(int);
}

int snmapi_ring_buzzer_twice(void)
{
	system_call_3(392, 0x1004, 0x7, 0x36);
	return_to_user_prog(int);
}

int snmapi_ring_buzzer_tripple(void)
{
	system_call_3(392, 0x1004, 0xa, 0x1b6);
	return_to_user_prog(int);
}

int snmapi_get_temperature(int rsx, uint32_t* temperature)
{
	system_call_2(383, (uint64_t)(int)rsx, (uint64_t)((uint32_t)temperature));
	return_to_user_prog(int);
}

int snmapi_ps3_shutdown(void)
{
	cellFsUnlink((char*)"/dev_hdd0/tmp/turnoff");
	system_call_4(379, 0x1100, 0, 0, 0);
	return_to_user_prog(int);
}

int snmapi_ps3_quick_reboot(void)
{
	cellFsUnlink((char*)"/dev_hdd0/tmp/turnoff");
	system_call_3(379, 0x8201, NULL, 0);
	return_to_user_prog(int);
}

int snmapi_ps3_hard_reboot(void)
{
	cellFsUnlink((char*)"/dev_hdd0/tmp/turnoff");
	system_call_3(379, 0x1200, NULL, 0);
	return_to_user_prog(int);
}

int snmapi_ps3_soft_reboot(void)
{
	cellFsUnlink((char*)"/dev_hdd0/tmp/turnoff");
	system_call_3(379, 0x200, NULL, 0);
	return_to_user_prog(int);
}

int snmapi_platform_info(platform_info* info)
{
	system_call_1(387, (uint64_t)info);
	return_to_user_prog(int);
}

int snmapi_get_ps3_type_number(uint64_t* type)
{
	system_call_1(985, (uint64_t)type);
	return_to_user_prog(int);
}

uint64_t snmapi_peek_lv2(uint64_t address)
{
	system_call_1(6, address);
	return_to_user_prog(uint64_t);
}

int snmapi_poke_lv2(uint64_t address, uint64_t value)
{
	system_call_2(7, address, value);
	return 0;
}

int snmapi_mount_device(const char *device, const char *format, const char *point, int a, int b, int c, void *buffer, int len)
{
	system_call_8(837, (uint64_t)device, (uint64_t)format, (uint64_t)point, a, b, c, (uint64_t)buffer, len);
	return_to_user_prog(int);
}

int snmapi_get_free_memory(memoryInfo* info)
{
	system_call_1(352, (uint64_t)(uint32_t)info);
	return_to_user_prog(int);
}
