/*B-em 1.1 by Tom Walker*/
/*Main loop*/

#include <stdio.h>
#include <allegro.h>
#include <winalleg.h>
#include "b-em.h"

int fasttape;
int tube;
int soundbuflen;
int wah=0;
int autoboot=0;
char exname[512];
int snline;
int fullscreen,updatewindow;
char discname[2][260];
void updatemenu();
int blurred,mono;
int hires,ddnoise,curwave,soundfilter;
void dumpram();
int model;
int uefena;
int soundon;
int logging;
AUDIOSTREAM *as;
volatile int fps;
unsigned char *ram;
int quit=0;
char uefname[260];

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[ ] = "WindowsApp";
HWND ghwnd;
int infocus;

static int spdcount=0;
static void update50()
{
        spdcount++;
}
END_OF_FUNCTION(update50);

HMENU menu;
#define IDM_FILE_LSNAP     40000
#define IDM_FILE_SSNAP     40001
#define IDM_FILE_EXIT      40002
#define IDM_DISC_AUTOSTART 40013
#define IDM_DISC_CHANGE0   40010
#define IDM_DISC_CHANGE1   40011
#define IDM_DISC_SOUND     40012
#define IDM_TAPE_CHANGE    40020
#define IDM_TAPE_REWIND    40021
#define IDM_TAPE_ENABLE    40022
#define IDM_TAPE_FAST      40023
#define IDM_MODEL_PALA     40030
#define IDM_MODEL_PALB     40031
#define IDM_MODEL_PALBSW   40032
#define IDM_MODEL_NTSCB    40033
#define IDM_MODEL_B1770    40034
#define IDM_MODEL_PALB64   40035
#define IDM_MODEL_PALB96   40036
#define IDM_MODEL_PALB128  40037
#define IDM_MODEL_PALM128  40038
#define IDM_MODEL_PALMC    40039
#define IDM_MODEL_ARM      40040
#define IDM_VIDEO_RES      40044
#define IDM_VIDEO_FULLSCR  40041
#define IDM_VIDEO_BLUR     40042
#define IDM_VIDEO_MONO     40043
#define IDM_SOUND_ENABLE   40050
#define IDM_SOUND_LOW      40051
#define IDM_SOUND_HIGH     40052
#define IDM_SOUND_WAVEFORM 40053
#define IDM_SOUND_STARTVGM 40054
#define IDM_SOUND_STOPVGM  40055
#define IDM_MISC_SCRSHOT   40060
#define IDM_RES_LOW        40070
#define IDM_RES_HIGH       40071
#define IDM_RES_HIGH2      40072
#define IDM_RES_2XSAI      40073
#define IDM_WAVE_SQU       40080
#define IDM_WAVE_SAW       40081
#define IDM_WAVE_SIN       40082
#define IDM_WAVE_TRI       40083
#define IDM_WAVE_SID       40084
#define IDM_BUF_80         40090
#define IDM_BUF_100        40091

void updatewindowsize(int x, int y)
{
        RECT r;
        GetWindowRect(ghwnd,&r);
        MoveWindow(ghwnd,r.left,r.top,
                     x+(GetSystemMetrics(SM_CXFIXEDFRAME)*2),
                     y+(GetSystemMetrics(SM_CYFIXEDFRAME)*2)+GetSystemMetrics(SM_CYMENUSIZE)+GetSystemMetrics(SM_CYCAPTION)+1,
                     TRUE);
}

void recreatemenu()
{
        SetMenu(ghwnd,menu);
}

