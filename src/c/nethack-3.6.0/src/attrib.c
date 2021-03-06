/* NetHack 3.6	attrib.c	$NHDT-Date: 1449269911 2015/12/04 22:58:31 $  $NHDT-Branch: NetHack-3.6.0 $:$NHDT-Revision: 1.51 $ */
/*      Copyright 1988, 1989, 1990, 1992, M. Stephenson           */
/* NetHack may be freely redistributed.  See license for details. */

/*  attribute modification routines. */

#include "hack.h"
#include <ctype.h>

/* part of the output on gain or loss of attribute */
static const char
    *const plusattr[] = { "strong", "smart", "wise",
                          "agile",  "tough", "charismatic" },
    *const minusattr[] = { "weak",    "stupid",
                           "foolish", "clumsy",
                           "fragile", "repulsive" };

static const struct innate {
    schar ulevel;
    int intrinsic;
    const char *gainstr, *losestr;
} arc_abil[] = { { 1, STEALTH, "", "" },
                 { 1, FAST, "", "" },
                 { 10, SEARCHING, "perceptive", "" },
                 { 0, 0, 0, 0 } },

  bar_abil[] = { { 1, POISON_RES, "", "" },
                 { 7, FAST, "quick", "slow" },
                 { 15, STEALTH, "stealthy", "" },
                 { 0, 0, 0, 0 } },

  cav_abil[] = { { 7, FAST, "quick", "slow" },
                 { 15, WARNING, "sensitive", "" },
                 { 0, 0, 0, 0 } },

  hea_abil[] = { { 1, POISON_RES, "", "" },
                 { 15, WARNING, "sensitive", "" },
                 { 0, 0, 0, 0 } },

  kni_abil[] = { { 7, FAST, "quick", "slow" }, { 0, 0, 0, 0 } },

  mon_abil[] = { { 1, FAST, "", "" },
                 { 1, SLEEP_RES, "", "" },
                 { 1, SEE_INVIS, "", "" },
                 { 3, POISON_RES, "healthy", "" },
                 { 5, STEALTH, "stealthy", "" },
                 { 7, WARNING, "sensitive", "" },
                 { 9, SEARCHING, "perceptive", "unaware" },
                 { 11, FIRE_RES, "cool", "warmer" },
                 { 13, COLD_RES, "warm", "cooler" },
                 { 15, SHOCK_RES, "insulated", "conductive" },
                 { 17, TELEPORT_CONTROL, "controlled", "uncontrolled" },
                 { 0, 0, 0, 0 } },

  pri_abil[] = { { 15, WARNING, "sensitive", "" },
                 { 20, FIRE_RES, "cool", "warmer" },
                 { 0, 0, 0, 0 } },

  ran_abil[] = { { 1, SEARCHING, "", "" },
                 { 7, STEALTH, "stealthy", "" },
                 { 15, SEE_INVIS, "", "" },
                 { 0, 0, 0, 0 } },

  rog_abil[] = { { 1, STEALTH, "", "" },
                 { 10, SEARCHING, "perceptive", "" },
                 { 0, 0, 0, 0 } },

  sam_abil[] = { { 1, FAST, "", "" },
                 { 15, STEALTH, "stealthy", "" },
                 { 0, 0, 0, 0 } },

  tou_abil[] = { { 10, SEARCHING, "perceptive", "" },
                 { 20, POISON_RES, "hardy", "" },
                 { 0, 0, 0, 0 } },

  val_abil[] = { { 1, COLD_RES, "", "" },
                 { 1, STEALTH, "", "" },
                 { 7, FAST, "quick", "slow" },
                 { 0, 0, 0, 0 } },

  wiz_abil[] = { { 15, WARNING, "sensitive", "" },
                 { 17, TELEPORT_CONTROL, "controlled", "uncontrolled" },
                 { 0, 0, 0, 0 } },

  /* Intrinsics conferred by race */
    elf_abil[] = { { 4, SLEEP_RES, "awake", "tired" },
                   { 0, 0, 0, 0 } },

  orc_abil[] = { { 1, POISON_RES, "", "" }, { 0, 0, 0, 0 } };

STATIC_DCL void NDECL(exerper);
STATIC_DCL void FDECL(postadjabil, (int));
STATIC_DCL const struct innate *FDECL(check_innate_abil, (int, long));
STATIC_DCL int FDECL(innately, (int));

/* adjust an attribute; return TRUE if change is made, FALSE otherwise */
boolean
adjattrib(ndx, incr, msgflg)
int ndx, incr;
int msgflg; /* positive => no message, zero => message, and */
{           /* negative => conditional (msg if change made) */
    int old_acurr;
    boolean abonflg;
    const char *attrstr;

    if (youHaveFixedAbilities() || !incr)
        return FALSE;

    if ((ndx == A_INT || ndx == A_WIS) && uarmh && uarmh->otyp == DUNCE_CAP) {
        if (msgflg == 0)
            Your("cap constricts briefly, then relaxes again.");
        return FALSE;
    }

    old_acurr = ACURR(ndx);
    if (incr > 0) {
        increaseYourCurrentAttr(ndx, incr);
        if (yourCurrentAttr(ndx) > yourAttrMax(ndx)) {
            incr = yourCurrentAttr(ndx) - yourAttrMax(ndx);
            increaseYourAttrMax(ndx, incr);
            if (yourAttrMax(ndx) > attributeMaximum(ndx))
                setYourAttrMax(ndx, attributeMaximum(ndx));
            setYourCurrentAttr(ndx, yourAttrMax(ndx));
        }
        attrstr = plusattr[ndx];
        abonflg = (yourAttrBonus(ndx) < 0);
    } else {
        increaseYourCurrentAttr(ndx, incr);
        if (yourCurrentAttr(ndx) < attributeMinimum(ndx)) {
            incr = yourCurrentAttr(ndx) - attributeMinimum(ndx);
            setYourCurrentAttr(ndx, attributeMinimum(ndx));
            increaseYourAttrMax(ndx, incr);
            if (yourAttrMax(ndx) < attributeMinimum(ndx))
                setYourAttrMax(ndx, attributeMinimum(ndx));
        }
        attrstr = minusattr[ndx];
        abonflg = (yourAttrBonus(ndx) > 0);
    }
    if (ACURR(ndx) == old_acurr) {
        if (msgflg == 0 && flags.verbose)
            pline("You're %s as %s as you can get.",
                  abonflg ? "currently" : "already", attrstr);
        return FALSE;
    }

    if (msgflg <= 0)
        You_feel("%s%s!", (incr > 1 || incr < -1) ? "very " : "", attrstr);
    context.botl = 1;
    if (moves > 1 && (ndx == A_STR || ndx == A_CON))
        (void) encumber_msg();
    return TRUE;
}

void
gainstr(otmp, incr, givemsg)
struct obj *otmp;
int incr;
boolean givemsg;
{
    int num = incr;

    if (!num) {
        if (yourCurrentAttr(A_STR) < 18)
            num = (rn2(4) ? 1 : rnd(6));
        else if (yourCurrentAttr(A_STR) < STR18(85))
            num = rnd(10);
        else
            num = 1;
    }
    (void) adjattrib(A_STR, (otmp && otmp->cursed) ? -num : num,
                     givemsg ? -1 : 1);
}

