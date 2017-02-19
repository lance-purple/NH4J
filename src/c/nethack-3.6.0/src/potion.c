/* NetHack 3.6	potion.c	$NHDT-Date: 1446861768 2015/11/07 02:02:48 $  $NHDT-Branch: master $:$NHDT-Revision: 1.121 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

boolean notonhead = FALSE;

static NEARDATA int nothing, unkn;
static NEARDATA const char beverages[] = { POTION_CLASS, 0 };

STATIC_DCL void NDECL(ghost_from_bottle);
STATIC_DCL boolean
FDECL(H2Opotion_dip, (struct obj *, struct obj *, BOOLEAN_P, const char *));
STATIC_DCL short FDECL(mixtype, (struct obj *, struct obj *));

static long itimeout_incr(old, incr)
long old;
int incr;
{
   long value = (old & TIMEOUT) + (long) incr;
   if (value >= TIMEOUT) {
     value = TIMEOUT;
   } else if (value < 1) {
     value = 0;
   }
   return value;
}

void
make_confused(xtime, talk)
long xtime;
boolean talk;
{
    long priorConfusion = yourIntrinsicTimeout(CONFUSION);

    if (youAreUnaware())
        talk = FALSE;

    if (!xtime && priorConfusion) {
        if (talk)
            You_feel("less %s now.", Hallucination ? "trippy" : "confused");
    }
    if ((xtime && !priorConfusion) || (!xtime && priorConfusion))
        context.botl = TRUE;

    setYourIntrinsicTimeout(CONFUSION, xtime);
}

void
make_stunned(xtime, talk)
long xtime;
boolean talk;
{
    long priorStunned = yourIntrinsicTimeout(STUNNED);

    if (youAreUnaware())
        talk = FALSE;

    if (!xtime && priorStunned) {
        if (talk)
            You_feel("%s now.",
                     Hallucination ? "less wobbly" : "a bit steadier");
    }
    if (xtime && !priorStunned) {
        if (talk) {
            if (u.usteed)
                You("wobble in the saddle.");
            else
                You("%s...", stagger(youmonst.data, "stagger"));
        }
    }
    if ((!xtime && priorStunned) || (xtime && !priorStunned))
        context.botl = TRUE;

    setYourIntrinsicTimeout(STUNNED, xtime);
}

void
make_sick(xtime, cause, talk, mask)
long xtime;
const char *cause; /* sickness cause */
boolean talk;
int mask;
{
    boolean foodPoisoning = (mask & SICK_VOMITABLE);
    boolean illness       = (mask & SICK_NONVOMITABLE);
    long old = Sick;

#if 0
    if (youAreUnaware())
        talk = FALSE;
#endif
    if (xtime > 0L) {
        if (youResistSickness())
            return;
        if (!old) {
            /* newly sick */
            You_feel("deathly sick.");
        } else {
            /* already sick */
            if (talk) {
                You_feel("%s worse.", xtime <= Sick / 2L ? "much" : "even");
            }
        }
        setYourIntrinsicTimeout(SICK, xtime);

        setSickWithFoodPoisoning(foodPoisoning);
        setSickWithIllness(illness);

        context.botl = TRUE;
    } else if (old && ((foodPoisoning != sickWithFoodPoisoning()) || (illness != sickWithIllness()))) {
        /* was sick, now not */
        if (sickWithFoodPoisoning() && !foodPoisoning) {
            setSickWithFoodPoisoning(FALSE);
        }
        if (sickWithIllness() && !illness) {
            setSickWithIllness(FALSE);
        }
        if (sickWithFoodPoisoning() || sickWithIllness()) { /* only partly cured */
            if (talk) {
                You_feel("somewhat better.");
            }
            setYourIntrinsicTimeout(SICK, Sick * 2); /* approximation */
        } else {
            if (talk)
                You_feel("cured.  What a relief!");
            Sick = 0L; /* setYourIntrinsicTimeout(SICK, 0L) */
        }
        context.botl = TRUE;
    }

    if (Sick) {
        exercise(A_CON, FALSE);
        delayed_killer(SICK, KILLED_BY_AN, cause);
    } else
        dealloc_killer(find_delayed_killer(SICK));
}

void
make_slimed(xtime, msg)
long xtime;
const char *msg;
{
    long old = Slimed;

#if 0
    if (youAreUnaware())
        msg = 0;
#endif
    if ((!xtime && old) || (xtime && !old)) {
        if (msg)
            pline1(msg);
        context.botl = 1;
    }
    setYourIntrinsicTimeout(SLIMED, xtime);
    if (!Slimed)
        dealloc_killer(find_delayed_killer(SLIMED));
}

/* start or stop petrification */
void
make_stoned(xtime, msg, killedby, killername)
long xtime;
const char *msg;
int killedby;
const char *killername;
{
    long old = Stoned;

#if 0
    if (youAreUnaware())
        msg = 0;
#endif
    if ((!xtime && old) || (xtime && !old)) {
        if (msg)
            pline1(msg);
        /* context.botl = 1;   --- Stoned is not a status line item */
    }
    setYourIntrinsicTimeout(STONED, xtime);
    if (!Stoned)
        dealloc_killer(find_delayed_killer(STONED));
    else if (!old)
        delayed_killer(STONED, killedby, killername);
}

void
make_vomiting(xtime, talk)
long xtime;
boolean talk;
{
    long old = Vomiting;

    if (youAreUnaware())
        talk = FALSE;

    if (!xtime && old)
        if (talk)
            You_feel("much less nauseated now.");

    setYourIntrinsicTimeout(VOMITING, xtime);
}

static const char vismsg[] = "vision seems to %s for a moment but is %s now.";
static const char eyemsg[] = "%s momentarily %s.";

void
make_blinded(xtime, talk)
long xtime;
boolean talk;
{
    long old = Blinded;
    boolean u_could_see, can_see_now;
    const char *eyes;

    /* we need to probe ahead in case the Eyes of the Overworld
       are or will be overriding blindness */
    u_could_see = !Blind;
    Blinded = xtime ? 1L : 0L;
    can_see_now = !Blind;
    Blinded = old; /* restore */

    if (youAreUnaware())
        talk = FALSE;

    if (can_see_now && !u_could_see) { /* regaining sight */
        if (talk) {
            if (Hallucination)
                pline("Far out!  Everything is all cosmic again!");
            else
                You("can see again.");
        }
    } else if (old && !xtime) {
        /* clearing temporary blindness without toggling blindness */
        if (talk) {
            if (!haseyes(youmonst.data)) {
                strange_feeling((struct obj *) 0, (char *) 0);
            } else if (Blindfolded) {
                eyes = body_part(EYE);
                if (eyecount(youmonst.data) != 1)
                    eyes = makeplural(eyes);
                Your(eyemsg, eyes, vtense(eyes, "itch"));
            } else { /* Eyes of the Overworld */
                Your(vismsg, "brighten", Hallucination ? "sadder" : "normal");
            }
        }
    }

    if (u_could_see && !can_see_now) { /* losing sight */
        if (talk) {
            if (Hallucination)
                pline("Oh, bummer!  Everything is dark!  Help!");
            else
                pline("A cloud of darkness falls upon you.");
        }
        /* Before the hero goes blind, set the ball&chain variables. */
        if (youAreBeingPunished())
            set_bc(0);
    } else if (!old && xtime) {
        /* setting temporary blindness without toggling blindness */
        if (talk) {
            if (!haseyes(youmonst.data)) {
                strange_feeling((struct obj *) 0, (char *) 0);
            } else if (Blindfolded) {
                eyes = body_part(EYE);
                if (eyecount(youmonst.data) != 1)
                    eyes = makeplural(eyes);
                Your(eyemsg, eyes, vtense(eyes, "twitch"));
            } else { /* Eyes of the Overworld */
                Your(vismsg, "dim", Hallucination ? "happier" : "normal");
            }
        }
    }

    setYourIntrinsicTimeout(BLINDED, xtime);

    if (u_could_see ^ can_see_now) { /* one or the other but not both */
        context.botl = 1;
        vision_full_recalc = 1; /* blindness just got toggled */
        /* this vision recalculation used to be deferred until
           moveloop(), but that made it possible for vision
           irregularities to occur (cited case was force bolt
           hitting adjacent potion of blindness and then a
           secret door; hero was blinded by vapors but then
           got the message "a door appears in the wall") */
        vision_recalc(0);
        if (Blind_telepat || Infravision)
            see_monsters();

        /* avoid either of the sequences
           "Sting starts glowing", [become blind], "Sting stops quivering" or
           "Sting starts quivering", [regain sight], "Sting stops glowing"
           by giving "Sting is quivering" when becoming blind or
           "Sting is glowing" when regaining sight so that the eventual
           "stops" message matches */
        if (warn_obj_cnt && uwep && (EWarn_of_mon & W_WEP) != 0L)
            Sting_effects(-1);
        /* update dknown flag for inventory picked up while blind */
        if (can_see_now)
            learn_unseen_invent();
    }
}

