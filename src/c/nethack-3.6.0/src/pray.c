/* NetHack 3.6	pray.c	$NHDT-Date: 1446854232 2015/11/06 23:57:12 $  $NHDT-Branch: master $:$NHDT-Revision: 1.87 $ */
/* Copyright (c) Benson I. Margulies, Mike Stephenson, Steve Linhart, 1989. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_PTR int NDECL(prayer_done);
STATIC_DCL struct obj *NDECL(worst_cursed_item);
STATIC_DCL int NDECL(in_trouble);
STATIC_DCL void FDECL(fix_worst_trouble, (int));
STATIC_DCL void FDECL(angrygods, (ALIGNTYP_P));
STATIC_DCL void FDECL(at_your_feet, (const char *));
STATIC_DCL void NDECL(gcrownu);
STATIC_DCL void FDECL(pleased, (ALIGNTYP_P));
STATIC_DCL void FDECL(godvoice, (ALIGNTYP_P, const char *));
STATIC_DCL void FDECL(god_zaps_you, (ALIGNTYP_P));
STATIC_DCL void FDECL(fry_by_god, (ALIGNTYP_P, BOOLEAN_P));
STATIC_DCL void FDECL(gods_angry, (ALIGNTYP_P));
STATIC_DCL void FDECL(gods_upset, (ALIGNTYP_P));
STATIC_DCL void FDECL(consume_offering, (struct obj *));
STATIC_DCL boolean FDECL(water_prayer, (BOOLEAN_P));
STATIC_DCL boolean FDECL(blocked_boulder, (int, int));

/* simplify a few tests */
#define Cursed_obj(obj, typ) ((obj) && (obj)->otyp == (typ) && (obj)->cursed)

/*
 * Logic behind deities and altars and such:
 * + prayers are made to your god if not on an altar, and to the altar's god
 *   if you are on an altar
 * + If possible, your god answers all prayers, which is why bad things happen
 *   if you try to pray on another god's altar
 * + sacrifices work basically the same way, but the other god may decide to
 *   accept your allegiance, after which they are your god.  If rejected,
 *   your god takes over with your punishment.
 * + if you're in Gehennom, all messages come from Moloch
 */

/*
 *      Moloch, who dwells in Gehennom, is the "renegade" cruel god
 *      responsible for the theft of the Amulet from Marduk, the Creator.
 *      Moloch is unaligned.
 */
static const char *Moloch = "Moloch";

static const char *godvoices[] = {
    "booms out", "thunders", "rings out", "booms",
};

/* values calculated when prayer starts, and used when completed */
static aligntyp p_aligntyp;
static int p_trouble;
static int p_type; /* (-1)-3: (-1)=really naughty, 3=really good */

#define PIOUS 20
#define DEVOUT 14
#define FERVENT 9
#define STRIDENT 4

/*
 * The actual trouble priority is determined by the order of the
 * checks performed in in_trouble() rather than by these numeric
 * values, so keep that code and these values synchronized in
 * order to have the values be meaningful.
 */

#define TROUBLE_STONED 14
#define TROUBLE_SLIMED 13
#define TROUBLE_STRANGLED 12
#define TROUBLE_LAVA 11
#define TROUBLE_SICK 10
#define TROUBLE_STARVING 9
#define TROUBLE_REGION 8 /* stinking cloud */
#define TROUBLE_HIT 7
#define TROUBLE_LYCANTHROPE 6
#define TROUBLE_COLLAPSING 5
#define TROUBLE_STUCK_IN_WALL 4
#define TROUBLE_CURSED_LEVITATION 3
#define TROUBLE_UNUSEABLE_HANDS 2
#define TROUBLE_CURSED_BLINDFOLD 1

#define TROUBLE_PUNISHED (-1)
#define TROUBLE_FUMBLING (-2)
#define TROUBLE_CURSED_ITEMS (-3)
#define TROUBLE_SADDLE (-4)
#define TROUBLE_BLIND (-5)
#define TROUBLE_POISONED (-6)
#define TROUBLE_WOUNDED_LEGS (-7)
#define TROUBLE_HUNGRY (-8)
#define TROUBLE_STUNNED (-9)
#define TROUBLE_CONFUSED (-10)
#define TROUBLE_HALLUCINATION (-11)


#define ugod_is_angry() (currentAlignmentRecord() < 0)
#define on_altar() IS_ALTAR(levl[currentX()][currentY()].typ)
#define on_shrine() ((levl[currentX()][currentY()].altarmask & AM_SHRINE) != 0)

int a_align(int x, int y) {
    return Amask2align(levl[x][y].altarmask & AM_MASK);
}

/* critically low hit points if hp <= 5 or hp <= maxhp/N for some N */
boolean
critically_low_hp(only_if_injured)
boolean only_if_injured; /* determines whether maxhp <= 5 matters */
{
    int divisor, hplim, curhp = areYouPolymorphed() ? currentHitPointsAsMonster() : currentHitPoints(),
                        maxhp = areYouPolymorphed() ? maximumHitPointsAsMonster() : maximumHitPoints();

    if (only_if_injured && !(curhp < maxhp))
        return FALSE;
    /* if maxhp is extremely high, use lower threshold for the division test
       (golden glow cuts off at 11+5*lvl, nurse interaction at 25*lvl; this
       ought to use monster hit dice--and a smaller multiplier--rather than
       ulevel when polymorphed, but polyself doesn't maintain that) */
    hplim = 15 * currentExperienceLevel();
    if (maxhp > hplim)
        maxhp = hplim;
    /* 7 used to be the unconditional divisor */
    switch (xlev_to_rank(currentExperienceLevel())) { /* maps 1..30 into 0..8 */
    case 0:
    case 1:
        divisor = 5;
        break; /* explvl 1 to 5 */
    case 2:
    case 3:
        divisor = 6;
        break; /* explvl 6 to 13 */
    case 4:
    case 5:
        divisor = 7;
        break; /* explvl 14 to 21 */
    case 6:
    case 7:
        divisor = 8;
        break; /* explvl 22 to 29 */
    default:
        divisor = 9;
        break; /* explvl 30+ */
    }
    /* 5 is a magic number in TROUBLE_HIT handling below */
    return (boolean) (curhp <= 5 || curhp * divisor <= maxhp);
}

/*
 * Return 0 if nothing particular seems wrong, positive numbers for
 * serious trouble, and negative numbers for comparative annoyances.
 * This returns the worst problem. There may be others, and the gods
 * may fix more than one.
 *
 * This could get as bizarre as noting surrounding opponents, (or
 * hostile dogs), but that's really hard.
 *
 * We could force rehumanize of polyselfed people, but we can't tell
 * unintentional shape changes from the other kind. Oh well.
 * 3.4.2: make an exception if polymorphed into a form which lacks
 * hands; that's a case where the ramifications override this doubt.
 */
STATIC_OVL int
in_trouble()
{
    struct obj *otmp;
    int i, j, count = 0;

    /*
     * major troubles
     */
    if (youAreTurningToStone())
        return TROUBLE_STONED;
    if (youAreTurningToSlime())
        return TROUBLE_SLIMED;
    if (youAreBeingStrangled())
        return TROUBLE_STRANGLED;
    if (currentlyTrapped() && currentTrapType() == TT_LAVA)
        return TROUBLE_LAVA;
    if (youAreSick())
        return TROUBLE_SICK;
    if (currentHungerState() >= WEAK)
        return TROUBLE_STARVING;
    if (region_danger())
        return TROUBLE_REGION;
    if (critically_low_hp(FALSE))
        return TROUBLE_HIT;
    if (lycanthropeType() >= LOW_PM)
        return TROUBLE_LYCANTHROPE;
    if (near_capacity() >= EXT_ENCUMBER && yourAttrMax(A_STR) - yourCurrentAttr(A_STR) > 3)
        return TROUBLE_COLLAPSING;

    for (i = -1; i <= 1; i++)
        for (j = -1; j <= 1; j++) {
            if (!i && !j)
                continue;
            if (!isok(currentX() + i, currentY() + j)
                || IS_ROCK(levl[currentX() + i][currentY() + j].typ)
                || (blocked_boulder(i, j) && !throwsRocks(pmid4you())))
                count++;
        }
    if (count == 8 && !youCanPassThroughWalls())
        return TROUBLE_STUCK_IN_WALL;

    if (Cursed_obj(uarmf, LEVITATION_BOOTS)
        || stuck_ring(uleft, RIN_LEVITATION)
        || stuck_ring(uright, RIN_LEVITATION))
        return TROUBLE_CURSED_LEVITATION;
    if (hasNoHands(pmid4you()) || !freehand()) {
        /* for bag/box access [cf use_container()]...
           make sure it's a case that we know how to handle;
           otherwise "fix all troubles" would get stuck in a loop */
        if (welded(uwep))
            return TROUBLE_UNUSEABLE_HANDS;
        if (areYouPolymorphed() && hasNoHands(pmid4you())
            && (!youAreUnchanging() || ((otmp = unchanger()) != 0 && otmp->cursed)))
            return TROUBLE_UNUSEABLE_HANDS;
    }
    if (youAreBlindfolded() && ublindf->cursed)
        return TROUBLE_CURSED_BLINDFOLD;

    /*
     * minor troubles
     */
    if (youAreBeingPunished() || (currentlyTrapped() && currentTrapType() == TT_BURIEDBALL))
        return TROUBLE_PUNISHED;
    if (Cursed_obj(uarmg, GAUNTLETS_OF_FUMBLING)
        || Cursed_obj(uarmf, FUMBLE_BOOTS))
        return TROUBLE_FUMBLING;
    if (worst_cursed_item())
        return TROUBLE_CURSED_ITEMS;
    if (u.usteed) { /* can't voluntarily dismount from a cursed saddle */
        otmp = which_armor(u.usteed, W_SADDLE);
        if (Cursed_obj(otmp, SADDLE))
            return TROUBLE_SADDLE;
    }

    if (yourIntrinsic(BLINDED) > 1 && hasEyes(pmid4you())
        && (!swallowed()
            || !monsterHasAttackWithDamageType(pmid4mon(u.ustuck), AT_ENGL, AD_BLND)))
        return TROUBLE_BLIND;
    for (i = 0; i < A_MAX; i++)
        if (yourCurrentAttr(i) < yourAttrMax(i))
            return TROUBLE_POISONED;
    if (youHaveWoundedLegs() && !u.usteed)
        return TROUBLE_WOUNDED_LEGS;
    if (currentHungerState() >= HUNGRY)
        return TROUBLE_HUNGRY;
    if (yourIntrinsicTimeout(STUNNED))
        return TROUBLE_STUNNED;
    if (yourIntrinsicTimeout(CONFUSION))
        return TROUBLE_CONFUSED;
    if (yourIntrinsicTimeout(HALLUC))
        return TROUBLE_HALLUCINATION;
    return 0;
}

