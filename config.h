/* See LICENSE file for copyright and license details. */
// Constants
#define TERMINAL "st"
#define TERMCLASS "St"
#include <X11/XF86keysym.h>

/* appearance */

static unsigned int borderpx  = 3;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const char *fonts[]          = { "monospace:size=10", "NotoColorEmoji:pixelsize=10:antialias=true:autohint=true" };
static unsigned int gappih    = 20;       /* horiz inner gap between windows */
static unsigned int gappiv    = 10;       /* vert inner gap between windows */
static unsigned int gappoh    = 15;       /* horiz outer gap between windows and screen edge */
static unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const Bool viewontag         = True;     /* Switch view on tag switch */
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char floatbordercolor[]      = "#770000";
static char selbgcolor[]            = "#005577";
static char *colors[][4] = {
       /*               fg           bg           border           floatborder         */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor, normbordercolor  },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor,  floatbordercolor },
 };

static const XPoint stickyicon[]    = { {0,0}, {4,0}, {4,8}, {2,6}, {0,8}, {0,0} }; /* represents the icon as an array of vertices */
static const XPoint stickyiconbb    = {4,8};	/* defines the bottom right corner of the polygon's bounding box (speeds up scaling) */

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"st", "-n", "spcalc", "-f", "monospace:size=16", "-g", "50x20", "-e", "bc", "-lq", NULL };
const char *spcmd3[] = {"st", "-n", "spnotes", "-g", "90x30", "-e", "notes", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
	{"spcalc",      spcmd2},
	{"spnotes",     spcmd3},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating		isterminal		nowswallow	 monitor */

	{ TERMCLASS,  NULL,	  NULL,		0,	     0,			1,			0,		  -1 },
	{ "Gimp",     NULL,	  NULL,		0,	     1,			0,			0,		  -1 },
	{ "Firefox",  NULL,	  NULL,		1 << 8,	     0,		        0,		       -1,		  -1 },
	{ NULL,	      "spterm",   NULL,		SPTAG(0),    1,			1,			0,		  -1 },
	{ NULL,	      "spcalc",   NULL,		SPTAG(1),    1,			1,			0,		  -1 },
	{ NULL,	      "spnotes",  NULL,		SPTAG(2),    1,			1,			0,		  -1 },
	{ NULL,      NULL,       "Event Tester",  0,         0,                 0,                      1,                -1 }, /* xev */
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"
#include "shiftview.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|Mod1Mask,              KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      ttag,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_z,     ACTION##stack, {.i = 0 } }, \
	/* { MOD, XK_z,     ACTION##stack, {.i = PREVSEL } }, \ */
	/* { MOD, XK_a,     ACTION##stack, {.i = 1 } }, \ */
	/* { MOD, XK_z,     ACTION##stack, {.i = 2 } }, \ */
	/* { MOD, XK_x,     ACTION##stack, {.i = -1 } }, */

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }
#define STATUSBAR "dwmblocks"

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		/* { "font",               STRING,  &font }, */
		/* { "dmenufont",          STRING,  &dmenufont }, */
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",               INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",       	FLOAT,   &mfact },
		{ "gappih",		INTEGER, &gappih },
		{ "gappiv",		INTEGER, &gappiv },
		{ "gappoh",		INTEGER, &gappoh },
		{ "gappov",		INTEGER, &gappov },
		{ "swallowfloating",	INTEGER, &swallowfloating },
		{ "smartgaps",		INTEGER, &smartgaps },
};

/* commands */
static const char *termcmd[]  = { "st", NULL };

