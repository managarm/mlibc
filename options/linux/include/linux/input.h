
#ifndef _LINUX_INPUT_H
#define _LINUX_INPUT_H

#include <stdint.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <bits/posix/timeval.h>

struct input_id {
	uint16_t bustype;
	uint16_t vendor;
	uint16_t product;
	uint16_t version;
};

struct input_event {
	struct timeval time;
	uint16_t type;
	uint16_t code;
	int32_t value;
};

struct input_absinfo {
	int32_t value;
	int32_t minimum;
	int32_t maximum;
	int32_t fuzz;
	int32_t flat;
	int32_t resolution;
};

//----------------------------------
// Event Types
//----------------------------------

#define EV_SYN 0x00
#define EV_KEY 0x01
#define EV_REL 0x02
#define EV_ABS 0x03
#define EV_LED 0x11
#define EV_MAX 0x1F
#define EV_CNT (EV_MAX + 1)

#define EVIOCGVERSION _IOR('E', 0x01, int)
#define EVIOCGID _IOR('E', 0x02, struct input_id)
#define EVIOCSKEYCODE _IOW('E', 0x04, unsigned int[2])
#define EVIOCGNAME(len) _IOC(_IOC_READ, 'E', 0x06, len)
#define EVIOCGPHYS(len) _IOC(_IOC_READ, 'E', 0x07, len)
#define EVIOCGUNIQ(len) _IOC(_IOC_READ, 'E', 0x08, len)
#define EVIOCGPROP(len) _IOC(_IOC_READ, 'E', 0x09, len)
#define EVIOCGKEY(len) _IOC(_IOC_READ, 'E', 0x18, len)
#define EVIOCGLED(len) _IOC(_IOC_READ, 'E', 0x19, len)
#define EVIOCGSND(len) _IOC(_IOC_READ, 'E', 0x1a, len)
#define EVIOCGSW(len) _IOC(_IOC_READ, 'E', 0x1b, len)
#define EVIOCGBIT(ev, len) _IOC(_IOC_READ, 'E', 0x20 + (ev), len)
#define EVIOCGABS(abs) _IOR('E', 0x40 + (abs),struct input_absinfo) 
#define EVIOCGRAB _IOW('E', 0x90, int)
#define EVIOSCLOCKID _IOW('E', 0xa0, int)
#define EVIOCSABS(abs) _IOW('E', 0xc0 + (abs), struct input_absinfo)

//----------------------------------
// Sync Types
//----------------------------------

#define SYN_REPORT 0
#define SYN_DROPPED 3

//----------------------------------
// Key Codes
//----------------------------------

#define KEY_RESERVED 0
#define KEY_ESC	1
#define KEY_1 2
#define KEY_2 3
#define KEY_3 4
#define KEY_4 5
#define KEY_5 6
#define KEY_6 7
#define KEY_7 8
#define KEY_8 9
#define KEY_9 10
#define KEY_0 11
#define KEY_MINUS 12
#define KEY_EQUAL 13
#define KEY_BACKSPACE 14
#define KEY_TAB	15
#define KEY_Q 16
#define KEY_W 17
#define KEY_E 18
#define KEY_R 19
#define KEY_T 20
#define KEY_Y 21
#define KEY_U 22
#define KEY_I 23
#define KEY_O 24
#define KEY_P 25
#define KEY_LEFTBRACE 26
#define KEY_RIGHTBRACE 27
#define KEY_ENTER 28
#define KEY_LEFTCTRL 29
#define KEY_A 30
#define KEY_S 31
#define KEY_D 32
#define KEY_F 33
#define KEY_G 34
#define KEY_H 35
#define KEY_J 36
#define KEY_K 37
#define KEY_L 38
#define KEY_SEMICOLON 39
#define KEY_APOSTROPHE 40
#define KEY_GRAVE 41
#define KEY_LEFTSHIFT 42
#define KEY_BACKSLASH 43
#define KEY_Z 44
#define KEY_X 45
#define KEY_C 46
#define KEY_V 47
#define KEY_B 48
#define KEY_N 49
#define KEY_M 50
#define KEY_COMMA 51
#define KEY_DOT 52
#define KEY_SLASH 53
#define KEY_RIGHTSHIFT 54
#define KEY_KPASTERISK 55
#define KEY_LEFTALT 56
#define KEY_SPACE 57
#define KEY_CAPSLOCK 58
#define KEY_F1 59
#define KEY_F2 60
#define KEY_F3 61
#define KEY_F4 62
#define KEY_F5 63
#define KEY_F6 64
#define KEY_F7 65
#define KEY_F8 66
#define KEY_F9 67
#define KEY_F10 68
#define KEY_NUMLOCK 69
#define KEY_SCROLLLOCK 70
#define KEY_KP7 71
#define KEY_KP8	72
#define KEY_KP9	73
#define KEY_KPMINUS	74
#define KEY_KP4	75
#define KEY_KP5	76
#define KEY_KP6	77
#define KEY_KPPLUS 78
#define KEY_KP1	79
#define KEY_KP2	80
#define KEY_KP3	81
#define KEY_KP0	82
#define KEY_KPDOT 83

