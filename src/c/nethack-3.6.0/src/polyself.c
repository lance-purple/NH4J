/* NetHack 3.6	polyself.c	$NHDT-Date: 1448496566 2015/11/26 00:09:26 $  $NHDT-Branch: master $:$NHDT-Revision: 1.104 $ */
/*      Copyright (C) 1987, 1988, 1989 by Ken Arromdee */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Polymorph self routine.
 *
 * Note:  the light source handling code assumes that both youmonst.m_id
 * and youmonst.mx will always remain 0 when it handles the case of the
 * player polymorphed into a light-emitting monster.
 *
 * Transformation sequences:
 *              /-> polymon                 poly into monster form
 *    polyself =
 *              \-> newman -> polyman       fail to poly, get human form
 *
 *    rehumanize -> polyman                 return to original form
 *
 *    polymon (called directly)             usually golem petrification
 */

#include "hack.h"

STATIC_DCL void FDECL(check_strangling, (BOOLEAN_P));
STATIC_DCL void FDECL(polyman, (const char *, const char *));
STATIC_DCL void NDECL(break_armor);
STATIC_DCL void FDECL(drop_weapon, (int));
STATIC_DCL void NDECL(uunstick);
STATIC_DCL int FDECL(armor_to_dragon, (int));
STATIC_DCL void NDECL(newman);
STATIC_DCL boolean FDECL(polysense, (struct permonst *));

STATIC_VAR const char no_longer_petrify_resistant[] =
    "No longer petrify-resistant, you";

/* controls whether taking on new form or becoming new man can also
   change sex (ought to be an arg to polymon() and newman() instead) */
STATIC_VAR int sex_change_ok = 0;

static void PROPSET(int propertyIndex, boolean on) {
  if (on)
    setYourIntrinsicMask(propertyIndex, FROMFORM);
  else
    unsetYourIntrinsicMask(propertyIndex, FROMFORM);
}


/* update the youmonst.data structure pointer and intrinsics */
void
set_uasmon()
{
    struct permonst *mdat = &mons[currentMonsterNumber()];

    set_mon_data(&youmonst, mdat, 0);

    PROPSET(FIRE_RES, resists_fire(&youmonst));
    PROPSET(COLD_RES, resists_cold(&youmonst));
    PROPSET(SLEEP_RES, resists_sleep(&youmonst));
    PROPSET(DISINT_RES, resists_disint(&youmonst));
    PROPSET(SHOCK_RES, resists_elec(&youmonst));
    PROPSET(POISON_RES, resists_poison(&youmonst));
    PROPSET(ACID_RES, resists_acid(&youmonst));
    PROPSET(STONE_RES, resists_ston(&youmonst));
    {
        /* resists_drli() takes wielded weapon into account; suppress it */
        struct obj *save_uwep = uwep;

        uwep = 0;
        PROPSET(DRAIN_RES, resists_drli(&youmonst));
        uwep = save_uwep;
    }
    /* resists_magm() takes wielded, worn, and carried equipment into
       into account; cheat and duplicate its monster-specific part */
    PROPSET(ANTIMAGIC, (dmgtype(mdat, AD_MAGM)
                        || mdat == &mons[PM_BABY_GRAY_DRAGON]
                        || dmgtype(mdat, AD_RBRE)));
    PROPSET(SICK_RES, (monsterClass(mdat->monsterTypeID) == S_FUNGUS || mdat == &mons[PM_GHOUL]));

    PROPSET(STUNNED, (mdat == &mons[PM_STALKER] || isBat(mdat->monsterTypeID)));
    PROPSET(HALLUC_RES, dmgtype(mdat, AD_HALU));
    PROPSET(SEE_INVIS, perceivesTheInvisible(mdat->monsterTypeID));
    PROPSET(TELEPAT, telepathic(mdat));
    PROPSET(INFRAVISION, hasInfravision(mdat->monsterTypeID));
    PROPSET(INVIS, pm_invisible(mdat));
    PROPSET(TELEPORT, canTeleport(mdat->monsterTypeID));
    PROPSET(TELEPORT_CONTROL, canControlTeleport(mdat->monsterTypeID));
    PROPSET(LEVITATION, isFloater(mdat->monsterTypeID));
    PROPSET(FLYING, isFlyer(mdat->monsterTypeID));
    PROPSET(SWIMMING, isSwimmer(mdat->monsterTypeID));
    /* [don't touch MAGICAL_BREATHING here; both Amphibious and Breathless
       key off of it but include different monster forms...] */
    PROPSET(PASSES_WALLS, passesThroughWalls(mdat->monsterTypeID));
    PROPSET(REGENERATION, regenerates(mdat->monsterTypeID));
    PROPSET(REFLECTING, (mdat == &mons[PM_SILVER_DRAGON]));

    float_vs_flight(); /* maybe toggle (BFlying & I_SPECIAL) */

#undef PROPSET

#ifdef STATUS_VIA_WINDOWPORT
    status_initialize(REASSESS_ONLY);
#endif
}

/* Levitation overrides Flying; set or clear BFlying|I_SPECIAL */
void
float_vs_flight()
{
    /* floating overrides flight; normally float_up() and float_down()
       handle this, but sometimes they're skipped */
    if (yourIntrinsic(LEVITATION) || yourExtrinsic(LEVITATION))
        setYourBlockerMask(FLYING, I_SPECIAL);
    else
        unsetYourBlockerMask(FLYING, I_SPECIAL);
}

/* for changing into form that's immune to strangulation */
STATIC_OVL void
check_strangling(on)
boolean on;
{
    /* on -- maybe resume strangling */
    if (on) {
        /* when Strangled is already set, polymorphing from one
           vulnerable form into another causes the counter to be reset */
        if (uamul && uamul->otyp == AMULET_OF_STRANGULATION
            && can_be_strangled(&youmonst)) {
            Your("%s %s your %s!", simpleonames(uamul),
                 youAreBeingStrangled() ? "still constricts" : "begins constricting",
                 body_part(NECK)); /* "throat" */
            setYourIntrinsic(STRANGLED, 6L);
            makeknown(AMULET_OF_STRANGULATION);
        }

    /* off -- maybe block strangling */
    } else {
        if (youAreBeingStrangled() && !can_be_strangled(&youmonst)) {
            setYourIntrinsic(STRANGLED, 0L);
            You("are no longer being strangled.");
        }
    }
}

/* make a (new) human out of the player */
STATIC_OVL void
polyman(fmt, arg)
const char *fmt, *arg;
{
    boolean sticky = (sticks(youmonst.data) && u.ustuck && !swallowed()),
            was_mimicking = (youmonst.m_ap_type == M_AP_OBJECT);
    boolean was_blind = !!youCannotSee();

    if (areYouPolymorphed()) {
        /* restore old attribs */
        int i;
        for (i = 0; i < A_MAX; i++) {
            setYourCurrentAttr(i, yourAttrAsMonster(i));
            setYourAttrMax    (i, yourAttrMaxAsMonster(i));
        } 
        setCurrentMonsterNumber(originalMonsterNumber());
        flags.female = inherentlyFemale();
    }
    set_uasmon();

    setCurrentHitPointsAsMonster(0);
    setMaximumHitPointsAsMonster(0);
    setTimeRemainingAsMonster(0);
    skinback(FALSE);
    setLurking(FALSE);

    if (sticky)
        uunstick();
    find_ac();
    if (was_mimicking) {
        if (multi < 0)
            unmul("");
        youmonst.m_ap_type = M_AP_NOTHING;
    }

    newsym(currentX(), currentY());

    You(fmt, arg);
    /* check whether player foolishly genocided self while poly'd */
    if ((mvitals[urole.malenum].mvflags & G_GENOD)
        || (urole.femalenum != NON_PM
            && (mvitals[urole.femalenum].mvflags & G_GENOD))
        || (mvitals[urace.malenum].mvflags & G_GENOD)
        || (urace.femalenum != NON_PM
            && (mvitals[urace.femalenum].mvflags & G_GENOD))) {
        /* intervening activity might have clobbered genocide info */
        struct kinfo *kptr = find_delayed_killer(POLYMORPH);

        if (kptr != (struct kinfo *) 0 && kptr->name[0]) {
            killer.format = kptr->format;
            Strcpy(killer.name, kptr->name);
        } else {
            killer.format = KILLED_BY;
            Strcpy(killer.name, "self-genocide");
        }
        dealloc_killer(kptr);
        done(GENOCIDED);
    }

    if (usingTwoWeapons() && cannotUseTwoWeapons(youmonst.data))
        untwoweapon();

    if (currentlyTrapped() && currentTrapType() == TT_PIT) {
        setCurrentTrapTimeout(rn1(6, 2)); /* time to escape resets */
    }
    if (was_blind && youCanSee()) { /* reverting from eyeless */
        setYourIntrinsic(BLINDED, 1L);
        make_blinded(0L, TRUE); /* remove blindness */
    }
    check_strangling(TRUE);

    if (!youAreLevitating() && !u.ustuck && is_pool_or_lava(currentX(), currentY()))
        spoteffects(TRUE);

    see_monsters();
}

