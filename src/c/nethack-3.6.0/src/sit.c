/* NetHack 3.6	sit.c	$NHDT-Date: 1445906863 2015/10/27 00:47:43 $  $NHDT-Branch: master $:$NHDT-Revision: 1.51 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"


/* take away the hero's money */
void
take_gold()
{
    struct obj *otmp, *nobj;
    int lost_money = 0;

    for (otmp = invent; otmp; otmp = nobj) {
        nobj = otmp->nobj;
        if (otmp->oclass == COIN_CLASS) {
            lost_money = 1;
            remove_worn_item(otmp, FALSE);
            delobj(otmp);
        }
    }
    if (!lost_money) {
        You_feel("a strange sensation.");
    } else {
        You("notice you have no money!");
        context.botl = 1;
    }
}

/* #sit command */
int
dosit()
{
    static const char sit_message[] = "sit on the %s.";
    register struct trap *trap = t_at(currentX(), currentY());
    register int typ = levl[currentX()][currentY()].typ;

    if (u.usteed) {
        You("are already sitting on %s.", mon_nam(u.usteed));
        return 0;
    }
    if (lurking() && isHider(youmonst.data->monsterTypeID) && currentMonsterNumber() != PM_TRAPPER)
        setLurking(FALSE); /* no longer on the ceiling */

    if (!can_reach_floor(FALSE)) {
        if (swallowed())
            There("are no seats in here!");
        else if (youAreLevitating())
            You("tumble in place.");
        else
            You("are sitting on air.");
        return 0;
    } else if (u.ustuck && !sticks(youmonst.data->monsterTypeID)) {
        /* holding monster is next to hero rather than beneath, but
           hero is in no condition to actually sit at has/her own spot */
        if (isHumanoid(u.ustuck->data->monsterTypeID))
            pline("%s won't offer %s lap.", Monnam(u.ustuck), mhis(u.ustuck));
        else
            pline("%s has no lap.", Monnam(u.ustuck));
        return 0;
    } else if (is_pool(currentX(), currentY()) && !underwater()) { /* water walking */
        goto in_water;
    }

    if (OBJ_AT(currentX(), currentY())
        /* ensure we're not standing on the precipice */
        && !uteetering_at_seen_pit(trap)) {
        register struct obj *obj;

        obj = level.objects[currentX()][currentY()];
        if (monsterClass(youmonst.data->monsterTypeID) == S_DRAGON && obj->oclass == COIN_CLASS) {
            You("coil up around your %shoard.",
                (obj->quan + money_cnt(invent) < currentExperienceLevel() * 1000) ? "meager "
                                                                  : "");
        } else {
            You("sit on %s.", the(xname(obj)));
            if (!(Is_box(obj) || objects[obj->otyp].oc_material == CLOTH))
                pline("It's not very comfortable...");
        }
    } else if (trap != 0 || (currentlyTrapped() && (currentTrapType() >= TT_LAVA))) {
        if (currentlyTrapped()) {
            exercise(A_WIS, FALSE); /* you're getting stuck longer */
            if (currentTrapType() == TT_BEARTRAP) {
                You_cant("sit down with your %s in the bear trap.",
                         body_part(FOOT));
                setCurrentTrapTimeout(currentTrapTimeout() + 1);
            } else if (currentTrapType() == TT_PIT) {
                if (trap && trap->ttyp == SPIKED_PIT) {
                    You("sit down on a spike.  Ouch!");
                    losehp(youTakeHalfDamageFromPhysicalAttacks() ? rn2(2) : 1,
                           "sitting on an iron spike", KILLED_BY);
                    exercise(A_STR, FALSE);
                } else {
                    You("sit down in the pit.");
                }
                setCurrentTrapTimeout(currentTrapTimeout() + rn2(5));
            } else if (currentTrapType() == TT_WEB) {
                You("sit in the spider web and get entangled further!");
                setCurrentTrapTimeout(currentTrapTimeout() + rn1(10, 5));
            } else if (currentTrapType() == TT_LAVA) {
                /* Must have fire resistance or they'd be dead already */
                You("sit in the lava!");
                if (youAreTurningToSlime())
                    burn_away_slime();
                setCurrentTrapTimeout(currentTrapTimeout() + rnd(4));
                losehp(d(2, 10), "sitting in lava",
                       KILLED_BY); /* lava damage */
            } else if (currentTrapType() == TT_INFLOOR
                       || currentTrapType() == TT_BURIEDBALL) {
                You_cant("maneuver to sit!");
                setCurrentTrapTimeout(currentTrapTimeout() + 1);
            }
        } else {
            You("sit down.");
            dotrap(trap, 0);
        }
    } else if (underwater() || areYouOnWaterLevel()) {
        if (areYouOnWaterLevel())
            There("are no cushions floating nearby.");
        else
            You("sit down on the muddy bottom.");
    } else if (is_pool(currentX(), currentY())) {
    in_water:
        You("sit in the water.");
        if (!rn2(10) && uarm)
            (void) water_damage(uarm, "armor", TRUE);
        if (!rn2(10) && uarmf && uarmf->otyp != WATER_WALKING_BOOTS)
            (void) water_damage(uarm, "armor", TRUE);
    } else if (IS_SINK(typ)) {
        You(sit_message, defsyms[S_sink].explanation);
        Your("%s gets wet.", isHumanoid(youmonst.data->monsterTypeID) ? "rump" : "underside");
    } else if (IS_ALTAR(typ)) {
        You(sit_message, defsyms[S_altar].explanation);
        altar_wrath(currentX(), currentY());
    } else if (IS_GRAVE(typ)) {
        You(sit_message, defsyms[S_grave].explanation);
    } else if (typ == STAIRS) {
        You(sit_message, "stairs");
    } else if (typ == LADDER) {
        You(sit_message, "ladder");
    } else if (is_lava(currentX(), currentY())) {
        /* must be WWalking */
        You(sit_message, "lava");
        burn_away_slime();
        if (likes_lava(youmonst.data)) {
            pline_The("lava feels warm.");
            return 1;
        }
        pline_The("lava burns you!");
        losehp(d((youResistFire() ? 2 : 10), 10), /* lava damage */
               "sitting on lava", KILLED_BY);
    } else if (is_ice(currentX(), currentY())) {
        You(sit_message, defsyms[S_ice].explanation);
        if (!youResistCold())
            pline_The("ice feels cold.");
    } else if (typ == DRAWBRIDGE_DOWN) {
        You(sit_message, "drawbridge");
    } else if (IS_THRONE(typ)) {
        You(sit_message, defsyms[S_throne].explanation);
        if (rnd(6) > 4) {
            switch (rnd(13)) {
            case 1:
                (void) adjattrib(rn2(A_MAX), -rn1(4, 3), FALSE);
                losehp(rnd(10), "cursed throne", KILLED_BY_AN);
                break;
            case 2:
                (void) adjattrib(rn2(A_MAX), 1, FALSE);
                break;
            case 3:
                pline("A%s electric shock shoots through your body!",
                      (youResistShock()) ? "n" : " massive");
                losehp(youResistShock() ? rnd(6) : rnd(30), "electric chair",
                       KILLED_BY_AN);
                exercise(A_CON, FALSE);
                break;
            case 4:
                You_feel("much, much better!");
                if (areYouPolymorphed()) {
                    if (currentHitPointsAsMonster() >= (maximumHitPointsAsMonster() - 5)) {
                        increaseMaximumHitPointsAsMonster(4);
                    }
                    setCurrentHitPointsAsMonster(maximumHitPointsAsMonster());
                }
                if (currentHitPoints() >= (maximumHitPoints() - 5))
                    increaseMaximumHitPoints(4);
                setCurrentHitPoints(maximumHitPoints());
                make_blinded(0L, TRUE);
                make_sick(0L, (char *) 0, FALSE, SICK_ALL);
                heal_legs();
                context.botl = 1;
                break;
            case 5:
                take_gold();
                break;
            case 6:
                if (currentLuck() + rn2(5) < 0) {
                    You_feel("your luck is changing.");
                    change_luck(1);
                } else
                    makewish();
                break;
            case 7:
              {
                int cnt = rnd(10);

                /* Magical voice not affected by deafness */
                pline("A voice echoes:");
                verbalize("Thy audience hath been summoned, %s!",
                          flags.female ? "Dame" : "Sire");
                while (cnt--)
                    (void) makemon(courtmon(), currentX(), currentY(), NO_MM_FLAGS);
                break;
              }
            case 8:
                /* Magical voice not affected by deafness */
                pline("A voice echoes:");
                verbalize("By thine Imperious order, %s...",
                          flags.female ? "Dame" : "Sire");
                do_genocide(5); /* REALLY|ONTHRONE, see do_genocide() */
                break;
            case 9:
                /* Magical voice not affected by deafness */
                pline("A voice echoes:");
                verbalize(
                 "A curse upon thee for sitting upon this most holy throne!");
                if (currentLuckWithBonus() > 0) {
                    make_blinded(yourIntrinsic(BLINDED) + rn1(100, 250), TRUE);
                } else
                    rndcurse();
                break;
            case 10:
                if (currentLuckWithBonus() < 0 || yourIntrinsicHasMask(SEE_INVIS, INTRINSIC)) {
                    if (level.flags.nommap) {
                        pline("A terrible drone fills your head!");
                        make_confused(yourIntrinsicTimeout(CONFUSION) + (long) rnd(30),
                                      FALSE);
                    } else {
                        pline("An image forms in your mind.");
                        do_mapping();
                    }
                } else {
                    Your("vision becomes clear.");
                    setYourIntrinsicMask(SEE_INVIS, FROMOUTSIDE);
                    newsym(currentX(), currentY());
                }
                break;
            case 11:
                if (currentLuckWithBonus() < 0) {
                    You_feel("threatened.");
                    aggravate();
                } else {
                    You_feel("a wrenching sensation.");
                    tele(); /* teleport him */
                }
                break;
            case 12:
                You("are granted an insight!");
                if (invent) {
                    /* rn2(5) agrees w/seffects() */
                    identify_pack(rn2(5), FALSE);
                }
                break;
            case 13:
                Your("mind turns into a pretzel!");
                make_confused(yourIntrinsicTimeout(CONFUSION) + (long) rn1(7, 16),
                              FALSE);
                break;
            default:
                impossible("throne effect");
                break;
            }
        } else {
            if (isPrince(youmonst.data->monsterTypeID))
                You_feel("very comfortable here.");
            else
                You_feel("somehow out of place...");
        }

        if (!rn2(3) && IS_THRONE(levl[currentX()][currentY()].typ)) {
            /* may have teleported */
            levl[currentX()][currentY()].typ = ROOM;
            pline_The("throne vanishes in a puff of logic.");
            newsym(currentX(), currentY());
        }
    } else if (laysEggs(youmonst.data->monsterTypeID)) {
        struct obj *uegg;

        if (!flags.female) {
            pline("%s can't lay eggs!",
                  youAreHallucinating()
                      ? "You may think you are a platypus, but a male still"
                      : "Males");
            return 0;
        } else if (currentNutrition() < (int) objects[EGG].oc_nutrition) {
            You("don't have enough energy to lay an egg.");
            return 0;
        }

        uegg = mksobj(EGG, FALSE, FALSE);
        uegg->spe = 1;
        uegg->quan = 1L;
        uegg->owt = weight(uegg);
        /* this sets hatch timers if appropriate */
        set_corpsenm(uegg, egg_type_from_parent(currentMonsterNumber(), FALSE));
        uegg->known = uegg->dknown = 1;
        You("lay an egg.");
        dropy(uegg);
        stackobj(uegg);
        morehungry((int) objects[EGG].oc_nutrition);
    } else {
        pline("Having fun sitting on the %s?", surface(currentX(), currentY()));
    }
    return 1;
}

