#include <sys/syscall.h>
#include <sys/return_code.h>
#include <cell/cell_fs.h>
#include <cellstatus.h>
#include <sys/prx.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <netinet/in.h>
#include "SNMAPI_Lib.h"

/*
	SNMAPI Global Variables
*/

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

/*
	SNMAPI Helper Functions
*/

int snmapi_get_firmware(int* firmware)
{
	platform_info info;
	int ret = snmapi_platform_info(&info);
	*firmware = (int)info.firmware_version;
	return ret;
}

int snmapi_get_ps3_type_string(char* type)
{
	uint64_t t;
	int ret = snmapi_get_ps3_type_number(&t);
	char* _t = "";
	switch (t)
	{
	case 1:
		_t = "CEX";
		break;

	case 2:
		_t = "DEX";
		break;

	case 3:
		_t = "DECH";
		break;

	default:
		_t = "unknown";
		break;
	}
	strncpy(type, _t, strlen(_t));
	return ret;
}

int snmapi_get_firmware_string(char* firmware)
{
	int ret, fw;
	ret = snmapi_get_firmware(&fw);
	char buffer[15];
	sprintf(buffer, "%X", fw);
	sprintf(buffer, "%c.%c%c", buffer[0], buffer[2], buffer[3]);
	strncpy(firmware, buffer, strlen(buffer));
	return ret;
}

int snmapi_get_temperature_celsius(int kernel, int* temp)
{
	int temperature;
	int ret = snmapi_get_temperature(kernel, &temperature);
	*temp = temperature >> 24;
	return ret;
}

uint64_t snmapi_peek_lv2_(uint64_t address)
{
	return snmapi_peek_lv2(address);
}

int snmapi_get_lv2_version(void)
{
	uint64_t toc = snmapi_peek_lv2_(0x8000000000003000ULL);
	switch (toc)
	{
	
	case 0x8000000000366BD0ULL:
		return 0x446D;
	case 0x8000000000348DF0ULL:
		return 0x446C;
	case 0x800000000034B160ULL:
		return 0x450C;
	case 0x800000000036EC40ULL:
		return 0x450D;
	case 0x800000000034B2E0ULL:
		return 0x453C;
	case 0x800000000034E620ULL:
		return 0x455C;
	case 0x80000000003738E0ULL:
		return 0x455D;
	case 0x800000000034F950ULL:
		return 0x460C;
	case 0x800000000034F960ULL:
		return (snmapi_peek_lv2_(0x80000000002FC938ULL)==0x323031342F31312FULL) ? 0x466C : 0x465C;
	case 0x8000000000375510ULL:
		if (snmapi_peek_lv2_(0x800000000031EBA8ULL) != 0x323031342F31312FULL) return 0x465D;
		break;
	case 0x800000000034FB10ULL:
		return 0x470C;
	case 0x8000000000375850ULL:
		return 0x470D;
	case 0x800000000034FBB0ULL:
			return (snmapi_peek_lv2_(0x80000000002FCB68ULL)==0x323031352F30382FULL) ? 0x476C : 0x475C;
	case 0x80000000003758E0ULL:
			return (snmapi_peek_lv2_(0x800000000031EF48ULL)==0x323031352F30382FULL) ? /*0x476D*/0 : 0x475D;
	default:
		return 0;
	}
	return 0;
}

uint64_t snmapi_get_syscall_table(void)
{
	int lv2_version = snmapi_get_lv2_version();
	switch (lv2_version)
	{
	case 0x446C:
		return 0x800000000035E860ULL;
	case 0x446D:
		return 0x800000000037CFE8ULL;
	case 0x450C:
		return 0x800000000035F0D0ULL;
	case 0x450D:
		return 0x8000000000383658ULL;
	case 0x453C:
		return 0x800000000035F300ULL;
	case 0x455C:
		return 0x8000000000362680ULL;
	case 0x460C:
		return 0x8000000000363A18ULL;
	case 0x465C:
		return 0x8000000000363A18ULL;
	case 0x465D:
		return 0x800000000038A120ULL;
	case 0x466C:
		return 0x8000000000363A18ULL;
	case 0x470C:
		return 0x8000000000363B60ULL;
	case 0x470D:
		return 0x800000000038A368ULL;
	case 0x475C:
		return 0x8000000000363BE0ULL;
	case 0x475D:
		return 0x800000000038A3E8ULL;
	case 0x476C:
		return 0x8000000000363BE0ULL;
	/*case 0x476D:
		return 0x800000000038A3E8ULL;*/
	default:
		return 0;
	}
	return 0;
}

