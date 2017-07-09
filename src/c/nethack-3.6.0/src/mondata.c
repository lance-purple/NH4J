/* NetHack 3.6	mondata.c	$NHDT-Date: 1446604115 2015/11/04 02:28:35 $  $NHDT-Branch: master $:$NHDT-Revision: 1.58 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

struct Attack NO_ATTACK = { 0, 0, 0, 0 };

static boolean monsterCanCauseDamageTypeWithAttackType(const struct permonst *, const int, const int);


/*      These routines provide basic data for any type of monster. */

/* set up an individual monster's base type (initial creation, shapechange) */
void
set_mon_data(mon, ptr, flag)
struct monst *mon;
struct permonst *ptr;
int flag;
{
    mon->data = ptr;
    if (flag == -1)
        return; /* "don't care" */

    if (flag == 1)
        mon->mintrinsics |= (monsterResistances(pmid4(ptr)) & 0x00FF);
    else
        mon->mintrinsics = (monsterResistances(pmid4(ptr)) & 0x00FF);
    return;
}


const struct Attack
monsterAttackWithDamageType(ptr, attackType, damageType)
struct permonst *ptr;
const int attackType;
const int damageType;
{
    int pmid = pmid4(ptr);
    int nAttacks = monsterAttacks(pmid);
    for (int i = 0; i < nAttacks; i++)
    {
	struct Attack mattk = monsterAttack(pmid, i);
        if (attackType == mattk.type)
	{
	    if ((damageType == AD_ANY) || (damageType == mattk.damageType))
	    {
                return mattk;
	    }
	}
    } 
    return NO_ATTACK;
}

/* does monster-type have any attack for a specific type of damage? */
boolean monsterHasAttackWithDamageType(ptr, attackType, damageType)
struct permonst *ptr;
const int attackType;
const int damageType;
{
    return validAttack(monsterAttackWithDamageType(ptr, attackType, damageType));
}

/* does monster-type have a particular type of attack */
boolean
attacktype(ptr, atyp)
struct permonst *ptr;
int atyp;
{
    return monsterHasAttackWithDamageType(ptr, atyp, AD_ANY);
}

/* does monster-type transform into something else when petrified? */
boolean
poly_when_stoned(ptr)
struct permonst *ptr;
{
    /* non-stone golems turn into stone golems unless latter is genocided */
    return (boolean) (isGolem(pmid4(ptr)) && ptr != &mons[PM_STONE_GOLEM]
                      && !(mvitals[PM_STONE_GOLEM].mvflags & G_GENOD));
    /* allow G_EXTINCT */
}

/* returns True if monster is drain-life resistant */
boolean
resists_drli(mon)
struct monst *mon;
{
    struct permonst *ptr = mon->data;
    int pmid = pmid4(ptr);
    struct obj *wep;

    if (isUndead(pmid) || isDemon(pmid) || isWere(pmid)
        /* isWere() doesn't handle hero in human form */
        || (mon == &youmonst && lycanthropeType() >= LOW_PM)
        || ptr == &mons[PM_DEATH] || is_vampshifter(mon))
        return TRUE;
    wep = (mon == &youmonst) ? uwep : MON_WEP(mon);
    return (boolean) (wep && wep->oartifact && defends(AD_DRLI, wep));
}

/* True if monster is magic-missile (actually, general magic) resistant */
boolean
resists_magm(mon)
struct monst *mon;
{
    struct permonst *ptr = mon->data;
    boolean is_you = (mon == &youmonst);
    long slotmask;
    struct obj *o;

    /* as of 3.2.0:  gray dragons, Angels, Oracle, Yeenoghu */
    if (dmgtype(ptr, AD_MAGM) || ptr == &mons[PM_BABY_GRAY_DRAGON]
        || dmgtype(ptr, AD_RBRE)) /* Chromatic Dragon */
        return TRUE;
    /* check for magic resistance granted by wielded weapon */
    o = is_you ? uwep : MON_WEP(mon);
    if (o && o->oartifact && defends(AD_MAGM, o))
        return TRUE;
    /* check for magic resistance granted by worn or carried items */
    o = is_you ? invent : mon->minvent;
    slotmask = W_ARMOR | W_ACCESSORY;
    if (!is_you /* assumes monsters don't wield non-weapons */
        || (uwep && (uwep->oclass == WEAPON_CLASS || is_weptool(uwep))))
        slotmask |= W_WEP;
    if (is_you && usingTwoWeapons())
        slotmask |= W_SWAPWEP;
    for (; o; o = o->nobj)
        if (((o->owornmask & slotmask) != 0L
             && objects[o->otyp].oc_oprop == ANTIMAGIC)
            || (o->oartifact && defends_when_carried(AD_MAGM, o)))
            return TRUE;
    return FALSE;
}

/* True iff monster is resistant to light-induced blindness */
boolean
resists_blnd(mon)
struct monst *mon;
{
    struct permonst *ptr = mon->data;
    boolean is_you = (mon == &youmonst);
    long slotmask;
    struct obj *o;

    if (is_you ? (youCannotSee() || youAreUnaware())
               : (mon->mblinded || !mon->mcansee || !hasEyes(pmid4(ptr))
                  /* BUG: temporary sleep sets mfrozen, but since
                          paralysis does too, we can't check it */
                  || mon->msleeping))
        return TRUE;
    /* yellow light, Archon; !dust vortex, !cobra, !raven */
    if (monsterCanCauseDamageTypeWithAttackType(ptr, AD_BLND, AT_EXPL)
        || monsterCanCauseDamageTypeWithAttackType(ptr, AD_BLND, AT_GAZE))
        return TRUE;
    o = is_you ? uwep : MON_WEP(mon);
    if (o && o->oartifact && defends(AD_BLND, o))
        return TRUE;
    o = is_you ? invent : mon->minvent;
    slotmask = W_ARMOR | W_ACCESSORY;
    if (!is_you /* assumes monsters don't wield non-weapons */
        || (uwep && (uwep->oclass == WEAPON_CLASS || is_weptool(uwep))))
        slotmask |= W_WEP;
    if (is_you && usingTwoWeapons())
        slotmask |= W_SWAPWEP;
    for (; o; o = o->nobj)
        if (((o->owornmask & slotmask) != 0L
             && objects[o->otyp].oc_oprop == BLINDED)
            || (o->oartifact && defends_when_carried(AD_BLND, o)))
            return TRUE;
    return FALSE;
}

/* True iff monster can be blinded by the given attack;
   note: may return True when mdef is blind (e.g. new cream-pie attack) */