/* select an item for TROUBLE_CURSED_ITEMS */
STATIC_OVL struct obj *
worst_cursed_item()
{
    register struct obj *otmp;

    /* if strained or worse, check for loadstone first */
    if (near_capacity() >= HVY_ENCUMBER) {
        for (otmp = invent; otmp; otmp = otmp->nobj)
            if (Cursed_obj(otmp, LOADSTONE))
                return otmp;
    }
    /* weapon takes precedence if it is interfering
       with taking off a ring or putting on a shield */
    if (welded(uwep) && (uright || bimanual(uwep))) { /* weapon */
        otmp = uwep;
        /* gloves come next, due to rings */
    } else if (uarmg && uarmg->cursed) { /* gloves */
        otmp = uarmg;
        /* then shield due to two handed weapons and spells */
    } else if (uarms && uarms->cursed) { /* shield */
        otmp = uarms;
        /* then cloak due to body armor */
    } else if (uarmc && uarmc->cursed) { /* cloak */
        otmp = uarmc;
    } else if (uarm && uarm->cursed) { /* suit */
        otmp = uarm;
    } else if (uarmh && uarmh->cursed) { /* helmet */
        otmp = uarmh;
    } else if (uarmf && uarmf->cursed) { /* boots */
        otmp = uarmf;
    } else if (uarmu && uarmu->cursed) { /* shirt */
        otmp = uarmu;
    } else if (uamul && uamul->cursed) { /* amulet */
        otmp = uamul;
    } else if (uleft && uleft->cursed) { /* left ring */
        otmp = uleft;
    } else if (uright && uright->cursed) { /* right ring */
        otmp = uright;
    } else if (ublindf && ublindf->cursed) { /* eyewear */
        otmp = ublindf;                      /* must be non-blinding lenses */
        /* if weapon wasn't handled above, do it now */
    } else if (welded(uwep)) { /* weapon */
        otmp = uwep;
        /* active secondary weapon even though it isn't welded */
    } else if (uswapwep && uswapwep->cursed && usingTwoWeapons()) {
        otmp = uswapwep;
        /* all worn items ought to be handled by now */
    } else {
        for (otmp = invent; otmp; otmp = otmp->nobj) {
            if (!otmp->cursed)
                continue;
            if (otmp->otyp == LOADSTONE || confers_luck(otmp))
                break;
        }
    }
    return otmp;
}

STATIC_OVL void
fix_worst_trouble(trouble)
int trouble;
{
    int i;
    struct obj *otmp = 0;
    const char *what = (const char *) 0;
    static NEARDATA const char leftglow[] = "Your left ring softly glows",
                               rightglow[] = "Your right ring softly glows";

    switch (trouble) {
    case TROUBLE_STONED:
        make_stoned(0L, "You feel more limber.", 0, (char *) 0);
        break;
    case TROUBLE_SLIMED:
        make_slimed(0L, "The slime disappears.");
        break;
    case TROUBLE_STRANGLED:
        if (uamul && uamul->otyp == AMULET_OF_STRANGULATION) {
            Your("amulet vanishes!");
            useup(uamul);
        }
        You("can breathe again.");
        setYourIntrinsic(STRANGLED, 0L);
        context.botl = 1;
        break;
    case TROUBLE_LAVA:
        You("are back on solid ground.");
        /* teleport should always succeed, but if not,
         * just untrap them.
         */
        if (!safe_teleds(FALSE)) {
            setCurrentTrapTimeout(0);
        }
        break;
    case TROUBLE_STARVING:
        losestr(-1);
        /*FALLTHRU*/
    case TROUBLE_HUNGRY:
        Your("%s feels content.", body_part(STOMACH));
        init_uhunger();
        context.botl = 1;
        break;
    case TROUBLE_SICK:
        You_feel("better.");
        make_sick(0L, (char *) 0, FALSE, sickAll());
        break;
    case TROUBLE_REGION:
        /* stinking cloud, with hero vulnerable to HP loss */
        region_safety();
        break;
    case TROUBLE_HIT:
        /* "fix all troubles" will keep trying if hero has
           5 or less hit points, so make sure they're always
           boosted to be more than that */
        You_feel("much better.");
        if (areYouPolymorphed()) {
            increaseMaximumHitPointsAsMonster(rnd(5));
            if (maximumHitPointsAsMonster() <= 5)
                setMaximumHitPointsAsMonster(5 + 1);
            setCurrentHitPointsAsMonster(maximumHitPointsAsMonster());
        }
        if (maximumHitPoints() < currentExperienceLevel() * 5 + 11)
            increaseMaximumHitPoints(rnd(5));
        if (maximumHitPoints() <= 5)
            setMaximumHitPoints(5 + 1);
        setCurrentHitPoints(maximumHitPoints());
        context.botl = 1;
        break;
    case TROUBLE_COLLAPSING:
        /* override fixed abilities; uncurse that if feasible */
        You_feel("%sstronger.",
                 (yourAttrMax(A_STR) - yourCurrentAttr(A_STR) > 6) ? "much " : "");
        setYourCurrentAttr(A_STR, yourAttrMax(A_STR));
        context.botl = 1;
        if (youHaveFixedAbilities()) {
            if ((otmp = stuck_ring(uleft, RIN_SUSTAIN_ABILITY)) != 0) {
                if (otmp == uleft)
                    what = leftglow;
            } else if ((otmp = stuck_ring(uright, RIN_SUSTAIN_ABILITY))
                       != 0) {
                if (otmp == uright)
                    what = rightglow;
            }
            if (otmp)
                goto decurse;
        }
        break;
    case TROUBLE_STUCK_IN_WALL:
        Your("surroundings change.");
        /* no control, but works on no-teleport levels */
        (void) safe_teleds(FALSE);
        break;
    case TROUBLE_CURSED_LEVITATION:
        if (Cursed_obj(uarmf, LEVITATION_BOOTS)) {
            otmp = uarmf;
        } else if ((otmp = stuck_ring(uleft, RIN_LEVITATION)) != 0) {
            if (otmp == uleft)
                what = leftglow;
        } else if ((otmp = stuck_ring(uright, RIN_LEVITATION)) != 0) {
            if (otmp == uright)
                what = rightglow;
        }
        goto decurse;
    case TROUBLE_UNUSEABLE_HANDS:
        if (welded(uwep)) {
            otmp = uwep;
            goto decurse;
        }
        if (areYouPolymorphed() && hasNoHands(pmid4you())) {
            if (!youAreUnchanging()) {
                Your("shape becomes uncertain.");
                rehumanize(); /* "You return to {normal} form." */
            } else if ((otmp = unchanger()) != 0 && otmp->cursed) {
                /* otmp is an amulet of unchanging */
                goto decurse;
            }
        }
        if (hasNoHands(pmid4you()) || !freehand())
            impossible("fix_worst_trouble: couldn't cure hands.");
        break;
    case TROUBLE_CURSED_BLINDFOLD:
        otmp = ublindf;
        goto decurse;
    case TROUBLE_LYCANTHROPE:
        you_unwere(TRUE);
        break;
    /*
     */
    case TROUBLE_PUNISHED:
        Your("chain disappears.");
        if (currentlyTrapped() && currentTrapType() == TT_BURIEDBALL)
            buried_ball_to_freedom();
        else
            unpunish();
        break;
    case TROUBLE_FUMBLING:
        if (Cursed_obj(uarmg, GAUNTLETS_OF_FUMBLING))
            otmp = uarmg;
        else if (Cursed_obj(uarmf, FUMBLE_BOOTS))
            otmp = uarmf;
        goto decurse;
        /*NOTREACHED*/
        break;
    case TROUBLE_CURSED_ITEMS:
        otmp = worst_cursed_item();
        if (otmp == uright)
            what = rightglow;
        else if (otmp == uleft)
            what = leftglow;
    decurse:
        if (!otmp) {
            impossible("fix_worst_trouble: nothing to uncurse.");
            return;
        }
        if (youCanSee() || (otmp == ublindf && youAreBlindDueToBlindfold())) {
            pline("%s %s.",
                  what ? what : (const char *) Yobjnam2(otmp, "softly glow"),
                  hcolor(NH_AMBER));
            iflags.last_msg = PLNMSG_OBJ_GLOWS;
            otmp->bknown = TRUE;
        }
        uncurse(otmp);
        update_inventory();
        break;
    case TROUBLE_POISONED:
        /* override fixed abilities; ignore items which confer that */
        if (youAreHallucinating())
            pline("There's a tiger in your tank.");
        else
            You_feel("in good health again.");
        for (i = 0; i < A_MAX; i++) {
            if (yourCurrentAttr(i) < yourAttrMax(i)) {
                setYourCurrentAttr(i, yourAttrMax(i));
                context.botl = 1;
            }
        }
        (void) encumber_msg();
        break;
    case TROUBLE_BLIND: {
        const char *eyes = body_part(EYE);

        if (eyeCount(pmid4you()) != 1)
            eyes = makeplural(eyes);
        Your("%s %s better.", eyes, vtense(eyes, "feel"));
        setCreamed(0);
        make_blinded(0L, FALSE);
        break;
    }
    case TROUBLE_WOUNDED_LEGS:
        heal_legs();
        break;
    case TROUBLE_STUNNED:
        make_stunned(0L, TRUE);
        break;
    case TROUBLE_CONFUSED:
        make_confused(0L, TRUE);
        break;
    case TROUBLE_HALLUCINATION:
        pline("Looks like you are back in Kansas.");
        (void) make_hallucinated(0L, FALSE, 0L);
        break;
    case TROUBLE_SADDLE:
        otmp = which_armor(u.usteed, W_SADDLE);
        if (youCanSee()) {
            pline("%s %s.", Yobjnam2(otmp, "softly glow"), hcolor(NH_AMBER));
            otmp->bknown = TRUE;
        }
        uncurse(otmp);
        break;
    }
}

