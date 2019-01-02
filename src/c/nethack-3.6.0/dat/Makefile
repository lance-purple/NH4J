#
# This file is generated automatically.  Do not edit.
# Your changes will be lost.  See sys/unix/NewInstall.unx.
# Identify this file:
MAKEFILE_DAT=1

###
### Start sys/unix/hints/cygwin PRE
###
# (new segment at source line 6 )
# unix hints file
# This hints file provides the legacy configuration that NetHack has shipped
# with historically - this means you will most likely need to hand edit .h
# and Makefiles.


PREFIX=/usr
HACKDIR=$(PREFIX)/games/lib/$(GAME)dir
INSTDIR=$(HACKDIR)
VARDIR=$(HACKDIR)
SHELLDIR=$(PREFIX)/games

CFLAGS=-O -I../include
LINK=$(CC)

WINSRC = $(WINTTYSRC)
WINOBJ = $(WINTTYOBJ)
WINLIB = $(WINTTYLIB)

WINTTYLIB=-lncurses

CHOWN=chown
CHGRP=chgrp

GAMEUID  = Lance
GAMEGRP  = Users

GAMEPERM = 04755
VARFILEPERM = 0644
VARDIRPERM = 0755

POSTINSTALL=cp -n sys/unix/sysconf $(INSTDIR)/sysconf; $(CHOWN) $(GAMEUID) $(INSTDIR)/sysconf; $(CHGRP) $(GAMEGRP) $(INSTDIR)/sysconf; chmod $(VARFILEPERM) $(INSTDIR)/sysconf;
### End sys/unix/hints/cygwin PRE

###
### Start Makefile.dat
###
#	NetHack Makefile.
# NetHack 3.6  Makefile.dat	$NHDT-Date: 1447844574 2015/11/18 11:02:54 $  $NHDT-Branch: master $:$NHDT-Revision: 1.18 $

# Root of source tree:
NHSROOT=..

# for Atari
# SHELL=E:/GEMINI2/MUPFEL.TTP
# UUDECODE=uudecode

VARDAT = bogusmon data engrave epitaph rumors quest.dat oracles options

all:	$(VARDAT) spec_levs quest_levs dungeon

../util/makedefs:
	(cd ../util ; $(MAKE) makedefs)

../util/dgn_comp:
	(cd ../util ; $(MAKE) dgn_comp)

../util/lev_comp:
	(cd ../util ; $(MAKE) lev_comp)

../util/tile2x11:
	(cd ../util ; $(MAKE) tile2x11)

../util/tile2beos:
	(cd ../util ; $(MAKE) tile2beos)

../util/tile2bmp:
	(cd ../util ; $(MAKE) tile2bmp)

x11tiles: ../util/tile2x11 ../win/share/monsters.txt ../win/share/objects.txt \
				../win/share/other.txt
	../util/tile2x11 ../win/share/monsters.txt ../win/share/objects.txt \
				../win/share/other.txt

beostiles: ../util/tile2beos ../win/share/monsters.txt \
				../win/share/objects.txt \
				../win/share/other.txt
	../util/tile2beos ../win/share/monsters.txt \
				../win/share/objects.txt \
				../win/share/other.txt

nhtiles.bmp: ../util/tile2bmp ../win/share/monsters.txt \
				../win/share/objects.txt \
				../win/share/other.txt
	../util/tile2bmp $@

NetHack.ad: ../win/X11/NetHack.ad
	cp ../win/X11/NetHack.ad NetHack.ad

pet_mark.xbm: ../win/X11/pet_mark.xbm
	cp ../win/X11/pet_mark.xbm pet_mark.xbm

pilemark.xbm: ../win/X11/pilemark.xbm
	cp ../win/X11/pilemark.xbm pilemark.xbm

rip.xpm: ../win/X11/rip.xpm
	cp ../win/X11/rip.xpm rip.xpm

mapbg.xpm: ../win/gnome/mapbg.xpm
	cp ../win/gnome/mapbg.xpm mapbg.xpm

nhsplash.xpm: ../win/Qt/nhsplash.xpm
	cp ../win/Qt/nhsplash.xpm nhsplash.xpm