void
change_sex()
{
    /* setting originalMonsterNumber() for caveman/cavewoman or priest/priestess
       swap unintentionally makes `areYouPolymorphed()' appear to be true */
    boolean already_polyd = (boolean) areYouPolymorphed();

    /* Some monsters are always of one sex and their sex can't be changed;
     * Succubi/incubi can change, but are handled below.
     *
     * !already_polyd check necessary because isMale() and isFemale()
     * are true if the player is a priest/priestess.
     */
    if (!already_polyd
        || (!isMale(youmonst.data->monsterTypeID) && !isFemale(youmonst.data->monsterTypeID)
            && !isNeuter(youmonst.data->monsterTypeID)))
        flags.female = !flags.female;
    if (already_polyd) /* poly'd: also change saved sex */
        setInherentlyFemale(!inherentlyFemale());
    max_rank_sz(); /* [this appears to be superfluous] */
    if ((already_polyd ? inherentlyFemale() : flags.female) && urole.name.f)
        Strcpy(pl_character, urole.name.f);
    else
        Strcpy(pl_character, urole.name.m);
    setOriginalMonsterNumber( ((already_polyd ? inherentlyFemale() : flags.female)
                  && urole.femalenum != NON_PM)
                     ? urole.femalenum
                     : urole.malenum );
    if (!already_polyd) {
        setCurrentMonsterNumber(originalMonsterNumber());
    } else if (currentMonsterNumber() == PM_SUCCUBUS || currentMonsterNumber() == PM_INCUBUS) {
        flags.female = !flags.female;
        /* change monster type to match new sex */
        setCurrentMonsterNumber((currentMonsterNumber() == PM_SUCCUBUS) ? PM_INCUBUS : PM_SUCCUBUS );
        set_uasmon();
    }
}

STATIC_OVL void
newman()
{
    int i, oldlvl, newlvl, hpmax, enmax;

    oldlvl = currentExperienceLevel();
    newlvl = oldlvl + rn1(5, -2);     /* new = old + {-2,-1,0,+1,+2} */
    if (newlvl > 127 || newlvl < 1) { /* level went below 0? */
        goto dead; /* old level is still intact (in case of lifesaving) */
    }
    if (newlvl > MAXULEV)
        newlvl = MAXULEV;
    /* If your level goes down, your peak level goes down by
       the same amount so that you can't simply use blessed
       full healing to undo the decrease.  But if your level
       goes up, your peak level does *not* undergo the same
       adjustment; you might end up losing out on the chance
       to regain some levels previously lost to other causes. */
    if (newlvl < oldlvl) {
        setHighestExperienceLevelSoFar(oldlvl - newlvl);
    }
    if (highestExperienceLevelSoFar() < newlvl) {
        setHighestExperienceLevelSoFar(newlvl);
    }
    setCurrentExperienceLevel(newlvl);

    if (sex_change_ok && !rn2(10))
        change_sex();

    adjabil(oldlvl, (int) currentExperienceLevel());
    reset_rndmonst(NON_PM); /* new monster generation criteria */

    /* random experience points for the new experience level */
    setCurrentXP(rndexp(FALSE));

    /* set up new attribute points (particularly Con) */
    redist_attr();

    /*
     * New hit points:
     *  remove level-gain based HP from any extra HP accumulated
     *  (the "extra" might actually be negative);
     *  modify the extra, retaining {80%, 90%, 100%, or 110%};
     *  add in newly generated set of level-gain HP.
     *
     * (This used to calculate new HP in direct proportion to old HP,
     * but that was subject to abuse:  accumulate a large amount of
     * extra HP, drain level down to 1, then polyself to level 2 or 3
     * [lifesaving capability needed to handle level 0 and -1 cases]
     * and the extra got multiplied by 2 or 3.  Repeat the level
     * drain and polyself steps until out of lifesaving capability.)
     */
    hpmax = maximumHitPoints();
    for (i = 0; i < oldlvl; i++) {
        hpmax -= hitPointIncreasePerLevel(i);
    }
    /* hpmax * rn1(4,8) / 10; 0.95*hpmax on average */
    hpmax = rounddiv((long) hpmax * (long) rn1(4, 8), 10);
    for (i = 0; (setCurrentExperienceLevel(i), i < newlvl); i++) {
        hpmax += newhp();
    }
    if (hpmax < currentExperienceLevel()) {
        hpmax = currentExperienceLevel(); /* min of 1 HP per level */
    }
    /* retain same proportion for current HP; currentHitPoints() * hpmax / maximumHitPoints() */
    setCurrentHitPoints(rounddiv((long) currentHitPoints() * (long) hpmax, maximumHitPoints()));
    setMaximumHitPoints(hpmax);
    /*
     * Do the same for spell power.
     */
    enmax = maximumMagicalEnergy();
    for (i = 0; i < oldlvl; i++) {
        enmax -= magicalEnergyIncreasePerLevel(i);
    }
    enmax = rounddiv((long) enmax * (long) rn1(4, 8), 10);
    for (i = 0; (setCurrentExperienceLevel(i), i < newlvl); i++) {
        enmax += newpw();
    }
    if (enmax < currentExperienceLevel()) {
        enmax = currentExperienceLevel();
    }
    setCurrentMagicalEnergy(rounddiv((long) currentMagicalEnergy() * (long) enmax,
                     ((maximumMagicalEnergy() < 1) ? 1 : maximumMagicalEnergy())));
    setMaximumMagicalEnergy(enmax);
    /* [should alignment record be tweaked too?] */

    setCurrentNutrition(rn1(500, 500));
    if (youAreSick())
        make_sick(0L, (char *) 0, FALSE, SICK_ALL);
    if (youAreTurningToStone())
        make_stoned(0L, (char *) 0, 0, (char *) 0);
    if (currentHitPoints() <= 0) {
        if (youHavePolymorphControl()) { /* even when stunned or unaware */
            if (currentHitPoints() <= 0)
                setCurrentHitPoints(1);
        } else {
        dead: /* we come directly here if their experience level went to 0 or
                 less */
            Your("new form doesn't seem healthy enough to survive.");
            killer.format = KILLED_BY_AN;
            Strcpy(killer.name, "unsuccessful polymorph");
            done(DIED);
            newuhs(FALSE);
            (void) polysense(youmonst.data);
            return; /* lifesaved */
        }
    }
    newuhs(FALSE);
    polyman("feel like a new %s!",
            /* use saved gender we're about to revert to, not current */
            (inherentlyFemale() && urace.individual.f)
                ? urace.individual.f
                : (urace.individual.m) ? urace.individual.m : urace.noun);
    if (youAreTurningToSlime()) {
        Your("body transforms, but there is still slime on you.");
        make_slimed(10L, (const char *) 0);
    }

    (void) polysense(youmonst.data);
    context.botl = 1;
    see_monsters();
    (void) encumber_msg();

    retouch_equipment(2);
    if (!uarmg)
        selftouch(no_longer_petrify_resistant);
}

