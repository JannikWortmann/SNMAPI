
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
		switch (snmapi_peek_lv2_(0x80000000002FCB68ULL))
		{
			case 0x323031352F30382FULL:
				return 0x476C;
			case 0x323031352F31322FULL:
				return 0x478C;
			case 0x323031352F30342FULL:
				return 0x475C;
			case 0x323031362F31302FULL:
					return 0x481C;
		}
		break;
	case 0x80000000003758E0ULL:
		switch (snmapi_peek_lv2_(0x800000000031EF48ULL))
		{
			case 0x323031352F30382FULL:
				return 0x476D;
			case 0x323031352F31322FULL:
				return 0x478D;
			case 0x323031352F30342FULL:
				return 0x475D;
		}
		break;
	case 0x800000000034FBA0ULL:
		return 0x480C;
	case 0x80000000003759B0ULL:
            return 0x480D;
	case 0x80000000003759C0ULL:
		return 0x481D;
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
	case 0x476C:
	case 0x478C:
	case 0x480C:
	case 0x481C:
		return 0x8000000000363BE0ULL;
	case 0x475D:
	case 0x476D:
	case 0x478D:
		return 0x800000000038A3E8ULL;
	case 0x480D:
	case 0x481D:
		return 0x800000000038A4E8ULL;
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
		return 0;
	}
	return SNMAPI_PAYLOAD_NOT_FOUND;
}

int snmapi_mount_flash(void)
{
	return snmapi_mount_device("CELL_FS_IOS:BUILTIN_FLSH1", "CELL_FS_FAT", "/dev_SNMAPI", 0, 0, 0, 0, 0);
}

int snmapi_send(int socket, char* buffer, size_t length)
{
	return send(socket, buffer, length, 0) ? SUCCESS : ERROR;
}

int snmapi_recv(int socket, char* buffer, size_t length)
{
	return recv(socket, buffer, length, 0) ? SUCCESS : ERROR;
}

uint64_t reverse_long(uint64_t x) {
	unsigned long rc = 0;
	rc = ((x & 0xFF00000000000000) >> 56) | ((x & 0x00FF000000000000) >> 40) | ((x & 0x0000FF0000000000) >> 24) | ((x & 0x000000FF00000000) >> 8) | ((x & 0x00000000FF000000) << 8) | ((x & 0x0000000000FF0000) << 24) | ((x & 0x000000000000FF00) << 40) | ((x & 0x00000000000000FF) << 56);
	return rc;
}

int32_t reverse_int(int32_t num)
{
	int res;
	res = ((num & 0xFF000000) >> 24) | ((num & 0x00FF0000) >> 8) | ((num & 0x0000FF00) << 8) | ((num & 0x000000FF) << 24);
	return res;
}

uint32_t byte_reverse_32(uint32_t num) {
	uint32_t res;
	res = ((num & 0xFF000000) >> 24) | ((num & 0x00FF0000) >> 8) | ((num & 0x0000FF00) << 8) | ((num & 0x000000FF) << 24);
	return res;
}

int16_t reverse_int16(int16_t num)
{
	int16_t res;
	res = ((num & 0xFF00) >> 8) | ((num & 0x00FF) << 8);
	return res;
}

int snmapi_send_int(int socket, int to_send)
{
	int tmp = byte_reverse_32(to_send);
	return snmapi_send(socket, &tmp, 4);
}

int snmapi_send_uint(int socket, uint32_t val)
{
	uint32_t x = byte_reverse_32(val);
	return snmapi_send(socket, &x, 4);
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
		cidAddr1 = 0x8000000000402AB0ULL;
		cidAddr2 = 0x8000000000494F0CULL;
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

	case 0x470C:
		cidAddr1 = 0x80000000003E2DB0ULL;
		cidAddr2 = 0x8000000000474AF4ULL;
		break;

	case 0x470D:
		cidAddr1 = 0x80000000004098B0ULL;
		cidAddr2 = 0x800000000049CAF4ULL;
		break;

	case 0x475C:
		cidAddr1 = 0x80000000003E2E30ULL;
		cidAddr2 = 0x8000000000474AF4ULL;
		break;

	case 0x475D:
		cidAddr1 = 0x8000000000409930ULL;
		cidAddr2 = 0x800000000049CAF4ULL;
		break;

	case 0x476C:
		cidAddr1 = 0x80000000003E2E30ULL;
		cidAddr2 = 0x8000000000474AF4ULL;
		break;

	case 0x476D:
		cidAddr1 = 0x8000000000409930ULL;
		cidAddr2 = 0x800000000049CAF4ULL;
		break;

	case 0x478C:
		cidAddr1 = 0x80000000003E2E30ULL;
		cidAddr2 = 0x8000000000474AF4ULL;
		break;

	case 0x478D:
		cidAddr1 = 0x8000000000409930ULL;
		cidAddr2 = 0x800000000049CAF4ULL;
		break;

	case 0x480C:
		cidAddr1 = 0x80000000003E2E30ULL;
		cidAddr2 = 0x8000000000474AF4ULL;
		break;

	case 0x480D:
        cidAddr1 = 0x8000000000409A30ULL;
        cidAddr2 = 0x800000000049CAF4ULL;
		break;

	case 0x481C:
		cidAddr1 = 0x80000000003E2E30ULL;
		cidAddr2 = 0x8000000000474AF4ULL;
		break;

	case 0x481D:
		cidAddr1 = 0x8000000000409A30ULL;
		cidAddr2 = 0x800000000049CAF4ULL;
		break;
	}
}

int snmapi_set_cid(uint8_t* cid)
{
	snmapi_poke_lv2(cidAddr1, *(uint64_t*)(cid));
	snmapi_poke_lv2(cidAddr1 + 8, *(uint64_t*)(cid + 8));
	snmapi_poke_lv2(cidAddr2, *(uint64_t*)(cid));
	snmapi_poke_lv2(cidAddr2 + 8, *(uint64_t*)(cid + 8));
	snmapi_display_message("IDPS changed!");
	return 0;
}