void makemenu()
{
        HMENU hpop,hpop2;
        menu=CreateMenu();
        hpop=CreateMenu();
        AppendMenu(hpop,MF_STRING,IDM_FILE_LSNAP,"&Load Snapshot...");
        AppendMenu(hpop,MF_STRING,IDM_FILE_SSNAP,"&Save Snapshot...");
        AppendMenu(hpop,MF_SEPARATOR,0,NULL);
        AppendMenu(hpop,MF_STRING,IDM_FILE_EXIT,"E&xit");
        AppendMenu(menu,MF_POPUP,hpop,"&File");
        hpop=CreateMenu();
        AppendMenu(hpop,MF_STRING,IDM_DISC_AUTOSTART,"Autostart Disc &0/2...");
        AppendMenu(hpop,MF_STRING,IDM_DISC_CHANGE0,"Change Disc &0/2...");
        AppendMenu(hpop,MF_STRING,IDM_DISC_CHANGE1,"Change Disc &1/3...");
        AppendMenu(hpop,MF_STRING,IDM_DISC_SOUND,"&Disc Sounds");
        AppendMenu(menu,MF_POPUP,hpop,"&Disc");
        hpop=CreateMenu();
        AppendMenu(hpop,MF_STRING,IDM_TAPE_CHANGE,"&Change Tape...");
        AppendMenu(hpop,MF_STRING,IDM_TAPE_REWIND,"&Rewind Tape");
        AppendMenu(hpop,MF_STRING,IDM_TAPE_ENABLE,"&Tape Enable");
        AppendMenu(hpop,MF_STRING,IDM_TAPE_FAST,"&Fast Tape");
        AppendMenu(menu,MF_POPUP,hpop,"&Tape");
        hpop=CreateMenu();
        AppendMenu(hpop,MF_STRING,IDM_MODEL_PALA,"Model &A");
        AppendMenu(hpop,MF_STRING,IDM_MODEL_PALB,"Model &B");
        AppendMenu(hpop,MF_STRING,IDM_MODEL_PALBSW,"Model B + &SWRAM");
        AppendMenu(hpop,MF_STRING,IDM_MODEL_B1770,"Model B + &1770");
        AppendMenu(hpop,MF_STRING,IDM_MODEL_NTSCB,"&NTSC B");
        AppendMenu(hpop,MF_STRING,IDM_MODEL_PALB64,"B&+");
        AppendMenu(hpop,MF_STRING,IDM_MODEL_PALB96,"B+&96K");
        AppendMenu(hpop,MF_STRING,IDM_MODEL_PALB128,"B+&128K");
        AppendMenu(hpop,MF_STRING,IDM_MODEL_PALM128,"&Master 128");
        AppendMenu(hpop,MF_STRING,IDM_MODEL_PALMC,"Master &Compact");
        AppendMenu(hpop,MF_STRING,IDM_MODEL_ARM,    "&ARM Evaluation System");
        AppendMenu(menu,MF_POPUP,hpop,"&Model");
        hpop=CreateMenu();
        hpop2=CreateMenu();
        AppendMenu(hpop2,MF_STRING,IDM_RES_LOW,"&Low");
        AppendMenu(hpop2,MF_STRING,IDM_RES_HIGH2,"High With &Scanlines");
        AppendMenu(hpop2,MF_STRING,IDM_RES_HIGH,"&High");
        AppendMenu(hpop2,MF_STRING,IDM_RES_2XSAI,"&2xSaI");
        AppendMenu(hpop,MF_POPUP,hpop2,"&Resolution");
        AppendMenu(hpop,MF_STRING,IDM_VIDEO_FULLSCR,"&Fullscreen");
        AppendMenu(hpop,MF_STRING,IDM_VIDEO_BLUR,"&Blur Filter");
        AppendMenu(hpop,MF_STRING,IDM_VIDEO_MONO,"&Monochrome");
        AppendMenu(menu,MF_POPUP,hpop,"&Video");
        hpop=CreateMenu();
        AppendMenu(hpop,MF_STRING,IDM_SOUND_ENABLE,"Sound &Enable");
        AppendMenu(hpop,MF_STRING,IDM_SOUND_LOW, "&Low Pass Filter");
        AppendMenu(hpop,MF_STRING,IDM_SOUND_HIGH,"&High Pass Filter");
        hpop2=CreateMenu();
        AppendMenu(hpop2,MF_STRING,IDM_WAVE_SQU,"&Square");
        AppendMenu(hpop2,MF_STRING,IDM_WAVE_SAW,"S&awtooth");
        AppendMenu(hpop2,MF_STRING,IDM_WAVE_SIN,"S&ine");
        AppendMenu(hpop2,MF_STRING,IDM_WAVE_TRI,"&Triangle");
        AppendMenu(hpop2,MF_STRING,IDM_WAVE_SID,"SI&D");
        AppendMenu(hpop,MF_POPUP,hpop2,"&Waveform");
        hpop2=CreateMenu();
        AppendMenu(hpop2,MF_STRING,IDM_BUF_80,"&80ms");
        AppendMenu(hpop2,MF_STRING,IDM_BUF_100,"&100ms");
        AppendMenu(hpop,MF_POPUP,hpop2,"&Sound Buffer");
        AppendMenu(hpop,MF_STRING,IDM_SOUND_STARTVGM, "&Start VGM Log...");
        AppendMenu(hpop,MF_STRING,IDM_SOUND_STOPVGM, "S&top VGM Log");
        AppendMenu(menu,MF_POPUP,hpop,"&Sound");
        hpop=CreateMenu();
        AppendMenu(hpop,MF_STRING,IDM_MISC_SCRSHOT,"&Save Screenshot...");
        AppendMenu(menu,MF_POPUP,hpop,"M&isc");
}