/* "I am sometimes shocked by...  the nuns who never take a bath without
 * wearing a bathrobe all the time.  When asked why, since no man can see
 * them,
 * they reply 'Oh, but you forget the good God'.  Apparently they conceive of
 * the Deity as a Peeping Tom, whose omnipotence enables Him to see through
 * bathroom walls, but who is foiled by bathrobes." --Bertrand Russell, 1943
 * Divine wrath, dungeon walls, and armor follow the same principle.
 */
STATIC_OVL void
god_zaps_you(resp_god)
aligntyp resp_god;
{
    if (swallowed()) {
        pline(
          "Suddenly a bolt of lightning comes down at you from the heavens!");
        pline("It strikes %s!", mon_nam(u.ustuck));
        if (!resists_elec(u.ustuck)) {
            pline("%s fries to a crisp!", Monnam(u.ustuck));
            /* Yup, you get experience.  It takes guts to successfully
             * pull off this trick on your god, anyway.
             */
            xkilled(u.ustuck, 0);
        } else
            pline("%s seems unaffected.", Monnam(u.ustuck));
    } else {
        pline("Suddenly, a bolt of lightning strikes you!");
        if (youCanReflectAttacks()) {
            shieldeff(currentX(), currentY());
            if (youCannotSee())
                pline("For some reason you're unaffected.");
            else
                (void) ureflects("%s reflects from your %s.", "It");
        } else if (youResistShock()) {
            shieldeff(currentX(), currentY());
            pline("It seems not to affect you.");
        } else
            fry_by_god(resp_god, FALSE);
    }

    javaString deity = nameOfAlignedDeityFromYourPantheon(resp_god);
    pline("%s is not deterred...", deity.c_str);
    releaseJavaString(deity);

    if (swallowed()) {
        pline("A wide-angle disintegration beam aimed at you hits %s!",
              mon_nam(u.ustuck));
        if (!resists_disint(u.ustuck)) {
            pline("%s disintegrates into a pile of dust!", Monnam(u.ustuck));
            xkilled(u.ustuck, 2); /* no corpse */
        } else
            pline("%s seems unaffected.", Monnam(u.ustuck));
    } else {
        pline("A wide-angle disintegration beam hits you!");

        /* disintegrate shield and body armor before disintegrating
         * the impudent mortal, like black dragon breath -3.
         */
        if (uarms && !(yourExtrinsicHasMask(REFLECTING, W_ARMS))
            && !(yourExtrinsicHasMask(DISINT_RES, W_ARMS)))
            (void) destroy_arm(uarms);
        if (uarmc && !(yourExtrinsicHasMask(REFLECTING, W_ARMC))
            && !(yourExtrinsicHasMask(DISINT_RES, W_ARMC)))
            (void) destroy_arm(uarmc);
        if (uarm && !(yourExtrinsicHasMask(REFLECTING, W_ARM))
            && !(yourExtrinsicHasMask(DISINT_RES, W_ARM))
            && !uarmc)
            (void) destroy_arm(uarm);
        if (uarmu && !uarm && !uarmc)
            (void) destroy_arm(uarmu);
        if (!youResistDisintegration())
            fry_by_god(resp_god, TRUE);
        else {
            You("bask in its %s glow for a minute...", NH_BLACK);
            godvoice(resp_god, "I believe it not!");
        }
        if (areYouOnAstralLevel() || areYouOnSanctumLevel()) {
            /* one more try for high altars */
            verbalize("Thou cannot escape my wrath, mortal!");
            summon_minion(resp_god, FALSE);
            summon_minion(resp_god, FALSE);
            summon_minion(resp_god, FALSE);

            int gender = flags.female ? 1 : 0;
            javaString objective = objectivePronoun(gender);
            verbalize("Destroy %s, my servants!", objective.c_str);
            releaseJavaString(objective);
        }
    }
}

STATIC_OVL void
fry_by_god(resp_god, via_disintegration)
aligntyp resp_god;
boolean via_disintegration;
{
    You("%s!", !via_disintegration ? "fry to a crisp"
                                   : "disintegrate into a pile of dust");
    killer.format = KILLED_BY;

    javaString deity = nameOfAlignedDeityFromYourPantheon(resp_god);
    Sprintf(killer.name, "the wrath of %s", deity.c_str);
    releaseJavaString(deity);

    done(DIED);
}

STATIC_OVL void
angrygods(resp_god)
aligntyp resp_god;
{
    int maxanger;

    if (areYouInHell())
        resp_god = A_NONE;
    setBlessings(0);

    /* changed from tmp = divineWrath() + abs (currentLuck()) -- rph */
    /* added test for alignment diff -dlc */
    if (resp_god != currentAlignmentType())
        maxanger = currentAlignmentRecord() / 2 + (currentLuckWithBonus() > 0 ? -currentLuckWithBonus() / 3 : -currentLuckWithBonus());
    else
        maxanger = 3 * divineWrath() + ((currentLuckWithBonus() > 0 || currentAlignmentRecord() >= STRIDENT)
                                   ? -currentLuckWithBonus() / 3
                                   : -currentLuckWithBonus());
    if (maxanger < 1)
        maxanger = 1; /* possible if bad align & good luck */
    else if (maxanger > 15)
        maxanger = 15; /* be reasonable */

    switch (rn2(maxanger)) {
    case 0:
    case 1:
	{
	    javaString deity = nameOfAlignedDeityFromYourPantheon(resp_god);
            You_feel("that %s is %s.", deity.c_str,
                 youAreHallucinating() ? "bummed" : "displeased");
	    releaseJavaString(deity);
	}
        break;
    case 2:
    case 3:
        godvoice(resp_god, (char *) 0);
        pline("\"Thou %s, %s.\"",
              (ugod_is_angry() && resp_god == currentAlignmentType())
                  ? "hast strayed from the path"
                  : "art arrogant",
              monsterClass(pmid4you()) == S_HUMAN ? "mortal" : "creature");
        verbalize("Thou must relearn thy lessons!");
        (void) adjattrib(A_WIS, -1, FALSE);
        losexp((char *) 0);
        break;
    case 6:
        if (!youAreBeingPunished()) {
            gods_angry(resp_god);
            punish((struct obj *) 0);
            break;
        } /* else fall thru */
    case 4:
    case 5:
        gods_angry(resp_god);
        if (youCanSee() && !youResistMagic())
            pline("%s glow surrounds you.", An(hcolor(NH_BLACK)));
        rndcurse();
        break;
    case 7:
    case 8:
        godvoice(resp_god, (char *) 0);
        verbalize("Thou durst %s me?",
                  (on_altar() && (a_align(currentX(), currentY()) != resp_god))
                      ? "scorn"
                      : "call upon");
        pline("\"Then die, %s!\"",
              monsterClass(pmid4you()) == S_HUMAN ? "mortal" : "creature");
        summon_minion(resp_god, FALSE);
        break;

    default:
        gods_angry(resp_god);
        god_zaps_you(resp_god);
        break;
    }
    setTimeToNextBlessing(rnz(300));
    return;
}

/* helper to print "str appears at your feet", or appropriate */
static void
at_your_feet(str)
const char *str;
{
    if (youCannotSee())
        str = Something;
    if (swallowed()) {
        /* barrier between you and the floor */
        pline("%s %s into %s %s.", str, vtense(str, "drop"),
              s_suffix(mon_nam(u.ustuck)), mbodypart(u.ustuck, STOMACH));
    } else {
        pline("%s %s %s your %s!", str,
              youCannotSee() ? "lands" : vtense(str, "appear"),
              youAreLevitating() ? "beneath" : "at", makeplural(body_part(FOOT)));
    }
}

