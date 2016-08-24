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
    long *ability;
    const char *gainstr, *losestr;
} arc_abil[] = { { 1, &(HStealth), "", "" },
                 { 1, &(HFast), "", "" },
                 { 10, &(HSearching), "perceptive", "" },
                 { 0, 0, 0, 0 } },

  bar_abil[] = { { 1, &(HPoison_resistance), "", "" },
                 { 7, &(HFast), "quick", "slow" },
                 { 15, &(HStealth), "stealthy", "" },
                 { 0, 0, 0, 0 } },

  cav_abil[] = { { 7, &(HFast), "quick", "slow" },
                 { 15, &(HWarning), "sensitive", "" },
                 { 0, 0, 0, 0 } },

  hea_abil[] = { { 1, &(HPoison_resistance), "", "" },
                 { 15, &(HWarning), "sensitive", "" },
                 { 0, 0, 0, 0 } },

  kni_abil[] = { { 7, &(HFast), "quick", "slow" }, { 0, 0, 0, 0 } },

  mon_abil[] = { { 1, &(HFast), "", "" },
                 { 1, &(HSleep_resistance), "", "" },
                 { 1, &(HSee_invisible), "", "" },
                 { 3, &(HPoison_resistance), "healthy", "" },
                 { 5, &(HStealth), "stealthy", "" },
                 { 7, &(HWarning), "sensitive", "" },
                 { 9, &(HSearching), "perceptive", "unaware" },
                 { 11, &(HFire_resistance), "cool", "warmer" },
                 { 13, &(HCold_resistance), "warm", "cooler" },
                 { 15, &(HShock_resistance), "insulated", "conductive" },
                 { 17, &(HTeleport_control), "controlled", "uncontrolled" },
                 { 0, 0, 0, 0 } },

  pri_abil[] = { { 15, &(HWarning), "sensitive", "" },
                 { 20, &(HFire_resistance), "cool", "warmer" },
                 { 0, 0, 0, 0 } },

  ran_abil[] = { { 1, &(HSearching), "", "" },
                 { 7, &(HStealth), "stealthy", "" },
                 { 15, &(HSee_invisible), "", "" },
                 { 0, 0, 0, 0 } },

  rog_abil[] = { { 1, &(HStealth), "", "" },
                 { 10, &(HSearching), "perceptive", "" },
                 { 0, 0, 0, 0 } },

  sam_abil[] = { { 1, &(HFast), "", "" },
                 { 15, &(HStealth), "stealthy", "" },
                 { 0, 0, 0, 0 } },

  tou_abil[] = { { 10, &(HSearching), "perceptive", "" },
                 { 20, &(HPoison_resistance), "hardy", "" },
                 { 0, 0, 0, 0 } },

  val_abil[] = { { 1, &(HCold_resistance), "", "" },
                 { 1, &(HStealth), "", "" },
                 { 7, &(HFast), "quick", "slow" },
                 { 0, 0, 0, 0 } },

  wiz_abil[] = { { 15, &(HWarning), "sensitive", "" },
                 { 17, &(HTeleport_control), "controlled", "uncontrolled" },
                 { 0, 0, 0, 0 } },

  /* Intrinsics conferred by race */
    elf_abil[] = { { 4, &(HSleep_resistance), "awake", "tired" },
                   { 0, 0, 0, 0 } },

  orc_abil[] = { { 1, &(HPoison_resistance), "", "" }, { 0, 0, 0, 0 } };

STATIC_DCL void NDECL(exerper);
STATIC_DCL void FDECL(postadjabil, (long *));
STATIC_DCL const struct innate *FDECL(check_innate_abil, (long *, long));
STATIC_DCL int FDECL(innately, (long *));

