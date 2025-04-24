#include "doomkeys.h"

#include "doomgeneric.h"

#include <basic.h>
#include <bstdio.h>
#include <bstdlib.h>
#include <errcode.h>
#include <tcode.h>
#include <keycode.h>
#include <btron/hmi.h>
#include <btron/vobj.h>
#include <btron/dp.h>
#include <btron/libapp.h>
#include <btron/message.h>
#include <sys/time.h>

WEVENT xevt;
static WINFOREC winfo[2];
GID gid;
RECT visrect;
BMP ScreenBuffer;
W wid;
int ptr_sts = 1;
int shift_up = 0;

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;


static unsigned char convertToDoomKey(unsigned short input)
{
	unsigned char key = 0;
	switch (input)
	{
	case KC_TAB:
		key = KEY_TAB;
		break;
	case KC_BS:
		key = KEY_BACKSPACE;
		break;
	case KC_NL:
		key = KEY_ENTER;
		break;
	case KC_SPACE:
		key = KEY_USE;
		break;
	case KC_DEL:
		key = KEY_DEL;
		break;
	case KC_CAN:
		key = KEY_ESCAPE;
		break;
	case KC_IEND:
		key = KEY_FIRE;
		break;
	case KC_SHT_R:
	case KC_SHT_L:
		key = KEY_RSHIFT;
		break;
	case KC_CC_U:
		key = KEY_UPARROW;
		break;
	case KC_CC_D:
		key = KEY_DOWNARROW;
		break;
	case KC_CC_R:
		key = KEY_RIGHTARROW;
		break;
	case KC_CC_L:
		key = KEY_LEFTARROW;
		break;
	case KC_SC_U:
		key = KEY_UPARROW;
		break;
	case KC_SC_D:
		key = KEY_DOWNARROW;
		break;
	case KC_SC_R:
		key = KEY_STRAFE_R;
		break;
	case KC_SC_L:
		key = KEY_STRAFE_L;
		break;
	case KC_PF1:
		key = KEY_F1;
		break;
	case KC_PF2:
		key = KEY_F2;
		break;
	case KC_PF3:
		key = KEY_F3;
		break;
	case KC_PF4:
		key = KEY_F4;
		break;
	case KC_PF5:
		key = KEY_F5;
		break;
	case KC_PF6:
		key = KEY_F6;
		break;
	case KC_PF7:
		key = KEY_F7;
		break;
	case KC_PF8:
		key = KEY_F8;
		break;
	case KC_PF9:
		key = KEY_F9;
		break;
	case KC_PF10:
		key = KEY_F10;
		break;
	case KC_PF11:
		key = KEY_F11;
		break;
	case KC_PF12:
		key = KEY_F12;
		break;
	case TK_ASTR:
		key = '*';
		break;
	case TK_PLUS:
		key = '+';
		break;
	case TK_CMMA:
		key = ',';
		break;
	case TK_MINS:
		key = '-';
		break;
	case TK_PROD:
		key = '.';
		break;
	case TK_SLSH:
		key = '/';
		break;
	case TK_COLN:
		key = ':';
		break;
	case TK_SCLN:
		key = ';';
		break;
	case TK_ATMK:
		key = '@';
		break;
	case TK_LABR:
		key = '[';
		break;
	case TK_RABR:
		key = ']';
		break;
	case TK_0:
		key = '0';
		break;
	case TK_1:
		key = '1';
		break;
	case TK_2:
		key = '2';
		break;
	case TK_3:
		key = '3';
		break;
	case TK_4:
		key = '4';
		break;
	case TK_5:
		key = '5';
		break;
	case TK_6:
		key = '6';
		break;
	case TK_7:
		key = '7';
		break;
	case TK_8:
		key = '8';
		break;
	case TK_9:
		key = '9';
		break;
	case TK_A:
	case TK_a:
		key = 'a';
		break;
	case TK_B:
	case TK_b:
		key = 'b';
		break;
	case TK_C:
	case TK_c:
		key = 'c';
		break;
	case TK_D:
	case TK_d:
		key = 'd';
		break;
	case TK_E:
	case TK_e:
		key = 'e';
		break;
	case TK_F:
	case TK_f:
		key = 'f';
		break;
	case TK_G:
	case TK_g:
		key = 'g';
		break;
	case TK_H:
	case TK_h:
		key = 'h';
		break;
	case TK_I:
	case TK_i:
		key = 'i';
		break;
	case TK_J:
	case TK_j:
		key = 'j';
		break;
	case TK_K:
	case TK_k:
		key = 'k';
		break;
	case TK_L:
	case TK_l:
		key = 'l';
		break;
	case TK_M:
	case TK_m:
		key = 'm';
		break;
	case TK_N:
	case TK_n:
		key = 'n';
		break;
	case TK_O:
	case TK_o:
		key = 'o';
		break;
	case TK_P:
	case TK_p:
		key = 'p';
		break;
	case TK_Q:
	case TK_q:
		key = 'q';
		break;
	case TK_R:
	case TK_r:
		key = 'r';
		break;
	case TK_S:
	case TK_s:
		key = 's';
		break;
	case TK_T:
	case TK_t:
		key = 't';
		break;
	case TK_U:
	case TK_u:
		key = 'u';
		break;
	case TK_V:
	case TK_v:
		key = 'v';
		break;
	case TK_X:
	case TK_x:
		key = 'x';
		break;
	case TK_Y:
	case TK_y:
		key = 'y';
		break;
	case TK_Z:
	case TK_z:
		key = 'z';
		break;
	default:
		break;
	}

	return key;
}