boolean
can_blnd(magr, mdef, type, obj)
struct monst *magr; /* NULL == no specific aggressor */
struct monst *mdef;
uchar type;
struct obj *obj; /* type == AT_WEAP, AT_SPIT */
{
    boolean is_you = (mdef == &youmonst);
    boolean check_visor = FALSE;
    struct obj *o;
    const char *s;

    /* no eyes protect against all attacks for now */
    if (!hasEyes(pmid4mon(mdef)))
        return FALSE;

    switch (type) {
    case AT_EXPL:
    case AT_BOOM:
    case AT_GAZE:
    case AT_MAGC:
    case AT_BREA: /* assumed to be lightning */
        /* light-based attacks may be cancelled or resisted */
        if (magr && magr->mcan)
            return FALSE;
        return !resists_blnd(mdef);

    case AT_WEAP:
    case AT_SPIT:
    case AT_NONE:
        /* an object is used (thrown/spit/other) */
        if (obj && (obj->otyp == CREAM_PIE)) {
            if (is_you && youAreBlindfolded())
                return FALSE;
        } else if (obj && (obj->otyp == BLINDING_VENOM)) {
            /* all ublindf, including LENSES, protect, cream-pies too */
            if (is_you && (ublindf || creamed()))
                return FALSE;
            check_visor = TRUE;
        } else if (obj && (obj->otyp == POT_BLINDNESS)) {
            return TRUE; /* no defense */
        } else
            return FALSE; /* other objects cannot cause blindness yet */
        if ((magr == &youmonst) && swallowed())
            return FALSE; /* can't affect eyes while inside monster */
        break;

    case AT_ENGL:
        if (is_you && (youAreBlindfolded() || youAreUnaware() || creamed()))
            return FALSE;
        if (!is_you && mdef->msleeping)
            return FALSE;
        break;

    case AT_CLAW:
        /* e.g. raven: all ublindf, including LENSES, protect */
        if (is_you && ublindf)
            return FALSE;
        if ((magr == &youmonst) && swallowed())
            return FALSE; /* can't affect eyes while inside monster */
        check_visor = TRUE;
        break;

    case AT_TUCH:
    case AT_STNG:
        /* some physical, blind-inducing attacks can be cancelled */
        if (magr && magr->mcan)
            return FALSE;
        break;

    default:
        break;
    }

    /* check if wearing a visor (only checked if visor might help) */
    if (check_visor) {
        o = (mdef == &youmonst) ? invent : mdef->minvent;
        for (; o; o = o->nobj)
            if ((o->owornmask & W_ARMH)
                && (s = OBJ_DESCR(objects[o->otyp])) != (char *) 0
                && !strcmp(s, "visored helmet"))
                return FALSE;
    }

    return TRUE;
}

/* returns True if monster can attack at range */
boolean
ranged_attk(ptr)
struct permonst *ptr;
{
    long atk_mask = (1L << AT_BREA) | (1L << AT_SPIT) | (1L << AT_GAZE);

    /* was: (attacktype(ptr, AT_BREA) || attacktype(ptr, AT_WEAP)
     *       || attacktype(ptr, AT_SPIT) || attacktype(ptr, AT_GAZE)
     *       || attacktype(ptr, AT_MAGC));
     * but that's too slow -dlc
     */
    int pmid = pmid4(ptr);
    int nAttacks = monsterAttacks(pmid);
    for (int i = 0; i < nAttacks; i++) {
        int atyp = monsterAttack(pmid, i).type;
        if (atyp >= AT_WEAP)
            return TRUE;
        /* assert(atyp < 32); */
        if ((atk_mask & (1L << atyp)) != 0L)
            return TRUE;
    }
    return FALSE;
}

/* True if specific monster is especially affected by silver weapons */
boolean
mon_hates_silver(mon)
struct monst *mon;
{
    return (boolean) (is_vampshifter(mon) || hates_silver(mon->data));
}

/* True if monster-type is especially affected by silver weapons */
boolean
hates_silver(ptr)
register struct permonst *ptr;
{
    int pmid = pmid4(ptr);
    int mc = monsterClass(pmid);
    return (boolean) (isWere(pmid) || mc == S_VAMPIRE || isDemon(pmid)
                      || ptr == &mons[PM_SHADE]
                      || (mc == S_IMP && ptr != &mons[PM_TENGU]));
}

/* True iff the type of monster pass through iron bars */
boolean
passes_bars(mptr)
struct permonst *mptr;
{
    int pmid = pmid4(mptr);
    return (boolean) (passesThroughWalls(pmid) || isAmorphous(pmid) || isUnsolid(pmid)
                      || isWhirly(pmid) || isVerySmallMonster(pmid)
                      || dmgtype(mptr, AD_CORR) || dmgtype(mptr, AD_RUST)
                      || (isSlithy(pmid) && !isBigMonster(pmid)));
}

/* returns True if monster can blow (whistle, etc) */
boolean
can_blow(mtmp)
register struct monst *mtmp;
{
    int pmid = pmid4mon(mtmp);
    if ((isSilent(pmid) || monsterSound(pmid) == MS_BUZZ)
        && (doesNotBreathe(pmid) || isVerySmallMonster(pmid)
            || !hasAHead(pmid) || monsterClass(pmid) == S_EEL))
        return FALSE;
    if ((mtmp == &youmonst) && youAreBeingStrangled())
        return FALSE;
    return TRUE;
}

/* True if mon is vulnerable to strangulation */
boolean
can_be_strangled(mon)
struct monst *mon;
{
    struct obj *mamul;
    boolean nonbreathing, nobrainer;

    /* For amulet of strangulation support:  here we're considering
       strangulation to be loss of blood flow to the brain due to
       constriction of the arteries in the neck, so all headless
       creatures are immune (no neck) as are mindless creatures
       who don't need to breathe (brain, if any, doesn't care).
       Mindless creatures who do need to breath are vulnerable, as
       are non-breathing creatures which have higher brain function. */
    if (!hasAHead(pmid4mon(mon)))
        return FALSE;
    if (mon == &youmonst) {
        /* hero can't be mindless but poly'ing into mindless form can
           confer strangulation protection */
        nobrainer = isMindless(pmid4you());
        nonbreathing = youNeedNotBreathe();
    } else {
        nobrainer = isMindless(pmid4mon(mon));
        /* monsters don't wear amulets of magical breathing,
           so second part doesn't achieve anything useful... */
        nonbreathing = (doesNotBreathe(pmid4mon(mon))
                        || ((mamul = which_armor(mon, W_AMUL)) != 0
                            && (mamul->otyp == AMULET_OF_MAGICAL_BREATHING)));
    }
    return (boolean) (!nobrainer || !nonbreathing);
}