void parsecommandline(LPSTR s)
{
        char *t;
        int c;
        char fn[256],s2[256];
        int found;
//        FILE *f=fopen("cmdline.txt","wt");
        for (t=strtok(s," ");t;t=strtok(0," "))
        {
                if (!stricmp(t,"-fullscreen"))
                   fullscreen=1;
                if (!stricmp(t,"-disc"))
                {
                        found=0;
                        t=strtok(0," ");
                        if (!t) return;
                        if (t[0]=='\"') /*Quoted filename*/
                        {
                                strcpy(fn,t+1);
                                while (1)
                                {
//                                        printf("filename : %s\n",fn);
                                        for (c=0;c<strlen(fn);c++)
                                        {
                                                if (fn[c]=='\"')
                                                {
                                                        found=1;
                                                        break;
                                                }
                                        }
                                        if (found) break;
                                        t=strtok(0," ");
                                        if (!t) break;
                                        strcat(fn," ");
                                        strcat(fn,t);
                                }
                                if (!found) return;
                                fn[c]=0;
//                                sprintf(s2,"Filename : %s\n",fn);
//                                fputs(s2,f);
                                strcpy(discname[0],fn);
                        }
                        else
                           strcpy(discname[0],t);
/*                        t=strtok(0," ");
                        if (t)
                        {
                                if (t[0]=='"')
                                strcpy(discname[0],t);
                        }
                        else
                           return;*/
                }
                if (!stricmp(t,"-autoboot"))
                   autoboot=50;
                if (!stricmp(t,"-esc"))
                   wah=1;
                if (!stricmp(t,"-model"))
                {
                        tube=0;
                        t=strtok(0," ");
                        if (t)
                        {
                                if (!stricmp(t,"a"))     model=0;
                                if (!stricmp(t,"b"))     model=1;
                                if (!stricmp(t,"bsw"))   model=2;
                                if (!stricmp(t,"ntscb")) model=3;
                                if (!stricmp(t,"b+"))    model=4;
                                if (!stricmp(t,"b+96"))  model=5;
                                if (!stricmp(t,"b+128")) model=6;
                                if (!stricmp(t,"m128"))  model=7;
                                if (!stricmp(t,"arm"))   { model=7; tube=1; }
                        }
                        else
                           return;
                }
        }
}