static void addKeyToQueue(int pressed, unsigned short keyCode)
{
	unsigned char key = convertToDoomKey(keyCode);

	unsigned short keyData = (pressed << 8) | key;

	s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
	s_KeyQueueWriteIndex++;
	s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

void eventloop()
{
	while(1)
	{
		wget_evt(&xevt, NOWAIT);

        	switch (xevt.s.type)
		{
		case EV_NULL:
			/*if (ptr_sts)
			{
				if(xevt.s.wid == wid && xevt.s.cmd == W_WORK)
				{
					gdsp_ptr(0);
					ptr_sts = 0;
				}
			}
			else
			{
				if (xevt.s.wid != wid || xevt.s.cmd != W_WORK)
				{
					gdsp_ptr(-1);
					ptr_sts = 1;
				}
			}*/
		case EV_NOMSG:
			if( ((xevt.e.stat & ES_LSHFT)  == ES_LSHFT) || ((xevt.e.stat & ES_RSHFT)  == ES_RSHFT) )
			{
				addKeyToQueue(1, KC_SHT_R);
				shift_up = 1;
			}
			else
			{
				if(shift_up)
				{
					addKeyToQueue(0, KC_SHT_R);
					shift_up = 0;
				}
			}
			return;
		case EV_REQUEST:
			switch (xevt.g.cmd)
			{
            		case W_REDISP:
				gid = wget_gid(winfo[0].wid);
				gget_vis(gid, &visrect);
				do 
				{
					if (wsta_dsp(winfo[0].wid, NULL, NULL) == 0)
					{
						break;
					}
					wera_wnd(winfo[0].wid, NULL);
				} while (wend_dsp(winfo[0].wid) > 0);
				break;
			case W_PASTE:
				wrsp_evt(&xevt, 1); /* NACK */
				break;
			case W_DELETE:
			case W_FINISH:
				wrsp_evt(&xevt, 0); /* ACK */
				//cont_loop = false;
				break;
			}
		case EV_RSWITCH:
			gid = wget_gid(winfo[0].wid);
			gget_vis(gid, &visrect);
			do 
			{
				if (wsta_dsp(winfo[0].wid, NULL, NULL) == 0)
				{
					break;
				}
				wera_wnd(winfo[0].wid, NULL);
			} while (wend_dsp(winfo[0].wid) > 0);
		case EV_SWITCH:
		case EV_BUTDWN:
			switch (xevt.g.cmd)
			{
				case W_FRAM:
				case W_TITL:
					wmov_drg(&xevt, NULL);
					break;
				case W_WORK:
					break;
			}
			break;
		case EV_INACT:
			break;
		case EV_DEVICE:
			oprc_dev(&(xevt.e), NULL, 0);
			break;
		case EV_MSG:
			clr_msg(MM_ALL, MM_ALL);
			break;
		case EV_MENU:
			break;
		case EV_AUTKEY:
			break;
		case EV_KEYDWN:
			addKeyToQueue(1, xevt.e.data.key.code);
			break;
		case EV_KEYUP:
			addKeyToQueue(0, xevt.e.data.key.code);
			break;
		}
	}
}

void DG_Init()
{
	static TC text[] = {TK_T, TK_R, TK_O, TK_N, TK_KSP, TK_D, TK_O, TK_O, TK_M, TNULL}; // TRON DOOM
	RECT r = {{ 100, 100, 740, 580 }};
	PAT bgpat = {{ 0, 1, 1, 0x90ffffff, 0x90000000, FILL0 }};
	W err, fid, ret;

	fid = dopn_dat(NULL);
	if (fid < 0)
	{
		printf("dopn_dat: %d\n", fid);
		ext_prc(1);
	}

	winfo[0].wid = wopn_wnd(WA_WORK, 0, &r, NULL, 2, text, NULL, NULL);
	if (winfo[0].wid < 0)
	{
		printf("wopn_wnd: %d\n", winfo[0].wid);
		ext_prc(1);
	}
	wid = winfo[0].wid;
	winfo[1].wid = 0;

	wset_bgp(winfo[0].wid, &bgpat);

	// Enable key up event.
	chg_emk(EM_ALL);

	ScreenBuffer.planes = 1;
	ScreenBuffer.pixbits = 0x2018;
	ScreenBuffer.rowbytes = DOOMGENERIC_RESX * 4;
	ScreenBuffer.bounds.c.left = 0;
	ScreenBuffer.bounds.c.top = 0;
	ScreenBuffer.bounds.c.right = DOOMGENERIC_RESX;
	ScreenBuffer.bounds.c.bottom = DOOMGENERIC_RESY;
	ScreenBuffer.baseaddr[0] = (UB *)DG_ScreenBuffer;

	memset(s_KeyQueue, 0, KEYQUEUE_SIZE * sizeof(unsigned short));
}

void DG_DrawFrame()
{
	gid = wget_gid(winfo[0].wid);
	gget_vis(gid, &visrect);
	grst_bmp(gid, &visrect, &ScreenBuffer, &ScreenBuffer.bounds, NULL, G_STORE);
}

void DG_SleepMs(unsigned int ms)
{
	dly_tsk(ms);
}

uint32_t DG_GetTicksMs()
{
	//unsigned int ret;

	//vget_otm(ret);

	//return ret; /* return milliseconds */

	struct timeval  tp;
	struct timezone tzp;

	gettimeofday(&tp, &tzp);

	return (tp.tv_sec * 1000) + (tp.tv_usec / 1000); /* return milliseconds */
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
	if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex)
	{
		//key queue is empty

		return 0;
	}
	else
	{
		unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
		s_KeyQueueReadIndex++;
		s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

		*pressed = keyData >> 8;
		*doomKey = keyData & 0xFF;

		return 1;
	}
}

void DG_SetWindowTitle(const char * title)
{

}

int main(int argc, char **argv)
{
	doomgeneric_Create(argc, argv);

	while (1)
	{
		eventloop();

		doomgeneric_Tick();
	}

    return 0;
}