boolean
make_hallucinated(xtime, talk, mask)
long xtime; /* nonzero if this is an attempt to turn on hallucination */
boolean talk;
long mask; /* nonzero if resistance status should change by mask */
{
    long old = HHallucination;
    boolean changed = 0;
    const char *message, *verb;

    if (youAreUnaware())
        talk = FALSE;

    message = (!xtime) ? "Everything %s SO boring now."
                       : "Oh wow!  Everything %s so cosmic!";
    verb = (!Blind) ? "looks" : "feels";

    if (mask) {
        if (HHallucination)
            changed = TRUE;

        if (!xtime)
            EHalluc_resistance |= mask;
        else
            EHalluc_resistance &= ~mask;
    } else {
        if (!EHalluc_resistance && (!!HHallucination != !!xtime))
            changed = TRUE;
        setYourIntrinsicTimeout(HALLUC, xtime);

        /* clearing temporary hallucination without toggling vision */
        if (!changed && !HHallucination && old && talk) {
            if (!haseyes(youmonst.data)) {
                strange_feeling((struct obj *) 0, (char *) 0);
            } else if (Blind) {
                const char *eyes = body_part(EYE);

                if (eyecount(youmonst.data) != 1)
                    eyes = makeplural(eyes);
                Your(eyemsg, eyes, vtense(eyes, "itch"));
            } else { /* Grayswandir */
                Your(vismsg, "flatten", "normal");
            }
        }
    }

    if (changed) {
        /* in case we're mimicking an orange (hallucinatory form
           of mimicking gold) update the mimicking's-over message */
        if (!Hallucination)
            eatmupdate();

        if (swallowed()) {
            showHeroBeingSwallowed(0); /* redraw swallow display */
        } else {
            /* The see_* routines should be called *before* the pline. */
            see_monsters();
            see_objects();
            see_traps();
        }

        /* for perm_inv and anything similar
        (eg. Qt windowport's equipped items display) */
        update_inventory();

        context.botl = 1;
        if (talk)
            pline(message, verb);
    }
    return changed;
}

void
make_deaf(xtime, talk)
long xtime;
boolean talk;
{
    long old = HDeaf;
    boolean toggled = FALSE;

    if (youAreUnaware())
        talk = FALSE;

    if (!xtime && old) {
        if (talk)
            You("can hear again.");
        toggled = TRUE;
    } else if (xtime && !old) {
        if (talk)
            You("are unable to hear anything.");
        toggled = TRUE;
    }
    /* deafness isn't presently shown on status line, but
       request a status update in case that changes someday */
    if (toggled)
        context.botl = TRUE;

    setYourIntrinsicTimeout(DEAF, xtime);
}

STATIC_OVL void
ghost_from_bottle()
{
    struct monst *mtmp = makemon(&mons[PM_GHOST], currentX(), currentY(), NO_MM_FLAGS);

    if (!mtmp) {
        pline("This bottle turns out to be empty.");
        return;
    }
    if (Blind) {
        pline("As you open the bottle, %s emerges.", something);
        return;
    }
    pline("As you open the bottle, an enormous %s emerges!",
          Hallucination ? rndmonnam(NULL) : (const char *) "ghost");
    if (flags.verbose)
        You("are frightened to death, and unable to move.");
    nomul(-3);
    multi_reason = "being frightened to death";
    nomovemsg = "You regain your composure.";
}

/* "Quaffing is like drinking, except you spill more." - Terry Pratchett */
int
dodrink()
{
    register struct obj *otmp;
    const char *potion_descr;

    if (Strangled) {
        pline("If you can't breathe air, how can you drink liquid?");
        return 0;
    }
    /* Is there a fountain to drink from here? */
    if (IS_FOUNTAIN(levl[currentX()][currentY()].typ)
        /* not as low as floor level but similar restrictions apply */
        && can_reach_floor(FALSE)) {
        if (yn("Drink from the fountain?") == 'y') {
            drinkfountain();
            return 1;
        }
    }
    /* Or a kitchen sink? */
    if (IS_SINK(levl[currentX()][currentY()].typ)
        /* not as low as floor level but similar restrictions apply */
        && can_reach_floor(FALSE)) {
        if (yn("Drink from the sink?") == 'y') {
            drinksink();
            return 1;
        }
    }
    /* Or are you surrounded by water? */
    if (underwater() && !swallowed()) {
        if (yn("Drink the water around you?") == 'y') {
            pline("Do you know what lives in this water?");
            return 1;
        }
    }

    otmp = getobj(beverages, "drink");
    if (!otmp)
        return 0;

    /* quan > 1 used to be left to useup(), but we need to force
       the current potion to be unworn, and don't want to do
       that for the entire stack when starting with more than 1.
       [Drinking a wielded potion of polymorph can trigger a shape
       change which causes hero's weapon to be dropped.  In 3.4.x,
       that led to an "object lost" panic since subsequent useup()
       was no longer dealing with an inventory item.  Unwearing
       the current potion is intended to keep it in inventory.] */
    if (otmp->quan > 1L) {
        otmp = splitobj(otmp, 1L);
        otmp->owornmask = 0L; /* rest of original stuck unaffected */
    } else if (otmp->owornmask) {
        remove_worn_item(otmp, FALSE);
    }
    otmp->in_use = TRUE; /* you've opened the stopper */

    potion_descr = OBJ_DESCR(objects[otmp->otyp]);
    if (potion_descr) {
        if (!strcmp(potion_descr, "milky")
            && !(mvitals[PM_GHOST].mvflags & G_GONE)
            && !rn2(POTION_OCCUPANT_CHANCE(mvitals[PM_GHOST].born))) {
            ghost_from_bottle();
            useup(otmp);
            return 1;
        } else if (!strcmp(potion_descr, "smoky")
                   && !(mvitals[PM_DJINNI].mvflags & G_GONE)
                   && !rn2(POTION_OCCUPANT_CHANCE(mvitals[PM_DJINNI].born))) {
            djinni_from_bottle(otmp);
            useup(otmp);
            return 1;
        }
    }
    return dopotion(otmp);
}

int
dopotion(otmp)
register struct obj *otmp;
{
    int retval;

    otmp->in_use = TRUE;
    nothing = unkn = 0;
    if ((retval = peffects(otmp)) >= 0)
        return retval;

    if (nothing) {
        unkn++;
        You("have a %s feeling for a moment, then it passes.",
            Hallucination ? "normal" : "peculiar");
    }
    if (otmp->dknown && !objects[otmp->otyp].oc_name_known) {
        if (!unkn) {
            makeknown(otmp->otyp);
            more_experienced(0, 10);
        } else if (!objects[otmp->otyp].oc_uname)
            docall(otmp);
    }
    useup(otmp);
    return 1;
}