void
polyself(psflags)
int psflags;
{
    char buf[BUFSZ];
    int old_light, new_light, mntmp, class, tryct;
    boolean forcecontrol = (psflags == 1), monsterpoly = (psflags == 2),
            draconian = (uarm && Is_dragon_armor(uarm)),
            iswere = (lycanthropeType() >= LOW_PM), isvamp = isVampire(youmonst.data->monsterTypeID),
            controllable_poly = youHavePolymorphControl() && !(youAreStunned() || youAreUnaware());

    if (youAreUnchanging()) {
        pline("You fail to transform!");
        return;
    }
    /* being stunned or unaware doesn't negate this aspect of Poly_control */
    if (!youHavePolymorphControl() && !forcecontrol && !draconian && !iswere
        && !isvamp) {
        if (rn2(20) > ACURR(A_CON)) {
            You1(shudder_for_moment);
            losehp(rnd(30), "system shock", KILLED_BY_AN);
            exercise(A_CON, FALSE);
            return;
        }
    }
    old_light = emitsLightWithRange(youmonst.data->monsterTypeID);
    mntmp = NON_PM;

    if (monsterpoly && isvamp)
        goto do_vampyr;

    if (controllable_poly || forcecontrol) {
        tryct = 5;
        do {
            mntmp = NON_PM;
            getlin("Become what kind of monster? [type the name]", buf);
            (void) mungspaces(buf);
            if (*buf == '\033') {
                /* user is cancelling controlled poly */
                if (forcecontrol) { /* wizard mode #polyself */
                    pline1(Never_mind);
                    return;
                }
                Strcpy(buf, "*"); /* resort to random */
            }
            if (!strcmp(buf, "*") || !strcmp(buf, "random")) {
                /* explicitly requesting random result */
                tryct = 0; /* will skip thats_enough_tries */
                continue;  /* end do-while(--tryct > 0) loop */
            }
            class = 0;
            mntmp = name_to_mon(buf);
            if (mntmp < LOW_PM) {
            by_class:
                class = name_to_monclass(buf, &mntmp);
                if (class && mntmp == NON_PM)
                    mntmp = mkclass_poly(class);
            }
            if (mntmp < LOW_PM) {
                if (!class)
                    pline("I've never heard of such monsters.");
                else
                    You_cant("polymorph into any of those.");
            } else if (iswere && (were_beastie(mntmp) == lycanthropeType()
                                  || mntmp == counter_were(lycanthropeType())
                                  || (areYouPolymorphed() && mntmp == PM_HUMAN))) {
                goto do_shift;
                /* Note:  humans are illegal as monsters, but an
                 * illegal monster forces newman(), which is what we
                 * want if they specified a human.... */
            } else if (!okToPolymorphInto(mons[mntmp].monsterTypeID)
                       && !(mntmp == PM_HUMAN || isOfYourRace(mons[mntmp].monsterTypeID, urace.selfmask)
                            || mntmp == urole.malenum
                            || mntmp == urole.femalenum)) {

                /* mkclass_poly() can pick a !okToPolymorphInto()
                   candidate; if so, usually try again */
                if (class) {
                    if (rn2(3) || --tryct > 0)
                        goto by_class;
                    /* no retries left; put one back on counter
                       so that end of loop decrement will yield
                       0 and trigger thats_enough_tries message */
                    ++tryct;
                }
                javaString pm_name = monsterTypeName(mons[mntmp].monsterTypeID);
                if (the_unique_pm(&mons[mntmp])) {
                    You_cant("polymorph into %s.", the(pm_name.c_str));
		}
                else if (!typeIsProperName(mons[mntmp].monsterTypeID)) {
                    You_cant("polymorph into %s.", an(pm_name.c_str));
		}
		else {
                    You_cant("polymorph into %s.", pm_name.c_str);
		}
		releaseJavaString(pm_name);
            } else
                break;
        } while (--tryct > 0);
        if (!tryct)
            pline1(thats_enough_tries);
        /* allow skin merging, even when polymorph is controlled */
        if (draconian && (tryct <= 0 || mntmp == armor_to_dragon(uarm->otyp)))
            goto do_merge;
        if (isvamp && (tryct <= 0 || mntmp == PM_WOLF || mntmp == PM_FOG_CLOUD
                       || isBat(mons[mntmp].monsterTypeID)))
            goto do_vampyr;
    } else if (draconian || iswere || isvamp) {
        /* special changes that don't require okToPolymorphInto() */
        if (draconian) {
        do_merge:
            mntmp = armor_to_dragon(uarm->otyp);
            if (!(mvitals[mntmp].mvflags & G_GENOD)) {
                /* allow G_EXTINCT */
                if (Is_dragon_scales(uarm)) {
                    /* dragon scales remain intact as uskin */
                    You("merge with your scaly armor.");
                } else { /* dragon scale mail */
                    /* d.scale mail first reverts to scales */
                    char *p, *dsmail;

                    /* similar to noarmor(invent.c),
                       shorten to "<color> scale mail" */
                    dsmail = strcpy(buf, simpleonames(uarm));
                    if ((p = strstri(dsmail, " dragon ")) != 0)
                        while ((p[1] = p[8]) != '\0')
                            ++p;
                    /* tricky phrasing; dragon scale mail
                       is singular, dragon scales are plural */
                    Your("%s reverts to scales as you merge with them.",
                         dsmail);
                    /* uarm->spe enchantment remains unchanged;
                       re-converting scales to mail poses risk
                       of evaporation due to over enchanting */
                    uarm->otyp += GRAY_DRAGON_SCALES - GRAY_DRAGON_SCALE_MAIL;
                    uarm->dknown = 1;
                    context.botl = 1; /* AC is changing */
                }
                uskin = uarm;
                uarm = (struct obj *) 0;
                /* save/restore hack */
                uskin->owornmask |= I_SPECIAL;
                update_inventory();
            }
        } else if (iswere) {
        do_shift:
            if (areYouPolymorphed() && were_beastie(mntmp) != lycanthropeType())
                mntmp = PM_HUMAN; /* Illegal; force newman() */
            else
                mntmp = lycanthropeType();
        } else if (isvamp) {
        do_vampyr:
            if (mntmp < LOW_PM || (monsterGenerationMask(mons[mntmp].monsterTypeID) & G_UNIQ))
                mntmp = (youmonst.data != &mons[PM_VAMPIRE] && !rn2(10))
                            ? PM_WOLF
                            : !rn2(4) ? PM_FOG_CLOUD : PM_VAMPIRE_BAT;
            if (controllable_poly) {
		javaString monsterName = monsterTypeName(mons[mntmp].monsterTypeID); 
                Sprintf(buf, "Become %s?", an(monsterName.c_str));
		releaseJavaString(monsterName);
                if (yn(buf) != 'y')
                    return;
            }
        }
        /* if polymon fails, "you feel" message has been given
           so don't follow up with another polymon or newman;
           sex_change_ok left disabled here */
        if (mntmp == PM_HUMAN)
            newman(); /* werecritter */
        else
            (void) polymon(mntmp);
        goto made_change; /* maybe not, but this is right anyway */
    }

    if (mntmp < LOW_PM) {
        tryct = 200;
        do {
            /* randomly pick an "ordinary" monster */
            mntmp = rn1(SPECIAL_PM - LOW_PM, LOW_PM);
            if (okToPolymorphInto(mons[mntmp].monsterTypeID) && !is_placeholder(&mons[mntmp]))
                break;
        } while (--tryct > 0);
    }

    /* The below okToPolymorphInto() fails either if everything is genocided, or if
     * we deliberately chose something illegal to force newman().
     */
    sex_change_ok++;
    if (!okToPolymorphInto(mons[mntmp].monsterTypeID) || (!forcecontrol && !rn2(5))
        || isOfYourRace(mons[mntmp].monsterTypeID, urace.selfmask)) {
        newman();
    } else {
        (void) polymon(mntmp);
    }
    sex_change_ok--; /* reset */

made_change:
    new_light = emitsLightWithRange(youmonst.data->monsterTypeID);
    if (old_light != new_light) {
        if (old_light)
            del_light_source(LS_MONSTER, monst_to_any(&youmonst));
        if (new_light == 1)
            ++new_light; /* otherwise it's undetectable */
        if (new_light)
            new_light_source(currentX(), currentY(), new_light, LS_MONSTER,
                             monst_to_any(&youmonst));
    }
}

/* (try to) make a mntmp monster out of the player;
   returns 1 if polymorph successful */
