/* NetHack 3.6	artifact.c	$NHDT-Date: 1446369462 2015/11/01 09:17:42 $  $NHDT-Branch: master $:$NHDT-Revision: 1.96 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"
#include "artilist.h"

/*
 * Note:  both artilist[] and artiexist[] have a dummy element #0,
 *        so loops over them should normally start at #1.  The primary
 *        exception is the save & restore code, which doesn't care about
 *        the contents, just the total size.
 */

extern boolean notonhead; /* for long worms */

#define get_artifact(o) \
    (((o) && (o)->oartifact) ? &artilist[(int) (o)->oartifact] : 0)

STATIC_DCL boolean
FDECL(bane_applies, (const struct artifact *, struct monst *));
STATIC_DCL int FDECL(spec_applies, (const struct artifact *, struct monst *));
STATIC_DCL int FDECL(arti_invoke, (struct obj *));
STATIC_DCL boolean
FDECL(Mb_hit, (struct monst * magr, struct monst *mdef, struct obj *, int *,
               int, BOOLEAN_P, char *));
STATIC_DCL unsigned long FDECL(extrinsic_to_spfx, (int));
STATIC_DCL uchar FDECL(extrinsic_to_damageType, (int));
STATIC_DCL boolean FDECL(untouchable, (struct obj *, BOOLEAN_P));

/* The amount added to the victim's total hit points to insure that the
   victim will be killed even after damage bonus/penalty adjustments.
   Most such penalties are small, and 200 is plenty; the exception is
   half physical damage.  3.3.1 and previous versions tried to use a very
   large number to account for this case; now, we just compute the fatal
   damage by adding it to 2 times the total hit points instead of 1 time.
   Note: this will still break if they have more than about half the number
   of hit points that will fit in a 15 bit integer. */
#define FATAL_DAMAGE_MODIFIER 200

/* coordinate effects from spec_dbon() with messages in artifact_hit() */
STATIC_OVL int spec_dbon_applies = 0;

/* flags including which artifacts have already been created */
static boolean artiexist[1 + NROFARTIFACTS + 1];
/* and a discovery list for them (no dummy first entry here) */
STATIC_OVL xchar artidisco[NROFARTIFACTS];

STATIC_DCL void NDECL(hack_artifacts);
STATIC_DCL boolean FDECL(attacks, (int, struct obj *));

/* handle some special cases; must be called after u_init() */
STATIC_OVL void
hack_artifacts()
{
    struct artifact *art;
    int alignmnt = alignmentType(flags.initalign);

    /* Fix up the alignments of "gift" artifacts */
    for (art = artilist + 1; art->otyp; art++)
        if ((art->role == yourRolePMID()) && (art->alignment != A_NONE))
        {
            art->alignment = alignmnt;
        }

    /* Excalibur can be used by any lawful character, not just knights */
    if (!yourRoleHasPMID(PM_KNIGHT))
        artilist[ART_EXCALIBUR].role = NON_PM;

    /* Fix up the quest artifact */
    if (yourRoleHasQuestArtifact()) {
        artilist[idOfQuestArtifactForYourRole()].alignment = alignmnt;
        artilist[idOfQuestArtifactForYourRole()].role = yourRolePMID();
    }
    return;
}

/* zero out the artifact existence list */
void
init_artifacts()
{
    (void) memset((genericptr_t) artiexist, 0, sizeof artiexist);
    (void) memset((genericptr_t) artidisco, 0, sizeof artidisco);
    hack_artifacts();
}

void
save_artifacts(fd)
int fd;
{
    bwrite(fd, (genericptr_t) artiexist, sizeof artiexist);
    bwrite(fd, (genericptr_t) artidisco, sizeof artidisco);
}

void
restore_artifacts(fd)
int fd;
{
    mread(fd, (genericptr_t) artiexist, sizeof artiexist);
    mread(fd, (genericptr_t) artidisco, sizeof artidisco);
    hack_artifacts(); /* redo non-saved special cases */
}

const char *
artiname(artinum)
int artinum;
{
    if (artinum <= 0 || artinum > NROFARTIFACTS)
        return "";
    return artilist[artinum].name;
}

/*
   Make an artifact.  If a specific alignment is specified, then an object of
   the appropriate alignment is created from scratch, or 0 is returned if
   none is available.  (If at least one aligned artifact has already been
   given, then unaligned ones also become eligible for this.)
   If no alignment is given, then 'otmp' is converted
   into an artifact of matching type, or returned as-is if that's not
   possible.
   For the 2nd case, caller should use ``obj = mk_artifact(obj, A_NONE);''
   for the 1st, ``obj = mk_artifact((struct obj *)0, some_alignment);''.
 */
struct obj *
mk_artifact(otmp, alignment)
struct obj *otmp;   /* existing object; ignored if alignment specified */
aligntyp alignment; /* target alignment, or A_NONE */
{
    const struct artifact *a;
    int m, n, altn;
    boolean by_align = (alignment != A_NONE);
    short o_typ = (by_align || !otmp) ? 0 : otmp->otyp;
    boolean unique = !by_align && otmp && objects[o_typ].oc_unique;
    short eligible[NROFARTIFACTS];

    n = altn = 0;    /* no candidates found yet */
    eligible[0] = 0; /* lint suppression */
    /* gather eligible artifacts */
    for (m = 1, a = &artilist[m]; a->otyp; a++, m++) {
        if (artiexist[m])
            continue;
        if ((a->spfx & SPFX_NOGEN) || unique)
            continue;

        if (!by_align) {
            /* looking for a particular type of item; not producing a
               divine gift so we don't care about role's first choice */
            if (a->otyp == o_typ)
                eligible[n++] = m;
            continue; /* move on to next possibility */
        }

        /* we're looking for an alignment-specific item
           suitable for hero's role+race */
        if ((a->alignment == alignment || a->alignment == A_NONE)
            /* avoid enemies' equipment */
            && (a->race == NON_PM || !racialHostility(a->race, yourSpeciesHostilityMask()))) {
            /* when a role-specific first choice is available, use it */
            if (yourRoleHasPMID(a->role)) {
                /* make this be the only possibility in the list */
                eligible[0] = m;
                n = 1;
                break; /* skip all other candidates */
            }
            /* found something to consider for random selection */
            if (a->alignment != A_NONE || giftsBestowed() > 0) {
                /* right alignment, or non-aligned with at least 1
                   previous gift bestowed, makes this one viable */
                eligible[n++] = m;
            } else {
                /* non-aligned with no previous gifts;
                   if no candidates have been found yet, record
                   this one as a[nother] fallback possibility in
                   case all aligned candidates have been used up
                   (via wishing, naming, bones, random generation) */
                if (!n)
                    eligible[altn++] = m;
                /* [once a regular candidate is found, the list
                   is overwritten and `altn' becomes irrelevant] */
            }
        }
    }

    /* resort to fallback list if main list was empty */
    if (!n)
        n = altn;

    if (n) {
        /* found at least one candidate; pick one at random */
        m = eligible[rn2(n)]; /* [0..n-1] */
        a = &artilist[m];

        /* make an appropriate object if necessary, then christen it */
        if (by_align)
            otmp = mksobj((int) a->otyp, TRUE, FALSE);

        if (otmp) {
            otmp = oname(otmp, a->name);
            otmp->oartifact = m;
            artiexist[m] = TRUE;
        }
    } else {
        /* nothing appropriate could be found; return original object */
        if (by_align)
            otmp = 0; /* (there was no original object) */
    }
    return otmp;
}

/*
 * Returns the full name (with articles and correct capitalization) of an
 * artifact named "name" if one exists, or NULL, it not.
 * The given name must be rather close to the real name for it to match.
 * The object type of the artifact is returned in otyp if the return value
 * is non-NULL.
 */
const char *
artifact_name(name, otyp)
const char *name;
short *otyp;
{
    register const struct artifact *a;
    register const char *aname;

    if (!strncmpi(name, "the ", 4))
        name += 4;

    for (a = artilist + 1; a->otyp; a++) {
        aname = a->name;
        if (!strncmpi(aname, "the ", 4))
            aname += 4;
        if (!strcmpi(name, aname)) {
            *otyp = a->otyp;
            return a->name;
        }
    }

    return (char *) 0;
}

boolean
exist_artifact(otyp, name)
int otyp;
const char *name;
{
    register const struct artifact *a;
    boolean *arex;

    if (otyp && *name)
        for (a = artilist + 1, arex = artiexist + 1; a->otyp; a++, arex++)
            if ((int) a->otyp == otyp && !strcmp(a->name, name))
                return *arex;
    return FALSE;
}

void
artifact_exists(otmp, name, mod)
struct obj *otmp;
const char *name;
boolean mod;
{
    register const struct artifact *a;

    if (otmp && *name)
        for (a = artilist + 1; a->otyp; a++)
            if (a->otyp == otmp->otyp && !strcmp(a->name, name)) {
                register int m = (int) (a - artilist);
                otmp->oartifact = (char) (mod ? m : 0);
                otmp->age = 0;
                if (otmp->otyp == RIN_INCREASE_DAMAGE)
                    otmp->spe = 0;
                artiexist[m] = mod;
                break;
            }
    return;
}