int
peffects(otmp)
register struct obj *otmp;
{
    register int i, ii, lim;

    switch (otmp->otyp) {
    case POT_RESTORE_ABILITY:
    case SPE_RESTORE_ABILITY:
        unkn++;
        if (otmp->cursed) {
            pline("Ulch!  This makes you feel mediocre!");
            break;
        } else {
            /* unlike unicorn horn, overrides Fixed_abil */
            pline("Wow!  This makes you feel %s!",
                  (otmp->blessed)
                      ? (unfixable_trouble_count(FALSE) ? "better" : "great")
                      : "good");
            i = rn2(A_MAX); /* start at a random point */
            for (ii = 0; ii < A_MAX; ii++) {
                lim = yourAttrMax(i);
                if (i == A_STR && currentHungerState() >= 3)
                    --lim; /* WEAK */
                if (yourCurrentAttr(i) < lim) {
                    setYourCurrentAttr(i, lim);
                    context.botl = 1;
                    /* only first found if not blessed */
                    if (!otmp->blessed)
                        break;
                }
                if (++i >= A_MAX)
                    i = 0;
            }
        }
        break;
    case POT_HALLUCINATION:
        if (Hallucination || Halluc_resistance)
            nothing++;
        (void) make_hallucinated(
            itimeout_incr(HHallucination, rn1(200, 600 - 300 * bcsign(otmp))),
            TRUE, 0L);
        break;
    case POT_WATER:
        if (!otmp->blessed && !otmp->cursed) {
            pline("This tastes like water.");
            increaseCurrentNutrition(rnd(10));
            newuhs(FALSE);
            break;
        }
        unkn++;
        if (is_undead(youmonst.data) || is_demon(youmonst.data)
            || currentAlignmentType() == A_CHAOTIC) {
            if (otmp->blessed) {
                pline("This burns like acid!");
                exercise(A_CON, FALSE);
                if (lycanthropeType() >= LOW_PM) {
                    Your("affinity to %s disappears!",
                         makeplural(mons[lycanthropeType()].mname));
                    if (youmonst.data == &mons[lycanthropeType()])
                        you_unwere(FALSE);
                    setLycanthropeType(NON_PM); /* cure lycanthropy */
                }
                losehp(Maybe_Half_Phys(d(2, 6)), "potion of holy water",
                       KILLED_BY_AN);
            } else if (otmp->cursed) {
                You_feel("quite proud of yourself.");
                healup(d(2, 6), 0, 0, 0);
                if (lycanthropeType() >= LOW_PM && !areYouPolymorphed())
                    you_were();
                exercise(A_CON, TRUE);
            }
        } else {
            if (otmp->blessed) {
                You_feel("full of awe.");
                make_sick(0L, (char *) 0, TRUE, SICK_ALL);
                exercise(A_WIS, TRUE);
                exercise(A_CON, TRUE);
                if (lycanthropeType() >= LOW_PM)
                    you_unwere(TRUE); /* "Purified" */
                /* make_confused(0L, TRUE); */
            } else {
                if (currentAlignmentType() == A_LAWFUL) {
                    pline("This burns like acid!");
                    losehp(Maybe_Half_Phys(d(2, 6)), "potion of unholy water",
                           KILLED_BY_AN);
                } else
                    You_feel("full of dread.");
                if (lycanthropeType() >= LOW_PM && !areYouPolymorphed())
                    you_were();
                exercise(A_CON, FALSE);
            }
        }
        break;
    case POT_BOOZE:
        unkn++;
        pline("Ooph!  This tastes like %s%s!",
              otmp->odiluted ? "watered down " : "",
              Hallucination ? "dandelion wine" : "liquid fire");
        if (!otmp->blessed)
            make_confused(itimeout_incr(yourIntrinsic(CONFUSION), d(3, 8)), FALSE);
        /* the whiskey makes us feel better */
        if (!otmp->odiluted)
            healup(1, 0, FALSE, FALSE);
        increaseCurrentNutrition(10 * (2 + bcsign(otmp)));
        newuhs(FALSE);
        exercise(A_WIS, FALSE);
        if (otmp->cursed) {
            You("pass out.");
            multi = -rnd(15);
            nomovemsg = "You awake with a headache.";
        }
        break;
    case POT_ENLIGHTENMENT:
        if (otmp->cursed) {
            unkn++;
            You("have an uneasy feeling...");
            exercise(A_WIS, FALSE);
        } else {
            if (otmp->blessed) {
                (void) adjattrib(A_INT, 1, FALSE);
                (void) adjattrib(A_WIS, 1, FALSE);
            }
            You_feel("self-knowledgeable...");
            display_nhwindow(WIN_MESSAGE, FALSE);
            enlightenment(MAGICENLIGHTENMENT, ENL_GAMEINPROGRESS);
            pline_The("feeling subsides.");
            exercise(A_WIS, TRUE);
        }
        break;
    case SPE_INVISIBILITY:
        /* spell cannot penetrate mummy wrapping */
        if (BInvis && uarmc->otyp == MUMMY_WRAPPING) {
            You_feel("rather itchy under %s.", yname(uarmc));
            break;
        }
        /* FALLTHRU */
    case POT_INVISIBILITY:
        if (Invis || Blind || BInvis) {
            nothing++;
        } else {
            self_invis_message();
        }
        if (otmp->blessed)
            HInvis |= FROMOUTSIDE;
        else
            incrementYourIntrinsicTimeout(INVIS, rn1(15, 31));
        newsym(currentX(), currentY()); /* update position */
        if (otmp->cursed) {
            pline("For some reason, you feel your presence is known.");
            aggravate();
        }
        break;
    case POT_SEE_INVISIBLE: /* tastes like fruit juice in Rogue */
    case POT_FRUIT_JUICE: {
        int msg = Invisible && !Blind;

        unkn++;
        if (otmp->cursed)
            pline("Yecch!  This tastes %s.",
                  Hallucination ? "overripe" : "rotten");
        else
            pline(
                Hallucination
                    ? "This tastes like 10%% real %s%s all-natural beverage."
                    : "This tastes like %s%s.",
                otmp->odiluted ? "reconstituted " : "", fruitname(TRUE));
        if (otmp->otyp == POT_FRUIT_JUICE) {
            increaseCurrentNutrition((otmp->odiluted ? 5 : 10) * (2 + bcsign(otmp)));
            newuhs(FALSE);
            break;
        }
        if (!otmp->cursed) {
            /* Tell them they can see again immediately, which
             * will help them identify the potion...
             */
            make_blinded(0L, TRUE);
        }
        if (otmp->blessed)
            HSee_invisible |= FROMOUTSIDE;
        else
            incrementYourIntrinsicTimeout(SEE_INVIS, rn1(100, 750));
        set_mimic_blocking(); /* do special mimic handling */
        see_monsters();       /* see invisible monsters */
        newsym(currentX(), currentY());   /* see yourself! */
        if (msg && !Blind) {  /* Blind possible if polymorphed */
            You("can see through yourself, but you are visible!");
            unkn--;
        }
        break;
    }
    case POT_PARALYSIS:
        if (Free_action) {
            You("stiffen momentarily.");
        } else {
            if (Levitation || areYouOnAirLevel() || areYouOnWaterLevel())
                You("are motionlessly suspended.");
            else if (u.usteed)
                You("are frozen in place!");
            else
                Your("%s are frozen to the %s!", makeplural(body_part(FOOT)),
                     surface(currentX(), currentY()));
            nomul(-(rn1(10, 25 - 12 * bcsign(otmp))));
            multi_reason = "frozen by a potion";
            nomovemsg = You_can_move_again;
            exercise(A_DEX, FALSE);
        }
        break;
    case POT_SLEEPING:
        if (youResistSleep() || Free_action) {
            You("yawn.");
        } else {
            You("suddenly fall asleep!");
            fall_asleep(-rn1(10, 25 - 12 * bcsign(otmp)), TRUE);
        }
        break;
    case POT_MONSTER_DETECTION:
    case SPE_DETECT_MONSTERS:
        if (otmp->blessed) {
            int x, y;

            if (Detect_monsters)
                nothing++;
            unkn++;
            /* after a while, repeated uses become less effective */
            if ((HDetect_monsters & TIMEOUT) >= 300L)
                i = 1;
            else
                i = rn1(40, 21);
            incrementYourIntrinsicTimeout(DETECT_MONSTERS, i);
            for (x = 1; x < COLNO; x++) {
                for (y = 0; y < ROWNO; y++) {
                    if (levl[x][y].glyph == GLYPH_INVISIBLE) {
                        unmap_object(x, y);
                        newsym(x, y);
                    }
                    if (MON_AT(x, y))
                        unkn = 0;
                }
            }
            see_monsters();
            if (unkn)
                You_feel("lonely.");
            break;
        }
        if (monster_detect(otmp, 0))
            return 1; /* nothing detected */
        exercise(A_WIS, TRUE);
        break;
    case POT_OBJECT_DETECTION:
    case SPE_DETECT_TREASURE:
        if (object_detect(otmp, 0))
            return 1; /* nothing detected */
        exercise(A_WIS, TRUE);
        break;
    case POT_SICKNESS:
        pline("Yecch!  This stuff tastes like poison.");
        if (otmp->blessed) {
            pline("(But in fact it was mildly stale %s.)", fruitname(TRUE));
            if (!Role_if(PM_HEALER)) {
                /* NB: blessed otmp->fromsink is not possible */
                losehp(1, "mildly contaminated potion", KILLED_BY_AN);
            }
        } else {
            if (youResistPoison())
                pline("(But in fact it was biologically contaminated %s.)",
                      fruitname(TRUE));
            if (Role_if(PM_HEALER)) {
                pline("Fortunately, you have been immunized.");
            } else {
                char contaminant[BUFSZ];
                int typ = rn2(A_MAX);

                Sprintf(contaminant, "%s%s",
                        (youResistPoison()) ? "mildly " : "",
                        (otmp->fromsink) ? "contaminated tap water"
                                         : "contaminated potion");
                if (!Fixed_abil) {
                    poisontell(typ, FALSE);
                    (void) adjattrib(typ, youResistPoison() ? -1 : -rn1(4, 3),
                                     1);
                }
                if (!youResistPoison()) {
                    if (otmp->fromsink)
                        losehp(rnd(10) + 5 * !!(otmp->cursed), contaminant,
                               KILLED_BY);
                    else
                        losehp(rnd(10) + 5 * !!(otmp->cursed), contaminant,
                               KILLED_BY_AN);
                } else {
                    /* rnd loss is so that unblessed poorer than blessed */
                    losehp(1 + rn2(2), contaminant,
                           (otmp->fromsink) ? KILLED_BY : KILLED_BY_AN);
                }
                exercise(A_CON, FALSE);
            }
        }
        if (Hallucination) {
            You("are shocked back to your senses!");
            (void) make_hallucinated(0L, FALSE, 0L);
        }
        break;
    case POT_CONFUSION:
        if (!youAreConfused()) {
            if (Hallucination) {
                pline("What a trippy feeling!");
                unkn++;
            } else
                pline("Huh, What?  Where am I?");
        } else
            nothing++;
        make_confused(itimeout_incr(yourIntrinsic(CONFUSION),
                                    rn1(7, 16 - 8 * bcsign(otmp))),
                      FALSE);
        break;
    case POT_GAIN_ABILITY:
        if (otmp->cursed) {
            pline("Ulch!  That potion tasted foul!");
            unkn++;
        } else if (Fixed_abil) {
            nothing++;
        } else {      /* If blessed, increase all; if not, try up to */
            int itmp; /* 6 times to find one which can be increased. */

            i = -1;   /* increment to 0 */
            for (ii = A_MAX; ii > 0; ii--) {
                i = (otmp->blessed ? i + 1 : rn2(A_MAX));
                /* only give "your X is already as high as it can get"
                   message on last attempt (except blessed potions) */
                itmp = (otmp->blessed || ii == 1) ? 0 : -1;
                if (adjattrib(i, 1, itmp) && !otmp->blessed)
                    break;
            }
        }
        break;
    case POT_SPEED:
        if (Wounded_legs && !otmp->cursed && !u.usteed) {
            /* heal_legs() would heal steeds legs */
            heal_legs();
            unkn++;
            break;
        }
        /* FALLTHRU */
    case SPE_HASTE_SELF:
        if (!Very_fast) { /* wwf@doe.carleton.ca */
            You("are suddenly moving %sfaster.", Fast ? "" : "much ");
        } else {
            Your("%s get new energy.", makeplural(body_part(LEG)));
            unkn++;
        }
        exercise(A_DEX, TRUE);
        incrementYourIntrinsicTimeout(FAST, rn1(10, 100 + 60 * bcsign(otmp)));
        break;
    case POT_BLINDNESS:
        if (Blind)
            nothing++;
        make_blinded(itimeout_incr(Blinded,
                                   rn1(200, 250 - 125 * bcsign(otmp))),
                     (boolean) !Blind);
        break;
    case POT_GAIN_LEVEL:
        if (otmp->cursed) {
            unkn++;
            /* they went up a level */
            if ((currentLevelLedgerNum() == 1 && haveSpecialItem(SPECIAL_ITEM_AMULET))
                || canYouRiseUp(currentX(), currentY())) {
                const char *riseup = "rise up, through the %s!";

                if (currentLevelLedgerNum() == 1) {
                    You(riseup, ceiling(currentX(), currentY()));
                    goto_level(&earth_level, FALSE, FALSE, FALSE);
                } else {
                    register int newlev = currentDepth() - 1;
                    d_level newlevel;

                    get_level(&newlevel, newlev);
                    if (areYouOnLevel(&newlevel)) {
                        pline("It tasted bad.");
                        break;
                    } else
                        You(riseup, ceiling(currentX(), currentY()));
                    goto_level(&newlevel, FALSE, FALSE, FALSE);
                }
            } else
                You("have an uneasy feeling.");
            break;
        }
        pluslvl(FALSE);
        /* blessed potions place you at a random spot in the
           middle of the new level instead of the low point */
        if (otmp->blessed)
            setCurrentXP(rndexp(TRUE));
        break;
    case POT_HEALING:
        You_feel("better.");
        healup(d(6 + 2 * bcsign(otmp), 4), !otmp->cursed ? 1 : 0,
               !!otmp->blessed, !otmp->cursed);
        exercise(A_CON, TRUE);
        break;
    case POT_EXTRA_HEALING:
        You_feel("much better.");
        healup(d(6 + 2 * bcsign(otmp), 8),
               otmp->blessed ? 5 : !otmp->cursed ? 2 : 0, !otmp->cursed,
               TRUE);
        (void) make_hallucinated(0L, TRUE, 0L);
        exercise(A_CON, TRUE);
        exercise(A_STR, TRUE);
        break;
    case POT_FULL_HEALING:
        You_feel("completely healed.");
        healup(400, 4 + 4 * bcsign(otmp), !otmp->cursed, TRUE);
        /* Restore one lost level if blessed */
        if (otmp->blessed && currentExperienceLevel() < highestExperienceLevelSoFar()) {
            /* when multiple levels have been lost, drinking
               multiple potions will only get half of them back */
            setHighestExperienceLevelSoFar(highestExperienceLevelSoFar() - 1);
            pluslvl(FALSE);
        }
        (void) make_hallucinated(0L, TRUE, 0L);
        exercise(A_STR, TRUE);
        exercise(A_CON, TRUE);
        break;
    case POT_LEVITATION:
    case SPE_LEVITATION:
        if (otmp->cursed)
            HLevitation &= ~I_SPECIAL;
        if (!Levitation && !BLevitation) {
            /* kludge to ensure proper operation of float_up() */
            setYourIntrinsicTimeout(LEVITATION, 1L);
            float_up();
            /* reverse kludge */
            setYourIntrinsicTimeout(LEVITATION, 0L);
            if (otmp->cursed) {
                if ((currentX() == xupstair && currentY() == yupstair)
                    || (sstairs.up && currentX() == sstairs.sx
                        && currentY() == sstairs.sy)
                    || (xupladder && currentX() == xupladder
                        && currentY() == yupladder)) {
                    (void) doup();
                } else if (currentLevelHasCeiling()) {
                    int dmg = uarmh ? 1 : rnd(10);

                    You("hit your %s on the %s.", body_part(HEAD),
                        ceiling(currentX(), currentY()));
                    losehp(Maybe_Half_Phys(dmg), "colliding with the ceiling",
                           KILLED_BY);
                }
            } /*cursed*/
        } else
            nothing++;
        if (otmp->blessed) {
            incrementYourIntrinsicTimeout(LEVITATION, rn1(50, 250));
            HLevitation |= I_SPECIAL;
        } else
            incrementYourIntrinsicTimeout(LEVITATION, rn1(140, 10));
        if (Levitation)
            spoteffects(FALSE); /* for sinks */
        float_vs_flight();
        break;
    case POT_GAIN_ENERGY: { /* M. Stephenson */
        int num;

        if (otmp->cursed)
            You_feel("lackluster.");
        else
            pline("Magical energies course through your body.");

        /* old: num = rnd(5) + 5 * otmp->blessed + 1;
         *      blessed:  +7..11 max & current (+9 avg)
         *      uncursed: +2.. 6 max & current (+4 avg)
         *      cursed:   -2.. 6 max & current (-4 avg)
         * new: (3.6.0)
         *      blessed:  +3..18 max (+10.5 avg), +9..54 current (+31.5 avg)
         *      uncursed: +2..12 max (+ 7   avg), +6..36 current (+21   avg)
         *      cursed:   -1.. 6 max (- 3.5 avg), -3..18 current (-10.5 avg)
         */
        num = d(otmp->blessed ? 3 : !otmp->cursed ? 2 : 1, 6);
        if (otmp->cursed)
            num = -num; /* subtract instead of add when cursed */
        increaseMaximumMagicalEnergy(num);
        if (maximumMagicalEnergy() <= 0)
            setMaximumMagicalEnergy(0);
        increaseCurrentMagicalEnergy(3 * num);
        if (currentMagicalEnergy() > maximumMagicalEnergy())
            setCurrentMagicalEnergy(maximumMagicalEnergy());
        else if (currentMagicalEnergy() <= 0)
            setCurrentMagicalEnergy(0);
        context.botl = 1;
        exercise(A_WIS, TRUE);
        break;
    }
    case POT_OIL: { /* P. Winner */
        boolean good_for_you = FALSE;

        if (otmp->lamplit) {
            if (likes_fire(youmonst.data)) {
                pline("Ahh, a refreshing drink.");
                good_for_you = TRUE;
            } else {
                You("burn your %s.", body_part(FACE));
                /* fire damage */
                losehp(d(youResistFire() ? 1 : 3, 4), "burning potion of oil",
                       KILLED_BY_AN);
            }
        } else if (otmp->cursed)
            pline("This tastes like castor oil.");
        else
            pline("That was smooth!");
        exercise(A_WIS, good_for_you);
        break;
    }
    case POT_ACID:
        if (youResistAcid()) {
            /* Not necessarily a creature who _likes_ acid */
            pline("This tastes %s.", Hallucination ? "tangy" : "sour");
        } else {
            int dmg;

            pline("This burns%s!",
                  otmp->blessed ? " a little" : otmp->cursed ? " a lot"
                                                             : " like acid");
            dmg = d(otmp->cursed ? 2 : 1, otmp->blessed ? 4 : 8);
            losehp(Maybe_Half_Phys(dmg), "potion of acid", KILLED_BY_AN);
            exercise(A_CON, FALSE);
        }
        if (Stoned)
            fix_petrification();
        unkn++; /* holy/unholy water can burn like acid too */
        break;
    case POT_POLYMORPH:
        You_feel("a little %s.", Hallucination ? "normal" : "strange");
        if (!Unchanging)
            polyself(0);
        break;
    default:
        impossible("What a funny potion! (%u)", otmp->otyp);
        return 0;
    }
    return -1;
}

