/* NetHack 3.6	allmain.c	$NHDT-Date: 1446975459 2015/11/08 09:37:39 $  $NHDT-Branch: master $:$NHDT-Revision: 1.66 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* various code that was replicated in *main.c */

#include "hack.h"

#ifndef NO_SIGNAL
#include <signal.h>
#endif

#ifdef POSITIONBAR
STATIC_DCL void NDECL(do_positionbar);
#endif

void
moveloop(resuming)
boolean resuming;
{
#if defined(MICRO) || defined(WIN32)
    char ch;
    int abort_lev;
#endif
    int moveamt = 0, wtcap = 0, change = 0;
    boolean monscanmove = FALSE;

    /* Note:  these initializers don't do anything except guarantee that
            we're linked properly.
    */
    decl_init();
    monst_init();
    monstr_init(); /* monster strengths */
    objects_init();

    if (wizard)
        add_debug_extended_commands();

    /* if a save file created in normal mode is now being restored in
       explore mode, treat it as normal restore followed by 'X' command
       to use up the save file and require confirmation for explore mode */
    if (resuming && iflags.deferred_X)
        (void) enter_explore_mode();

    /* side-effects from the real world */
    flags.moonphase = phase_of_the_moon();
    if (flags.moonphase == FULL_MOON) {
        You("are lucky!  Full moon tonight.");
        change_luck(1);
    } else if (flags.moonphase == NEW_MOON) {
        pline("Be careful!  New moon tonight.");
    }
    flags.friday13 = friday_13th();
    if (flags.friday13) {
        pline("Watch out!  Bad things can happen on Friday the 13th.");
        change_luck(-1);
    }

    if (!resuming) { /* new game */
        context.rndencode = rnd(9000);
        set_wear((struct obj *) 0); /* for side-effects of starting gear */
        (void) pickup(1);      /* autopickup at initial location */
    } else {                   /* restore old game */
#ifndef WIN32
        update_inventory(); /* for perm_invent */
#endif
        read_engr_at(currentX(), currentY()); /* subset of pickup() */
    }
#ifdef WIN32
    update_inventory(); /* for perm_invent */
#endif

    (void) encumber_msg(); /* in case they auto-picked up something */
    if (defer_see_monsters) {
        defer_see_monsters = FALSE;
        see_monsters();
    }
    initrack();

    setPreviousDungeonLevel(currentDungeonLevel());
    youmonst.movement = NORMAL_SPEED; /* give the hero some movement points */
    context.move = 0;

    program_state.in_moveloop = 1;
    for (;;) {
#ifdef SAFERHANGUP
        if (program_state.done_hup)
            end_of_input();
#endif
        get_nh_event();
#ifdef POSITIONBAR
        do_positionbar();
#endif

        if (context.move) {
            /* actual time passed */
            youmonst.movement -= NORMAL_SPEED;

            do { /* hero can't move this turn loop */
                wtcap = encumber_msg();

                context.mon_moving = TRUE;
                do {
                    monscanmove = movemon();
                    if (youmonst.movement >= NORMAL_SPEED)
                        break; /* it's now your turn */
                } while (monscanmove);
                context.mon_moving = FALSE;

                if (!monscanmove && youmonst.movement < NORMAL_SPEED) {
                    /* both you and the monsters are out of steam this round
                     */
                    /* set up for a new turn */
                    struct monst *mtmp;
                    mcalcdistress(); /* adjust monsters' trap, blind, etc */

                    /* reallocate movement rations to monsters */
                    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
                        mtmp->movement += mcalcmove(mtmp);

                    if (!rn2(becameDemigod()
                                 ? 25
                                 : (currentDepth() > depth(&stronghold_level))
                                       ? 50
                                       : 70))
                        (void) makemon((struct permonst *) 0, 0, 0,
                                       NO_MM_FLAGS);

                    /* calculate how much time passed. */
                    if (u.usteed && youMoved()) {
                        /* your speed doesn't augment steed's speed */
                        moveamt = mcalcmove(u.usteed);
                    } else {
                        moveamt = youmonst.data->mmove;

                        if (youAreVeryFast()) { /* speed boots or potion */
                            /* average movement is 1.67 times normal */
                            moveamt += NORMAL_SPEED / 2;
                            if (rn2(3) == 0)
                                moveamt += NORMAL_SPEED / 2;
                        } else if (youAreFast()) {
                            /* average movement is 1.33 times normal */
                            if (rn2(3) != 0)
                                moveamt += NORMAL_SPEED / 2;
                        }
                    }

                    switch (wtcap) {
                    case UNENCUMBERED:
                        break;
                    case SLT_ENCUMBER:
                        moveamt -= (moveamt / 4);
                        break;
                    case MOD_ENCUMBER:
                        moveamt -= (moveamt / 2);
                        break;
                    case HVY_ENCUMBER:
                        moveamt -= ((moveamt * 3) / 4);
                        break;
                    case EXT_ENCUMBER:
                        moveamt -= ((moveamt * 7) / 8);
                        break;
                    default:
                        break;
                    }

                    youmonst.movement += moveamt;
                    if (youmonst.movement < 0)
                        youmonst.movement = 0;
                    settrack();

                    monstermoves++;
                    moves++;

                    /********************************/
                    /* once-per-turn things go here */
                    /********************************/

                    if (youHaveSlipperyFingers())
                        glibr();
                    nh_timeout();
                    run_regions();

                    if (timeToNextBlessing()) {
                        decreaseTimeToNextBlessing(1);
                    }
                    if (flags.time && !running())
                        context.botl = 1;

                    /* One possible result of prayer is healing.  Whether or
                     * not you get healed depends on your current hit points.
                     * If you are allowed to regenerate during the prayer, the
                     * end-of-prayer calculation messes up on this.
                     * Another possible result is rehumanization, which
                     * requires
                     * that encumbrance and movement rate be recalculated.
                     */
                    if (invulnerableWhilePraying()) {
                        /* for the moment at least, you're in tiptop shape */
                        wtcap = UNENCUMBERED;
                    } else if (areYouPolymorphed() && youmonst.data->mlet == S_EEL
                               && !is_pool(currentX(), currentY())
                               && !areYouOnWaterLevel()) {
                        /* eel out of water loses hp, same as for monsters;
                           as hp gets lower, rate of further loss slows down
                           */
                        if (currentHitPointsAsMonster() > 1 && rn2(currentHitPointsAsMonster()) > rn2(8)
                            && (!youTakeHalfDamageFromPhysicalAttacks() || !(moves % 2L))) {
                            decreaseCurrentHitPointsAsMonster(1);
                            context.botl = 1;
                        } else if (currentHitPointsAsMonster() < 1)
                            rehumanize();
                    } else if (areYouPolymorphed() && currentHitPointsAsMonster() < maximumHitPointsAsMonster()) {
                        if (currentHitPointsAsMonster() < 1)
                            rehumanize();
                        else if (youRegenerate()
                                 || (wtcap < MOD_ENCUMBER && !(moves % 20))) {
                            context.botl = 1;
                            increaseCurrentHitPointsAsMonster(1);
                        }
                    } else if (currentHitPoints() < maximumHitPoints()
                               && (wtcap < MOD_ENCUMBER || (! youMoved())
                                   || youRegenerate())) {
                        if (currentExperienceLevel() > 9 && !(moves % 3)) {
                            int heal, Con = (int) ACURR(A_CON);

                            if (Con <= 12) {
                                heal = 1;
                            } else {
                                heal = rnd(Con);
                                if (heal > currentExperienceLevel() - 9)
                                    heal = currentExperienceLevel() - 9;
                            }
                            context.botl = 1;
                            increaseCurrentHitPoints(heal);
                            if (currentHitPoints() > maximumHitPoints())
                                setCurrentHitPoints(maximumHitPoints());
                        } else if (youRegenerate()
                                   || (currentExperienceLevel() <= 9
                                       && !(moves
                                            % ((MAXULEV + 12) / (currentExperienceLevel() + 2)
                                               + 1)))) {
                            context.botl = 1;
                            increaseCurrentHitPoints(1);
                        }
                    }

                    /* moving around while encumbered is hard work */
                    if (wtcap > MOD_ENCUMBER && youMoved()) {
                        if (!(wtcap < EXT_ENCUMBER ? moves % 30
                                                   : moves % 10)) {
                            if (areYouPolymorphed() && currentHitPointsAsMonster() > 1) {
                                decreaseCurrentHitPointsAsMonster(1);
                            } else if (!areYouPolymorphed() && currentHitPoints() > 1) {
                                decreaseCurrentHitPoints(1);
                            } else {
                                You("pass out from exertion!");
                                exercise(A_CON, FALSE);
                                fall_asleep(-10, FALSE);
                            }
                        }
                    }

                    if ((currentMagicalEnergy() < maximumMagicalEnergy())
                        && ((wtcap < MOD_ENCUMBER
                             && (!(moves % ((MAXULEV + 8 - currentExperienceLevel())
                                            * (Role_if(PM_WIZARD) ? 3 : 4)
                                            / 6)))) || yourEnergyRegenerates())) {
                        increaseCurrentMagicalEnergy(rn1(
                            (int) (ACURR(A_WIS) + ACURR(A_INT)) / 15 + 1, 1));
                        if (currentMagicalEnergy() > maximumMagicalEnergy())
                            setCurrentMagicalEnergy(maximumMagicalEnergy());
                        context.botl = 1;
                    }

                    if (!invulnerableWhilePraying()) {
                        if (youCanTeleport() && !rn2(85)) {
                            xchar old_ux = currentX();
                            xchar old_uy = currentY();
                            tele();
                            if (currentX() != old_ux || currentY() != old_uy) {
                                if (!next_to_u()) {
                                    check_leash(old_ux, old_uy);
                                }
                                /* clear doagain keystrokes */
                                pushch(0);
                                savech(0);
                            }
                        }
                        /* delayed change may not be valid anymore */
                        if ((change == 1 && !youPolymorph())
                            || (change == 2 && lycanthropeType() == NON_PM))
                            change = 0;
                        if (youPolymorph() && !rn2(100))
                            change = 1;
                        else if (lycanthropeType() >= LOW_PM && !areYouPolymorphed()
                                 && !rn2(80 - (20 * night())))
                            change = 2;
                        if (change && !youAreUnchanging()) {
                            if (multi >= 0) {
                                if (occupation)
                                    stop_occupation();
                                else
                                    nomul(0);
                                if (change == 1)
                                    polyself(0);
                                else
                                    you_were();
                                change = 0;
                            }
                        }
                    }

                    if (youHaveAutomaticSearching() && multi >= 0)
                        (void) dosearch0(1);
                    dosounds();
                    do_storms();
                    gethungry();
                    age_spells();
                    exerchk();
                    invault();
                    if (haveSpecialItem(SPECIAL_ITEM_AMULET))
                        amulet();
                    if (!rn2(40 + (int) (ACURR(A_DEX) * 3)))
                        u_wipe_engr(rnd(3));
                    if (becameDemigod() && !invulnerableWhilePraying()) {
                        if (timeSinceBecomingADemigod())
                            decreaseTimeSinceBecomingADemigod(1);
                        if (!timeSinceBecomingADemigod()) {
                            intervene();
                            setTimeSinceBecomingADemigod(rn1(200, 50));
                        }
                    }
                    restore_attrib();
                    /* underwater and waterlevel vision are done here */
                    if (areYouOnWaterLevel() || areYouOnAirLevel())
                        movebubbles();
                    else if (areYouOnFireLevel())
                        fumaroles();
                    else if (underwater())
                        showHeroUnderwater(0);
                    /* vision while buried done here */
                    else if (buried())
                        showHeroBeingBuried(0);

                    /* when immobile, count is in turns */
                    if (multi < 0) {
                        if (++multi == 0) { /* finished yet? */
                            unmul((char *) 0);
                            /* if unmul caused a level change, take it now */
                            if (0 != typeOfLevelYouWereSentTo()) {
                                deferred_goto();
                            }
                        }
                    }
                }
            } while (youmonst.movement
                     < NORMAL_SPEED); /* hero can't move loop */

            /******************************************/
            /* once-per-hero-took-time things go here */
            /******************************************/

            if (context.bypasses) {
                clear_bypasses();
            }
            if ((haveSpecialItem(SPECIAL_ITEM_AMULET) || youAreClairvoyant()) && !areYouInEndgame()
                && !youAreBlockedFrom(CLAIRVOYANT) && !(moves % 15) && !rn2(2)) {
                do_vicinity_map();
            }
            if (currentlyTrapped() && (currentTrapType() == TT_LAVA)) {
                sink_into_lava();
            }
            /* when/if hero escapes from lava, he can't just stay there */
            else if (! youMoved()) {
                (void) pooleffects(FALSE);
            }

        } /* actual time passed */

        /****************************************/
        /* once-per-player-input things go here */
        /****************************************/

        clear_splitobjs();
        find_ac();
        if (!context.mv || youCannotSee()) {
            /* redo monsters if hallu or wearing a helm of telepathy */
            if (youAreHallucinating()) { /* update screen randomly */
                see_monsters();
                see_objects();
                see_traps();
                if (swallowed())
                    showHeroBeingSwallowed(0);
            } else if (youHaveTelepathyWhenNotBlind()) {
                see_monsters();
            } else if (youSenseWarnings() || youAreWarnedOfMonsters())
                see_monsters();

            if (vision_full_recalc)
                vision_recalc(0); /* vision! */
        }
        if (context.botl || context.botlx) {
            bot();
            curs_on_u();
        }

        context.move = 1;

        if (multi >= 0 && occupation) {
#if defined(MICRO) || defined(WIN32)
            abort_lev = 0;
            if (kbhit()) {
                if ((ch = pgetchar()) == ABORT)
                    abort_lev++;
                else
                    pushch(ch);
            }
            if (!abort_lev && (*occupation)() == 0)
#else
            if ((*occupation)() == 0)
#endif
                occupation = 0;
            if (
#if defined(MICRO) || defined(WIN32)
                abort_lev ||
#endif
                monster_nearby()) {
                stop_occupation();
                reset_eat();
            }
#if defined(MICRO) || defined(WIN32)
            if (!(++occtime % 7))
                display_nhwindow(WIN_MAP, FALSE);
#endif
            continue;
        }

        if (iflags.sanity_check)
            sanity_check();

#ifdef CLIPPING
        /* just before rhack */
        cliparound(currentX(), currentY());
#endif

        setYouMoved(FALSE);

        if (multi > 0) {
            lookaround();
            if (!multi) {
                /* lookaround may clear multi */
                context.move = 0;
                if (flags.time)
                    context.botl = 1;
                continue;
            }
            if (context.mv) {
                if (multi < COLNO && !--multi) {
                    context.travel = context.travel1 = context.mv = 0;
                    setRunningPace(WALK_ONE_SQUARE);
                }
                domove();
            } else {
                --multi;
                rhack(save_cm);
            }
        } else if (multi == 0) {
#ifdef MAIL
            ckmailstatus();
#endif
            rhack((char *) 0);
        }
        if (0 != typeOfLevelYouWereSentTo()) {       /* change dungeon level */
            deferred_goto(); /* after rhack() */
        }
        /* !context.move here: multiple movement command stopped */
        else if (flags.time && (!context.move || !context.mv))
            context.botl = 1;

        if (vision_full_recalc)
            vision_recalc(0); /* vision! */
        /* when running in non-tport mode, this gets done through domove() */
        if ((!running() || flags.runmode == RUN_TPORT)
            && (multi && (!context.travel ? !(multi % 7) : !(moves % 7L)))) {
            if (flags.time && running())
                context.botl = 1;
            display_nhwindow(WIN_MAP, FALSE);
        }
    }
}