int snmapi_write_htab(void)
{
	uint64_t cont = 0;
	uint64_t reg5, reg6;
	uint32_t val;
	while (cont < 0x80)
	{
		val = (cont << 7);
		reg5 = snmapi_peek_lv2_(0x800000000f000000ULL | ((uint64_t)val));
		reg6 = snmapi_peek_lv2_(0x800000000f000008ULL | ((uint64_t)val));
		reg6 = (reg6 & 0xff0000ULL) | 0x190ULL;
		system_call_8(10, 0, (cont << 3ULL), reg5, reg6, 0, 0, 0, 1);
		cont++;
	}
	return 0;
}

int snmapi_install_payload(void)
{
	int fd, lv2 = snmapi_get_lv2_version();
	char path[256];
	
	snmapi_write_htab();
	sprintf(path, SNMAPI_PayloadPath, lv2);

	CellFsStat buf;
	cellFsStat(path, &buf);
	uint64_t payload[buf.st_size / 8], read, syscall_table = snmapi_get_syscall_table();

	if (cellFsOpen(path, CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
	{
		//read the payload data
		cellFsRead(fd, payload, buf.st_size, &read);
		cellFsClose(fd);

		//write the payload into the lv2
		int i;
		for (i = 0; i < sizeof(payload) / 8; i++)
		{
			snmapi_poke_lv2(SNMAPI_PayloadAddr + i * 8, payload[i]);
		}
		//install syscall opd
		snmapi_poke_lv2(SNMAPI_PayloadAddr - 0x10, SNMAPI_PayloadAddr);
		snmapi_poke_lv2(syscall_table + SNMAPI_SYSCALL_NUMBER * 8, SNMAPI_PayloadAddr - 0x10);
	}
	//else
		//return -1;
	return 0;
}

int snmapi_load_sys_audio(void)
{
	uint64_t ret = 0;
	int prxID = sys_prx_load_module("/dev_flash/sys/internal/sys_SNMAPI.sprx", 0, &ret);
	char arr[] = { 0 };
	int modres = 0;
	sys_prx_start_module(prxID, 0, &arr, &modres, 0, NULL);
	return 0;
}

int snmapi_mount_flash(void)
{
	return snmapi_mount_device("CELL_FS_IOS:BUILTIN_FLSH1", "CELL_FS_FAT", "/dev_SNMAPI", 0, 0, 0, 0, 0);
}

int snmapi_encrypt(uint8_t* destination, uint8_t* source, size_t length, int Key)
{
	size_t i;
	for (i = 0; i < length; i++)
	{
		destination[i] = ((source[i] + Key) % 256);
	}
	return 0;
}

int snmapi_decrypt(uint8_t* destination, uint8_t* source, size_t length, int Key)
{
	size_t i;
	for (i = 0; i < length; i++)
	{
		destination[i] = ((source[i] - Key) % 256);
	}
	return 0;
}

int snmapi_send(int socket, char* buffer, size_t length, int Key)
{
#ifdef EncryptConnection
	uint8_t* dest = (uint8_t*)snmapi_malloc(length);
	snmapi_encrypt(dest, (uint8_t*)buffer, length, Key);
	return send(socket, (char*)dest, length, 0) ? 0 : -1;
#else
	return send(socket, buffer, length, 0) ? 0 : -1;
#endif
}

int snmapi_recv(int socket, char* buffer, size_t length, int Key)
{
#ifdef EncryptConnection
	uint8_t* dest = (uint8_t*)snmapi_malloc(length);
	int ret = recv(socket, (char*)dest, length, 0);
	if (ret > 0)
	{
		int ret = snmapi_decrypt((uint8_t*)buffer, dest, length, Key);
		snmapi_free(dest);
		return ret;
	}
	return ERROR;
#else
	return recv(socket, buffer, length, 0) ? -1 : 0;
#endif
}

uint64_t reverse_long(uint64_t x) {
	unsigned long rc = 0;
	rc = ((x & 0xFF00000000000000) >> 56) | ((x & 0x00FF000000000000) >> 40) | ((x & 0x0000FF0000000000) >> 24) | ((x & 0x000000FF00000000) >> 8) | ((x & 0x00000000FF000000) << 8) | ((x & 0x0000000000FF0000) << 24) | ((x & 0x000000000000FF00) << 40) | ((x & 0x00000000000000FF) << 56);
	return rc;
}

uint32_t byte_reverse_32(uint32_t num) {
	uint32_t res;
	res = ((num & 0xFF000000) >> 24) | ((num & 0x00FF0000) >> 8) | ((num & 0x0000FF00) << 8) | ((num & 0x000000FF) << 24);
	return res;
}

int snmapi_send_int(int socket, int to_send, int Key)
{
	int tmp = byte_reverse_32(to_send);
	return snmapi_send(socket, &tmp, 4, Key);
}

uint64_t cidAddr1;
uint64_t cidAddr2;
void InitConsoleID()
{
	int lv2 = snmapi_get_lv2_version();
	switch (lv2)
	{
	case 0x446C:
		cidAddr1 = 0x80000000003DBE30ULL;
		cidAddr2 = 0x8000000000476F3CULL;
		break;

	case 0x446D:
		cidAddr1 = 0x80000000003FA8B0ULL;
		cidAddr2 = 0x8000000000496F3CULL;
		break;

	case 0x450C:
		cidAddr1 = 0x80000000003DE230ULL;
		cidAddr2 = 0x800000000046CF0CULL;
		break;

	case 0x450D:
		cidAddr1 = 0x8000000000402AB0ULL;//EID0
		cidAddr2 = 0x8000000000494F0CULL;//EID5
		break;

	case 0x453C:
		cidAddr1 = 0x80000000003DE430ULL;
		cidAddr2 = 0x800000000046CF0CULL;
		break;

	case 0x453D:
		cidAddr1 = 0x80000000004045B0ULL;
		cidAddr2 = 0x8000000000494F0CULL;
		break;

	case 0x455C:
		cidAddr1 = 0x80000000003E17B0ULL;
		cidAddr2 = 0x8000000000474F1CULL;
		break;

	case 0x455D:
		cidAddr1 = 0x8000000000407930ULL;
		cidAddr2 = 0x8000000000494F1CULL;
		break;

	case 0x460C:
		cidAddr1 = 0x80000000003E2BB0ULL;
		cidAddr2 = 0x8000000000474F1CULL;
		break;

	case 0x465C:
		cidAddr1 = 0x80000000003E2BB0ULL;
		cidAddr2 = 0x8000000000474F1CULL;
		break;

	case 0x465D:
		cidAddr1 = 0x80000000004095B0ULL;
		cidAddr2 = 0x800000000049CF1CULL;
		break;

	case 0x466C:
		//remove this version
		break;

	case 0x470C:
		cidAddr1 = 0x80000000003E2DB0ULL;
		cidAddr2 = 0x8000000000474AF4ULL;
		break;

	case 0x470D:
		cidAddr1 = 0x80000000004098B0ULL;
		cidAddr2 = 0x800000000049CaF4ULL;
		break;


	}
}

int snmapi_set_cid(uint8_t* cid)
{
	consoleid_union x;
	x.str = cid;
	snmapi_poke_lv2(cidAddr1, x.cid[0]);
	snmapi_poke_lv2(cidAddr1 + 8, x.cid[1]);
	snmapi_poke_lv2(cidAddr2, x.cid[0]);
	snmapi_poke_lv2(cidAddr2 + 8, x.cid[1]);
	return 0;
}

/*
	SNMAPI VSH Functions
*/

void * getNIDfunc(const char * vsh_module, uint32_t fnid)
{
	// 0x10000 = ELF
	// 0x10080 = segment 2 start
	// 0x10200 = code start

	uint32_t table = (*(uint32_t*)0x1008C) + 0x984; // vsh table address
	//	uint32_t table = (*(uint32_t*)0x1002C) + 0x214 - 0x10000; // vsh table address
	//	uint32_t table = 0x63A9D4;


	while (((uint32_t)*(uint32_t*)table) != 0)
	{
		uint32_t* export_stru_ptr = (uint32_t*)*(uint32_t*)table; // ptr to export stub, size 2C, "sys_io" usually... Exports:0000000000635BC0 stru_635BC0:    ExportStub_s <0x1C00, 1, 9, 0x39, 0, 0x2000000, aSys_io, ExportFNIDTable_sys_io, ExportStubTable_sys_io>

		const char* lib_name_ptr = (const char*)*(uint32_t*)((char*)export_stru_ptr + 0x10);

		if (strncmp(vsh_module, lib_name_ptr, strlen(lib_name_ptr)) == 0)
		{
			// we got the proper export struct
			uint32_t lib_fnid_ptr = *(uint32_t*)((char*)export_stru_ptr + 0x14);
			uint32_t lib_func_ptr = *(uint32_t*)((char*)export_stru_ptr + 0x18);
			uint16_t count = *(uint16_t*)((char*)export_stru_ptr + 6); // number of exports
			for (int i = 0; i<count; i++)
			{
				if (fnid == *(uint32_t*)((char*)lib_fnid_ptr + i * 4))
				{
					// take adress from OPD
					return (void**)*((uint32_t*)(lib_func_ptr)+i);
				}
			}
		}
		table = table + 4;
	}
	return 0;
}

int(*vsh_notify)(int32_t, const char* eventName, int32_t, int32_t* texture, int32_t*, const char*, const char*, float, const wchar_t* text, int32_t, int32_t, int32_t) = NULL;
void*(*vsh_malloc)(unsigned int size) = NULL;
int(*vsh_free)(void* ptr) = NULL;
int(*vsh_getTexture)(int32_t*, uint32_t, const char* name) = NULL;
uint32_t(*vsh_findPlugin)(const char* plugin) = NULL;
void(*vsh_playsound)(uint32_t, const char*, float, int) = NULL;
int(*vsh_notify_char)(int unk, char* text) = NULL;
int(*UTF8stoUTF16s)(const uint8_t utf8[], size_t* utf8_len, uint16_t utf16[], size_t* utf16_len) = NULL;

void* snmapi_malloc(size_t size)
{
	if (!vsh_malloc)
		vsh_malloc = (void*)((int)getNIDfunc("allocator", 0x759E0635));

	if (vsh_malloc)
	{
		void* buf = vsh_malloc(size);
		if (buf)
			memset(buf, 0, size);
		return buf;
	}
	else
		printf("Unable to find >malloc<");
	return NULL;
}

void snmapi_free(void* ptr)
{
	if (!vsh_free)
		vsh_free = (void*)((int)getNIDfunc("allocator", 0x77A602DD));

	if (vsh_free)
		vsh_free(ptr);
	else
		printf("Unable to find >free<");
}

void snmapi_gettexture(int* texturePtr, uint32_t plugin, char* textureName)
{
	if (!vsh_getTexture)
		vsh_getTexture = (void*)((int)getNIDfunc("paf", 0x3A8454FC));

	if (vsh_getTexture)
		vsh_getTexture(texturePtr, plugin, textureName);
	else
		printf("Unable to find >getTexture<");
}

uint32_t snmapi_findPlugin(char* pluginName)
{
	if (!vsh_findPlugin)
		vsh_findPlugin = (void*)((int)getNIDfunc("paf", 0xF21655F3));

	if (vsh_findPlugin)
		return vsh_findPlugin(pluginName);
	else
		printf("Unable to find >findPlugin<");
	return 0;
}

int snmapi_playSound(char* soundName, char* pluginName)
{
	if (!vsh_playsound)
		vsh_playsound = (void*)((int)getNIDfunc("paf", 0xB93AFE7E));

	if (vsh_playsound)
		vsh_playsound(snmapi_findPlugin(pluginName), soundName, 1, 0);
	else
		printf("Unable to find >playsound<");
	return 0;
}

int snmapi_notify(char* texture, char* plugin, char* message)
{
	if (!vsh_notify)
		vsh_notify = (void*)((int)getNIDfunc("vshcommon", 0xA20E43DB));

	int texturePtr, dummy = 0;
	snmapi_gettexture(&texturePtr, snmapi_findPlugin(plugin), texture);
	//convert utf8 to utf16
	wchar_t* msg;
	snmapi_UTF8_to_UTF16((const char*)message, msg);
	if (vsh_notify)
		vsh_notify(0, "sceNpCommerce", 0, &texturePtr, &dummy, "", "", 0, msg, 0, 0, 0);
	else
		printf("Unable to find >notify<");
	snmapi_free(msg);
	return 0;
}

int snmapi_notify_char(char* message)
{
	if (!vsh_notify_char)
		vsh_notify_char = (void*)((int)getNIDfunc("vshtask", 0xA02D46E7));

	if (vsh_notify_char)
		vsh_notify_char(0, message);
	else
		printf("Unable to find >notify_char<");
	return 0;
}

int snmapi_UTF8_to_UTF16(const uint8_t* utf8, uint16_t* utf16)
{
	if (!UTF8stoUTF16s)
		UTF8stoUTF16s = (void*)((int)getNIDfunc("sdk", 0xf7681b9a));

	//get size of the char* and alloc space for the wchar*
	size_t size1 = strlen((char*)utf8), size2;
	utf16 = snmapi_malloc(size1* sizeof(uint16_t));

	if (UTF8stoUTF16s)
		UTF8stoUTF16s(utf8, &size1, utf16, &size2);
	else
		printf("Unable to find >UTF8_to_UTF16<");
	return 0;
}
/*
	SNMAPI Client Functions
*/

int snmapi_establish_safe_connection(int socket, int* Key)
{
	for (;;)
	{
		if ((int)recv(socket, (char*)Key, 4, 0))
		{
			break;
		}
	}
	*Key = *Key % 256;
	return SUCCESS;
}

int snmapi_client_get_version(int socket, int Key)
{
	return snmapi_send(socket, (char*)SNMAPI_Lib_Version, strlen((char*)SNMAPI_Lib_Version), Key);
}

int snmapi_client_notify(int socket, snmapi_notify_struct* args, int Key)
{
	char* plugin = "system_plugin";
	char* tex = "";
	switch ((int)args->texture)
	{
	case 0:
		tex = "tex_notification_caution";
		break;

	case 1:
		tex = "tex_notification_friend";
		break;

	case 2:
		tex = "tex_notification_headset";
		break;

	case 3:
		tex = "tex_notification_info";
		break;

	case 4:
		tex = "tex_notification_keypad";
		break;

	case 5:
		tex = "tex_notification_mediasever";
		break;

	case 6:
		tex = "tex_notification_music";
		break;

	case 7:
		tex = "tex_notification_psbutton_insensitive";
		break;

	case 8:
		tex = "tex_notification_settings";
		break;

	case 9:
		tex = "tex_notification_trophy_bronze";
		break;

	case 10:
		tex = "tex_notification_trophy_gold";
		break;

	case 11:
		tex = "tex_notification_trophy_platinum";
		break;

	case 12:
		tex = "tex_notification_trophy_silver";
		break;

	case 13:
		tex = "tex_pointer_arrow";
		break;

	case 14:
		tex = "tex_pointer_pen";
		break;

	case 15:
		tex = "tex_pointer_hand";
		break;

	case 16:
		tex = "tex_pointer_grab";
		break;

	case 17:
		tex = "tex_arrow_right";
		break;

	case 18:
		tex = "tex_psn";
		plugin = "xmb_plugin";
		break;

	case 19:
		tex = "tex_indi_plus";
		plugin = "xmb_plugin";
		break;

	case 20:
		tex = "tex_Signing_In";
		plugin = "explore_plugin";
		break;

	case 21:
		tex = "tex_new_ws";
		plugin = "explore_plugin";
		break;

	case 22:
		tex = "tex_check_ws";
		plugin = "explore_plugin";
		break;

	case 23:
		tex = "tex_urgent_ws";
		plugin = "explore_plugin";
		break;

	case 24:
		tex = "item_tex_cam_facebook";
		plugin = "explore_plugin";
		break;

	case 25:
		tex = "item_tex_Profile_LevelIcon";
		plugin = "explore_plugin";
		break;

	case 26:
		tex = "item_tex_ps_store";
		plugin = "explore_plugin";
		break;

	default:
		tex = "tex_notification_info";
		break;
	}
	return snmapi_send_int(socket, snmapi_notify((char*)tex, (char*)plugin,(char*)args->message), Key);
}

int snmapi_client_ring_buzzer(int socket, snmapi_ring_buzzer_struct* args, int Key)
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
	return snmapi_send_int(socket, ret, Key);
}

int snmapi_client_shutdown(int socket, snmapi_shutdown_struct* args, int Key)
{
	switch (args->shutdown_flag)
	{
	case 0:
		//shutdown
		snmapi_send_int(socket, 0, Key);
		snmapi_ps3_shutdown();
		break;

	case 1:
		//quick reboot
		snmapi_send_int(socket, 0, Key);
		snmapi_ps3_quick_reboot();
		break;

	case 2:
		//soft reboot
		snmapi_send_int(socket, 0, Key);
		snmapi_ps3_soft_reboot();
		break;

	case 3:
		//hard reboot
		snmapi_send_int(socket, 0, Key);
		snmapi_ps3_hard_reboot();
		break;
	}
	return 0;
}

int snmapi_client_get_temperature(int socket, snmapi_get_temperature_struct* args, int Key)
{
	int tmp, ret = -1;
	ret = snmapi_get_temperature_celsius((int)args->kernel, &tmp);
	return snmapi_send_int(socket, tmp, Key);
}

int snmapi_client_get_firmware(int socket, int Key)
{
	char fw[5];
	char type[5];
	char buffer[15];
	int ret = snmapi_get_firmware_string(fw);
	ret = snmapi_get_ps3_type_string(type);
	sprintf(buffer, "%s %s", fw, type);
	return snmapi_send(socket, buffer, 15, Key);
}

int snmapi_client_play_sound(int socket, snmapi_play_sound_struct* args, int Key)
{
	char* plugin = "", sound = "";
	switch(args->soundNum)
	{
		case 0:
			plugin = "system_plugin";
			sound = "snd_trophy";
			break;

		case 1:
			plugin = "";
			break;
	}
	return snmapi_send_int(socket, snmapi_playSound(sound, plugin), Key);
}

int snmapi_client_peek_lv2(int socket, snmapi_peek_lv2_struct* args, int Key)
{
	uint64_t value = snmapi_peek_lv2_(args->address);
	value = reverse_long(value);
	return snmapi_send(socket, &value, sizeof(value), Key);
}

int snmapi_client_poke_lv2(int socket, snmapi_poke_lv2_struct* args, int Key)
{
	return snmapi_send_int(socket, snmapi_poke_lv2(args->address, args->value), Key);
}

int snmapi_client_set_consoleid(int socket, snmapi_set_consoleid_struct* args, int Key)
{
	return snmapi_send_int(snmapi_set_cid(args->cid),socket, Key	);
}

int snmapi_client_get_memory(int socket, snmapi_get_memory_struct* args, int Key)
{
	int ret;
	char* buffer = snmapi_malloc(args->length);
	ret = snmapi_getmemory(args->processID, args->address, args->length, buffer);
	ret = snmapi_send(socket, buffer, args->length, Key);
	snmapi_free(buffer);
	return ret;
}

int snmapi_client_set_memory(int socket, snmapi_set_memory_struct* args, int Key)
{
	int ret;
	ret = snmapi_setmemory(args->processID, args->address, args->length, args->buffer);
	return snmapi_send_int(socket, ret, Key);
}

int snmapi_client_get_processes(int socket, int Key)
{
	uint32_t* buffer = (uint32_t*)snmapi_malloc(4 * 16);
	snmapi_getprocesslist((process_id_t*)buffer);
	int i;
	for (i = 0; i < 16; i++)
		buffer[i] = byte_reverse_32(buffer[i]);
	int ret = snmapi_send(socket, buffer, 64, Key);
	snmapi_free(buffer);
	return ret;
}

int snmapi_client_get_processname(int socket, snmapi_get_processname_struct* args, int Key)
{
	char* buffer = snmapi_malloc(MAX_CHAR_LENGTH);
	int ret;
	snmapi_getprocessname(args->processID, buffer);
	if (strlen(buffer) == 0)
	{
		char* n = "NULL";
		strncpy(buffer, n, strlen(n));
		ret = snmapi_send(socket, buffer, strlen(buffer), Key);
	}
	else
		ret = snmapi_send(socket, buffer, strlen(buffer), Key);
	snmapi_free(buffer);
	return ret;
}

int snmapi_client_load_sprx(int socket, snmapi_load_sprx_struct* args, int Key)
{
	sys_prx_id_t prxID;
	snmapi_loadsprx(args->processID, args->path, &prxID);
	return snmapi_send_int(socket, prxID, Key);
}

int snmapi_client_unload_sprx(int socket, snmapi_unload_sprx_struct* args, int Key)
{
	return snmapi_send_int(socket, snmapi_unloadsprx(args->processID, args->prxID), Key);
}

int snmapi_client_get_game_process(int socket, int Key)
{
	process_id_t* buff = (process_id_t*)snmapi_malloc(64);
	snmapi_getprocesslist(buff);
	process_id_t pid = buff[2];
	snmapi_free(buff);
	return snmapi_send_int(socket, pid, Key);
}