int
polymon(mntmp)
int mntmp;
{
    boolean sticky = sticks(youmonst.data) && u.ustuck && !swallowed(),
            was_blind = !!youCannotSee(), dochange = FALSE;
    int mlvl;

    if (mvitals[mntmp].mvflags & G_GENOD) { /* allow G_EXTINCT */
	javaString monsterName = monsterTypeName(mons[mntmp].monsterTypeID);
        You_feel("rather %s-ish.", monsterName.c_str);
	releaseJavaString(monsterName);
        exercise(A_WIS, TRUE);
        return 0;
    }

    /* KMH, conduct */
    incrementPolyselfCount(1);

    /* exercise used to be at the very end but only Wis was affected
       there since the polymorph was always in effect by then */
    exercise(A_CON, FALSE);
    exercise(A_WIS, TRUE);

    if (!areYouPolymorphed()) {
        /* Human to monster; save human stats */
        int i;
        for (i = 0; i < A_MAX; i++) {
            setYourAttrAsMonster(i, yourCurrentAttr(i));
            setYourAttrMaxAsMonster(i, yourAttrMax(i));
        }
        setInherentlyFemale(flags.female);
    } else {
        /* Monster to monster; restore human stats, to be
         * immediately changed to provide stats for the new monster
         */
        int i;
        for (i = 0; i < A_MAX; i++) {
            setYourCurrentAttr(i, yourAttrAsMonster(i));
            setYourAttrMax(i,     yourAttrMaxAsMonster(i));
        }
        flags.female = inherentlyFemale();
    }

    /* if stuck mimicking gold, stop immediately */
    if (multi < 0 && youmonst.m_ap_type == M_AP_OBJECT
        && monsterClass(youmonst.data->monsterTypeID) != S_MIMIC)
        unmul("");
    /* if becoming a non-mimic, stop mimicking anything */
    if (monsterClass(mons[mntmp].monsterTypeID) != S_MIMIC) {
        /* as in polyman() */
        youmonst.m_ap_type = M_AP_NOTHING;
    }
    if (isMale(mons[mntmp].monsterTypeID)) {
        if (flags.female)
            dochange = TRUE;
    } else if (isFemale(mons[mntmp].monsterTypeID)) {
        if (!flags.female)
            dochange = TRUE;
    } else if (!isNeuter(mons[mntmp].monsterTypeID) && mntmp != lycanthropeType()) {
        if (sex_change_ok && !rn2(10))
            dochange = TRUE;
    }

    javaString monsterName = monsterTypeName(mons[mntmp].monsterTypeID);
    if (dochange) {
        flags.female = !flags.female;
        You("%s %s%s!",
            (currentMonsterNumber() != mntmp) ? "turn into a" : "feel like a new",
            (isMale(mons[mntmp].monsterTypeID) || isFemale(mons[mntmp].monsterTypeID))
                ? ""
                : flags.female ? "female " : "male ",
            monsterName.c_str);
    } else {
        if (currentMonsterNumber() != mntmp)
            You("turn into %s!", an(monsterName.c_str));
        else
            You_feel("like a new %s!", monsterName.c_str);
    }
    releaseJavaString(monsterName);

    if (youAreTurningToStone() && poly_when_stoned(&mons[mntmp])) {
        /* poly_when_stoned already checked stone golem genocide */
        mntmp = PM_STONE_GOLEM;
        make_stoned(0L, "You turn to stone!", 0, (char *) 0);
    }

    setTimeRemainingAsMonster(rn1(500, 500));
    setCurrentMonsterNumber(mntmp);
    set_uasmon();

    /* New stats for monster, to last only as long as polymorphed.
     * Currently only strength gets changed.
     */
    if (isStrongMonster(mons[mntmp].monsterTypeID)) {
        setYourCurrentAttr(A_STR, STR18(100));
        setYourAttrMax(A_STR, STR18(100));
    }

    if (youResistStoning() && youAreTurningToStone()) { /* parnes@eniac.seas.upenn.edu */
        make_stoned(0L, "You no longer seem to be petrifying.", 0,
                    (char *) 0);
    }
    if (youResistSickness() && youAreSick()) {
        make_sick(0L, (char *) 0, FALSE, SICK_ALL);
        You("no longer feel sick.");
    }
    if (youAreTurningToSlime()) {
        if (flaming(youmonst.data)) {
            make_slimed(0L, "The slime burns away!");
        } else if (mntmp == PM_GREEN_SLIME) {
            /* do it silently */
            make_slimed(0L, (char *) 0);
        }
    }
    check_strangling(FALSE); /* maybe stop strangling */
    if (hasNoHands(youmonst.data->monsterTypeID))
        setYourIntrinsic(SLIPPERY_FINGERS, 0);

    /*
    mlvl = adj_lev(&mons[mntmp]);
     * We can't do the above, since there's no such thing as an
     * "experience level of you as a monster" for a polymorphed character.
     */
    mlvl = monsterLevel(mons[mntmp].monsterTypeID);
    if (monsterClass(youmonst.data->monsterTypeID) == S_DRAGON && mntmp >= PM_GRAY_DRAGON) {
        setMaximumHitPointsAsMonster(areYouInEndgame() ? (8 * mlvl) : (4 * mlvl + d(mlvl, 4)));
    } else if (isGolem(youmonst.data->monsterTypeID)) {
        setMaximumHitPointsAsMonster(golemhp(mntmp));
    } else {
        if (!mlvl)
            setMaximumHitPointsAsMonster(rnd(4));
        else
            setMaximumHitPointsAsMonster(d(mlvl, 8));
        if (is_home_elemental(&mons[mntmp]))
            multiplyMaximumHitPointsAsMonster(3);
    }
    setCurrentHitPointsAsMonster(maximumHitPointsAsMonster());

    if (currentExperienceLevel() < mlvl) {
        /* Low level characters can't become high level monsters for long */
#ifdef DUMB
        /* DRS/NS 2.2.6 messes up -- Peter Kendell */
        int mtd = timeRemainingAsMonster();
        int ulv = currentExperienceLevel();


        setTimeRemainingAsMonster(mtd * ulv / mlvl);
#else
        multiplyTimeRemainingAsMonster(currentExperienceLevel() / mlvl);
#endif
    }

    if (uskin && mntmp != armor_to_dragon(uskin->otyp))
        skinback(FALSE);
    break_armor();
    drop_weapon(1);
    (void) hideunder(&youmonst);

    if (currentlyTrapped() && currentTrapType() == TT_PIT) {
        setCurrentTrapTimeout(rn1(6, 2)); /* time to escape resets */
    }
    if (was_blind && youCanSee()) { /* previous form was eyeless */
        setYourIntrinsic(BLINDED, 1L);
        make_blinded(0L, TRUE); /* remove blindness */
    }
    newsym(currentX(), currentY()); /* Change symbol */

    if (!sticky && !swallowed() && u.ustuck && sticks(youmonst.data))
        u.ustuck = 0;
    else if (sticky && !sticks(youmonst.data))
        uunstick();
    if (u.usteed) {
        if (touchPetrifies(u.usteed->data->monsterTypeID) && !youResistStoning() && rnl(3)) {
            char buf[BUFSZ];

            pline("%s touch %s.", no_longer_petrify_resistant,
                  mon_nam(u.usteed));
	    javaString steedName = monsterTypeName(u.usteed->data->monsterTypeID);
            Sprintf(buf, "riding %s", an(steedName.c_str));
	    releaseJavaString(steedName);
            instapetrify(buf);
        }
        if (!can_ride(u.usteed))
            dismount_steed(DISMOUNT_POLY);
    }

    if (flags.verbose) {
        static const char use_thec[] = "Use the command #%s to %s.";
        static const char monsterc[] = "monster";

        int upmid = youmonst.data->monsterTypeID;

        if (can_breathe(youmonst.data))
            pline(use_thec, monsterc, "use your breath weapon");
        if (attacktype(youmonst.data, AT_SPIT))
            pline(use_thec, monsterc, "spit venom");
        if (monsterClass(upmid) == S_NYMPH)
            pline(use_thec, monsterc, "remove an iron ball");
        if (attacktype(youmonst.data, AT_GAZE))
            pline(use_thec, monsterc, "gaze at monsters");
        if (isHider(upmid))
            pline(use_thec, monsterc, "hide");
        if (isWere(youmonst.data->monsterTypeID))
            pline(use_thec, monsterc, "summon help");
        if (webmaker(youmonst.data))
            pline(use_thec, monsterc, "spin a web");
        if (currentMonsterNumber() == PM_GREMLIN)
            pline(use_thec, monsterc, "multiply in a fountain");
        if (isUnicorn(upmid))
            pline(use_thec, monsterc, "use your horn");
        if (isMindFlayer(upmid))
            pline(use_thec, monsterc, "emit a mental blast");
        if (monsterSound(youmonst.data->monsterTypeID) == MS_SHRIEK) /* worthless, actually */
            pline(use_thec, monsterc, "shriek");
        if (isVampire(upmid))
            pline(use_thec, monsterc, "change shape");

        if (laysEggs(youmonst.data->monsterTypeID) && flags.female)
            pline(use_thec, "sit", "lay an egg");
    }

    /* you now know what an egg of your type looks like */
    if (laysEggs(youmonst.data->monsterTypeID)) {
        learn_egg_type(currentMonsterNumber());
        /* make queen bees recognize killer bee eggs */
        learn_egg_type(egg_type_from_parent(currentMonsterNumber(), TRUE));
    }
    find_ac();
    if ((!youAreLevitating() && !u.ustuck && !youAreFlying() && is_pool_or_lava(currentX(), currentY()))
        || (underwater() && !youCanSwim()))
        spoteffects(TRUE);
    if (youCanPassThroughWalls() && currentlyTrapped()
        && (currentTrapType() == TT_INFLOOR || currentTrapType() == TT_BURIEDBALL)) {
        setCurrentTrapTimeout(0);
        if (currentTrapType() == TT_INFLOOR)
            pline_The("rock seems to no longer trap you.");
        else {
            pline_The("buried ball is no longer bound to you.");
            buried_ball_to_freedom();
        }
    } else if (likesLava(youmonst.data->monsterTypeID) && currentlyTrapped()
               && currentTrapType() == TT_LAVA) {
        setCurrentTrapTimeout(0);
        pline_The("lava now feels soothing.");
    }
    if (isAmorphous(youmonst.data->monsterTypeID) || isWhirly(youmonst.data->monsterTypeID)
        || isUnsolid(youmonst.data->monsterTypeID)) {
        if (youAreBeingPunished()) {
            You("slip out of the iron chain.");
            unpunish();
        } else if (currentlyTrapped() && currentTrapType() == TT_BURIEDBALL) {
            You("slip free of the buried ball and chain.");
            buried_ball_to_freedom();
        }
    }
    if (currentlyTrapped() && (currentTrapType() == TT_WEB || currentTrapType() == TT_BEARTRAP)
        && (isAmorphous(youmonst.data->monsterTypeID) || isWhirly(youmonst.data->monsterTypeID)
            || isUnsolid(youmonst.data->monsterTypeID) || (monsterSize(youmonst.data->monsterTypeID) <= MZ_SMALL
                                          && currentTrapType() == TT_BEARTRAP))) {
        You("are no longer stuck in the %s.",
            currentTrapType() == TT_WEB ? "web" : "bear trap");
        /* probably should burn webs too if PM_FIRE_ELEMENTAL */
        setCurrentTrapTimeout(0);
    }
    if (webmaker(youmonst.data) && currentlyTrapped() && currentTrapType() == TT_WEB) {
        You("orient yourself on the web.");
        setCurrentTrapTimeout(0);
    }
    check_strangling(TRUE); /* maybe start strangling */
    (void) polysense(youmonst.data);

    context.botl = 1;
    vision_full_recalc = 1;
    see_monsters();
    (void) encumber_msg();

    retouch_equipment(2);
    /* this might trigger a recursive call to polymon() [stone golem
       wielding cockatrice corpse and hit by stone-to-flesh, becomes
       flesh golem above, now gets transformed back into stone golem] */
    if (!uarmg)
        selftouch(no_longer_petrify_resistant);
    return 1;
}

