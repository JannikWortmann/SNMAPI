// Mysis impose_plugin.h v0.1
typedef struct
{
	int (*DoUnk0)(int,void *);
	int (*DoUnk1)(void);
	int (*DoUnk2)(void);
	int (*DoUnk3)(int);
	int (*DoUnk4)(void);
	int (*DoUnk5)(void);
	int (*DoUnk6)(void);
	int (*DoUnk7)(void);
	int (*DoUnk8)(void);
	int (*DoUnk9)(void);
	int (*DoUnk10)(void);
	int (*DoUnk11)(void);
	int (*DoUnk12)(void);
	int (*DoUnk13_start_xmb)(int);
	int (*DoUnk14)(void);
	int (*DoUnk15)(void);
	int (*DoUnk16)(void);
	int (*DoUnk17)(int);
	int (*DoUnk18)(void);
} impose_plugin_interface;

impose_plugin_interface * impose_interface;