STATIC_OVL void
gcrownu()
{
    struct obj *obj;
    boolean already_exists, in_hand;
    short class_gift;
    int sp_no;
#define ok_wep(o) ((o) && ((o)->oclass == WEAPON_CLASS || is_weptool(o)))

    setYourIntrinsicMask(SEE_INVIS, FROMOUTSIDE);
    setYourIntrinsicMask(FIRE_RES, FROMOUTSIDE);
    setYourIntrinsicMask(COLD_RES, FROMOUTSIDE);
    setYourIntrinsicMask(SHOCK_RES, FROMOUTSIDE);
    setYourIntrinsicMask(SLEEP_RES, FROMOUTSIDE);
    setYourIntrinsicMask(POISON_RES, FROMOUTSIDE);

    godvoice(currentAlignmentType(), (char *) 0);

    obj = ok_wep(uwep) ? uwep : 0;
    already_exists = in_hand = FALSE; /* lint suppression */
    switch (currentAlignmentType()) {
    case A_LAWFUL:
        setHandOfElberethLevel(1);
        verbalize("I crown thee...  The Hand of Elbereth!");
        break;
    case A_NEUTRAL:
        setHandOfElberethLevel(2);
        in_hand = (uwep && uwep->oartifact == ART_VORPAL_BLADE);
        already_exists =
            exist_artifact(LONG_SWORD, artiname(ART_VORPAL_BLADE));
        verbalize("Thou shalt be my Envoy of Balance!");
        break;
    case A_CHAOTIC:
        setHandOfElberethLevel(3);
        in_hand = (uwep && uwep->oartifact == ART_STORMBRINGER);
        already_exists =
            exist_artifact(RUNESWORD, artiname(ART_STORMBRINGER));
        verbalize("Thou art chosen to %s for My Glory!",
                  already_exists && !in_hand ? "take lives" : "steal souls");
        break;
    }

    class_gift = STRANGE_OBJECT;
    /* 3.3.[01] had this in the A_NEUTRAL case below,
       preventing chaotic wizards from receiving a spellbook */
    if (yourRoleHasPMID(PM_WIZARD)
        && (!uwep || (uwep->oartifact != ART_VORPAL_BLADE
                      && uwep->oartifact != ART_STORMBRINGER))
        && !carrying(SPE_FINGER_OF_DEATH)) {
        class_gift = SPE_FINGER_OF_DEATH;
    make_splbk:
        obj = mksobj(class_gift, TRUE, FALSE);
        bless(obj);
        obj->bknown = TRUE;
        at_your_feet("A spellbook");
        dropy(obj);
        increaseGiftsBestowed(1);
        /* when getting a new book for known spell, enhance
           currently wielded weapon rather than the book */
        for (sp_no = 0; sp_no < MAXSPELL; sp_no++)
            if (spl_book[sp_no].sp_id == class_gift) {
                if (ok_wep(uwep))
                    obj = uwep; /* to be blessed,&c */
                break;
            }
    } else if (yourRoleHasPMID(PM_MONK) && (!uwep || !uwep->oartifact)
               && !carrying(SPE_RESTORE_ABILITY)) {
        /* monks rarely wield a weapon */
        class_gift = SPE_RESTORE_ABILITY;
        goto make_splbk;
    }

    switch (currentAlignmentType()) {
    case A_LAWFUL:
        if (class_gift != STRANGE_OBJECT) {
            ; /* already got bonus above */
        } else if (obj && obj->otyp == LONG_SWORD && !obj->oartifact) {
            if (youCanSee())
                Your("sword shines brightly for a moment.");
            obj = oname(obj, artiname(ART_EXCALIBUR));
            if (obj && obj->oartifact == ART_EXCALIBUR)
                increaseGiftsBestowed(1);
        }
        /* acquire Excalibur's skill regardless of weapon or gift */
        unrestrict_weapon_skill(P_LONG_SWORD);
        if (obj && obj->oartifact == ART_EXCALIBUR)
            discover_artifact(ART_EXCALIBUR);
        break;
    case A_NEUTRAL:
        if (class_gift != STRANGE_OBJECT) {
            ; /* already got bonus above */
        } else if (obj && in_hand) {
            Your("%s goes snicker-snack!", xname(obj));
            obj->dknown = TRUE;
        } else if (!already_exists) {
            obj = mksobj(LONG_SWORD, FALSE, FALSE);
            obj = oname(obj, artiname(ART_VORPAL_BLADE));
            obj->spe = 1;
            at_your_feet("A sword");
            dropy(obj);
            increaseGiftsBestowed(1);
        }
        /* acquire Vorpal Blade's skill regardless of weapon or gift */
        unrestrict_weapon_skill(P_LONG_SWORD);
        if (obj && obj->oartifact == ART_VORPAL_BLADE)
            discover_artifact(ART_VORPAL_BLADE);
        break;
    case A_CHAOTIC: {
        char swordbuf[BUFSZ];

        Sprintf(swordbuf, "%s sword", hcolor(NH_BLACK));
        if (class_gift != STRANGE_OBJECT) {
            ; /* already got bonus above */
        } else if (obj && in_hand) {
            Your("%s hums ominously!", swordbuf);
            obj->dknown = TRUE;
        } else if (!already_exists) {
            obj = mksobj(RUNESWORD, FALSE, FALSE);
            obj = oname(obj, artiname(ART_STORMBRINGER));
            obj->spe = 1;
            at_your_feet(An(swordbuf));
            dropy(obj);
            increaseGiftsBestowed(1);
        }
        /* acquire Stormbringer's skill regardless of weapon or gift */
        unrestrict_weapon_skill(P_BROAD_SWORD);
        if (obj && obj->oartifact == ART_STORMBRINGER)
            discover_artifact(ART_STORMBRINGER);
        break;
    }
    default:
        obj = 0; /* lint */
        break;
    }

    /* enhance weapon regardless of alignment or artifact status */
    if (ok_wep(obj)) {
        bless(obj);
        obj->oeroded = obj->oeroded2 = 0;
        obj->oerodeproof = TRUE;
        obj->bknown = obj->rknown = TRUE;
        if (obj->spe < 1)
            obj->spe = 1;
        /* acquire skill in this weapon */
        unrestrict_weapon_skill(weapon_type(obj));
    } else if (class_gift == STRANGE_OBJECT) {
        /* opportunity knocked, but there was nobody home... */
        You_feel("unworthy.");
    }
    update_inventory();

    /* lastly, confer an extra skill slot/credit beyond the
       up-to-29 you can get from gaining experience levels */
    add_weapon_skill(1);
    return;
}