#define KEY_ZENKAKUHANKAKU 85
#define KEY_102ND 86
#define KEY_F11	87
#define KEY_F12	88
#define KEY_RO 89
#define KEY_KATAKANA 90
#define KEY_HIRAGANA 91
#define KEY_HENKAN 92
#define KEY_KATAKANAHIRAGANA 93
#define KEY_MUHENKAN 94
#define KEY_KPJPCOMMA 95
#define KEY_KPENTER 96
#define KEY_RIGHTCTRL 97
#define KEY_KPSLASH	98
#define KEY_SYSRQ 99
#define KEY_RIGHTALT 100
#define KEY_LINEFEED 101
#define KEY_HOME 102
#define KEY_UP 103
#define KEY_PAGEUP 104
#define KEY_LEFT 105
#define KEY_RIGHT 106
#define KEY_END	107
#define KEY_DOWN 108
#define KEY_PAGEDOWN 109
#define KEY_INSERT 110
#define KEY_DELETE 111
#define KEY_MACRO 112
#define KEY_MUTE 113
#define KEY_VOLUMEDOWN 114
#define KEY_VOLUMEUP 115
#define KEY_POWER 116	
#define KEY_KPEQUAL 117
#define KEY_KPPLUSMINUS 118
#define KEY_PAUSE 119
#define KEY_SCALE 120	

#define KEY_KPCOMMA 121
#define KEY_HANGEUL 122
#define KEY_HANGUEL KEY_HANGEUL
#define KEY_HANJA 123
#define KEY_YEN	124
#define KEY_LEFTMETA 125
#define KEY_RIGHTMETA 126
#define KEY_COMPOSE	127

#define KEY_STOP 128	
#define KEY_AGAIN 129
#define KEY_PROPS 130	
#define KEY_UNDO 131	
#define KEY_FRONT 132
#define KEY_COPY 133	
#define KEY_OPEN 134	
#define KEY_PASTE 135	
#define KEY_FIND 136	
#define KEY_CUT	137	
#define KEY_HELP 138	
#define KEY_MENU 139	
#define KEY_CALC 140	
#define KEY_SETUP 141
#define KEY_SLEEP 142	
#define KEY_WAKEUP 143	
#define KEY_FILE 144	
#define KEY_SENDFILE 145
#define KEY_DELETEFILE 146
#define KEY_XFER 147
#define KEY_PROG1 148
#define KEY_PROG2 149
#define KEY_WWW	150	
#define KEY_MSDOS 151
#define KEY_COFFEE 152	
#define KEY_SCREENLOCK KEY_COFFEE
#define KEY_ROTATE_DISPLAY 153	
#define KEY_DIRECTION KEY_ROTATE_DISPLAY
#define KEY_CYCLEWINDOWS 154
#define KEY_MAIL 155
#define KEY_BOOKMARKS 156	
#define KEY_COMPUTER 157
#define KEY_BACK 158	
#define KEY_FORWARD 	159	
#define KEY_CLOSECD	160
#define KEY_EJECTCD 161
#define KEY_EJECTCLOSECD 162
#define KEY_NEXTSONG 163
#define KEY_PLAYPAUSE 164
#define KEY_PREVIOUSSONG 165
#define KEY_STOPCD 166
#define KEY_RECORD 167
#define KEY_REWIND 168
#define KEY_PHONE 169	
#define KEY_ISO	170
#define KEY_CONFIG 171	
#define KEY_HOMEPAGE 172	
#define KEY_REFRESH	173	
#define KEY_EXIT 174	
#define KEY_MOVE 175
#define KEY_EDIT 176
#define KEY_SCROLLUP 177
#define KEY_SCROLLDOWN 178
#define KEY_KPLEFTPAREN 179
#define KEY_KPRIGHTPAREN 180
#define KEY_NEW	181	
#define KEY_REDO 182	