/* returns True if monster can track well */
boolean
can_track(ptr)
register struct permonst *ptr;
{
    if (uwep && uwep->oartifact == ART_EXCALIBUR)
        return TRUE;
    else
        return (boolean) hasEyes(pmid4(ptr));
}

/* creature will slide out of armor */
boolean
sliparm(ptr)
register struct permonst *ptr;
{
    return (boolean) (isWhirly(pmid4(ptr)) || monsterSize(pmid4(ptr)) <= MZ_SMALL
                      || isNoncorporeal(pmid4(ptr)));
}

/* creature will break out of armor */
boolean
breakarm(ptr)
register struct permonst *ptr;
{
    if (sliparm(ptr))
        return FALSE;

    return (boolean) (isBigMonster(pmid4(ptr))
                      || (monsterSize(pmid4(ptr)) > MZ_SMALL && !isHumanoid(pmid4(ptr)))
                      /* special cases of humanoids that cannot wear suits */
                      || ptr == &mons[PM_MARILITH]
                      || ptr == &mons[PM_WINGED_GARGOYLE]);
}

/* creature sticks other creatures it hits */
boolean
sticks(ptr)
register struct permonst *ptr;
{
    return (boolean) (dmgtype(ptr, AD_STCK) || dmgtype(ptr, AD_WRAP)
                      || attacktype(ptr, AT_HUGS));
}

/* some monster-types can't vomit */
boolean
cantvomit(ptr)
struct permonst *ptr;
{
    /* rats and mice are incapable of vomiting;
       which other creatures have the same limitation? */
    if (monsterClass(pmid4(ptr)) == S_RODENT && ptr != &mons[PM_ROCK_MOLE]
        && ptr != &mons[PM_WOODCHUCK])
        return TRUE;
    return FALSE;
}

/* does monster-type deal out a particular type of damage from a particular
   type of attack? */
boolean monsterCanCauseDamageTypeWithAttackType(ptr, dtyp, atyp)
const struct permonst *ptr;
const int dtyp;
const int atyp;
{
    int pmid = pmid4(ptr);
    int nAttacks = monsterAttacks(pmid);

    for (int i = 0; i < nAttacks; i++)
    {
        struct Attack mattk = monsterAttack(pmid, i);
        if (mattk.damageType == dtyp && (atyp == AT_ANY || mattk.type == atyp)) {
            return TRUE;
	}
    }
    return FALSE;
}

/* does monster-type deal out a particular type of damage from any attack */
boolean
dmgtype(ptr, dtyp)
struct permonst *ptr;
int dtyp;
{
    return monsterCanCauseDamageTypeWithAttackType(ptr, dtyp, AT_ANY);
}

/* returns the maximum damage a defender can do to the attacker via
   a passive defense */
int
max_passive_dmg(mdef, magr)
register struct monst *mdef, *magr;
{
    int i, dmg = 0, multi2 = 0;

    /* each attack by magr can result in passive damage */
    int magrpmid = pmid4mon(magr);
    int nAgressorAttacks = monsterAttacks(magrpmid);
    for (i = 0; i < nAgressorAttacks; i++) {
        switch (monsterAttack(magrpmid, i).type) {
        case AT_CLAW:
        case AT_BITE:
        case AT_KICK:
        case AT_BUTT:
        case AT_TUCH:
        case AT_STNG:
        case AT_HUGS:
        case AT_ENGL:
        case AT_TENT:
        case AT_WEAP:
            multi2++;
            break;
        default:
            break;
        }
    }

    int mdefpmid = pmid4mon(mdef);
    int nDefenderAttacks = monsterAttacks(mdefpmid);
    for (i = 0; i < nDefenderAttacks; i++) {
	struct Attack mattk = monsterAttack(mdefpmid, i);
        if (mattk.type == AT_NONE || mattk.type == AT_BOOM) {
            int damageType = mattk.damageType;
            if ((damageType == AD_ACID && !resists_acid(magr))
                || (damageType == AD_COLD && !resists_cold(magr))
                || (damageType == AD_FIRE && !resists_fire(magr))
                || (damageType == AD_ELEC && !resists_elec(magr))
                || damageType == AD_PHYS) {
                dmg = mattk.dice;
                if (!dmg) {
                    dmg = monsterLevel(mdefpmid) + 1;
		}
                dmg *= mattk.diceSides;
            } else {
                dmg = 0;
	    }

            return dmg * multi2;
        }
    }
    return 0;
}

/* determine whether two monster types are from the same species */
boolean
same_race(pm1, pm2)
struct permonst *pm1, *pm2;
{
    int pmid1 = pmid4(pm1);
    int pmid2 = pmid4(pm2);

    int let1 = monsterClass(pmid1);
    int let2 = monsterClass(pmid2);


    if (pm1 == pm2)
        return TRUE; /* exact match */
    /* player races have their own predicates */
    if (isHuman(pmid1))
        return isHuman(pmid2);
    if (isElf(pmid1))
        return isElf(pmid2);
    if (isDwarf(pmid1))
        return isDwarf(pmid2);
    if (isGnome(pmid1))
        return isGnome(pmid2);
    if (isOrc(pmid1))
        return isOrc(pmid2);
    /* other creatures are less precise */
    if (isGiant(pmid1))
        return isGiant(pmid2); /* open to quibbling here */
    if (isGolem(pmid1))
        return isGolem(pmid2); /* even moreso... */
    if (isMindFlayer(pmid1))
        return isMindFlayer(pmid2);
    if (let1 == S_KOBOLD || pm1 == &mons[PM_KOBOLD_ZOMBIE]
        || pm1 == &mons[PM_KOBOLD_MUMMY])
        return (let2 == S_KOBOLD || pm2 == &mons[PM_KOBOLD_ZOMBIE]
                || pm2 == &mons[PM_KOBOLD_MUMMY]);
    if (let1 == S_OGRE)
        return (let2 == S_OGRE);
    if (let1 == S_NYMPH)
        return (let2 == S_NYMPH);
    if (let1 == S_CENTAUR)
        return (let2 == S_CENTAUR);
    if (isUnicorn(pmid1))
        return isUnicorn(pmid2);
    if (let1 == S_DRAGON)
        return (let2 == S_DRAGON);
    if (let1 == S_NAGA)
        return (let2 == S_NAGA);
    /* other critters get steadily messier */
    if (isRiderOfTheApocalypse(pmid1))
        return isRiderOfTheApocalypse(pmid2); /* debatable */
    if (isMinion(pmid1))
        return isMinion(pmid2); /* [needs work?] */
    /* tengu don't match imps (first test handled case of both being tengu) */
    if (pm1 == &mons[PM_TENGU] || pm2 == &mons[PM_TENGU])
        return FALSE;
    if (let1 == S_IMP)
        return (let2 == S_IMP);
    /* and minor demons (imps) don't match major demons */
    else if (let2 == S_IMP)
        return FALSE;
    if (isDemon(pmid1))
        return isDemon(pmid2);
    if (isUndead(pmid1)) {
        if (let1 == S_ZOMBIE)
            return (let2 == S_ZOMBIE);
        if (let1 == S_MUMMY)
            return (let2 == S_MUMMY);
        if (let1 == S_VAMPIRE)
            return (let2 == S_VAMPIRE);
        if (let1 == S_LICH)
            return (let2 == S_LICH);
        if (let1 == S_WRAITH)
            return (let2 == S_WRAITH);
        if (let1 == S_GHOST)
            return (let2 == S_GHOST);
    } else if (isUndead(pmid2))
        return FALSE;