void
stop_occupation()
{
    if (occupation) {
        if (!maybe_finished_meal(TRUE))
            You("stop %s.", occtxt);
        occupation = 0;
        context.botl = 1; /* in case hungerState changed */
        nomul(0);
        pushch(0);
    }
}

void
display_gamewindows()
{
    WIN_MESSAGE = create_nhwindow(NHW_MESSAGE);
#ifdef STATUS_VIA_WINDOWPORT
    status_initialize(0);
#else
    WIN_STATUS = create_nhwindow(NHW_STATUS);
#endif
    WIN_MAP = create_nhwindow(NHW_MAP);
    WIN_INVEN = create_nhwindow(NHW_MENU);

#ifdef MAC
    /* This _is_ the right place for this - maybe we will
     * have to split display_gamewindows into create_gamewindows
     * and show_gamewindows to get rid of this ifdef...
     */
    if (!strcmp(windowprocs.name, "mac"))
        SanePositions();
#endif

    /*
     * The mac port is not DEPENDENT on the order of these
     * displays, but it looks a lot better this way...
     */
#ifndef STATUS_VIA_WINDOWPORT
    display_nhwindow(WIN_STATUS, FALSE);
#endif
    display_nhwindow(WIN_MESSAGE, FALSE);
    clear_glyph_buffer();
    display_nhwindow(WIN_MAP, FALSE);
}