/* adjust an attribute; return TRUE if change is made, FALSE otherwise */
boolean
adjattrib(ndx, incr, msgflg)
int ndx, incr;
int msgflg; /* positive => no message, zero => message, and */
{           /* negative => conditional (msg if change made) */
    int old_acurr;
    boolean abonflg;
    const char *attrstr;

    if (Fixed_abil || !incr)
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
            if (yourAttrMax(ndx) > ATTRMAX(ndx))
                setYourAttrMax(ndx, ATTRMAX(ndx));
            setYourCurrentAttr(ndx, yourAttrMax(ndx));
        }
        attrstr = plusattr[ndx];
        abonflg = (yourAttrBonus(ndx) < 0);
    } else {
        increaseYourCurrentAttr(ndx, incr);
        if (yourCurrentAttr(ndx) < ATTRMIN(ndx)) {
            incr = yourCurrentAttr(ndx) - ATTRMIN(ndx);
            setYourCurrentAttr(ndx, ATTRMIN(ndx));
            increaseYourAttrMax(ndx, incr);
            if (yourAttrMax(ndx) < ATTRMIN(ndx))
                setYourAttrMax(ndx, ATTRMIN(ndx));
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
        if (Upolyd) {
            u.mh -= 6;
            u.mhmax -= 6;
        } else {
            u.uhp -= 6;
            u.uhpmax -= 6;
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
    if (Poison_resistance) {
        if (!strcmp(reason, "blast"))
            shieldeff(u.ux, u.uy);
        pline_The("poison doesn't seem to affect you.");
        return;
    }

    /* suppress killer prefix if it already has one */
    i = name_to_mon(pkiller);
    if (i >= LOW_PM && (mons[i].geno & G_UNIQ)) {
        kprefix = KILLED_BY;
        if (!type_is_pname(&mons[i]))
            pkiller = the(pkiller);
    } else if (!strncmpi(pkiller, "the ", 4) || !strncmpi(pkiller, "an ", 3)
               || !strncmpi(pkiller, "a ", 2)) {
        /*[ does this need a plural check too? ]*/
        kprefix = KILLED_BY;
    }

    i = !fatal ? 1 : rn2(fatal + (thrown_weapon ? 20 : 0));
    if (i == 0 && typ != A_CHA) {
        /* instant kill */
        u.uhp = -1;
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

    if (u.uhp < 1) {
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
    u.uluck += n;
    if (u.uluck < 0 && u.uluck < LUCKMIN)
        u.uluck = LUCKMIN;
    if (u.uluck > 0 && u.uluck > LUCKMAX)
        u.uluck = LUCKMAX;
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

    if (!luckbon && !carrying(LUCKSTONE))
        u.moreluck = 0;
    else if (luckbon >= 0)
        u.moreluck = LUCKADD;
    else
        u.moreluck = -LUCKADD;
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
    if (Upolyd && i != A_WIS)
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

        int hs = (u.uhunger > 1000) ? SATIATED : (u.uhunger > 150)
                                                     ? NOT_HUNGRY
                                                     : (u.uhunger > 50)
                                                           ? HUNGRY
                                                           : (u.uhunger > 0)
                                                                 ? WEAK
                                                                 : FAINTING;

        debugpline0("exerper: Hunger checks");
        switch (hs) {
        case SATIATED:
            exercise(A_DEX, FALSE);
            if (Role_if(PM_MONK))
                exercise(A_WIS, FALSE);
            break;
        case NOT_HUNGRY:
            exercise(A_CON, TRUE);
            break;
        case WEAK:
            exercise(A_STR, FALSE);
            if (Role_if(PM_MONK)) /* fasting */
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
        if ((HClairvoyant & (INTRINSIC | TIMEOUT)) && !BClairvoyant)
            exercise(A_WIS, TRUE);
        if (HRegeneration)
            exercise(A_STR, TRUE);

        if (Sick || Vomiting)
            exercise(A_CON, FALSE);
        if (Confusion || Hallucination)
            exercise(A_WIS, FALSE);
        if ((Wounded_legs && !u.usteed) || Fumbling || HStun)
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
            lolim = ATTRMIN(i); /* usually 3; might be higher */
            hilim = ATTRMAX(i); /* usually 18; maybe lower or higher */
            if (hilim > 18)
                hilim = 18;
            if ((ax < 0) ? (yourCurrentAttr(i) <= lolim) : (yourCurrentAttr(i) >= hilim))
                goto nextattrib;
            /* can't exercise non-Wisdom while polymorphed; previous
               exercise/abuse gradually wears off without impact then */
            if (Upolyd && i != A_WIS)
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
        setYourCurrentAttr(i, urole.attrbase[i]);
        setYourAttrMax(i,     urole.attrbase[i]);
        setYourTemporaryAttrChange(i, 0);
        setYourAttrChangeTimeout(i, 0);
        np -= urole.attrbase[i];
    }

    tryct = 0;
    while (np > 0 && tryct < 100) {
        x = rn2(100);
        for (i = 0; (i < A_MAX) && ((x -= urole.attrdist[i]) > 0); i++)
            ;
        if (i >= A_MAX)
            continue; /* impossible */

        if (yourCurrentAttr(i) >= ATTRMAX(i)) {
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
        for (i = 0; (i < A_MAX) && ((x -= urole.attrdist[i]) > 0); i++)
            ;
        if (i >= A_MAX)
            continue; /* impossible */

        if (yourCurrentAttr(i) <= ATTRMIN(i)) {
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
        if (yourAttrMax(i) > ATTRMAX(i))
            setYourAttrMax(i, ATTRMAX(i));
        if (yourAttrMax(i) < ATTRMIN(i))
            setYourAttrMax(i, ATTRMIN(i));
        setYourCurrentAttr(i, (yourCurrentAttr(i) * yourAttrMax(i) / tmp));
        /* yourCurrentAttr(i) > ATTRMAX(i) is impossible */
        if (yourCurrentAttr(i) < ATTRMIN(i))
            setYourCurrentAttr(i, ATTRMIN(i));
    }
    (void) encumber_msg();
}

STATIC_OVL
void
postadjabil(ability)
long *ability;
{
    if (!ability)
        return;
    if (ability == &(HWarning) || ability == &(HSee_invisible))
        see_monsters();
}

STATIC_OVL const struct innate *
check_innate_abil(ability, frommask)
long *ability;
long frommask;
{
    const struct innate *abil = 0;

    if (frommask == FROMEXPER)
        switch (Role_switch) {
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
        switch (Race_switch) {
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

    while (abil && abil->ability) {
        if ((abil->ability == ability) && (u.ulevel >= abil->ulevel))
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
innately(ability)
long *ability;
{
    const struct innate *iptr;

    if ((iptr = check_innate_abil(ability, FROMRACE)) != 0)
        return 1;
    else if ((iptr = check_innate_abil(ability, FROMEXPER)) != 0)
        return (iptr->ulevel == 1) ? 1 : 2;
    return 0;
}

int
is_innate(propidx)
int propidx;
{
    if (propidx == BLINDED && !haseyes(youmonst.data))
        return 1;
    return innately(&u.uprops[propidx].intrinsic);
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
                     && (obj = what_gives(&u.uprops[propidx].extrinsic)))
                Sprintf(buf, because_of, obj->oartifact
                                             ? bare_artifactname(obj)
                                             : ysimple_name(obj));
            else if (propidx == BLINDED && u.uroleplay.blind)
                Sprintf(buf, " from birth");
            else if (propidx == BLINDED && Blindfolded_only)
                Sprintf(buf, because_of, ysimple_name(ublindf));

            /* remove some verbosity and/or redundancy */
            if ((p = strstri(buf, " pair of ")) != 0)
                copynchars(p + 1, p + 9, BUFSZ); /* overlapping buffers ok */
            else if (propidx == STRANGLED
                     && (p = strstri(buf, " of strangulation")) != 0)
                *p = '\0';

        } else { /* negative property index */
            /* if more blocking capabilities get implemented we'll need to
               replace this with what_blocks() comparable to what_gives() */
            switch (-propidx) {
            case BLINDED:
                if (ublindf
                    && ublindf->oartifact == ART_EYES_OF_THE_OVERWORLD)
                    Sprintf(buf, because_of, bare_artifactname(ublindf));
                break;
            case INVIS:
                if (u.uprops[INVIS].blocked & W_ARMC)
                    Sprintf(buf, because_of,
                            ysimple_name(uarmc)); /* mummy wrapping */
                break;
            case CLAIRVOYANT:
                if (wizard && (u.uprops[CLAIRVOYANT].blocked & W_ARMH))
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

    switch (Role_switch) {
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

    switch (Race_switch) {
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
        if (!abil || !abil->ability) {
            /* Try the race intrinsics */
            if (!rabil || !rabil->ability)
                break;
            abil = rabil;
            rabil = 0;
            mask = FROMRACE;
        }
        prevabil = *(abil->ability);
        if (oldlevel < abil->ulevel && newlevel >= abil->ulevel) {
            /* Abilities gained at level 1 can never be lost
             * via level loss, only via means that remove _any_
             * sort of ability.  A "gain" of such an ability from
             * an outside source is devoid of meaning, so we set
             * FROMOUTSIDE to avoid such gains.
             */
            if (abil->ulevel == 1)
                *(abil->ability) |= (mask | FROMOUTSIDE);
            else
                *(abil->ability) |= mask;
            if (!(*(abil->ability) & INTRINSIC & ~mask)) {
                if (*(abil->gainstr))
                    You_feel("%s!", abil->gainstr);
            }
        } else if (oldlevel >= abil->ulevel && newlevel < abil->ulevel) {
            *(abil->ability) &= ~mask;
            if (!(*(abil->ability) & INTRINSIC)) {
                if (*(abil->losestr))
                    You_feel("%s!", abil->losestr);
                else if (*(abil->gainstr))
                    You_feel("less %s!", abil->gainstr);
            }
        }
        if (prevabil != *(abil->ability)) /* it changed */
            postadjabil(abil->ability);
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
    int hp, conplus;

    if (u.ulevel == 0) {
        /* Initialize hit points */
        hp = urole.hpadv.infix + urace.hpadv.infix;
        if (urole.hpadv.inrnd > 0)
            hp += rnd(urole.hpadv.inrnd);
        if (urace.hpadv.inrnd > 0)
            hp += rnd(urace.hpadv.inrnd);
        if (moves <= 1L) { /* initial hero; skip for polyself to new man */
            /* Initialize alignment stuff */
            u.ualign.type = aligns[flags.initalign].value;
            u.ualign.record = urole.initrecord;
        }
        /* no Con adjustment for initial hit points */
    } else {
        if (u.ulevel < urole.xlev) {
            hp = urole.hpadv.lofix + urace.hpadv.lofix;
            if (urole.hpadv.lornd > 0)
                hp += rnd(urole.hpadv.lornd);
            if (urace.hpadv.lornd > 0)
                hp += rnd(urace.hpadv.lornd);
        } else {
            hp = urole.hpadv.hifix + urace.hpadv.hifix;
            if (urole.hpadv.hirnd > 0)
                hp += rnd(urole.hpadv.hirnd);
            if (urace.hpadv.hirnd > 0)
                hp += rnd(urace.hpadv.hirnd);
        }
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
    if (u.ulevel < MAXULEV)
        u.uhpinc[u.ulevel] = (xchar) hp;
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
            && (youmonst.data->mlet == S_NYMPH || u.umonnum == PM_SUCCUBUS
                || u.umonnum == PM_INCUBUS))
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
    /* Fixed_abil and racial MINATTR/MAXATTR aren't relevant here */
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
    int newalign = u.ualign.record + n;

    if (n < 0) {
        if (newalign < u.ualign.record)
            u.ualign.record = newalign;
    } else if (newalign > u.ualign.record) {
        u.ualign.record = newalign;
        if (u.ualign.record > ALIGNLIM)
            u.ualign.record = ALIGNLIM;
    }
}

/* change hero's alignment type, possibly losing use of artifacts */
void
uchangealign(newalign, reason)
int newalign;
int reason; /* 0==conversion, 1==helm-of-OA on, 2==helm-of-OA off */
{
    aligntyp oldalign = u.ualign.type;

    u.ublessed = 0;   /* lose divine protection */
    context.botl = 1; /* status line needs updating */
    if (reason == 0) {
        /* conversion via altar */
        u.ualignbase[A_CURRENT] = (aligntyp) newalign;
        /* worn helm of opposite alignment might block change */
        if (!uarmh || uarmh->otyp != HELM_OF_OPPOSITE_ALIGNMENT)
            u.ualign.type = u.ualignbase[A_CURRENT];
        You("have a %ssense of a new direction.",
            (u.ualign.type != oldalign) ? "sudden " : "");
    } else {
        /* putting on or taking off a helm of opposite alignment */
        u.ualign.type = (aligntyp) newalign;
        if (reason == 1)
            Your("mind oscillates %s.", Hallucination ? "wildly" : "briefly");
        else if (reason == 2)
            Your("mind is %s.", Hallucination
                                    ? "much of a muchness"
                                    : "back in sync with your body");
    }

    if (u.ualign.type != oldalign) {
        u.ualign.record = 0; /* slate is wiped clean */
        retouch_equipment(0);
    }
}

jclass getJavaClass(const char* className) {
    jclass javaClass = (*jni_env)->FindClass(jni_env, className);
    if ((*jni_env)->ExceptionCheck(jni_env)) {
        (*jni_env)->ExceptionDescribe(jni_env);
    }
    return javaClass;
}

jmethodID getStaticMethod(jclass javaClass, const char* methodName, const char* typeSignature) {
    jmethodID method = (*jni_env)->GetStaticMethodID(jni_env, javaClass, methodName, typeSignature);
    if ((*jni_env)->ExceptionCheck(jni_env)) {
        (*jni_env)->ExceptionDescribe(jni_env);
    }
    return method;
}

xchar yourCurrentAttr(int index) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "currentAttribute", "(I)I");
    return (*jni_env)->CallStaticIntMethod(jni_env, you_class, method, index);
}

void setYourCurrentAttr(int index, xchar value) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "setCurrentAttribute", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, value);
}

void increaseYourCurrentAttr(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "increaseCurrentAttribute", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

void decreaseYourCurrentAttr(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "decreaseCurrentAttribute", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

xchar yourAttrMax(int index) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "maximumAttribute", "(I)I");
    return (*jni_env)->CallStaticIntMethod(jni_env, you_class, method, index);
}

void setYourAttrMax(int index, xchar value) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "setMaximumAttribute", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, value);
}

void increaseYourAttrMax(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "increaseMaximumAttribute", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

void decreaseYourAttrMax(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "decreaseMaximumAttribute", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

xchar yourAttrBonus(int index) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "attributeBonus", "(I)I");
    return (*jni_env)->CallStaticIntMethod(jni_env, you_class, method, index);
}

void setYourAttrBonus(int index, xchar value) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "setAttributeBonus", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, value);
}

void increaseYourAttrBonus(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "increaseAttributeBonus", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

void decreaseYourAttrBonus(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "decreaseAttributeBonus", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

xchar yourAttrChangeFromExercise(int index) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "attributeChangeFromExertion", "(I)I");
    return (*jni_env)->CallStaticIntMethod(jni_env, you_class, method, index);
}

void setYourAttrChangeFromExercise(int index, xchar value) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "setAttributeChangeFromExertion", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, value);
}

void increaseYourAttrChangeFromExercise(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "increaseAttributeChangeFromExertion", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

void decreaseYourAttrChangeFromExercise(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "decreaseAttributeChangeFromExertion", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

xchar yourAttrAsMonster(int index) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "attributeAsMonster", "(I)I");
    return (*jni_env)->CallStaticIntMethod(jni_env, you_class, method, index);
}

void setYourAttrAsMonster(int index, xchar value) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "setAttributeAsMonster", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, value);
}

