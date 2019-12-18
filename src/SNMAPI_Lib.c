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
#include "../vsh/xmb_plugin.h"
#include "../vsh/xregistry.h"
#include "../vsh/netctl_main.h"
#include "../vsh/stdc.h"
#include "vsh_functions.h"
#include "systemcall_functions.h"
#include "helper_functions.h"
//#include "GTA_functions.h"
#include "snmapi_client_functions.h"
/*
	SNMAPI Global Variables
*/

int snmapi_load_sys_audio(void)
{
	uint64_t ret = 0;
	int prxID = sys_prx_load_module("/dev_flash/sys/internal/sys_SNMAPI.sprx", 0, &ret);
	char arr[] = { 0 };
	int modres = 0;
	sys_prx_start_module(prxID, 0, &arr, &modres, 0, NULL);
	return 0;
}

/*
	SNMAPI Client Functions
*/
/*
int snmapi_client_handle_game_cmd(int socket, snmapi_handle_game_cmd_struct* args)
{
	char buffer1[150];
	sprintf(buffer1, "Game: %i, Option: %i, Value: %i", args->game_id, args->option_id, args->value);
	snmapi_notify_char(buffer1);
    /*
        game_id:
        0 = MW2
        1 = BO1
        2 = MW3
        3 = BO2
        4 = GTA5

        option_id:
            MW2:
        0 = prestige
        1 = level
        2 = score
        3 = kills
        4 = deaths
        5 = assists
        6 = headshots
        7 = wins
        8 = losses
        9 = ties
        10 = killstreak
        11 = winstreak
        12 = time played
     */




    process_id_t* buff = (process_id_t*)snmapi_malloc(64);
    snmapi_getprocesslist(buff);
    process_id_t pid = buff[2];
    snmapi_free(buff);

    if(pid <= 0)
    {
        return snmapi_send_int(socket, -1);
    }

    uint8_t _char = (uint8_t)args->value;
    int _int = reverse_int(args->value);
    uint32_t _uint = byte_reverse_32((uint32_t)args->value);
    uint16_t _short = (uint16_t)reverse_int16((int16_t)args->value);

	if(args->game_id == GTA5)
	{
	//irgendwo hier freeze
		GTA5PID = pid;
		if(!GTA5RPCInstalled())
		{
			uint32_t SFA1 = 0x1BE4C80, EFA1 = 0x1BE4D08, BFA1 = 0x18614, BAB1 = 0x18620;
			snmapi_setmemory(pid, SFA1, sizeof(gtavrpc), gtavrpc);

			uint32_t n1 = (CBAB(EFA1, BAB1));
			uint32_t n2 = (CBAB(BFA1, SFA1));

			snmapi_setmemory(pid, EFA1, 4, &n1);
			snmapi_setmemory(pid, BFA1, 4, &n2);
			snmapi_notify_char("RPC installed");
		}
	}

	  int r_mw2 = reverse_int(mw2xp[args->value]);
    int r_bo1 = reverse_int(bo1xp[args->value]);
    int r_mw3 = reverse_int(mw3xp[args->value]);
    int r_bo2 = reverse_int(bo2xp[args->value]);

    switch(args->game_id)
    {
        case MW2:
                switch(args->option_id)
                {
                    case 0:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01ff9a9c, 1, (const char*)&_char));

                    case 1:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01FF9A94, 4, &r_mw2));

                    case 2:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01FF9AA4, 4, &_uint));

                    case 3:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01FF9AA8, 4, &_int));

                    case 4:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01FF9AB0, 4, &_int));

                    case 5:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01FF9AB8, 4, &_int));

                    case 6:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01FF9ABC, 4, &_int));

                    case 7:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01FF9ADC, 4, &_int));

                    case 8:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01FF9AE0, 4, &_int));

                    case 9:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01FF9AE4, 4, &_int));

                    case 10:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01FF9AE8, 4, &_int));

                    case 11:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01FF9AAC, 4, &_int));

                    case 12:
                        return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01FF9AC8, 4, &_int) | snmapi_setmemory(pid, 0x01FF9ACC, 4, &_int));

                    case 13:
                        return snmapi_send_int(socket, MW2UnlockAll(pid));
                }
            break;

        case BO1:
            switch(args->option_id)
            {
                case 0:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[0], 1, (const char*)&_char));

                case 1:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[1], 4, &r_bo1));

                case 2:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[2], 4, &_uint));//cod points

                case 3:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[3], 4, &_int));//kills

                case 4:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[4], 4, &_int));//deaths

                case 5:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[5], 4, &_int));//assists

                case 6:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[6], 4, &_int));//headshots

                case 7:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[7], 4, &_int));//wins

                case 8:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[8], 4, &_int));//losses

                case 9:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[9], 2, &_short));//winstreak

                case 10:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[10], 2, &_short));//killstreak

                case 11:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[11], 4, &_int));//timeplayed

                case 12:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, bo1offsets[12], sizeof(bo1ua), bo1ua));
            }
            break;

        case MW3:
            switch(args->option_id)
            {
                case 0:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C1947C, 1, &_char));//prestige

                case 1:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C1926C, 4, &r_mw3));
                    //XP
                case 2:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C19484, 4, &_int));//score

                case 3:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C194AC, 4, &_int));//kills

                case 4:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C194B4, 4, &_int));//deaths

                case 5:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C194BC, 4, &_int));//assists

                case 6:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C194C0, 4, &_int));//headshots

                case 7:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C194E0, 4, &_int));//wins

                case 8:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C194E4, 4, &_int));//losses

                case 9:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C194E8, 4, &_int));//ties

                case 10:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C194B0, 4, &_int));//killstreak

                case 11:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C194EC, 4, &_int));//winstreak

                case 12:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x01C194CE, 4, &_int) | snmapi_setmemory(pid, 0x01C194D6, 4, &_int));//time played
            }
            break;

        case BO2:
            switch(args->option_id)
            {
                case 0:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x26FD014, 1, &_char));//prestige

                case 1:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x0, 4, &r_bo2));//lvl

                case 2:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x0, 4, &_int));//score

                case 3:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x0, 4, &_int));//kills

                case 4:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x0, 4, &_int));//deaths

                case 5:
                    return snmapi_send_int(socket, snmapi_setmemory(pid, 0x0, 4, &_int));

            }
            break;

		case GTA5:
			switch(args->option_id)
			{
                case 0:
                    return snmapi_send_int(socket, (GTA5PlayerNum = args->value));

				case 1://rank
					return snmapi_send_int(socket, GTA5_SetRank(gta5xp[args->value]));

				case 2:
					return snmapi_send_int(socket, GTA5_AddCash(args->value));

				case 3:
                    return snmapi_send_int(socket, GTA5_Snacks(args->value));

				case 4:
					return snmapi_send_int(socket, GTA5_Smokes(args->value));

				case 5:
					return snmapi_send_int(socket, GTA5_Armor(args->value));

				case 6:
					return snmapi_send_int(socket, GTA5_Firework(args->value));

				case 7:
					return snmapi_send_int(socket, GTA5_Achievements());

				case 8:
					return snmapi_send_int(socket, GTA5_LSC_UA());

				case 9:
					return snmapi_send_int(socket, GTA5_Clothing());

				case 10:
					return snmapi_send_int(socket, GTA5_Tattoos());

				case 11:
					return snmapi_send_int(socket, GTA5_Stats(args->value));

                case 12:
                    return snmapi_send_int(socket, GTA5_UnlockCamos());

                case 13:
                    return snmapi_send_int(socket, GTA5_UnlockTints());

                case 14:
                    return snmapi_send_int(socket, GTA5_TimeOverall(args->value));

                case 15:
                    return snmapi_send_int(socket, GTA5_ClearStats());

                case 16:
                    return snmapi_send(socket, GTA5_GetPlayerName(), strlen(GTAPlayerName));
			}
			break;
    }

    return -1;
}*/