    /* check for monsters which grow into more mature forms */
    if (let1 == let2) {
        int m1 = monsndx(pm1), m2 = monsndx(pm2), prv, nxt;

        /* we know m1 != m2 (very first check above); test all smaller
           forms of m1 against m2, then all larger ones; don't need to
           make the corresponding tests for variants of m2 against m1 */
        for (prv = m1, nxt = big_to_little(m1); nxt != prv;
             prv = nxt, nxt = big_to_little(nxt))
            if (nxt == m2)
                return TRUE;
        for (prv = m1, nxt = little_to_big(m1); nxt != prv;
             prv = nxt, nxt = little_to_big(nxt))
            if (nxt == m2)
                return TRUE;
    }
    /* not caught by little/big handling */
    if (pm1 == &mons[PM_GARGOYLE] || pm1 == &mons[PM_WINGED_GARGOYLE])
        return (pm2 == &mons[PM_GARGOYLE]
                || pm2 == &mons[PM_WINGED_GARGOYLE]);
    if (pm1 == &mons[PM_KILLER_BEE] || pm1 == &mons[PM_QUEEN_BEE])
        return (pm2 == &mons[PM_KILLER_BEE] || pm2 == &mons[PM_QUEEN_BEE]);

    if (isLongWorm(pmid1))
        return isLongWorm(pmid2); /* handles tail */
    /* [currently there's no reason to bother matching up
        assorted bugs and blobs with their closest variants] */
    /* didn't match */
    return FALSE;
}

/* return an index into the mons array */
int
monsndx(ptr)
struct permonst *ptr;
{
    register int i;

    i = (int) (ptr - &mons[0]);
    if (i < LOW_PM || i >= NUMMONS) {
        panic("monsndx - could not index monster (%s)",
              fmt_ptr((genericptr_t) ptr));
        return NON_PM; /* will not get here */
    }
    return i;
}

/* for handling alternate spellings */
struct alt_spl {
    const char *name;
    short pm_val;
};

/* figure out what type of monster a user-supplied string is specifying */
int
name_to_mon(in_str)
const char *in_str;
{
    /* Be careful.  We must check the entire string in case it was
     * something such as "ettin zombie corpse".  The calling routine
     * doesn't know about the "corpse" until the monster name has
     * already been taken off the front, so we have to be able to
     * read the name with extraneous stuff such as "corpse" stuck on
     * the end.
     * This causes a problem for names which prefix other names such
     * as "ettin" on "ettin zombie".  In this case we want the _longest_
     * name which exists.
     * This also permits plurals created by adding suffixes such as 's'
     * or 'es'.  Other plurals must still be handled explicitly.
     */
    register int i;
    register int mntmp = NON_PM;
    register char *s, *str, *term;
    char buf[BUFSZ];
    int len, slen;

    str = strcpy(buf, in_str);

    if (!strncmp(str, "a ", 2))
        str += 2;
    else if (!strncmp(str, "an ", 3))
        str += 3;
    else if (!strncmp(str, "the ", 4))
        str += 4;

    slen = strlen(str);
    term = str + slen;

    if ((s = strstri(str, "vortices")) != 0)
        Strcpy(s + 4, "ex");
    /* be careful with "ies"; "priest", "zombies" */
    else if (slen > 3 && !strcmpi(term - 3, "ies")
             && (slen < 7 || strcmpi(term - 7, "zombies")))
        Strcpy(term - 3, "y");
    /* luckily no monster names end in fe or ve with ves plurals */
    else if (slen > 3 && !strcmpi(term - 3, "ves"))
        Strcpy(term - 3, "f");

    slen = strlen(str); /* length possibly needs recomputing */

    {
        static const struct alt_spl names[] = {
            /* Alternate spellings */
            { "grey dragon", PM_GRAY_DRAGON },
            { "baby grey dragon", PM_BABY_GRAY_DRAGON },
            { "grey unicorn", PM_GRAY_UNICORN },
            { "grey ooze", PM_GRAY_OOZE },
            { "gray-elf", PM_GREY_ELF },
            { "mindflayer", PM_MIND_FLAYER },
            { "master mindflayer", PM_MASTER_MIND_FLAYER },
            /* More alternates; priest and priestess are separate monster
               types but that isn't the case for {aligned,high} priests */
            { "aligned priestess", PM_ALIGNED_PRIEST },
            { "high priestess", PM_HIGH_PRIEST },
            /* Inappropriate singularization by -ves check above */
            { "master of thief", PM_MASTER_OF_THIEVES },
            /* Potential misspellings where we want to avoid falling back
               to the rank title prefix (input has been singularized) */
            { "master thief", PM_MASTER_OF_THIEVES },
            { "master of assassin", PM_MASTER_ASSASSIN },
            /* Outdated names */
            { "invisible stalker", PM_STALKER },
            { "high-elf", PM_ELVENKING }, /* PM_HIGH_ELF is obsolete */
            { "halfling", PM_HOBBIT },    /* potential guess for polyself */
            /* Hyphenated names */
            { "ki rin", PM_KI_RIN },
            { "uruk hai", PM_URUK_HAI },
            { "orc captain", PM_ORC_CAPTAIN },
            { "woodland elf", PM_WOODLAND_ELF },
            { "green elf", PM_GREEN_ELF },
            { "grey elf", PM_GREY_ELF },
            { "gray elf", PM_GREY_ELF },
            { "elf lord", PM_ELF_LORD },
            { "olog hai", PM_OLOG_HAI },
            { "arch lich", PM_ARCH_LICH },
            /* Some irregular plurals */
            { "incubi", PM_INCUBUS },
            { "succubi", PM_SUCCUBUS },
            { "violet fungi", PM_VIOLET_FUNGUS },
            { "homunculi", PM_HOMUNCULUS },
            { "baluchitheria", PM_BALUCHITHERIUM },
            { "lurkers above", PM_LURKER_ABOVE },
            { "cavemen", PM_CAVEMAN },
            { "cavewomen", PM_CAVEWOMAN },
            { "djinn", PM_DJINNI },
            { "mumakil", PM_MUMAK },
            { "erinyes", PM_ERINYS },
            /* end of list */
            { 0, NON_PM }
        };
        register const struct alt_spl *namep;

        for (namep = names; namep->name; namep++)
            if (!strncmpi(str, namep->name, (int) strlen(namep->name)))
                return namep->pm_val;
    }

    for (len = 0, i = LOW_PM; i < NUMMONS; i++) {
	javaString monsterName = monsterTypeName(i);
        register int m_i_len = strlen(monsterName.c_str);

        if (m_i_len > len && !strncmpi(monsterName.c_str, str, m_i_len)) {
            if (m_i_len == slen) {
		releaseJavaString(monsterName);
                return i; /* exact match */
            } else if (slen > m_i_len
                       && (str[m_i_len] == ' '
                           || !strcmpi(&str[m_i_len], "s")
                           || !strncmpi(&str[m_i_len], "s ", 2)
                           || !strcmpi(&str[m_i_len], "'")
                           || !strncmpi(&str[m_i_len], "' ", 2)
                           || !strcmpi(&str[m_i_len], "'s")
                           || !strncmpi(&str[m_i_len], "'s ", 3)
                           || !strcmpi(&str[m_i_len], "es")
                           || !strncmpi(&str[m_i_len], "es ", 3))) {
                mntmp = i;
                len = m_i_len;
            }
        }
	releaseJavaString(monsterName);
    }
    if (mntmp == NON_PM) {
        mntmp = title_to_mon(str, (int *) 0, (int *) 0);
    }
    return mntmp;
}