void increaseYourAttrAsMonster(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "increaseAttributeAsMonster", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

void decreaseYourAttrAsMonster(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "decreaseAttributeAsMonster", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

xchar yourAttrMaxAsMonster(int index) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "maximumAttributeAsMonster", "(I)I");
    return (*jni_env)->CallStaticIntMethod(jni_env, you_class, method, index);
}

void setYourAttrMaxAsMonster(int index, xchar value) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "setMaximumAttributeAsMonster", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, value);
}

void increaseYourAttrMaxAsMonster(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "increaseMaximumAttributeAsMonster", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

void decreaseYourAttrMaxAsMonster(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "decreaseMaximumAttributeAsMonster", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

xchar yourTemporaryAttrChange(int index) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "temporaryAttributeChange", "(I)I");
    return (*jni_env)->CallStaticIntMethod(jni_env, you_class, method, index);
}

void setYourTemporaryAttrChange(int index, xchar value) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "setTemporaryAttributeChange", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, value);
}

void increaseYourTemporaryAttrChange(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "increaseTemporaryAttributeChange", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

void decreaseYourTemporaryAttrChange(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "decreaseTemporaryAttributeChange", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

xchar yourAttrChangeTimeout(int index) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "attributeChangeTimeout", "(I)I");
    return (*jni_env)->CallStaticIntMethod(jni_env, you_class, method, index);
}

void setYourAttrChangeTimeout(int index, xchar value) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "setAttributeChangeTimeout", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, value);
}

void increaseYourAttrChangeTimeout(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "increaseAttributeChangeTimeout", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

void decreaseYourAttrChangeTimeout(int index, xchar delta) {
    jclass you_class = getJavaClass("rec/games/roguelike/nh4j/PlayerCharacter");
    jmethodID method = getStaticMethod(you_class, "decreaseAttributeChangeTimeout", "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, index, delta);
}

/*attrib.c*/