static Key keys[] = {
	/* modifier                     key			   function        argument */
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	{ MODKEY,                       XK_Return,		   spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,			   togglebar,      {0} },
	{ MODKEY,                       XK_c,			   incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_c,			   incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,			   setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,			   setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_a,			   shiftview,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_a,			   shifttag,       {.i = -1 } },
	{ MODKEY,                       XK_g,		   	   shiftview,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_g,		           shifttag,       {.i = +1 } },
	{ MODKEY|Mod1Mask,              XK_u,			   incrgaps,       {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_u,			   incrgaps,       {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_Return,		   zoom,           {0} },
	{ MODKEY,                       XK_Tab,			   view,           {0} },
	{ MODKEY,                       XK_x,			   killclient,     {0} },
	{ MODKEY,                       XK_s,			   togglesticky,   {0} },
	{ MODKEY|ShiftMask,             XK_s,			   focussticky,    {0} },
	{ MODKEY,                       XK_t,			   setlayout,      {.v = &layouts[0]} }, // Tile
	{ MODKEY,                       XK_o,			   setlayout,      {.v = &layouts[1]} }, // Spiral
	{ MODKEY|ShiftMask,             XK_o,			   setlayout,      {.v = &layouts[2]} }, // Dwindle
	{ MODKEY|ShiftMask,             XK_t,			   setlayout,      {.v = &layouts[3]} }, // Deck
	{ MODKEY,                       XK_u,			   setlayout,      {.v = &layouts[4]} }, // bstack
	{ MODKEY|ShiftMask,             XK_u,			   setlayout,      {.v = &layouts[5]} }, // bstackhoriz
	{ MODKEY,                       XK_p,			   setlayout,      {.v = &layouts[6]} }, // grid
	{ MODKEY|ShiftMask,             XK_p,			   setlayout,      {.v = &layouts[7]} }, // nrowgrid
	{ MODKEY,                       XK_y,			   setlayout,      {.v = &layouts[8]} }, // horizgrid
	{ MODKEY|ShiftMask,             XK_y,			   setlayout,      {.v = &layouts[9]} }, // gaplessgrid
	{ MODKEY,                       XK_i,			   setlayout,      {.v = &layouts[10]} }, // centeredmaster
	{ MODKEY|ShiftMask,             XK_i,			   setlayout,      {.v = &layouts[11]} }, // centeredfloatingmaster
	{ MODKEY,                       XK_f,			   setlayout,      {.v = &layouts[12]} }, // floating
	{ MODKEY|ShiftMask,             XK_f,			   togglefullscr,  {0} }, //actualfullscreen
	{ MODKEY,                       XK_space,		   togglefloating, {0} }, //change selected window to floating mode
	{ MODKEY,                       XK_0,			   view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,			   tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,		   focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,		   focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,		   tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,		   tagmon,         {.i = +1 } },
	{ MODKEY,            		XK_BackSpace,		   togglescratch,  {.ui = 0 } },//normal scratchpad
	{ MODKEY|ShiftMask,	   	XK_BackSpace,		   togglescratch,  {.ui = 1 } },//calculator scratchpad
	{ MODKEY,                       XK_n,			   togglescratch,  {.ui = 2 } },//notetaking scratchpad
	{ MODKEY|ShiftMask,             XK_n,			   spawn,         {.v = (const char*[]) { TERMINAL, "-e", "nvim", "-c", "VimwikiIndex", NULL } } },
	{ MODKEY,                       XK_d,			   spawn,          SHCMD("dmenu_run -c -g 1 -l 5") },
	{ MODKEY|ShiftMask,             XK_d,			   spawn,          SHCMD("passmenu") },
	{ MODKEY,                       XK_w,			   spawn,          SHCMD("$BROWSER") },
	{ MODKEY|ShiftMask,             XK_w,			   spawn,          SHCMD("$BROWSER --private-window") },
	{ MODKEY,                       XK_m,			   spawn,         SHCMD(TERMINAL " -e ncmpcpp") }, 
	{ MODKEY,                       XK_e,			   spawn,         SHCMD(TERMINAL " -e neomutt") }, 
	{ MODKEY,                       XK_r,			   spawn,         SHCMD(TERMINAL " -e lfub") },
	{ MODKEY|ShiftMask,             XK_r,			   spawn,         SHCMD(TERMINAL " -e tremc") },
	{ MODKEY,         		XK_v,		           spawn,         SHCMD(TERMINAL " -e pulsemixer; pkill -RTMIN+10 dwmblocks") }, 
	{ MODKEY|ShiftMask,   		XK_v,		           spawn,         SHCMD(TERMINAL " -e calcurse") }, 
	{ 0,				XF86XK_MonBrightnessDown,  spawn,         SHCMD("xbacklight -dec 1; pkill -RTMIN+9 dwmblocks") },
	{ 0,				XF86XK_MonBrightnessUp,    spawn,         SHCMD("xbacklight -inc 1; pkill -RTMIN+9 dwmblocks") },
	{ 0,				XF86XK_AudioMute,          spawn,         SHCMD("pamixer -t; pkill -RTMIN+10 dwmblocks") },
	{ 0,				XF86XK_AudioLowerVolume,   spawn,         SHCMD("pamixer --allow-boost -d 1; pkill -RTMIN+10 dwmblocks") },
	{ 0,				XF86XK_AudioRaiseVolume,   spawn,         SHCMD("pamixer --allow-boost -i 1; pkill -RTMIN+10 dwmblocks") },
	{ 0,				XF86XK_AudioPrev,          spawn,         SHCMD("mpc prev") },
	{ 0,				XF86XK_AudioPlay,          spawn,         SHCMD("mpc | grep -iq playing && mpc -q pause || mpc -q play") },
	{ 0,				XF86XK_AudioNext,          spawn,         SHCMD("mpc next") },
	{ 0,				XK_Print,                  spawn,         SHCMD("screenshots") },
	{ 0,				XK_F1,                     spawn,         SHCMD("dmenumount") },
	{ 0,			        XK_F2,                     spawn,         SHCMD("dmenuumount") },
	TAGKEYS(                        XK_1,					  0)
	TAGKEYS(                        XK_2,					  1)
	TAGKEYS(                        XK_3,					  2)
	TAGKEYS(                        XK_4,					  3)
	TAGKEYS(                        XK_5,					  4)
	TAGKEYS(                        XK_6,					  5)
	TAGKEYS(                        XK_7,					  6)
	TAGKEYS(                        XK_8,					  7)
	TAGKEYS(                        XK_9,					  8)
	{ MODKEY|ShiftMask,             XK_q,			   quit,          {0} },
	{ MODKEY|ControlMask,           XK_q,			   quit,          {1} },
};

	/* { MODKEY|Mod1Mask,              XK_i,			   incrigaps,      {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_i,			   incrigaps,      {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_o,			   incrogaps,      {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_o,			   incrogaps,      {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_6,			   incrihgaps,     {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_6,			   incrihgaps,     {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_7,			   incrivgaps,     {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_7,			   incrivgaps,     {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_8,			   incrohgaps,     {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_8,			   incrohgaps,     {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_9,			   incrovgaps,     {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_9,			   incrovgaps,     {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_0,			   togglegaps,     {0} }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_0,			   defaultgaps,    {0} }, */

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigstatusbar,   {.i = 4} },
	{ ClkStatusText,        0,              Button5,        sigstatusbar,   {.i = 5} },
	{ ClkStatusText,        ShiftMask,      Button1,        sigstatusbar,   {.i = 6} },
	{ ClkStatusText,        ShiftMask,      Button3,        spawn,          SHCMD(TERMINAL "-e nvim ~/Downloads/dwmblocks/config.h") },
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button1,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