void
healup(nhp, nxtra, curesick, cureblind)
int nhp, nxtra;
register boolean curesick, cureblind;
{
    if (nhp) {
        if (areYouPolymorphed()) {
            increaseCurrentHitPointsAsMonster(nhp);
            if (currentHitPointsAsMonster() > maximumHitPointsAsMonster()) {
                increaseMaximumHitPointsAsMonster(nxtra);
                setCurrentHitPointsAsMonster(maximumHitPointsAsMonster());
            }
        } else {
            increaseCurrentHitPoints(nhp);
            if (currentHitPoints() > maximumHitPoints()) {
                increaseMaximumHitPoints(nxtra);
                setCurrentHitPoints(maximumHitPoints());
            }
        }
    }
    if (cureblind)
        make_blinded(0L, TRUE);
    if (curesick) {
        make_vomiting(0L, TRUE);
        make_sick(0L, (char *) 0, TRUE, SICK_ALL);
    }
    context.botl = 1;
    return;
}

void
strange_feeling(obj, txt)
struct obj *obj;
const char *txt;
{
    if (flags.beginner || !txt)
        You("have a %s feeling for a moment, then it passes.",
            Hallucination ? "normal" : "strange");
    else
        pline1(txt);

    if (!obj) /* e.g., crystal ball finds no traps */
        return;

    if (obj->dknown && !objects[obj->otyp].oc_name_known
        && !objects[obj->otyp].oc_uname)
        docall(obj);

    useup(obj);
}

const char *bottlenames[] = { "bottle", "phial", "flagon", "carafe",
                              "flask",  "jar",   "vial" };

const char *
bottlename()
{
    return bottlenames[rn2(SIZE(bottlenames))];
}