STATIC_OVL void
break_armor()
{
    register struct obj *otmp;

    if (breakarm(youmonst.data)) {
        if ((otmp = uarm) != 0) {
            if (donning(otmp))
                cancel_don();
            You("break out of your armor!");
            exercise(A_STR, FALSE);
            (void) Armor_gone();
            useup(otmp);
        }
        if ((otmp = uarmc) != 0) {
            if (otmp->oartifact) {
                Your("%s falls off!", cloak_simple_name(otmp));
                (void) Cloak_off();
                dropx(otmp);
            } else {
                Your("%s tears apart!", cloak_simple_name(otmp));
                (void) Cloak_off();
                useup(otmp);
            }
        }
        if (uarmu) {
            Your("shirt rips to shreds!");
            useup(uarmu);
        }
    } else if (sliparm(youmonst.data)) {
        if (((otmp = uarm) != 0) && (racial_exception(&youmonst, otmp) < 1)) {
            if (donning(otmp))
                cancel_don();
            Your("armor falls around you!");
            (void) Armor_gone();
            dropx(otmp);
        }
        if ((otmp = uarmc) != 0) {
            if (isWhirly(youmonst.data->monsterTypeID))
                Your("%s falls, unsupported!", cloak_simple_name(otmp));
            else
                You("shrink out of your %s!", cloak_simple_name(otmp));
            (void) Cloak_off();
            dropx(otmp);
        }
        if ((otmp = uarmu) != 0) {
            if (isWhirly(youmonst.data->monsterTypeID))
                You("seep right through your shirt!");
            else
                You("become much too small for your shirt!");
            setworn((struct obj *) 0, otmp->owornmask & W_ARMU);
            dropx(otmp);
        }
    }
    if (has_horns(youmonst.data)) {
        if ((otmp = uarmh) != 0) {
            if (is_flimsy(otmp) && !donning(otmp)) {
                char hornbuf[BUFSZ];

                /* Future possibilities: This could damage/destroy helmet */
                Sprintf(hornbuf, "horn%s", plur(num_horns(youmonst.data)));
                Your("%s %s through %s.", hornbuf, vtense(hornbuf, "pierce"),
                     yname(otmp));
            } else {
                if (donning(otmp))
                    cancel_don();
                Your("%s falls to the %s!", helm_simple_name(otmp),
                     surface(currentX(), currentY()));
                (void) Helmet_off();
                dropx(otmp);
            }
        }
    }
    if (cannotWieldThings(youmonst.data->monsterTypeID)) {
        if ((otmp = uarmg) != 0) {
            if (donning(otmp))
                cancel_don();
            /* Drop weapon along with gloves */
            You("drop your gloves%s!", uwep ? " and weapon" : "");
            drop_weapon(0);
            (void) Gloves_off();
            dropx(otmp);
        }
        if ((otmp = uarms) != 0) {
            You("can no longer hold your shield!");
            (void) Shield_off();
            dropx(otmp);
        }
        if ((otmp = uarmh) != 0) {
            if (donning(otmp))
                cancel_don();
            Your("%s falls to the %s!", helm_simple_name(otmp),
                 surface(currentX(), currentY()));
            (void) Helmet_off();
            dropx(otmp);
        }
    }
    if (cannotWieldThings(youmonst.data->monsterTypeID)
        || isSlithy(youmonst.data->monsterTypeID) || monsterClass(youmonst.data->monsterTypeID) == S_CENTAUR) {
        if ((otmp = uarmf) != 0) {
            if (donning(otmp))
                cancel_don();
            if (isWhirly(youmonst.data->monsterTypeID))
                Your("boots fall away!");
            else
                Your("boots %s off your feet!",
                     isVerySmallMonster(youmonst.data->monsterTypeID) ? "slide" : "are pushed");
            (void) Boots_off();
            dropx(otmp);
        }
    }
}

STATIC_OVL void
drop_weapon(alone)
int alone;
{
    struct obj *otmp;
    const char *what, *which, *whichtoo;
    boolean candropwep, candropswapwep;

    if (uwep) {
        /* !alone check below is currently superfluous but in the
         * future it might not be so if there are monsters which cannot
         * wear gloves but can wield weapons
         */
        if (!alone || cannotWieldThings(youmonst.data->monsterTypeID)) {
            candropwep = canletgo(uwep, "");
            candropswapwep = !usingTwoWeapons() || canletgo(uswapwep, "");
            if (alone) {
                what = (candropwep && candropswapwep) ? "drop" : "release";
                which = is_sword(uwep) ? "sword" : weapon_descr(uwep);
                if (usingTwoWeapons()) {
                    whichtoo =
                        is_sword(uswapwep) ? "sword" : weapon_descr(uswapwep);
                    if (strcmp(which, whichtoo))
                        which = "weapon";
                }
                if (uwep->quan != 1L || usingTwoWeapons())
                    which = makeplural(which);

                You("find you must %s %s %s!", what,
                    the_your[!!strncmp(which, "corpse", 6)], which);
            }
            if (usingTwoWeapons()) {
                otmp = uswapwep;
                uswapwepgone();
                if (candropswapwep)
                    dropx(otmp);
            }
            otmp = uwep;
            uwepgone();
            if (candropwep)
                dropx(otmp);
            update_inventory();
        } else if (cannotUseTwoWeapons(youmonst.data)) {
            untwoweapon();
        }
    }
}

void
rehumanize()
{
    /* You can't revert back while unchanging */
    if (youAreUnchanging() && (currentHitPointsAsMonster() < 1)) {
        killer.format = NO_KILLER_PREFIX;
        Strcpy(killer.name, "killed while stuck in creature form");
        done(DIED);
    }

    if (emitsLightWithRange(youmonst.data->monsterTypeID))
        del_light_source(LS_MONSTER, monst_to_any(&youmonst));
    polyman("return to %s form!", urace.adj);

    if (currentHitPoints() < 1) {
        /* can only happen if some bit of code reduces u.uhp
           instead of currentHitPointsAsMonster() while poly'd */
        Your("old form was not healthy enough to survive.");
        Sprintf(killer.name, "reverting to unhealthy %s form", urace.adj);
        killer.format = KILLED_BY;
        done(DIED);
    }
    nomul(0);

    context.botl = 1;
    vision_full_recalc = 1;
    (void) encumber_msg();

    retouch_equipment(2);
    if (!uarmg)
        selftouch(no_longer_petrify_resistant);
}

int
dobreathe()
{
    if (youAreBeingStrangled()) {
        You_cant("breathe.  Sorry.");
        return 0;
    }
    if (currentMagicalEnergy() < 15) {
        You("don't have enough energy to breathe!");
        return 0;
    }
    decreaseCurrentMagicalEnergy(15);
    context.botl = 1;

    if (!getdir((char *) 0))
        return 0;

    struct Attack mattk = monsterAttackWithDamageType(youmonst.data, AT_BREA, AD_ANY);

    if (!validAttack(mattk)) {
        impossible("bad breath attack?"); /* mouthwash needed... */
    }
    else if (!directionX() && !directionY() && !directionZ()) {
        ubreatheu(mattk);
    }
    else {
        buzz((20 + mattk.damageType - 1), mattk.dice, currentX(), currentY(),
             directionX(), directionY());
    }
    return 1;
}

int
dospit()
{
    struct obj *otmp;

    if (!getdir((char *) 0))
        return 0;

    struct Attack mattk = monsterAttackWithDamageType(youmonst.data, AT_SPIT, AD_ANY);

    if (!validAttack(mattk)) {
        impossible("bad spit attack?");
    } else {
        switch (mattk.damageType) {
        case AD_BLND:
        case AD_DRST:
            otmp = mksobj(BLINDING_VENOM, TRUE, FALSE);
            break;
        default:
            impossible("bad attack type in dospit");
        /* fall through */
        case AD_ACID:
            otmp = mksobj(ACID_VENOM, TRUE, FALSE);
            break;
        }
        otmp->spe = 1; /* to indicate it's yours */
        throwit(otmp, 0L, FALSE);
    }
    return 1;
}