/* may kill you; cause may be poison or monster like 'a' */
void
losestr(num)
register int num;
{
    int ustr = yourCurrentAttr(A_STR) - num;

    while (ustr < 3) {
        ++ustr;
        --num;
        if (areYouPolymorphed()) {
            decreaseCurrentHitPointsAsMonster(6);
            decreaseMaximumHitPointsAsMonster(6);
        } else {
            decreaseCurrentHitPoints(6);
            decreaseMaximumHitPoints(6);
        }
    }
    (void) adjattrib(A_STR, -num, 1);
}

static const struct poison_effect_message {
    void VDECL((*delivery_func), (const char *, ...));
    const char *effect_msg;
} poiseff[] = {
    { You_feel, "weaker" },             /* A_STR */
    { Your, "brain is on fire" },       /* A_INT */
    { Your, "judgement is impaired" },  /* A_WIS */
    { Your, "muscles won't obey you" }, /* A_DEX */
    { You_feel, "very sick" },          /* A_CON */
    { You, "break out in hives" }       /* A_CHA */
};

/* feedback for attribute loss due to poisoning */
void
poisontell(typ, exclaim)
int typ;         /* which attribute */
boolean exclaim; /* emphasis */
{
    void VDECL((*func), (const char *, ...)) = poiseff[typ].delivery_func;

    (*func)("%s%c", poiseff[typ].effect_msg, exclaim ? '!' : '.');
}

/* called when an attack or trap has poisoned the hero (used to be in mon.c)
 */
void
poisoned(reason, typ, pkiller, fatal, thrown_weapon)
const char *reason,    /* controls what messages we display */
    *pkiller;          /* for score+log file if fatal */
int typ, fatal;        /* if fatal is 0, limit damage to adjattrib */
boolean thrown_weapon; /* thrown weapons are less deadly */
{
    int i, loss, kprefix = KILLED_BY_AN;

    /* inform player about being poisoned unless that's already been done;
       "blast" has given a "blast of poison gas" message; "poison arrow",
       "poison dart", etc have implicitly given poison messages too... */
    if (strcmp(reason, "blast") && !strstri(reason, "poison")) {
        boolean plural = (reason[strlen(reason) - 1] == 's') ? 1 : 0;

        /* avoid "The" Orcus's sting was poisoned... */
        pline("%s%s %s poisoned!", isupper(*reason) ? "" : "The ", reason,
              plural ? "were" : "was");
    }
    if (youResistPoison()) {
        if (!strcmp(reason, "blast"))
            shieldeff(currentX(), currentY());
        pline_The("poison doesn't seem to affect you.");
        return;
    }

    /* suppress killer prefix if it already has one */
    i = name_to_mon(pkiller);
    if (i >= LOW_PM && (monsterGenerationMask(i) & G_UNIQ)) {
        kprefix = KILLED_BY;
        if (!typeIsProperName(i))
            pkiller = the(pkiller);
    } else if (!strncmpi(pkiller, "the ", 4) || !strncmpi(pkiller, "an ", 3)
               || !strncmpi(pkiller, "a ", 2)) {
        /*[ does this need a plural check too? ]*/
        kprefix = KILLED_BY;
    }

    i = !fatal ? 1 : rn2(fatal + (thrown_weapon ? 20 : 0));
    if (i == 0 && typ != A_CHA) {
        /* instant kill */
        decreaseCurrentHitPoints(1);
        pline_The("poison was deadly...");
    } else if (i > 5) {
        /* HP damage; more likely--but less severe--with missiles */
        loss = thrown_weapon ? rnd(6) : rn1(10, 6);
        losehp(loss, pkiller, kprefix); /* poison damage */
    } else {
        /* attribute loss; if typ is A_STR, reduction in current and
           maximum HP will occur once strength has dropped down to 3 */
        loss = (thrown_weapon || !fatal) ? 1 : d(2, 2); /* was rn1(3,3) */
        /* check that a stat change was made */
        if (adjattrib(typ, -loss, 1))
            poisontell(typ, TRUE);
    }

    if (currentHitPoints() < 1) {
        killer.format = kprefix;
        Strcpy(killer.name, pkiller);
        /* "Poisoned by a poisoned ___" is redundant */
        done(strstri(pkiller, "poison") ? DIED : POISONING);
    }
    (void) encumber_msg();
}

void
change_luck(n)
register schar n;
{
    increaseCurrentLuck(n);
    if (currentLuck() < 0) {
        if (currentLuck() < minimumPossibleLuck()) {
            setCurrentLuck(minimumPossibleLuck());
        }
    }
    if (currentLuck() > 0) {
        if (currentLuck() > maximumPossibleLuck()) {
            setCurrentLuck(maximumPossibleLuck());
        }
    }
}

int
stone_luck(parameter)
boolean parameter; /* So I can't think up of a good name.  So sue me. --KAA */
{
    register struct obj *otmp;
    register long bonchance = 0;

    for (otmp = invent; otmp; otmp = otmp->nobj)
        if (confers_luck(otmp)) {
            if (otmp->cursed)
                bonchance -= otmp->quan;
            else if (otmp->blessed)
                bonchance += otmp->quan;
            else if (parameter)
                bonchance += otmp->quan;
        }

    return sgn((int) bonchance);
}

/* there has just been an inventory change affecting a luck-granting item */
void
set_moreluck()
{
    int luckbon = stone_luck(TRUE);
    int LUCKADD = 3; /* added value when carrying luck stone */

    if (!luckbon && !carrying(LUCKSTONE))
        setLuckBonus(0);
    else if (luckbon >= 0)
        setLuckBonus(LUCKADD);
    else
        setLuckBonus(-LUCKADD);
}

void
restore_attrib()
{
    int i;

    for (i = 0; i < A_MAX; i++) { /* all temporary losses/gains */

        if (yourTemporaryAttrChange(i) && yourAttrChangeTimeout(i)) {
            decreaseYourAttrChangeTimeout(i, 1);
            if (! yourAttrChangeTimeout(i)) { /* countdown for change */
                increaseYourTemporaryAttrChange(i, (yourTemporaryAttrChange(i) > 0 ? -1 : 1));

                if (yourTemporaryAttrChange(i)) /* reset timer */
                    setYourAttrChangeTimeout(i, (100 / ACURR(A_CON)));
            }
        }
    }
    (void) encumber_msg();
}

#define AVAL 50 /* tune value for exercise gains */

void
exercise(i, inc_or_dec)
int i;
boolean inc_or_dec;
{
    debugpline0("Exercise:");
    if (i == A_INT || i == A_CHA)
        return; /* can't exercise these */

    /* no physical exercise while polymorphed; the body's temporary */
    if (areYouPolymorphed() && i != A_WIS)
        return;

    if (abs(yourAttrChangeFromExercise(i)) < AVAL) {
        /*
         *      Law of diminishing returns (Part I):
         *
         *      Gain is harder at higher attribute values.
         *      79% at "3" --> 0% at "18"
         *      Loss is even at all levels (50%).
         *
         *      Note: *YES* ACURR is the right one to use.
         */
        increaseYourAttrChangeFromExercise(i, ((inc_or_dec) ? (rn2(19) > ACURR(i)) : -rn2(2)));

        debugpline3("%s, %s AttrChangeFromExercise = %d",
                    (i == A_STR) ? "Str" : (i == A_WIS) ? "Wis" : (i == A_DEX)
                                                                      ? "Dex"
                                                                      : "Con",
                    (inc_or_dec) ? "inc" : "dec", yourAttrChangeFromExercise(i));
    }
    if (moves > 0 && (i == A_STR || i == A_CON))
        (void) encumber_msg();
}