#define KEY_F13	183
#define KEY_F14	184
#define KEY_F15	185
#define KEY_F16	186
#define KEY_F17	187
#define KEY_F18	188
#define KEY_F19	189
#define KEY_F20	190
#define KEY_F21	191
#define KEY_F22	192
#define KEY_F23	193
#define KEY_F24	194

#define KEY_PLAYCD 200
#define KEY_PAUSECD 201
#define KEY_PROG3 202
#define KEY_PROG4 203
#define KEY_DASHBOARD 204	
#define KEY_SUSPEND 205
#define KEY_CLOSE 206	
#define KEY_PLAY 207
#define KEY_FASTFORWARD 208
#define KEY_BASSBOOST 209
#define KEY_PRINT 210	
#define KEY_HP 211
#define KEY_CAMERA 212
#define KEY_SOUND 213
#define KEY_QUESTION 214
#define KEY_EMAIL 215
#define KEY_CHAT 216
#define KEY_SEARCH 217
#define KEY_CONNECT 218
#define KEY_FINANCE 219	
#define KEY_SPORT 220
#define KEY_SHOP 221
#define KEY_ALTERASE 222
#define KEY_CANCEL 223	
#define KEY_BRIGHTNESSDOWN 224
#define KEY_BRIGHTNESSUP 225
#define KEY_MEDIA 226

#define KEY_SWITCHVIDEOMODE	227	
#define KEY_KBDILLUMTOGGLE 228
#define KEY_KBDILLUMDOWN 229
#define KEY_KBDILLUMUP 230

#define KEY_SEND 231	
#define KEY_REPLY 232	
#define KEY_FORWARDMAIL 233
#define KEY_SAVE 234
#define KEY_DOCUMENTS 235

#define KEY_BATTERY 236

#define KEY_BLUETOOTH 237
#define KEY_WLAN 238
#define KEY_UWB	239

#define KEY_UNKNOWN 240

#define KEY_VIDEO_NEXT 241
#define KEY_VIDEO_PREV 242
#define KEY_BRIGHTNESS_CYCLE 243
#define KEY_BRIGHTNESS_AUTO	244
#define KEY_BRIGHTNESS_ZERO	KEY_BRIGHTNESS_AUTO
#define KEY_DISPLAY_OFF 245

#define KEY_WWAN 246
#define KEY_WIMAX KEY_WWAN
#define KEY_RFKILL 247

#define KEY_MICMUTE 248

//----------------------------------
// Button Codes
//----------------------------------

#define BTN_MISC 0x100
#define BTN_1 0x101

#define BTN_MOUSE 0x110
#define BTN_LEFT 0x110
#define BTN_RIGHT 0x111
#define BTN_MIDDLE 0x112
#define BTN_SIDE 0x113
#define BTN_EXTRA 0x114

#define BTN_TRIGGER 0x120

#define BTN_SOUTH 0x130
#define BTN_A BTN_SOUTH

#define BTN_TOOL_PEN 0x140
#define BTN_TOOL_FINGER	0x145
#define BTN_TOUCH 0x14A
#define BTN_STYLUS 0x14B

