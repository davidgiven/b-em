# Makefile.am for B-em

bin_PROGRAMS = b-em
noinst_SCRIPTS = ../b-em
CLEANFILES = $(noinst_SCRIPTS)

../b-em: b-em
	cp b-em ..

amrefresh:

# The following flags are used by PiTubeDirect Co Pros:
#                BEM - fixes for compilation in B-Em envorinment
#   INCLUDE_DEBUGGER - include the cpu_debug implementation
# USE_MEMORY_POINTER - do not assume Co Pro memory starts at address 0

b_em_CFLAGS = $(allegro_CFLAGS) -DBEM -DINCLUDE_DEBUGGER -DUSE_MEMORY_POINTER

# workaround for Win32 Allegro, which has `allegro-config' missing
if OS_WIN
b_em_LDADD = -lalleg -lz -lopenal -lalut -lm
else
b_em_LDADD = $(allegro_LIBS) -lz -lopenal -lalut -lm -lpthread
endif

b_em_SOURCES = \
	6502.c \
	6502debug.c \
	6502tube.c \
	65816.c \
	NS32016/32016.c \
	NS32016/32016_debug.c \
	NS32016/Decode.c \
	NS32016/NSDis.c \
	NS32016/Profile.c \
	NS32016/Trap.c \
	NS32016/mem32016.c \
	Z80.c \
	z80dis.c \
	acia.c \
	adc.c \
	arm.c \
	darm/darm.c \
	darm/darm-tbl.c \
	darm/armv7.c \
	darm/armv7-tbl.c \
	darm/thumb.c \
	darm/thumb-tbl.c \
	darm/thumb2.c \
	darm/thumb2-decoder.c \
	darm/thumb2-tbl.c \
	cmos.c \
	compact_joystick.c \
	compactcmos.c \
	compat_wrappers.c \
	config.c \
	csw.c \
	ddnoise.c \
	debugger.c \
	disc.c fdi.c \
	fdi2raw.c \
	i8271.c \
	ide.c \
	keyboard.c \
	linux-gui.c \
	linux-keydefine.c \
	linux.c \
	logging.c \
	main.c \
	mem.c \
	model.c \
	mouse.c \
	midi-linux.c \
	music2000.c \
	music4000.c \
	music5000.c \
	resid.cc \
	savestate.c \
	scsi.c \
	sdf.c \
	serial.c \
	sn76489.c \
	sound.c \
	soundopenal.c \
	sysacia.c \
	sysvia.c \
	tape.c \
	tapenoise.c \
	tube.c \
	uef.c \
	uservia.c \
	vdfs.c \
	via.c \
	vidalleg.c \
	video.c \
	wd1770.c \
	x86.c \
	x86dasm.c \
	resid-fp/convolve-sse.cc \
	resid-fp/convolve.cc \
	resid-fp/envelope.cc \
	resid-fp/extfilt.cc \
	resid-fp/filter.cc \
	resid-fp/pot.cc \
	resid-fp/sid.cc \
	resid-fp/voice.cc \
	resid-fp/wave.cc \
	resid-fp/wave6581_PST.cc \
	resid-fp/wave6581_PS_.cc \
	resid-fp/wave6581_P_T.cc \
	resid-fp/wave6581__ST.cc \
	resid-fp/wave8580_PST.cc \
	resid-fp/wave8580_PS_.cc \
	resid-fp/wave8580_P_T.cc \
	resid-fp/wave8580__ST.cc

if NO_TSEARCH
b_em_SOURCES += tsearch.c
endif