int
nartifact_exist()
{
    int a = 0;
    int n = SIZE(artiexist);

    while (n > 1)
        if (artiexist[--n])
            a++;

    return a;
}

boolean
spec_ability(otmp, abil)
struct obj *otmp;
unsigned long abil;
{
    const struct artifact *arti = get_artifact(otmp);

    return (boolean) (arti && (arti->spfx & abil) != 0L);
}

/* used so that callers don't need to known about SPFX_ codes */
boolean
confers_luck(obj)
struct obj *obj;
{
    /* might as well check for this too */
    if (obj->otyp == LUCKSTONE)
        return TRUE;

    return (boolean) (obj->oartifact && spec_ability(obj, SPFX_LUCK));
}

/* used to check whether a monster is getting reflection from an artifact */
boolean
arti_reflects(obj)
struct obj *obj;
{
    const struct artifact *arti = get_artifact(obj);

    if (arti) {
        /* while being worn */
        if ((obj->owornmask & ~W_ART) && (arti->spfx & SPFX_REFLECT))
            return TRUE;
        /* just being carried */
        if (arti->cspfx & SPFX_REFLECT)
            return TRUE;
    }
    return FALSE;
}

/* decide whether this obj is effective when attacking against shades;
   does not consider the bonus for blessed objects versus undead */
boolean
shade_glare(obj)
struct obj *obj;
{
    const struct artifact *arti;

    /* any silver object is effective */
    if (objects[obj->otyp].oc_material == SILVER)
        return TRUE;
    /* non-silver artifacts with bonus against undead also are effective */
    arti = get_artifact(obj);
    if (arti && (arti->spfx & SPFX_DFLAG2) && arti->mtype == M2_UNDEAD)
        return TRUE;
    /* [if there was anything with special bonus against noncorporeals,
       it would be effective too] */
    /* otherwise, harmless to shades */
    return FALSE;
}

/* returns 1 if name is restricted for otmp->otyp */
boolean
restrict_name(otmp, name)
struct obj *otmp;
const char *name;
{
    register const struct artifact *a;
    const char *aname, *odesc, *other;
    boolean sametype[NUM_OBJECTS];
    int i, lo, hi, otyp = otmp->otyp, ocls = objects[otyp].oc_class;

    if (!*name)
        return FALSE;
    if (!strncmpi(name, "the ", 4))
        name += 4;

    /* decide what types of objects are the same as otyp;
       if it's been discovered, then only itself matches;
       otherwise, include all other undiscovered objects
       of the same class which have the same description
       or share the same pool of shuffled descriptions */
    (void) memset((genericptr_t) sametype, 0, sizeof sametype); /* FALSE */
    sametype[otyp] = TRUE;
    if (!objects[otyp].oc_name_known
        && (odesc = OBJ_DESCR(objects[otyp])) != 0) {
        obj_shuffle_range(otyp, &lo, &hi);
        for (i = bases[ocls]; i < NUM_OBJECTS; i++) {
            if (objects[i].oc_class != ocls)
                break;
            if (!objects[i].oc_name_known
                && (other = OBJ_DESCR(objects[i])) != 0
                && (!strcmp(odesc, other) || (i >= lo && i <= hi)))
                sametype[i] = TRUE;
        }
    }

    /* Since almost every artifact is SPFX_RESTR, it doesn't cost
       us much to do the string comparison before the spfx check.
       Bug fix:  don't name multiple elven daggers "Sting".
     */
    for (a = artilist + 1; a->otyp; a++) {
        if (!sametype[a->otyp])
            continue;
        aname = a->name;
        if (!strncmpi(aname, "the ", 4))
            aname += 4;
        if (!strcmp(aname, name))
            return (boolean) ((a->spfx & (SPFX_NOGEN | SPFX_RESTR)) != 0
                              || otmp->quan > 1L);
    }

    return FALSE;
}

STATIC_OVL boolean
attacks(damageType, otmp)
int damageType;
struct obj *otmp;
{
    register const struct artifact *weap;

    if ((weap = get_artifact(otmp)) != 0)
        return (boolean) (weap->attk.damageType == damageType);
    return FALSE;
}

boolean
defends(damageType, otmp)
int damageType;
struct obj *otmp;
{
    register const struct artifact *weap;

    if ((weap = get_artifact(otmp)) != 0)
        return (boolean) (weap->defn.damageType == damageType);
    return FALSE;
}

/* used for monsters */
boolean
defends_when_carried(damageType, otmp)
int damageType;
struct obj *otmp;
{
    register const struct artifact *weap;

    if ((weap = get_artifact(otmp)) != 0)
        return (boolean) (weap->cary.damageType == damageType);
    return FALSE;
}

/* determine whether an item confers Protection */
boolean
protects(otmp, being_worn)
struct obj *otmp;
boolean being_worn;
{
    const struct artifact *arti;

    if (being_worn && objects[otmp->otyp].oc_oprop == PROTECTION)
        return TRUE;
    arti = get_artifact(otmp);
    if (!arti)
        return FALSE;
    return (boolean) ((arti->cspfx & SPFX_PROTECT) != 0
                      || (being_worn && (arti->spfx & SPFX_PROTECT) != 0));
}

/*
 * a potential artifact has just been worn/wielded/picked-up or
 * unworn/unwielded/dropped.  Pickup/drop only set/reset the W_ART mask.
 */
void
set_artifact_intrinsic(otmp, on, wp_mask)
register struct obj *otmp;
boolean on;
long wp_mask;
{
    int extrinsic = 0;
    register const struct artifact *oart = get_artifact(otmp);
    register uchar dtyp;
    register long spfx;

    if (!oart)
        return;

    /* effects from the defn field */
    dtyp = (wp_mask != W_ART) ? oart->defn.damageType : oart->cary.damageType;

    if (dtyp == AD_FIRE)
        extrinsic = FIRE_RES;
    else if (dtyp == AD_COLD)
        extrinsic = COLD_RES;
    else if (dtyp == AD_ELEC)
        extrinsic = SHOCK_RES;
    else if (dtyp == AD_MAGM)
        extrinsic = ANTIMAGIC;
    else if (dtyp == AD_DISN)
        extrinsic = DISINT_RES;
    else if (dtyp == AD_DRST)
        extrinsic = POISON_RES;

    if (extrinsic && wp_mask == W_ART && !on) {
        /* find out if some other artifact also confers this intrinsic */
        /* if so, leave the mask alone */
        register struct obj *obj;
        for (obj = invent; obj; obj = obj->nobj)
            if (obj != otmp && obj->oartifact) {
                register const struct artifact *art = get_artifact(obj);
                if (art->cary.damageType == dtyp) {
                    extrinsic = 0;
                    break;
                }
            }
    }
    if (extrinsic) {
        if (on) {
            setYourExtrinsicMask(extrinsic, wp_mask);
	} else {
            unsetYourExtrinsicMask(extrinsic, wp_mask);
	}
    }

    /* intrinsics from the spfx field; there could be more than one */
    spfx = (wp_mask != W_ART) ? oart->spfx : oart->cspfx;
    if (spfx && wp_mask == W_ART && !on) {
        /* don't change any spfx also conferred by other artifacts */
        register struct obj *obj;
        for (obj = invent; obj; obj = obj->nobj)
            if (obj != otmp && obj->oartifact) {
                register const struct artifact *art = get_artifact(obj);
                spfx &= ~art->cspfx;
            }
    }

    if (spfx & SPFX_SEARCH) {
        if (on) {
            setYourExtrinsicMask(SEARCHING, wp_mask);
	} else {
            unsetYourExtrinsicMask(SEARCHING, wp_mask);
	}
    }
    if (spfx & SPFX_HALRES) {
        /* make_hallucinated must (re)set the mask itself to get
         * the display right */
        /* restoring needed because this is the only artifact intrinsic
         * that can print a message--need to guard against being printed
         * when restoring a game
         */
        (void) make_hallucinated((long) !on, restoring ? FALSE : TRUE,
                                 wp_mask);
    }
    if (spfx & SPFX_ESP) {
        if (on)
            setYourExtrinsicMask(TELEPAT, wp_mask);
        else
            unsetYourExtrinsicMask(TELEPAT, wp_mask);
        see_monsters();
    }
    if (spfx & SPFX_STLTH) {
        if (on)
            setYourExtrinsicMask(STEALTH, wp_mask);
        else
            unsetYourExtrinsicMask(STEALTH, wp_mask);
    }
    if (spfx & SPFX_REGEN) {
        if (on)
            setYourExtrinsicMask(REGENERATION, wp_mask);
        else
            unsetYourExtrinsicMask(REGENERATION, wp_mask);
    }
    if (spfx & SPFX_TCTRL) {
        if (on)
            setYourExtrinsicMask(TELEPORT_CONTROL, wp_mask);
        else
            unsetYourExtrinsicMask(TELEPORT_CONTROL, wp_mask);
    }
    if (spfx & SPFX_WARN) {
        if (spec_m2(otmp)) {
            if (on) {
                setYourExtrinsicMask(WARN_OF_MON, wp_mask);
                context.warntype.obj |= spec_m2(otmp);
            } else {
                unsetYourExtrinsicMask(WARN_OF_MON, wp_mask);
                context.warntype.obj &= ~spec_m2(otmp);
            }
            see_monsters();
        } else {
            if (on)
                setYourExtrinsicMask(WARNING, wp_mask);
            else
                unsetYourExtrinsicMask(WARNING, wp_mask);
        }
    }
    if (spfx & SPFX_EREGEN) {
        if (on)
            setYourExtrinsicMask(ENERGY_REGENERATION, wp_mask);
        else
            unsetYourExtrinsicMask(ENERGY_REGENERATION, wp_mask);
    }
    if (spfx & SPFX_HSPDAM) {
        if (on)
            setYourExtrinsicMask(HALF_SPDAM, wp_mask);
        else
            unsetYourExtrinsicMask(HALF_SPDAM, wp_mask);
    }
    if (spfx & SPFX_HPHDAM) {
        if (on)
            setYourExtrinsicMask(HALF_PHDAM, wp_mask);
        else
            unsetYourExtrinsicMask(HALF_PHDAM, wp_mask);
    }
    if (spfx & SPFX_XRAY) {
        /* this assumes that no one else is using xray_range */
        if (on) {
            setXRayVisionRange(3);
        }
        else {
            setXRayVisionRange(-1);
        }
        vision_full_recalc = 1;
    }
    if ((spfx & SPFX_REFLECT) && (wp_mask & W_WEP)) {
        if (on)
            setYourExtrinsicMask(REFLECTING, wp_mask);
        else
            unsetYourExtrinsicMask(REFLECTING, wp_mask);
    }
    if (spfx & SPFX_PROTECT) {
        if (on)
            setYourExtrinsicMask(PROTECTION, wp_mask);
        else
            unsetYourExtrinsicMask(PROTECTION, wp_mask);
    }

    if (wp_mask == W_ART && !on && oart->inv_prop) {
        /* might have to turn off invoked power too */
        if (oart->inv_prop <= LAST_PROP
            && yourExtrinsicHasMask(oart->inv_prop, W_ARTI))
            (void) arti_invoke(otmp);
    }
}