STATIC_OVL void
exerper()
{
    if (!(moves % 10)) {
        /* Hunger Checks */

        int hs = (currentNutrition() > 1000) ? SATIATED : (currentNutrition() > 150)
                                                     ? NOT_HUNGRY
                                                     : (currentNutrition() > 50)
                                                           ? HUNGRY
                                                           : (currentNutrition() > 0)
                                                                 ? WEAK
                                                                 : FAINTING;

        debugpline0("exerper: Hunger checks");
        switch (hs) {
        case SATIATED:
            exercise(A_DEX, FALSE);
            if (yourRoleHasPMID(PM_MONK))
                exercise(A_WIS, FALSE);
            break;
        case NOT_HUNGRY:
            exercise(A_CON, TRUE);
            break;
        case WEAK:
            exercise(A_STR, FALSE);
            if (yourRoleHasPMID(PM_MONK)) /* fasting */
                exercise(A_WIS, TRUE);
            break;
        case FAINTING:
        case FAINTED:
            exercise(A_CON, FALSE);
            break;
        }

        /* Encumbrance Checks */
        debugpline0("exerper: Encumber checks");
        switch (near_capacity()) {
        case MOD_ENCUMBER:
            exercise(A_STR, TRUE);
            break;
        case HVY_ENCUMBER:
            exercise(A_STR, TRUE);
            exercise(A_DEX, FALSE);
            break;
        case EXT_ENCUMBER:
            exercise(A_DEX, FALSE);
            exercise(A_CON, FALSE);
            break;
        }
    }

    /* status checks */
    if (!(moves % 5)) {
        debugpline0("exerper: Status checks");
        if (yourIntrinsicHasMask(CLAIRVOYANT, (INTRINSIC | TIMEOUT)) && !youAreBlockedFrom(CLAIRVOYANT))
            exercise(A_WIS, TRUE);
        if (youRegenerate())
            exercise(A_STR, TRUE);

        if (youAreSick() || youAreVomiting())
            exercise(A_CON, FALSE);
        if (youAreConfused() || youAreHallucinating())
            exercise(A_WIS, FALSE);
        if ((youHaveWoundedLegs() && !u.usteed) || youKeepFumbling() || youAreStunned())
            exercise(A_DEX, FALSE);
    }
}

/* exercise/abuse text (must be in attribute order, not botl order);
   phrased as "You must have been [][0]." or "You haven't been [][1]." */
static NEARDATA const char *const exertext[A_MAX][2] = {
    { "exercising diligently", "exercising properly" },           /* Str */
    { 0, 0 },                                                     /* Int */
    { "very observant", "paying attention" },                     /* Wis */
    { "working on your reflexes", "working on reflexes lately" }, /* Dex */
    { "leading a healthy life-style", "watching your health" },   /* Con */
    { 0, 0 },                                                     /* Cha */
};

void
exerchk()
{
    int i, ax, mod_val, lolim, hilim;

    /*  Check out the periodic accumulations */
    exerper();

    if (moves >= context.next_attrib_check) {
        debugpline1("exerchk: ready to test. multi = %d.", multi);
    }
    /*  Are we ready for a test? */
    if (moves >= context.next_attrib_check && !multi) {
        debugpline0("exerchk: testing.");
        /*
         *      Law of diminishing returns (Part II):
         *
         *      The effects of "exercise" and "abuse" wear
         *      off over time.  Even if you *don't* get an
         *      increase/decrease, you lose some of the
         *      accumulated effects.
         */
        for (i = 0; i < A_MAX; ++i) {
            ax = yourAttrChangeFromExercise(i);
            /* nothing to do here if no exercise or abuse has occurred
               (Int and Cha always fall into this category) */
            if (!ax)
                continue; /* ok to skip nextattrib */

            mod_val = sgn(ax); /* +1 or -1; used below */
            /* no further effect for exercise if at max or abuse if at min;
               can't exceed 18 via exercise even if actual max is higher */
            lolim = attributeMinimum(i); /* usually 3; might be higher */
            hilim = attributeMaximum(i); /* usually 18; maybe lower or higher */
            if (hilim > 18)
                hilim = 18;
            if ((ax < 0) ? (yourCurrentAttr(i) <= lolim) : (yourCurrentAttr(i) >= hilim))
                goto nextattrib;
            /* can't exercise non-Wisdom while polymorphed; previous
               exercise/abuse gradually wears off without impact then */
            if (areYouPolymorphed() && i != A_WIS)
                goto nextattrib;

            debugpline2("exerchk: testing %s (%d).",
                        (i == A_STR)
                            ? "Str"
                            : (i == A_INT)
                                  ? "Int?"
                                  : (i == A_WIS)
                                        ? "Wis"
                                        : (i == A_DEX)
                                              ? "Dex"
                                              : (i == A_CON)
                                                    ? "Con"
                                                    : (i == A_CHA)
                                                          ? "Cha?"
                                                          : "???",
                        ax);
            /*
             *  Law of diminishing returns (Part III):
             *
             *  You don't *always* gain by exercising.
             *  [MRS 92/10/28 - Treat Wisdom specially for balance.]
             */
            if (rn2(AVAL) > ((i != A_WIS) ? (abs(ax) * 2 / 3) : abs(ax)))
                goto nextattrib;

            debugpline1("exerchk: changing %d.", i);
            if (adjattrib(i, mod_val, -1)) {
                debugpline1("exerchk: changed %d.", i);
                /* if you actually changed an attrib - zero accumulation */
                setYourAttrChangeFromExercise(i, 0);
                ax = 0;
                /* then print an explanation */
                You("%s %s.",
                    (mod_val > 0) ? "must have been" : "haven't been",
                    exertext[i][(mod_val > 0) ? 0 : 1]);
            }
        nextattrib:
            /* this used to be ``yourAttrChangeFromExercise(i) /= 2''
               but that would produce
               platform-dependent rounding/truncation for negative vals */
            setYourAttrChangeFromExercise(i, ((abs(ax) / 2) * mod_val));
        }
        context.next_attrib_check += rn1(200, 800);
        debugpline1("exerchk: next check at %ld.", context.next_attrib_check);
    }
}

void
init_attr(np)
register int np;
{
    register int i, x, tryct;

    for (i = 0; i < A_MAX; i++) {
	int base = attributeBaseForYourRole(i);
        setYourCurrentAttr(i, base);
        setYourAttrMax(i, base);
        setYourTemporaryAttrChange(i, 0);
        setYourAttrChangeTimeout(i, 0);
        np -= base;
    }

    tryct = 0;
    while (np > 0 && tryct < 100) {
        x = rn2(100);
        for (i = 0; (i < A_MAX) && ((x -= attributeDistributionForYourRole(i)) > 0); i++)
            ;
        if (i >= A_MAX)
            continue; /* impossible */

        if (yourCurrentAttr(i) >= attributeMaximum(i)) {
            tryct++;
            continue;
        }
        tryct = 0;
        increaseYourCurrentAttr(i, 1);
        increaseYourAttrMax(i, 1);
        np--;
    }

    tryct = 0;
    while (np < 0 && tryct < 100) { /* for redistribution */

        x = rn2(100);
        for (i = 0; (i < A_MAX) && ((x -= attributeDistributionForYourRole(i)) > 0); i++)
            ;
        if (i >= A_MAX)
            continue; /* impossible */

        if (yourCurrentAttr(i) <= attributeMinimum(i)) {
            tryct++;
            continue;
        }
        tryct = 0;
        decreaseYourCurrentAttr(i, 1);
        decreaseYourAttrMax(i, 1);
        np++;
    }
}