STATIC_OVL void
pleased(g_align)
aligntyp g_align;
{
    /* don't use p_trouble, worst trouble may get fixed while praying */
    int trouble = in_trouble(); /* what's your worst difficulty? */
    int pat_on_head = 0, kick_on_butt;

    javaString deity = nameOfAlignedDeityFromYourPantheon(g_align);
    You_feel("that %s is %s.", deity.c_str,
             (currentAlignmentRecord() >= DEVOUT)
                 ? youAreHallucinating() ? "pleased as punch" : "well-pleased"
                 : (currentAlignmentRecord() >= STRIDENT)
                       ? youAreHallucinating() ? "ticklish" : "pleased"
                       : youAreHallucinating() ? "full" : "satisfied");
    releaseJavaString(deity);

    /* not your deity */
    if (on_altar() && p_aligntyp != currentAlignmentType()) {
        adjalign(-1);
        return;
    } else if (currentAlignmentRecord() < 2 && trouble <= 0)
        adjalign(1);

    /*
     * Depending on your luck & align level, the god you prayed to will:
     *  - fix your worst problem if it's major;
     *  - fix all your major problems;
     *  - fix your worst problem if it's minor;
     *  - fix all of your problems;
     *  - do you a gratuitous favor.
     *
     * If you make it to the the last category, you roll randomly again
     * to see what they do for you.
     *
     * If your luck is at least 0, then you are guaranteed rescued from
     * your worst major problem.
     */
    if (!trouble && currentAlignmentRecord() >= DEVOUT) {
        /* if hero was in trouble, but got better, no special favor */
        if (p_trouble == 0)
            pat_on_head = 1;
    } else {
        int action, prayer_luck;
        int tryct = 0;

        /* Negative luck is normally impossible here (can_pray() forces
           prayer failure in that situation), but it's possible for
           Luck to drop during the period of prayer occupation and
           become negative by the time we get here.  [Reported case
           was lawful character whose stinking cloud caused a delayed
           killing of a peaceful human, triggering the "murderer"
           penalty while successful prayer was in progress.  It could
           also happen due to inconvenient timing on Friday 13th, but
           the magnitude there (-1) isn't big enough to cause trouble.]
           We don't bother remembering start-of-prayer luck, just make
           sure it's at least -1 so that Luck+2 is big enough to avoid
           a divide by zero crash when generating a random number.  */
        prayer_luck = max(currentLuckWithBonus(), -1); /* => (prayer_luck + 2 > 0) */
        action = rn1(prayer_luck + (on_altar() ? 3 + on_shrine() : 2), 1);
        if (!on_altar())
            action = min(action, 3);
        if (currentAlignmentRecord() < STRIDENT)
            action = (currentAlignmentRecord() > 0 || !rnl(2)) ? 1 : 0;

        switch (min(action, 5)) {
        case 5:
            pat_on_head = 1;
        case 4:
            do
                fix_worst_trouble(trouble);
            while ((trouble = in_trouble()) != 0);
            break;

        case 3:
            fix_worst_trouble(trouble);
        case 2:
            /* arbitrary number of tries */
            while ((trouble = in_trouble()) > 0 && (++tryct < 10))
                fix_worst_trouble(trouble);
            break;

        case 1:
            if (trouble > 0)
                fix_worst_trouble(trouble);
        case 0:
            break; /* your god blows you off, too bad */
        }
    }

    /* note: can't get pat_on_head unless all troubles have just been
       fixed or there were no troubles to begin with; hallucination
       won't be in effect so special handling for it is superfluous */
    if (pat_on_head)
        switch (rn2((currentLuckWithBonus() + 6) >> 1)) {
        case 0:
            break;
        case 1:
            if (uwep && (welded(uwep) || uwep->oclass == WEAPON_CLASS
                         || is_weptool(uwep))) {
                char repair_buf[BUFSZ];

                *repair_buf = '\0';
                if (uwep->oeroded || uwep->oeroded2)
                    Sprintf(repair_buf, " and %s now as good as new",
                            otense(uwep, "are"));

                if (uwep->cursed) {
                    if (youCanSee()) {
                        pline("%s %s%s.", Yobjnam2(uwep, "softly glow"),
                              hcolor(NH_AMBER), repair_buf);
                        iflags.last_msg = PLNMSG_OBJ_GLOWS;
                    } else {
		        javaString yourDeity = yourDeityName();
                        You_feel("the power of %s over %s.", yourDeity.c_str,
                                 yname(uwep));
		        releaseJavaString(yourDeity);
		    }
                    uncurse(uwep);
                    uwep->bknown = TRUE;
                    *repair_buf = '\0';
                } else if (!uwep->blessed) {
                    if (youCanSee()) {
                        pline("%s with %s aura%s.",
                              Yobjnam2(uwep, "softly glow"),
                              an(hcolor(NH_LIGHT_BLUE)), repair_buf);
                        iflags.last_msg = PLNMSG_OBJ_GLOWS;
                    } else {
		        javaString yourDeity = yourDeityName();
                        You_feel("the blessing of %s over %s.", yourDeity.c_str,
                                 yname(uwep));
		        releaseJavaString(yourDeity);
		    }
                    bless(uwep);
                    uwep->bknown = TRUE;
                    *repair_buf = '\0';
                }

                /* fix any rust/burn/rot damage, but don't protect
                   against future damage */
                if (uwep->oeroded || uwep->oeroded2) {
                    uwep->oeroded = uwep->oeroded2 = 0;
                    /* only give this message if we didn't just bless
                       or uncurse (which has already given a message) */
                    if (*repair_buf)
                        pline("%s as good as new!",
                              Yobjnam2(uwep, youCannotSee() ? "feel" : "look"));
                }
                update_inventory();
            }
            break;
        case 3:
            /* takes 2 hints to get the music to enter the stronghold;
               skip if you've solved it via mastermind or destroyed the
               drawbridge (both set uopened_dbridge) or if you've already
               travelled past the Valley of the Dead (gehennom_entered) */
            if (! haveOpenedDrawbridge() && ! enteredGehennomViaValley()) {
                if (knowledgeOfPasstune() < 1) {
                    godvoice(g_align, (char *) 0);
                    verbalize("Hark, %s!", monsterClass(pmid4you()) == S_HUMAN
                                               ? "mortal"
                                               : "creature");
                    verbalize(
                       "To enter the castle, thou must play the right tune!");
                    incrementKnowledgeOfPasstune(1);
                    break;
                } else if (knowledgeOfPasstune() < 2) {
                    You_hear("a divine music...");
                    pline("It sounds like:  \"%s\".", tune);
                    incrementKnowledgeOfPasstune(1);
                    break;
                }
            }
        /* Otherwise, falls into next case */
        case 2:
            if (youCanSee())
                You("are surrounded by %s glow.", an(hcolor(NH_GOLDEN)));
            /* if any levels have been lost (and not yet regained),
               treat this effect like blessed full healing */
            if (currentExperienceLevel() < highestExperienceLevelSoFar()) {
                setHighestExperienceLevelSoFar(highestExperienceLevelSoFar() - 1); /* see potion.c */
                pluslvl(FALSE);
            } else {
                increaseMaximumHitPoints(5);
                if (areYouPolymorphed())
                    increaseMaximumHitPointsAsMonster(5);
            }
            setCurrentHitPoints(maximumHitPoints());
            if (areYouPolymorphed())
                setCurrentHitPointsAsMonster(maximumHitPointsAsMonster());
            setYourCurrentAttr(A_STR, yourAttrMax(A_STR));
            if (currentNutrition() < 900) {
                init_uhunger();
            }
            if (currentLuck() < 0)
                setCurrentLuck(0);
            make_blinded(0L, TRUE);
            context.botl = 1;
            break;
        case 4: {
            register struct obj *otmp;
            int any = 0;

            if (youCannotSee()) {
		javaString yourDeity = yourDeityName();
                You_feel("the power of %s.", yourDeity.c_str);
		releaseJavaString(yourDeity);
	    }
            else
                You("are surrounded by %s aura.", an(hcolor(NH_LIGHT_BLUE)));
            for (otmp = invent; otmp; otmp = otmp->nobj) {
                if (otmp->cursed) {
                    if (youCanSee()) {
                        pline("%s %s.", Yobjnam2(otmp, "softly glow"),
                              hcolor(NH_AMBER));
                        iflags.last_msg = PLNMSG_OBJ_GLOWS;
                        otmp->bknown = TRUE;
                        ++any;
                    }
                    uncurse(otmp);
                }
            }
            if (any)
                update_inventory();
            break;
        }
        case 5: {
            static NEARDATA const char msg[] =
                "\"and thus I grant thee the gift of %s!\"";

            godvoice(currentAlignmentType(),
                     "Thou hast pleased me with thy progress,");
            if (!(yourIntrinsicHasMask(TELEPAT, INTRINSIC))) {
                setYourIntrinsicMask(TELEPAT, FROMOUTSIDE);
                pline(msg, "Telepathy");
                if (youCannotSee())
                    see_monsters();
            } else if (!(yourIntrinsicHasMask(FAST, INTRINSIC))) {
                setYourIntrinsicMask(FAST, FROMOUTSIDE);
                pline(msg, "Speed");
            } else if (!yourIntrinsicHasMask(STEALTH, INTRINSIC)) {
                setYourIntrinsicMask(STEALTH, FROMOUTSIDE);
                pline(msg, "Stealth");
            } else {
                if (!(yourIntrinsicHasMask(PROTECTION, INTRINSIC))) {
                    setYourIntrinsicMask(PROTECTION, FROMOUTSIDE);
                    if (!blessings()) {
                        setBlessings(rn1(3, 2));
                    }
                } else {
                    increaseBlessings(1);
                }
                pline(msg, "my protection");
            }
            verbalize("Use it wisely in my name!");
            break;
        }
        case 7:
        case 8:
            if (currentAlignmentRecord() >= PIOUS && (0 == handOfElberethLevel())) {
                gcrownu();
                break;
            } /* else FALLTHRU */
        case 6: {
            struct obj *otmp;
            int sp_no, trycnt = currentExperienceLevel() + 1;

            /* not yet known spells given preference over already known ones
             */
            /* Also, try to grant a spell for which there is a skill slot */
            otmp = mkobj(SPBOOK_CLASS, TRUE);
            while (--trycnt > 0) {
                if (otmp->otyp != SPE_BLANK_PAPER) {
                    for (sp_no = 0; sp_no < MAXSPELL; sp_no++)
                        if (spl_book[sp_no].sp_id == otmp->otyp)
                            break;
                    if (sp_no == MAXSPELL
                        && !weaponSkillIsRestricted(spell_skilltype(otmp->otyp)))
                        break; /* usable, but not yet known */
                } else {
                    if (!objects[SPE_BLANK_PAPER].oc_name_known
                        || carrying(MAGIC_MARKER))
                        break;
                }
                otmp->otyp = rnd_class(bases[SPBOOK_CLASS], SPE_BLANK_PAPER);
            }
            bless(otmp);
            at_your_feet("A spellbook");
            place_object(otmp, currentX(), currentY());
            newsym(currentX(), currentY());
            break;
        }
        default:
            impossible("Confused deity!");
            break;
        }

    setTimeToNextBlessing(rnz(350));
    kick_on_butt = becameDemigod() ? 1 : 0;
    if (handOfElberethLevel() > 0)
        kick_on_butt++;
    if (kick_on_butt)
        increaseTimeToNextBlessing(kick_on_butt * rnz(1000));

    return;
}

/* either blesses or curses water on the altar,
 * returns true if it found any water here.
 */
STATIC_OVL boolean
water_prayer(bless_water)
boolean bless_water;
{
    register struct obj *otmp;
    register long changed = 0;
    boolean other = FALSE, bc_known = !(youCannotSee() || youAreHallucinating());

    for (otmp = level.objects[currentX()][currentY()]; otmp; otmp = otmp->nexthere) {
        /* turn water into (un)holy water */
        if (otmp->otyp == POT_WATER
            && (bless_water ? !otmp->blessed : !otmp->cursed)) {
            otmp->blessed = bless_water;
            otmp->cursed = !bless_water;
            otmp->bknown = bc_known;
            changed += otmp->quan;
        } else if (otmp->oclass == POTION_CLASS)
            other = TRUE;
    }
    if (youCanSee() && changed) {
        pline("%s potion%s on the altar glow%s %s for a moment.",
              ((other && changed > 1L) ? "Some of the"
                                       : (other ? "One of the" : "The")),
              ((other || changed > 1L) ? "s" : ""), (changed > 1L ? "" : "s"),
              (bless_water ? hcolor(NH_LIGHT_BLUE) : hcolor(NH_BLACK)));
    }
    return (boolean) (changed > 0L);
}

STATIC_OVL void
godvoice(g_align, words)
aligntyp g_align;
const char *words;
{
    const char *quot = "";

    if (words)
        quot = "\"";
    else
        words = "";

    javaString deity = nameOfAlignedDeityFromYourPantheon(g_align);
    pline_The("voice of %s %s: %s%s%s", deity.c_str,
              godvoices[rn2(SIZE(godvoices))], quot, words, quot);
    releaseJavaString(deity);
}

STATIC_OVL void
gods_angry(g_align)
aligntyp g_align;
{
    godvoice(g_align, "Thou hast angered me.");
}

/* The g_align god is upset with you. */
STATIC_OVL void
gods_upset(g_align)
aligntyp g_align;
{
    if (g_align == currentAlignmentType())
        increaseDivineWrath(1);
    else if (divineWrath())
        decreaseDivineWrath(1);
    angrygods(g_align);
}

STATIC_OVL void
consume_offering(otmp)
register struct obj *otmp;
{
    if (youAreHallucinating())
        switch (rn2(3)) {
        case 0:
            Your("sacrifice sprouts wings and a propeller and roars away!");
            break;
        case 1:
            Your("sacrifice puffs up, swelling bigger and bigger, and pops!");
            break;
        case 2:
            Your(
     "sacrifice collapses into a cloud of dancing particles and fades away!");
            break;
        }
    else if (youCannotSee() && currentAlignmentType() == A_LAWFUL)
        Your("sacrifice disappears!");
    else
        Your("sacrifice is consumed in a %s!",
             currentAlignmentType() == A_LAWFUL ? "flash of light" : "burst of flame");
    if (carried(otmp))
        useup(otmp);
    else
        useupf(otmp, 1L);
    exercise(A_WIS, TRUE);
}

