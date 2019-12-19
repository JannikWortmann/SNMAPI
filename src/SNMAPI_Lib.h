#ifndef __SNMAPI_LIB_H__
#define __SNMAPI_LIB_H__

#define SNMAPI_SYSCALL_NUMBER	1013

//SNMAPI CALL NUMBERS
#define SNMAPI_GetVersion	1
#define SNMAPI_Notify		2
#define SNMAPI_RingBuzzer	3
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
#define SNMAPI_UnloadSPRX	16
#define SNMAPI_GetGamePID	17
#define SNMAPI_GetFreeMemory 18
#define SNMAPI_NotifySmall 19
#define SNMAPI_GetFreeSpace 20
#define SNMAPI_GetConType  21
#define SNMAPI_Handle_Game_CMD 22


#define Make_XML(x) x << 5
/*XML Commands == normal cmds << 5
#####
<<5 == *2^5 == 32
*/

//SNMAPI PAYLOAD ERRORS
#define SNMAPI_PAYLOAD_NOT_FOUND	1

//SNMAPI NOTIFICATION DEFINES
#define SNMAPI_Notify_Caution			0
#define SNMAPI_Notify_Friend			1
#define SNMAPI_Notify_Headset			2
#define SNMAPI_Notify_Info				3
#define SNMAPI_Notify_Keyboard			4
#define SNMAPI_Notify_Mediaserver		5
#define SNMAPI_Notify_Music				6
#define SNMAPI_Notify_Error				7
#define SNMAPI_Notify_Settings			8
#define SNMAPI_Notify_Trophy_Bronze		9
#define SNMAPI_Notify_Trophy_Gold		0xA
#define SNMAPI_Notify_Trophy_Platinum	0xB
#define SNMAPI_Notify_Trophy_Silver		0xC
#define SNMAPI_Notify_Arrow				0xD
#define SNMAPI_Notify_Pen				0xE
#define SNMAPI_Notify_Hand				0xF
#define SNMAPI_Notify_Grab				0x10
#define SNMAPI_Notify_Arrow_Right		0x11
#define SNMAPI_Notify_PSN				0x12
#define SNMAPI_Notify_PSPlus			0x13
#define SNMAPI_Notify_SignIn			0x14
#define SNMAPI_Notify_New				0x15
#define SNMAPI_Notify_Check				0x16
#define SNMAPI_Notify_Attention			0x17
#define SNMAPI_Notify_Facebook			0x18
#define SNMAPI_Notify_TrophyRank		0x19
#define SNMAPI_Notify_PSNStore			0x1A

#define SNMAPI_Lib_Version	"4.81D"
#define SNMAPI_Port			1337

#define SNMAPI_PayloadAddr  0x80000000006F0000ULL
#define SNMAPI_PayloadPath	"/dev_flash/sys/internal/payloads/payload_%X.bin"

#define process_id_t		uint32_t
#define sys_prx_id_t		int32_t

#define MAX_CHAR_LENGTH		128

#define SUCCESS				0
#define ERROR			   -1

#define LV2_PEEK_END		0x800000003000000

#define EncryptConnection 1

#define MW2         0
#define BO1         1
#define MW3         2
#define BO2         3
#define GTA5        4
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
	uint32_t total;
	uint32_t available;//bytes
}memoryInfo;

typedef struct
{
	uint8_t texture;
	char message[128];
}snmapi_notify_struct;
//#define SNMAPI_Notify_Struct_Length 129

typedef struct
{
	uint8_t shutdown_flag;
}snmapi_shutdown_struct;
//#define SNMAPI_Shutdown_Struct_Length 1

typedef struct
{
	uint8_t buzzermode;
}snmapi_ring_buzzer_struct;
//#define SNMAPI_Ring_Buzzer_Struct_Length 1

typedef struct
{
	uint8_t kernel;
}snmapi_get_temperature_struct;
//#define SNMAPI_Get_Temperature_Struct_Length 1

typedef struct
{
	uint8_t soundNum;
}snmapi_play_sound_struct;
//#define SNMAPI_Play_Sound_Struct_Length 1

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
//#define SNMAPI_Set_CID_Struct_Length 16