void
redist_attr()
{
    register int i, tmp;

    for (i = 0; i < A_MAX; i++) {
        if (i == A_INT || i == A_WIS)
            continue;
        /* Polymorphing doesn't change your mind */
        tmp = yourAttrMax(i);
        increaseYourAttrMax(i, (rn2(5) - 2));
        if (yourAttrMax(i) > attributeMaximum(i))
            setYourAttrMax(i, attributeMaximum(i));
        if (yourAttrMax(i) < attributeMinimum(i))
            setYourAttrMax(i, attributeMinimum(i));
        setYourCurrentAttr(i, (yourCurrentAttr(i) * yourAttrMax(i) / tmp));
        /* yourCurrentAttr(i) > attributeMaximum(i) is impossible */
        if (yourCurrentAttr(i) < attributeMinimum(i))
            setYourCurrentAttr(i, attributeMinimum(i));
    }
    (void) encumber_msg();
}

STATIC_OVL
void
postadjabil(intrinsic)
int intrinsic;
{
    if ((intrinsic == WARNING) || (intrinsic == SEE_INVIS))
        see_monsters();
}

STATIC_OVL const struct innate *
check_innate_abil(intrinsic, frommask)
int intrinsic;
long frommask;
{
    const struct innate *abil = 0;

    if (frommask == FROMEXPER)
        switch (yourRolePMID()) {
        case PM_ARCHEOLOGIST:
            abil = arc_abil;
            break;
        case PM_BARBARIAN:
            abil = bar_abil;
            break;
        case PM_CAVEMAN:
            abil = cav_abil;
            break;
        case PM_HEALER:
            abil = hea_abil;
            break;
        case PM_KNIGHT:
            abil = kni_abil;
            break;
        case PM_MONK:
            abil = mon_abil;
            break;
        case PM_PRIEST:
            abil = pri_abil;
            break;
        case PM_RANGER:
            abil = ran_abil;
            break;
        case PM_ROGUE:
            abil = rog_abil;
            break;
        case PM_SAMURAI:
            abil = sam_abil;
            break;
        case PM_TOURIST:
            abil = tou_abil;
            break;
        case PM_VALKYRIE:
            abil = val_abil;
            break;
        case PM_WIZARD:
            abil = wiz_abil;
            break;
        default:
            break;
        }
    else if (frommask == FROMRACE)
        switch (yourSpeciesPMIDAsMale()) {
        case PM_ELF:
            abil = elf_abil;
            break;
        case PM_ORC:
            abil = orc_abil;
            break;
        case PM_HUMAN:
        case PM_DWARF:
        case PM_GNOME:
        default:
            break;
        }

    while (abil && abil->intrinsic) {
        if ((abil->intrinsic == intrinsic) && (currentExperienceLevel() >= abil->ulevel))
            return abil;
        abil++;
    }
    return (struct innate *) 0;
}

/*
 * returns 1 if FROMRACE or FROMEXPER and exper level == 1
 * returns 2 if FROMEXPER and exper level > 1
 * otherwise returns 0
 */
STATIC_OVL int
innately(intrinsic)
int intrinsic;
{
    const struct innate *iptr;

    if ((iptr = check_innate_abil(intrinsic, FROMRACE)) != 0)
        return 1;
    else if ((iptr = check_innate_abil(intrinsic, FROMEXPER)) != 0)
        return (iptr->ulevel == 1) ? 1 : 2;
    return 0;
}

int
is_innate(intrinsic)
int intrinsic;
{
    if (intrinsic == BLINDED && !hasEyes(pmid4you()))
        return 1;
    return innately(intrinsic);
}

char *
from_what(propidx)
int propidx; /* special cases can have negative values */
{
    static char buf[BUFSZ];

    buf[0] = '\0';
    /*
     * Restrict the source of the attributes just to debug mode for now
     */
    if (wizard) {
        static NEARDATA const char because_of[] = " because of %s";

        if (propidx >= 0) {
            char *p;
            struct obj *obj = (struct obj *) 0;
            int innate = is_innate(propidx);

            if (innate == 2)
                Strcpy(buf, " because of your experience");
            else if (innate == 1)
                Strcpy(buf, " innately");
            else if (wizard
                     && (obj = whatGivesExtrinsic(propidx)))
                Sprintf(buf, because_of, obj->oartifact
                                             ? bare_artifactname(obj)
                                             : ysimple_name(obj));
            else if (propidx == BLINDED && permanentlyBlind())
                Sprintf(buf, " from birth");
            else if (propidx == BLINDED && youAreBlindDueToBlindfold())
                Sprintf(buf, because_of, ysimple_name(ublindf));

            /* remove some verbosity and/or redundancy */
            if ((p = strstri(buf, " pair of ")) != 0)
                copynchars(p + 1, p + 9, BUFSZ); /* overlapping buffers ok */
            else if (propidx == STRANGLED
                     && (p = strstri(buf, " of strangulation")) != 0)
                *p = '\0';

        } else { /* negative property index */
            /* if more blocking capabilities get implemented we'll need to
               replace this with what_blocks() comparable to whatGivesExtrinsic() */
            switch (-propidx) {
            case BLINDED:
                if (ublindf
                    && ublindf->oartifact == ART_EYES_OF_THE_OVERWORLD)
                    Sprintf(buf, because_of, bare_artifactname(ublindf));
                break;
            case INVIS:
                if (yourBlockerHasMask(INVIS, W_ARMC))
                    Sprintf(buf, because_of,
                            ysimple_name(uarmc)); /* mummy wrapping */
                break;
            case CLAIRVOYANT:
                if (wizard && (yourBlockerHasMask(CLAIRVOYANT, W_ARMH)))
                    Sprintf(buf, because_of,
                            ysimple_name(uarmh)); /* cornuthaum */
                break;
            }
        }

    } /*wizard*/
    return buf;
}