int framenum=0;
//FILE *spdlog;

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)
{
        unsigned short *p;
        int resetting=0;
        char s[160];
        MSG messages;            /* Here messages to the application are saved */
//        spdlog=fopen("spdlog.txt","wt");
//        FILE *tempf=fopen("temp.txt","wt");
        /* This is the handle for our window */
        WNDCLASSEX wincl;        /* Data structure for the windowclass */

        /* The Window structure */
        wincl.hInstance = hThisInstance;
        wincl.lpszClassName = szClassName;
        wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
        wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
        wincl.cbSize = sizeof (WNDCLASSEX);

        /* Use default icon and mouse-pointer */
        wincl.hIcon = LoadIcon(hThisInstance, "allegro_icon");
        wincl.hIconSm = LoadIcon(hThisInstance, "allegro_icon");
        wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
        wincl.lpszMenuName = NULL;                 /* No menu */
        wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
        wincl.cbWndExtra = 0;                      /* structure or the window instance */
        /* Use Windows's default color as the background of the window */
        wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

        /* Register the window class, and if it fails quit the program */
        if (!RegisterClassEx (&wincl))
           return 0;

        makemenu();
        /* The class is registered, let's create the program*/
        ghwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "B-em v1.1",         /* Title Text */
           WS_OVERLAPPEDWINDOW&~(WS_MAXIMIZEBOX|WS_SIZEBOX), /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           400+(GetSystemMetrics(SM_CXFIXEDFRAME)*2),/* The programs width */
           300+(GetSystemMetrics(SM_CYFIXEDFRAME)*2)+GetSystemMetrics(SM_CYMENUSIZE)+GetSystemMetrics(SM_CYCAPTION)+1,/* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           menu,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

        /* Make the window visible on the screen */
        ShowWindow (ghwnd, nFunsterStil);
        win_set_window(ghwnd);
        allegro_init();
        get_executable_name(exname,511);
        load_config();
        if (fullscreen) SetMenu(ghwnd,NULL);
        parsecommandline(lpszArgument);
        if (hires) updatewindowsize(800,600);
        loadcmos();
        install_keyboard();
        key_led_flag=0;
        install_timer();
        if (fullscreen) install_mouse();
        initmem();
        loadroms();
        reset6502();
        resetsysvia();
        resetuservia();
        resetcrtc();
        resetacia();
        initserial();
        initvideo();
        initsnd();
        initadc();
        reset8271(1);
        reset1770();
        loadarmrom();
        resetarm();
        resettube();
//        tubeinit6502();
//        tubeinitz80();
        loaddiscsamps();
        openuef(uefname);
        if (!uefena && model<3) trapos();
        install_int_ex(update50,MSEC_TO_TIMER(20));
        if (!tube) CheckMenuItem(menu,model+40030,MF_CHECKED);
        else       CheckMenuItem(menu,40040,MF_CHECKED);
        CheckMenuItem(menu,curwave+40080,MF_CHECKED);
        updatemenu();
        fasttape=1;
        while (spdcount<25)
        {
                if (PeekMessage(&messages,NULL,0,0,PM_REMOVE))
                {
                        if (messages.message==WM_QUIT)
                           quit=1;
                        /* Translate virtual-key messages into character messages */
                        TranslateMessage(&messages);
                        /* Send message to WindowProcedure */
                        DispatchMessage(&messages);
                }
        }
//        sprintf(s,"Model %i Tube %i",model,tube);
//        MessageBox(NULL,s,s,MB_OK);
        while (!quit)
        {
                if (framenum==5 && !(fasttape && motor))
                {
//                        fputs("Sound\n",tempf);
                        framenum=0;
                        p=0;
                        while (!p)
                        {
                                p=(unsigned short *)get_audio_stream_buffer(as);
                                sleep(0);
//                                yield_timeslice();
//                                rest(0);
                        }
                        updatebuffer(p,soundbuflen);
                        free_audio_stream_buffer(as);
//                        spdcount=1;
                        snline=0;
                }
                if (infocus && (spdcount || (fasttape && motor)))
                {
                        exec6502(312,128);
                        if (logging) logsound();
                        checkkeys();
                        poll_joystick();
                        spdcount=0;
                        framenum++;
                        if (autoboot)
                        {
                                autoboot--;
                                if (!autoboot)
                                   releasekey(0,0);
                        }
                        if (fasttape && motor)
                        {
                                p=(unsigned short *)get_audio_stream_buffer(as);
                                if (p)
                                {
                                        memset(p,0,soundbuflen*2);
                                        free_audio_stream_buffer(as);
                                        framenum=snline=0;
                                }
                        }
                }
                if (fullscreen)
                {
                        if (key[KEY_F11])
                        {
                                while (key[KEY_F11]) yield_timeslice();
                                entergui();
                        }
                        if (mouse_b&2)
                        {
                                while (mouse_b&2) yield_timeslice();
                                entergui();
                        }
                }
                if (updatewindow)
                {
                        if (hires) updatewindowsize(800,600);
                        else       updatewindowsize(400,300);
                        updatewindow=0;
                }
                if (resetting && !key[KEY_F12]) resetting=0;
                if (key[KEY_F12] && !resetting)
                {
                        if (key[KEY_LCONTROL] || key[KEY_RCONTROL])
                        {
                                resetsysvia();
                                memset(ram,0,65536);
                        }
                        resetarm();
                        resetuservia();
                        reset1770s();
                        reset8271s();
                        if (model<1) remaketablesa();
                        else         remaketables();
                        reset6502();
                        resetting=1;
                }
                if (PeekMessage(&messages,NULL,0,0,PM_REMOVE))
                {
                        if (messages.message==WM_QUIT)
                           quit=1;
                        /* Translate virtual-key messages into character messages */
                        TranslateMessage(&messages);
                        /* Send message to WindowProcedure */
                        DispatchMessage(&messages);
                }
                if (wah && key[KEY_ESC])
                {
                        PostQuitMessage (0);
                        wah=0;
                }
//                rest(1);
//                sprintf(s,"B-em %i %i",spdcount,framenum);
//                set_window_title(s);
        }
//        fclose(spdlog);
/*                if (resetting && !key[KEY_F12]) resetting=0;
                if (key[KEY_F12] && !resetting)
                {
                        if (key[KEY_LCONTROL] || key[KEY_RCONTROL])
                        {
                                resetsysvia();
                                memset(ram,0,65536);
                        }
                        resetuservia();
//                        resetacia();
//                        initserial();
                        reset1770s();
                        reset8271s();
                        if (model<1) remaketablesa();
                        else         remaketables();
                        reset6502();
//                        printf("Reset\n");
                        resetting=1;
//                        while (key[KEY_F12])
//                        {
//                                sleep(1);
//                        }
                }
                if (key[KEY_F11])
                {
                        while (key[KEY_F11]) yield_timeslice();
                        entergui();
                }
                if (mouse_b&2)
                {
                        while (mouse_b&2) yield_timeslice();
                        entergui();
                }*/
//        dumpram();
        save_config();
        allegro_exit();
        savecmos();
        dumpregs();
//        savebuffers();
//        dumpregs();
        checkdiscchanged(0);
        checkdiscchanged(1);
/*        dumptuberegs();
        dumptube();
        dumpram2();
        dumpuservia();
        dumparmregs();*/
//        dumpsysvia();
//        printf("%i\n",vidbank);
//        dumpcrtc();
        return 0;
}