/* touch_artifact()'s return value isn't sufficient to tell whether it
   dished out damage, and tracking changes to currentHitPoints, currentHitPointsAsMonster, Lifesaved
   when trying to avoid second wounding is too cumbersome */
STATIC_VAR boolean touch_blasted; /* for retouch_object() */

/*
 * creature (usually hero) tries to touch (pick up or wield) an artifact obj.
 * Returns 0 if the object refuses to be touched.
 * This routine does not change any object chains.
 * Ignores such things as gauntlets, assuming the artifact is not
 * fooled by such trappings.
 */
int
touch_artifact(obj, mon)
struct obj *obj;
struct monst *mon;
{
    register const struct artifact *oart = get_artifact(obj);
    boolean badclass, badalign, self_willed, yours;

    touch_blasted = FALSE;
    if (!oart)
        return 1;

    yours = (mon == &youmonst);
    /* all quest artifacts are self-willed; it this ever changes, `badclass'
       will have to be extended to explicitly include quest artifacts */
    self_willed = ((oart->spfx & SPFX_INTEL) != 0);
    if (yours) {
        badclass = self_willed
                   && ((oart->role != NON_PM && !yourRoleHasPMID(oart->role))
                       || (oart->race != NON_PM && !yourSpeciesIs(oart->race)));
        badalign =
            (oart->spfx & SPFX_RESTR) && oart->alignment != A_NONE
            && (oart->alignment != currentAlignmentType() || currentAlignmentRecord() < 0);
    } else if (!isCovetous(pmid4mon(mon)) && !isMonsterPlayer(pmid4mon(mon))) {
        badclass = self_willed && oart->role != NON_PM
                   && oart != &artilist[ART_EXCALIBUR];
        badalign = (oart->spfx & SPFX_RESTR) && oart->alignment != A_NONE
                   && (oart->alignment != mon_aligntyp(mon));
    } else { /* an M3_WANTSxxx monster or a fake player */
        /* special monsters trying to take the Amulet, invocation tools or
           quest item can touch anything except `spec_applies' artifacts */
        badclass = badalign = FALSE;
    }
    /* weapons which attack specific categories of monsters are
       bad for them even if their alignments happen to match */
    if (!badalign)
        badalign = bane_applies(oart, mon);

    if (((badclass || badalign) && self_willed)
        || (badalign && (!yours || !rn2(4)))) {
        int dmg, tmp;
        char buf[BUFSZ];

        if (!yours)
            return 0;
        You("are blasted by %s power!", s_suffix(the(xname(obj))));
        touch_blasted = TRUE;
        dmg = d((youResistMagic() ? 2 : 4), (self_willed ? 10 : 4));
        /* add half (maybe quarter) of the usual silver damage bonus */
        if (objects[obj->otyp].oc_material == SILVER && youHateSilver())
            tmp = rnd(10), dmg += Maybe_Half_Phys(tmp);
        Sprintf(buf, "touching %s", oart->name);
        losehp(dmg, buf, KILLED_BY); /* magic damage, not physical */
        exercise(A_WIS, FALSE);
    }

    /* can pick it up unless you're totally non-synch'd with the artifact */
    if (badclass && badalign && self_willed) {
        if (yours) {
            if (!carried(obj))
                pline("%s your grasp!", Tobjnam(obj, "evade"));
            else
                pline("%s beyond your control!", Tobjnam(obj, "are"));
        }
        return 0;
    }

    return 1;
}

/* decide whether an artifact itself is vulnerable to a particular type
   of erosion damage, independent of the properties of its bearer */
boolean
arti_immune(obj, dtyp)
struct obj *obj;
int dtyp;
{
    register const struct artifact *weap = get_artifact(obj);

    if (!weap)
        return FALSE;
    if (dtyp == AD_PHYS)
        return FALSE; /* nothing is immune to phys dmg */
    return (boolean) (weap->attk.damageType == dtyp
                      || weap->defn.damageType == dtyp
                      || weap->cary.damageType == dtyp);
}

STATIC_OVL boolean
bane_applies(oart, mon)
const struct artifact *oart;
struct monst *mon;
{
    struct artifact atmp;

    if (oart && (oart->spfx & SPFX_DBONUS) != 0) {
        atmp = *oart;
        atmp.spfx &= SPFX_DBONUS; /* clear other spfx fields */
        if (spec_applies(&atmp, mon))
            return TRUE;
    }
    return FALSE;
}

/* decide whether an artifact's special attacks apply against mtmp */
STATIC_OVL int
spec_applies(weap, mtmp)
register const struct artifact *weap;
struct monst *mtmp;
{
    boolean yours;

    if (!(weap->spfx & (SPFX_DBONUS | SPFX_ATTK)))
        return (weap->attk.damageType == AD_PHYS);

    yours = (mtmp == &youmonst);
    int pmid = pmid4mon(mtmp);

    if (weap->spfx & SPFX_DMONS) {
        return (pmid == (int) weap->mtype);
    } else if (weap->spfx & SPFX_DCLAS) {
        return (weap->mtype == (unsigned long) monsterClass(pmid));
    } else if (weap->spfx & SPFX_DFLAG1) {
        return isAffectedByWeaponFlag1(pmid, weap->mtype);
    } else if (weap->spfx & SPFX_DFLAG2) {
        return (isAffectedByWeaponFlag2(pmid, weap->mtype)
                || (yours
                    && ((!areYouPolymorphed() && (yourSpeciesSelfMask() & weap->mtype))
                        || ((weap->mtype & M2_WERE) && lycanthropeType() >= LOW_PM))));
    } else if (weap->spfx & SPFX_DALIGN) {
	int malign = monsterAlignment(pmid);
        return yours ? (currentAlignmentType() != weap->alignment)
                     : (malign == A_NONE
                        || sgn(malign) != weap->alignment);
    } else if (weap->spfx & SPFX_ATTK) {
        struct obj *defending_weapon = (yours ? uwep : MON_WEP(mtmp));

        if (defending_weapon && defending_weapon->oartifact
            && defends((int) weap->attk.damageType, defending_weapon))
            return FALSE;
        switch (weap->attk.damageType) {
        case AD_FIRE:
            return !(yours ? youResistFire() : resists_fire(mtmp));
        case AD_COLD:
            return !(yours ? youResistCold() : resists_cold(mtmp));
        case AD_ELEC:
            return !(yours ? youResistShock() : resists_elec(mtmp));
        case AD_MAGM:
        case AD_STUN:
            return !(yours ? youResistMagic() : (rn2(100) < monsterBaseMagicResistance(pmid)));
        case AD_DRST:
            return !(yours ? youResistPoison() : resists_poison(mtmp));
        case AD_DRLI:
            return !(yours ? youResistDraining() : resists_drli(mtmp));
        case AD_STON:
            return !(yours ? youResistStoning() : resists_ston(mtmp));
        default:
            impossible("Weird weapon special attack.");
        }
    }
    return 0;
}

