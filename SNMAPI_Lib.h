#ifndef __SNMAPI_LIB_H__
#define __SNMAPI_LIB_H__

#define SNMAPI_SYSCALL_NUMBER	1013

#define SNMAPI_GetVersion	0
#define SNMAPI_Notify		1
#define SNMAPI_RingBuzzer	2
//#define SNMAPI_ControlLED	3
#define SNMAPI_GetTemp		4
#define SNMAPI_Shutdown		5
#define SNMAPI_GetFirmware	6
#define SNMAPI_PlaySound	7
#define SNMAPI_PeekLV2		8
#define SNMAPI_PokeLV2		9
#define SNMAPI_SetCID		10
#define SNMAPI_GetMemory	11
#define SNMAPI_SetMemory	12
#define SNMAPI_GetProcesses	13
#define SNMAPI_GetProcName	14
#define SNMAPI_LoadSPRX		15
#define SNMAPI_Disconnect	16
#define SNMAPI_UnloadSPRX	17
#define SNMAPI_GetGamePID	18

#define SNMAPI_Lib_Version	"2.50"
#define SNMAPI_Port			1337

#define SNMAPI_PayloadAddr  0x80000000006F0000ULL
#define SNMAPI_PayloadPath	"/dev_flash/sys/internal/payloads/payload_%X.bin"

#define process_id_t		uint32_t
#define sys_prx_id_t		uint32_t

#define MAX_CHAR_LENGTH		128

#define SUCCESS				0
#define ERROR				-1

#define LV2_PEEK_END		0x800000003000000

#define EncryptConnection 0
/*
	SNMAPI Structs,Unions, etc
*/

typedef struct
{
	uint32_t firmware_version; //Ex: 0x03055000
	uint8_t res1[4];
	uint64_t platform_id;
	uint32_t build_number;
	uint8_t res2[4];
}platform_info;

typedef struct
{
	uint8_t texture;
	char message[128];
}snmapi_notify_struct;

typedef struct
{
	uint8_t shutdown_flag;
}snmapi_shutdown_struct;

typedef struct
{
	uint8_t buzzermode;
}snmapi_ring_buzzer_struct;

typedef struct
{
	uint8_t kernel;
}snmapi_get_temperature_struct;

typedef struct
{
	uint8_t soundNum;
}snmapi_play_sound_struct;

typedef struct
{
	uint64_t address;
}snmapi_peek_lv2_struct;

typedef struct
{
	uint64_t address;
	uint64_t value;
}snmapi_poke_lv2_struct;

typedef struct
{
	uint8_t cid[16];
}snmapi_set_consoleid_struct;

typedef union
{
	uint8_t* str;
	uint64_t cid[2];
}consoleid_union;

typedef struct
{
	process_id_t processID;
	uint32_t address;
	uint32_t length;
}snmapi_get_memory_struct;

typedef struct
{
	process_id_t processID;
	uint32_t address;
	uint32_t length;
	char buffer[];
}snmapi_set_memory_struct;

typedef struct
{
	process_id_t processID;
}snmapi_get_processname_struct;

typedef struct
{
	process_id_t processID;
	char path[100];
}snmapi_load_sprx_struct;

typedef struct
{
	process_id_t processID;
	sys_prx_id_t prxID;
}snmapi_unload_sprx_struct;

/*
	SNMAPI Payload Functions
*/

int snmapi_getmemory(process_id_t processID, uint64_t address, uint32_t size, char* buffer);
int snmapi_setmemory(process_id_t processID, uint64_t address, uint32_t size, const char* buffer);
int snmapi_getprocesslist(process_id_t* processlist);
int snmapi_getprocessname(process_id_t processID, char* name);
int snmapi_loadsprx(process_id_t processID, char* sprxPath, sys_prx_id_t* prxID);
int snmapi_unloadsprx(process_id_t processID, sys_prx_id_t prxID);

/*
	SNMAPI Syscall Functions
*/
int snmapi_ring_buzzer_once(void);
int snmapi_ring_buzzer_twice(void);
int snmapi_ring_buzzer_tripple(void);
int snmapi_get_temperature(int rsx, uint32_t* temperature);//0 = CPU; 1 = RSX
int snmapi_ps3_shutdown(void);
int snmapi_ps3_quick_reboot(void);
int snmapi_ps3_hard_reboot(void);
int snmapi_ps3_soft_reboot(void);
int snmapi_platform_info(platform_info* info);
int snmapi_get_ps3_type_number(uint64_t* type);
uint64_t snmapi_peek_lv2(uint64_t address);
int snmapi_poke_lv2(uint64_t adddress, uint64_t value);

/*
	SNMAPI Helper Functions
*/
int snmapi_get_firmware(int* firmware);
int snmapi_get_ps3_type_string(char* type);
uint64_t snmapi_peek_lv2_(uint64_t address);
int snmapi_get_lv2_version(void);
uint64_t snmapi_get_syscall_table(void);
int snmapi_write_htab(void);
int snmapi_install_payload(void);
int snmapi_load_sys_audio(void);
int snmapi_mount_flash(void);
void InitConsoleID();
/*
	SNMAPI VSH Funcs
*/
int snmapi_notify(char* texture, char* plugin, char* message);
uint32_t snmapi_findPlugin(char* pluginName);
void snmapi_free(void* ptr);
void* snmapi_malloc(size_t size);
int snmapi_UTF8_to_UTF16(const uint8_t* utf8, uint16_t* utf16);
/*
	SNMAPI Client Functions
*/
int snmapi_establish_safe_connection(int socket,int* Key);
int snmapi_client_get_version(int socket, int Key);
int snmapi_client_notify(int socket, snmapi_notify_struct* args, int Key);
int snmapi_client_ring_buzzer(int socket, snmapi_ring_buzzer_struct* args, int Key);
int snmapi_client_shutdown(int socket, snmapi_shutdown_struct* args, int Key);
int snmapi_client_get_temperature(int socket, snmapi_get_temperature_struct* args, int Key);
int snmapi_client_get_firmware(int socket, int Key);
int snmapi_client_play_sound(int socket, snmapi_play_sound_struct* args, int Key);
int snmapi_client_peek_lv2(int socket, snmapi_peek_lv2_struct* args, int Key);
int snmapi_client_set_consoleid(int socket, snmapi_set_consoleid_struct* args, int Key);
int snmapi_client_get_memory(int socket, snmapi_get_memory_struct* args, int Key);
int snmapi_client_set_memory(int socket, snmapi_set_memory_struct* args, int Key);
int snmapi_client_get_processes(int socket, int Key);
int snmapi_client_get_processname(int socket, snmapi_get_processname_struct* args, int Key);
int snmapi_client_load_sprx(int socket, snmapi_load_sprx_struct* args, int Key);
int snmapi_client_unload_sprx(int socket, snmapi_unload_sprx_struct* args, int Key);
int snmapi_client_get_game_process(int socket, int Key);
#endif //__SNMAPI_LIB_H__