int
doremove()
{
    if (!youAreBeingPunished()) {
        if (currentlyTrapped() && currentTrapType() == TT_BURIEDBALL) {
            pline_The("ball and chain are buried firmly in the %s.",
                      surface(currentX(), currentY()));
            return 0;
        }
        You("are not chained to anything!");
        return 0;
    }
    unpunish();
    return 1;
}

int
dospinweb()
{
    register struct trap *ttmp = t_at(currentX(), currentY());

    if (youAreLevitating() || areYouOnAirLevel() || underwater()
        || areYouOnWaterLevel()) {
        You("must be on the ground to spin a web.");
        return 0;
    }
    if (swallowed()) {
        You("release web fluid inside %s.", mon_nam(u.ustuck));
        if (isAnimal(u.ustuck->data->monsterTypeID)) {
            expels(u.ustuck, u.ustuck->data, TRUE);
            return 0;
        }
	int ustuckpmid = u.ustuck->data->monsterTypeID;
        if (isWhirly(ustuckpmid)) {
            int i;
	    int nAttacks = monsterAttacks(ustuckpmid);

            for (i = 0; i < nAttacks; i++) {
                if (monsterAttack(ustuckpmid, i).type == AT_ENGL) {
                    break;
		}
	    }
            if (i == nAttacks) {
                impossible("Swallower has no engulfing attack?");
	    } else {
                char sweep[30];

                sweep[0] = '\0';
                switch (monsterAttack(ustuckpmid, i).damageType) {
                case AD_FIRE:
                    Strcpy(sweep, "ignites and ");
                    break;
                case AD_ELEC:
                    Strcpy(sweep, "fries and ");
                    break;
                case AD_COLD:
                    Strcpy(sweep, "freezes, shatters and ");
                    break;
                }
                pline_The("web %sis swept away!", sweep);
            }
            return 0;
        } /* default: a nasty jelly-like creature */
        pline_The("web dissolves into %s.", mon_nam(u.ustuck));
        return 0;
    }
    if (currentlyTrapped()) {
        You("cannot spin webs while stuck in a trap.");
        return 0;
    }
    exercise(A_DEX, TRUE);
    if (ttmp) {
        switch (ttmp->ttyp) {
        case PIT:
        case SPIKED_PIT:
            You("spin a web, covering up the pit.");
            deltrap(ttmp);
            bury_objs(currentX(), currentY());
            newsym(currentX(), currentY());
            return 1;
        case SQKY_BOARD:
            pline_The("squeaky board is muffled.");
            deltrap(ttmp);
            newsym(currentX(), currentY());
            return 1;
        case TELEP_TRAP:
        case LEVEL_TELEP:
        case MAGIC_PORTAL:
        case VIBRATING_SQUARE:
            Your("webbing vanishes!");
            return 0;
        case WEB:
            You("make the web thicker.");
            return 1;
        case HOLE:
        case TRAPDOOR:
            You("web over the %s.",
                (ttmp->ttyp == TRAPDOOR) ? "trap door" : "hole");
            deltrap(ttmp);
            newsym(currentX(), currentY());
            return 1;
        case ROLLING_BOULDER_TRAP:
            You("spin a web, jamming the trigger.");
            deltrap(ttmp);
            newsym(currentX(), currentY());
            return 1;
        case ARROW_TRAP:
        case DART_TRAP:
        case BEAR_TRAP:
        case ROCKTRAP:
        case FIRE_TRAP:
        case LANDMINE:
        case SLP_GAS_TRAP:
        case RUST_TRAP:
        case MAGIC_TRAP:
        case ANTI_MAGIC:
        case POLY_TRAP:
            You("have triggered a trap!");
            dotrap(ttmp, 0);
            return 1;
        default:
            impossible("Webbing over trap type %d?", ttmp->ttyp);
            return 0;
        }
    } else if (On_stairs(currentX(), currentY())) {
        /* cop out: don't let them hide the stairs */
        Your("web fails to impede access to the %s.",
             (levl[currentX()][currentY()].typ == STAIRS) ? "stairs" : "ladder");
        return 1;
    }
    ttmp = maketrap(currentX(), currentY(), WEB);
    if (ttmp) {
        ttmp->madeby_u = 1;
        feeltrap(ttmp);
    }
    return 1;
}

int
dosummon()
{
    int placeholder;
    if (currentMagicalEnergy() < 10) {
        You("lack the energy to send forth a call for help!");
        return 0;
    }
    decreaseCurrentMagicalEnergy(10);
    context.botl = 1;

    You("call upon your brethren for help!");
    exercise(A_WIS, TRUE);
    if (!were_summon(youmonst.data, TRUE, &placeholder, (char *) 0))
        pline("But none arrive.");
    return 1;
}

int
dogaze()
{
    register struct monst *mtmp;
    int looked = 0;
    char qbuf[QBUFSZ];
    int i;
    uchar damageType = 0;

    int upmid = youmonst.data->monsterTypeID;
    int nAttacks = monsterAttacks(upmid);

    for (i = 0; i < nAttacks; i++) {
	struct Attack uattk = monsterAttack(upmid, i);
        if (uattk.type == AT_GAZE) {
            damageType = uattk.damageType;
            break;
        }
    }
    if (damageType != AD_CONF && damageType != AD_FIRE) {
        impossible("gaze attack %d?", damageType);
        return 0;
    }

    if (youCannotSee()) {
        You_cant("see anything to gaze at.");
        return 0;
    } else if (youAreHallucinating()) {
        You_cant("gaze at anything you can see.");
        return 0;
    }
    if (currentMagicalEnergy() < 15) {
        You("lack the energy to use your special gaze!");
        return 0;
    }
    decreaseCurrentMagicalEnergy(15);
    context.botl = 1;

    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
        if (DEADMONSTER(mtmp))
            continue;
        if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my)) {
            looked++;
            if (youAreInvisibleToOthers() && !perceivesTheInvisible(mtmp->data->monsterTypeID)) {
                pline("%s seems not to notice your gaze.", Monnam(mtmp));
            } else if (mtmp->minvis && !youCanSeeInvisible()) {
                You_cant("see where to gaze at %s.", Monnam(mtmp));
            } else if (mtmp->m_ap_type == M_AP_FURNITURE
                       || mtmp->m_ap_type == M_AP_OBJECT) {
                looked--;
                continue;
            } else if (flags.safe_dog && mtmp->mtame && !youAreConfused()) {
                You("avoid gazing at %s.", y_monnam(mtmp));
            } else {
                if (flags.confirm && mtmp->mpeaceful && !youAreConfused()) {
                    Sprintf(qbuf, "Really %s %s?",
                            (damageType == AD_CONF) ? "confuse" : "attack",
                            mon_nam(mtmp));
                    if (yn(qbuf) != 'y')
                        continue;
                    setmangry(mtmp);
                }
                if (!mtmp->mcanmove || mtmp->mstun || mtmp->msleeping
                    || !mtmp->mcansee || !hasEyes(mtmp->data->monsterTypeID)) {
                    looked--;
                    continue;
                }
                /* No reflection check for consistency with when a monster
                 * gazes at *you*--only medusa gaze gets reflected then.
                 */
                if (damageType == AD_CONF) {
                    if (!mtmp->mconf)
                        Your("gaze confuses %s!", mon_nam(mtmp));
                    else
                        pline("%s is getting more and more confused.",
                              Monnam(mtmp));
                    mtmp->mconf = 1;
                } else if (damageType == AD_FIRE) {
                    int dmg = d(2, 6), lev = (int) currentExperienceLevel();

                    You("attack %s with a fiery gaze!", mon_nam(mtmp));
                    if (resists_fire(mtmp)) {
                        pline_The("fire doesn't burn %s!", mon_nam(mtmp));
                        dmg = 0;
                    }
                    if (lev > rn2(20))
                        (void) destroy_mitem(mtmp, SCROLL_CLASS, AD_FIRE);
                    if (lev > rn2(20))
                        (void) destroy_mitem(mtmp, POTION_CLASS, AD_FIRE);
                    if (lev > rn2(25))
                        (void) destroy_mitem(mtmp, SPBOOK_CLASS, AD_FIRE);
                    if (dmg)
                        mtmp->mhp -= dmg;
                    if (mtmp->mhp <= 0)
                        killed(mtmp);
                }
                /* For consistency with passive() in uhitm.c, this only
                 * affects you if the monster is still alive.
                 */
                if (DEADMONSTER(mtmp))
                    continue;

                if (mtmp->data == &mons[PM_FLOATING_EYE] && !mtmp->mcan) {
                    if (!youHaveFreeAction()) {
			int pmid = mtmp->data->monsterTypeID;
                        You("are frozen by %s gaze!",
                            s_suffix(mon_nam(mtmp)));
                        nomul((currentExperienceLevel() > 6 || rn2(4))
                                  ? -d((int) mtmp->m_lev + 1,
                                       (int) monsterAttack(pmid, 0).diceSides)
                                  : -200);
                        multi_reason = "frozen by a monster's gaze";
                        nomovemsg = 0;
                        return 1;
                    } else
                        You("stiffen momentarily under %s gaze.",
                            s_suffix(mon_nam(mtmp)));
                }
                /* Technically this one shouldn't affect you at all because
                 * the Medusa gaze is an active monster attack that only
                 * works on the monster's turn, but for it to *not* have an
                 * effect would be too weird.
                 */
                if (mtmp->data == &mons[PM_MEDUSA] && !mtmp->mcan) {
                    pline("Gazing at the awake %s is not a very good idea.",
                          l_monnam(mtmp));
                    /* as if gazing at a sleeping anything is fruitful... */
                    You("turn to stone...");
                    killer.format = KILLED_BY;
                    Strcpy(killer.name, "deliberately meeting Medusa's gaze");
                    done(STONING);
                }
            }
        }
    }
    if (!looked)
        You("gaze at no place in particular.");
    return 1;
}