void updatemenu()
{
        CheckMenuItem(menu,IDM_RES_LOW,MF_UNCHECKED);
        CheckMenuItem(menu,IDM_RES_HIGH,MF_UNCHECKED);
        CheckMenuItem(menu,IDM_RES_HIGH2,MF_UNCHECKED);
        CheckMenuItem(menu,IDM_RES_2XSAI,MF_UNCHECKED);
        switch (hires)
        {
                case 0: CheckMenuItem(menu,IDM_RES_LOW,MF_CHECKED); break;
                case 1: CheckMenuItem(menu,IDM_RES_HIGH2,MF_CHECKED); break;
                case 2: CheckMenuItem(menu,IDM_RES_2XSAI,MF_CHECKED); break;
                case 3: CheckMenuItem(menu,IDM_RES_HIGH,MF_CHECKED); break;
        }
        if (blurred) CheckMenuItem(menu,IDM_VIDEO_BLUR,MF_CHECKED);
        else         CheckMenuItem(menu,IDM_VIDEO_BLUR,MF_UNCHECKED);
        if (mono) CheckMenuItem(menu,IDM_VIDEO_MONO,MF_CHECKED);
        else      CheckMenuItem(menu,IDM_VIDEO_MONO,MF_UNCHECKED);
        if (uefena) CheckMenuItem(menu,IDM_TAPE_ENABLE,MF_CHECKED);
        else        CheckMenuItem(menu,IDM_TAPE_ENABLE,MF_UNCHECKED);
        if (fasttape) CheckMenuItem(menu,IDM_TAPE_FAST,MF_CHECKED);
        else          CheckMenuItem(menu,IDM_TAPE_FAST,MF_UNCHECKED);
        if (ddnoise) CheckMenuItem(menu,IDM_DISC_SOUND,MF_CHECKED);
        else         CheckMenuItem(menu,IDM_DISC_SOUND,MF_UNCHECKED);
        if (soundfilter&1) CheckMenuItem(menu,IDM_SOUND_HIGH,MF_CHECKED);
        else               CheckMenuItem(menu,IDM_SOUND_HIGH,MF_UNCHECKED);
        if (soundfilter&2) CheckMenuItem(menu,IDM_SOUND_LOW,MF_CHECKED);
        else               CheckMenuItem(menu,IDM_SOUND_LOW,MF_UNCHECKED);
        if (soundon) CheckMenuItem(menu,IDM_SOUND_ENABLE,MF_CHECKED);
        else         CheckMenuItem(menu,IDM_SOUND_ENABLE,MF_UNCHECKED);
        if (soundbuflen==3120)
        {
                CheckMenuItem(menu,IDM_BUF_100,MF_CHECKED);
                CheckMenuItem(menu,IDM_BUF_80,MF_UNCHECKED);
        }
        else
        {
                CheckMenuItem(menu,IDM_BUF_100,MF_UNCHECKED);
                CheckMenuItem(menu,IDM_BUF_80,MF_CHECKED);
        }
}

