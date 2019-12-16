#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cell/cell_fs.h>
#include <cellstatus.h>
#include <netdb.h>
#include <sys/fs.h>
#include <cell/rtc.h>
#include <cell/gcm.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ppu_thread.h>
#include <netex/sockinfo.h>

#include "ftp.h"
#include "printf.h"

const char* VERSION = "ftpd 1.1";
#define THREAD_NAME "ftpdt"
#define STOP_THREAD_NAME "ftpds"

#define MIN(a, b)	((a) <= (b) ? (a) : (b))
#define ABS(a)		(((a) < 0) ? -(a) : (a))

#define FTPPORT			21		// port to start ftp server on (21 is standard)
#define DISABLE_PASS	1		// whether or not to disable the checking of the password (1 - yes, 0 - no)

#define ssend(socket, str) send(socket, str, strlen(str), 0)
#define FD(socket) (socket & ~SOCKET_FD_MASK)
#define getPort(p1x, p2x) ((p1x * 256) + p2x)

int my_atoi(const char *c)
{
	int previous_result = 0, result = 0;
	int multiplier = 1;

	if (c && *c == '-')
	{
		multiplier = -1;
		c++;
	}
	else
	{
		multiplier = 1;
	}

	while (*c)
	{
		if (*c < '0' || *c > '9')
		{
			return result * multiplier;
		}
		result *= 10;
		if (result < previous_result)
		{

			return(0);
		}
		else
		{
			previous_result *= 10;
		}

		result += *c - '0';

		if (result < previous_result)
		{

			return(0);
		}
		else
		{
			previous_result += *c - '0';
		}
		c++;
	}
	return(result * multiplier);
}

void absPath(char* absPath_s, const char* path, const char* cwd)
{
	if (path[0] == '/')
	{
		strcpy(absPath_s, path);
	}
	else
	{
		strcpy(absPath_s, cwd);

		if (cwd[strlen(cwd) - 1] != '/')
		{
			strcat(absPath_s, "/");
		}

		strcat(absPath_s, path);
	}
}

int isDir(const char* path)
{
	struct CellFsStat s;
	if (cellFsStat(path, &s) == CELL_FS_SUCCEEDED)
		return ((s.st_mode & CELL_FS_S_IFDIR) != 0);
	else
		return 0;
}

int ssplit(const char* str, char* left, int lmaxlen, char* right, int rmaxlen)
{
	int ios = strcspn(str, " ");
	int ret = (ios < (int)strlen(str) - 1);
	int lmax = (ios < lmaxlen) ? ios : lmaxlen;

	strncpy(left, str, lmax);
	left[lmax] = '\0';

	if (ret)
	{
		strncpy(right, str + ios + 1, rmaxlen);
		right[rmaxlen] = '\0';
	}
	else
	{
		right[0] = '\0';
	}

	return ret;
}

int slisten(int port, int backlog)
{
	int list_s = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in sa;
	socklen_t sin_len = sizeof(sa);
	memset(&sa, 0, sin_len);

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(list_s, (struct sockaddr *)&sa, sin_len);
	listen(list_s, backlog);

	return list_s;
}

void sclose(int *socket_e)
{
	if (*socket_e != -1)
	{
		shutdown(*socket_e, SHUT_RDWR);
		socketclose(*socket_e);
		*socket_e = -1;
	}
}

int connect_to_server(char *server, uint16_t port)
{
	struct sockaddr_in sin;
	unsigned int temp;
	int s;

	if ((temp = inet_addr(server)) != (unsigned int)-1)
	{
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = temp;
	}
	else {
		struct hostent *hp;

		if ((hp = gethostbyname(server)) == NULL)
		{
			return -1;
		}

		sin.sin_family = hp->h_addrtype;
		memcpy(&sin.sin_addr, hp->h_addr, hp->h_length);
	}

	sin.sin_port = htons(port);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
	{
		return -1;
	}

	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		return -1;
	}

	return s;
}