int
dohide()
{
    int umc = monsterClass(youmonst.data->monsterTypeID);
    boolean ismimic = (umc == S_MIMIC);
    boolean on_ceiling = isClinger(youmonst.data->monsterTypeID) || youAreFlying();

    /* can't hide while being held (or holding) or while trapped
       (except for floor hiders [trapper or mimic] in pits) */
    if (u.ustuck || (currentlyTrapped() && (currentTrapType() != TT_PIT || on_ceiling))) {
        You_cant("hide while you're %s.",
                 !u.ustuck ? "trapped" : !sticks(youmonst.data)
                                             ? "being held"
                                             : isHumanoid(u.ustuck->data->monsterTypeID)
                                                   ? "holding someone"
                                                   : "holding that creature");
        if (lurking()
            || (ismimic && youmonst.m_ap_type != M_AP_NOTHING)) {
            setLurking(FALSE);
            youmonst.m_ap_type = M_AP_NOTHING;
            newsym(currentX(), currentY());
        }
        return 0;
    }
    /* note: the eel and hides_under cases are hypothetical;
       such critters aren't offered the option of hiding via #monster */
    if (umc == S_EEL && !is_pool(currentX(), currentY())) {
        if (IS_FOUNTAIN(levl[currentX()][currentY()].typ))
            The("fountain is not deep enough to hide in.");
        else
            There("is no water to hide in here.");
        setLurking(FALSE);
        return 0;
    }
    if (hidesUnderStuff(youmonst.data->monsterTypeID) && !level.objects[currentX()][currentY()]) {
        There("is nothing to hide under here.");
        setLurking(FALSE);
        return 0;
    }
    /* Planes of Air and Water */
    if (on_ceiling && !currentLevelHasCeiling()) {
        There("is nowhere to hide above you.");
        setLurking(FALSE);
        return 0;
    }
    if ((isHider(youmonst.data->monsterTypeID) && !youAreFlying()) /* floor hider */
        && (areYouOnAirLevel() || areYouOnWaterLevel())) {
        There("is nowhere to hide beneath you.");
        setLurking(FALSE);
        return 0;
    }
    /* TODO? inhibit floor hiding at furniture locations, or
     * else make youhiding() give smarter messages at such spots.
     */

    if (lurking() || (ismimic && youmonst.m_ap_type != M_AP_NOTHING)) {
        youhiding(FALSE, 1); /* "you are already hiding" */
        return 0;
    }

    if (ismimic) {
        /* should bring up a dialog "what would you like to imitate?" */
        youmonst.m_ap_type = M_AP_OBJECT;
        youmonst.mappearance = STRANGE_OBJECT;
    } else
        setLurking(TRUE);
    newsym(currentX(), currentY());
    youhiding(FALSE, 0); /* "you are now hiding" */
    return 1;
}

int
dopoly()
{
    struct permonst *savedat = youmonst.data;

    if (isVampire(youmonst.data->monsterTypeID)) {
        polyself(2);
        if (savedat != youmonst.data) {
            javaString youMonsterName = monsterTypeName(youmonst.data->monsterTypeID);
            You("transform into %s.", an(youMonsterName.c_str));
            releaseJavaString(youMonsterName);
            newsym(currentX(), currentY());
        }
    }
    return 1;
}

int
domindblast()
{
    struct monst *mtmp, *nmon;

    if (currentMagicalEnergy() < 10) {
        You("concentrate but lack the energy to maintain doing so.");
        return 0;
    }
    decreaseCurrentMagicalEnergy(10);
    context.botl = 1;

    You("concentrate.");
    pline("A wave of psychic energy pours out.");
    for (mtmp = fmon; mtmp; mtmp = nmon) {
        int u_sen;

        nmon = mtmp->nmon;
        if (DEADMONSTER(mtmp))
            continue;
        if (distanceSquaredToYou(mtmp->mx, mtmp->my) > BOLT_LIM * BOLT_LIM)
            continue;
        if (mtmp->mpeaceful)
            continue;
        u_sen = telepathic(mtmp->data) && !mtmp->mcansee;
        if (u_sen || (telepathic(mtmp->data) && rn2(2)) || !rn2(10)) {
            You("lock in on %s %s.", s_suffix(mon_nam(mtmp)),
                u_sen ? "telepathy"
                      : telepathic(mtmp->data) ? "latent telepathy" : "mind");
            mtmp->mhp -= rnd(15);
            if (mtmp->mhp <= 0)
                killed(mtmp);
        }
    }
    return 1;
}

STATIC_OVL void
uunstick()
{
    pline("%s is no longer in your clutches.", Monnam(u.ustuck));
    u.ustuck = 0;
}

void
skinback(silently)
boolean silently;
{
    if (uskin) {
        if (!silently)
            Your("skin returns to its original form.");
        uarm = uskin;
        uskin = (struct obj *) 0;
        /* undo save/restore hack */
        uarm->owornmask &= ~I_SPECIAL;
    }
}