int
dosacrifice()
{
    static NEARDATA const char cloud_of_smoke[] =
        "A cloud of %s smoke surrounds you...";
    register struct obj *otmp;
    int value = 0, pm;
    boolean highaltar;
    aligntyp altaralign = a_align(currentX(), currentY());

    if (!on_altar() || swallowed()) {
        You("are not standing on an altar.");
        return 0;
    }
    highaltar = ((areYouOnAstralLevel() || areYouOnSanctumLevel())
                 && (levl[currentX()][currentY()].altarmask & AM_SHRINE));

    otmp = floorfood("sacrifice", 1);
    if (!otmp)
        return 0;
    /*
     * Was based on nutritional value and aging behavior (< 50 moves).
     * Sacrificing a food ration got you max luck instantly, making the
     * gods as easy to please as an angry dog!
     *
     * Now only accepts corpses, based on the game's evaluation of their
     * toughness.  Human and pet sacrifice, as well as sacrificing unicorns
     * of your alignment, is strongly discouraged.
     */
#define MAXVALUE 24 /* Highest corpse value (besides Wiz) */

    if (otmp->otyp == CORPSE) {
        int pmid = otmp->corpsenm;
        struct monst *mtmp;

        /* KMH, conduct */
        setAtheistConduct(FALSE);

        /* you're handling this corpse, even if it was killed upon the altar
         */
        feel_cockatrice(otmp, TRUE);
        if (rider_corpse_revival(otmp, FALSE))
            return 1;

        if (otmp->corpsenm == PM_ACID_BLOB
            || (monstermoves <= peek_at_iced_corpse_age(otmp) + 50)) {
            value = monsterDifficulty(otmp->corpsenm) + 1;
            if (otmp->oeaten)
                value = eaten_stat(value, otmp);
        }

        if (isOfYourRace(pmid)) {
            if (isDemon(pmid4you())) {
                You("find the idea very satisfying.");
                exercise(A_WIS, TRUE);
            } else if (currentAlignmentType() != A_CHAOTIC) {
                pline("You'll regret this infamous offense!");
                exercise(A_WIS, FALSE);
            }

            if (highaltar
                && (altaralign != A_CHAOTIC || currentAlignmentType() != A_CHAOTIC)) {
                goto desecrate_high_altar;
            } else if (altaralign != A_CHAOTIC && altaralign != A_NONE) {
                /* curse the lawful/neutral altar */
                javaString speciesAdjective = yourSpeciesAdjective();
                pline_The("altar is stained with %s blood.", speciesAdjective.c_str);
                releaseJavaString(speciesAdjective);
                levl[currentX()][currentY()].altarmask = AM_CHAOTIC;
                angry_priest();
            } else {
                struct monst *dmon;
                const char *demonless_msg;

                /* Human sacrifice on a chaotic or unaligned altar */
                /* is equivalent to demon summoning */
                if (altaralign == A_CHAOTIC && currentAlignmentType() != A_CHAOTIC) {
                    pline(
                    "The blood floods the altar, which vanishes in %s cloud!",
                          an(hcolor(NH_BLACK)));
                    levl[currentX()][currentY()].typ = ROOM;
                    levl[currentX()][currentY()].altarmask = 0;
                    newsym(currentX(), currentY());
                    angry_priest();
                    demonless_msg = "cloud dissipates";
                } else {
                    /* either you're chaotic or altar is Moloch's or both */
                    pline_The("blood covers the altar!");
                    change_luck(altaralign == A_NONE ? -2 : 2);
                    demonless_msg = "blood coagulates";
                }
                if ((pm = dlord(altaralign)) != NON_PM
                    && (dmon = makeMonsterOfType(pm, currentX(), currentY(), NO_MM_FLAGS)) != 0) {
                    char dbuf[BUFSZ];

                    Strcpy(dbuf, a_monnam(dmon));
                    if (!strcmpi(dbuf, "it"))
                        Strcpy(dbuf, "something dreadful");
                    else
                        dmon->mstrategy &= ~STRAT_APPEARMSG;
                    You("have summoned %s!", dbuf);
                    if (sgn(currentAlignmentType()) == sgn(monsterAlignment(pmid4mon(dmon))))
                        dmon->mpeaceful = TRUE;
                    You("are terrified, and unable to move.");
                    nomul(-3);
                    multi_reason = "being terrified of a demon";
                    nomovemsg = 0;
                } else
                    pline_The("%s.", demonless_msg);
            }

            if (currentAlignmentType() != A_CHAOTIC) {
                adjalign(-5);
                increaseDivineWrath(3);
                (void) adjattrib(A_WIS, -1, TRUE);
                if (!areYouInHell())
                    angrygods(currentAlignmentType());
                change_luck(-5);
            } else
                adjalign(5);
            if (carried(otmp))
                useup(otmp);
            else
                useupf(otmp, 1L);
            return 1;
        } else if (has_omonst(otmp)
                   && (mtmp = get_mtraits(otmp, FALSE)) != 0
                   && mtmp->mtame) {
                /* mtmp is a temporary pointer to a tame monster's attributes,
                 * not a real monster */
            pline("So this is how you repay loyalty?");
            adjalign(-3);
            value = -1;
            setYourIntrinsicMask(AGGRAVATE_MONSTER, FROMOUTSIDE);
        } else if (isUndead(pmid)) { /* Not demons--no demon corpses */
            if (currentAlignmentType() != A_CHAOTIC)
                value += 1;
        } else if (isUnicorn(pmid)) {
            int unicalign = sgn(monsterAlignment(pmid));

            if (unicalign == altaralign) {
                /* When same as altar, always a very bad action.
                 */
                pline("Such an action is an insult to %s!",
                      (unicalign == A_CHAOTIC) ? "chaos"
                         : unicalign ? "law" : "balance");
                (void) adjattrib(A_WIS, -1, TRUE);
                value = -5;
            } else if (currentAlignmentType() == altaralign) {
                /* When different from altar, and altar is same as yours,
                 * it's a very good action.
                 */
                if (currentAlignmentRecord() < ALIGNLIM)
                    You_feel("appropriately %s.", align_str(currentAlignmentType()));
                else
                    You_feel("you are thoroughly on the right path.");
                adjalign(5);
                value += 3;
            } else if (unicalign == currentAlignmentType()) {
                /* When sacrificing unicorn of your alignment to altar not of
                 * your alignment, your god gets angry and it's a conversion.
                 */
                setCurrentAlignmentRecord(-1);
                value = 1;
            } else {
                /* Otherwise, unicorn's alignment is different from yours
                 * and different from the altar's.  It's an ordinary (well,
                 * with a bonus) sacrifice on a cross-aligned altar.
                 */
                value += 3;
            }
        }
    } /* corpse */

    if (otmp->otyp == AMULET_OF_YENDOR) {
        if (!highaltar) {
        too_soon:
            if (altaralign == A_NONE && areYouInHell())
                /* hero has left Moloch's Sanctum so is in the process
                   of getting away with the Amulet (outside of Gehennom,
                   fall through to the "ashamed" feedback) */
                gods_upset(A_NONE);
            else
                You_feel("%s.",
                         youAreHallucinating()
                            ? "homesick"
                            /* if on track, give a big hint */
                            : (altaralign == currentAlignmentType())
                               ? "an urge to return to the surface"
                               /* else headed towards celestial disgrace */
                               : "ashamed");
            return 1;
        } else {
            /* The final Test.  Did you win? */
            if (uamul == otmp)
                Amulet_off();
            setHaveAscended(TRUE);
            if (carried(otmp))
                useup(otmp); /* well, it's gone now */
            else
                useupf(otmp, 1L);

	    javaString altarDeity = altarDeityName();
            You("offer the Amulet of Yendor to %s...", altarDeity.c_str);

            if (altaralign == A_NONE) {
                /* Moloch's high altar */
                if (currentAlignmentRecord() > -99)
                    setCurrentAlignmentRecord(-99);
                /*[apparently shrug/snarl can be sensed without being seen]*/

		javaString yourDeity = yourDeityName();
                pline("%s shrugs and retains dominion over %s,", Moloch,
                      yourDeity.c_str);
		releaseJavaString(yourDeity);

                pline("then mercilessly snuffs out your life.");
                Sprintf(killer.name, "%s indifference", s_suffix(Moloch));
                killer.format = KILLED_BY;
                done(DIED);
                /* life-saved (or declined to die in wizard/explore mode) */
                pline("%s snarls and tries again...", Moloch);
                fry_by_god(A_NONE, TRUE); /* wrath of Moloch */
                /* declined to die in wizard or explore mode */
                pline(cloud_of_smoke, hcolor(NH_BLACK));
                done(ESCAPED);
            } else if (currentAlignmentType() != altaralign) {
                /* And the opposing team picks you up and
                   carries you off on their shoulders */
                adjalign(-99);
		
		javaString yourDeity = yourDeityName();
                pline("%s accepts your gift, and gains dominion over %s...",
                      altarDeity.c_str, yourDeity.c_str);
                pline("%s is enraged...", yourDeity.c_str);
		releaseJavaString(yourDeity);

                pline("Fortunately, %s permits you to live...", altarDeity.c_str);
                pline(cloud_of_smoke, hcolor(NH_ORANGE));
                done(ESCAPED);
            } else { /* super big win */
                adjalign(10);
                setAchieved(ACHIEVEMENT_ASCENDED(), TRUE);
                pline(
               "An invisible choir sings, and you are bathed in radiance...");
                godvoice(altaralign, "Congratulations, mortal!");
                display_nhwindow(WIN_MESSAGE, FALSE);
                verbalize(
          "In return for thy service, I grant thee the gift of Immortality!");
                You("ascend to the status of Demigod%s...",
                    flags.female ? "dess" : "");
                done(ASCENDED);
            }
	    releaseJavaString(altarDeity);
        }
    } /* real Amulet */

    if (otmp->otyp == FAKE_AMULET_OF_YENDOR) {
        if (!highaltar && !otmp->known)
            goto too_soon;
        You_hear("a nearby thunderclap.");
        if (!otmp->known) {
            You("realize you have made a %s.",
                youAreHallucinating() ? "boo-boo" : "mistake");
            otmp->known = TRUE;
            change_luck(-1);
            return 1;
        } else {
            /* don't you dare try to fool the gods */
            if (youAreDeaf())
                pline("Oh, no."); /* didn't hear thunderclap */
            change_luck(-3);
            adjalign(-1);
            increaseDivineWrath(3);
            value = -3;
        }
    } /* fake Amulet */

    if (value == 0) {
        pline1(nothing_happens);
        return 1;
    }

    if (altaralign != currentAlignmentType() && highaltar) {
    desecrate_high_altar:
        /*
         * REAL BAD NEWS!!! High altars cannot be converted.  Even an attempt
         * gets the god who owns it truly pissed off.
         */
        You_feel("the air around you grow charged...");

	javaString altarDeity = altarDeityName();
        pline("Suddenly, you realize that %s has noticed you...", altarDeity.c_str);
	releaseJavaString(altarDeity);

        godvoice(altaralign,
                 "So, mortal!  You dare desecrate my High Temple!");
        /* Throw everything we have at the player */
        god_zaps_you(altaralign);
    } else if (value
               < 0) { /* I don't think the gods are gonna like this... */
        gods_upset(altaralign);
    } else {
        int saved_anger = divineWrath();
        int saved_cnt = timeToNextBlessing();
        int saved_luck = currentLuck();

        /* Sacrificing at an altar of a different alignment */
        if (currentAlignmentType() != altaralign) {
            /* Is this a conversion ? */
            /* An unaligned altar in Gehennom will always elicit rejection. */
            if (ugod_is_angry() || (altaralign == A_NONE && areYouInHell())) {
                if (currentAlignmentBase() == originalAlignmentBase()
                    && altaralign != A_NONE) {

		    javaString yourDeity = yourDeityName();
                    You("have a strong feeling that %s is angry...",
                        yourDeity.c_str);
		    releaseJavaString(yourDeity);

                    consume_offering(otmp);
	            javaString altarDeity = altarDeityName();
                    pline("%s accepts your allegiance.", altarDeity.c_str);
		    releaseJavaString(altarDeity);

                    uchangealign(altaralign, 0);
                    /* Beware, Conversion is costly */
                    change_luck(-3);
                    increaseTimeToNextBlessing(300);
                } else {
                    increaseDivineWrath(3);
                    adjalign(-5);
	            javaString altarDeity = altarDeityName();
                    pline("%s rejects your sacrifice!", altarDeity.c_str);
		    releaseJavaString(altarDeity);
                    godvoice(altaralign, "Suffer, infidel!");
                    change_luck(-5);
                    (void) adjattrib(A_WIS, -2, TRUE);
                    if (!areYouInHell())
                        angrygods(currentAlignmentType());
                }
                return 1;
            } else {
                consume_offering(otmp);
		javaString yourDeity = yourDeityName();
	        javaString altarDeity = altarDeityName();
                You("sense a conflict between %s and %s.", yourDeity.c_str,
                    altarDeity.c_str);
	        releaseJavaString(altarDeity);

                if (rn2(8 + currentExperienceLevel()) > 5) {
                    struct monst *pri;
                    You_feel("the power of %s increase.", yourDeity.c_str);
                    exercise(A_WIS, TRUE);
                    change_luck(1);
                    /* Yes, this is supposed to be &=, not |= */
                    levl[currentX()][currentY()].altarmask &= AM_SHRINE;
                    /* the following accommodates stupid compilers */
                    levl[currentX()][currentY()].altarmask =
                        levl[currentX()][currentY()].altarmask
                        | (Align2amask(currentAlignmentType()));
                    if (youCanSee())
                        pline_The("altar glows %s.",
                                  hcolor((currentAlignmentType() == A_LAWFUL)
                                            ? NH_WHITE
                                            : currentAlignmentType()
                                               ? NH_BLACK
                                               : (const char *) "gray"));

                    if (rnl(currentExperienceLevel()) > 6 && currentAlignmentRecord() > 0
                        && rnd(currentAlignmentRecord()) > (3 * ALIGNLIM) / 4)
                        summon_minion(altaralign, TRUE);
                    /* anger priest; test handles bones files */
                    if ((pri = findpriest(mostRecentCurrentlyOccupiedTempleID()))
                        && !p_coaligned(pri))
                        angry_priest();
                } else {
                    pline("Unluckily, you feel the power of %s decrease.",
                          yourDeity.c_str);
                    change_luck(-1);
                    exercise(A_WIS, FALSE);
                    if (rnl(currentExperienceLevel()) > 6 && currentAlignmentRecord() > 0
                        && rnd(currentAlignmentRecord()) > (7 * ALIGNLIM) / 8)
                        summon_minion(altaralign, TRUE);
                }

		releaseJavaString(yourDeity);
                return 1;
            }
        }

        consume_offering(otmp);
        /* OK, you get brownie points. */
        if (divineWrath()) {
            decreaseDivineWrath(((value * (currentAlignmentType() == A_CHAOTIC ? 2 : 3))
                         / MAXVALUE));
            if (divineWrath() < 0)
                setDivineWrath(0);
            if (divineWrath() != saved_anger) {

		javaString yourDeity = yourDeityName();
                if (divineWrath()) {
                    pline("%s seems %s.", yourDeity.c_str,
                          youAreHallucinating() ? "groovy" : "slightly mollified");
                    if (currentLuck() < 0)
                        change_luck(1);
                } else {
                    pline("%s seems %s.", yourDeity.c_str,
                          youAreHallucinating() ? "cosmic (not a new fact)"
                                        : "mollified");

                    if (currentLuck() < 0)
                        setCurrentLuck(0);
                }
		releaseJavaString(yourDeity);

            } else { /* not satisfied yet */
                if (youAreHallucinating())
                    pline_The("gods seem tall.");
                else
                    You("have a feeling of inadequacy.");
            }
        } else if (ugod_is_angry()) {
            if (value > MAXVALUE)
                value = MAXVALUE;
            if (value > -currentAlignmentRecord())
                value = -currentAlignmentRecord();
            adjalign(value);
            You_feel("partially absolved.");
        } else if (timeToNextBlessing() > 0) {
            decreaseTimeToNextBlessing((value * (currentAlignmentType() == A_CHAOTIC ? 500 : 300))
                            / MAXVALUE);
            if (timeToNextBlessing() < 0)
                setTimeToNextBlessing(0);
            if (timeToNextBlessing() != saved_cnt) {
                if (timeToNextBlessing()) {
                    if (youAreHallucinating())
                        You("realize that the gods are not like you and I.");
                    else
                        You("have a hopeful feeling.");
                    if (currentLuck() < 0)
                        change_luck(1);
                } else {
                    if (youAreHallucinating())
                        pline("Overall, there is a smell of fried onions.");
                    else
                        You("have a feeling of reconciliation.");
                    if (currentLuck() < 0)
                        setCurrentLuck(0);
                }
            }
        } else {
            int nartifacts = nartifact_exist();

            /* you were already in pretty good standing */
            /* The player can gain an artifact */
            /* The chance goes down as the number of artifacts goes up */
            if (currentExperienceLevel() > 2 && currentLuck() >= 0
                && !rn2(10 + (2 * giftsBestowed() * nartifacts))) {
                otmp = mk_artifact((struct obj *) 0, a_align(currentX(), currentY()));
                if (otmp) {
                    if (otmp->spe < 0)
                        otmp->spe = 0;
                    if (otmp->cursed)
                        uncurse(otmp);
                    otmp->oerodeproof = TRUE;
                    at_your_feet("An object");
                    dropy(otmp);
                    godvoice(currentAlignmentType(), "Use my gift wisely!");
                    increaseGiftsBestowed(1);
                    setTimeToNextBlessing(rnz(300 + (50 * nartifacts)));
                    exercise(A_WIS, TRUE);
                    /* make sure we can use this weapon */
                    unrestrict_weapon_skill(weapon_type(otmp));
                    if (!youAreHallucinating() && youCanSee()) {
                        otmp->dknown = 1;
                        makeknown(otmp->otyp);
                        discover_artifact(otmp->oartifact);
                    }
                    return 1;
                }
            }
            change_luck((value * maximumPossibleLuck()) / (MAXVALUE * 2));
            if (currentLuck() < 0)
                setCurrentLuck(0);
            if (currentLuck() != saved_luck) {
                if (youCannotSee())
                    You("think %s brushed your %s.", something,
                        body_part(FOOT));
                else
                    You(youAreHallucinating()
                    ? "see crabgrass at your %s.  A funny thing in a dungeon."
                            : "glimpse a four-leaf clover at your %s.",
                        makeplural(body_part(FOOT)));
            }
        }
    }
    return 1;
}