/* handle item dipped into water potion or steed saddle splashed by same */
STATIC_OVL boolean
H2Opotion_dip(potion, targobj, useeit, objphrase)
struct obj *potion, *targobj;
boolean useeit;
const char *objphrase; /* "Your widget glows" or "Steed's saddle glows" */
{
    void FDECL((*func), (OBJ_P)) = 0;
    const char *glowcolor = 0;
#define COST_alter (-2)
#define COST_none (-1)
    int costchange = COST_none;
    boolean altfmt = FALSE, res = FALSE;

    if (!potion || potion->otyp != POT_WATER)
        return FALSE;

    if (potion->blessed) {
        if (targobj->cursed) {
            func = uncurse;
            glowcolor = NH_AMBER;
            costchange = COST_UNCURS;
        } else if (!targobj->blessed) {
            func = bless;
            glowcolor = NH_LIGHT_BLUE;
            costchange = COST_alter;
            altfmt = TRUE; /* "with a <color> aura" */
        }
    } else if (potion->cursed) {
        if (targobj->blessed) {
            func = unbless;
            glowcolor = "brown";
            costchange = COST_UNBLSS;
        } else if (!targobj->cursed) {
            func = curse;
            glowcolor = NH_BLACK;
            costchange = COST_alter;
            altfmt = TRUE;
        }
    } else {
        /* dipping into uncursed water; carried() check skips steed saddle */
        if (carried(targobj)) {
            if (water_damage(targobj, 0, TRUE) != ER_NOTHING)
                res = TRUE;
        }
    }
    if (func) {
        /* give feedback before altering the target object;
           this used to set obj->bknown even when not seeing
           the effect; now hero has to see the glow, and bknown
           is cleared instead of set if perception is distorted */
        if (useeit) {
            glowcolor = hcolor(glowcolor);
            if (altfmt)
                pline("%s with %s aura.", objphrase, an(glowcolor));
            else
                pline("%s %s.", objphrase, glowcolor);
            iflags.last_msg = PLNMSG_OBJ_GLOWS;
            targobj->bknown = !Hallucination;
        }
        /* potions of water are the only shop goods whose price depends
           on their curse/bless state */
        if (targobj->unpaid && targobj->otyp == POT_WATER) {
            if (costchange == COST_alter)
                /* added blessing or cursing; update shop
                   bill to reflect item's new higher price */
                alter_cost(targobj, 0L);
            else if (costchange != COST_none)
                /* removed blessing or cursing; you
                   degraded it, now you'll have to buy it... */
                costly_alteration(targobj, costchange);
        }
        /* finally, change curse/bless state */
        (*func)(targobj);
        res = TRUE;
    }
    return res;
}

void
potionhit(mon, obj, your_fault)
register struct monst *mon;
register struct obj *obj;
boolean your_fault;
{
    const char *botlnam = bottlename();
    boolean isyou = (mon == &youmonst);
    int distance;
    struct obj *saddle = (struct obj *) 0;
    boolean hit_saddle = FALSE;

    if (isyou) {
        distance = 0;
        pline_The("%s crashes on your %s and breaks into shards.", botlnam,
                  body_part(HEAD));
        losehp(Maybe_Half_Phys(rnd(2)), "thrown potion", KILLED_BY_AN);
    } else {
        /* sometimes it hits the saddle */
        if (((mon->misc_worn_check & W_SADDLE)
             && (saddle = which_armor(mon, W_SADDLE)))
            && (!rn2(10)
                || (obj->otyp == POT_WATER
                    && ((rnl(10) > 7 && obj->cursed)
                        || (rnl(10) < 4 && obj->blessed) || !rn2(3)))))
            hit_saddle = TRUE;
        distance = distanceSquaredToYou(mon->mx, mon->my);
        if (!cansee(mon->mx, mon->my))
            pline("Crash!");
        else {
            char *mnam = mon_nam(mon);
            char buf[BUFSZ];

            if (hit_saddle && saddle) {
                Sprintf(buf, "%s saddle",
                        s_suffix(x_monnam(mon, ARTICLE_THE, (char *) 0,
                                          (SUPPRESS_IT | SUPPRESS_SADDLE),
                                          FALSE)));
            } else if (has_head(mon->data)) {
                Sprintf(buf, "%s %s", s_suffix(mnam),
                        (notonhead ? "body" : "head"));
            } else {
                Strcpy(buf, mnam);
            }
            pline_The("%s crashes on %s and breaks into shards.", botlnam,
                      buf);
        }
        if (rn2(5) && mon->mhp > 1 && !hit_saddle)
            mon->mhp--;
    }

    /* oil doesn't instantly evaporate; Neither does a saddle hit */
    if (obj->otyp != POT_OIL && !hit_saddle && cansee(mon->mx, mon->my))
        pline("%s.", Tobjnam(obj, "evaporate"));

    if (isyou) {
        switch (obj->otyp) {
        case POT_OIL:
            if (obj->lamplit)
                explode_oil(obj, currentX(), currentY());
            break;
        case POT_POLYMORPH:
            You_feel("a little %s.", Hallucination ? "normal" : "strange");
            if (!Unchanging && !youResistMagic())
                polyself(0);
            break;
        case POT_ACID:
            if (!youResistAcid()) {
                int dmg;
                pline("This burns%s!",
                      obj->blessed ? " a little"
                                   : obj->cursed ? " a lot" : "");
                dmg = d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8);
                losehp(Maybe_Half_Phys(dmg), "potion of acid", KILLED_BY_AN);
            }
            break;
        }
    } else if (hit_saddle && saddle) {
        char *mnam, buf[BUFSZ], saddle_glows[BUFSZ];
        boolean affected = FALSE;
        boolean useeit = !Blind && canseemon(mon) && cansee(mon->mx, mon->my);

        mnam = x_monnam(mon, ARTICLE_THE, (char *) 0,
                        (SUPPRESS_IT | SUPPRESS_SADDLE), FALSE);
        Sprintf(buf, "%s", upstart(s_suffix(mnam)));

        switch (obj->otyp) {
        case POT_WATER:
            Sprintf(saddle_glows, "%s %s", buf, aobjnam(saddle, "glow"));
            affected = H2Opotion_dip(obj, saddle, useeit, saddle_glows);
            break;
        case POT_POLYMORPH:
            /* Do we allow the saddle to polymorph? */
            break;
        }
        if (useeit && !affected)
            pline("%s %s wet.", buf, aobjnam(saddle, "get"));
    } else {
        boolean angermon = TRUE;

        if (!your_fault)
            angermon = FALSE;
        switch (obj->otyp) {
        case POT_HEALING:
        case POT_EXTRA_HEALING:
        case POT_FULL_HEALING:
            if (mon->data == &mons[PM_PESTILENCE])
                goto do_illness;
        /*FALLTHRU*/
        case POT_RESTORE_ABILITY:
        case POT_GAIN_ABILITY:
        do_healing:
            angermon = FALSE;
            if (mon->mhp < mon->mhpmax) {
                mon->mhp = mon->mhpmax;
                if (canseemon(mon))
                    pline("%s looks sound and hale again.", Monnam(mon));
            }
            break;
        case POT_SICKNESS:
            if (mon->data == &mons[PM_PESTILENCE])
                goto do_healing;
            if (dmgtype(mon->data, AD_DISE)
                /* won't happen, see prior goto */
                || dmgtype(mon->data, AD_PEST)
                /* most common case */
                || resists_poison(mon)) {
                if (canseemon(mon))
                    pline("%s looks unharmed.", Monnam(mon));
                break;
            }
        do_illness:
            if ((mon->mhpmax > 3) && !resist(mon, POTION_CLASS, 0, NOTELL))
                mon->mhpmax /= 2;
            if ((mon->mhp > 2) && !resist(mon, POTION_CLASS, 0, NOTELL))
                mon->mhp /= 2;
            if (mon->mhp > mon->mhpmax)
                mon->mhp = mon->mhpmax;
            if (canseemon(mon))
                pline("%s looks rather ill.", Monnam(mon));
            break;
        case POT_CONFUSION:
        case POT_BOOZE:
            if (!resist(mon, POTION_CLASS, 0, NOTELL))
                mon->mconf = TRUE;
            break;
        case POT_INVISIBILITY:
            angermon = FALSE;
            mon_set_minvis(mon);
            break;
        case POT_SLEEPING:
            /* wakeup() doesn't rouse victims of temporary sleep */
            if (sleep_monst(mon, rnd(12), POTION_CLASS)) {
                pline("%s falls asleep.", Monnam(mon));
                slept_monst(mon);
            }
            break;
        case POT_PARALYSIS:
            if (mon->mcanmove) {
                /* really should be rnd(5) for consistency with players
                 * breathing potions, but...
                 */
                paralyze_monst(mon, rnd(25));
            }
            break;
        case POT_SPEED:
            angermon = FALSE;
            mon_adjust_speed(mon, 1, obj);
            break;
        case POT_BLINDNESS:
            if (haseyes(mon->data)) {
                register int btmp = 64 + rn2(32)
                            + rn2(32) * !resist(mon, POTION_CLASS, 0, NOTELL);

                btmp += mon->mblinded;
                mon->mblinded = min(btmp, 127);
                mon->mcansee = 0;
            }
            break;
        case POT_WATER:
            if (is_undead(mon->data) || is_demon(mon->data)
                || is_were(mon->data) || is_vampshifter(mon)) {
                if (obj->blessed) {
                    pline("%s %s in pain!", Monnam(mon),
                          is_silent(mon->data) ? "writhes" : "shrieks");
                    if (!is_silent(mon->data))
                        wake_nearto(mon->mx, mon->my, mon->data->mlevel * 10);
                    mon->mhp -= d(2, 6);
                    /* should only be by you */
                    if (mon->mhp < 1)
                        killed(mon);
                    else if (is_were(mon->data) && !is_human(mon->data))
                        new_were(mon); /* revert to human */
                } else if (obj->cursed) {
                    angermon = FALSE;
                    if (canseemon(mon))
                        pline("%s looks healthier.", Monnam(mon));
                    mon->mhp += d(2, 6);
                    if (mon->mhp > mon->mhpmax)
                        mon->mhp = mon->mhpmax;
                    if (is_were(mon->data) && is_human(mon->data)
                        && !Protection_from_shape_changers)
                        new_were(mon); /* transform into beast */
                }
            } else if (mon->data == &mons[PM_GREMLIN]) {
                angermon = FALSE;
                (void) split_mon(mon, (struct monst *) 0);
            } else if (mon->data == &mons[PM_IRON_GOLEM]) {
                if (canseemon(mon))
                    pline("%s rusts.", Monnam(mon));
                mon->mhp -= d(1, 6);
                /* should only be by you */
                if (mon->mhp < 1)
                    killed(mon);
            }
            break;
        case POT_OIL:
            if (obj->lamplit)
                explode_oil(obj, mon->mx, mon->my);
            break;
        case POT_ACID:
            if (!resists_acid(mon) && !resist(mon, POTION_CLASS, 0, NOTELL)) {
                pline("%s %s in pain!", Monnam(mon),
                      is_silent(mon->data) ? "writhes" : "shrieks");
                if (!is_silent(mon->data))
                    wake_nearto(mon->mx, mon->my, mon->data->mlevel * 10);
                mon->mhp -= d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8);
                if (mon->mhp < 1) {
                    if (your_fault)
                        killed(mon);
                    else
                        monkilled(mon, "", AD_ACID);
                }
            }
            break;
        case POT_POLYMORPH:
            (void) bhitm(mon, obj);
            break;
        /*
        case POT_GAIN_LEVEL:
        case POT_LEVITATION:
        case POT_FRUIT_JUICE:
        case POT_MONSTER_DETECTION:
        case POT_OBJECT_DETECTION:
            break;
        */
        }
        if (angermon)
            wakeup(mon);
        else
            mon->msleeping = 0;
    }

    /* Note: potionbreathe() does its own docall() */
    if ((distance == 0 || ((distance < 3) && rn2(5)))
        && (!breathless(youmonst.data) || haseyes(youmonst.data)))
        potionbreathe(obj);
    else if (obj->dknown && !objects[obj->otyp].oc_name_known
             && !objects[obj->otyp].oc_uname && cansee(mon->mx, mon->my))
        docall(obj);
    if (currentlyOccupiedShops(0) && obj->unpaid) {
        struct monst *shkp = shop_keeper(*in_rooms(currentX(), currentY(), SHOPBASE));

        if (shkp)
            (void) stolen_value(obj, currentX(), currentY(), (boolean) shkp->mpeaceful,
                                FALSE);
        else
            obj->unpaid = 0;
    }
    obfree(obj, (struct obj *) 0);
}