const char *
mbodypart(mon, part)
struct monst *mon;
int part;
{
    static NEARDATA const char
        *humanoid_parts[] = { "arm",       "eye",  "face",         "finger",
                              "fingertip", "foot", "hand",         "handed",
                              "head",      "leg",  "light headed", "neck",
                              "spine",     "toe",  "hair",         "blood",
                              "lung",      "nose", "stomach" },
        *jelly_parts[] = { "pseudopod", "dark spot", "front",
                           "pseudopod extension", "pseudopod extremity",
                           "pseudopod root", "grasp", "grasped",
                           "cerebral area", "lower pseudopod", "viscous",
                           "middle", "surface", "pseudopod extremity",
                           "ripples", "juices", "surface", "sensor",
                           "stomach" },
        *animal_parts[] = { "forelimb",  "eye",           "face",
                            "foreclaw",  "claw tip",      "rear claw",
                            "foreclaw",  "clawed",        "head",
                            "rear limb", "light headed",  "neck",
                            "spine",     "rear claw tip", "fur",
                            "blood",     "lung",          "nose",
                            "stomach" },
        *bird_parts[] = { "wing",     "eye",  "face",         "wing",
                          "wing tip", "foot", "wing",         "winged",
                          "head",     "leg",  "light headed", "neck",
                          "spine",    "toe",  "feathers",     "blood",
                          "lung",     "bill", "stomach" },
        *horse_parts[] = { "foreleg",  "eye",           "face",
                           "forehoof", "hoof tip",      "rear hoof",
                           "forehoof", "hooved",        "head",
                           "rear leg", "light headed",  "neck",
                           "backbone", "rear hoof tip", "mane",
                           "blood",    "lung",          "nose",
                           "stomach" },
        *sphere_parts[] = { "appendage", "optic nerve", "body", "tentacle",
                            "tentacle tip", "lower appendage", "tentacle",
                            "tentacled", "body", "lower tentacle",
                            "rotational", "equator", "body",
                            "lower tentacle tip", "cilia", "life force",
                            "retina", "olfactory nerve", "interior" },
        *fungus_parts[] = { "mycelium", "visual area", "front",
                            "hypha",    "hypha",       "root",
                            "strand",   "stranded",    "cap area",
                            "rhizome",  "sporulated",  "stalk",
                            "root",     "rhizome tip", "spores",
                            "juices",   "gill",        "gill",
                            "interior" },
        *vortex_parts[] = { "region",        "eye",           "front",
                            "minor current", "minor current", "lower current",
                            "swirl",         "swirled",       "central core",
                            "lower current", "addled",        "center",
                            "currents",      "edge",          "currents",
                            "life force",    "center",        "leading edge",
                            "interior" },
        *snake_parts[] = { "vestigial limb", "eye", "face", "large scale",
                           "large scale tip", "rear region", "scale gap",
                           "scale gapped", "head", "rear region",
                           "light headed", "neck", "length", "rear scale",
                           "scales", "blood", "lung", "forked tongue",
                           "stomach" },
        *worm_parts[] = { "anterior segment", "light sensitive cell",
                          "clitellum", "setae", "setae", "posterior segment",
                          "segment", "segmented", "anterior segment",
                          "posterior", "over stretched", "clitellum",
                          "length", "posterior setae", "setae", "blood",
                          "skin", "prostomium", "stomach" },
        *fish_parts[] = { "fin", "eye", "premaxillary", "pelvic axillary",
                          "pelvic fin", "anal fin", "pectoral fin", "finned",
                          "head", "peduncle", "played out", "gills",
                          "dorsal fin", "caudal fin", "scales", "blood",
                          "gill", "nostril", "stomach" };
    /* claw attacks are overloaded in mons[]; most humanoids with
       such attacks should still reference hands rather than claws */
    static const char not_claws[] = {
        S_HUMAN,     S_MUMMY,   S_ZOMBIE, S_ANGEL, S_NYMPH, S_LEPRECHAUN,
        S_QUANTMECH, S_VAMPIRE, S_ORC,    S_GIANT, /* quest nemeses */
        '\0' /* string terminator; assert( S_xxx != 0 ); */
    };
    struct permonst *mptr = mon->data;

    /* some special cases */
    int mc = monsterClass(mptr->monsterTypeID);
    if (mc == S_DOG || mc == S_FELINE
        || mc == S_RODENT || mptr == &mons[PM_OWLBEAR]) {
        switch (part) {
        case HAND:
            return "paw";
        case HANDED:
            return "pawed";
        case FOOT:
            return "rear paw";
        case ARM:
        case LEG:
            return horse_parts[part]; /* "foreleg", "rear leg" */
        default:
            break; /* for other parts, use animal_parts[] below */
        }
    } else if (mc == S_YETI) { /* excl. owlbear due to 'if' above */
        /* opposable thumbs, hence "hands", "arms", "legs", &c */
        return humanoid_parts[part]; /* yeti/sasquatch, monkey/ape */
    }
    if ((part == HAND || part == HANDED)
        && (isHumanoid(mptr->monsterTypeID) && attacktype(mptr, AT_CLAW)
            && !index(not_claws, mc) && mptr != &mons[PM_STONE_GOLEM]
            && mptr != &mons[PM_INCUBUS] && mptr != &mons[PM_SUCCUBUS]))
        return (part == HAND) ? "claw" : "clawed";
    if ((mptr == &mons[PM_MUMAK] || mptr == &mons[PM_MASTODON])
        && part == NOSE)
        return "trunk";
    if (mptr == &mons[PM_SHARK] && part == HAIR)
        return "skin"; /* sharks don't have scales */
    if ((mptr == &mons[PM_JELLYFISH] || mptr == &mons[PM_KRAKEN])
        && (part == ARM || part == FINGER || part == HAND || part == FOOT
            || part == TOE))
        return "tentacle";
    if (mptr == &mons[PM_FLOATING_EYE] && part == EYE)
        return "cornea";
    if (isHumanoid(mptr->monsterTypeID) && (part == ARM || part == FINGER || part == FINGERTIP
                           || part == HAND || part == HANDED))
        return humanoid_parts[part];
    if (mptr == &mons[PM_RAVEN])
        return bird_parts[part];
    if (mc == S_CENTAUR || mc == S_UNICORN
        || (mptr == &mons[PM_ROTHE] && part != HAIR))
        return horse_parts[part];
    if (mc == S_LIGHT) {
        if (part == HANDED)
            return "rayed";
        else if (part == ARM || part == FINGER || part == FINGERTIP
                 || part == HAND)
            return "ray";
        else
            return "beam";
    }
    if (mptr == &mons[PM_STALKER] && part == HEAD)
        return "head";
    if (mc == S_EEL && mptr != &mons[PM_JELLYFISH])
        return fish_parts[part];
    if (mc == S_WORM)
        return worm_parts[part];
    if (isSlithy(mptr->monsterTypeID) || (mc == S_DRAGON && part == HAIR))
        return snake_parts[part];
    if (mc == S_EYE)
        return sphere_parts[part];
    if (mc == S_JELLY || mc == S_PUDDING
        || mc == S_BLOB || mptr == &mons[PM_JELLYFISH])
        return jelly_parts[part];
    if (mc == S_VORTEX || mc == S_ELEMENTAL)
        return vortex_parts[part];
    if (mc == S_FUNGUS)
        return fungus_parts[part];
    if (isHumanoid(mptr->monsterTypeID))
        return humanoid_parts[part];
    return animal_parts[part];
}

const char *
body_part(part)
int part;
{
    return mbodypart(&youmonst, part);
}

int
poly_gender()
{
    /* Returns gender of polymorphed player;
     * 0/1=same meaning as flags.female, 2=none.
     */
    if (isNeuter(youmonst.data->monsterTypeID) || !isHumanoid(youmonst.data->monsterTypeID))
        return 2;
    return flags.female;
}

void
ugolemeffects(damtype, dam)
int damtype, dam;
{
    int heal = 0;

    /* We won't bother with "slow"/"haste" since players do not
     * have a monster-specific slow/haste so there is no way to
     * restore the old velocity once they are back to human.
     */
    if (currentMonsterNumber() != PM_FLESH_GOLEM && currentMonsterNumber() != PM_IRON_GOLEM)
        return;
    switch (damtype) {
    case AD_ELEC:
        if (currentMonsterNumber() == PM_FLESH_GOLEM)
            heal = (dam + 5) / 6; /* Approx 1 per die */
        break;
    case AD_FIRE:
        if (currentMonsterNumber() == PM_IRON_GOLEM)
            heal = dam;
        break;
    }
    if (heal && (currentHitPointsAsMonster() < maximumHitPointsAsMonster())) {
        increaseCurrentHitPointsAsMonster(heal);
        if (currentHitPointsAsMonster() > maximumHitPointsAsMonster())
            setCurrentHitPointsAsMonster(maximumHitPointsAsMonster());
        context.botl = 1;
        pline("Strangely, you feel better than before.");
        exercise(A_STR, TRUE);
    }
}

STATIC_OVL int
armor_to_dragon(atyp)
int atyp;
{
    switch (atyp) {
    case GRAY_DRAGON_SCALE_MAIL:
    case GRAY_DRAGON_SCALES:
        return PM_GRAY_DRAGON;
    case SILVER_DRAGON_SCALE_MAIL:
    case SILVER_DRAGON_SCALES:
        return PM_SILVER_DRAGON;
#if 0 /* DEFERRED */
    case SHIMMERING_DRAGON_SCALE_MAIL:
    case SHIMMERING_DRAGON_SCALES:
        return PM_SHIMMERING_DRAGON;
#endif
    case RED_DRAGON_SCALE_MAIL:
    case RED_DRAGON_SCALES:
        return PM_RED_DRAGON;
    case ORANGE_DRAGON_SCALE_MAIL:
    case ORANGE_DRAGON_SCALES:
        return PM_ORANGE_DRAGON;
    case WHITE_DRAGON_SCALE_MAIL:
    case WHITE_DRAGON_SCALES:
        return PM_WHITE_DRAGON;
    case BLACK_DRAGON_SCALE_MAIL:
    case BLACK_DRAGON_SCALES:
        return PM_BLACK_DRAGON;
    case BLUE_DRAGON_SCALE_MAIL:
    case BLUE_DRAGON_SCALES:
        return PM_BLUE_DRAGON;
    case GREEN_DRAGON_SCALE_MAIL:
    case GREEN_DRAGON_SCALES:
        return PM_GREEN_DRAGON;
    case YELLOW_DRAGON_SCALE_MAIL:
    case YELLOW_DRAGON_SCALES:
        return PM_YELLOW_DRAGON;
    default:
        return -1;
    }
}

/*
 * Some species have awareness of other species
 */
static boolean
polysense(mptr)
struct permonst *mptr;
{
    short warnidx = 0;

    context.warntype.speciesidx = 0;
    context.warntype.species = 0;
    context.warntype.polyd = 0;

    switch (monsndx(mptr)) {
    case PM_PURPLE_WORM:
        warnidx = PM_SHRIEKER;
        break;
    case PM_VAMPIRE:
    case PM_VAMPIRE_LORD:
        context.warntype.polyd = M2_HUMAN | M2_ELF;
        setYourIntrinsicMask(WARN_OF_MON, FROMRACE);
        return TRUE;
    }
    if (warnidx) {
        context.warntype.speciesidx = warnidx;
        context.warntype.species = &mons[warnidx];
        setYourIntrinsicMask(WARN_OF_MON, FROMRACE);
        return TRUE;
    }
    context.warntype.speciesidx = 0;
    context.warntype.species = 0;
    unsetYourIntrinsicMask(WARN_OF_MON, FROMRACE);
    return FALSE;
}

/*polyself.c*/