void
adjabil(oldlevel, newlevel)
int oldlevel, newlevel;
{
    register const struct innate *abil, *rabil;
    long prevabil, mask = FROMEXPER;

    switch (yourRolePMID()) {
    case PM_ARCHEOLOGIST:
        abil = arc_abil;
        break;
    case PM_BARBARIAN:
        abil = bar_abil;
        break;
    case PM_CAVEMAN:
        abil = cav_abil;
        break;
    case PM_HEALER:
        abil = hea_abil;
        break;
    case PM_KNIGHT:
        abil = kni_abil;
        break;
    case PM_MONK:
        abil = mon_abil;
        break;
    case PM_PRIEST:
        abil = pri_abil;
        break;
    case PM_RANGER:
        abil = ran_abil;
        break;
    case PM_ROGUE:
        abil = rog_abil;
        break;
    case PM_SAMURAI:
        abil = sam_abil;
        break;
    case PM_TOURIST:
        abil = tou_abil;
        break;
    case PM_VALKYRIE:
        abil = val_abil;
        break;
    case PM_WIZARD:
        abil = wiz_abil;
        break;
    default:
        abil = 0;
        break;
    }

    switch (yourSpeciesPMIDAsMale()) {
    case PM_ELF:
        rabil = elf_abil;
        break;
    case PM_ORC:
        rabil = orc_abil;
        break;
    case PM_HUMAN:
    case PM_DWARF:
    case PM_GNOME:
    default:
        rabil = 0;
        break;
    }

    while (abil || rabil) {
        /* Have we finished with the intrinsics list? */
        if (!abil || !abil->intrinsic) {
            /* Try the race intrinsics */
            if (!rabil || !rabil->intrinsic)
                break;
            abil = rabil;
            rabil = 0;
            mask = FROMRACE;
        }
        prevabil = yourIntrinsic(abil->intrinsic);
        if (oldlevel < abil->ulevel && newlevel >= abil->ulevel) {
            /* Abilities gained at level 1 can never be lost
             * via level loss, only via means that remove _any_
             * sort of ability.  A "gain" of such an ability from
             * an outside source is devoid of meaning, so we set
             * FROMOUTSIDE to avoid such gains.
             */
            if (abil->ulevel == 1) {
                setYourIntrinsicMask(abil->intrinsic, (mask | FROMOUTSIDE));
	    } else {
                setYourIntrinsicMask(abil->intrinsic, mask);
	    }

            if (!(yourIntrinsic(abil->intrinsic) & INTRINSIC & ~mask)) {
                if (*(abil->gainstr))
                    You_feel("%s!", abil->gainstr);
            }
        } else if (oldlevel >= abil->ulevel && newlevel < abil->ulevel) {
            unsetYourIntrinsicMask(abil->intrinsic, mask);
            if (!(yourIntrinsicHasMask(abil->intrinsic, INTRINSIC))) {
                if (*(abil->losestr))
                    You_feel("%s!", abil->losestr);
                else if (*(abil->gainstr))
                    You_feel("less %s!", abil->gainstr);
            }
        }
        if (prevabil != yourIntrinsic(abil->intrinsic)) /* it changed */
            postadjabil(abil->intrinsic);
        abil++;
    }

    if (oldlevel > 0) {
        if (newlevel > oldlevel)
            add_weapon_skill(newlevel - oldlevel);
        else
            lose_weapon_skill(oldlevel - newlevel);
    }
}

int
newhp()
{
    int hp = 0;
    int conplus;

    hp += hitPointAdvancementForYourRole();
    hp += hitPointAdvancementForYourSpecies();

    if (currentExperienceLevel() == 0) {
        /* Initialize hit points */
        if (moves <= 1L) { /* initial hero; skip for polyself to new man */
            /* Initialize alignment stuff */
            setCurrentAlignmentType(alignmentType(flags.initalign));
            setCurrentAlignmentRecord(initialAlignmentRecordForYourRole());
        }
        /* no Con adjustment for initial hit points */
    } else {
        if (ACURR(A_CON) <= 3)
            conplus = -2;
        else if (ACURR(A_CON) <= 6)
            conplus = -1;
        else if (ACURR(A_CON) <= 14)
            conplus = 0;
        else if (ACURR(A_CON) <= 16)
            conplus = 1;
        else if (ACURR(A_CON) == 17)
            conplus = 2;
        else if (ACURR(A_CON) == 18)
            conplus = 3;
        else
            conplus = 4;
        hp += conplus;
    }
    if (hp <= 0)
        hp = 1;
    if (currentExperienceLevel() < MAXULEV) {
        setHitPointIncreasePerLevel(currentExperienceLevel(), hp);
    }
    return hp;
}

schar
acurr(x)
int x;
{
    register int tmp = (yourAttrBonus(x) + yourTemporaryAttrChange(x) + yourCurrentAttr(x));

    if (x == A_STR) {
        if (tmp >= 125 || (uarmg && uarmg->otyp == GAUNTLETS_OF_POWER))
            return (schar) 125;
        else
#ifdef WIN32_BUG
            return (x = ((tmp <= 3) ? 3 : tmp));
#else
        return (schar) ((tmp <= 3) ? 3 : tmp);
#endif
    } else if (x == A_CHA) {
        if (tmp < 18
            && (monsterClass(pmid4you()) == S_NYMPH || currentMonsterNumber() == PM_SUCCUBUS
                || currentMonsterNumber() == PM_INCUBUS))
            return (schar) 18;
    } else if (x == A_INT || x == A_WIS) {
        /* yes, this may raise int/wis if player is sufficiently
         * stupid.  there are lower levels of cognition than "dunce".
         */
        if (uarmh && uarmh->otyp == DUNCE_CAP)
            return (schar) 6;
    }
#ifdef WIN32_BUG
    return (x = ((tmp >= 25) ? 25 : (tmp <= 3) ? 3 : tmp));
#else
    return (schar) ((tmp >= 25) ? 25 : (tmp <= 3) ? 3 : tmp);
#endif
}

/* condense clumsy ACURR(A_STR) value into value that fits into game formulas
 */
schar
acurrstr()
{
    register int str = ACURR(A_STR);

    if (str <= 18)
        return (schar) str;
    if (str <= 121)
        return (schar) (19 + str / 50); /* map to 19..21 */
    else
        return (schar) (min(str, 125) - 100); /* 22..25 */
}

/* when wearing (or taking off) an unID'd item, this routine is used
   to distinguish between observable +0 result and no-visible-effect
   due to an attribute not being able to exceed maximum or minimum */
boolean
extremeattr(attrindx) /* does attrindx's value match its max or min? */
int attrindx;
{
    /* Fixed abilities and racial MINATTR/MAXATTR aren't relevant here */
    int lolimit = 3, hilimit = 25, curval = ACURR(attrindx);

    /* upper limit for Str is 25 but its value is encoded differently */
    if (attrindx == A_STR) {
        hilimit = STR19(25); /* 125 */
        /* lower limit for Str can also be 25 */
        if (uarmg && uarmg->otyp == GAUNTLETS_OF_POWER)
            lolimit = hilimit;
    }
    /* this exception is hypothetical; the only other worn item affecting
       Int or Wis is another helmet so can't be in use at the same time */
    if (attrindx == A_INT || attrindx == A_WIS) {
        if (uarmh && uarmh->otyp == DUNCE_CAP)
            hilimit = lolimit = 6;
    }

    /* are we currently at either limit? */
    return (curval == lolimit || curval == hilimit) ? TRUE : FALSE;
}

/* avoid possible problems with alignment overflow, and provide a centralized
   location for any future alignment limits */
void
adjalign(n)
int n;
{
    int newalign = currentAlignmentRecord() + n;

    if (n < 0) {
        if (newalign < currentAlignmentRecord())
            setCurrentAlignmentRecord(newalign);
    } else if (newalign > currentAlignmentRecord()) {
        setCurrentAlignmentRecord(newalign);
        if (currentAlignmentRecord() > ALIGNLIM)
            setCurrentAlignmentRecord(ALIGNLIM);
    }
}