/* curse a few inventory items at random! */
void
rndcurse()
{
    int nobj = 0;
    int cnt, onum;
    struct obj *otmp;
    static const char mal_aura[] = "feel a malignant aura surround %s.";

    if (uwep && (uwep->oartifact == ART_MAGICBANE) && rn2(20)) {
        You(mal_aura, "the magic-absorbing blade");
        return;
    }

    if (youResistMagic()) {
        shieldeff(currentX(), currentY());
        You(mal_aura, "you");
    }

    for (otmp = invent; otmp; otmp = otmp->nobj) {
        /* gold isn't subject to being cursed or blessed */
        if (otmp->oclass == COIN_CLASS)
            continue;
        nobj++;
    }
    if (nobj) {
        for (cnt = rnd(6 / ((youResistMagic()?1:0) + (youTakeHalfDamageFromSpells() ? 1:0) + 1));
             cnt > 0; cnt--) {
            onum = rnd(nobj);
            for (otmp = invent; otmp; otmp = otmp->nobj) {
                /* as above */
                if (otmp->oclass == COIN_CLASS)
                    continue;
                if (--onum == 0)
                    break; /* found the target */
            }
            /* the !otmp case should never happen; picking an already
               cursed item happens--avoid "resists" message in that case */
            if (!otmp || otmp->cursed)
                continue; /* next target */

            if (otmp->oartifact && spec_ability(otmp, SPFX_INTEL)
                && rn2(10) < 8) {
                pline("%s!", Tobjnam(otmp, "resist"));
                continue;
            }

            if (otmp->blessed)
                unbless(otmp);
            else
                curse(otmp);
        }
        update_inventory();
    }

    /* treat steed's saddle as extended part of hero's inventory */
    if (u.usteed && !rn2(4) && (otmp = which_armor(u.usteed, W_SADDLE)) != 0
        && !otmp->cursed) { /* skip if already cursed */
        if (otmp->blessed)
            unbless(otmp);
        else
            curse(otmp);
        if (youCanSee()) {
            pline("%s %s.", Yobjnam2(otmp, "glow"),
                  hcolor(otmp->cursed ? NH_BLACK : (const char *) "brown"));
            otmp->bknown = TRUE;
        }
    }
}