/* return the M2 flags of monster that an artifact's special attacks apply
 * against */
long
spec_m2(otmp)
struct obj *otmp;
{
    const struct artifact *artifact = get_artifact(otmp);

    if (artifact)
        return artifact->mtype;
    return 0L;
}

/* special attack bonus */
int
spec_abon(otmp, mon)
struct obj *otmp;
struct monst *mon;
{
    const struct artifact *weap = get_artifact(otmp);

    /* no need for an extra check for `NO_ATTK' because this will
       always return 0 for any artifact which has that attribute */

    if (weap && weap->attk.dice && spec_applies(weap, mon))
        return rnd((int) weap->attk.dice);
    return 0;
}

/* special damage bonus */
int
spec_dbon(otmp, mon, tmp)
struct obj *otmp;
struct monst *mon;
int tmp;
{
    register const struct artifact *weap = get_artifact(otmp);

    if (!weap || (weap->attk.damageType == AD_PHYS /* check for `NO_ATTK' */
                  && weap->attk.dice == 0 && weap->attk.diceSides == 0))
        spec_dbon_applies = FALSE;
    else if (otmp->oartifact == ART_GRIMTOOTH)
        /* Grimtooth has SPFX settings to warn against elves but we want its
           damage bonus to apply to all targets, so bypass spec_applies() */
        spec_dbon_applies = TRUE;
    else
        spec_dbon_applies = spec_applies(weap, mon);

    if (spec_dbon_applies)
        return weap->attk.diceSides ? rnd((int) weap->attk.diceSides) : max(tmp, 1);
    return 0;
}

/* add identified artifact to discoveries list */
void
discover_artifact(m)
xchar m;
{
    int i;

    /* look for this artifact in the discoveries list;
       if we hit an empty slot then it's not present, so add it */
    for (i = 0; i < NROFARTIFACTS; i++)
        if (artidisco[i] == 0 || artidisco[i] == m) {
            artidisco[i] = m;
            return;
        }
    /* there is one slot per artifact, so we should never reach the
       end without either finding the artifact or an empty slot... */
    impossible("couldn't discover artifact (%d)", (int) m);
}

/* used to decide whether an artifact has been fully identified */
boolean
undiscovered_artifact(m)
xchar m;
{
    int i;

    /* look for this artifact in the discoveries list;
       if we hit an empty slot then it's undiscovered */
    for (i = 0; i < NROFARTIFACTS; i++)
        if (artidisco[i] == m)
            return FALSE;
        else if (artidisco[i] == 0)
            break;
    return TRUE;
}

/* display a list of discovered artifacts; return their count */
int
disp_artifact_discoveries(tmpwin)
winid tmpwin; /* supplied by dodiscover() */
{
    int i, m, otyp;
    char buf[BUFSZ];

    for (i = 0; i < NROFARTIFACTS; i++) {
        if (artidisco[i] == 0)
            break; /* empty slot implies end of list */
        if (tmpwin == WIN_ERR)
            continue; /* for WIN_ERR, we just count */

        if (i == 0)
            putstr(tmpwin, iflags.menu_headings, "Artifacts");
        m = artidisco[i];
        otyp = artilist[m].otyp;
        Sprintf(buf, "  %s [%s %s]", artiname(m),
                align_str(artilist[m].alignment), simple_typename(otyp));
        putstr(tmpwin, 0, buf);
    }
    return i;
}

/*
 * Magicbane's intrinsic magic is incompatible with normal
 * enchantment magic.  Thus, its effects have a negative
 * dependence on spe.  Against low mr victims, it typically
 * does "double athame" damage, 2d4.  Occasionally, it will
 * cast unbalancing magic which effectively averages out to
 * 4d4 damage (3d4 against high mr victims), for spe = 0.
 *
 * Prior to 3.4.1, the cancel (aka purge) effect always
 * included the scare effect too; now it's one or the other.
 * Likewise, the stun effect won't be combined with either
 * of those two; it will be chosen separately or possibly
 * used as a fallback when scare or cancel fails.
 *
 * [Historical note: a change to artifact_hit() for 3.4.0
 * unintentionally made all of Magicbane's special effects
 * be blocked if the defender successfully saved against a
 * stun attack.  As of 3.4.1, those effects can occur but
 * will be slightly less likely than they were in 3.3.x.]
 */
#define MB_MAX_DIEROLL 8 /* rolls above this aren't magical */
static const char *const mb_verb[2][4] = {
    { "probe", "stun", "scare", "cancel" },
    { "prod", "amaze", "tickle", "purge" },
};
#define MB_INDEX_PROBE 0
#define MB_INDEX_STUN 1
#define MB_INDEX_SCARE 2
#define MB_INDEX_CANCEL 3

/* called when someone is being hit by Magicbane */
STATIC_OVL boolean
Mb_hit(magr, mdef, mb, dmgptr, dieroll, vis, hittee)
struct monst *magr, *mdef; /* attacker and defender */
struct obj *mb;            /* Magicbane */
int *dmgptr;               /* extra damage target will suffer */
int dieroll;               /* d20 that has already scored a hit */
boolean vis;               /* whether the action can be seen */
char *hittee;              /* target's name: "you" or mon_nam(mdef) */
{
    int old_uasmon;
    const char *verb, *fakename;
    boolean youattack = (magr == &youmonst), youdefend = (mdef == &youmonst),
            resisted = FALSE, do_stun, do_confuse, result;
    int attack_indx, scare_dieroll = MB_MAX_DIEROLL / 2;

    result = FALSE; /* no message given yet */
    /* the most severe effects are less likely at higher enchantment */
    if (mb->spe >= 3)
        scare_dieroll /= (1 << (mb->spe / 3));
    /* if target successfully resisted the artifact damage bonus,
       reduce overall likelihood of the assorted special effects */
    if (!spec_dbon_applies)
        dieroll += 1;

    /* might stun even when attempting a more severe effect, but
       in that case it will only happen if the other effect fails;
       extra damage will apply regardless; 3.4.1: sometimes might
       just probe even when it hasn't been enchanted */
    do_stun = (max(mb->spe, 0) < rn2(spec_dbon_applies ? 11 : 7));

    /* the special effects also boost physical damage; increments are
       generally cumulative, but since the stun effect is based on a
       different criterium its damage might not be included; the base
       damage is either 1d4 (athame) or 2d4 (athame+spec_dbon) depending
       on target's resistance check against AD_STUN (handled by caller)
       [note that a successful save against AD_STUN doesn't actually
       prevent the target from ending up stunned] */
    attack_indx = MB_INDEX_PROBE;
    *dmgptr += rnd(4); /* (2..3)d4 */
    if (do_stun) {
        attack_indx = MB_INDEX_STUN;
        *dmgptr += rnd(4); /* (3..4)d4 */
    }
    if (dieroll <= scare_dieroll) {
        attack_indx = MB_INDEX_SCARE;
        *dmgptr += rnd(4); /* (3..5)d4 */
    }
    if (dieroll <= (scare_dieroll / 2)) {
        attack_indx = MB_INDEX_CANCEL;
        *dmgptr += rnd(4); /* (4..6)d4 */
    }

    /* give the hit message prior to inflicting the effects */
    verb = mb_verb[!!youAreHallucinating()][attack_indx];
    if (youattack || youdefend || vis) {
        result = TRUE;
        pline_The("magic-absorbing blade %s %s!",
                  vtense((const char *) 0, verb), hittee);
        /* assume probing has some sort of noticeable feedback
           even if it is being done by one monster to another */
        if (attack_indx == MB_INDEX_PROBE && !canspotmon(mdef))
            map_invisible(mdef->mx, mdef->my);
    }

    /* now perform special effects */
    switch (attack_indx) {
    case MB_INDEX_CANCEL:
        old_uasmon = pmid4you();
        /* No mdef->mcan check: even a cancelled monster can be polymorphed
         * into a golem, and the "cancel" effect acts as if some magical
         * energy remains in spellcasting defenders to be absorbed later.
         */
        if (!cancel_monst(mdef, mb, youattack, FALSE, FALSE)) {
            resisted = TRUE;
        } else {
            do_stun = FALSE;
            if (youdefend) {
                if (pmid4you() != old_uasmon)
                    *dmgptr = 0; /* rehumanized, so no more damage */
                if (maximumMagicalEnergy() > 0) {
                    You("lose magical energy!");
                    decreaseMaximumMagicalEnergy(1);
                    if (currentMagicalEnergy() > 0)
                        decreaseCurrentMagicalEnergy(1);
                    context.botl = 1;
                }
            } else {
                if (pmid4mon(mdef) == PM_CLAY_GOLEM)
                    mdef->mhp = 1; /* cancelled clay golems will die */
                if (youattack && monsterHasAttackType(pmid4mon(mdef), AT_MAGC)) {
                    You("absorb magical energy!");
                    increaseMaximumMagicalEnergy(1);
                    increaseCurrentMagicalEnergy(1);
                    context.botl = 1;
                }
            }
        }
        break;

    case MB_INDEX_SCARE:
        if (youdefend) {
            if (youResistMagic()) {
                resisted = TRUE;
            } else {
                nomul(-3);
                multi_reason = "being scared stiff";
                nomovemsg = "";
                if (magr && magr == u.ustuck && monsterSticksInCombat(pmid4you())) {
                    u.ustuck = (struct monst *) 0;
                    You("release %s!", mon_nam(magr));
                }
            }
        } else {
            if (rn2(2) && resist(mdef, WEAPON_CLASS, 0, NOTELL))
                resisted = TRUE;
            else
                monflee(mdef, 3, FALSE, (mdef->mhp > *dmgptr));
        }
        if (!resisted)
            do_stun = FALSE;
        break;

    case MB_INDEX_STUN:
        do_stun = TRUE; /* (this is redundant...) */
        break;

    case MB_INDEX_PROBE:
        if (youattack && (mb->spe == 0 || !rn2(3 * abs(mb->spe)))) {
            pline_The("%s is insightful.", verb);
            /* pre-damage status */
            probe_monster(mdef);
        }
        break;
    }
    /* stun if that was selected and a worse effect didn't occur */
    if (do_stun) {
        if (youdefend)
            make_stunned((yourIntrinsicTimeout(STUNNED) + 3L), FALSE);
        else
            mdef->mstun = 1;
        /* avoid extra stun message below if we used mb_verb["stun"] above */
        if (attack_indx == MB_INDEX_STUN)
            do_stun = FALSE;
    }
    /* lastly, all this magic can be confusing... */
    do_confuse = !rn2(12);
    if (do_confuse) {
        if (youdefend)
            make_confused((yourIntrinsicTimeout(CONFUSION) + 4L), FALSE);
        else
            mdef->mconf = 1;
    }

    /* now give message(s) describing side-effects;
       don't let vtense() be fooled by assigned name ending in 's' */
    fakename = youdefend ? "you" : "mon";
    if (youattack || youdefend || vis) {
        (void) upstart(hittee); /* capitalize */
        if (resisted) {
            pline("%s %s!", hittee, vtense(fakename, "resist"));
            shieldeff(youdefend ? currentX() : mdef->mx,
                      youdefend ? currentY() : mdef->my);
        }
        if ((do_stun || do_confuse) && flags.verbose) {
            char buf[BUFSZ];

            buf[0] = '\0';
            if (do_stun)
                Strcat(buf, "stunned");
            if (do_stun && do_confuse)
                Strcat(buf, " and ");
            if (do_confuse)
                Strcat(buf, "confused");
            pline("%s %s %s%c", hittee, vtense(fakename, "are"), buf,
                  (do_stun && do_confuse) ? '!' : '.');
        }
    }

    return result;
}