/* determine prayer results in advance; also used for enlightenment */
boolean
can_pray(praying)
boolean praying; /* false means no messages should be given */
{
    int alignment;

    p_aligntyp = on_altar() ? a_align(currentX(), currentY()) : currentAlignmentType();
    p_trouble = in_trouble();

    if (isDemon(pmid4you()) && (p_aligntyp != A_CHAOTIC)) {
        if (praying)
            pline_The("very idea of praying to a %s god is repugnant to you.",
                      p_aligntyp ? "lawful" : "neutral");
        return FALSE;
    }

    if (praying)
    {
	javaString deity = nameOfAlignedDeityFromYourPantheon(p_aligntyp);
        You("begin praying to %s.", deity.c_str);
	releaseJavaString(deity);
    }

    if (currentAlignmentType() && currentAlignmentType() == -p_aligntyp)
        alignment = -currentAlignmentRecord(); /* Opposite alignment altar */
    else if (currentAlignmentType() != p_aligntyp)
        alignment = currentAlignmentRecord() / 2; /* Different alignment altar */
    else
        alignment = currentAlignmentRecord();

    if ((p_trouble > 0) ? (timeToNextBlessing() > 200)      /* big trouble */
           : (p_trouble < 0) ? (timeToNextBlessing() > 100) /* minor difficulties */
              : (timeToNextBlessing() > 0))                 /* not in trouble */
        p_type = 0;                     /* too soon... */
    else if (currentLuckWithBonus() < 0 || divineWrath() || alignment < 0)
        p_type = 1; /* too naughty... */
    else /* alignment >= 0 */ {
        if (on_altar() && currentAlignmentType() != p_aligntyp)
            p_type = 2;
        else
            p_type = 3;
    }

    if (isUndead(pmid4you()) && !areYouInHell()
        && (p_aligntyp == A_LAWFUL || (p_aligntyp == A_NEUTRAL && !rn2(10))))
        p_type = -1;
    /* Note:  when !praying, the random factor for neutrals makes the
       return value a non-deterministic approximation for enlightenment.
       This case should be uncommon enough to live with... */

    return !praying ? (boolean) (p_type == 3 && !areYouInHell()) : TRUE;
}