typedef struct
{
	process_id_t processID;
	uint32_t address;
	uint32_t length;
}snmapi_get_memory_struct;
//#define SNMAPI_Get_Memory_Struct_Length 12

typedef struct
{
	process_id_t processID;
	uint32_t address;
	uint32_t length;
	char buffer[];
}snmapi_set_memory_struct;
//#define SNMAPI_Set_Memory_Struct_Length 12

typedef struct
{
	process_id_t processID;
}snmapi_get_processname_struct;
//#define SNMAPI_Get_ProcessName_Struct_Length 4

typedef struct
{
	process_id_t processID;
	char path[100];
}snmapi_load_sprx_struct;
//#define SNMAPI_Load_SPRX_Struct_Length 104

typedef struct
{
	process_id_t processID;
	sys_prx_id_t prxID;
}snmapi_unload_sprx_struct;

typedef struct
{
	char msg[128];
}snmapi_notify_small_struct;
//#define SNMAPI_Notify_Small_Struct_Length 128

typedef struct
{
    int game_id;
    int option_id;
    int value;
}snmapi_handle_game_cmd_struct;

/*
	SNMAPI Payload Functions
*/
/*
int snmapi_getmemory(process_id_t processID, uint64_t address, uint32_t size, char* buffer);
int snmapi_setmemory(process_id_t processID, uint64_t address, uint32_t size, const char* buffer);
int snmapi_getprocesslist(process_id_t* processlist);
int snmapi_getprocessname(process_id_t processID, char* name);
int snmapi_loadsprx(process_id_t processID, char* sprxPath, sys_prx_id_t* prxID);
int snmapi_unloadsprx(process_id_t processID, sys_prx_id_t prxID);


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
int snmapi_get_free_memory(memoryInfo* info);

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
void SHA1_Hash(unsigned char* data, unsigned int data_len, unsigned char* hash);


int snmapi_notify(char* texture, char* plugin, char* message);
uint32_t snmapi_findPlugin(char* pluginName);
int snmapi_playSound(const char* soundName, char* pluginName);
void snmapi_free(void* ptr);
void* snmapi_malloc(size_t size);
int snmapi_swprintf(wchar_t* buffer, size_t maxLen, char* text);
int snmapi_display_message(char* message);
int snmapi_notify_char(char* message);

//int snmapi_establish_safe_connection(int socket,uint8_t* key);

int snmapi_client_get_version(int socket);
int snmapi_client_notify(int socket, snmapi_notify_struct* args);
int snmapi_client_ring_buzzer(int socket, snmapi_ring_buzzer_struct* args);
int snmapi_client_shutdown(int socket, snmapi_shutdown_struct* args);
int snmapi_client_get_temperature(int socket, snmapi_get_temperature_struct* args);
int snmapi_client_get_firmware(int socket);
int snmapi_client_play_sound(int socket, snmapi_play_sound_struct* args);
int snmapi_client_peek_lv2(int socket, snmapi_peek_lv2_struct* args);
int snmapi_client_set_consoleid(int socket, snmapi_set_consoleid_struct* args);
int snmapi_client_get_memory(int socket, snmapi_get_memory_struct* args);
int snmapi_client_set_memory(int socket, snmapi_set_memory_struct* args);
int snmapi_client_get_processes(int socket);
int snmapi_client_get_processname(int socket, snmapi_get_processname_struct* args);
int snmapi_client_load_sprx(int socket, snmapi_load_sprx_struct* args);
int snmapi_client_unload_sprx(int socket, snmapi_unload_sprx_struct* args);
int snmapi_client_get_game_process(int socket);
int snmapi_client_get_free_memory(int socket);
int snmapi_client_notify_small(int socket, snmapi_notify_small_struct* args);
int snmapi_client_get_free_space(int socket);
int snmapi_client_get_connection_type(int socket);
int snmapi_client_handle_game_cmd(int socket, snmapi_handle_game_cmd_struct* args);

int snmapi_client_get_version_xml(int socket);*/
#endif //__SNMAPI_LIB_H__