/* Function used when someone attacks someone else with an artifact
 * weapon.  Only adds the special (artifact) damage, and returns a 1 if it
 * did something special (in which case the caller won't print the normal
 * hit message).  This should be called once upon every artifact attack;
 * dmgval() no longer takes artifact bonuses into account.  Possible
 * extension: change the killer so that when an orc kills you with
 * Stormbringer it's "killed by Stormbringer" instead of "killed by an orc".
 */
boolean
artifact_hit(magr, mdef, otmp, dmgptr, dieroll)
struct monst *magr, *mdef;
struct obj *otmp;
int *dmgptr;
int dieroll; /* needed for Magicbane and vorpal blades */
{
    boolean youattack = (magr == &youmonst);
    boolean youdefend = (mdef == &youmonst);
    boolean vis = (!youattack && magr && cansee(magr->mx, magr->my))
                  || (!youdefend && cansee(mdef->mx, mdef->my))
                  || (youattack && swallowed() && mdef == u.ustuck && youCanSee());
    boolean realizes_damage;
    const char *wepdesc;
    static const char you[] = "you";
    char hittee[BUFSZ];

    Strcpy(hittee, youdefend ? you : mon_nam(mdef));

    /* The following takes care of most of the damage, but not all--
     * the exception being for level draining, which is specially
     * handled.  Messages are done in this function, however.
     */
    *dmgptr += spec_dbon(otmp, mdef, *dmgptr);

    if (youattack && youdefend) {
        impossible("attacking yourself with weapon?");
        return FALSE;
    }

    realizes_damage = (youdefend || vis
                       /* feel the effect even if not seen */
                       || (youattack && mdef == u.ustuck));

    /* the four basic attacks: fire, cold, shock and missiles */
    if (attacks(AD_FIRE, otmp)) {
        if (realizes_damage)
            pline_The("fiery blade %s %s%c",
                      !spec_dbon_applies
                          ? "hits"
                          : (pmid4mon(mdef) == PM_WATER_ELEMENTAL)
                                ? "vaporizes part of"
                                : "burns",
                      hittee, !spec_dbon_applies ? '.' : '!');
        if (!rn2(4))
            (void) destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
        if (!rn2(4))
            (void) destroy_mitem(mdef, SCROLL_CLASS, AD_FIRE);
        if (!rn2(7))
            (void) destroy_mitem(mdef, SPBOOK_CLASS, AD_FIRE);
        if (youdefend && youAreTurningToSlime())
            burn_away_slime();
        return realizes_damage;
    }
    if (attacks(AD_COLD, otmp)) {
        if (realizes_damage)
            pline_The("ice-cold blade %s %s%c",
                      !spec_dbon_applies ? "hits" : "freezes", hittee,
                      !spec_dbon_applies ? '.' : '!');
        if (!rn2(4))
            (void) destroy_mitem(mdef, POTION_CLASS, AD_COLD);
        return realizes_damage;
    }
    if (attacks(AD_ELEC, otmp)) {
        if (realizes_damage)
            pline_The("massive hammer hits%s %s%c",
                      !spec_dbon_applies ? "" : "!  Lightning strikes",
                      hittee, !spec_dbon_applies ? '.' : '!');
        if (!rn2(5))
            (void) destroy_mitem(mdef, RING_CLASS, AD_ELEC);
        if (!rn2(5))
            (void) destroy_mitem(mdef, WAND_CLASS, AD_ELEC);
        return realizes_damage;
    }
    if (attacks(AD_MAGM, otmp)) {
        if (realizes_damage)
            pline_The("imaginary widget hits%s %s%c",
                      !spec_dbon_applies
                          ? ""
                          : "!  A hail of magic missiles strikes",
                      hittee, !spec_dbon_applies ? '.' : '!');
        return realizes_damage;
    }

    if (attacks(AD_STUN, otmp) && dieroll <= MB_MAX_DIEROLL) {
        /* Magicbane's special attacks (possibly modifies hittee[]) */
        return Mb_hit(magr, mdef, otmp, dmgptr, dieroll, vis, hittee);
    }

    if (!spec_dbon_applies) {
        /* since damage bonus didn't apply, nothing more to do;
           no further attacks have side-effects on inventory */
        return FALSE;
    }

    /* We really want "on a natural 20" but Nethack does it in */
    /* reverse from AD&D. */
    if (spec_ability(otmp, SPFX_BEHEAD)) {
        if (otmp->oartifact == ART_TSURUGI_OF_MURAMASA && dieroll == 1) {
            wepdesc = "The razor-sharp blade";
            /* not really beheading, but so close, why add another SPFX */
            if (youattack && swallowed() && mdef == u.ustuck) {
                You("slice %s wide open!", mon_nam(mdef));
                *dmgptr = 2 * mdef->mhp + FATAL_DAMAGE_MODIFIER;
                return TRUE;
            }
            if (!youdefend) {
                /* allow normal cutworm() call to add extra damage */
                if (notonhead)
                    return FALSE;

                if (isBigMonster(pmid4mon(mdef))) {
                    if (youattack)
                        You("slice deeply into %s!", mon_nam(mdef));
                    else if (vis)
                        pline("%s cuts deeply into %s!", Monnam(magr),
                              hittee);
                    *dmgptr *= 2;
                    return TRUE;
                }
                *dmgptr = 2 * mdef->mhp + FATAL_DAMAGE_MODIFIER;
                pline("%s cuts %s in half!", wepdesc, mon_nam(mdef));
                otmp->dknown = TRUE;
                return TRUE;
            } else {
                if (isBigMonster(pmid4you())) {
                    pline("%s cuts deeply into you!",
                          magr ? Monnam(magr) : wepdesc);
                    *dmgptr *= 2;
                    return TRUE;
                }

                /* Players with negative AC's take less damage instead
                 * of just not getting hit.  We must add a large enough
                 * value to the damage so that this reduction in
                 * damage does not prevent death.
                 */
                *dmgptr = 2 * (areYouPolymorphed() ? currentHitPointsAsMonster() : currentHitPoints()) + FATAL_DAMAGE_MODIFIER;
                pline("%s cuts you in half!", wepdesc);
                otmp->dknown = TRUE;
                return TRUE;
            }
        } else if (otmp->oartifact == ART_VORPAL_BLADE
                   && (dieroll == 1 || pmid4mon(mdef) == PM_JABBERWOCK)) {
            static const char *const behead_msg[2] = { "%s beheads %s!",
                                                       "%s decapitates %s!" };

            if (youattack && swallowed() && mdef == u.ustuck)
                return FALSE;
            wepdesc = artilist[ART_VORPAL_BLADE].name;
            if (!youdefend) {
		int pmid = pmid4mon(mdef);
                if (!hasAHead(pmid) || notonhead || swallowed()) {
                    if (youattack)
                        pline("Somehow, you miss %s wildly.", mon_nam(mdef));
                    else if (vis)
                        pline("Somehow, %s misses wildly.", mon_nam(magr));
                    *dmgptr = 0;
                    return (boolean) (youattack || vis);
                }
                if (isNoncorporeal(pmid) || isAmorphous(pmid)) {
                    pline("%s slices through %s %s.", wepdesc,
                          s_suffix(mon_nam(mdef)), mbodypart(mdef, NECK));
                    return TRUE;
                }
                *dmgptr = 2 * mdef->mhp + FATAL_DAMAGE_MODIFIER;
                pline(behead_msg[rn2(SIZE(behead_msg))], wepdesc,
                      mon_nam(mdef));
                if (youAreHallucinating() && !flags.female)
                    pline("Good job Henry, but that wasn't Anne.");
                otmp->dknown = TRUE;
                return TRUE;
            } else {
		int upmid = pmid4you();
                if (!hasAHead(upmid)) {
                    pline("Somehow, %s misses you wildly.",
                          magr ? mon_nam(magr) : wepdesc);
                    *dmgptr = 0;
                    return TRUE;
                }
                if (isNoncorporeal(upmid) || isAmorphous(upmid)) {
                    pline("%s slices through your %s.", wepdesc,
                          body_part(NECK));
                    return TRUE;
                }
                *dmgptr = 2 * (areYouPolymorphed() ? currentHitPointsAsMonster() : currentHitPoints()) + FATAL_DAMAGE_MODIFIER;
                pline(behead_msg[rn2(SIZE(behead_msg))], wepdesc, "you");
                otmp->dknown = TRUE;
                /* Should amulets fall off? */
                return TRUE;
            }
        }
    }
    if (spec_ability(otmp, SPFX_DRLI)) {
        /* some non-living creatures (golems, vortices) are
           vulnerable to life drain effects */
        const char *life = isNonliving(pmid4mon(mdef)) ? "animating force" : "life";

        if (!youdefend) {
            if (vis) {
                if (otmp->oartifact == ART_STORMBRINGER)
                    pline_The("%s blade draws the %s from %s!",
                              hcolor(NH_BLACK), life, mon_nam(mdef));
                else
                    pline("%s draws the %s from %s!",
                          The(distant_name(otmp, xname)), life,
                          mon_nam(mdef));
            }
            if (mdef->m_lev == 0) {
                *dmgptr = 2 * mdef->mhp + FATAL_DAMAGE_MODIFIER;
            } else {
                int drain = monhp_per_lvl(mdef);

                *dmgptr += drain;
                mdef->mhpmax -= drain;
                mdef->m_lev--;
                drain /= 2;
                if (drain)
                    healup(drain, 0, FALSE, FALSE);
            }
            return vis;
        } else { /* youdefend */
            int oldhpmax = maximumHitPoints();

            if (youCannotSee())
                You_feel("an %s drain your %s!",
                         (otmp->oartifact == ART_STORMBRINGER)
                            ? "unholy blade"
                            : "object",
                         life);
            else if (otmp->oartifact == ART_STORMBRINGER)
                pline_The("%s blade drains your %s!", hcolor(NH_BLACK), life);
            else
                pline("%s drains your %s!", The(distant_name(otmp, xname)),
                      life);
            losexp("life drainage");
            if (magr && magr->mhp < magr->mhpmax) {
                magr->mhp += (oldhpmax - maximumHitPoints()) / 2;
                if (magr->mhp > magr->mhpmax)
                    magr->mhp = magr->mhpmax;
            }
            return TRUE;
        }
    }
    return FALSE;
}

