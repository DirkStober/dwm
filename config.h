/* See LICENSE file for copyright and license details. */
#include "X11/XF86keysym.h"

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { 
	"monospace:size=16",
	"JetBrains Mono Nerd Font:size=16"  
	};
static const char dmenufont[]       = "monospace:size=18";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_red[]         = "#ff0000";
static const char col_green[]      = "#00ff00";
static const char col_blue[]       = "#0000ff";
static const char col_yellow[]      = "#FFFF00";
static const char col_white[]       = "#ffffff";
static const char aurora_green[]    = "#6adc99";

enum { SchemeNorm, SchemeCol1, SchemeCol2, SchemeCol3, SchemeCol4,
       SchemeCol5, SchemeCol6, SchemeSel }; /* color schemes */

static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm]  = { col_gray3, col_gray1, col_gray2 },
	[SchemeCol1]  = { col_red,      col_gray1, col_gray2 },
	[SchemeCol2]  = { col_green,      col_gray1, col_gray2 },
	[SchemeCol3]  = { col_blue,      col_gray1, col_gray2 },
	[SchemeCol4]  = { col_white,      col_gray1, col_gray2 },
	[SchemeCol5]  = { col_yellow ,      col_gray1, col_gray2 },
	[SchemeCol6]  = { aurora_green,col_gray1, col_gray2 },
	[SchemeSel]   = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

#define MATLAB_CONSOLE "dirk@dirk-thinkpad: matlab -nodesktop"

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title       tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Firefox", 	NULL,   NULL,       	1 << 8,	0,	0,	-1,	-1 },
	{ "Brave", 	NULL,	NULL,          	0,	0,	0,	-1,	-1 },
	{ "MATLAB R2022a - academic use","sun-awt-X11-XFramePeer",NULL,0,	0,	0,	1,	-1},
	{ "MATLABWindow","MATLABWindow",NULL,	0,	0,	0,	1,	-1},
	{ NULL,		"Matlab-GLEE",	NULL,	0,	0,	0,	1,	-1},
	{ "Alacritty",	NULL,  	NULL,           0,	0,	1,	0,	-1 },
	{ "Alacritty",	"MTerm",NULL,		0,     	1,	1,	0,	-1 },
	{ NULL,      NULL,     "Event Tester", 	0,      0,	0,	1,	-1 }, /* xev */
};
/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "alacritty", NULL };
static const char *webcmd[]  = { "brave", NULL };
static const char *emailcmd[]  = { "alacritty", "-e", "neomutt", NULL };
static const char *xcompmgrcmd[] = {"compmgr",NULL};
static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY|ShiftMask,	XK_p,		spawn,          {.v = dmenucmd } },
	{ MODKEY,		XK_p,		spawn,		SHCMD("passmenu")},
	{ MODKEY|ShiftMask,     XK_Return, 	spawn,          {.v = termcmd } },
	{ MODKEY,	        XK_w, 	   	spawn,          {.v = webcmd } },
	{ MODKEY,	        XK_e, 	   	spawn,          {.v = emailcmd} },
	{ MODKEY,		XK_minus,   spawn,	 SHCMD("pamixer --allow-boost -d 5; pkill -45 dwmblocks")},
	{ MODKEY,		XK_equal,	spawn,	 SHCMD("pamixer --allow-boost -i 5; pkill -45 dwmblocks")},
	{ MODKEY,               XK_b,      togglebar,      {0} },
	{ MODKEY,               XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,               XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,               XK_r,      rotatestack,    {.i = +1 } },
	{ MODKEY,               XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,               XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,               XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,               XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,               XK_Return, zoom,           {0} },
	{ MODKEY,               XK_Tab,    view,           {0} },
	{ MODKEY,             	XK_c,      killclient,     {0} },
	{ MODKEY,               XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,               XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,               XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,               XK_u,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,               XK_o,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,               XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,     XK_space,  togglefloating, {0} },
	{ MODKEY,               XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,     XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,               XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,               XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,     XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,     XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,     XK_o, spawn,         	{.v = xcompmgrcmd} },
	{ MODKEY,             	XK_bracketleft,	setborderpx,{.i = -1 } },
	{ MODKEY,             	XK_bracketright, setborderpx,    {.i = +1 } },
	{ MODKEY,             	XK_backslash, 	setborderpx,    {.i = 0 } },
	TAGKEYS(                XK_1,                      0)
	TAGKEYS(                XK_2,                      1)
	TAGKEYS(                XK_3,                      2)
	TAGKEYS(                XK_4,                      3)
	TAGKEYS(                XK_5,                      4)
	TAGKEYS(                XK_6,                      5)
	TAGKEYS(                XK_7,                      6)
	TAGKEYS(                XK_8,                      7)
	TAGKEYS(                XK_9,                      8)
	{ MODKEY|ShiftMask,     XK_q,      quit,           {0} },
// Special keys lenovo laptop
	{ 0,				XF86XK_AudioMute,		spawn,		SHCMD("pamixer -t 0; pkill -45 dwmblocks")},
	{ 0,				XF86XK_AudioLowerVolume,	spawn,		SHCMD("pamixer --allow-boost -d 5; pkill -45 dwmblocks")},
	{ 0,				XF86XK_AudioRaiseVolume,	spawn,		SHCMD("pamixer --allow-boost -i 5; pkill -45 dwmblocks")},
	{ 0,				XF86XK_AudioMicMute,		spawn,		SHCMD("pactl set-source-mute 1 toggle; pkill -45 dwmblocks")},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