/* vapors are inhaled or get in your eyes */
void
potionbreathe(obj)
register struct obj *obj;
{
    register int i, ii, isdone, kn = 0;

    switch (obj->otyp) {
    case POT_RESTORE_ABILITY:
    case POT_GAIN_ABILITY:
        if (obj->cursed) {
            if (!breathless(youmonst.data))
                pline("Ulch!  That potion smells terrible!");
            else if (haseyes(youmonst.data)) {
                const char *eyes = body_part(EYE);

                if (eyecount(youmonst.data) != 1)
                    eyes = makeplural(eyes);
                Your("%s %s!", eyes, vtense(eyes, "sting"));
            }
            break;
        } else {
            i = rn2(A_MAX); /* start at a random point */
            for (isdone = ii = 0; !isdone && ii < A_MAX; ii++) {
                if (yourCurrentAttr(i) < yourAttrMax(i)) {
                    increaseYourCurrentAttr(i, 1);
                    /* only first found if not blessed */
                    isdone = !(obj->blessed);
                    context.botl = 1;
                }
                if (++i >= A_MAX)
                    i = 0;
            }
        }
        break;
    case POT_FULL_HEALING:
        if (areYouPolymorphed() && currentHitPointsAsMonster() < maximumHitPointsAsMonster()) {
            increaseCurrentHitPointsAsMonster(1);
            context.botl = 1;
        }
        if (currentHitPoints() < maximumHitPoints()) {
            increaseCurrentHitPoints(1);
            context.botl = 1;
        }
        /*FALLTHRU*/
    case POT_EXTRA_HEALING:
        if (areYouPolymorphed() && currentHitPointsAsMonster() < maximumHitPointsAsMonster()) {
            increaseCurrentHitPointsAsMonster(1);
            context.botl = 1;
        }
        if (currentHitPoints() < maximumHitPoints()) {
            increaseCurrentHitPoints(1);
            context.botl = 1;
        }
        /*FALLTHRU*/
    case POT_HEALING:
        if (areYouPolymorphed() && currentHitPointsAsMonster() < maximumHitPointsAsMonster()) {
            increaseCurrentHitPointsAsMonster(1);
            context.botl = 1;
        }
        if (currentHitPoints() < maximumHitPoints()) {
            increaseCurrentHitPoints(1);
            context.botl = 1;
        }
        exercise(A_CON, TRUE);
        break;
    case POT_SICKNESS:
        if (!Role_if(PM_HEALER)) {
            if (areYouPolymorphed()) {
                if (currentHitPointsAsMonster() <= 5)
                    setCurrentHitPointsAsMonster(1);
                else
                    decreaseCurrentHitPointsAsMonster(5);
            } else {
                if (currentHitPoints() <= 5)
                    setCurrentHitPoints(1);
                else
                    decreaseCurrentHitPoints(5);
            }
            context.botl = 1;
            exercise(A_CON, FALSE);
        }
        break;
    case POT_HALLUCINATION:
        You("have a momentary vision.");
        break;
    case POT_CONFUSION:
    case POT_BOOZE:
        if (!youAreConfused())
            You_feel("somewhat dizzy.");
        make_confused(itimeout_incr(yourIntrinsic(CONFUSION), rnd(5)), FALSE);
        break;
    case POT_INVISIBILITY:
        if (!Blind && !Invis) {
            kn++;
            pline("For an instant you %s!",
                  See_invisible ? "could see right through yourself"
                                : "couldn't see yourself");
        }
        break;
    case POT_PARALYSIS:
        kn++;
        if (!Free_action) {
            pline("%s seems to be holding you.", Something);
            nomul(-rnd(5));
            multi_reason = "frozen by a potion";
            nomovemsg = You_can_move_again;
            exercise(A_DEX, FALSE);
        } else
            You("stiffen momentarily.");
        break;
    case POT_SLEEPING:
        kn++;
        if (!Free_action && !youResistSleep()) {
            You_feel("rather tired.");
            nomul(-rnd(5));
            multi_reason = "sleeping off a magical draught";
            nomovemsg = You_can_move_again;
            exercise(A_DEX, FALSE);
        } else
            You("yawn.");
        break;
    case POT_SPEED:
        if (!Fast)
            Your("knees seem more flexible now.");
        incrementYourIntrinsicTimeout(FAST, rnd(5));
        exercise(A_DEX, TRUE);
        break;
    case POT_BLINDNESS:
        if (!Blind && !youAreUnaware()) {
            kn++;
            pline("It suddenly gets dark.");
        }
        make_blinded(itimeout_incr(Blinded, rnd(5)), FALSE);
        if (!Blind && youAreAware())
            Your1(vision_clears);
        break;
    case POT_WATER:
        if (currentMonsterNumber() == PM_GREMLIN) {
            (void) split_mon(&youmonst, (struct monst *) 0);
        } else if (lycanthropeType() >= LOW_PM) {
            /* vapor from [un]holy water will trigger
               transformation but won't cure lycanthropy */
            if (obj->blessed && youmonst.data == &mons[lycanthropeType()])
                you_unwere(FALSE);
            else if (obj->cursed && !areYouPolymorphed())
                you_were();
        }
        break;
    case POT_ACID:
    case POT_POLYMORPH:
        exercise(A_CON, FALSE);
        break;
    /*
    case POT_GAIN_LEVEL:
    case POT_LEVITATION:
    case POT_FRUIT_JUICE:
    case POT_MONSTER_DETECTION:
    case POT_OBJECT_DETECTION:
    case POT_OIL:
        break;
     */
    }
    /* note: no obfree() */
    if (obj->dknown) {
        if (kn)
            makeknown(obj->otyp);
        else if (!objects[obj->otyp].oc_name_known
                 && !objects[obj->otyp].oc_uname)
            docall(obj);
    }
}

/* returns the potion type when o1 is dipped in o2 */
STATIC_OVL short
mixtype(o1, o2)
register struct obj *o1, *o2;
{
    /* cut down on the number of cases below */
    if (o1->oclass == POTION_CLASS
        && (o2->otyp == POT_GAIN_LEVEL || o2->otyp == POT_GAIN_ENERGY
            || o2->otyp == POT_HEALING || o2->otyp == POT_EXTRA_HEALING
            || o2->otyp == POT_FULL_HEALING || o2->otyp == POT_ENLIGHTENMENT
            || o2->otyp == POT_FRUIT_JUICE)) {
        struct obj *swp;

        swp = o1;
        o1 = o2;
        o2 = swp;
    }

    switch (o1->otyp) {
    case POT_HEALING:
        switch (o2->otyp) {
        case POT_SPEED:
        case POT_GAIN_LEVEL:
        case POT_GAIN_ENERGY:
            return POT_EXTRA_HEALING;
        }
    case POT_EXTRA_HEALING:
        switch (o2->otyp) {
        case POT_GAIN_LEVEL:
        case POT_GAIN_ENERGY:
            return POT_FULL_HEALING;
        }
    case POT_FULL_HEALING:
        switch (o2->otyp) {
        case POT_GAIN_LEVEL:
        case POT_GAIN_ENERGY:
            return POT_GAIN_ABILITY;
        }
    case UNICORN_HORN:
        switch (o2->otyp) {
        case POT_SICKNESS:
            return POT_FRUIT_JUICE;
        case POT_HALLUCINATION:
        case POT_BLINDNESS:
        case POT_CONFUSION:
            return POT_WATER;
        }
        break;
    case AMETHYST: /* "a-methyst" == "not intoxicated" */
        if (o2->otyp == POT_BOOZE)
            return POT_FRUIT_JUICE;
        break;
    case POT_GAIN_LEVEL:
    case POT_GAIN_ENERGY:
        switch (o2->otyp) {
        case POT_CONFUSION:
            return (rn2(3) ? POT_BOOZE : POT_ENLIGHTENMENT);
        case POT_HEALING:
            return POT_EXTRA_HEALING;
        case POT_EXTRA_HEALING:
            return POT_FULL_HEALING;
        case POT_FULL_HEALING:
            return POT_GAIN_ABILITY;
        case POT_FRUIT_JUICE:
            return POT_SEE_INVISIBLE;
        case POT_BOOZE:
            return POT_HALLUCINATION;
        }
        break;
    case POT_FRUIT_JUICE:
        switch (o2->otyp) {
        case POT_SICKNESS:
            return POT_SICKNESS;
        case POT_SPEED:
            return POT_BOOZE;
        case POT_GAIN_LEVEL:
        case POT_GAIN_ENERGY:
            return POT_SEE_INVISIBLE;
        }
        break;
    case POT_ENLIGHTENMENT:
        switch (o2->otyp) {
        case POT_LEVITATION:
            if (rn2(3))
                return POT_GAIN_LEVEL;
            break;
        case POT_FRUIT_JUICE:
            return POT_BOOZE;
        case POT_BOOZE:
            return POT_CONFUSION;
        }
        break;
    }

    return 0;
}