static NEARDATA const char recharge_type[] = { ALLOW_COUNT, ALL_CLASSES, 0 };
static NEARDATA const char invoke_types[] = { ALL_CLASSES, 0 };
/* #invoke: an "ugly check" filters out most objects */

/* the #invoke command */
int
doinvoke()
{
    struct obj *obj;

    obj = getobj(invoke_types, "invoke");
    if (!obj)
        return 0;
    if (!retouch_object(&obj, FALSE))
        return 1;
    return arti_invoke(obj);
}

STATIC_OVL int
arti_invoke(obj)
struct obj *obj;
{
    register const struct artifact *oart = get_artifact(obj);
    if (!obj) {
        impossible("arti_invoke without obj");
        return 0;
    }
    if (!oart || !oart->inv_prop) {
        if (obj->otyp == CRYSTAL_BALL)
            use_crystal_ball(&obj);
        else
            pline1(nothing_happens);
        return 1;
    }

    if (oart->inv_prop > LAST_PROP) {
        /* It's a special power, not "just" a property */
        if (obj->age > monstermoves) {
            /* the artifact is tired :-) */
            You_feel("that %s %s ignoring you.", the(xname(obj)),
                     otense(obj, "are"));
            /* and just got more so; patience is essential... */
            obj->age += (long) d(3, 10);
            return 1;
        }
        obj->age = monstermoves + rnz(100);

        switch (oart->inv_prop) {
        case TAMING: {
            struct obj pseudo;

            pseudo =
                zeroobj; /* neither cursed nor blessed, zero oextra too */
            pseudo.otyp = SCR_TAMING;
            (void) seffects(&pseudo);
            break;
        }
        case HEALING: {
            int healamt = (maximumHitPoints() + 1 - currentHitPoints()) / 2;

            if (areYouPolymorphed())
                healamt = (maximumHitPointsAsMonster() + 1 - currentHitPointsAsMonster()) / 2;
            if (healamt || youAreSick() || youAreTurningToSlime() || yourIntrinsic(BLINDED) > creamed())
                You_feel("better.");
            else
                goto nothing_special;
            if (healamt > 0) {
                if (areYouPolymorphed())
                    increaseCurrentHitPointsAsMonster(healamt);
                else
                    increaseCurrentHitPoints(healamt);
            }
            if (youAreSick())
                make_sick(0L, (char *) 0, FALSE, sickAll());
            if (youAreTurningToSlime())
                make_slimed(0L, (char *) 0);
            if (yourIntrinsic(BLINDED) > creamed())
                make_blinded(creamed(), FALSE);
            context.botl = 1;
            break;
        }
        case ENERGY_BOOST: {
            int epboost = (maximumMagicalEnergy() + 1 - currentMagicalEnergy()) / 2;
            if (epboost > 120)
                epboost = 120; /* arbitrary */
            else if (epboost < 12)
                epboost = maximumMagicalEnergy() - currentMagicalEnergy();
            if (epboost) {
                You_feel("re-energized.");
                increaseCurrentMagicalEnergy(epboost);
                context.botl = 1;
            } else
                goto nothing_special;
            break;
        }
        case UNTRAP: {
            if (!untrap(TRUE)) {
                obj->age = 0; /* don't charge for changing their mind */
                return 0;
            }
            break;
        }
        case CHARGE_OBJ: {
            struct obj *otmp = getobj(recharge_type, "charge");
            boolean b_effect;

            if (!otmp) {
                obj->age = 0;
                return 0;
            }
            b_effect =
                obj->blessed && ((yourRolePMID() == oart->role) || !oart->role);
            recharge(otmp, b_effect ? 1 : obj->cursed ? -1 : 0);
            update_inventory();
            break;
        }
        case LEV_TELE:
            level_tele();
            break;
        case CREATE_PORTAL: {
            int i, num_ok_dungeons, last_ok_dungeon = 0;
            d_level newlev;
            extern int n_dgns; /* from dungeon.c */
            winid tmpwin = create_nhwindow(NHW_MENU);
            anything any;

            any = zeroany; /* set all bits to zero */
            start_menu(tmpwin);
            /* use index+1 (cant use 0) as identifier */
            for (i = num_ok_dungeons = 0; i < n_dgns; i++) {
                if (!dungeons[i].dunlev_ureached)
                    continue;
                any.a_int = i + 1;
                add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                         dungeons[i].dname, MENU_UNSELECTED);
                num_ok_dungeons++;
                last_ok_dungeon = i;
            }
            end_menu(tmpwin, "Open a portal to which dungeon?");
            if (num_ok_dungeons > 1) {
                /* more than one entry; display menu for choices */
                menu_item *selected;
                int n;

                n = select_menu(tmpwin, PICK_ONE, &selected);
                if (n <= 0) {
                    destroy_nhwindow(tmpwin);
                    goto nothing_special;
                }
                i = selected[0].item.a_int - 1;
                free((genericptr_t) selected);
            } else
                i = last_ok_dungeon; /* also first & only OK dungeon */
            destroy_nhwindow(tmpwin);

            /*
             * i is now index into dungeon structure for the new dungeon.
             * Find the closest level in the given dungeon, open
             * a use-once portal to that dungeon and go there.
             * The closest level is either the entry or dunlev_ureached.
             */
            newlev.dnum = i;
            if (dungeons[i].depth_start >= currentDepth())
                newlev.dlevel = dungeons[i].entry_lev;
            else
                newlev.dlevel = dungeons[i].dunlev_ureached;

            if (haveSpecialItem(SPECIAL_ITEM_AMULET()) || areYouInEndgame() || In_endgame(&newlev)
                || newlev.dnum == currentDungeonNumber() || !next_to_u()) {
                You_feel("very disoriented for a moment.");
            } else {
                if (youCanSee())
                    You("are surrounded by a shimmering sphere!");
                else
                    You_feel("weightless for a moment.");
                goto_level(&newlev, FALSE, FALSE, FALSE);
            }
            break;
        }
        case ENLIGHTENING:
            enlightenment(MAGICENLIGHTENMENT, ENL_GAMEINPROGRESS);
            break;
        case CREATE_AMMO: {
            struct obj *otmp = mksobj(ARROW, TRUE, FALSE);

            if (!otmp)
                goto nothing_special;
            otmp->blessed = obj->blessed;
            otmp->cursed = obj->cursed;
            otmp->bknown = obj->bknown;
            if (obj->blessed) {
                if (otmp->spe < 0)
                    otmp->spe = 0;
                otmp->quan += rnd(10);
            } else if (obj->cursed) {
                if (otmp->spe > 0)
                    otmp->spe = 0;
            } else
                otmp->quan += rnd(5);
            otmp->owt = weight(otmp);
            otmp =
                hold_another_object(otmp, "Suddenly %s out.",
                                    aobjnam(otmp, "fall"), (const char *) 0);
            break;
        }
        }
    } else {
        toggleYourExtrinsicMask(oart->inv_prop, W_ARTI);

        long eprop = yourExtrinsic(oart->inv_prop);
        long iprop = yourIntrinsic(oart->inv_prop);
        boolean on = (eprop & W_ARTI) != 0; /* true if prop just set */

        if (on && obj->age > monstermoves) {
            /* the artifact is tired :-) */
            toggleYourExtrinsicMask(oart->inv_prop, W_ARTI);
            You_feel("that %s %s ignoring you.", the(xname(obj)),
                     otense(obj, "are"));
            /* can't just keep repeatedly trying */
            obj->age += (long) d(3, 10);
            return 1;
        } else if (!on) {
            /* when turning off property, determine downtime */
            /* arbitrary for now until we can tune this -dlc */
            obj->age = monstermoves + rnz(100);
        }

        if ((eprop & ~W_ARTI) || iprop) {
        nothing_special:
            /* you had the property from some other source too */
            if (carried(obj))
                You_feel("a surge of power, but nothing seems to happen.");
            return 1;
        }
        switch (oart->inv_prop) {
        case CONFLICT:
            if (on)
                You_feel("like a rabble-rouser.");
            else
                You_feel("the tension decrease around you.");
            break;
        case LEVITATION:
            if (on) {
                float_up();
                spoteffects(FALSE);
            } else
                (void) float_down(I_SPECIAL | TIMEOUT, W_ARTI);
            break;
        case INVIS:
            if (youAreBlockedFrom(INVIS) || youCannotSee())
                goto nothing_special;
            newsym(currentX(), currentY());
            if (on)
                Your("body takes on a %s transparency...",
                     youAreHallucinating() ? "normal" : "strange");
            else
                Your("body seems to unfade...");
            break;
        }
    }

    return 1;
}