/* change hero's alignment type, possibly losing use of artifacts */
void
uchangealign(newalign, reason)
int newalign;
int reason; /* 0==conversion, 1==helm-of-OA on, 2==helm-of-OA off */
{
    aligntyp oldalign = currentAlignmentType();

    setBlessings(0);   /* lose divine protection */
    context.botl = 1; /* status line needs updating */
    if (reason == 0) {
        /* conversion via altar */
        setCurrentAlignmentBase(newalign);
        /* worn helm of opposite alignment might block change */
        if (!uarmh || uarmh->otyp != HELM_OF_OPPOSITE_ALIGNMENT)
            setCurrentAlignmentType((aligntyp) currentAlignmentBase());
        You("have a %ssense of a new direction.",
            (currentAlignmentType() != oldalign) ? "sudden " : "");
    } else {
        /* putting on or taking off a helm of opposite alignment */
        setCurrentAlignmentType((aligntyp) newalign);
        if (reason == 1)
            Your("mind oscillates %s.", youAreHallucinating() ? "wildly" : "briefly");
        else if (reason == 2)
            Your("mind is %s.", youAreHallucinating()
                                    ? "much of a muchness"
                                    : "back in sync with your body");
    }

    if (currentAlignmentType() != oldalign) {
        setCurrentAlignmentRecord(0); /* slate is wiped clean */
        retouch_equipment(0);
    }
}

xchar yourCurrentAttr(int index) {
    return javaGetIntFromInt(PLAYER_CHARACTER_CLASS, "currentAttribute", index);
}

void setYourCurrentAttr(int index, xchar value) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "setCurrentAttribute", index, value);
}

void increaseYourCurrentAttr(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "increaseCurrentAttribute", index, delta);
}

void decreaseYourCurrentAttr(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "decreaseCurrentAttribute", index, delta);
}

xchar yourAttrMax(int index) {
    return javaGetIntFromInt(PLAYER_CHARACTER_CLASS, "maximumAttribute", index);
}

void setYourAttrMax(int index, xchar value) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "setMaximumAttribute", index, value);
}

void increaseYourAttrMax(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "increaseMaximumAttribute", index, delta);
}

void decreaseYourAttrMax(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "decreaseMaximumAttribute", index, delta);
}

xchar yourAttrBonus(int index) {
    return javaGetIntFromInt(PLAYER_CHARACTER_CLASS, "attributeBonus", index);
}

void setYourAttrBonus(int index, xchar value) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "setAttributeBonus", index, value);
}

void increaseYourAttrBonus(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "increaseAttributeBonus", index, delta);
}

void decreaseYourAttrBonus(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "decreaseAttributeBonus", index, delta);
}

xchar yourAttrChangeFromExercise(int index) {
    return javaGetIntFromInt(PLAYER_CHARACTER_CLASS, "attributeChangeFromExertion", index);
}

void setYourAttrChangeFromExercise(int index, xchar value) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "setAttributeChangeFromExertion", index, value);
}

void increaseYourAttrChangeFromExercise(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "increaseAttributeChangeFromExertion", index, delta);
}

void decreaseYourAttrChangeFromExercise(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "decreaseAttributeChangeFromExertion", index, delta);
}

xchar yourAttrAsMonster(int index) {
    return javaGetIntFromInt(PLAYER_CHARACTER_CLASS, "attributeAsMonster", index);
}

void setYourAttrAsMonster(int index, xchar value) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "setAttributeAsMonster", index, value);
}

void increaseYourAttrAsMonster(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "increaseAttributeAsMonster", index, delta);
}

void decreaseYourAttrAsMonster(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "decreaseAttributeAsMonster", index, delta);
}

xchar yourAttrMaxAsMonster(int index) {
    return javaGetIntFromInt(PLAYER_CHARACTER_CLASS, "maximumAttributeAsMonster", index);
}

void setYourAttrMaxAsMonster(int index, xchar value) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "setMaximumAttributeAsMonster", index, value);
}

void increaseYourAttrMaxAsMonster(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "increaseMaximumAttributeAsMonster", index, delta);
}

void decreaseYourAttrMaxAsMonster(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "decreaseMaximumAttributeAsMonster", index, delta);
}

xchar yourTemporaryAttrChange(int index) {
    return javaGetIntFromInt(PLAYER_CHARACTER_CLASS, "temporaryAttributeChange", index);
}

void setYourTemporaryAttrChange(int index, xchar value) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "setTemporaryAttributeChange", index, value);
}

void increaseYourTemporaryAttrChange(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "increaseTemporaryAttributeChange", index, delta);
}

void decreaseYourTemporaryAttrChange(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "decreaseTemporaryAttributeChange", index, delta);
}

xchar yourAttrChangeTimeout(int index) {
    return javaGetIntFromInt(PLAYER_CHARACTER_CLASS, "attributeChangeTimeout", index);
}

void setYourAttrChangeTimeout(int index, xchar value) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "setAttributeChangeTimeout", index, value);
}

void increaseYourAttrChangeTimeout(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "increaseAttributeChangeTimeout", index, delta);
}

void decreaseYourAttrChangeTimeout(int index, xchar delta) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "decreaseAttributeChangeTimeout", index, delta);
}

/* unconscious() includes u.usleep but not is_fainted(); the multi test is
   redundant but allows the function calls to be skipped most of the time */
boolean youAreUnaware() {
   return (multi < 0 && (unconscious() || is_fainted()));
}

boolean youAreAware() {
   return !youAreUnaware();
}

extern boolean youHaveExtrinsic(int index) {
  return (0L != yourExtrinsic(index));
}

extern boolean yourExtrinsicHasMask(int index, long mask) {
  return ((yourExtrinsic(index) & mask) != 0);
}

extern void setYourExtrinsicMask(int index, long mask) {
  long curr = yourExtrinsic(index);
  setYourExtrinsic(index, (curr | mask));
}

extern void unsetYourExtrinsicMask(int index, long mask) {
  long curr = yourExtrinsic(index);
  setYourExtrinsic(index, (curr & ~mask));
}

extern void toggleYourExtrinsicMask(int index, long mask) {
  long curr = yourExtrinsic(index);
  setYourExtrinsic(index, (curr ^ mask));
}

extern boolean youHaveIntrinsic(int index) {
  return (0L != yourIntrinsic(index));
}

extern boolean yourIntrinsicHasMask(int index, long mask) {
  return ((yourIntrinsic(index) & mask) != 0);
}

extern void setYourIntrinsicMask(int index, long mask) {
  long curr = yourIntrinsic(index);
  setYourIntrinsic(index, (curr | mask));
}

extern void unsetYourIntrinsicMask(int index, long mask) {
  long curr = yourIntrinsic(index);
  setYourIntrinsic(index, (curr & ~mask));
}

extern boolean yourBlockerHasMask(int index, long mask) {
  return ((yourBlocker(index) & mask) != 0);
}

extern void setYourBlockerMask(int index, long mask) {
  long curr = yourBlocker(index);
  setYourBlocker(index, (curr | mask));
}

extern void unsetYourBlockerMask(int index, long mask) {
  long curr = yourBlocker(index);
  setYourBlocker(index, (curr & ~mask));
}

extern boolean youAreBlockedFrom(int index) {
  return (0 != yourBlocker(index));
}


extern boolean youResistFire() {
 return (youHaveExtrinsic(FIRE_RES) || youHaveIntrinsic(FIRE_RES));
}