void
newgame()
{
    int i;

#ifdef MFLOPPY
    gameDiskPrompt();
#endif

    context.botlx = 1;
    initIdentifier(1);
    context.stethoscope_move = -1L;
    context.warnlevel = 1;
    context.next_attrib_check = 600L; /* arbitrary first setting */
    context.tribute.enabled = TRUE;   /* turn on 3.6 tributes    */
    context.tribute.tributesz = sizeof(struct tribute_info);

    for (i = 0; i < NUMMONS; i++)
        mvitals[i].mvflags = mons[i].geno & G_NOCORPSE;

    init_objects(); /* must be before u_init() */

    flags.pantheon = -1; /* role_init() will reset this */
    role_init();         /* must be before init_dungeons(), u_init(),
                          * and init_artifacts() */

    init_dungeons();  /* must be before u_init() to avoid rndmonst()
                       * creating odd monsters for any tins and eggs
                       * in hero's initial inventory */
    init_artifacts(); /* before u_init() in case $WIZKIT specifies
                       * any artifacts */
    u_init();

#ifndef NO_SIGNAL
    (void) signal(SIGINT, (SIG_RET_TYPE) done1);
#endif
#ifdef NEWS
    if (iflags.news)
        display_file(NEWS, FALSE);
#endif
    load_qtlist();          /* load up the quest text info */
    /* quest_init();  --  Now part of role_init() */

    mklev();
    u_on_upstairs();
    if (wizard)
        obj_delivery(FALSE); /* finish wizkit */
    vision_reset();          /* set up internals for level (after mklev) */
    check_special_room(FALSE);

    if (MON_AT(currentX(), currentY()))
        mnexto(m_at(currentX(), currentY()));
    (void) makedog();
    docrt();

    if (flags.legacy) {
        flush_screen(1);
        com_pager(1);
    }

#ifdef INSURANCE
    save_currentstate();
#endif
    program_state.something_worth_saving++; /* useful data now exists */

    urealtime.realtime = 0L;
#if defined(BSD) && !defined(POSIX_TYPES)
    (void) time((long *) &urealtime.restored);
#else
    (void) time(&urealtime.restored);
#endif

    /* Success! */
    welcome(TRUE);
    return;
}