nethack.icns: ../win/Qt/nhicns.uu
	$(UUDECODE) ../win/Qt/nhicns.uu

Info.plist: ../win/Qt/Info.pli
	cp ../win/Qt/Info.pli Info.plist

../util/tile2img.ttp:
	(cd ../util ; $(MAKE) tile2img.ttp)

../util/xpm2img.ttp:
	(cd ../util ; $(MAKE) xpm2img.ttp)
nh16.img: ../util/tile2img.ttp ../win/share/monsters.txt \
				../win/share/objects.txt ../win/share/other.txt
	../util/tile2img.ttp nh16.img

rip.img: ../util/xpm2img.ttp
	../util/xpm2img.ttp ../win/X11/rip.xpm rip.img
title.img:
	# cp ../win/gem/title.img title.img
	$(UUDECODE) ../win/gem/title.uu

GEM_RSC.RSC:
	# cp ../win/gem/GEM_RSC.RSC GEM_RSC.RSC
	$(UUDECODE) ../win/gem/gem_rsc.uu

MONSTER_COUNT := $(shell java -cp ../../../../classes/ rec.games.roguelike.nh4j.build.Makedefs monster.count)

MAKEDEFS := MONSTER_COUNT=$(MONSTER_COUNT) ../util/makedefs

data:	data.base ../util/makedefs
	$(MAKEDEFS) -d

rumors:	rumors.tru rumors.fal ../util/makedefs
	$(MAKEDEFS) -r

quest.dat:	quest.txt ../util/makedefs
	$(MAKEDEFS) -q

oracles:	oracles.txt ../util/makedefs
	$(MAKEDEFS) -h

engrave:	engrave.txt ../util/makedefs
	$(MAKEDEFS) -s

epitaph:	epitaph.txt ../util/makedefs
	$(MAKEDEFS) -s

bogusmon:	bogusmon.txt ../util/makedefs
	$(MAKEDEFS) -s

# note: 'options' should have already been made when include/date.h was created
options:	../util/makedefs
	$(MAKEDEFS) -v


spec_levs: ../util/lev_comp \
	bigroom.des castle.des endgame.des gehennom.des knox.des medusa.des \
	mines.des oracle.des sokoban.des tower.des yendor.des
	../util/lev_comp bigroom.des
	../util/lev_comp castle.des
	../util/lev_comp endgame.des
	../util/lev_comp gehennom.des
	../util/lev_comp knox.des
	../util/lev_comp medusa.des
	../util/lev_comp mines.des
	../util/lev_comp oracle.des
	../util/lev_comp sokoban.des
	../util/lev_comp tower.des
	../util/lev_comp yendor.des
	touch spec_levs

quest_levs: ../util/lev_comp \
	Arch.des Barb.des Caveman.des Healer.des Knight.des Monk.des \
	Priest.des Ranger.des Rogue.des Samurai.des Tourist.des Valkyrie.des \
	Wizard.des
	../util/lev_comp Arch.des
	../util/lev_comp Barb.des
	../util/lev_comp Caveman.des
	../util/lev_comp Healer.des
	../util/lev_comp Knight.des
	../util/lev_comp Monk.des
	../util/lev_comp Priest.des
	../util/lev_comp Ranger.des
	../util/lev_comp Rogue.des
	../util/lev_comp Samurai.des
	../util/lev_comp Tourist.des
	../util/lev_comp Valkyrie.des
	../util/lev_comp Wizard.des
	touch quest_levs

dungeon: dungeon.def ../util/makedefs ../util/dgn_comp
	$(MAKEDEFS) -e
	../util/dgn_comp dungeon.pdf

spotless:
	-rm -f spec_levs quest_levs *.lev $(VARDAT) dungeon dungeon.pdf
	-rm -f nhdat x11tiles beostiles pet_mark.xbm pilemark.xbm rip.xpm mapbg.xpm
	-rm -f rip.img GEM_RSC.RSC title.img nh16.img NetHack.ad
### End Makefile.dat

###
### Start sys/unix/hints/cygwin POST
###
### End sys/unix/hints/cygwin POST