/* remove a random INTRINSIC ability */
void
attrcurse()
{
    switch (rnd(11)) {
    case 1:
        if (yourIntrinsicHasMask(FIRE_RES, INTRINSIC)) {
            unsetYourIntrinsicMask(FIRE_RES, INTRINSIC);
            You_feel("warmer.");
            break;
        }
    case 2:
        if (yourIntrinsicHasMask(TELEPORT, INTRINSIC)) {
            unsetYourIntrinsicMask(TELEPORT, INTRINSIC);
            You_feel("less jumpy.");
            break;
        }
    case 3:
        if (yourIntrinsicHasMask(POISON_RES, INTRINSIC)) {
            unsetYourIntrinsicMask(POISON_RES, INTRINSIC);
            You_feel("a little sick!");
            break;
        }
    case 4:
        if (yourIntrinsicHasMask(TELEPAT, INTRINSIC)) {
            unsetYourIntrinsicMask(TELEPAT, INTRINSIC);
            if (youCannotSee() && !youHaveTelepathyWhenBlind())
                see_monsters(); /* Can't sense mons anymore! */
            Your("senses fail!");
            break;
        }
    case 5:
        if (yourIntrinsicHasMask(COLD_RES, INTRINSIC)) {
            unsetYourIntrinsicMask(COLD_RES, INTRINSIC);
            You_feel("cooler.");
            break;
        }
    case 6:
        if (yourIntrinsicHasMask(INVIS, INTRINSIC)) {
            unsetYourIntrinsicMask(INVIS, INTRINSIC);
            You_feel("paranoid.");
            break;
        }
    case 7:
        if (yourIntrinsicHasMask(SEE_INVIS, INTRINSIC)) {
            unsetYourIntrinsicMask(SEE_INVIS, INTRINSIC);
            You("%s!", youAreHallucinating() ? "tawt you taw a puttie tat"
                                     : "thought you saw something");
            break;
        }
    case 8:
        if (yourIntrinsicHasMask(FAST, INTRINSIC)) {
            unsetYourIntrinsicMask(FAST, INTRINSIC);
            You_feel("slower.");
            break;
        }
    case 9:
        if (yourIntrinsicHasMask(STEALTH, INTRINSIC)) {
            unsetYourIntrinsicMask(STEALTH, INTRINSIC);
            You_feel("clumsy.");
            break;
        }
    case 10:
        /* intrinsic protection is just disabled, not set back to 0 */
        if (yourIntrinsicHasMask(PROTECTION, INTRINSIC)) {
            unsetYourIntrinsicMask(PROTECTION, INTRINSIC);
            You_feel("vulnerable.");
            break;
        }
    case 11:
        if (yourIntrinsicHasMask(AGGRAVATE_MONSTER, INTRINSIC)) {
            unsetYourIntrinsicMask(AGGRAVATE_MONSTER, INTRINSIC);
            You_feel("less attractive.");
            break;
        }
    default:
        break;
    }
}

/*sit.c*/