#define KEY_OK 0x160
#define KEY_SELECT 0x161
#define KEY_GOTO 0x162
#define KEY_CLEAR 0x163
#define KEY_POWER2 0x164
#define KEY_OPTION 0x165
#define KEY_INFO 0x166
#define KEY_TIME 0x167
#define KEY_VENDOR 0x168
#define KEY_ARCHIVE 0x169
#define KEY_PROGRAM 0x16a
#define KEY_CHANNEL 0x16b
#define KEY_FAVORITES 0x16c
#define KEY_EPG	0x16d
#define KEY_PVR	0x16e
#define KEY_MHP	0x16f
#define KEY_LANGUAGE 0x170
#define KEY_TITLE 0x171
#define KEY_SUBTITLE 0x172
#define KEY_ANGLE 0x173
#define KEY_ZOOM 0x174
#define KEY_MODE 0x175
#define KEY_KEYBOARD 0x176
#define KEY_SCREEN 0x177
#define KEY_PC 0x178
#define KEY_TV 0x179	
#define KEY_TV2	0x17a
#define KEY_VCR	0x17b
#define KEY_VCR2 0x17c
#define KEY_SAT	0x17d
#define KEY_SAT2 0x17e
#define KEY_CD 0x17f
#define KEY_TAPE 0x180
#define KEY_RADIO 0x181
#define KEY_TUNER 0x182
#define KEY_PLAYER 0x183
#define KEY_TEXT 0x184
#define KEY_DVD 0x185
#define KEY_AUX 0x186
#define KEY_MP3 0x187
#define KEY_AUDIO 0x188
#define KEY_VIDEO 0x189
#define KEY_DIRECTORY 0x18a
#define KEY_LIST 0x18b
#define KEY_MEMO 0x18c
#define KEY_CALENDAR 0x18d
#define KEY_RED	0x18e
#define KEY_GREEN 0x18f
#define KEY_YELLOW 0x190
#define KEY_BLUE 0x191
#define KEY_CHANNELUP 0x192
#define KEY_CHANNELDOWN	0x193
#define KEY_FIRST 0x194
#define KEY_LAST 0x195
#define KEY_AB 0x196
#define KEY_NEXT 0x197
#define KEY_RESTART 0x198
#define KEY_SLOW 0x199
#define KEY_SHUFFLE 0x19a
#define KEY_BREAK 0x19b
#define KEY_PREVIOUS 0x19c
#define KEY_DIGITS 0x19d
#define KEY_TEEN 0x19e
#define KEY_TWEN 0x19f
#define KEY_VIDEOPHONE 0x1a0
#define KEY_GAMES 0x1a1
#define KEY_ZOOMIN 0x1a2
#define KEY_ZOOMOUT 0x1a3
#define KEY_ZOOMRESET 0x1a4
#define KEY_WORDPROCESSOR 0x1a5
#define KEY_EDITOR 0x1a6
#define KEY_SPREADSHEET 0x1a7
#define KEY_GRAPHICSEDITOR 0x1a8
#define KEY_PRESENTATION 0x1a9
#define KEY_DATABASE 0x1aa
#define KEY_NEWS 0x1ab
#define KEY_VOICEMAIL 0x1ac
#define KEY_ADDRESSBOOK 0x1ad
#define KEY_MESSENGER 0x1ae
#define KEY_DISPLAYTOGGLE 0x1af
#define KEY_BRIGHTNESS_TOGGLE KEY_DISPLAYTOGGLE
#define KEY_SPELLCHECK 0x1b0 
#define KEY_LOGOFF 0x1b1

#define KEY_DOLLAR 0x1b2
#define KEY_EURO 0x1b3

#define KEY_FRAMEBACK 0x1b4
#define KEY_FRAMEFORWARD 0x1b5
#define KEY_CONTEXT_MENU 0x1b6
#define KEY_MEDIA_REPEAT 0x1b7
#define KEY_10CHANNELSUP 0x1b8
#define KEY_10CHANNELSDOWN 0x1b9
#define KEY_IMAGES 0x1ba

#define KEY_DEL_EOL 0x1c0
#define KEY_DEL_EOS	0x1c1
#define KEY_INS_LINE 0x1c2
#define KEY_DEL_LINE 0x1c3

#define KEY_FN 0x1d0
#define KEY_FN_ESC 0x1d1
#define KEY_FN_F1 0x1d2
#define KEY_FN_F2 0x1d3
#define KEY_FN_F3 0x1d4
#define KEY_FN_F4 0x1d5
#define KEY_FN_F5 0x1d6
#define KEY_FN_F6 0x1d7
#define KEY_FN_F7 0x1d8
#define KEY_FN_F8 0x1d9
#define KEY_FN_F9 0x1da
#define KEY_FN_F10 0x1db
#define KEY_FN_F11 0x1dc
#define KEY_FN_F12 0x1dd
#define KEY_FN_1 0x1de
#define KEY_FN_2 0x1df
#define KEY_FN_D 0x1e0
#define KEY_FN_E 0x1e1
#define KEY_FN_F 0x1e2
#define KEY_FN_S 0x1e3
#define KEY_FN_B 0x1e4

#define KEY_BRL_DOT1 0x1f1
#define KEY_BRL_DOT2 0x1f2
#define KEY_BRL_DOT3 0x1f3
#define KEY_BRL_DOT4 0x1f4
#define KEY_BRL_DOT5 0x1f5
#define KEY_BRL_DOT6 0x1f6
#define KEY_BRL_DOT7 0x1f7
#define KEY_BRL_DOT8 0x1f8
#define KEY_BRL_DOT9 0x1f9
#define KEY_BRL_DOT10 0x1fa