/* monster class from user input; used for genocide and controlled polymorph;
   returns 0 rather than MAXMCLASSES if no match is found */
int
name_to_monclass(in_str, mndx_p)
const char *in_str;
int *mndx_p;
{
    /* Single letters are matched against def_monsyms[].sym; words
       or phrases are first matched against def_monsyms[].explain
       to check class description; if not found there, then against
       mons[].mname to test individual monster types.  Input can be a
       substring of the full description or mname, but to be accepted,
       such partial matches must start at beginning of a word.  Some
       class descriptions include "foo or bar" and "foo or other foo"
       so we don't want to accept "or", "other", "or other" there. */
    static NEARDATA const char *const falsematch[] = {
        /* multiple-letter input which matches any of these gets rejected */
        "an", "the", "or", "other", "or other", 0
    };
    /* positive pm_val => specific monster; negative => class */
    static NEARDATA const struct alt_spl truematch[] = {
        /* "long worm" won't match "worm" class but would accidentally match
           "long worm tail" class before the comparison with monster types */
        { "long worm", PM_LONG_WORM },
        /* matches wrong--or at least suboptimal--class */
        { "demon", -S_DEMON }, /* hits "imp or minor demon" */
        /* matches specific monster (overly restrictive) */
        { "devil", -S_DEMON }, /* always "horned devil" */
        /* some plausible guesses which need help */
        { "bug", -S_XAN },  /* would match bugbear... */
        { "fish", -S_EEL }, /* wouldn't match anything */
        /* end of list */
        { 0, NON_PM }
    };
    const char *p, *x;
    int i;

    if (mndx_p)
        *mndx_p = NON_PM; /* haven't [yet] matched a specific type */

    if (!in_str || !in_str[0]) {
        /* empty input */
        return 0;
    } else if (!in_str[1]) {
        /* single character */
        i = def_char_to_monclass(*in_str);
        if (i == S_MIMIC_DEF) { /* ']' -> 'm' */
            i = S_MIMIC;
        } else if (i == S_WORM_TAIL) { /* '~' -> 'w' */
            i = S_WORM;
            if (mndx_p)
                *mndx_p = PM_LONG_WORM;
        } else if (i == MAXMCLASSES) /* maybe 'I' */
            i = (*in_str == DEF_INVISIBLE) ? S_invisible : 0;
        return i;
    } else {
        /* multiple characters */
        in_str = makesingular(in_str);
        /* check for special cases */
        for (i = 0; falsematch[i]; i++)
            if (!strcmpi(in_str, falsematch[i]))
                return 0;
        for (i = 0; truematch[i].name; i++)
            if (!strcmpi(in_str, truematch[i].name)) {
                i = truematch[i].pm_val;
                if (i < 0)
                    return -i; /* class */
                if (mndx_p)
                    *mndx_p = i; /* monster */
                return monsterClass(i);
            }
        /* check monster class descriptions */
        for (i = 1; i < MAXMCLASSES; i++) {
            x = def_monsyms[i].explain;
            if ((p = strstri(x, in_str)) != 0 && (p == x || *(p - 1) == ' '))
                return i;
        }
        /* check individual species names; not as thorough as mon_to_name()
           but our caller can call that directly if desired */
        for (i = LOW_PM; i < NUMMONS; i++) {
            javaString x = monsterTypeName(i);
            if ((p = strstri(x.c_str, in_str)) != 0
                && (p == x.c_str || *(p - 1) == ' ')) {
                if (mndx_p) {
                    *mndx_p = i;
		}
                releaseJavaString(x);
                return monsterClass(i);
            } else {
                releaseJavaString(x);
	    }
        }
    }
    return 0;
}

/* returns 3 values (0=male, 1=female, 2=none) */
int
gender(mtmp)
register struct monst *mtmp;
{
    if (isNeuter(pmid4mon(mtmp)))
        return 2;
    return mtmp->female;
}

/* Like gender(), but lower animals and such are still "it".
   This is the one we want to use when printing messages. */
int
pronoun_gender(mtmp)
register struct monst *mtmp;
{
    int pmid = pmid4mon(mtmp);
    if (isNeuter(pmid) || !canspotmon(mtmp))
        return 2;
    return (isHumanoid(pmid) || (monsterGenerationMask(pmid) & G_UNIQ)
            || typeIsProperName(pmid)) ? (int) mtmp->female : 2;
}

