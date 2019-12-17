// Mysis nas_plugin.h v0.1
typedef struct
{
	int (*DoUnk0)(int);
	int (*DoUnk1)(int,int);
	int (*DoUnk2)(void);
	int (*DoUnk3)(void);
	int (*DoUnk4)(void *,int,int,int,void *,int);
	int (*DoUnk5)(char *,int,int,void*,int);
	int (*DoUnk6_SetCallback)(void *,int);
	int (*DoUnk7)(void *);
	int (*DoUnk8)(char * username, char * password);
	int (*DoUnk9)(void);
	int (*DoUnk10)(void *);
	int (*DoUnk11)(void *);
	int (*DoUnk12)(void *);
	int (*DoUnk13)(void *);
	int (*DoUnk14)(void *);
	int (*DoUnk15)(void *);
	int (*DoUnk16)(void *);
	int (*DoUnk17)(void *);
	int (*DoUnk18)(void *);
	int (*DoUnk19)(int * bgdl_id);
	int (*DoUnk20_installerDrmActivation)(int drmActivationItemAction,int drmActivationItemTarget,int drmActivationItemOption);
	int (*DoUnk21)(char *,const void * input,int len);
	int (*DoUnk22_installerDownload)(void);
	int (*DoUnk23)(int, void *);
	int (*DoUnk24)(char *);
	int (*DoUnk25)(int task_id, int flags);
	int (*DoUnk26)(void);
	int (*DoUnk27)(void *);
	int (*DoUnk28)(char * path);
	int (*DoUnk29)(unsigned long long *,unsigned long long *);
	int (*DoUnk30)(void *);
	int (*DoUnk31)(void *);
	int (*DoUnk32_ResumeMarlinVideoStreaming)(void * callback); //?
	int (*DoUnk33)(char *,int,unsigned long long *,unsigned long long *);
	int (*DoUnk34)(void *);
} nas_plugin_interface;

nas_plugin_interface * nas_interface;