/* show "welcome [back] to nethack" message at program startup */
void
welcome(new_game)
boolean new_game; /* false => restoring an old game */
{
    char buf[BUFSZ];
    boolean currentgend = areYouPolymorphed() ? inherentlyFemale() : flags.female;

    /*
     * The "welcome back" message always describes your innate form
     * even when polymorphed or wearing a helm of opposite alignment.
     * Alignment is shown unconditionally for new games; for restores
     * it's only shown if it has changed from its original value.
     * Sex is shown for new games except when it is redundant; for
     * restores it's only shown if different from its original value.
     */
    *buf = '\0';
    if (new_game || originalAlignmentBase() != currentAlignmentBase())
        Sprintf(eos(buf), " %s", align_str(originalAlignmentBase()));
    if (!urole.name.f
        && (new_game
                ? (urole.allow & ROLE_GENDMASK) == (ROLE_MALE | ROLE_FEMALE)
                : currentgend != flags.initgend))
        Sprintf(eos(buf), " %s", genders[currentgend].adj);

    pline(new_game ? "%s %s, welcome to NetHack!  You are a%s %s %s."
                   : "%s %s, the%s %s %s, welcome back to NetHack!",
          Hello((struct monst *) 0), plname, buf, urace.adj,
          (currentgend && urole.name.f) ? urole.name.f : urole.name.m);
}