/* used for nearby monsters when you go to another level */
boolean
levl_follower(mtmp)
struct monst *mtmp;
{
    if (mtmp == u.usteed)
        return TRUE;

    /* Wizard with Amulet won't bother trying to follow across levels */
    if (mtmp->iswiz && mon_has_amulet(mtmp))
        return FALSE;
    /* some monsters will follow even while intending to flee from you */
    if (mtmp->mtame || mtmp->iswiz || is_fshk(mtmp))
        return TRUE;
    /* stalking types follow, but won't when fleeing unless you hold
       the Amulet */
    return (boolean) (monsterHasFlag2(pmid4mon(mtmp), M2_STALK)
                      && (!mtmp->mflee || haveSpecialItem(SPECIAL_ITEM_AMULET)));
}

static const short grownups[][2] = {
    { PM_CHICKATRICE, PM_COCKATRICE },
    { PM_LITTLE_DOG, PM_DOG },
    { PM_DOG, PM_LARGE_DOG },
    { PM_HELL_HOUND_PUP, PM_HELL_HOUND },
    { PM_WINTER_WOLF_CUB, PM_WINTER_WOLF },
    { PM_KITTEN, PM_HOUSECAT },
    { PM_HOUSECAT, PM_LARGE_CAT },
    { PM_PONY, PM_HORSE },
    { PM_HORSE, PM_WARHORSE },
    { PM_KOBOLD, PM_LARGE_KOBOLD },
    { PM_LARGE_KOBOLD, PM_KOBOLD_LORD },
    { PM_GNOME, PM_GNOME_LORD },
    { PM_GNOME_LORD, PM_GNOME_KING },
    { PM_DWARF, PM_DWARF_LORD },
    { PM_DWARF_LORD, PM_DWARF_KING },
    { PM_MIND_FLAYER, PM_MASTER_MIND_FLAYER },
    { PM_ORC, PM_ORC_CAPTAIN },
    { PM_HILL_ORC, PM_ORC_CAPTAIN },
    { PM_MORDOR_ORC, PM_ORC_CAPTAIN },
    { PM_URUK_HAI, PM_ORC_CAPTAIN },
    { PM_SEWER_RAT, PM_GIANT_RAT },
    { PM_CAVE_SPIDER, PM_GIANT_SPIDER },
    { PM_OGRE, PM_OGRE_LORD },
    { PM_OGRE_LORD, PM_OGRE_KING },
    { PM_ELF, PM_ELF_LORD },
    { PM_WOODLAND_ELF, PM_ELF_LORD },
    { PM_GREEN_ELF, PM_ELF_LORD },
    { PM_GREY_ELF, PM_ELF_LORD },
    { PM_ELF_LORD, PM_ELVENKING },
    { PM_LICH, PM_DEMILICH },
    { PM_DEMILICH, PM_MASTER_LICH },
    { PM_MASTER_LICH, PM_ARCH_LICH },
    { PM_VAMPIRE, PM_VAMPIRE_LORD },
    { PM_BAT, PM_GIANT_BAT },
    { PM_BABY_GRAY_DRAGON, PM_GRAY_DRAGON },
    { PM_BABY_SILVER_DRAGON, PM_SILVER_DRAGON },
#if 0 /* DEFERRED */
    {PM_BABY_SHIMMERING_DRAGON, PM_SHIMMERING_DRAGON},
#endif
    { PM_BABY_RED_DRAGON, PM_RED_DRAGON },
    { PM_BABY_WHITE_DRAGON, PM_WHITE_DRAGON },
    { PM_BABY_ORANGE_DRAGON, PM_ORANGE_DRAGON },
    { PM_BABY_BLACK_DRAGON, PM_BLACK_DRAGON },
    { PM_BABY_BLUE_DRAGON, PM_BLUE_DRAGON },
    { PM_BABY_GREEN_DRAGON, PM_GREEN_DRAGON },
    { PM_BABY_YELLOW_DRAGON, PM_YELLOW_DRAGON },
    { PM_RED_NAGA_HATCHLING, PM_RED_NAGA },
    { PM_BLACK_NAGA_HATCHLING, PM_BLACK_NAGA },
    { PM_GOLDEN_NAGA_HATCHLING, PM_GOLDEN_NAGA },
    { PM_GUARDIAN_NAGA_HATCHLING, PM_GUARDIAN_NAGA },
    { PM_SMALL_MIMIC, PM_LARGE_MIMIC },
    { PM_LARGE_MIMIC, PM_GIANT_MIMIC },
    { PM_BABY_LONG_WORM, PM_LONG_WORM },
    { PM_BABY_PURPLE_WORM, PM_PURPLE_WORM },
    { PM_BABY_CROCODILE, PM_CROCODILE },
    { PM_SOLDIER, PM_SERGEANT },
    { PM_SERGEANT, PM_LIEUTENANT },
    { PM_LIEUTENANT, PM_CAPTAIN },
    { PM_WATCHMAN, PM_WATCH_CAPTAIN },
    { PM_ALIGNED_PRIEST, PM_HIGH_PRIEST },
    { PM_STUDENT, PM_ARCHEOLOGIST },
    { PM_ATTENDANT, PM_HEALER },
    { PM_PAGE, PM_KNIGHT },
    { PM_ACOLYTE, PM_PRIEST },
    { PM_APPRENTICE, PM_WIZARD },
    { PM_MANES, PM_LEMURE },
    { PM_KEYSTONE_KOP, PM_KOP_SERGEANT },
    { PM_KOP_SERGEANT, PM_KOP_LIEUTENANT },
    { PM_KOP_LIEUTENANT, PM_KOP_KAPTAIN },
    { NON_PM, NON_PM }
};

int
little_to_big(montype)
int montype;
{
    register int i;

    for (i = 0; grownups[i][0] >= LOW_PM; i++)
        if (montype == grownups[i][0]) {
            montype = grownups[i][1];
            break;
        }
    return montype;
}

int
big_to_little(montype)
int montype;
{
    register int i;

    for (i = 0; grownups[i][0] >= LOW_PM; i++)
        if (montype == grownups[i][1]) {
            montype = grownups[i][0];
            break;
        }
    return montype;
}

/*
 * Return the permonst ptr for the race of the monster.
 * Returns correct pointer for non-polymorphed and polymorphed
 * player.  It does not return a pointer to player role character.
 */
const struct permonst *
raceptr(mtmp)
struct monst *mtmp;
{
    if (mtmp == &youmonst && !areYouPolymorphed())
        return &mons[urace.malenum];
    else
        return mtmp->data;
}