extern boolean youResistCold() {
 return (youHaveExtrinsic(COLD_RES) || youHaveIntrinsic(COLD_RES));
}

extern boolean youResistSleep() {
 return (youHaveExtrinsic(SLEEP_RES) || youHaveIntrinsic(SLEEP_RES));
}

extern boolean youResistDisintegration() {
 return (youHaveExtrinsic(DISINT_RES) || youHaveIntrinsic(DISINT_RES));
}

extern boolean youResistShock() {
 return (youHaveExtrinsic(SHOCK_RES) || youHaveIntrinsic(SHOCK_RES));
}

extern boolean youResistPoison() {
 return (youHaveExtrinsic(POISON_RES) || youHaveIntrinsic(POISON_RES));
}

extern boolean youResistDraining() {
 return (youHaveExtrinsic(DRAIN_RES) || youHaveIntrinsic(DRAIN_RES));
}

extern boolean youResistMagic() {
 return (youHaveExtrinsic(ANTIMAGIC) || youHaveIntrinsic(ANTIMAGIC));
}

extern boolean youResistAcid() {
 return (youHaveExtrinsic(ACID_RES) || youHaveIntrinsic(ACID_RES));
}

extern boolean youResistStoning() {
 return (youHaveExtrinsic(STONE_RES) || youHaveIntrinsic(STONE_RES));
}

extern boolean youResistSickness() {
 return (youHaveExtrinsic(SICK_RES) || youHaveIntrinsic(SICK_RES));
}

extern boolean youResistHallucination() {
 return (youHaveExtrinsic(HALLUC_RES) || youHaveIntrinsic(HALLUC_RES));
}

extern boolean youAreInvulnerable() {
 return (youHaveIntrinsic(INVULNERABLE));
}

extern boolean youAreBeingPunished() {
 return (uball != 0);
}

extern boolean youAreStunned() {
 return (youHaveIntrinsic(STUNNED));
}

extern boolean youAreConfused() {
 return (youHaveIntrinsic(CONFUSION));
}

extern boolean youAreTemporarilyBlinded() {
 return (youHaveIntrinsic(BLINDED));
}

/* you are blind because of a cover */
extern boolean youAreBlindfolded() {
    return (ublindf && ublindf->otyp != LENSES);
}

/* you are blind because of a blindfold, and *only* that */
extern boolean youAreBlindDueToBlindfold() {
    return (youAreBlindfolded() && ublindf->oartifact != ART_EYES_OF_THE_OVERWORLD 
     && !permanentlyBlind() && !youAreTemporarilyBlinded() && hasEyes(pmid4you()));
}

/* you cannot see for whatever reason */
extern boolean youCannotSee() {
    return ((permanentlyBlind() || youAreTemporarilyBlinded() || youAreBlindfolded()
      || !hasEyes(pmid4you())) 
     && !(ublindf && ublindf->oartifact == ART_EYES_OF_THE_OVERWORLD));
    /* ...the Eyes operate even when you really are blind
           or don't have any eyes */
}

extern boolean youCanSee() {
    return !youCannotSee();
}

extern boolean youAreSick() {
 return (youHaveIntrinsic(SICK));
}

extern boolean youAreVomiting() {
 return (youHaveIntrinsic(VOMITING));
}

extern boolean youAreHallucinating() {
 return (youHaveIntrinsic(HALLUC) && !youResistHallucination());
}

extern boolean youHaveSlipperyFingers() {
 return (youHaveIntrinsic(SLIPPERY_FINGERS));
}

extern boolean youAreTurningToStone() {
 return (youHaveIntrinsic(STONED));
}

extern boolean youAreTurningToSlime() {
 return (youHaveIntrinsic(SLIMED));
}

extern boolean youAreBeingStrangled() {
 return (youHaveIntrinsic(STRANGLED));
}

extern boolean youAreDeaf() {
 return (youHaveIntrinsic(DEAF) || youHaveExtrinsic(DEAF));
}

extern boolean youKeepFumbling() {
 return (youHaveIntrinsic(FUMBLING) || youHaveExtrinsic(FUMBLING));
}

extern boolean youHaveWoundedLegs() {
 return (youHaveIntrinsic(WOUNDED_LEGS) || youHaveExtrinsic(WOUNDED_LEGS));
}

extern boolean youAreSleepy() {
 return (youHaveIntrinsic(SLEEPY) || youHaveExtrinsic(SLEEPY));
}

extern boolean youHunger() {
 return (youHaveIntrinsic(HUNGER) || youHaveExtrinsic(HUNGER));
}

extern boolean youCanSeeInvisible() {
 return (youHaveIntrinsic(SEE_INVIS) || youHaveExtrinsic(SEE_INVIS));
}

extern boolean youHaveTelepathyWhenBlind() {
 return (youHaveIntrinsic(TELEPAT) || youHaveExtrinsic(TELEPAT));
}

extern boolean youHaveTelepathyWhenNotBlind() {
 return (youHaveExtrinsic(TELEPAT));
}

extern boolean youSenseWarnings() {
 return (youHaveIntrinsic(WARNING) || youHaveExtrinsic(WARNING));
}

extern boolean youAreWarnedOfMonsters() {
 return (youHaveIntrinsic(WARN_OF_MON) || youHaveExtrinsic(WARN_OF_MON));
}

extern boolean youAreWarnedOfUndead() {
 return (youHaveIntrinsic(WARN_UNDEAD) || youHaveExtrinsic(WARN_UNDEAD));
}

extern boolean youHaveAutomaticSearching() {
 return (youHaveIntrinsic(SEARCHING) || youHaveExtrinsic(SEARCHING));
}

extern boolean youAreClairvoyant() {
  return (youHaveIntrinsic(CLAIRVOYANT) || youHaveExtrinsic(CLAIRVOYANT)) && !youAreBlockedFrom(CLAIRVOYANT);
}

extern boolean youHaveInfravision() {
 return (youHaveIntrinsic(INFRAVISION) || youHaveExtrinsic(INFRAVISION));
}

extern boolean youCanDetectMonsters() {
 return (youHaveIntrinsic(DETECT_MONSTERS) || youHaveExtrinsic(DETECT_MONSTERS));
}

extern boolean youAreAdorned() {
 return (youHaveExtrinsic(ADORNED));
}

extern boolean youAreInvisibleToOthers() {
 return ((youHaveIntrinsic(INVIS) || youHaveExtrinsic(INVIS)) && !youAreBlockedFrom(INVIS));
}

/* Note: invisibility also hides inventory and steed */
extern boolean youAreFullyInvisible() {
 return (youAreInvisibleToOthers() && !youCanSeeInvisible());
}

extern boolean youAppearDisplaced() {
 return (youHaveExtrinsic(DISPLACED));
}

extern boolean youAreStealthy() {
 return ((youHaveIntrinsic(STEALTH) || youHaveExtrinsic(STEALTH)) && !youAreBlockedFrom(STEALTH));
}

extern boolean youAggravateMonsters() {
 return (youHaveIntrinsic(AGGRAVATE_MONSTER) || youHaveExtrinsic(AGGRAVATE_MONSTER));
}

extern boolean youCauseConflict() {
 return (youHaveIntrinsic(CONFLICT) || youHaveExtrinsic(CONFLICT));
}