/* #dip command */
int
dodip()
{
    register struct obj *potion, *obj;
    struct obj *singlepotion;
    uchar here;
    char allowall[2];
    short mixture;
    char qbuf[QBUFSZ], qtoo[QBUFSZ];

    allowall[0] = ALL_CLASSES;
    allowall[1] = '\0';
    if (!(obj = getobj(allowall, "dip")))
        return 0;
    if (inaccessible_equipment(obj, "dip", FALSE))
        return 0;

    Sprintf(qbuf, "dip %s into", thesimpleoname(obj));
    here = levl[currentX()][currentY()].typ;
    /* Is there a fountain to dip into here? */
    if (IS_FOUNTAIN(here)) {
        /* "Dip <the object> into the fountain?" */
        Sprintf(qtoo, "%s the fountain?", qbuf);
        if (yn(upstart(qtoo)) == 'y') {
            dipfountain(obj);
            return 1;
        }
    } else if (is_pool(currentX(), currentY())) {
        const char *pooltype = waterbody_name(currentX(), currentY());

        /* "Dip <the object> into the {pool, moat, &c}?" */
        Sprintf(qtoo, "%s the %s?", qbuf, pooltype);
        if (yn(upstart(qtoo)) == 'y') {
            if (Levitation) {
                floating_above(pooltype);
            } else if (u.usteed && !is_swimmer(u.usteed->data)
                       && P_SKILL(P_RIDING) < P_BASIC) {
                rider_cant_reach(); /* not skilled enough to reach */
            } else {
                if (obj->otyp == POT_ACID)
                    obj->in_use = 1;
                if (water_damage(obj, 0, TRUE) != ER_DESTROYED && obj->in_use)
                    useup(obj);
            }
            return 1;
        }
    }

    /* "What do you want to dip <the object> into?" */
    potion = getobj(beverages, qbuf); /* "dip into" */
    if (!potion)
        return 0;
    if (potion == obj && potion->quan == 1L) {
        pline("That is a potion bottle, not a Klein bottle!");
        return 0;
    }
    potion->in_use = TRUE; /* assume it will be used up */
    if (potion->otyp == POT_WATER) {
        boolean useeit = !Blind || (obj == ublindf && Blindfolded_only);
        const char *obj_glows = Yobjnam2(obj, "glow");

        if (H2Opotion_dip(potion, obj, useeit, obj_glows))
            goto poof;
    } else if (obj->otyp == POT_POLYMORPH || potion->otyp == POT_POLYMORPH) {
        /* some objects can't be polymorphed */
        if (obj->otyp == potion->otyp /* both POT_POLY */
            || obj->otyp == WAN_POLYMORPH || obj->otyp == SPE_POLYMORPH
            || obj == uball || obj == uskin
            || obj_resists(obj->otyp == POT_POLYMORPH ? potion : obj,
                           5, 95)) {
            pline1(nothing_happens);
        } else {
            boolean was_wep, was_swapwep, was_quiver;
            short save_otyp = obj->otyp;

            /* KMH, conduct */
            incrementPolypileCount(1);

            was_wep = (obj == uwep);
            was_swapwep = (obj == uswapwep);
            was_quiver = (obj == uquiver);

            obj = poly_obj(obj, STRANGE_OBJECT);

            if (was_wep)
                setuwep(obj);
            else if (was_swapwep)
                setuswapwep(obj);
            else if (was_quiver)
                setuqwep(obj);

            if (obj->otyp != save_otyp) {
                makeknown(POT_POLYMORPH);
                useup(potion);
                prinv((char *) 0, obj, 0L);
                return 1;
            } else {
                pline("Nothing seems to happen.");
                goto poof;
            }
        }
        potion->in_use = FALSE; /* didn't go poof */
        return 1;
    } else if (obj->oclass == POTION_CLASS && obj->otyp != potion->otyp) {
        long amt = obj->quan;

        Strcpy(qbuf, "The");
        if (amt > (objects[potion->otyp].oc_magic ? 2L : 9L)) {
            /* trying to dip multiple potions will usually affect only a
               subset; pick an amount between 2 and min(N,9), inclusive */
            amt -= 1L;
            do {
                amt = (long) rnd((int) amt);
            } while (amt >= 9L);
            amt += 1L;
            if (amt < obj->quan) {
                obj = splitobj(obj, amt);
                Sprintf(qbuf, "%ld of the", obj->quan);
            }
        }
        /* [N of] the {obj(s)} mix(es) with [one of] {the potion}... */
        pline("%s %s %s with %s%s...", qbuf, simpleonames(obj),
              otense(obj, "mix"), (potion->quan > 1L) ? "one of " : "",
              thesimpleoname(potion));
        /* Mixing potions is dangerous...
           KMH, balance patch -- acid is particularly unstable */
        if (obj->cursed || obj->otyp == POT_ACID || !rn2(10)) {
            /* it would be better to use up the whole stack in advance
               of the message, but we can't because we need to keep it
               around for potionbreathe() [and we can't set obj->in_use
               to 'amt' because that's not implemented] */
            obj->in_use = 1;
            pline("BOOM!  They explode!");
            wake_nearto(currentX(), currentY(), (BOLT_LIM + 1) * (BOLT_LIM + 1));
            exercise(A_STR, FALSE);
            if (!breathless(youmonst.data) || haseyes(youmonst.data))
                potionbreathe(obj);
            useupall(obj);
            useup(potion);
            losehp((int) (amt + rnd(9)), /* not physical damage */
                   "alchemic blast", KILLED_BY_AN);
            return 1;
        }

        obj->blessed = obj->cursed = obj->bknown = 0;
        if (Blind || Hallucination)
            obj->dknown = 0;

        if ((mixture = mixtype(obj, potion)) != 0) {
            obj->otyp = mixture;
        } else {
            switch (obj->odiluted ? 1 : rnd(8)) {
            case 1:
                obj->otyp = POT_WATER;
                break;
            case 2:
            case 3:
                obj->otyp = POT_SICKNESS;
                break;
            case 4: {
                struct obj *otmp = mkobj(POTION_CLASS, FALSE);

                obj->otyp = otmp->otyp;
                obfree(otmp, (struct obj *) 0);
                break;
            }
            default:
                useupall(obj);
                useup(potion);
                if (!Blind)
                    pline_The("mixture glows brightly and evaporates.");
                return 1;
            }
        }
        obj->odiluted = (obj->otyp != POT_WATER);

        if (obj->otyp == POT_WATER && !Hallucination) {
            pline_The("mixture bubbles%s.", Blind ? "" : ", then clears");
        } else if (!Blind) {
            pline_The("mixture looks %s.",
                      hcolor(OBJ_DESCR(objects[obj->otyp])));
        }

        useup(potion);
        /* this is required when 'obj' was split off from a bigger stack,
           so that 'obj' will now be assigned its own inventory slot;
           it has a side-effect of merging 'obj' into another compatible
           stack if there is one, so we do it even when no split has
           been made in order to get the merge result for both cases;
           as a consequence, mixing while Fumbling drops the mixture */
        freeinv(obj);
        (void) hold_another_object(obj, "You drop %s!", doname(obj),
                                   (const char *) 0);
        return 1;
    }

    if (potion->otyp == POT_ACID && obj->otyp == CORPSE
        && obj->corpsenm == PM_LICHEN && !Blind) {
        pline("%s %s %s around the edges.", The(cxname(obj)),
              otense(obj, "turn"),
              potion->odiluted ? hcolor(NH_ORANGE) : hcolor(NH_RED));
        potion->in_use = FALSE; /* didn't go poof */
        return 1;
    }

    if (potion->otyp == POT_WATER && obj->otyp == TOWEL) {
        pline_The("towel soaks it up!");
        /* wetting towel already done via water_damage() in H2Opotion_dip */
        goto poof;
    }

    if (is_poisonable(obj)) {
        if (potion->otyp == POT_SICKNESS && !obj->opoisoned) {
            char buf[BUFSZ];

            if (potion->quan > 1L)
                Sprintf(buf, "One of %s", the(xname(potion)));
            else
                Strcpy(buf, The(xname(potion)));
            pline("%s forms a coating on %s.", buf, the(xname(obj)));
            obj->opoisoned = TRUE;
            goto poof;
        } else if (obj->opoisoned && (potion->otyp == POT_HEALING
                                      || potion->otyp == POT_EXTRA_HEALING
                                      || potion->otyp == POT_FULL_HEALING)) {
            pline("A coating wears off %s.", the(xname(obj)));
            obj->opoisoned = 0;
            goto poof;
        }
    }

    if (potion->otyp == POT_ACID) {
        if (erode_obj(obj, 0, ERODE_CORRODE, EF_GREASE) != ER_NOTHING)
            goto poof;
    }

    if (potion->otyp == POT_OIL) {
        boolean wisx = FALSE;

        if (potion->lamplit) { /* burning */
            fire_damage(obj, TRUE, currentX(), currentY());
        } else if (potion->cursed) {
            pline_The("potion spills and covers your %s with oil.",
                      makeplural(body_part(FINGER)));
            incrementYourIntrinsicTimeout(GLIB, d(2, 10));
        } else if (obj->oclass != WEAPON_CLASS && !is_weptool(obj)) {
            /* the following cases apply only to weapons */
            goto more_dips;
            /* Oil removes rust and corrosion, but doesn't unburn.
             * Arrows, etc are classed as metallic due to arrowhead
             * material, but dipping in oil shouldn't repair them.
             */
        } else if ((!is_rustprone(obj) && !is_corrodeable(obj))
                   || is_ammo(obj) || (!obj->oeroded && !obj->oeroded2)) {
            /* uses up potion, doesn't set obj->greased */
            pline("%s %s with an oily sheen.", Yname2(obj),
                  otense(obj, "gleam"));
        } else {
            pline("%s %s less %s.", Yname2(obj), otense(obj, "are"),
                  (obj->oeroded && obj->oeroded2)
                      ? "corroded and rusty"
                      : obj->oeroded ? "rusty" : "corroded");
            if (obj->oeroded > 0)
                obj->oeroded--;
            if (obj->oeroded2 > 0)
                obj->oeroded2--;
            wisx = TRUE;
        }
        exercise(A_WIS, wisx);
        makeknown(potion->otyp);
        useup(potion);
        return 1;
    }
more_dips:

    /* Allow filling of MAGIC_LAMPs to prevent identification by player */
    if ((obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP)
        && (potion->otyp == POT_OIL)) {
        /* Turn off engine before fueling, turn off fuel too :-)  */
        if (obj->lamplit || potion->lamplit) {
            useup(potion);
            explode(currentX(), currentY(), 11, d(6, 6), 0, EXPL_FIERY);
            exercise(A_WIS, FALSE);
            return 1;
        }
        /* Adding oil to an empty magic lamp renders it into an oil lamp */
        if ((obj->otyp == MAGIC_LAMP) && obj->spe == 0) {
            obj->otyp = OIL_LAMP;
            obj->age = 0;
        }
        if (obj->age > 1000L) {
            pline("%s %s full.", Yname2(obj), otense(obj, "are"));
            potion->in_use = FALSE; /* didn't go poof */
        } else {
            You("fill %s with oil.", yname(obj));
            check_unpaid(potion);        /* Yendorian Fuel Tax */
            obj->age += 2 * potion->age; /* burns more efficiently */
            if (obj->age > 1500L)
                obj->age = 1500L;
            useup(potion);
            exercise(A_WIS, TRUE);
        }
        makeknown(POT_OIL);
        obj->spe = 1;
        update_inventory();
        return 1;
    }

    potion->in_use = FALSE; /* didn't go poof */
    if ((obj->otyp == UNICORN_HORN || obj->otyp == AMETHYST)
        && (mixture = mixtype(obj, potion)) != 0) {
        char oldbuf[BUFSZ], newbuf[BUFSZ];
        short old_otyp = potion->otyp;
        boolean old_dknown = FALSE;
        boolean more_than_one = potion->quan > 1L;

        oldbuf[0] = '\0';
        if (potion->dknown) {
            old_dknown = TRUE;
            Sprintf(oldbuf, "%s ", hcolor(OBJ_DESCR(objects[potion->otyp])));
        }
        /* with multiple merged potions, split off one and
           just clear it */
        if (potion->quan > 1L) {
            singlepotion = splitobj(potion, 1L);
        } else
            singlepotion = potion;

        costly_alteration(singlepotion, COST_NUTRLZ);
        singlepotion->otyp = mixture;
        singlepotion->blessed = 0;
        if (mixture == POT_WATER)
            singlepotion->cursed = singlepotion->odiluted = 0;
        else
            singlepotion->cursed = obj->cursed; /* odiluted left as-is */
        singlepotion->bknown = FALSE;
        if (Blind) {
            singlepotion->dknown = FALSE;
        } else {
            singlepotion->dknown = !Hallucination;
            if (mixture == POT_WATER && singlepotion->dknown)
                Sprintf(newbuf, "clears");
            else
                Sprintf(newbuf, "turns %s",
                        hcolor(OBJ_DESCR(objects[mixture])));
            pline_The("%spotion%s %s.", oldbuf,
                      more_than_one ? " that you dipped into" : "", newbuf);
            if (!objects[old_otyp].oc_uname
                && !objects[old_otyp].oc_name_known && old_dknown) {
                struct obj fakeobj;
                fakeobj = zeroobj;
                fakeobj.dknown = 1;
                fakeobj.otyp = old_otyp;
                fakeobj.oclass = POTION_CLASS;
                docall(&fakeobj);
            }
        }
        obj_extract_self(singlepotion);
        singlepotion =
            hold_another_object(singlepotion, "You juggle and drop %s!",
                                doname(singlepotion), (const char *) 0);
        update_inventory();
        return 1;
    }

    pline("Interesting...");
    return 1;

poof:
    if (!objects[potion->otyp].oc_name_known
        && !objects[potion->otyp].oc_uname)
        docall(potion);
    useup(potion);
    return 1;
}