static const char *levitate[4] = { "float", "Float", "wobble", "Wobble" };
static const char *flys[4] = { "fly", "Fly", "flutter", "Flutter" };
static const char *flyl[4] = { "fly", "Fly", "stagger", "Stagger" };
static const char *slither[4] = { "slither", "Slither", "falter", "Falter" };
static const char *ooze[4] = { "ooze", "Ooze", "tremble", "Tremble" };
static const char *immobile[4] = { "wiggle", "Wiggle", "pulsate", "Pulsate" };
static const char *crawl[4] = { "crawl", "Crawl", "falter", "Falter" };

const char *
locomotion(ptr, def)
const struct permonst *ptr;
const char *def;
{
    int capitalize = (*def == highc(*def));

    int pmid = pmid4(ptr);

    int msize = monsterSize(pmid);

    return (isFloater(pmid) ? levitate[capitalize]
            : (isFlyer(pmid) && msize <= MZ_SMALL) ? flys[capitalize]
              : (isFlyer(pmid) && msize > MZ_SMALL) ? flyl[capitalize]
                : isSlithy(pmid) ? slither[capitalize]
                  : isAmorphous(pmid) ? ooze[capitalize]
                    : !monsterMovementSpeed(pmid) ? immobile[capitalize]
                      : hasNoLimbs(pmid) ? crawl[capitalize]
                        : def);
}

const char *
stagger(ptr, def)
const struct permonst *ptr;
const char *def;
{
    int capitalize = 2 + (*def == highc(*def));

    int pmid = pmid4(ptr);

    int msize = monsterSize(pmid);

    return (isFloater(pmid) ? levitate[capitalize]
            : (isFlyer(pmid) && msize <= MZ_SMALL) ? flys[capitalize]
              : (isFlyer(pmid) && msize > MZ_SMALL) ? flyl[capitalize]
                : isSlithy(pmid) ? slither[capitalize]
                  : isAmorphous(pmid) ? ooze[capitalize]
                    : !monsterMovementSpeed(pmid) ? immobile[capitalize]
                      : hasNoLimbs(pmid) ? crawl[capitalize]
                        : def);
}

/* return phrase describing the effect of fire attack on a type of monster */
const char *
on_fire(mptr, mattk)
struct permonst *mptr;
const struct Attack mattk;
{
    const char *what;

    switch (monsndx(mptr)) {
    case PM_FLAMING_SPHERE:
    case PM_FIRE_VORTEX:
    case PM_FIRE_ELEMENTAL:
    case PM_SALAMANDER:
        what = "already on fire";
        break;
    case PM_WATER_ELEMENTAL:
    case PM_FOG_CLOUD:
    case PM_STEAM_VORTEX:
        what = "boiling";
        break;
    case PM_ICE_VORTEX:
    case PM_GLASS_GOLEM:
        what = "melting";
        break;
    case PM_STONE_GOLEM:
    case PM_CLAY_GOLEM:
    case PM_GOLD_GOLEM:
    case PM_AIR_ELEMENTAL:
    case PM_EARTH_ELEMENTAL:
    case PM_DUST_VORTEX:
    case PM_ENERGY_VORTEX:
        what = "heating up";
        break;
    default:
        what = (mattk.type == AT_HUGS) ? "being roasted" : "on fire";
        break;
    }
    return what;
}

/*
 * Returns:
 *      True if monster is presumed to have a sense of smell.
 *      False if monster definitely does not have a sense of smell.
 *
 * Do not base this on presence of a head or nose, since many
 * creatures sense smells other ways (feelers, forked-tongues, etc.)
 * We're assuming all insects can smell at a distance too.
 */
boolean
olfaction(mdat)
struct permonst *mdat;
{
    int mc = monsterClass(pmid4(mdat));
    if (isGolem(pmid4(mdat))
        || mc == S_EYE /* spheres  */
        || mc == S_JELLY || mc == S_PUDDING
        || mc == S_BLOB  || mc == S_VORTEX
        || mc == S_ELEMENTAL
        || mc == S_FUNGUS /* mushrooms and fungi */
        || mc == S_LIGHT)
        return FALSE;
    return TRUE;
}

boolean validAttack(const struct Attack attack) 
{
    return ((attack.type       != NO_ATTACK.type) &&
	    (attack.damageType != NO_ATTACK.damageType) &&
	    (attack.dice       != NO_ATTACK.dice) &&
	    (attack.diceSides  != NO_ATTACK.diceSides));
}

extern boolean javaGetBooleanFromInt(const char* classname, const char* methodname, int i);
extern boolean javaGetBooleanFromIntAndLong(const char* classname, const char* methodname, int i, long j);
extern boolean javaGetIntFromInt(const char* classname, const char* methodname, int i);

int emitsLightWithRange(int pmid) {
    return javaGetIntFromInt(MONSTER_DATA_CLASS, "emitsLightWithRange", pmid);
}
boolean isFloater(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isFloater", pmid);
}
boolean isGolem(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isGolem", pmid);
}
boolean isNoncorporeal(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isNoncorporeal", pmid);
}
boolean isNonliving(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isNonliving", pmid);
}
boolean isReviver(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isReviver", pmid);
}
boolean isSlimeproof(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isSlimeproof", pmid);
}
boolean isUnicorn(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isUnicorn", pmid);
}
boolean isVampire(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isVampire", pmid);
}
boolean isVeganOption(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isVeganOption", pmid);
}
boolean isVegetarianOption(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isVegetarianOption", pmid);
}
boolean isWhirly(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isWhirly", pmid);
}
boolean corpseOrStatueIsUnique(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "corpseOrStatueIsUnique", pmid);
}

boolean isSilent(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isSilent", pmid);
}
boolean isVerySmallMonster(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isVerySmallMonster", pmid);
}
boolean isBigMonster(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isBigMonster", pmid);
}
boolean cannotWieldThings(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "cannotWieldThings", pmid);
}

boolean monsterTypeResistsFire(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "monsterTypeResistsFire", pmid);
}
boolean monsterTypeResistsStoning(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "monsterTypeResistsStoning", pmid);
}

boolean cannotUseTwoWeapons(struct permonst* ptr) {
    return (monsterAttack(pmid4(ptr), 1).type != AT_WEAP);
}

boolean isFlyer(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isFlyer", pmid);
}

boolean isClinger(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isClinger", pmid);
}

boolean isSwimmer(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isSwimmer", pmid);
}

boolean doesNotBreathe(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "doesNotBreathe", pmid);
}

boolean isAmphibious(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isAmphibious", pmid);
}

boolean passesThroughWalls(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "passesThroughWalls", pmid);
}