/* will freeing this object from inventory cause levitation to end? */
boolean
finesse_ahriman(obj)
struct obj *obj;
{
    const struct artifact *oart;

    long save_hLev;
    long save_eLev;
    long save_bLev;

    boolean result;

    /* if we aren't levitating or this isn't an artifact which confers
       levitation via #invoke then freeinv() won't toggle levitation */
    if (!youAreLevitating() || (oart = get_artifact(obj)) == 0
        || oart->inv_prop != LEVITATION || !(yourExtrinsicHasMask(LEVITATION, W_ARTI)))
        return FALSE;

    /* arti_invoke(off) -> float_down() clears I_SPECIAL|TIMEOUT & W_ARTI;
       probe ahead to see whether that actually results in floating down;
       (this assumes that there aren't two simultaneously invoked artifacts
       both conferring levitation--safe, since if there were two of them,
       invoking the 2nd would negate the 1st rather than stack with it) */

    save_hLev = yourIntrinsic(LEVITATION);
    save_eLev = yourExtrinsic(LEVITATION);
    save_bLev = yourBlocker(LEVITATION);

    unsetYourIntrinsicMask(LEVITATION, (I_SPECIAL | TIMEOUT));
    unsetYourExtrinsicMask(LEVITATION, W_ARTI);
    result = !youAreLevitating();

    setYourIntrinsic(LEVITATION, save_hLev);
    setYourExtrinsic(LEVITATION, save_eLev);
    setYourBlocker(LEVITATION, save_bLev);

    return result;
}

/* WAC return TRUE if artifact is always lit */
boolean
artifact_light(obj)
struct obj *obj;
{
    return (boolean) (get_artifact(obj) && obj->oartifact == ART_SUNSWORD);
}

/* KMH -- Talking artifacts are finally implemented */
void
arti_speak(obj)
struct obj *obj;
{
    register const struct artifact *oart = get_artifact(obj);
    const char *line;
    char buf[BUFSZ];

    /* Is this a speaking artifact? */
    if (!oart || !(oart->spfx & SPFX_SPEAK))
        return;

    line = getrumor(bcsign(obj), buf, TRUE);
    if (!*line)
        line = "NetHack rumors file closed for renovation.";
    pline("%s:", Tobjnam(obj, "whisper"));
    verbalize1(line);
    return;
}

boolean
artifact_has_invprop(otmp, inv_prop)
struct obj *otmp;
uchar inv_prop;
{
    const struct artifact *arti = get_artifact(otmp);

    return (boolean) (arti && (arti->inv_prop == inv_prop));
}

/* Return the price sold to the hero of a given artifact or unique item */
long
arti_cost(otmp)
struct obj *otmp;
{
    if (!otmp->oartifact)
        return (long) objects[otmp->otyp].oc_cost;
    else if (artilist[(int) otmp->oartifact].cost)
        return artilist[(int) otmp->oartifact].cost;
    else
        return (100L * (long) objects[otmp->otyp].oc_cost);
}

STATIC_OVL uchar
extrinsic_to_damageType(extrinsic)
int extrinsic;
{
    struct abil2damageType {
        int extrinsic;
        uchar damageType;
    } abil2damageType[] = {
        { FIRE_RES, AD_FIRE },
        { COLD_RES, AD_COLD },
        { SHOCK_RES, AD_ELEC },
        { ANTIMAGIC, AD_MAGM },
        { DISINT_RES, AD_DISN },
        { POISON_RES, AD_DRST },
    };
    int k;

    for (k = 0; k < SIZE(abil2damageType); k++) {
        if (abil2damageType[k].extrinsic == extrinsic)
            return abil2damageType[k].damageType;
    }
    return 0;
}

STATIC_OVL unsigned long
extrinsic_to_spfx(extrinsic)
int extrinsic;
{
    static const struct abil2spfx_tag {
        int extrinsic;
        unsigned long spfx;
    } abil2spfx[] = {
        { SEARCHING, SPFX_SEARCH },
        { HALLUC_RES, SPFX_HALRES },
        { TELEPAT, SPFX_ESP },
        { STEALTH, SPFX_STLTH },
        { REGENERATION, SPFX_REGEN },
        { TELEPORT_CONTROL, SPFX_TCTRL },
        { WARN_OF_MON, SPFX_WARN },
        { WARNING, SPFX_WARN },
        { ENERGY_REGENERATION, SPFX_EREGEN },
        { HALF_SPDAM, SPFX_HSPDAM },
        { HALF_PHDAM, SPFX_HPHDAM },
        { REFLECTING, SPFX_REFLECT },
    };
    int k;

    for (k = 0; k < SIZE(abil2spfx); k++) {
        if (abil2spfx[k].extrinsic == extrinsic)
            return abil2spfx[k].spfx;
    }
    return 0L;
}

/*
 * Return the first item that is conveying a particular intrinsic.
 */
