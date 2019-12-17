// Mysis system_plugin.h v0.1
typedef struct
{
	int (*DoUnk0)(const char *, int);
	int (*DoUnk1)(float,int);
	int (*DoUnk2)(float *);
	int (*DoUnk3)(float *);
	int (*DoUnk4)(int *);
	int (*DoUnk5)(int *);
	int (*DoUnk6)(void);
	int (*DoUnk7)(int, float);
	int (*DoUnk8)(int, float);
	int (*DoUnk9)(float, float);
	int (*DoUnk10)(int, float);
	int (*DoUnk11)(int *, int *,const wchar_t *, const char *, int);
	int (*DoUnk12)(int);
	int (*DoUnk13)(void);
	int (*DoUnk14)(int *);
	int (*DoUnk15)(void *);
	int (*DoUnk16)(void *);
	int (*DoUnk17)(void);
	int (*DoUnk18)(void);
	int (*DoUnk19)(int);
	int (*DoUnk20)(int);
	int (*DoUnk21)(int);
	int (*DoUnk22)(int);
	int (*DoUnk23)(int);
	int (*DoUnk24)(int);
	int (*DoUnk25)(void *, const wchar_t *);
	int (*DoUnk26)(void *, const wchar_t *);
	int (*DoUnk27)(void *, const wchar_t *);
	int (*DoUnk28)(void *, const wchar_t *);
	int (*DoUnk29)(void);
	int (*DoUnk30)(const char *);
	int (*saveBMP)(const char *); // XMB ScreenDump
	int (*DoUnk32)(void);
	int (*DoUnk33)(void);
	int (*DoUnk34)(void);
	int (*DoUnk35)(void);
} system_plugin_interface;

system_plugin_interface * system_interface;