void handleclient(uint64_t conn_s_p)
{

	int conn_s = (int)conn_s_p; // main communications socket
	int data_s = -1;			// data socket

	int connactive = 1;			// whether the ftp connection is active or not
	int dataactive = 0;			// prevent the data connection from being closed at the end of the loop
	int loggedin = 0;			// whether the user is logged in or not

	static char rnfr[512];		// stores the path/to/file for the RNFR command - not multithread safe

	char cwd[512];				// Current Working Directory

	char smonth[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
		"Aug", "Sep", "Oct", "Nov", "Dec" };
	char buffer[3000];
	char filename[384];
	char tempcwd[512];
	char cmd[16], param[384];
	struct CellFsStat buf;
	int fd;

	int p1x = 0;
	int p2x = 0;

	CellRtcDateTime rDate;
	CellRtcTick pTick;
	cellRtcGetCurrentTick(&pTick);
	p1x = ((pTick.tick && 0xff) % 96) + 32;
	p2x = pTick.tick % 256;

	// welcome message
	ssend(conn_s, "220-SNMAPI v2.50 FTP Server\r\n");
	sprintf(buffer, "%i %s\r\n", 220, VERSION);	ssend(conn_s, buffer);

	// set working directory
	strcpy(cwd, "/");

	while (connactive == 1)
	{

		if (recv(conn_s, buffer, 2047, 0) > 0)
		{
			
			// get rid of the newline at the end of the string
			buffer[strcspn(buffer, "\n")] = '\0';
			buffer[strcspn(buffer, "\r")] = '\0';

			int split = ssplit(buffer, cmd, 15, param, 511);
			printf("cmd: %s\n", cmd);
			if (loggedin == 1)
			{
				// available commands when logged in
				if (strcasecmp(cmd, "CWD") == 0)
				{

					strcpy(tempcwd, cwd);

					if (split == 1)
					{
						absPath(tempcwd, param, cwd);
					}

					if (isDir(tempcwd))
					{
						strcpy(cwd, tempcwd);
						ssend(conn_s, "250 O\r\n");
					}
					else
					{
						ssend(conn_s, "550 E\r\n");
					}
				}
				else
					if (strcasecmp(cmd, "CDUP") == 0)
					{
						int pos = strlen(cwd) - 2;

						for (int i = pos; i > 0; i--)
						{
							if (i < pos && cwd[i] == '/')
							{
								break;
							}
							else
							{
								cwd[i] = '\0';
							}
						}
						ssend(conn_s, "250 O\r\n");
					}
					else
						if (strcasecmp(cmd, "PWD") == 0)
						{
							sprintf(buffer, "257 \"%s\"\r\n", cwd);
							ssend(conn_s, buffer);
						}
						else
							if (strcasecmp(cmd, "TYPE") == 0)
							{
								ssend(conn_s, "200 TYPE OK\r\n");
								dataactive = 1;
							}
							else
								if (strcasecmp(cmd, "QUIT") == 0 || strcasecmp(cmd, "BYE") == 0)
								{
									ssend(conn_s, "221 BYE\r\n");
									connactive = 0;
								}
								else
									if (strcasecmp(cmd, "FEAT") == 0)
									{
										ssend(conn_s, "211-Ext:\r\n");
										ssend(conn_s, " SIZE\r\n");
										ssend(conn_s, " MDTM\r\n");
										ssend(conn_s, " PORT\r\n");
										ssend(conn_s, " CDUP\r\n");
										ssend(conn_s, " ABOR\r\n");
										ssend(conn_s, " PASV\r\n");
										ssend(conn_s, " LIST\r\n");
										ssend(conn_s, " MLSD\r\n");
										ssend(conn_s, " MLST type*;size*;modify*;UNIX.mode*;UNIX.uid*;UNIX.gid*;\r\n");
										ssend(conn_s, "211 End\r\n");
									}
									else
										if (strcasecmp(cmd, "PORT") == 0)
										{
											if (split == 1)
											{
												char data[6][4];
												int i = 0;
												uint8_t k = 0;

												for (uint8_t j = 0; j <= strlen(param); j++)
												{
													if (param[j] != ',' && param[j] != 0) { data[i][k] = param[j]; k++; }
													else { data[i][k] = 0; i++; k = 0; }
													if (i >= 6)
													{
														printf("FTP: error the port function\n");
														break;
													}
												}

												if (i == 6)
												{
													char ipaddr[16];
													sprintf(ipaddr, "%s.%s.%s.%s", data[0], data[1], data[2], data[3]);
													data_s = connect_to_server(ipaddr, getPort(my_atoi(data[4]), my_atoi(data[5])));
													if (data_s >= 0)
													{
														ssend(conn_s, "200 O\r\n");
														dataactive = 1;
														printf("FTP: client connected!\n");
													}
													else
													{
														printf("could not connect!\n");
														ssend(conn_s, "451 E\r\n");
													}
												}
												else
												{
													ssend(conn_s, "501 E\r\n");
												}
											}
											else
											{
												ssend(conn_s, "501 E\r\n");
											}
										}
										else
											if (strcasecmp(cmd, "SITE") == 0)
											{
												if (split == 1)
												{
													split = ssplit(param, cmd, 31, filename, 511);

													if (strcasecmp(cmd, "HELP") == 0)
													{
														ssend(conn_s, "214-CMDs:\r\n");
														ssend(conn_s, " SITE SHUTDOWN\r\n");
														ssend(conn_s, "214 End\r\n");
													}
													else
														if (strcasecmp(cmd, "SHUTDOWN") == 0)
														{
															ssend(conn_s, "221 O\r\n");

															{system_call_4(379, 0x1100, 0, 0, 0); }
															sys_ppu_thread_exit(0);
														}
														else
														{
															ssend(conn_s, "500 E\r\n");
														}
												}
												else
												{
													ssend(conn_s, "501 E\r\n");
												}
											}
											else
												if (strcasecmp(cmd, "NOOP") == 0)
												{
													ssend(conn_s, "200 NOOP\r\n");
												}
												else
													if (strcasecmp(cmd, "MLSD") == 0 || strcasecmp(cmd, "LIST") == 0 || strcasecmp(cmd, "MLST") == 0)
													{
														if (data_s > 0)
														{

															strcpy(tempcwd, cwd);

															if (split == 1)
															{
																absPath(tempcwd, param, cwd);
															}

															if (cellFsOpendir((isDir(tempcwd) ? tempcwd : cwd), &fd) == CELL_FS_SUCCEEDED)
															{
																ssend(conn_s, "150 O\r\n");

																CellFsDirent entry;
																uint64_t read_e;

																while (cellFsReaddir(fd, &entry, &read_e) == 0 && read_e > 0)
																{
																	if (!strcmp(entry.d_name, "app_home") || !strcmp(entry.d_name, "host_root")) continue;

																	absPath(filename, entry.d_name, cwd);

																	cellFsStat(filename, &buf);
																	cellRtcSetTime_t(&rDate, buf.st_mtime);
																	if (strcasecmp(cmd, "MLSD") == 0 || strcasecmp(cmd, "MLST") == 0)
																	{

																		char dirtype[2];
																		if (strcmp(entry.d_name, ".") == 0)
																		{
																			dirtype[0] = 'c';
																		}
																		else
																			if (strcmp(entry.d_name, "..") == 0)
																			{
																				dirtype[0] = 'p';
																			}
																			else
																			{
																				dirtype[0] = '\0';
																			}

																		dirtype[1] = '\0';

																		if (strcasecmp(cmd, "MLSD") == 0)
																			sprintf(buffer, "type=%s%s;siz%s=%llu;modify=%04i%02i%02i%02i%02i%02i;UNIX.mode=0%i%i%i;UNIX.uid=root;UNIX.gid=root; %s\r\n",
																			dirtype,
																			((buf.st_mode & S_IFDIR) != 0) ? "dir" : "file",
																			((buf.st_mode & S_IFDIR) != 0) ? "d" : "e", (unsigned long long)buf.st_size, rDate.year, rDate.month, rDate.day, rDate.hour, rDate.minute, rDate.second,
																			(((buf.st_mode & S_IRUSR) != 0) * 4 + ((buf.st_mode & S_IWUSR) != 0) * 2 + ((buf.st_mode & S_IXUSR) != 0) * 1),
																			(((buf.st_mode & S_IRGRP) != 0) * 4 + ((buf.st_mode & S_IWGRP) != 0) * 2 + ((buf.st_mode & S_IXGRP) != 0) * 1),
																			(((buf.st_mode & S_IROTH) != 0) * 4 + ((buf.st_mode & S_IWOTH) != 0) * 2 + ((buf.st_mode & S_IXOTH) != 0) * 1),
																			entry.d_name);
																		else
																			sprintf(buffer, " type=%s%s;siz%s=%llu;modify=%04i%02i%02i%02i%02i%02i;UNIX.mode=0%i%i%i;UNIX.uid=root;UNIX.gid=root; %s\r\n",
																			dirtype,
																			((buf.st_mode & S_IFDIR) != 0) ? "dir" : "file",
																			((buf.st_mode & S_IFDIR) != 0) ? "d" : "e", (unsigned long long)buf.st_size, rDate.year, rDate.month, rDate.day, rDate.hour, rDate.minute, rDate.second,
																			(((buf.st_mode & S_IRUSR) != 0) * 4 + ((buf.st_mode & S_IWUSR) != 0) * 2 + ((buf.st_mode & S_IXUSR) != 0) * 1),
																			(((buf.st_mode & S_IRGRP) != 0) * 4 + ((buf.st_mode & S_IWGRP) != 0) * 2 + ((buf.st_mode & S_IXGRP) != 0) * 1),
																			(((buf.st_mode & S_IROTH) != 0) * 4 + ((buf.st_mode & S_IWOTH) != 0) * 2 + ((buf.st_mode & S_IXOTH) != 0) * 1),
																			entry.d_name);
																	}
																	else
																		sprintf(buffer, "%s%s%s%s%s%s%s%s%s%s   1 root  root        %llu %s %02i %02i:%02i %s\r\n",
																		((buf.st_mode & S_IFDIR) != 0) ? "d" : "-",
																		((buf.st_mode & S_IRUSR) != 0) ? "r" : "-",
																		((buf.st_mode & S_IWUSR) != 0) ? "w" : "-",
																		((buf.st_mode & S_IXUSR) != 0) ? "x" : "-",
																		((buf.st_mode & S_IRGRP) != 0) ? "r" : "-",
																		((buf.st_mode & S_IWGRP) != 0) ? "w" : "-",
																		((buf.st_mode & S_IXGRP) != 0) ? "x" : "-",
																		((buf.st_mode & S_IROTH) != 0) ? "r" : "-",
																		((buf.st_mode & S_IWOTH) != 0) ? "w" : "-",
																		((buf.st_mode & S_IXOTH) != 0) ? "x" : "-",
																		(unsigned long long)buf.st_size, smonth[rDate.month - 1], rDate.day,
																		rDate.hour, rDate.minute, entry.d_name);

																	ssend(data_s, buffer);
																}

																cellFsClosedir(fd);
																if (strlen(tempcwd)>6)
																{
																	uint32_t blockSize;
																	uint64_t freeSize;
																	char tempstr[128];
																	if (strchr(tempcwd + 1, '/'))
																		tempcwd[strchr(tempcwd + 1, '/') - tempcwd] = 0;
																	cellFsGetFreeSize(tempcwd, &blockSize, &freeSize);
																	sprintf(tempstr, "226 [%s] [ %i MB free ]\r\n", tempcwd, (int)((blockSize*freeSize) >> 20));
																	ssend(conn_s, tempstr);
																}
																else
																{
																	ssend(conn_s, "226 O\r\n");
																}
															}
															else
															{
																ssend(conn_s, "550 E\r\n");
															}
														}
														else
														{
															ssend(conn_s, "425 E\r\n");
														}
													}
													else
														if (strcasecmp(cmd, "PASV") == 0)
														{
															int data_ls = slisten(getPort(p1x, p2x), 1);

															if (data_ls > 0)
															{

																sys_net_sockinfo_t conn_info;
																sys_net_get_sockinfo(conn_s, &conn_info, 1);

																char ip_address[16];
																char pasv_output[56];
																sprintf(ip_address, "%s", inet_ntoa(conn_info.local_adr));
																for (uint8_t n = 0; n<strlen(ip_address); n++) if (ip_address[n] == '.') ip_address[n] = ',';
																sprintf(pasv_output, "227 Entering Passive Mode (%s,%i,%i)\r\n", ip_address, p1x, p2x);

																ssend(conn_s, pasv_output);

																if ((data_s = accept(data_ls, NULL, NULL)) > 0)
																{
																	dataactive = 1;
																}
																else
																{
																	ssend(conn_s, "451 E\r\n");
																}

																sclose(&data_ls);
															}
															else
															{
																ssend(conn_s, "451 E\r\n");
															}
														}
														else
															if (strcasecmp(cmd, "RETR") == 0)
															{
																printf("get file!\n");
																if (data_s > 0)
																{
																	printf("data_spassed\n");
																	if (split == 1)
																	{
																		absPath(filename, param, cwd);
																		printf("split passed\n");
																		if (cellFsStat(filename, &buf) == CELL_FS_SUCCEEDED)
																		{
																			ssend(conn_s, "150 O\r\n");
																			printf("filestats passed\n");
																			int rr = 0;

																			if (cellFsOpen(filename, CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
																			{
																				printf("cellFsOpen passed\n");
																				uint64_t read_e = 0, pos; //, write_e

																				cellFsLseek(fd, 0, CELL_FS_SEEK_SET, &pos);
																				printf("cellFsSeek passed\n");
																				while (1)
																				{
																					printf("while loop reached\n"); 
																					if (cellFsRead(fd, (void *)buffer, 3000, &read_e) == CELL_FS_SUCCEEDED)
																					{
																						printf("cellfsread doing\n");
																						if (read_e>0)
																							send(data_s, buffer, (size_t)read_e, 0);
																						else
																						{
																							printf("FTP: Error the read function\n");
																							break;
																						}
																					}
																					else
																					{
																						printf("FTP: Error read not succeeded!\n");
																						rr = -2; break;
																					}
																				}

																				cellFsClose(fd);
																			}

																			if (rr == 0)
																			{
																				ssend(conn_s, "226 O\r\n");
																			}
																			else
																			{
																				ssend(conn_s, "451 E\r\n");
																			}
																		}
																		else
																		{
																			ssend(conn_s, "550 E\r\n");
																		}
																	}
																	else
																	{
																		ssend(conn_s, "501 E\r\n");
																	}
																}
																else
																{
																	printf("data conn not active!\n");
																	ssend(conn_s, "425 E\r\n");
																}
															}
															else
																if (strcasecmp(cmd, "DELE") == 0)
																{
																	if (split == 1)
																	{

																		absPath(filename, param, cwd);

																		if (cellFsUnlink(filename) == 0)
																		{
																			ssend(conn_s, "250 O\r\n");
																		}
																		else
																		{
																			ssend(conn_s, "550 E\r\n");
																		}
																	}
																	else
																	{
																		ssend(conn_s, "501 E\r\n");
																	}
																}
																else
																	if (strcasecmp(cmd, "MKD") == 0)
																	{
																		if (split == 1)
																		{

																			absPath(filename, param, cwd);

																			if (cellFsMkdir(filename, 0777) == 0)
																			{
																				sprintf(buffer, "257 \"%s\" OK\r\n", param);
																				ssend(conn_s, buffer);
																			}
																			else
																			{
																				ssend(conn_s, "550 E\r\n");
																			}
																		}
																		else
																		{
																			ssend(conn_s, "501 E\r\n");
																		}
																	}
																	else
																		if (strcasecmp(cmd, "RMD") == 0)
																		{
																			if (split == 1)
																			{

																				absPath(filename, param, cwd);

																				if (cellFsRmdir(filename) == 0)
																				{
																					ssend(conn_s, "250 O\r\n");
																				}
																				else
																				{
																					ssend(conn_s, "550 E\r\n");
																				}
																			}
																			else
																			{
																				ssend(conn_s, "501 E\r\n");
																			}
																		}
																		else
																			if (strcasecmp(cmd, "STOR") == 0)
																			{
																				if (data_s > 0)
																				{
																					if (split == 1)
																					{
																						absPath(filename, param, cwd);

																						ssend(conn_s, "150 O\r\n");

																						int rr = -1;

																						if (cellFsOpen(filename, CELL_FS_O_CREAT | CELL_FS_O_WRONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
																						{
																							uint64_t read_e = 0;

																							cellFsFtruncate(fd, 0);

																							rr = 0;

																							while (1)
																							{
																								if ((read_e = (uint64_t)recv(data_s, buffer, 3000, MSG_WAITALL)) > 0)
																								{
																									if (cellFsWrite(fd, buffer, read_e, NULL) != CELL_FS_SUCCEEDED) { printf("FTP: write data\n"); rr = -1; break; }
																								}
																								else
																								{
																									printf("FTP: read_e smaller than 0\n");
																									break;
																								}
																							}

																							cellFsClose(fd);
																							cellFsChmod(filename, 0666);
																						}


																						if (rr == 0)
																						{
																							ssend(conn_s, "226 O\r\n");
																						}
																						else
																						{
																							ssend(conn_s, "451 E\r\n");
																						}
																					}
																					else
																					{
																						ssend(conn_s, "501 E\r\n");
																					}
																				}
																				else
																				{
																					ssend(conn_s, "425 E\r\n");
																				}
																			}
																			else
																				if (strcasecmp(cmd, "SIZE") == 0)
																				{
																					if (split == 1)
																					{
																						absPath(filename, param, cwd);
																						if (cellFsStat(filename, &buf) == CELL_FS_SUCCEEDED)
																						{
																							sprintf(buffer, "213 %llu\r\n", (unsigned long long)buf.st_size);
																							ssend(conn_s, buffer);
																							dataactive = 1;
																						}
																						else
																						{
																							ssend(conn_s, "550 E\r\n");
																						}
																					}
																					else
																					{
																						ssend(conn_s, "501 E\r\n");
																					}
																				}
																				else
																					if (strcasecmp(cmd, "SYST") == 0)
																					{
																						ssend(conn_s, "215 UNIX Type: L8\r\n");
																					}
																					else
																						if (strcasecmp(cmd, "MDTM") == 0)
																						{
																							if (split == 1)
																							{
																								absPath(filename, param, cwd);
																								if (cellFsStat(filename, &buf) == CELL_FS_SUCCEEDED)
																								{
																									cellRtcSetTime_t(&rDate, buf.st_mtime);
																									sprintf(buffer, "213 %04i%02i%02i%02i%02i%02i\r\n", rDate.year, rDate.month, rDate.day, rDate.hour, rDate.minute, rDate.second);
																									ssend(conn_s, buffer);
																								}
																								else
																								{
																									ssend(conn_s, "550 E\r\n");
																								}
																							}
																							else
																							{
																								ssend(conn_s, "501 E\r\n");
																							}
																						}
																						else
																							if (strcasecmp(cmd, "ABOR") == 0)
																							{
																								sclose(&data_s);
																								ssend(conn_s, "226 ABOR OK\r\n");
																							}

																							else
																								if (strcasecmp(cmd, "RNFR") == 0)
																								{
																									if (split == 1)
																									{
																										absPath(rnfr, param, cwd);

																										if (cellFsStat(rnfr, &buf) == CELL_FS_SUCCEEDED)
																										{
																											ssend(conn_s, "350 RNFR OK\r\n");
																										}
																										else
																										{
																											ssend(conn_s, "550 RNFR E\r\n");
																										}
																									}
																									else
																									{
																										ssend(conn_s, "501 E\r\n");
																									}
																								}

																								else
																									if (strcasecmp(cmd, "RNTO") == 0)
																									{
																										if (split == 1)
																										{
																											absPath(filename, param, cwd);

																											if (cellFsRename(rnfr, filename) == CELL_FS_SUCCEEDED)
																											{
																												ssend(conn_s, "250 O\r\n");
																											}
																											else
																											{
																												ssend(conn_s, "550 E\r\n");
																											}
																										}
																										else
																										{
																											ssend(conn_s, "501 E\r\n");
																										}
																									}

																									else
																										if (strcasecmp(cmd, "USER") == 0 || strcasecmp(cmd, "PASS") == 0)
																										{
																											ssend(conn_s, "230 Already in\r\n");
																										}
																										else
																											if (strcasecmp(cmd, "OPTS") == 0
																												|| strcasecmp(cmd, "REIN") == 0 || strcasecmp(cmd, "ADAT") == 0
																												|| strcasecmp(cmd, "AUTH") == 0 || strcasecmp(cmd, "CCC") == 0
																												|| strcasecmp(cmd, "CONF") == 0 || strcasecmp(cmd, "ENC") == 0
																												|| strcasecmp(cmd, "EPRT") == 0 || strcasecmp(cmd, "EPSV") == 0
																												|| strcasecmp(cmd, "LANG") == 0 || strcasecmp(cmd, "LPRT") == 0
																												|| strcasecmp(cmd, "LPSV") == 0 || strcasecmp(cmd, "MIC") == 0
																												|| strcasecmp(cmd, "PBSZ") == 0 || strcasecmp(cmd, "PROT") == 0
																												|| strcasecmp(cmd, "SMNT") == 0 || strcasecmp(cmd, "STOU") == 0
																												|| strcasecmp(cmd, "XRCP") == 0 || strcasecmp(cmd, "XSEN") == 0
																												|| strcasecmp(cmd, "XSEM") == 0 || strcasecmp(cmd, "XRSQ") == 0
																												|| strcasecmp(cmd, "STAT") == 0)
																											{
																												ssend(conn_s, "502 Not implemented\r\n");
																											}
																											else
																											{
																												ssend(conn_s, "502 E\r\n");
																											}

				if (dataactive == 1)
				{
					dataactive = 0;
				}
				else
				{
					sclose(&data_s);
				}

			}
			else
			{
				// available commands when not logged in
				if (strcasecmp(cmd, "USER") == 0)
				{
					if (split == 1)
					{
						ssend(conn_s, "331 O\r\n");
					}
					else
					{
						ssend(conn_s, "501 E\r\n");
					}
				}
				else
					if (strcasecmp(cmd, "PASS") == 0)
					{
						if (split == 1)
						{
							if (DISABLE_PASS)// || (strcmp(D_USER, user) == 0 && strcmp(userpass, param) == 0))
							{
								ssend(conn_s, "230 O\r\n");
								loggedin = 1;
							}
							else
							{
								ssend(conn_s, "430 E\r\n");
							}
						}
						else
						{
							ssend(conn_s, "501 E\r\n");
						}
					}
					else
						if (strcasecmp(cmd, "QUIT") == 0 || strcasecmp(cmd, "BYE") == 0)
						{
							ssend(conn_s, "221 O\r\n");
							connactive = 0;
						}
						else
						{
							ssend(conn_s, "530 E\r\n");
						}
			}
		}
		
		sys_timer_usleep(1668);
		sys_ppu_thread_yield();

	}
	printf("FTP: closed thread! \n");
	sclose(&conn_s);
	sclose(&data_s);
	sys_ppu_thread_exit(0);
}