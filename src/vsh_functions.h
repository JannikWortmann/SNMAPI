#include <cellstatus.h>
#include <sys/prx.h>
#include <stdio.h>
#include <sys/syscall.h>
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
void(*vsh_playsound)(uint32_t plugin, const char* sound, float unk, int unk2) = NULL;
int(*vsh_notify_char)(int unk, char* text) = NULL;
int(*vsh_swprintf)(wchar_t* s, size_t n, const wchar_t* format, ...) = NULL;
int (*vsh_plugin_GetInterface)(int,int) = NULL;
int(*vsh_find_widget)(uint32_t plugin, const char* widget) = NULL;
int(*vsh_display_message)(int widgetptr, wchar_t* msg, int _4, int _0) = NULL;
int(*vsh_unk_method)(int ptr, float _null) = NULL;
int(*vsh_tolower)(int c) = NULL;

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

int snmapi_playSound(const char* soundName, char* pluginName)
{
	if (!vsh_playsound)
		vsh_playsound = (void*)((int)getNIDfunc("paf", 0xB93AFE7E));

	if (vsh_playsound)
	{
		uint32_t plugin = snmapi_findPlugin(pluginName);
		if(plugin <= 0)
			printf("error finding plugin");
		vsh_playsound(plugin, soundName, 1, 0);
	}
	else
		printf("Unable to find >playsound<");
	return 0;
}

int snmapi_notify(char* texture, char* plugin, char* message)
{
	if (!vsh_notify)
		vsh_notify = (void*)((int)getNIDfunc("vshcommon", 0xA20E43DB));

	int texturePtr, dummy = 0;
	uint32_t pluginPtr = snmapi_findPlugin(plugin);
	if (pluginPtr <= 0)
		return -1;
	snmapi_gettexture(&texturePtr, pluginPtr, texture);
	wchar_t msg[128];
	if (snmapi_swprintf(msg, 128, message) < 0)
		return -1;

	if (vsh_notify)
		vsh_notify(0, "sceNpCommerce", 0, &texturePtr, &dummy, "", "", 0, msg, 0, 0, 0);
	else
		printf("Unable to find >notify<");
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

int snmapi_swprintf(wchar_t* buffer,size_t maxLen, char* text)
{
	if (!vsh_swprintf)
		vsh_swprintf = (void*)((int)getNIDfunc("stdc", 0x62bf1d6c));

	if (vsh_swprintf)
		return vsh_swprintf(buffer, maxLen, L"%hs", text);
	else
		printf("Unable to find >swprintf<");
	return 0;
}

int snmapi_get_interface(uint32_t plugin, int indicator)
{
	if(!vsh_plugin_GetInterface)
		vsh_plugin_GetInterface = (void*)((int)getNIDfunc("paf",0x23AFB290));

		if(vsh_plugin_GetInterface)
			return vsh_plugin_GetInterface(plugin, indicator);
		else
			printf("Unable to find >plugin_GetInterface<");
		return 0;
}

int snmapi_find_widget(uint32_t plugin, char* widgetName)
{
	if(!vsh_find_widget)
		vsh_find_widget = (void*)((int)getNIDfunc("paf",0x794CEACB));

	if(vsh_find_widget)
		return vsh_find_widget(plugin, widgetName);
	else
		printf("Unable to find >find_widget<");
	return 0;
}

int snmapi_unk0(int ptr)
{
	if(!vsh_unk_method)
		vsh_unk_method = (void*)((int)getNIDfunc("vshcommon",0xF1918912));

	if(vsh_unk_method)
		return vsh_unk_method(ptr,5000.0);
	else
		printf("Unable to find >unk method<");
	return 0;
}

int snmapi_display_message(char* message)
{
	if(!vsh_display_message)
		vsh_display_message = (void*)((int)getNIDfunc("vshcommon",0xF55812AE));

	if(strlen(message) > 128)
		message[128] = 0;

	wchar_t msg[128];
	if(snmapi_swprintf(msg,128, message)<0)
		return -1;
	int ret = 0;
	int widget = snmapi_find_widget(snmapi_findPlugin("system_plugin"), "page_notification");
	if(widget != 0)
	{
		if(vsh_display_message)
			ret = vsh_display_message(widget, msg, 4, 0);
		else
			printf("Unable to find >display message<");

		if(ret != 0)
			snmapi_unk0(ret);
	}
	return 0;
}

int snmapi_tolower(int c)
{
	if(c >=65 && c<=90)
        {
            // It fails in the below assignment
            return c + 32;
        }
		return c;
}

int _tolower(int c)
{
	if(c >=65 && c<=90)
        {
            // It fails in the below assignment
            return c + 32;
        }
		return c;
}