extern boolean youCanJump() {
 return (youHaveIntrinsic(JUMPING) || youHaveExtrinsic(JUMPING));
}

extern boolean youCanTeleport() {
 return (youHaveIntrinsic(TELEPORT) || youHaveExtrinsic(TELEPORT));
}

extern boolean youHaveTeleportControl() {
 return (youHaveIntrinsic(TELEPORT_CONTROL) || youHaveExtrinsic(TELEPORT_CONTROL));
}

/* Can't touch surface, can't go under water; overrides all others */
extern boolean youAreLevitating() {
 return ((youHaveIntrinsic(LEVITATION) || youHaveExtrinsic(LEVITATION)) && !youAreBlockedFrom(LEVITATION));
}

extern boolean youCanLevitateAtWill() {
  return ((yourIntrinsicHasMask(LEVITATION, I_SPECIAL) || yourExtrinsicHasMask(LEVITATION, W_ARTI))
     && (0L == yourIntrinsic(LEVITATION) & ~(I_SPECIAL | TIMEOUT))
     && (0L == yourExtrinsic(LEVITATION) & ~W_ARTI));
}

/* May touch surface; does not override any others */
extern boolean youAreFlying() {
  return ((youHaveIntrinsic(FLYING) || youHaveExtrinsic(FLYING) || (u.usteed && isFlyer(pmid4mon(u.usteed))))
     && !youAreBlockedFrom(FLYING));
}

/* Get wet, don't go under water unless if amphibious */
extern boolean youCanSwim() {
  return (youHaveIntrinsic(SWIMMING) || youHaveExtrinsic(SWIMMING) ||
		   (u.usteed && isSwimmer(pmid4mon(u.usteed))));
}

/* Get wet, may go under surface */
extern boolean youAreAmphibious() {
  return (youHaveIntrinsic(MAGICAL_BREATHING) || youHaveExtrinsic(MAGICAL_BREATHING) || isAmphibious(pmid4you()));
}

extern boolean youNeedNotBreathe() {
  return (youHaveIntrinsic(MAGICAL_BREATHING) || youHaveExtrinsic(MAGICAL_BREATHING) || doesNotBreathe(pmid4you()));
}

extern boolean youCanPassThroughWalls() {
  return (youHaveIntrinsic(PASSES_WALLS) || youHaveExtrinsic(PASSES_WALLS));
}

extern boolean youHaveSlowDigestion() {
  return (youHaveIntrinsic(SLOW_DIGESTION) || youHaveExtrinsic(SLOW_DIGESTION));
}

extern boolean youTakeHalfDamageFromSpells() {
  return (youHaveIntrinsic(HALF_SPDAM) || youHaveExtrinsic(HALF_SPDAM));
}

/*
 * Physical damage
 *
 * Damage is NOT physical damage if (in order of priority):
 * 1. it already qualifies for some other special category
 *    for which a special resistance already exists in the game
 *    including: cold, fire, shock, acid, and magic.
 *    Note that fire is extended to include all non-acid forms of
 *    burning, even boiling water since that is already dealt with
 *    by fire resistance, and in most or all cases is caused by fire.
 * 2. it doesn't leave a mark. Marks include destruction of, or
 *    damage to, an internal organ (including the brain),
 *    lacerations, bruises, crushed body parts, bleeding.
 *
 * The following were evaluated and determined _NOT_ to be
 * susceptible to Half_physical_damage protection:
 *   Being caught in a fireball                      [fire damage]
 *   Sitting in lava                                 [lava damage]
 *   Thrown potion (acid)                            [acid damage]
 *   Splattered burning oil from thrown potion       [fire damage]
 *   Mixing water and acid                           [acid damage]
 *   Molten lava (entering or being splashed)        [lava damage]
 *   boiling water from a sink                       [fire damage]
 *   Fire traps                                      [fire damage]
 *   Scrolls of fire (confused and otherwise)        [fire damage]
 *   Alchemical explosion                            [not physical]
 *   System shock                                    [shock damage]
 *   Bag of holding explosion                        [magical]
 *   Being undead-turned by your god                 [magical]
 *   Level-drain                                     [magical]
 *   Magical explosion of a magic trap               [magical]
 *   Sitting on a throne with a bad effect           [magical]
 *   Contaminated water from a sink                  [poison/sickness]
 *   Contact-poisoned spellbooks                     [poison/sickness]
 *   Eating acidic/poisonous/mildly-old corpses      [poison/sickness]
 *   Eating a poisoned weapon while polyselfed       [poison/sickness]
 *   Engulfing a zombie or mummy (AT_ENGL in hmonas) [poison/sickness]
 *   Quaffed potions of sickness, lit oil, acid      [poison/sickness]
 *   Pyrolisks' fiery gaze                           [fire damage]
 *   Any passive attack                              [most don't qualify]
 */

extern boolean youTakeHalfDamageFromPhysicalAttacks() {
  return (youHaveIntrinsic(HALF_PHDAM) || youHaveExtrinsic(HALF_PHDAM));
}

extern boolean youRegenerate() {
  return (youHaveIntrinsic(REGENERATION) || youHaveExtrinsic(REGENERATION));
}

extern boolean yourEnergyRegenerates() {
  return (youHaveIntrinsic(ENERGY_REGENERATION) || youHaveExtrinsic(ENERGY_REGENERATION));
}

extern boolean youAreProtected() {
  return (youHaveIntrinsic(PROTECTION) || youHaveExtrinsic(PROTECTION));
}

extern boolean youHaveProtectionFromShapeChangers() {
  return (youHaveIntrinsic(PROT_FROM_SHAPE_CHANGERS) || youHaveExtrinsic(PROT_FROM_SHAPE_CHANGERS));
}

extern boolean youPolymorph() {
  return (youHaveIntrinsic(POLYMORPH) || youHaveExtrinsic(POLYMORPH));
}

extern boolean youHavePolymorphControl() {
  return (youHaveIntrinsic(POLYMORPH_CONTROL) || youHaveExtrinsic(POLYMORPH_CONTROL));
}

extern boolean youAreUnchanging() {
  return (youHaveIntrinsic(UNCHANGING) || youHaveExtrinsic(UNCHANGING));
}

extern boolean youAreFast() {
  return (youHaveIntrinsic(FAST) || youHaveExtrinsic(FAST));
}

extern boolean youAreVeryFast() {
  return ((youHaveIntrinsic(FAST) & ~INTRINSIC)) || youHaveExtrinsic(FAST);
}

extern boolean youCanReflectAttacks() {
  return (youHaveIntrinsic(REFLECTING) || youHaveExtrinsic(REFLECTING));
}

extern boolean youHaveFreeAction() {
  return youHaveExtrinsic(FREE_ACTION);
}

extern boolean youHaveFixedAbilities() {
  return youHaveExtrinsic(FIXED_ABIL);
}

extern boolean yourLifeCanBeSaved() {
  return youHaveExtrinsic(LIFESAVED);
}

extern int attributeMaximum(int index) {
    if ((index == A_STR) && areYouPolymorphed() && isStrongMonster(pmid4you())) {
        return STR18(100);
    } else {
        return yourSpeciesAttributeMaximum(index);
    }
}

extern int attributeMinimum(int index) {
    return yourSpeciesAttributeMinimum(index);
}

/*attrib.c*/