#define KEY_NUMERIC_0 0x200
#define KEY_NUMERIC_1 0x201
#define KEY_NUMERIC_2 0x202
#define KEY_NUMERIC_3 0x203
#define KEY_NUMERIC_4 0x204
#define KEY_NUMERIC_5 0x205
#define KEY_NUMERIC_6 0x206
#define KEY_NUMERIC_7 0x207
#define KEY_NUMERIC_8 0x208
#define KEY_NUMERIC_9 0x209
#define KEY_NUMERIC_STAR 0x20a
#define KEY_NUMERIC_POUND 0x20b
#define KEY_NUMERIC_A 0x20c
#define KEY_NUMERIC_B 0x20d
#define KEY_NUMERIC_C 0x20e
#define KEY_NUMERIC_D 0x20f

#define KEY_CAMERA_FOCUS 0x210
#define KEY_WPS_BUTTON 0x211

#define KEY_TOUCHPAD_TOGGLE	0x212
#define KEY_TOUCHPAD_ON	0x213
#define KEY_TOUCHPAD_OFF 0x214

#define KEY_CAMERA_ZOOMIN 0x215
#define KEY_CAMERA_ZOOMOUT 0x216
#define KEY_CAMERA_UP 0x217
#define KEY_CAMERA_DOWN	0x218
#define KEY_CAMERA_LEFT	0x219
#define KEY_CAMERA_RIGHT 0x21a

#define KEY_ATTENDANT_ON 0x21b
#define KEY_ATTENDANT_OFF 0x21c
#define KEY_ATTENDANT_TOGGLE 0x21d
#define KEY_LIGHTS_TOGGLE 0x21e

#define KEY_ALS_TOGGLE 0x230

#define KEY_BUTTONCONFIG 0x240
#define KEY_TASKMANAGER 0x241
#define KEY_JOURNAL 0x242
#define KEY_CONTROLPANEL 0x243
#define KEY_APPSELECT 0x244
#define KEY_SCREENSAVER 0x245
#define KEY_VOICECOMMAND 0x246

#define KEY_BRIGHTNESS_MIN 0x250
#define KEY_BRIGHTNESS_MAX 0x251

#define KEY_KBDINPUTASSIST_PREV 0x260
#define KEY_KBDINPUTASSIST_NEXT 0x261
#define KEY_KBDINPUTASSIST_PREVGROUP 0x262
#define KEY_KBDINPUTASSIST_NEXTGROUP 0x263
#define KEY_KBDINPUTASSIST_ACCEPT 0x264
#define KEY_KBDINPUTASSIST_CANCEL 0x265

#define KEY_RIGHT_UP 0x266
#define KEY_RIGHT_DOWN 0x267
#define KEY_LEFT_UP 0x268
#define KEY_LEFT_DOWN 0x269

#define KEY_ROOT_MENU 0x26a
#define KEY_MEDIA_TOP_MENU 0x26b
#define KEY_NUMERIC_11 0x26c
#define KEY_NUMERIC_12 0x26d

#define KEY_AUDIO_DESC 0x26e
#define KEY_3D_MODE 0x26f
#define KEY_NEXT_FAVORITE 0x270
#define KEY_STOP_RECORD 0x271
#define KEY_PAUSE_RECORD 0x272
#define KEY_VOD 0x273
#define KEY_UNMUTE 0x274
#define KEY_FASTREVERSE 0x275
#define KEY_SLOWREVERSE 0x276

#define KEY_DATA 0x277
#define KEY_ONSCREEN_KEYBOARD 0x278

#define KEY_MIN_INTERESTING KEY_MUTE
#define KEY_MAX 0x2ff
#define KEY_CNT (KEY_MAX+1)

//----------------------------------
// Relative Codes
//----------------------------------

#define REL_X 0x00
#define REL_Y 0x01
#define REL_HWHEEL 0x06
#define REL_WHEEL 0x08
#define REL_MAX	0x0F

//----------------------------------
// Absolute Codes
//----------------------------------

#define ABS_X 0x00
#define ABS_Y 0x01
#define ABS_Z 0x02
#define ABS_RX 0x03
#define ABS_RY 0x04
#define ABS_RZ 0x05
#define ABS_THROTTLE 0x06
#define ABS_RUDDER 0x07
#define ABS_WHEEL 0x08
#define ABS_GAS	0x09
#define ABS_BRAKE 0x0A
#define ABS_PRESSURE 0x18

#define ABS_MAX 0x3f
#define ABS_CNT (ABS_MAX+1)

#define LED_NUML 0x00
#define LED_CAPSL 0x01
#define LED_SCROLLL 0x02

#endif // _LINUX_INPUT_H

