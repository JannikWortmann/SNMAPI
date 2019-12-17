int getEID0PSID(uint8_t *psid)
{
  uint64_t eid0_idps[2], buffer[0x40], start_sector;
  uint32_t read;
  int ret = -1;
  sys_device_handle_t source;
	if(sys_storage_open(0x100000000000004ULL, 0, &source, 0)!=0)
	{
		start_sector = 0x204;
		ret = sys_storage_close(source);
    if(ret != 0)
      return ret;
		ret = sys_storage_open(0x100000000000001ULL, 0, &source, 0);
    if(ret != 0)
      return ret;
	}
	else start_sector = 0x178;
	ret = sys_storage_read(source, 0, start_sector, 1, buffer, &read, 0);
  if(ret != 0)
    return ret;
	ret = sys_storage_close(source);
  if(ret != 0)
    return ret;

  memcpy(psid, &buffer[0x0E], 8);
  memcpy(&psid[8], &buffer[0x0F], 8);
  return 0;
}