/* #pray commmand */
int
dopray()
{
    /* Confirm accidental slips of Alt-P */
    if (ParanoidPray && yn("Are you sure you want to pray?") != 'y')
        return 0;

    setAtheistConduct(FALSE);

    /* set up p_type and p_alignment */
    if (!can_pray(TRUE))
        return 0;

    if (wizard && p_type >= 0) {
        if (yn("Force the gods to be pleased?") == 'y') {
            setTimeToNextBlessing(0);
            if (currentLuck() < 0)
                setCurrentLuck(0);
            if (currentAlignmentRecord() <= 0)
                setCurrentAlignmentRecord(1);
            setDivineWrath(0);
            if (p_type < 2)
                p_type = 3;
        }
    }
    nomul(-3);
    multi_reason = "praying";
    nomovemsg = "You finish your prayer.";
    afternmv = prayer_done;

    if (p_type == 3 && !areYouInHell()) {
        /* if you've been true to your god you can't die while you pray */
        if (youCanSee())
            You("are surrounded by a shimmering light.");
        setInvulnerableWhilePraying(TRUE);
    }

    return 1;
}

STATIC_PTR int
prayer_done() /* M. Stephenson (1.0.3b) */
{
    aligntyp alignment = p_aligntyp;

    setInvulnerableWhilePraying(FALSE);
    if (p_type == -1) {
        godvoice(alignment,
                 (alignment == A_LAWFUL)
                    ? "Vile creature, thou durst call upon me?"
                    : "Walk no more, perversion of nature!");
        You_feel("like you are falling apart.");
        /* KMH -- Gods have mastery over unchanging */
        rehumanize();
        /* no Half_physical_damage adjustment here */
        losehp(rnd(20), "residual undead turning effect", KILLED_BY_AN);
        exercise(A_CON, FALSE);
        return 1;
    }
    if (areYouInHell()) {
	javaString deity = nameOfAlignedDeityFromYourPantheon(alignment);
        pline("Since you are in Gehennom, %s won't help you.",
              deity.c_str);
	releaseJavaString(deity);

        /* haltingly aligned is least likely to anger */
        if (currentAlignmentRecord() <= 0 || rnl(currentAlignmentRecord()))
            angrygods(currentAlignmentType());
        return 0;
    }

    if (p_type == 0) {
        if (on_altar() && currentAlignmentType() != alignment)
            (void) water_prayer(FALSE);
        increaseTimeToNextBlessing(rnz(250));
        change_luck(-3);
        gods_upset(currentAlignmentType());
    } else if (p_type == 1) {
        if (on_altar() && currentAlignmentType() != alignment)
            (void) water_prayer(FALSE);
        angrygods(currentAlignmentType()); /* naughty */
    } else if (p_type == 2) {
        if (water_prayer(FALSE)) {
            /* attempted water prayer on a non-coaligned altar */
            increaseTimeToNextBlessing(rnz(250));
            change_luck(-3);
            gods_upset(currentAlignmentType());
        } else
            pleased(alignment);
    } else {
        /* coaligned */
        if (on_altar())
            (void) water_prayer(TRUE);
        pleased(alignment); /* nice */
    }
    return 1;
}

/* #turn command */
int
doturn()
{
    /* Knights & Priest(esse)s only please */
    struct monst *mtmp, *mtmp2;
    int once, range, xlev;

    if (!yourRoleHasPMID(PM_PRIEST) && !yourRoleHasPMID(PM_KNIGHT)) {
        /* Try to use the "turn undead" spell.
         *
         * This used to be based on whether hero knows the name of the
         * turn undead spellbook, but it's possible to know--and be able
         * to cast--the spell while having lost the book ID to amnesia.
         * (It also used to tell spelleffects() to cast at self?)
         */
        int sp_no;

        for (sp_no = 0; sp_no < MAXSPELL; ++sp_no) {
            if (spl_book[sp_no].sp_id == NO_SPELL)
                break;
            else if (spl_book[sp_no].sp_id == SPE_TURN_UNDEAD)
                return spelleffects(sp_no, FALSE);
        }
        You("don't know how to turn undead!");
        return 0;
    }
    setAtheistConduct(FALSE);

    javaString yourDeity = yourDeityName();
    if ((currentAlignmentType() != A_CHAOTIC
         && (isDemon(pmid4you()) || isUndead(pmid4you())))
        || divineWrath() > 6) { /* "Die, mortal!" */
        pline("For some reason, %s seems to ignore you.", yourDeity.c_str);
        aggravate();
        exercise(A_WIS, FALSE);
        return 0;
    }
    if (areYouInHell()) {
        pline("Since you are in Gehennom, %s won't help you.", yourDeity.c_str);
        aggravate();
        return 0;
    }
    pline("Calling upon %s, you chant an arcane formula.", yourDeity.c_str);
    releaseJavaString(yourDeity);
    exercise(A_WIS, TRUE);

    /* note: does not perform unturn_dead() on victims' inventories */
    range = BOLT_LIM + (currentExperienceLevel() / 5); /* 5 to 11 */
    range *= range;
    once = 0;
    for (mtmp = fmon; mtmp; mtmp = mtmp2) {
        mtmp2 = mtmp->nmon;

        if (DEADMONSTER(mtmp))
            continue;
        if (!cansee(mtmp->mx, mtmp->my) || distanceSquaredToYou(mtmp->mx, mtmp->my) > range)
            continue;

        if (!mtmp->mpeaceful
            && (isUndead(pmid4mon(mtmp)) || is_vampshifter(mtmp)
                || (isDemon(pmid4mon(mtmp)) && (currentExperienceLevel() > (MAXULEV / 2))))) {
            mtmp->msleeping = 0;
            if (youAreConfused()) {
                if (!once++)
                    pline("Unfortunately, your voice falters.");
                mtmp->mflee = 0;
                mtmp->mfrozen = 0;
                mtmp->mcanmove = 1;
            } else if (!resist(mtmp, '\0', 0, TELL)) {
                xlev = 6;
                switch (monsterClass(pmid4mon(mtmp))) {
                /* this is intentional, lichs are tougher
                   than zombies. */
                case S_LICH:
                    xlev += 2; /*FALLTHRU*/
                case S_GHOST:
                    xlev += 2; /*FALLTHRU*/
                case S_VAMPIRE:
                    xlev += 2; /*FALLTHRU*/
                case S_WRAITH:
                    xlev += 2; /*FALLTHRU*/
                case S_MUMMY:
                    xlev += 2; /*FALLTHRU*/
                case S_ZOMBIE:
                    if (currentExperienceLevel() >= xlev && !resist(mtmp, '\0', 0, NOTELL)) {
                        if (currentAlignmentType() == A_CHAOTIC) {
                            mtmp->mpeaceful = 1;
                            set_malign(mtmp);
                        } else { /* damn them */
                            killed(mtmp);
                        }
                        break;
                    } /* else flee */
                /*FALLTHRU*/
                default:
                    monflee(mtmp, 0, FALSE, TRUE);
                    break;
                }
            }
        }
    }
    nomul(-5);
    multi_reason = "trying to turn the monsters";
    nomovemsg = You_can_move_again;
    return 1;
}

javaString altarDeityName()
{
    int x = currentX();
    int y = currentY();

    if (!IS_ALTAR(levl[x][y].typ))
        return javaStringFromC("");

    return nameOfAlignedDeityFromYourPantheon(a_align(x, y));
}

void
altar_wrath(x, y)
register int x, y;
{
    aligntyp altaralign = a_align(x, y);

    javaString altarDeity = nameOfAlignedDeityFromYourPantheon(altaralign);
    javaString yourDeity  = yourDeityName();

    if (!strcmp(altarDeity.c_str, yourDeity.c_str)) {
        godvoice(altaralign, "How darest thou desecrate my altar!");
        (void) adjattrib(A_WIS, -1, FALSE);
    } else {
        pline("A voice (could it be %s?) whispers:", altarDeity.c_str);
        verbalize("Thou shalt pay, infidel!");
        change_luck(-1);
    }
    releaseJavaString(altarDeity);
    releaseJavaString(yourDeity);
}

/* assumes isok() at one space away, but not necessarily at two */
STATIC_OVL boolean
blocked_boulder(dx, dy)
int dx, dy;
{
    register struct obj *otmp;
    long count = 0L;

    for (otmp = level.objects[currentX() + dx][currentY() + dy]; otmp;
         otmp = otmp->nexthere) {
        if (otmp->otyp == BOULDER)
            count += otmp->quan;
    }

    switch (count) {
    case 0:
        /* no boulders--not blocked */
        return FALSE;
    case 1:
        /* possibly blocked depending on if it's pushable */
        break;
    default:
        /* more than one boulder--blocked after they push the top one;
           don't force them to push it first to find out */
        return TRUE;
    }

    if (!isok(currentX() + 2 * dx, currentY() + 2 * dy))
        return TRUE;
    if (IS_ROCK(levl[currentX() + 2 * dx][currentY() + 2 * dy].typ))
        return TRUE;
    if (sobj_at(BOULDER, currentX() + 2 * dx, currentY() + 2 * dy))
        return TRUE;

    return FALSE;
}

/*pray.c*/