#ifdef POSITIONBAR
STATIC_DCL void
do_positionbar()
{
    static char pbar[COLNO];
    char *p;

    p = pbar;
    /* up stairway */
    if (upstair.sx
        && (glyph_to_cmap(level.locations[upstair.sx][upstair.sy].glyph)
                == S_upstair
            || glyph_to_cmap(level.locations[upstair.sx][upstair.sy].glyph)
                   == S_upladder)) {
        *p++ = '<';
        *p++ = upstair.sx;
    }
    if (sstairs.sx
        && (glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph)
                == S_upstair
            || glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph)
                   == S_upladder)) {
        *p++ = '<';
        *p++ = sstairs.sx;
    }

    /* down stairway */
    if (dnstair.sx
        && (glyph_to_cmap(level.locations[dnstair.sx][dnstair.sy].glyph)
                == S_dnstair
            || glyph_to_cmap(level.locations[dnstair.sx][dnstair.sy].glyph)
                   == S_dnladder)) {
        *p++ = '>';
        *p++ = dnstair.sx;
    }
    if (sstairs.sx
        && (glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph)
                == S_dnstair
            || glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph)
                   == S_dnladder)) {
        *p++ = '>';
        *p++ = sstairs.sx;
    }

    /* hero location */
    if (currentX()) {
        *p++ = '@';
        *p++ = currentX();
    }
    /* fence post */
    *p = 0;

    update_positionbar(pbar);
}
#endif

/*allmain.c*/