int getfn(HWND hwnd, char *f, char *s, int save, char *de)
{
        char fn[512];
        char start[512];
        OPENFILENAME ofn;
        fn[0]=0;
        start[0]=0;
        strcpy(fn,s);

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = fn;
        ofn.nMaxFile = 260;
        ofn.lpstrFilter = f;//"All\0*.*\0Text\0*.TXT\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;// | 0x10000000;//OFN_FORCESHOWHIDDEN;
        ofn.lpstrDefExt=de;
        if (save)
        {
                if (GetSaveFileName(&ofn))
                {
                        strcpy(s,fn);
                        return 0;
                }
        }
        else
        {
                if (GetOpenFileName(&ofn))
                {
                        strcpy(s,fn);
                        return 0;
                }
        }
        return -1;
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
        int c;
        char s[260];
        HMENU hmenu;
        switch (message)                  /* handle the messages */
        {
                case WM_COMMAND:
                switch (LOWORD(wParam))
                {
                        case IDM_FILE_LSNAP:
                        s[0]=0;
                        if (!getfn(hwnd,"B-em Savestate (*.SNP)\0*.SNP\0All\0*.*\0\0",s,0,"SNP"))
                           loadstate(s);
                        return 0;
                        case IDM_FILE_SSNAP:
                        s[0]=0;
                        if (!getfn(hwnd,"B-em Savestate (*.SNP)\0*.SNP\0All\0*.*\0\0",s,1,"SNP"))
                           savestate(s);
                        return 0;
                        case IDM_FILE_EXIT:
                        PostQuitMessage(0);
                        return 0;
                        case IDM_DISC_CHANGE0: case IDM_DISC_AUTOSTART:
                        if (soundon) stop_audio_stream(as);
                        if (!getfn(hwnd,"Disc Image\0*.SSD;*.DSD;*.ADF;*.ADL;*.FDI\0DFS Single Sided Disc Image (*.SSD)\0*.SSD\0DFS Double Sided Disc Image (*.DSD)\0*.DSD\0ADFS Disc Image (*.ADF)\0*.ADF\0FDI Disc Image (*.FDI)\0All\0*.*\0\0",discname[0],0,"SSD"))
                        {
                                for (c=0;c<strlen(discname[0]);c++)
                                {
                                        if (discname[0][c]=='.')
                                        {
                                                c++;
                                                break;
                                        }
                                }
                                if ((discname[0][c]=='d'||discname[0][c]=='D')&&(c!=strlen(discname[0])))
                                   load8271dsd(discname[0],0);
                                else if ((discname[0][c]=='f'||discname[0][c]=='F')&&(c!=strlen(discname[0])))
                                   load8271fdi(discname[0],0);
                                else if ((discname[0][c]=='a'||discname[0][c]=='A')&&(c!=strlen(discname[0])))
                                   load1770adfs(discname[0],0);
                                else if (c!=strlen(discname[0]))
                                   load8271ssd(discname[0],0);
                        }
                        if (soundon) as=play_audio_stream(soundbuflen,16,0,31200,255,127);
                        if (LOWORD(wParam)==IDM_DISC_AUTOSTART)
                        {
                                autoboot=50;
                                resetsysvia();
                                memset(ram,0,65536);
                                resetarm();
                                resetuservia();
                                reset1770s();
                                reset8271s();
                                if (model<1) remaketablesa();
                                else         remaketables();
                                reset6502();
                        }
                        return 0;
                        case IDM_DISC_CHANGE1:
                        if (soundon) stop_audio_stream(as);
                        if (!getfn(hwnd,"Disc Image\0*.SSD;*.DSD;*.ADF;*.ADL;*.FDI\0DFS Single Sided Disc Image (*.SSD)\0*.SSD\0DFS Double Sided Disc Image (*.DSD)\0*.DSD\0ADFS Disc Image (*.ADF)\0*.ADF\0FDI Disc Image (*.FDI)\0All\0*.*\0\0",discname[0],0,"SSD"))
                        {
                                for (c=0;c<strlen(discname[1]);c++)
                                {
                                        if (discname[1][c]=='.')
                                        {
                                                c++;
                                                break;
                                        }
                                }
                                if ((discname[1][c]=='d'||discname[1][c]=='D')&&(c!=strlen(discname[1])))
                                   load8271dsd(discname[1],1);
                                else if ((discname[0][c]=='f'||discname[0][c]=='F')&&(c!=strlen(discname[0])))
                                   load8271fdi(discname[0],0);
                                else if ((discname[1][c]=='a'||discname[1][c]=='A')&&(c!=strlen(discname[1])))
                                   load1770adfs(discname[1],1);
                                else if (c!=strlen(discname[1]))
                                   load8271ssd(discname[1],1);
                        }
                        if (soundon) as=play_audio_stream(soundbuflen,16,0,31200,255,127);
                        return 0;
                        case IDM_DISC_SOUND:
                        ddnoise^=1;
                        updatemenu();
                        return 0;
                        case IDM_TAPE_CHANGE:
                        if (!getfn(hwnd,"UEF Tape Image (*.UEF)\0*.UEF\0All\0*.*\0\0",uefname,0,"UEF"))
                           openuef(uefname);
                        return 0;
                        case IDM_TAPE_REWIND:
                        rewindit();
                        return 0;
                        case IDM_TAPE_ENABLE:
                        uefena^=1;
                        if (uefena) loadroms();
                        else if (model<3) trapos();
                        updatemenu();
                        return 0;
                        case IDM_TAPE_FAST:
                        fasttape^=1;
                        updatemenu();
                        return 0;
                        case IDM_MODEL_PALA: case IDM_MODEL_PALB:
                        case IDM_MODEL_PALBSW: case IDM_MODEL_NTSCB:
                        case IDM_MODEL_PALB64: case IDM_MODEL_PALB96:
                        case IDM_MODEL_PALB128: case IDM_MODEL_PALM128:
                        case IDM_MODEL_ARM: case IDM_MODEL_PALMC:
                        case IDM_MODEL_B1770:
                        if (!tube) CheckMenuItem(menu,model+40030,MF_UNCHECKED);
                        else       CheckMenuItem(menu,40040,MF_UNCHECKED);
                        CheckMenuItem(menu,LOWORD(wParam),MF_CHECKED);
                        model=LOWORD(wParam)-40030;
                        if (model==10)
                        {
                                model=8;
                                tube=1;
                        }
                        else
                           tube=0;
                        if (model) remaketables();
                        else       remaketablesa();
                        loadroms();
                        reset6502();
                        reset1770();
                        reset8271(0);
                        resetarm();
                        resetsysvia();
                        resetuservia();
                        memset(ram,0,65536);
                        return 0;
                        case IDM_RES_LOW:
                        hires=0;
                        updategfxmode();
                        updatemenu();
                        updatewindowsize(400,300);
                        return 0;
                        case IDM_RES_HIGH:
                        hires=3;
                        updategfxmode();
                        updatemenu();
                        updatewindowsize(800,600);
                        return 0;
                        case IDM_RES_HIGH2:
                        hires=1;
                        updategfxmode();
                        updatemenu();
                        updatewindowsize(800,600);
                        return 0;
                        case IDM_RES_2XSAI:
                        hires=2;
                        updategfxmode();
                        updatemenu();
                        updatewindowsize(800,600);
                        return 0;
                        case IDM_VIDEO_FULLSCR:
                        SetMenu(hwnd,NULL);
                        if (soundon) stop_audio_stream(as);
                        fullscreen=1;
                        updategfxmode();
                        install_mouse();
                        if (soundon) as=play_audio_stream(soundbuflen,16,0,31200,255,127);
                        framenum=0;
                        clear(screen);
                        return 0;
                        case IDM_VIDEO_BLUR:
                        blurred^=1;
                        updatemenu();
                        return 0;
                        case IDM_VIDEO_MONO:
                        mono^=1;
                        updatepalette();
                        updatemenu();
                        return 0;
                        case IDM_MISC_SCRSHOT:
                        s[0]=0;
                        if (!getfn(hwnd,"Bitmap (*.BMP)\0*.BMP\0All\0*.*\0\0",s,1,"BMP"))
                           scrshot(s);
                        return 0;
                        case IDM_WAVE_SQU: case IDM_WAVE_SAW: case IDM_WAVE_SIN:
                        case IDM_WAVE_TRI: case IDM_WAVE_SID:
                        CheckMenuItem(menu,curwave+40080,MF_UNCHECKED);
                        CheckMenuItem(menu,LOWORD(wParam),MF_CHECKED);
                        curwave=LOWORD(wParam)-40080;
                        return 0;
                        case IDM_SOUND_ENABLE:
                        soundon^=1;
                        if (!soundon) stop_audio_stream(as);
                        else          as=play_audio_stream(soundbuflen,16,0,31200,255,127);
                        updatemenu();
                        return 0;
                        case IDM_SOUND_HIGH:
                        soundfilter^=1;
                        updatemenu();
                        return 0;
                        case IDM_SOUND_LOW:
                        soundfilter^=2;
                        updatemenu();
                        return 0;
                        case IDM_SOUND_STARTVGM:
                        s[0]=0;
                        if (!getfn(hwnd,"VGM Sound file (*.VGM)\0*.VGM\0All\0*.*\0\0",s,1,"VGM"))
                           startsnlog(s);
                        return 0;
                        case IDM_SOUND_STOPVGM:
                        stopsnlog();
                        return 0;
                        case IDM_BUF_80:
                        if (soundon) stop_audio_stream(as);
                        soundbuflen=2496;
                        updatemenu();
                        if (soundon) as=play_audio_stream(soundbuflen,16,0,31200,255,127);
                        framenum=0;
                        return 0;
                        case IDM_BUF_100:
                        if (soundon) stop_audio_stream(as);
                        soundbuflen=3120;
                        updatemenu();
                        if (soundon) as=play_audio_stream(soundbuflen,16,0,31200,255,127);
                        framenum=0;
                        return 0;
                }
                return DefWindowProc (hwnd, message, wParam, lParam);
                case WM_SETFOCUS:
                infocus=1;
                break;
                case WM_KILLFOCUS:
                infocus=0;
                break;
                case WM_DESTROY:
                PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
                break;
                default:                      /* for messages that we don't deal with */
                return DefWindowProc (hwnd, message, wParam, lParam);
        }
        return 0;
}