/* *monp grants a wish and then leaves the game */
void
mongrantswish(monp)
struct monst **monp;
{
    struct monst *mon = *monp;
    int mx = mon->mx, my = mon->my, glyph = glyph_at(mx, my);

    /* remove the monster first in case wish proves to be fatal
       (blasted by artifact), to keep it out of resulting bones file */
    mongone(mon);
    *monp = 0; /* inform caller that monster is gone */
    /* hide that removal from player--map is visible during wish prompt */
    tmp_at(DISP_ALWAYS, glyph);
    tmp_at(mx, my);
    /* grant the wish */
    makewish();
    /* clean up */
    tmp_at(DISP_END, 0);
}

void
djinni_from_bottle(obj)
struct obj *obj;
{
    struct monst *mtmp;
    int chance;

    if (!(mtmp = makemon(&mons[PM_DJINNI], currentX(), currentY(), NO_MM_FLAGS))) {
        pline("It turns out to be empty.");
        return;
    }

    if (!Blind) {
        pline("In a cloud of smoke, %s emerges!", a_monnam(mtmp));
        pline("%s speaks.", Monnam(mtmp));
    } else {
        You("smell acrid fumes.");
        pline("%s speaks.", Something);
    }

    chance = rn2(5);
    if (obj->blessed)
        chance = (chance == 4) ? rnd(4) : 0;
    else if (obj->cursed)
        chance = (chance == 0) ? rn2(4) : 4;
    /* 0,1,2,3,4:  b=80%,5,5,5,5; nc=20%,20,20,20,20; c=5%,5,5,5,80 */

    switch (chance) {
    case 0:
        verbalize("I am in your debt.  I will grant one wish!");
        /* give a wish and discard the monster (mtmp set to null) */
        mongrantswish(&mtmp);
        break;
    case 1:
        verbalize("Thank you for freeing me!");
        (void) tamedog(mtmp, (struct obj *) 0);
        break;
    case 2:
        verbalize("You freed me!");
        mtmp->mpeaceful = TRUE;
        set_malign(mtmp);
        break;
    case 3:
        verbalize("It is about time!");
        if (canspotmon(mtmp))
            pline("%s vanishes.", Monnam(mtmp));
        mongone(mtmp);
        break;
    default:
        verbalize("You disturbed me, fool!");
        mtmp->mpeaceful = FALSE;
        set_malign(mtmp);
        break;
    }
}

/* clone a gremlin or mold (2nd arg non-null implies heat as the trigger);
   hit points are cut in half (odd HP stays with original) */
struct monst *
split_mon(mon, mtmp)
struct monst *mon,  /* monster being split */
             *mtmp; /* optional attacker whose heat triggered it */
{
    struct monst *mtmp2;
    char reason[BUFSZ];

    reason[0] = '\0';
    if (mtmp)
        Sprintf(reason, " from %s heat",
                (mtmp == &youmonst) ? the_your[1]
                                    : (const char *) s_suffix(mon_nam(mtmp)));

    if (mon == &youmonst) {
        mtmp2 = cloneu();
        if (mtmp2) {
            mtmp2->mhpmax = maximumHitPointsAsMonster() / 2;
            decreaseMaximumHitPointsAsMonster(mtmp2->mhpmax);
            context.botl = 1;
            You("multiply%s!", reason);
        }
    } else {
        mtmp2 = clone_mon(mon, 0, 0);
        if (mtmp2) {
            mtmp2->mhpmax = mon->mhpmax / 2;
            mon->mhpmax -= mtmp2->mhpmax;
            if (canspotmon(mon))
                pline("%s multiplies%s!", Monnam(mon), reason);
        }
    }
    return mtmp2;
}

/*potion.c*/