struct obj *
whatGivesExtrinsic(extrinsic)
int extrinsic;
{
    struct obj *obj;
    uchar dtyp;
    unsigned long spfx;
    long wornbits;
    long wornmask = (W_ARM | W_ARMC | W_ARMH | W_ARMS
                     | W_ARMG | W_ARMF | W_ARMU
                     | W_AMUL | W_RINGL | W_RINGR | W_TOOL
                     /* [do W_ART and W_ARTI actually belong here?] */
                     | W_ART | W_ARTI);

    if (usingTwoWeapons())
        wornmask |= W_SWAPWEP;
    dtyp = extrinsic_to_damageType(extrinsic);
    spfx = extrinsic_to_spfx(extrinsic);
    wornbits = (wornmask & yourExtrinsic(extrinsic));

    for (obj = invent; obj; obj = obj->nobj) {
        if (obj->oartifact
            && (extrinsic != WARN_OF_MON || context.warntype.obj)) {
            const struct artifact *art = get_artifact(obj);

            if (art) {
                if (dtyp) {
                    if (art->cary.damageType == dtyp || art->defn.damageType == dtyp)
                        return obj;
                }
                if (spfx) {
                    /* property conferred when carried */
                    if ((art->cspfx & spfx) == spfx)
                        return obj;
                    /* property conferred when wielded or worn */
                    if ((art->spfx & spfx) == spfx && obj->owornmask)
                        return obj;
                }
            }
        } else {
            if (wornbits && wornbits == (wornmask & obj->owornmask))
                return obj;
        }
    }
    return (struct obj *) 0;
}

const char *
glow_color(arti_indx)
int arti_indx;
{
    int colornum = artilist[arti_indx].acolor;
    const char *colorstr = clr2colorname(colornum);

    return hcolor(colorstr);
}

/* use for warning "glow" for Sting, Orcrist, and Grimtooth */
void
Sting_effects(orc_count)
int orc_count; /* new count (warn_obj_cnt is old count); -1 is a flag value */
{
    if (uwep
        && (uwep->oartifact == ART_STING
            || uwep->oartifact == ART_ORCRIST
            || uwep->oartifact == ART_GRIMTOOTH)) {
        if (orc_count == -1 && warn_obj_cnt > 0) {
            /* -1 means that blindness has just been toggled; give a
               'continue' message that eventual 'stop' message will match */
            pline("%s is %s.", bare_artifactname(uwep),
                  youCanSee() ? "glowing" : "quivering");
        } else if (orc_count > 0 && warn_obj_cnt == 0) {
            /* 'start' message */
            if (youCanSee())
                pline("%s %s %s!", bare_artifactname(uwep),
                      otense(uwep, "glow"), glow_color(uwep->oartifact));
            else
                pline("%s quivers slightly.", bare_artifactname(uwep));
        } else if (orc_count == 0 && warn_obj_cnt > 0) {
            /* 'stop' message */
            pline("%s stops %s.", bare_artifactname(uwep),
                  youCanSee() ? "glowing" : "quivering");
        }
    }
}

/* called when hero is wielding/applying/invoking a carried item, or
   after undergoing a transformation (alignment change, lycanthropy,
   polymorph) which might affect item access */
int
retouch_object(objp, loseit)
struct obj **objp; /* might be destroyed or unintentionally dropped */
boolean loseit;    /* whether to drop it if hero can longer touch it */
{
    struct obj *obj = *objp;

    if (touch_artifact(obj, &youmonst)) {
        char buf[BUFSZ];
        int dmg = 0, tmp;
        boolean ag =
                    (objects[obj->otyp].oc_material == SILVER && youHateSilver()),
                bane = bane_applies(get_artifact(obj), &youmonst);

        /* nothing else to do if hero can successfully handle this object */
        if (!ag && !bane)
            return 1;

        /* hero can't handle this object, but didn't get touch_artifact()'s
           "<obj> evades your grasp|control" message; give an alternate one */
        You_cant("handle %s%s!", yname(obj),
                 obj->owornmask ? " anymore" : "");
        /* also inflict damage unless touch_artifact() already did so */
        if (!touch_blasted) {
            /* damage is somewhat arbitrary; half the usual 1d20 physical
               for silver, 1d10 magical for <foo>bane, potentially both */
            if (ag)
                tmp = rnd(10), dmg += Maybe_Half_Phys(tmp);
            if (bane)
                dmg += rnd(10);
            Sprintf(buf, "handling %s", killer_xname(obj));
            losehp(dmg, buf, KILLED_BY);
            exercise(A_CON, FALSE);
        }
    }

    /* removing a worn item might result in loss of levitation,
       dropping the hero onto a polymorph trap or into water or
       lava and potentially dropping or destroying the item */
    if (obj->owornmask) {
        struct obj *otmp;

        remove_worn_item(obj, FALSE);
        for (otmp = invent; otmp; otmp = otmp->nobj)
            if (otmp == obj)
                break;
        if (!otmp)
            *objp = obj = 0;
    }

    /* if we still have it and caller wants us to drop it, do so now */
    if (loseit && obj) {
        if (youAreLevitating()) {
            freeinv(obj);
            hitfloor(obj);
        } else {
            /* dropx gives a message iff item lands on an altar */
            if (!IS_ALTAR(levl[currentX()][currentY()].typ))
                pline("%s to the %s.", Tobjnam(obj, "fall"),
                      surface(currentX(), currentY()));
            dropx(obj);
        }
        *objp = obj = 0; /* no longer in inventory */
    }
    return 0;
}

/* an item which is worn/wielded or an artifact which conveys
   something via being carried or which has an #invoke effect
   currently in operation undergoes a touch test; if it fails,
   it will be unworn/unwielded and revoked but not dropped */
STATIC_OVL boolean
untouchable(obj, drop_untouchable)
struct obj *obj;
boolean drop_untouchable;
{
    struct artifact *art;
    boolean beingworn, carryeffect, invoked;
    long wearmask = ~(W_QUIVER | (usingTwoWeapons() ? 0L : W_SWAPWEP) | W_BALL);

    beingworn = ((obj->owornmask & wearmask) != 0L
                 /* some items in use don't have any wornmask setting */
                 || (obj->oclass == TOOL_CLASS
                     && (obj->lamplit || (obj->otyp == LEASH && obj->leashmon)
                         || (Is_container(obj) && Has_contents(obj)))));

    if ((art = get_artifact(obj)) != 0) {
        carryeffect = (art->cary.damageType || art->cspfx);
        invoked = (art->inv_prop > 0 && art->inv_prop <= LAST_PROP
                   && yourExtrinsicHasMask(art->inv_prop, W_ARTI));
    } else {
        carryeffect = invoked = FALSE;
    }

    if (beingworn || carryeffect || invoked) {
        if (!retouch_object(&obj, drop_untouchable)) {
            /* "<artifact> is beyond your control" or "you can't handle
               <object>" has been given and it is now unworn/unwielded
               and possibly dropped (depending upon caller); if dropped,
               carried effect was turned off, else we leave that alone;
               we turn off invocation property here if still carried */
            if (invoked && obj)
                arti_invoke(obj); /* reverse #invoke */
            return TRUE;
        }
    }
    return FALSE;
}

/* check all items currently in use (mostly worn) for touchability */
void
retouch_equipment(dropflag)
int dropflag; /* 0==don't drop, 1==drop all, 2==drop weapon */
{
    static int nesting = 0; /* recursion control */
    struct obj *obj;
    boolean dropit, had_gloves = (uarmg != 0);
    int had_rings = (!!uleft + !!uright);

    /*
     * We can potentially be called recursively if losing/unwearing
     * an item causes worn helm of opposite alignment to come off or
     * be destroyed.
     *
     * BUG: if the initial call was due to putting on a helm of
     * opposite alignment and it does come off to trigger recursion,
     * after the inner call executes, the outer call will finish
     * using the non-helm alignment rather than the helm alignment
     * which triggered this in the first place.
     */
    if (!nesting++)
        clear_bypasses(); /* init upon initial entry */

    dropit = (dropflag > 0); /* drop all or drop weapon */
    /* check secondary weapon first, before possibly unwielding primary */
    if (usingTwoWeapons())
        (void) untouchable(uswapwep, dropit);
    /* check primary weapon next so that they're handled together */
    if (uwep)
        (void) untouchable(uwep, dropit);

    /* in case someone is daft enough to add artifact or silver saddle */
    if (u.usteed && (obj = which_armor(u.usteed, W_SADDLE)) != 0) {
        /* untouchable() calls retouch_object() which expects an object in
           hero's inventory, but remove_worn_item() will be harmless for
           saddle and we're suppressing drop, so this works as intended */
        if (untouchable(obj, FALSE))
            dismount_steed(DISMOUNT_THROWN);
    }
    /*
     * TODO?  Force off gloves if either or both rings are going to
     * become unworn; force off cloak [suit] before suit [shirt].
     * The torso handling is hypothetical; the case for gloves is
     * not, due to the possibility of unwearing silver rings.
     */

    dropit = (dropflag == 1); /* all untouchable items */
    /* loss of levitation (silver ring, or Heart of Ahriman invocation)
       might cause hero to lose inventory items (by dropping into lava,
       for instance), so inventory traversal needs to rescan the whole
       invent chain each time it moves on to another object; we use bypass
       handling to keep track of which items have already been processed */
    while ((obj = nxt_unbypassed_obj(invent)) != 0)
        (void) untouchable(obj, dropit);

    if (had_rings != (!!uleft + !!uright) && uarmg && uarmg->cursed)
        uncurse(uarmg); /* temporary? hack for ring removal plausibility */
    if (had_gloves && !uarmg)
        selftouch("After losing your gloves, you");

    if (!--nesting)
        clear_bypasses(); /* reset upon final exit */
}

/*artifact.c*/