boolean isAmorphous(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isAmorphous", pmid);
}

boolean isTunneler(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isTunneler", pmid);
}

boolean needsPickaxe(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "needsPickaxe", pmid);
}

boolean hidesUnderStuff(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "hidesUnderStuff", pmid);
}

boolean isHider(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isHider", pmid);
}

boolean hasEyes(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "hasEyes", pmid);
}

int eyeCount(int pmid) {
    return javaGetIntFromInt(MONSTER_DATA_CLASS, "eyeCount", pmid);
}

boolean hasNoHands(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "hasNoHands", pmid);
}

boolean hasNoLimbs(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "hasNoLimbs", pmid);
}

boolean doesNotTakeStuff(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "doesNotTakeStuff", pmid);
}

boolean hasAHead(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "hasAHead", pmid);
}

boolean isUnsolid(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isUnsolid", pmid);
}

boolean isMindless(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isMindless", pmid);
}

boolean isHumanoid(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isHumanoid", pmid);
}

boolean isAnimal(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isAnimal", pmid);
}

boolean isSlithy(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isSlithy", pmid);
}

boolean isThickSkinned(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isThickSkinned", pmid);
}

boolean laysEggs(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "laysEggs", pmid);
}

boolean regenerates(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "regenerates", pmid);
}

boolean perceivesTheInvisible(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "perceivesTheInvisible", pmid);
}

boolean canTeleport(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "canTeleport", pmid);
}

boolean canControlTeleport(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "canControlTeleport", pmid);
}

boolean isAcidic(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isAcidic", pmid);
}

boolean isPoisonous(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isPoisonous", pmid);
}

boolean isCarnivorous(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isCarnivorous", pmid);
}

boolean isHerbivorous(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isHerbivorous", pmid);
}

boolean isMetallivorous(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isMetallivorous", pmid);
}

boolean okToPolymorphInto(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "okToPolymorphInto", pmid);
}

boolean isShapeshifter(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isShapeshifter", pmid);
}

boolean isUndead(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isUndead", pmid);
}

boolean isWere(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isWere", pmid);
}

boolean isElf(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isElf", pmid);
}

boolean isDwarf(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isDwarf", pmid);
}

boolean isGnome(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isGnome", pmid);
}

boolean isOrc(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isOrc", pmid);
}

boolean isHuman(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isHuman", pmid);
}

boolean isOfYourRace(int pmid, long race) {
    return javaGetBooleanFromIntAndLong(MONSTER_DATA_CLASS, "isOfYourRace", pmid, race);
}

boolean isBat(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isBat", pmid);
}

boolean isGiant(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isGiant", pmid);
}

boolean isDomestic(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isDomestic", pmid);
}

boolean isDemon(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isDemon", pmid);
}

boolean isMercenary(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isMercenary", pmid);
}

boolean isMale(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isMale", pmid);
}

boolean isFemale(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isFemale", pmid);
}

boolean isNeuter(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isNeuter", pmid);
}

boolean isWanderer(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isWanderer", pmid);
}

boolean isAlwaysHostile(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isAlwaysHostile", pmid);
}

boolean isAlwaysPeaceful(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isAlwaysPeaceful", pmid);
}

boolean racialHostility(int pmid, long mask) {
    return javaGetBooleanFromIntAndLong(MONSTER_DATA_CLASS, "racialHostility", pmid, mask);
}

boolean racialFriendship(int pmid, long mask) {
    return javaGetBooleanFromIntAndLong(MONSTER_DATA_CLASS, "racialFriendship", pmid, mask);
}

boolean isExtraNasty(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isExtraNasty", pmid);
}

boolean isStrongMonster(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isStrongMonster", pmid);
}

boolean throwsRocks(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "throwsRocks", pmid);
}

boolean typeIsProperName(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "typeIsProperName", pmid);
}

boolean isLord(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isLord", pmid);
} 

boolean isPrince(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isPrince", pmid);
}

boolean isNamelessMajorDemon(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isNamelessMajorDemon", pmid);
}

boolean isDemonLord(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isDemonLord", pmid);
} 

boolean isDemonPrince(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isDemonPrince", pmid);
}

boolean isMinion(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isMinion", pmid);
}

boolean isLawfulMinion(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isLawfulMinion", pmid);
}

boolean likesGold(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "likesGold", pmid);
}

boolean likesGems(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "likesGems", pmid);
}

boolean likesObjects(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "likesObjects", pmid);
}

boolean likesMagicItems(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "likesMagicItems", pmid);
}

boolean isCovetous(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isCovetous", pmid);
}

boolean hasInfravision(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "hasInfravision", pmid);
}

boolean isInfravisible(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isInfravisible", pmid);
}

boolean isDisplacer(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isDisplacer", pmid);
}

boolean allowsCloseApproach(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "allowsCloseApproach", pmid);
}

boolean touchPetrifies(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "touchPetrifies", pmid);
}

boolean fleshPetrifies(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "fleshPetrifies", pmid);
}

boolean isMindFlayer(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isMindFlayer", pmid);
}

boolean likesFire(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "likesFire", pmid);
}

boolean likesLava(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "likesLava", pmid);
}

boolean hasHorns(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "hasHorns", pmid);
}

int numberOfHorns(int pmid) {
    return javaGetIntFromInt(MONSTER_DATA_CLASS, "numberOfHorns", pmid);
}

extern boolean befriendWithObject(struct permonst* pm, struct obj* otmp) {
    return (otmp && (otmp->oclass == FOOD_CLASS) && isDomestic(pmid4(pm)));
}

boolean isFlaming(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isFlaming", pmid);
}

boolean isWooden(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isWooden", pmid);
}

boolean isTelepathic(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isTelepathic", pmid);
}

boolean isArmed(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isArmed", pmid);
}

boolean hasBreathWeapon(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "hasBreathWeapon", pmid);
}

boolean cannotWearArmor(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "cannotWearArmor", pmid);
}

boolean makesWebs(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "makesWebs", pmid);
}

boolean isLongWorm(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isLongWorm", pmid);
}

boolean isMonsterPlayer(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isMonsterPlayer", pmid);
}

boolean isWatchman(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isWatchman", pmid);
}

boolean isRiderOfTheApocalypse(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isRiderOfTheApocalypse", pmid);
}

boolean isPlaceholderMonster(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isPlaceholderMonster", pmid);
}

boolean isInvisibleMonsterType(int pmid) {
    return javaGetBooleanFromInt(MONSTER_DATA_CLASS, "isInvisibleMonsterType", pmid);
}

/*mondata.c*/
