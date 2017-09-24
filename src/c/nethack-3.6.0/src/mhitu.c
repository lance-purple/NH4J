/* NetHack 3.6	mhitu.c	$NHDT-Date: 1446854230 2015/11/06 23:57:10 $  $NHDT-Branch: master $:$NHDT-Revision: 1.130 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"

STATIC_VAR NEARDATA struct obj *otmp;

STATIC_DCL boolean FDECL(u_slip_free, (struct monst *, const struct Attack));
STATIC_DCL int FDECL(passiveum, (int, struct monst *, const struct Attack));

STATIC_DCL void FDECL(mayberem, (struct obj *, const char *));

STATIC_DCL boolean FDECL(diseasemu, (struct permonst *));
STATIC_DCL int FDECL(hitmu, (struct monst *, const struct Attack));
STATIC_DCL int FDECL(gulpmu, (struct monst *, const struct Attack));
STATIC_DCL int FDECL(explmu, (struct monst *, const struct Attack, BOOLEAN_P));
STATIC_DCL void FDECL(missmu, (struct monst *, BOOLEAN_P, const struct Attack));
STATIC_DCL void FDECL(mswings, (struct monst *, struct obj *));
STATIC_DCL void FDECL(wildmiss, (struct monst *, const struct Attack));

STATIC_DCL void FDECL(hitmsg, (struct monst *, const struct Attack));

/* See comment in mhitm.c.  If we use this a lot it probably should be */
/* changed to a parameter to mhitu. */
static int dieroll;

STATIC_OVL void
hitmsg(mtmp, mattk)
register struct monst *mtmp;
register const struct Attack mattk;
{
    int compat;

    /* Note: if opposite gender, "seductively" */
    /* If same gender, "engagingly" for nymph, normal msg for others */
    if ((compat = could_seduce(mtmp, &youmonst, mattk)) && !mtmp->mcan
        && !mtmp->mspec_used) {
        pline("%s %s you %s.", Monnam(mtmp), youCannotSee() ? "talks to" : "smiles at",
              compat == 2 ? "engagingly" : "seductively");
    } else
        switch (mattk.type) {
        case AT_BITE:
            pline("%s bites!", Monnam(mtmp));
            break;
        case AT_KICK:
            pline("%s kicks%c", Monnam(mtmp),
                  isThickSkinned(pmid4you()) ? '.' : '!');
            break;
        case AT_STNG:
            pline("%s stings!", Monnam(mtmp));
            break;
        case AT_BUTT:
            pline("%s butts!", Monnam(mtmp));
            break;
        case AT_TUCH:
            pline("%s touches you!", Monnam(mtmp));
            break;
        case AT_TENT:
            pline("%s tentacles suck you!", s_suffix(Monnam(mtmp)));
            break;
        case AT_EXPL:
        case AT_BOOM:
            pline("%s explodes!", Monnam(mtmp));
            break;
        default:
            pline("%s hits!", Monnam(mtmp));
        }
}

/* monster missed you */
STATIC_OVL void
missmu(mtmp, nearmiss, mattk)
struct monst *mtmp;
boolean nearmiss;
const struct Attack mattk;
{
    if (!canspotmon(mtmp)) {
        map_invisible(mtmp->mx, mtmp->my);
    }

    if (could_seduce(mtmp, &youmonst, mattk) && !mtmp->mcan) {
        pline("%s pretends to be friendly.", Monnam(mtmp));
    } else {
        pline("%s %smisses!", Monnam(mtmp),
              (nearmiss && flags.verbose) ? "just " : "");
    }

    stop_occupation();
}

/* monster swings obj */
STATIC_OVL void
mswings(mtmp, otemp)
struct monst *mtmp;
struct obj *otemp;
{
    if (flags.verbose && youCanSee() && mon_visible(mtmp)) {
        pline("%s %s %s%s %s.", Monnam(mtmp),
              (objects[otemp->otyp].oc_dir & PIERCE) ? "thrusts" : "swings",
              (otemp->quan > 1L) ? "one of " : "", mhis(mtmp), xname(otemp));
    }
}

/* return how a poison attack was delivered */
const char *
mpoisons_subj(mtmp, mattk)
struct monst *mtmp;
const struct Attack mattk;
{
    if (mattk.type == AT_WEAP) {
        struct obj *mwep = (mtmp == &youmonst) ? uwep : MON_WEP(mtmp);
        /* "Foo's attack was poisoned." is pretty lame, but at least
           it's better than "sting" when not a stinging attack... */
        return (!mwep || !mwep->opoisoned) ? "attack" : "weapon";
    } else {
        return (mattk.type == AT_TUCH) ? "contact"
                  : (mattk.type == AT_GAZE) ? "gaze"
                       : (mattk.type == AT_BITE) ? "bite" : "sting";
    }
}

/* called when your intrinsic speed is taken away */
void
u_slow_down()
{
    setYourIntrinsicMask(FAST, 0L);
    if (!youAreFast())
        You("slow down.");
    else /* speed boots */
        Your("quickness feels less natural.");
    exercise(A_DEX, FALSE);
}

/* monster attacked your displaced image */
STATIC_OVL void
wildmiss(mtmp, mattk)
register struct monst *mtmp;
register const struct Attack mattk;
{
    int compat;

    /* no map_invisible() -- no way to tell where _this_ is coming from */

    if (!flags.verbose)
        return;
    if (!cansee(mtmp->mx, mtmp->my))
        return;
    /* maybe it's attacking an image around the corner? */

    compat = ((mattk.damageType == AD_SEDU || mattk.damageType == AD_SSEX)
              && could_seduce(mtmp, &youmonst, NO_ATTACK));

    if (!mtmp->mcansee || (youAreInvisibleToOthers() && !perceivesTheInvisible(pmid4mon(mtmp)))) {
        const char *swings =
            mattk.type == AT_BITE
                ? "snaps"
                : mattk.type == AT_KICK
                      ? "kicks"
                      : (mattk.type == AT_STNG || mattk.type == AT_BUTT
                         || hasNoLimbs(pmid4mon(mtmp)))
                            ? "lunges"
                            : "swings";

        if (compat)
            pline("%s tries to touch you and misses!", Monnam(mtmp));
        else
            switch (rn2(3)) {
            case 0:
                pline("%s %s wildly and misses!", Monnam(mtmp), swings);
                break;
            case 1:
                pline("%s attacks a spot beside you.", Monnam(mtmp));
                break;
            case 2:
                pline("%s strikes at %s!", Monnam(mtmp),
                      levl[mtmp->mux][mtmp->muy].typ == WATER ? "empty water"
                                                              : "thin air");
                break;
            default:
                pline("%s %s wildly!", Monnam(mtmp), swings);
                break;
            }

    } else if (youAppearDisplaced()) {
        if (compat)
            pline("%s smiles %s at your %sdisplaced image...", Monnam(mtmp),
                  compat == 2 ? "engagingly" : "seductively",
                  youAreInvisibleToOthers() ? "invisible " : "");
        else
            pline("%s strikes at your %sdisplaced image and misses you!",
                  /* Note: if you're both invisible and displaced,
                   * only monsters which see invisible will attack your
                   * displaced image, since the displaced image is also
                   * invisible.
                   */
                  Monnam(mtmp), youAreInvisibleToOthers() ? "invisible " : "");

    } else if (underwater()) {
        /* monsters may miss especially on water level where
           bubbles shake the player here and there */
        if (compat)
            pline("%s reaches towards your distorted image.", Monnam(mtmp));
        else
            pline("%s is fooled by water reflections and misses!",
                  Monnam(mtmp));

    } else
        impossible("%s attacks you without knowing your location?",
                   Monnam(mtmp));
}

void
expels(mtmp, pmid, message)
struct monst *mtmp;
int pmid; /* if mtmp is polymorphed, mdat != mtmp->data */
boolean message;
{
    if (message) {
        if (isAnimal(pmid)) {
            You("get regurgitated!");
        } else {
            char blast[40];
            register int i;

            blast[0] = '\0';
            for (i = 0; i < monsterAttacks(pmid); i++) {
                if (monsterAttack(pmid, i).type == AT_ENGL) {
                    break;
		}
	    }
	    struct Attack mattk = monsterAttack(pmid, i);
            if (mattk.type != AT_ENGL) {
                impossible("Swallower has no engulfing attack?");
	    } else {
                if (isWhirly(pmid)) {
                    switch (mattk.damageType) {
                    case AD_ELEC:
                        Strcpy(blast, " in a shower of sparks");
                        break;
                    case AD_COLD:
                        Strcpy(blast, " in a blast of frost");
                        break;
                    }
                } else
                    Strcpy(blast, " with a squelch");
                You("get expelled from %s%s!", mon_nam(mtmp), blast);
            }
        }
    }
    unstuck(mtmp); /* ball&chain returned in unstuck() */
    mnexto(mtmp);
    newsym(currentX(), currentY());
    spoteffects(TRUE);
    /* to cover for a case where mtmp is not in a next square */
    if (um_dist(mtmp->mx, mtmp->my, 1))
        pline("Brrooaa...  You land hard at some distance.");
}

/* select a monster's next attack, possibly substituting for its usual one */
const struct Attack 
getMonsterAttack(pmid, indx, prev_result)
int pmid;
int indx;
int prev_result[];
{
    struct Attack attk = monsterAttack(pmid, indx);

    /* prevent a monster with two consecutive disease or hunger attacks
       from hitting with both of them on the same turn; if the first has
       already hit, switch to a stun attack for the second */
    if (indx > 0 && prev_result[indx - 1] > 0
        && (attk.damageType == AD_DISE || attk.damageType == AD_PEST
            || attk.damageType == AD_FAMN)
        && attk.damageType == monsterAttack(pmid, indx - 1).damageType) {
        attk.damageType = AD_STUN;
    }
    return attk;
}

/*
 * mattacku: monster attacks you
 *      returns 1 if monster dies (e.g. "yellow light"), 0 otherwise
 *      Note: if you're displaced or invisible the monster might attack the
 *              wrong position...
 *      Assumption: it's attacking you or an empty square; if there's another
 *              monster which it attacks by mistake, the caller had better
 *              take care of it...
 */
int
mattacku(mtmp)
register struct monst *mtmp;
{
    struct Attack mattk;
    int i, j, tmp, sum[NATTK];
    struct permonst *mdat = mtmp->data;
    boolean ranged = (distanceSquaredToYou(mtmp->mx, mtmp->my) > 3);
    /* Is it near you?  Affects your actions */
    boolean range2 = !monnear(mtmp, mtmp->mux, mtmp->muy);
    /* Does it think it's near you?  Affects its actions */
    boolean foundyou = (mtmp->mux == currentX() && mtmp->muy == currentY());
    /* Is it attacking you or your image? */
    boolean youseeit = canseemon(mtmp);
    /* Might be attacking your image around the corner, or
     * invisible, or you might be blind....
     */
    boolean skipnonmagc = FALSE;
    /* Are further physical attack attempts useless? */

    if (!ranged)
        nomul(0);
    if (mtmp->mhp <= 0 || (underwater() && !isSwimmer(pmid4mon(mtmp))))
        return 0;

    /* If swallowed, can only be affected by u.ustuck */
    if (swallowed()) {
        if (mtmp != u.ustuck)
            return 0;
        u.ustuck->mux = currentX();
        u.ustuck->muy = currentY();
        range2 = 0;
        foundyou = 1;
        if (invulnerableWhilePraying())
            return 0; /* stomachs can't hurt you! */

    } else if (u.usteed) {
        if (mtmp == u.usteed)
            /* Your steed won't attack you */
            return 0;
        /* Orcs like to steal and eat horses and the like */
        if (!rn2(isOrc(pmid4mon(mtmp)) ? 2 : 4)
            && distanceSquaredToYou(mtmp->mx, mtmp->my) <= 2) {
            /* Attack your steed instead */
            i = mattackm(mtmp, u.usteed);
            if ((i & MM_AGR_DIED))
                return 1;
            /* make sure steed is still alive and within range */
            if ((i & MM_DEF_DIED) || !u.usteed
                || distanceSquaredToYou(mtmp->mx, mtmp->my) > 2)
                return 0;
            /* Let your steed retaliate */
            return !!(mattackm(u.usteed, mtmp) & MM_DEF_DIED);
        }
    }

    if (lurking() && !range2 && foundyou && !swallowed()) {
        if (!canspotmon(mtmp))
            map_invisible(mtmp->mx, mtmp->my);
        setLurking(FALSE);
        if (isHider(pmid4you()) && currentMonsterNumber() != PM_TRAPPER) {
            /* ceiling hider */
            coord cc; /* maybe we need a unexto() function? */
            struct obj *obj;

            You("fall from the %s!", ceiling(currentX(), currentY()));
            /* take monster off map now so that its location
               is eligible for placing hero; we assume that a
               removed monster remembers its old spot <mx,my> */
            remove_monster(mtmp->mx, mtmp->my);
            if (!canPlaceMonsterNear(&cc, currentX(), currentY(), pmid4you(), 0)
                /* a fish won't voluntarily swap positions
                   when it's in water and hero is over land */
                || (monsterClass(pmid4mon(mtmp)) == S_EEL
                    && is_pool(mtmp->mx, mtmp->my)
                    && !is_pool(currentX(), currentY()))) {
                /* couldn't find any spot for hero; this used to
                   kill off attacker, but now we just give a "miss"
                   message and keep both mtmp and hero at their
                   original positions; hero has become unconcealed
                   so mtmp's next move will be a regular attack */
                place_monster(mtmp, mtmp->mx, mtmp->my); /* put back */
                newsym(currentX(), currentY()); /* luking() was toggled */
                pline("%s draws back as you drop!", Monnam(mtmp));
                return 0;
            }

            /* put mtmp at hero's spot and move hero to <cc.x,.y> */
            newsym(mtmp->mx, mtmp->my); /* finish removal */
            place_monster(mtmp, currentX(), currentY());
            if (mtmp->wormno) {
                worm_move(mtmp);
                /* tail hasn't grown, so if it now occupies <cc.x,.y>
                   then one of its original spots must be free */
                if (m_at(cc.x, cc.y))
                    (void) canPlaceMonsterNear(&cc, currentX(), currentY(), pmid4you(), 0);
            }
            teleds(cc.x, cc.y, TRUE); /* move hero */
            set_apparxy(mtmp);
            newsym(currentX(), currentY());

            if (monsterClass(pmid4you()) != S_PIERCER)
                return 0; /* lurkers don't attack */

            obj = which_armor(mtmp, WORN_HELMET);
            if (obj && is_metallic(obj)) {
                Your("blow glances off %s %s.", s_suffix(mon_nam(mtmp)),
                     helm_simple_name(obj));
            } else {
                if (3 + find_mac(mtmp) <= rnd(20)) {
                    pline("%s is hit by a falling piercer (you)!",
                          Monnam(mtmp));
                    if ((mtmp->mhp -= d(3, 6)) < 1)
                        killed(mtmp);
                } else
                    pline("%s is almost hit by a falling piercer (you)!",
                          Monnam(mtmp));
            }

        } else {
            /* surface hider */
            if (!youseeit) {
                pline("It tries to move where you are hiding.");
            } else {
                /* Ugly kludge for eggs.  The message is phrased so as
                 * to be directed at the monster, not the player,
                 * which makes "laid by you" wrong.  For the
                 * parallelism to work, we can't rephrase it, so we
                 * zap the "laid by you" momentarily instead.
                 */
                struct obj *obj = level.objects[currentX()][currentY()];

                if (obj || currentMonsterNumber() == PM_TRAPPER
                    || (monsterClass(pmid4you()) == S_EEL
                        && is_pool(currentX(), currentY()))) {
                    int save_spe = 0; /* suppress warning */

                    if (obj) {
                        save_spe = obj->spe;
                        if (obj->otyp == EGG) {
                            obj->spe = 0;
			}
                    }
		    javaString youMonsterName = monsterTypeName(pmid4you());
                    if (monsterClass(pmid4you()) == S_EEL
                        || currentMonsterNumber() == PM_TRAPPER) {
                        pline(
                             "Wait, %s!  There's a hidden %s named %s there!",
                              m_monnam(mtmp), youMonsterName.c_str, plname);
		    } else {
                        pline(
                          "Wait, %s!  There's a %s named %s hiding under %s!",
                              m_monnam(mtmp), youMonsterName.c_str, plname,
                              doname(level.objects[currentX()][currentY()]));
		    }
		    releaseJavaString(youMonsterName);
                    if (obj) {
                        obj->spe = save_spe;
		    }
                } else {
                    impossible("hiding under nothing?");
		}
            }
            newsym(currentX(), currentY());
        }
        return 0;
    }

    /* hero might be a mimic, concealed via #monster */
    if (monsterClass(pmid4you()) == S_MIMIC && youmonst.m_ap_type && !range2
        && foundyou && !swallowed()) {
        boolean sticky = monsterSticksInCombat(pmid4you());

        if (!canspotmon(mtmp)) {
            map_invisible(mtmp->mx, mtmp->my);
        }
	if (sticky && !youseeit) {
            pline("It gets stuck on you.");
	} else {
	    javaString youMonsterName = monsterTypeName(pmid4you());
            pline("Wait, %s!  That's a %s named %s!", m_monnam(mtmp),
                  youMonsterName.c_str, plname);
	    releaseJavaString(youMonsterName);
	}
        if (sticky) {
            u.ustuck = mtmp;
	}
        youmonst.m_ap_type = M_AP_NOTHING;
        youmonst.mappearance = 0;
        newsym(currentX(), currentY());
        return 0;
    }

    /* non-mimic hero might be mimicking an object after eating m corpse */
    if (youmonst.m_ap_type == M_AP_OBJECT && !range2 && foundyou
        && !swallowed()) {
        if (!canspotmon(mtmp)) {
            map_invisible(mtmp->mx, mtmp->my);
	}
        if (!youseeit) {
            pline("%s %s!", Something, (likesGold(pmid4mon(mtmp))
                                        && youmonst.mappearance == GOLD_PIECE)
                                           ? "tries to pick you up"
                                           : "disturbs you");
	} else {
	    javaString monsterName = monsterTypeName(currentMonsterNumber());
            pline("Wait, %s!  That %s is really %s named %s!", m_monnam(mtmp),
                  mimic_obj_name(&youmonst), an(monsterName.c_str),
                  plname);
	    releaseJavaString(monsterName);
	}
        if (multi < 0) { /* this should always be the case */
            char buf[BUFSZ];

	    javaString youMonsterName = monsterTypeName(pmid4you());
            Sprintf(buf, "You appear to be %s again.",
                    areYouPolymorphed() ? (const char *) an(youMonsterName.c_str)
                           : (const char *) "yourself");
	    releaseJavaString(youMonsterName);
            unmul(buf); /* immediately stop mimicking */
        }
        return 0;
    }

    /*  Work out the armor class differential   */
    tmp = AC_VALUE(armorClass()) + 10; /* tmp ~= 0 - 20 */
    tmp += mtmp->m_lev;
    if (multi < 0)
        tmp += 4;
    if ((youAreInvisibleToOthers() && !perceivesTheInvisible(pmid4(mdat))) || !mtmp->mcansee)
        tmp -= 2;
    if (mtmp->mtrapped)
        tmp -= 2;
    if (tmp <= 0)
        tmp = 1;

    /* make eels visible the moment they hit/miss us */
    if (monsterClass(pmid4(mdat)) == S_EEL && mtmp->minvis && cansee(mtmp->mx, mtmp->my)) {
        mtmp->minvis = 0;
        newsym(mtmp->mx, mtmp->my);
    }

    /*  Special demon handling code */
    if ((mtmp->cham == NON_PM) && isDemon(pmid4(mdat)) && !range2
        && mtmp->data != &mons[PM_BALROG] && mtmp->data != &mons[PM_SUCCUBUS]
        && mtmp->data != &mons[PM_INCUBUS])
        if (!mtmp->mcan && !rn2(13))
            (void) msummon(mtmp);

    /*  Special lycanthrope handling code */
    if ((mtmp->cham == NON_PM) && isWere(pmid4(mdat)) && !range2) {
        if (isHuman(pmid4(mdat))) {
            if (!rn2(5 - (night() * 2)) && !mtmp->mcan)
                new_were(mtmp);
        } else if (!rn2(30) && !mtmp->mcan)
            new_were(mtmp);
        mdat = mtmp->data;

        if (!rn2(10) && !mtmp->mcan) {
            int numseen, numhelp;
            char buf[BUFSZ], genericwere[BUFSZ];

            Strcpy(genericwere, "creature");
            numhelp = wereCreatureSummonsOtherMonsters(pmid4mon(mtmp), FALSE, &numseen, genericwere);
            if (youseeit) {
                pline("%s summons help!", Monnam(mtmp));
                if (numhelp > 0) {
                    if (numseen == 0)
                        You_feel("hemmed in.");
                } else
                    pline("But none comes.");
            } else {
                const char *from_nowhere;

                if (!youAreDeaf()) {
                    pline("%s %s!", Something, makeplural(growl_sound(mtmp)));
                    from_nowhere = "";
                } else
                    from_nowhere = " from nowhere";
                if (numhelp > 0) {
                    if (numseen < 1)
                        You_feel("hemmed in.");
                    else {
                        if (numseen == 1)
                            Sprintf(buf, "%s appears", an(genericwere));
                        else
                            Sprintf(buf, "%s appear",
                                    makeplural(genericwere));
                        pline("%s%s!", upstart(buf), from_nowhere);
                    }
                } /* else no help came; but you didn't know it tried */
            }
        }
    }

    if (invulnerableWhilePraying()) {
        /* monsters won't attack you */
        if (mtmp == u.ustuck)
            pline("%s loosens its grip slightly.", Monnam(mtmp));
        else if (!range2) {
            if (youseeit || senseMonsters(mtmp))
                pline("%s starts to attack you, but pulls back.",
                      Monnam(mtmp));
            else
                You_feel("%s move nearby.", something);
        }
        return 0;
    }

    /* Unlike defensive stuff, don't let them use item _and_ attack. */
    if (find_offensive(mtmp)) {
        int foo = use_offensive(mtmp);

        if (foo != 0)
            return (foo == 1);
    }

    for (i = 0; i < monsterAttacks(pmid4(mdat)); i++) {
        sum[i] = 0;
        mattk = getMonsterAttack(pmid4(mdat), i, sum);
        if ((swallowed() && mattk.type != AT_ENGL)
            || (skipnonmagc && mattk.type != AT_MAGC))
            continue;

        switch (mattk.type) {
        case AT_CLAW: /* "hand to hand" attacks */
        case AT_KICK:
        case AT_BITE:
        case AT_STNG:
        case AT_TUCH:
        case AT_BUTT:
        case AT_TENT:
            if (!range2 && (!MON_WEP(mtmp) || mtmp->mconf || youCauseConflict()
                            || !touchPetrifies(pmid4you()))) {
                if (foundyou) {
                    if (tmp > (j = rnd(20 + i))) {
                        if (mattk.type != AT_KICK
                            || !isThickSkinned(pmid4you()))
                            sum[i] = hitmu(mtmp, mattk);
                    } else
                        missmu(mtmp, (tmp == j), mattk);
                } else {
                    wildmiss(mtmp, mattk);
                    /* skip any remaining non-spell attacks */
                    skipnonmagc = TRUE;
                }
            }
            break;

        case AT_HUGS: /* automatic if prev two attacks succeed */
            /* Note: if displaced, prev attacks never succeeded */
            if ((!range2 && i >= 2 && sum[i - 1] && sum[i - 2])
                || mtmp == u.ustuck)
                sum[i] = hitmu(mtmp, mattk);
            break;

        case AT_GAZE: /* can affect you either ranged or not */
            /* Medusa gaze already operated through m_respond in
               dochug(); don't gaze more than once per round. */
            if (mdat != &mons[PM_MEDUSA])
                sum[i] = gazemu(mtmp, mattk);
            break;

        case AT_EXPL: /* automatic hit if next to, and aimed at you */
            if (!range2)
                sum[i] = explmu(mtmp, mattk, foundyou);
            break;

        case AT_ENGL:
            if (!range2) {
                if (foundyou) {
                    if (swallowed() || tmp > (j = rnd(20 + i))) {
                        /* Force swallowing monster to be
                         * displayed even when player is
                         * moving away */
                        flush_screen(1);
                        sum[i] = gulpmu(mtmp, mattk);
                    } else {
                        missmu(mtmp, (tmp == j), mattk);
                    }
                } else if (isAnimal(pmid4mon(mtmp))) {
                    pline("%s gulps some air!", Monnam(mtmp));
                } else {
                    if (youseeit)
                        pline("%s lunges forward and recoils!", Monnam(mtmp));
                    else
                        You_hear("a %s nearby.",
                                 isWhirly(pmid4mon(mtmp)) ? "rushing noise"
                                                       : "splat");
                }
            }
            break;
        case AT_BREA:
            if (range2)
                sum[i] = breamu(mtmp, mattk);
            /* Note: breamu takes care of displacement */
            break;
        case AT_SPIT:
            if (range2)
                sum[i] = spitmu(mtmp, mattk);
            /* Note: spitmu takes care of displacement */
            break;
        case AT_WEAP:
            if (range2) {
                if (!areYouOnRogueLevel())
                    thrwmu(mtmp);
            } else {
                int hittmp = 0;

                /* Rare but not impossible.  Normally the monster
                 * wields when 2 spaces away, but it can be
                 * teleported or whatever....
                 */
                if (mtmp->weapon_check == NEED_WEAPON || !MON_WEP(mtmp)) {
                    mtmp->weapon_check = NEED_HTH_WEAPON;
                    /* mon_wield_item resets weapon_check as appropriate */
                    if (mon_wield_item(mtmp) != 0)
                        break;
                }
                if (foundyou) {
                    otmp = MON_WEP(mtmp);
                    if (otmp) {
                        hittmp = hitval(otmp, &youmonst);
                        tmp += hittmp;
                        mswings(mtmp, otmp);
                    }
                    if (tmp > (j = dieroll = rnd(20 + i)))
                        sum[i] = hitmu(mtmp, mattk);
                    else
                        missmu(mtmp, (tmp == j), mattk);
                    /* KMH -- Don't accumulate to-hit bonuses */
                    if (otmp)
                        tmp -= hittmp;
                } else {
                    wildmiss(mtmp, mattk);
                    /* skip any remaining non-spell attacks */
                    skipnonmagc = TRUE;
                }
            }
            break;
        case AT_MAGC:
            if (range2)
                sum[i] = buzzmu(mtmp, mattk);
            else
                sum[i] = castmu(mtmp, mattk, TRUE, foundyou);
            break;

        default: /* no attack */
            break;
        }
        if (context.botl)
            bot();
        /* give player a chance of waking up before dying -kaa */
        if (sum[i] == 1) { /* successful attack */
            if (sleepingSinceMove() && (sleepingSinceMove() < monstermoves) && !rn2(10)) {
                multi = -1;
                nomovemsg = "The combat suddenly awakens you.";
            }
        }
        if (sum[i] == 2)
            return 1; /* attacker dead */
        if (sum[i] == 3)
            break; /* attacker teleported, no more attacks */
        /* sum[i] == 0: unsuccessful attack */
    }
    return 0;
}

STATIC_OVL boolean
diseasemu(mdat)
struct permonst *mdat;
{
    if (youResistSickness()) {
        You_feel("a slight illness.");
        return FALSE;
    } else {
	javaString monsterName = monsterTypeName(pmid4(mdat));
        make_sick(youAreSick() ? yourIntrinsic(SICK) / 3L + 1L : (long) rn1(ACURR(A_CON), 20),
                  monsterName.c_str, TRUE, SICK_NONVOMITABLE);
	releaseJavaString(monsterName);
        return TRUE;
    }
}

/* check whether slippery clothing protects from hug or wrap attack */
STATIC_OVL boolean
u_slip_free(mtmp, mattk)
struct monst *mtmp;
const struct Attack mattk;
{
    struct obj *obj = (uarmc ? uarmc : uarm);

    if (!obj)
        obj = uarmu;
    if (mattk.damageType == AD_DRIN)
        obj = uarmh;

    /* if your cloak/armor is greased, monster slips off; this
       protection might fail (33% chance) when the armor is cursed */
    if (obj && (obj->greased || obj->otyp == OILSKIN_CLOAK)
        && (!obj->cursed || rn2(3))) {
        pline("%s %s your %s %s!", Monnam(mtmp),
              (mattk.damageType == AD_WRAP) ? "slips off of"
                                        : "grabs you, but cannot hold onto",
              obj->greased ? "greased" : "slippery",
              /* avoid "slippery slippery cloak"
                 for undiscovered oilskin cloak */
              (obj->greased || objects[obj->otyp].oc_name_known)
                  ? xname(obj)
                  : cloak_simple_name(obj));

        if (obj->greased && !rn2(2)) {
            pline_The("grease wears off.");
            obj->greased = 0;
            update_inventory();
        }
        return TRUE;
    }
    return FALSE;
}

/* armor that sufficiently covers the body might be able to block magic */
int
magic_negation(mon)
struct monst *mon;
{
    struct obj *o;
    long wearmask;
    int armpro, mc = 0;
    boolean is_you = (mon == &youmonst);
    boolean gotprot = is_you ? (yourExtrinsic(PROTECTION) != 0L)
                             /* high priests have innate protection */
                             : (mon->data == &mons[PM_HIGH_PRIEST]);

    for (o = is_you ? invent : mon->minvent; o; o = o->nobj) {
        /* a_can field is only applicable for armor (which must be worn) */
        if ((o->owornmask & W_ARMOR) != 0L) {
            armpro = objects[o->otyp].a_can;
            if (armpro > mc)
                mc = armpro;
        }
        /* if we've already confirmed Protection, skip additional checks */
        if (is_you || gotprot)
            continue;

        /* omit W_SWAPWEP+W_QUIVER; W_ART+W_ARTI handled by protects() */
        wearmask = W_ARMOR | W_ACCESSORY;
        if (o->oclass == WEAPON_CLASS || is_weptool(o))
            wearmask |= W_WEP;
        if (protects(o, ((o->owornmask & wearmask) != 0L) ? TRUE : FALSE))
            gotprot = TRUE;
    }

    if (gotprot) {
        /* extrinsic Protection increases mc by 1 */
        if (mc < 3)
            mc += 1;
    } else if (mc < 1) {
        /* intrinsic Protection is weaker (play balance; obtaining divine
           protection is too easy); it confers minimum mc 1 instead of 0 */
        if ((is_you && ((yourIntrinsic(PROTECTION) && blessings()) || armorBonusFromProtectionSpell()))
            /* aligned priests and angels have innate intrinsic Protection */
            || (mon->data == &mons[PM_ALIGNED_PRIEST] || isMinion(pmid4mon(mon))))
            mc = 1;
    }
    return mc;
}

/*
 * hitmu: monster hits you
 *        returns 2 if monster dies (e.g. "yellow light"), 1 otherwise
 *        3 if the monster lives but teleported/paralyzed, so it can't keep
 *             attacking you
 */
STATIC_OVL int
hitmu(mtmp, mattk)
register struct monst *mtmp;
register const struct Attack mattk;
{
    register struct permonst *mdat = mtmp->data;
    register int uncancelled, ptmp;
    int dmg, armpro, permdmg;
    char buf[BUFSZ];
    int oldupmid = pmid4you();
    int res;

    if (!canspotmon(mtmp))
        map_invisible(mtmp->mx, mtmp->my);

    /*  If the monster is undetected & hits you, you should know where
     *  the attack came from.
     */
    if (mtmp->mundetected && (hidesUnderStuff(pmid4(mdat)) || monsterClass(pmid4(mdat)) == S_EEL)) {
        mtmp->mundetected = 0;
        if (!(youCannotSee() ? youHaveTelepathyWhenBlind() : youHaveTelepathyWhenNotBlind())) {
            struct obj *obj;
            const char *what;

            if ((obj = level.objects[mtmp->mx][mtmp->my]) != 0) {
                if (youCannotSee() && !obj->dknown)
                    what = something;
                else if (is_pool(mtmp->mx, mtmp->my) && !underwater())
                    what = "the water";
                else
                    what = doname(obj);

                pline("%s was hidden under %s!", Amonnam(mtmp), what);
            }
            newsym(mtmp->mx, mtmp->my);
        }
    }

    /*  First determine the base damage done */
    dmg = d(mattk.dice, mattk.diceSides);
    if ((isUndead(pmid4(mdat)) || is_vampshifter(mtmp)) && midnight())
        dmg += d(mattk.dice, mattk.diceSides); /* extra damage */

    /*  Next a cancellation factor.
     *  Use uncancelled when cancellation factor takes into account certain
     *  armor's special magic protection.  Otherwise just use !mtmp->mcan.
     */
    armpro = magic_negation(&youmonst);
    uncancelled = !mtmp->mcan && (rn2(10) >= 3 * armpro);

    permdmg = 0;
    /*  Now, adjust damages via resistances or specific attacks */
    switch (mattk.damageType) {
    case AD_PHYS:
        if (mattk.type == AT_HUGS && !monsterSticksInCombat(pmid4you())) {
            if (!u.ustuck && rn2(2)) {
                if (u_slip_free(mtmp, mattk)) {
                    dmg = 0;
                } else {
                    u.ustuck = mtmp;
                    pline("%s grabs you!", Monnam(mtmp));
                }
            } else if (u.ustuck == mtmp) {
                exercise(A_STR, FALSE);
                You("are being %s.", (mtmp->data == &mons[PM_ROPE_GOLEM])
                                         ? "choked"
                                         : "crushed");
            }
        } else { /* hand to hand weapon */
            if (mattk.type == AT_WEAP && otmp) {
                int tmp;

                if (otmp->otyp == CORPSE
                    && touchPetrifies(otmp->corpsenm)) {
                    dmg = 1;
		    javaString corpseName = monsterTypeName(otmp->corpsenm);
                    pline("%s hits you with the %s corpse.", Monnam(mtmp),
                          corpseName.c_str);
		    releaseJavaString(corpseName);
                    if (!youAreTurningToStone())
                        goto do_stone;
                }
                dmg += dmgval(otmp, &youmonst);
                if (dmg <= 0)
                    dmg = 1;
                if (!(otmp->oartifact
                      && artifact_hit(mtmp, &youmonst, otmp, &dmg, dieroll)))
                    hitmsg(mtmp, mattk);
                if (!dmg)
                    break;
                if (objects[otmp->otyp].oc_material == SILVER
                    && youHateSilver()) {
                    pline_The("silver sears your flesh!");
                    exercise(A_CON, FALSE);
                }
                /* this redundancy necessary because you have
                   to take the damage _before_ being cloned;
                   need to have at least 2 hp left to split */
                tmp = dmg;
                if (armorClass() < 0)
                    tmp -= rnd(-1 * armorClass());
                if (tmp < 1)
                    tmp = 1;
                if (currentHitPointsAsMonster() - tmp > 1 && objects[otmp->otyp].oc_material == IRON
                    && (currentMonsterNumber() == PM_BLACK_PUDDING
                        || currentMonsterNumber() == PM_BROWN_PUDDING)) {
                    if (tmp > 1)
                        exercise(A_STR, FALSE);
                    /* inflict damage now; we know it can't be fatal */
                    decreaseCurrentHitPointsAsMonster(tmp);
                    context.botl = 1;
                    dmg = 0; /* don't inflict more damage below */
                    if (cloneu())
                        You("divide as %s hits you!", mon_nam(mtmp));
                }
                rustm(&youmonst, otmp);
            } else if (mattk.type != AT_TUCH || dmg != 0
                       || mtmp != u.ustuck)
                hitmsg(mtmp, mattk);
        }
        break;
    case AD_DISE:
        hitmsg(mtmp, mattk);
        if (!diseasemu(mdat))
            dmg = 0;
        break;
    case AD_FIRE:
        hitmsg(mtmp, mattk);
        if (uncancelled) {
            pline("You're %s!", fireDescription(pmid4you(), mattk));
            if ((pmid4you() == PM_STRAW_GOLEM)
                || (pmid4you() == PM_PAPER_GOLEM)) {
                You("roast!");
                /* KMH -- this is okay with unchanging */
                rehumanize();
                break;
            } else if (youResistFire()) {
                pline_The("fire doesn't feel hot!");
                dmg = 0;
            }
            if ((int) mtmp->m_lev > rn2(20))
                destroy_item(SCROLL_CLASS, AD_FIRE);
            if ((int) mtmp->m_lev > rn2(20))
                destroy_item(POTION_CLASS, AD_FIRE);
            if ((int) mtmp->m_lev > rn2(25))
                destroy_item(SPBOOK_CLASS, AD_FIRE);
            burn_away_slime();
        } else
            dmg = 0;
        break;
    case AD_COLD:
        hitmsg(mtmp, mattk);
        if (uncancelled) {
            pline("You're covered in frost!");
            if (youResistCold()) {
                pline_The("frost doesn't seem cold!");
                dmg = 0;
            }
            if ((int) mtmp->m_lev > rn2(20))
                destroy_item(POTION_CLASS, AD_COLD);
        } else
            dmg = 0;
        break;
    case AD_ELEC:
        hitmsg(mtmp, mattk);
        if (uncancelled) {
            You("get zapped!");
            if (youResistShock()) {
                pline_The("zap doesn't shock you!");
                dmg = 0;
            }
            if ((int) mtmp->m_lev > rn2(20))
                destroy_item(WAND_CLASS, AD_ELEC);
            if ((int) mtmp->m_lev > rn2(20))
                destroy_item(RING_CLASS, AD_ELEC);
        } else
            dmg = 0;
        break;
    case AD_SLEE:
        hitmsg(mtmp, mattk);
        if (uncancelled && multi >= 0 && !rn2(5)) {
            if (youResistSleep())
                break;
            fall_asleep(-rnd(10), TRUE);
            if (youCannotSee())
                You("are put to sleep!");
            else
                You("are put to sleep by %s!", mon_nam(mtmp));
        }
        break;
    case AD_BLND:
        if (can_blnd(mtmp, &youmonst, mattk.type, (struct obj *) 0)) {
            if (youCanSee())
                pline("%s blinds you!", Monnam(mtmp));
            make_blinded(yourIntrinsic(BLINDED) + (long) dmg, FALSE);
            if (!yourIntrinsic(BLINDED))
                Your1(vision_clears);
        }
        dmg = 0;
        break;
    case AD_DRST:
        ptmp = A_STR;
        goto dopois;
    case AD_DRDX:
        ptmp = A_DEX;
        goto dopois;
    case AD_DRCO:
        ptmp = A_CON;
    dopois:
        hitmsg(mtmp, mattk);
        if (uncancelled && !rn2(8)) {
            Sprintf(buf, "%s %s", s_suffix(Monnam(mtmp)),
                    mpoisons_subj(mtmp, mattk));
	    javaString monsterName = monsterTypeName(pmid4(mdat));
            poisoned(buf, ptmp, monsterName.c_str, 30, FALSE);
	    releaseJavaString(monsterName);
        }
        break;
    case AD_DRIN:
        hitmsg(mtmp, mattk);
        if (defends(AD_DRIN, uwep) || !hasAHead(pmid4you())) {
            You("don't seem harmed.");
            /* Not clear what to do for green slimes */
            break;
        }
        if (u_slip_free(mtmp, mattk))
            break;

        if (uarmh && rn2(8)) {
            /* not body_part(HEAD) */
            Your("%s blocks the attack to your head.",
                 helm_simple_name(uarmh));
            break;
        }
        if (youTakeHalfDamageFromPhysicalAttacks())
            dmg = (dmg + 1) / 2;
        mdamageu(mtmp, dmg);

        if (!uarmh || uarmh->otyp != DUNCE_CAP) {
            /* eat_brains() will miss if target isMindless() (won't
               happen here; hero is considered to retain his mind
               regardless of current shape) or is noncorporeal
               (can't happen here; no one can poly into a ghost
               or shade) so this check for missing is academic */
            if (eat_brains(mtmp, &youmonst, TRUE, (int *) 0) == MM_MISS)
                break;
        }
        /* adjattrib gives dunce cap message when appropriate */
        (void) adjattrib(A_INT, -rnd(2), FALSE);
        forget_levels(25);  /* lose memory of 25% of levels */
        forget_objects(25); /* lose memory of 25% of objects */
        break;
    case AD_PLYS:
        hitmsg(mtmp, mattk);
        if (uncancelled && multi >= 0 && !rn2(3)) {
            if (youHaveFreeAction()) {
                You("momentarily stiffen.");
            } else {
                if (youCannotSee())
                    You("are frozen!");
                else
                    You("are frozen by %s!", mon_nam(mtmp));
                nomovemsg = You_can_move_again;
                nomul(-rnd(10));
                multi_reason = "paralyzed by a monster";
                exercise(A_DEX, FALSE);
            }
        }
        break;
    case AD_DRLI:
        hitmsg(mtmp, mattk);
        if (uncancelled && !rn2(3) && !youResistDraining()) {
            losexp("life drainage");
        }
        break;
    case AD_LEGS: {
        register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
        const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";

        /* This case is too obvious to ignore, but Nethack is not in
         * general very good at considering height--most short monsters
         * still _can_ attack you when you're flying or mounted.
         * [FIXME: why can't a flying attacker overcome this?]
         */
        if (u.usteed || youAreLevitating() || youAreFlying()) {
            pline("%s tries to reach your %s %s!", Monnam(mtmp), sidestr,
                  body_part(LEG));
            dmg = 0;
        } else if (mtmp->mcan) {
            pline("%s nuzzles against your %s %s!", Monnam(mtmp), sidestr,
                  body_part(LEG));
            dmg = 0;
        } else {
            if (uarmf) {
                if (rn2(2) && (uarmf->otyp == LOW_BOOTS
                               || uarmf->otyp == IRON_SHOES))
                    pline("%s pricks the exposed part of your %s %s!",
                          Monnam(mtmp), sidestr, body_part(LEG));
                else if (!rn2(5))
                    pline("%s pricks through your %s boot!", Monnam(mtmp),
                          sidestr);
                else {
                    pline("%s scratches your %s boot!", Monnam(mtmp),
                          sidestr);
                    dmg = 0;
                    break;
                }
            } else
                pline("%s pricks your %s %s!", Monnam(mtmp), sidestr,
                      body_part(LEG));
            set_wounded_legs(side, rnd(60 - ACURR(A_DEX)));
            exercise(A_STR, FALSE);
            exercise(A_DEX, FALSE);
        }
        break;
    }
    case AD_STON: /* cockatrice */
        hitmsg(mtmp, mattk);
        if (!rn2(3)) {
            if (mtmp->mcan) {
                if (!youAreDeaf())
                    You_hear("a cough from %s!", mon_nam(mtmp));
            } else {
                if (!youAreDeaf())
                    You_hear("%s hissing!", s_suffix(mon_nam(mtmp)));
                if (!rn2(10)
                    || (flags.moonphase == NEW_MOON && !have_lizard())) {
                do_stone:
                    if (!youAreTurningToStone() && !youResistStoning()
                        && !(monsterPolymorphsWhenStoned(pmid4you())
                             && polymon(PM_STONE_GOLEM))) {
                        javaString kname = monsterTypeName(pmid4mon(mtmp));

                        if (monsterGenerationMask(pmid4mon(mtmp)) & G_UNIQ) {
                            if (!typeIsProperName(pmid4mon(mtmp))) {
                                make_stoned(5L, (char *) 0, KILLED_BY, the(kname.c_str));
			    } else {
                                make_stoned(5L, (char *) 0, KILLED_BY, kname.c_str);
			    }
                        } else {
                            make_stoned(5L, (char *) 0, KILLED_BY_AN, kname.c_str);
			}
			releaseJavaString(kname);
                        return 1;
                        /* done_in_by(mtmp, STONING); */
                    }
                }
            }
        }
        break;
    case AD_STCK:
        hitmsg(mtmp, mattk);
        if (uncancelled && !u.ustuck && !monsterSticksInCombat(pmid4you()))
            u.ustuck = mtmp;
        break;
    case AD_WRAP:
        if ((!mtmp->mcan || u.ustuck == mtmp) && !monsterSticksInCombat(pmid4you())) {
            if (!u.ustuck && !rn2(10)) {
                if (u_slip_free(mtmp, mattk)) {
                    dmg = 0;
                } else {
                    pline("%s swings itself around you!", Monnam(mtmp));
                    u.ustuck = mtmp;
                }
            } else if (u.ustuck == mtmp) {
                if (is_pool(mtmp->mx, mtmp->my) && !youCanSwim() && !youAreAmphibious()) {
                    boolean moat = (levl[mtmp->mx][mtmp->my].typ != POOL)
                                   && (levl[mtmp->mx][mtmp->my].typ != WATER)
                                   && !areYouOnMedusaLevel()
                                   && !areYouOnWaterLevel();

                    pline("%s drowns you...", Monnam(mtmp));
                    killer.format = KILLED_BY_AN;
		    javaString monsterName = monsterTypeName(pmid4mon(mtmp));
                    Sprintf(killer.name, "%s by %s",
                            moat ? "moat" : "pool of water",
                            an(monsterName.c_str));
		    releaseJavaString(monsterName);
                    done(DROWNING);
                } else if (mattk.type == AT_HUGS)
                    You("are being crushed.");
            } else {
                dmg = 0;
                if (flags.verbose)
                    pline("%s brushes against your %s.", Monnam(mtmp),
                          body_part(LEG));
            }
        } else
            dmg = 0;
        break;
    case AD_WERE:
        hitmsg(mtmp, mattk);
        if (uncancelled && !rn2(4) && lycanthropeType() == NON_PM
            && !youHaveProtectionFromShapeChangers() && !defends(AD_WERE, uwep)) {
            You_feel("feverish.");
            exercise(A_CON, FALSE);
            setLycanthropeType(pmid4(mdat));
            retouch_equipment(2);
        }
        break;
    case AD_SGLD:
        hitmsg(mtmp, mattk);
        if (monsterClass(pmid4you()) == monsterClass(pmid4(mdat)))
            break;
        if (!mtmp->mcan)
            stealgold(mtmp);
        break;

    case AD_SSEX:
        if (SYSOPT_SEDUCE) {
            if (could_seduce(mtmp, &youmonst, mattk) == 1 && !mtmp->mcan)
                if (doseduce(mtmp))
                    return 3;
            break;
        }
    /* else FALLTHRU */
    case AD_SITM: /* for now these are the same */
    case AD_SEDU:
        if (isAnimal(pmid4mon(mtmp))) {
            hitmsg(mtmp, mattk);
            if (mtmp->mcan)
                break;
            /* Continue below */
        } else if (monsterTypeCanCauseDamageType(pmid4you(), AD_SEDU)
                   || (SYSOPT_SEDUCE && monsterTypeCanCauseDamageType(pmid4you(), AD_SSEX))) {
            pline("%s %s.", Monnam(mtmp),
                  mtmp->minvent
                      ? "brags about the goods some dungeon explorer provided"
                  : "makes some remarks about how difficult theft is lately");
            if (!tele_restrict(mtmp))
                (void) rloc(mtmp, TRUE);
            return 3;
        } else if (mtmp->mcan) {
            if (youCanSee()) {
                pline("%s tries to %s you, but you seem %s.",
                      Adjmonnam(mtmp, "plain"),
                      flags.female ? "charm" : "seduce",
                      flags.female ? "unaffected" : "uninterested");
            }
            if (rn2(3)) {
                if (!tele_restrict(mtmp))
                    (void) rloc(mtmp, TRUE);
                return 3;
            }
            break;
        }
        buf[0] = '\0';
        switch (steal(mtmp, buf)) {
        case -1:
            return 2;
        case 0:
            break;
        default:
            if (!isAnimal(pmid4mon(mtmp)) && !tele_restrict(mtmp))
                (void) rloc(mtmp, TRUE);
            if (isAnimal(pmid4mon(mtmp)) && *buf) {
                if (canseemon(mtmp))
                    pline("%s tries to %s away with %s.", Monnam(mtmp),
                          locomotionVerb(pmid4mon(mtmp), "run"), buf);
            }
            monflee(mtmp, 0, FALSE, FALSE);
            return 3;
        }
        break;

    case AD_SAMU:
        hitmsg(mtmp, mattk);
        /* when the Wiz hits, 1/20 steals the amulet */
        if (haveSpecialItem(SPECIAL_ITEM_AMULET) ||
            haveSpecialItem(SPECIAL_ITEM_BELL)   ||
            haveSpecialItem(SPECIAL_ITEM_BOOK)   ||
            haveSpecialItem(SPECIAL_ITEM_CANDELABRUM) ||
            haveSpecialItem(SPECIAL_ITEM_QUEST_ARTIFACT)) {
            if (!rn2(20))
                stealamulet(mtmp);
	}
        break;

    case AD_TLPT:
        hitmsg(mtmp, mattk);
        if (uncancelled) {
            if (flags.verbose)
                Your("position suddenly seems very uncertain!");
            tele();
        }
        break;
    case AD_RUST:
        hitmsg(mtmp, mattk);
        if (mtmp->mcan)
            break;
        if (currentMonsterNumber() == PM_IRON_GOLEM) {
            You("rust!");
            /* KMH -- this is okay with unchanging */
            rehumanize();
            break;
        }
        erode_armor(&youmonst, ERODE_RUST);
        break;
    case AD_CORR:
        hitmsg(mtmp, mattk);
        if (mtmp->mcan)
            break;
        erode_armor(&youmonst, ERODE_CORRODE);
        break;
    case AD_DCAY:
        hitmsg(mtmp, mattk);
        if (mtmp->mcan)
            break;
        if (currentMonsterNumber() == PM_WOOD_GOLEM || currentMonsterNumber() == PM_LEATHER_GOLEM) {
            You("rot!");
            /* KMH -- this is okay with unchanging */
            rehumanize();
            break;
        }
        erode_armor(&youmonst, ERODE_ROT);
        break;
    case AD_HEAL:
        /* a cancelled nurse is just an ordinary monster,
         * nurses don't heal those that cause petrification */
        if (mtmp->mcan || (areYouPolymorphed() && touchPetrifies(pmid4you()))) {
            hitmsg(mtmp, mattk);
            break;
        }
        if (!uwep && !uarmu && !uarm && !uarmh && !uarms && !uarmg && !uarmc
            && !uarmf) {
            boolean goaway = FALSE;
            pline("%s hits!  (I hope you don't mind.)", Monnam(mtmp));
            if (areYouPolymorphed()) {
                increaseCurrentHitPointsAsMonster(rnd(7));
                if (!rn2(7)) {
                    /* no upper limit necessary; effect is temporary */
                    increaseMaximumHitPointsAsMonster(1);
                    if (!rn2(13))
                        goaway = TRUE;
                }
                if (currentHitPointsAsMonster() > maximumHitPointsAsMonster())
                    setCurrentHitPointsAsMonster(maximumHitPointsAsMonster());
            } else {
                increaseCurrentHitPoints(rnd(7));
                if (!rn2(7)) {
                    /* hard upper limit via nurse care: 25 * ulevel */
                    if (maximumHitPoints() < 5 * currentExperienceLevel() + d(2 * currentExperienceLevel(), 10))
                        increaseMaximumHitPoints(1);
                    if (!rn2(13))
                        goaway = TRUE;
                }
                if (currentHitPoints() > maximumHitPoints())
                    setCurrentHitPoints(maximumHitPoints());
            }
            if (!rn2(3))
                exercise(A_STR, TRUE);
            if (!rn2(3))
                exercise(A_CON, TRUE);
            if (youAreSick())
                make_sick(0L, (char *) 0, FALSE, SICK_ALL);
            context.botl = 1;
            if (goaway) {
                mongone(mtmp);
                return 2;
            } else if (!rn2(33)) {
                if (!tele_restrict(mtmp))
                    (void) rloc(mtmp, TRUE);
                monflee(mtmp, d(3, 6), TRUE, FALSE);
                return 3;
            }
            dmg = 0;
        } else {
            if (Role_if(PM_HEALER)) {
                if (!youAreDeaf() && !(moves % 5))
                    verbalize("Doc, I can't help you unless you cooperate.");
                dmg = 0;
            } else
                hitmsg(mtmp, mattk);
        }
        break;
    case AD_CURS:
        hitmsg(mtmp, mattk);
        if (!night() && mdat == &mons[PM_GREMLIN])
            break;
        if (!mtmp->mcan && !rn2(10)) {
            if (!youAreDeaf()) {
                if (youCannotSee())
                    You_hear("laughter.");
                else
                    pline("%s chuckles.", Monnam(mtmp));
            }
            if (currentMonsterNumber() == PM_CLAY_GOLEM) {
                pline("Some writing vanishes from your head!");
                /* KMH -- this is okay with unchanging */
                rehumanize();
                break;
            }
            attrcurse();
        }
        break;
    case AD_STUN:
        hitmsg(mtmp, mattk);
        if (!mtmp->mcan && !rn2(4)) {
            make_stunned(yourIntrinsicTimeout(STUNNED) + (long) dmg, TRUE);
            dmg /= 2;
        }
        break;
    case AD_ACID:
        hitmsg(mtmp, mattk);
        if (!mtmp->mcan && !rn2(3))
            if (youResistAcid()) {
                pline("You're covered in acid, but it seems harmless.");
                dmg = 0;
            } else {
                pline("You're covered in acid!  It burns!");
                exercise(A_STR, FALSE);
            }
        else
            dmg = 0;
        break;
    case AD_SLOW:
        hitmsg(mtmp, mattk);
        if (uncancelled && yourIntrinsic(FAST) && !defends(AD_SLOW, uwep) && !rn2(4))
            u_slow_down();
        break;
    case AD_DREN:
        hitmsg(mtmp, mattk);
        if (uncancelled && !rn2(4))
            drain_en(dmg);
        dmg = 0;
        break;
    case AD_CONF:
        hitmsg(mtmp, mattk);
        if (!mtmp->mcan && !rn2(4) && !mtmp->mspec_used) {
            mtmp->mspec_used = mtmp->mspec_used + (dmg + rn2(6));
            if (youAreConfused())
                You("are getting even more confused.");
            else
                You("are getting confused.");
            make_confused(yourIntrinsic(CONFUSION) + dmg, FALSE);
        }
        dmg = 0;
        break;
    case AD_DETH:
        pline("%s reaches out with its deadly touch.", Monnam(mtmp));
        if (isUndead(pmid4you())) {
            /* Still does normal damage */
            pline("Was that the touch of death?");
            break;
        }
        switch (rn2(20)) {
        case 19:
        case 18:
        case 17:
            if (!youResistMagic()) {
                killer.format = KILLED_BY_AN;
                Strcpy(killer.name, "touch of death");
                done(DIED);
                dmg = 0;
                break;
            }    /* else FALLTHRU */
        default: /* case 16: ... case 5: */
            You_feel("your life force draining away...");
            permdmg = 1; /* actual damage done below */
            break;
        case 4:
        case 3:
        case 2:
        case 1:
        case 0:
            if (youResistMagic())
                shieldeff(currentX(), currentY());
            pline("Lucky for you, it didn't work!");
            dmg = 0;
            break;
        }
        break;
    case AD_PEST:
        pline("%s reaches out, and you feel fever and chills.", Monnam(mtmp));
        (void) diseasemu(mdat); /* plus the normal damage */
        break;
    case AD_FAMN:
        pline("%s reaches out, and your body shrivels.", Monnam(mtmp));
        exercise(A_CON, FALSE);
        if (!is_fainted())
            morehungry(rn1(40, 40));
        /* plus the normal damage */
        break;
    case AD_SLIM:
        hitmsg(mtmp, mattk);
        if (!uncancelled)
            break;
        if (isFlaming(pmid4you())) {
            pline_The("slime burns away!");
            dmg = 0;
        } else if (youAreUnchanging() || isNoncorporeal(pmid4you())
                   || (pmid4you() == PM_GREEN_SLIME)) {
            You("are unaffected.");
            dmg = 0;
        } else if (!youAreTurningToSlime()) {
            You("don't feel very well.");
            make_slimed(10L, (char *) 0);
	    javaString monsterName = monsterTypeName(pmid4mon(mtmp));
            delayed_killer(SLIMED, KILLED_BY_AN, monsterName.c_str);
	    releaseJavaString(monsterName);
        } else
            pline("Yuck!");
        break;
    case AD_ENCH: /* KMH -- remove enchantment (disenchanter) */
        hitmsg(mtmp, mattk);
        /* uncancelled is sufficient enough; please
           don't make this attack less frequent */
        if (uncancelled) {
            struct obj *obj = some_armor(&youmonst);

            if (drain_item(obj)) {
                pline("%s less effective.", Yobjnam2(obj, "seem"));
            }
        }
        break;
    default:
        dmg = 0;
        break;
    }
    if (currentHitPoints() < 1)
        done_in_by(mtmp, DIED);

    /*  Negative armor class reduces damage done instead of fully protecting
     *  against hits.
     */
    if (dmg && armorClass() < 0) {
        dmg -= rnd(-1 * armorClass());
        if (dmg < 1)
            dmg = 1;
    }

    if (dmg) {
        if (youTakeHalfDamageFromPhysicalAttacks()
            /* Mitre of Holiness */
            || (Role_if(PM_PRIEST) && uarmh && is_quest_artifact(uarmh)
                && (isUndead(pmid4mon(mtmp)) || isDemon(pmid4mon(mtmp))
                    || is_vampshifter(mtmp))))
            dmg = (dmg + 1) / 2;

        if (permdmg) { /* Death's life force drain */
            int lowerlimit;
            /*
             * Apply some of the damage to permanent hit points:
             *  polymorphed         100% against poly'd hpmax
             *  hpmax > 25*lvl      100% against normal hpmax
             *  hpmax > 10*lvl  50..100%
             *  hpmax >  5*lvl  25..75%
             *  otherwise        0..50%
             * Never reduces hpmax below 1 hit point per level.
             */
            permdmg = rn2(dmg / 2 + 1);
            if (areYouPolymorphed() || maximumHitPoints() > 25 * currentExperienceLevel())
                permdmg = dmg;
            else if (maximumHitPoints() > 10 * currentExperienceLevel())
                permdmg += dmg / 2;
            else if (maximumHitPoints() > 5 * currentExperienceLevel())
                permdmg += dmg / 4;

            if (areYouPolymorphed()) {
                /* [can't use youmonst.m_lev] */
                lowerlimit = min(monsterLevel(pmid4you()), currentExperienceLevel());
                if (maximumHitPointsAsMonster() - permdmg > lowerlimit) {
                    decreaseMaximumHitPointsAsMonster(permdmg);
                } else if (maximumHitPointsAsMonster() > lowerlimit) {
                    setMaximumHitPointsAsMonster(lowerlimit);
                }
            } else {
                lowerlimit = currentExperienceLevel();
                if (maximumHitPoints() - permdmg > lowerlimit)
                    decreaseMaximumHitPoints(permdmg);
                else if (maximumHitPoints() > lowerlimit)
                    setMaximumHitPoints(lowerlimit);
            } 
            /* else unlikely...
             * already at or below minimum threshold; do nothing */
            context.botl = 1;
        }

        mdamageu(mtmp, dmg);
    }

    if (dmg)
        res = passiveum(oldupmid, mtmp, mattk);
    else
        res = 1;
    stop_occupation();
    return res;
}

/* An interface for use when taking a blindfold off, for example,
 * to see if an engulfing attack should immediately take affect, like
 * a passive attack. TRUE if engulfing blindness occurred */
boolean
gulp_blnd_check()
{
    if (!youAreTemporarilyBlinded() && swallowed()) {
        struct Attack mattk = monsterAttackWithDamageType(pmid4mon(u.ustuck), AT_ENGL, AD_BLND);

	if (validAttack(mattk) && can_blnd(u.ustuck, &youmonst, mattk.type, (struct obj *) 0)) {
            increaseTimeSinceBeingSwallowed(1); /* compensate for gulpmu change */

            (void) gulpmu(u.ustuck, mattk);
            return TRUE;
        }
    }
    return FALSE;
}

/* monster swallows you, or damage if swallowed() */
STATIC_OVL int
gulpmu(mtmp, mattk)
register struct monst *mtmp;
register const struct Attack mattk;
{
    struct trap *t = t_at(currentX(), currentY());
    int tmp = d(mattk.dice, mattk.diceSides);
    int tim_tmp;
    register struct obj *otmp2;
    int i;
    boolean physical_damage = FALSE;

    if (!swallowed()) { /* swallows you */
        int omx = mtmp->mx, omy = mtmp->my;

        if (!engulf_target(mtmp, &youmonst))
            return 0;
        if ((t && ((t->ttyp == PIT) || (t->ttyp == SPIKED_PIT)))
            && sobj_at(BOULDER, currentX(), currentY()))
            return 0;

        if (youAreBeingPunished())
            unplacebc(); /* ball&chain go away */
        remove_monster(omx, omy);
        mtmp->mtrapped = 0; /* no longer on old trap */
        place_monster(mtmp, currentX(), currentY());
        u.ustuck = mtmp;
        newsym(mtmp->mx, mtmp->my);
        if (isAnimal(pmid4mon(mtmp)) && u.usteed) {
            char buf[BUFSZ];
            /* Too many quirks presently if hero and steed
             * are swallowed. Pretend purple worms don't
             * like horses for now :-)
             */
            Strcpy(buf, mon_nam(u.usteed));
            pline("%s lunges forward and plucks you off %s!", Monnam(mtmp),
                  buf);
            dismount_steed(DISMOUNT_ENGULFED);
        } else
            pline("%s engulfs you!", Monnam(mtmp));
        stop_occupation();
        reset_occupations(); /* behave as if you had moved */

        if (currentlyTrapped()) {
            You("are released from the %s!",
                currentTrapType() == TT_WEB ? "web" : "trap");
            setCurrentTrapTimeout(0);
        }

        i = number_leashed();
        if (i > 0) {
            const char *s = (i > 1) ? "leashes" : "leash";
            pline_The("%s %s loose.", s, vtense(s, "snap"));
            unleash_all();
        }

        if (touchPetrifies(pmid4you()) && !resists_ston(mtmp)) {
            /* put the attacker back where it started;
               the resulting statue will end up there */
            remove_monster(mtmp->mx, mtmp->my); /* currentX(),currentY() */
            place_monster(mtmp, omx, omy);
            minstapetrify(mtmp, TRUE);
            /* normally unstuck() would do this, but we're not
               fully swallowed yet so that won't work here */
            if (youAreBeingPunished())
                placebc();
            u.ustuck = 0;
            return (mtmp->mhp > 0) ? 0 : 2;
        }

        display_nhwindow(WIN_MESSAGE, FALSE);
        vision_recalc(2); /* hero can't see anything */
        setSwallowed(TRUE);
        /* for digestion, shorter time is more dangerous;
           for other swallowings, longer time means more
           chances for the swallower to attack */
        if (mattk.damageType == AD_DGST) {
            tim_tmp = 25 - (int) mtmp->m_lev;
            if (tim_tmp > 0)
                tim_tmp = rnd(tim_tmp) / 2;
            else if (tim_tmp < 0)
                tim_tmp = -(rnd(-tim_tmp) / 2);
            /* having good armor & high constitution makes
               it take longer for you to be digested, but
               you'll end up trapped inside for longer too */
            tim_tmp += (-1 * armorClass()) + 10 + (ACURR(A_CON) / 3 - 1);
        } else {
            /* higher level attacker takes longer to eject hero */
            tim_tmp = rnd((int) mtmp->m_lev + 10 / 2);
        }
        /* timeSinceBeingSwallowed always set > 1 */
        setTimeSinceBeingSwallowed((unsigned) ((tim_tmp < 2) ? 2 : tim_tmp));
        showHeroBeingSwallowed(1);
        for (otmp2 = invent; otmp2; otmp2 = otmp2->nobj)
            (void) snuff_lit(otmp2);
    }

    if (mtmp != u.ustuck)
        return 0;
    if (timeSinceBeingSwallowed() > 0)
        decreaseTimeSinceBeingSwallowed(1);

    switch (mattk.damageType) {
    case AD_DGST:
        physical_damage = TRUE;
        if (youHaveSlowDigestion()) {
            /* Messages are handled below */
            setTimeSinceBeingSwallowed(0);
            tmp = 0;
        } else if (timeSinceBeingSwallowed() == 0) {
            pline("%s totally digests you!", Monnam(mtmp));
            tmp = currentHitPoints();
            if (youTakeHalfDamageFromPhysicalAttacks())
                tmp *= 2; /* sorry */
        } else {
            pline("%s%s digests you!", Monnam(mtmp),
                  (timeSinceBeingSwallowed() == 2) ? " thoroughly"
                                    : (timeSinceBeingSwallowed() == 1) ? " utterly" : "");
            exercise(A_STR, FALSE);
        }
        break;
    case AD_PHYS:
        physical_damage = TRUE;
        if (mtmp->data == &mons[PM_FOG_CLOUD]) {
            You("are laden with moisture and %s",
                isFlaming(pmid4you())
                    ? "are smoldering out!"
                    : youNeedNotBreathe() ? "find it mildly uncomfortable."
                                 : isAmphibious(pmid4you())
                                       ? "feel comforted."
                                       : "can barely breathe!");
            /* NB: Amphibious includes Breathless */
            if (youAreAmphibious() && !isFlaming(pmid4you()))
                tmp = 0;
        } else {
            You("are pummeled with debris!");
            exercise(A_STR, FALSE);
        }
        break;
    case AD_ACID:
        if (youResistAcid()) {
            You("are covered with a seemingly harmless goo.");
            tmp = 0;
        } else {
            if (youAreHallucinating())
                pline("Ouch!  You've been slimed!");
            else
                You("are covered in slime!  It burns!");
            exercise(A_STR, FALSE);
        }
        break;
    case AD_BLND:
        if (can_blnd(mtmp, &youmonst, mattk.type, (struct obj *) 0)) {
            if (youCanSee()) {
                long was_blinded = yourIntrinsic(BLINDED);
                if (!was_blinded)
                    You_cant("see in here!");
                make_blinded((long) tmp, FALSE);
                if (!was_blinded && !yourIntrinsic(BLINDED))
                    Your1(vision_clears);
            } else
                /* keep him blind until disgorged */
                make_blinded(yourIntrinsic(BLINDED) + 1, FALSE);
        }
        tmp = 0;
        break;
    case AD_ELEC:
        if (!mtmp->mcan && rn2(2)) {
            pline_The("air around you crackles with electricity.");
            if (youResistShock()) {
                shieldeff(currentX(), currentY());
                You("seem unhurt.");
                ugolemeffects(AD_ELEC, tmp);
                tmp = 0;
            }
        } else
            tmp = 0;
        break;
    case AD_COLD:
        if (!mtmp->mcan && rn2(2)) {
            if (youResistCold()) {
                shieldeff(currentX(), currentY());
                You_feel("mildly chilly.");
                ugolemeffects(AD_COLD, tmp);
                tmp = 0;
            } else
                You("are freezing to death!");
        } else
            tmp = 0;
        break;
    case AD_FIRE:
        if (!mtmp->mcan && rn2(2)) {
            if (youResistFire()) {
                shieldeff(currentX(), currentY());
                You_feel("mildly hot.");
                ugolemeffects(AD_FIRE, tmp);
                tmp = 0;
            } else
                You("are burning to a crisp!");
            burn_away_slime();
        } else
            tmp = 0;
        break;
    case AD_DISE:
        if (!diseasemu(mtmp->data))
            tmp = 0;
        break;
    case AD_DREN:
        /* AC magic cancellation doesn't help when engulfed */
        if (!mtmp->mcan && rn2(4))
            drain_en(tmp);
        tmp = 0;
        break;
    default:
        physical_damage = TRUE;
        tmp = 0;
        break;
    }

    if (physical_damage)
        tmp = Maybe_Half_Phys(tmp);

    mdamageu(mtmp, tmp);
    if (tmp)
        stop_occupation();

    if (touchPetrifies(pmid4you()) && !resists_ston(mtmp)) {
        pline("%s very hurriedly %s you!", Monnam(mtmp),
              isAnimal(pmid4mon(mtmp)) ? "regurgitates" : "expels");
        expels(mtmp, pmid4mon(mtmp), FALSE);
    } else if (!timeSinceBeingSwallowed() || monsterSize(pmid4you()) >= MZ_HUGE) {
        You("get %s!", isAnimal(pmid4mon(mtmp)) ? "regurgitated" : "expelled");
        if (flags.verbose
            && (isAnimal(pmid4mon(mtmp))
                || (monsterTypeCanCauseDamageType(pmid4mon(mtmp), AD_DGST) && youHaveSlowDigestion())))
            pline("Obviously %s doesn't like your taste.", mon_nam(mtmp));
        expels(mtmp, pmid4mon(mtmp), FALSE);
    }
    return 1;
}

/* monster explodes in your face */
STATIC_OVL int
explmu(mtmp, mattk, ufound)
register struct monst *mtmp;
register const struct Attack mattk;
boolean ufound;
{
    boolean physical_damage = TRUE, kill_agr = TRUE;

    if (mtmp->mcan)
        return 0;

    if (!ufound)
        pline("%s explodes at a spot in %s!",
              canseemon(mtmp) ? Monnam(mtmp) : "It",
              levl[mtmp->mux][mtmp->muy].typ == WATER ? "empty water"
                                                      : "thin air");
    else {
        register int tmp = d(mattk.dice, mattk.diceSides);
        register boolean not_affected = defends(mattk.damageType, uwep);

        hitmsg(mtmp, mattk);

        switch (mattk.damageType) {
        case AD_COLD:
            physical_damage = FALSE;
            not_affected |= youResistCold();
            goto common;
        case AD_FIRE:
            physical_damage = FALSE;
            not_affected |= youResistFire();
            goto common;
        case AD_ELEC:
            physical_damage = FALSE;
            not_affected |= youResistShock();
        common:

            if (!not_affected) {
                if (ACURR(A_DEX) > rnd(20)) {
                    You("duck some of the blast.");
                    tmp = (tmp + 1) / 2;
                } else {
                    if (flags.verbose)
                        You("get blasted!");
                }
                if (mattk.damageType == AD_FIRE)
                    burn_away_slime();
                if (physical_damage)
                    tmp = Maybe_Half_Phys(tmp);
                mdamageu(mtmp, tmp);
            }
            break;

        case AD_BLND:
            not_affected = resists_blnd(&youmonst);
            if (!not_affected) {
                /* sometimes you're affected even if it's invisible */
                if (mon_visible(mtmp) || (rnd(tmp /= 2) > currentExperienceLevel())) {
                    You("are blinded by a blast of light!");
                    make_blinded((long) tmp, FALSE);
                    if (youCanSee())
                        Your1(vision_clears);
                } else if (flags.verbose)
                    You("get the impression it was not terribly bright.");
            }
            break;

        case AD_HALU:
            not_affected |= youCannotSee() || (currentMonsterNumber() == PM_BLACK_LIGHT
                                      || currentMonsterNumber() == PM_VIOLET_FUNGUS
                                      || monsterTypeCanCauseDamageType(pmid4you(), AD_STUN));
            if (!not_affected) {
                boolean chg;
                if (!youAreHallucinating())
                    You("are caught in a blast of kaleidoscopic light!");
                /* avoid hallucinating the black light as it dies */
                mondead(mtmp);    /* remove it from map now */
                kill_agr = FALSE; /* already killed (maybe lifesaved) */
                chg =
                    make_hallucinated(yourIntrinsic(HALLUC) + (long) tmp, FALSE, 0L);
                You("%s.", chg ? "are freaked out" : "seem unaffected");
            }
            break;

        default:
            break;
        }
        if (not_affected) {
            You("seem unaffected by it.");
            ugolemeffects((int) mattk.damageType, tmp);
        }
    }
    if (kill_agr)
        mondead(mtmp);
    wake_nearto(mtmp->mx, mtmp->my, 7 * 7);
    return (mtmp->mhp > 0) ? 0 : 2;
}

/* monster gazes at you */
int
gazemu(mtmp, mattk)
register struct monst *mtmp;
register const struct Attack mattk;
{
    static const char *const reactions[] = {
        "confused",              /* [0] */
        "stunned",               /* [1] */
        "puzzled",   "dazzled",  /* [2,3] */
        "irritated", "inflamed", /* [4,5] */
        "tired",                 /* [6] */
        "dulled",                /* [7] */
    };
    int react = -1;
    boolean cancelled = (mtmp->mcan != 0), already = FALSE;

    /* assumes that hero has to see monster's gaze in order to be
       affected, rather than monster just having to look at hero;
       when hallucinating, hero's brain doesn't register what
       it's seeing correctly so the gaze is usually ineffective
       [this could be taken a lot farther and select a gaze effect
       appropriate to what's currently being displayed, giving
       ordinary monsters a gaze attack when hero thinks he or she
       is facing a gazing creature, but let's not go that far...] */
    if (youAreHallucinating() && rn2(4))
        cancelled = TRUE;

    switch (mattk.damageType) {
    case AD_STON:
        if (cancelled || !mtmp->mcansee) {
            if (!canseemon(mtmp))
                break; /* silently */
            pline("%s %s.", Monnam(mtmp),
                  (mtmp->data == &mons[PM_MEDUSA] && mtmp->mcan)
                      ? "doesn't look all that ugly"
                      : "gazes ineffectually");
            break;
        }
        if (youCanReflectAttacks() && couldsee(mtmp->mx, mtmp->my)
            && mtmp->data == &mons[PM_MEDUSA]) {
            /* hero has line of sight to Medusa and she's not blind */
            boolean useeit = canseemon(mtmp);

            if (useeit)
                (void) ureflects("%s gaze is reflected by your %s.",
                                 s_suffix(Monnam(mtmp)));
            if (mon_reflects(
                    mtmp, !useeit ? (char *) 0
                                  : "The gaze is reflected away by %s %s!"))
                break;
            if (!monsterCanSeeYou(mtmp)) { /* probably you're invisible */
                if (useeit)
                    pline(
                      "%s doesn't seem to notice that %s gaze was reflected.",
                          Monnam(mtmp), mhis(mtmp));
                break;
            }
            if (useeit)
                pline("%s is turned to stone!", Monnam(mtmp));
            stoned = TRUE;
            killed(mtmp);

            if (mtmp->mhp > 0)
                break;
            return 2;
        }
        if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my)
            && !youResistStoning()) {
            You("meet %s gaze.", s_suffix(mon_nam(mtmp)));
            stop_occupation();
            if (monsterPolymorphsWhenStoned(pmid4you()) && polymon(PM_STONE_GOLEM))
                break;
            You("turn to stone...");
            killer.format = KILLED_BY;
	    javaString monsterName = monsterTypeName(pmid4mon(mtmp));
            Strcpy(killer.name, monsterName.c_str);
	    releaseJavaString(monsterName);
            done(STONING);
        }
        break;
    case AD_CONF:
        if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) && mtmp->mcansee
            && !mtmp->mspec_used && rn2(5)) {
            if (cancelled) {
                react = 0; /* "confused" */
                already = (mtmp->mconf != 0);
            } else {
                int conf = d(3, 4);

                mtmp->mspec_used = mtmp->mspec_used + (conf + rn2(6));
                if (!youAreConfused())
                    pline("%s gaze confuses you!", s_suffix(Monnam(mtmp)));
                else
                    You("are getting more and more confused.");
                make_confused(yourIntrinsic(CONFUSION) + conf, FALSE);
                stop_occupation();
            }
        }
        break;
    case AD_STUN:
        if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) && mtmp->mcansee
            && !mtmp->mspec_used && rn2(5)) {
            if (cancelled) {
                react = 1; /* "stunned" */
                already = (mtmp->mstun != 0);
            } else {
                int stun = d(2, 6);

                mtmp->mspec_used = mtmp->mspec_used + (stun + rn2(6));
                pline("%s stares piercingly at you!", Monnam(mtmp));
                make_stunned(yourIntrinsicTimeout(STUNNED) + (long) stun, TRUE);
                stop_occupation();
            }
        }
        break;
    case AD_BLND:
        if (canseemon(mtmp) && !resists_blnd(&youmonst)
            && distanceSquaredToYou(mtmp->mx, mtmp->my) <= BOLT_LIM * BOLT_LIM) {
            if (cancelled) {
                react = rn1(2, 2); /* "puzzled" || "dazzled" */
                already = (mtmp->mcansee == 0);
                /* Archons gaze every round; we don't want cancelled ones
                   giving the "seems puzzled/dazzled" message that often */
                if (mtmp->mcan && mtmp->data == &mons[PM_ARCHON] && rn2(5))
                    react = -1;
            } else {
                int blnd = d(mattk.dice, mattk.diceSides);

                You("are blinded by %s radiance!", s_suffix(mon_nam(mtmp)));
                make_blinded((long) blnd, FALSE);
                stop_occupation();
                /* not blind at this point implies you're wearing
                   the Eyes of the Overworld; make them block this
                   particular stun attack too */
                if (youCanSee())
                    Your1(vision_clears);
                else
                    make_stunned((long) d(1, 3), TRUE);
            }
        }
        break;
    case AD_FIRE:
        if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) && mtmp->mcansee
            && !mtmp->mspec_used && rn2(5)) {
            if (cancelled) {
                react = rn1(2, 4); /* "irritated" || "inflamed" */
            } else {
                int dmg = d(2, 6), lev = (int) mtmp->m_lev;

                pline("%s attacks you with a fiery gaze!", Monnam(mtmp));
                stop_occupation();
                if (youResistFire()) {
                    pline_The("fire doesn't feel hot!");
                    dmg = 0;
                }
                burn_away_slime();
                if (lev > rn2(20))
                    destroy_item(SCROLL_CLASS, AD_FIRE);
                if (lev > rn2(20))
                    destroy_item(POTION_CLASS, AD_FIRE);
                if (lev > rn2(25))
                    destroy_item(SPBOOK_CLASS, AD_FIRE);
                if (dmg)
                    mdamageu(mtmp, dmg);
            }
        }
        break;
#ifdef PM_BEHOLDER /* work in progress */
    case AD_SLEE:
        if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) && mtmp->mcansee
            && multi >= 0 && !rn2(5) && !youResistSleep()) {
            if (cancelled) {
                react = 6;                      /* "tired" */
                already = (mtmp->mfrozen != 0); /* can't happen... */
            } else {
                fall_asleep(-rnd(10), TRUE);
                pline("%s gaze makes you very sleepy...",
                      s_suffix(Monnam(mtmp)));
            }
        }
        break;
    case AD_SLOW:
        if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) && mtmp->mcansee
            && (yourIntrinsicHasMask(FAST, (INTRINSIC | TIMEOUT))) && !defends(AD_SLOW, uwep)
            && !rn2(4)) {
            if (cancelled) {
                react = 7; /* "dulled" */
                already = (mtmp->mspeed == MSLOW);
            } else {
                u_slow_down();
                stop_occupation();
            }
        }
        break;
#endif /* BEHOLDER */
    default:
        impossible("Gaze attack %d?", mattk.damageType);
        break;
    }
    if (react >= 0) {
        if (youAreHallucinating() && rn2(3))
            react = rn2(SIZE(reactions));
        /* cancelled/hallucinatory feedback; monster might look "confused",
           "stunned",&c but we don't actually set corresponding attribute */
        pline("%s looks %s%s.", Monnam(mtmp),
              !rn2(3) ? "" : already ? "quite "
                                     : (!rn2(2) ? "a bit " : "somewhat "),
              reactions[react]);
    }
    return 0;
}

/* mtmp hits you for n points damage */
void
mdamageu(mtmp, n)
register struct monst *mtmp;
register int n;
{
    context.botl = 1;
    if (areYouPolymorphed()) {
        decreaseCurrentHitPointsAsMonster(n);
        if (currentHitPointsAsMonster() < 1)
            rehumanize();
    } else {
        decreaseCurrentHitPoints(n);
        if (currentHitPoints() < 1)
            done_in_by(mtmp, DIED);
    }
}

/* returns 0 if seduction impossible,
 *         1 if fine,
 *         2 if wrong gender for nymph
 */
int
could_seduce(magr, mdef, mattk)
struct monst *magr, *mdef;
const struct Attack mattk;
{
    int pmidagr;
    boolean agrinvis, defperc;
    xchar genagr, gendef;

    if (isAnimal(pmid4mon(magr)))
        return 0;
    if (magr == &youmonst) {
        pmidagr = pmid4you();
        agrinvis = youAreInvisibleToOthers();
        genagr = poly_gender();
    } else {
        pmidagr = pmid4mon(magr);
        agrinvis = magr->minvis;
        genagr = gender(magr);
    }
    if (mdef == &youmonst) {
        defperc = youCanSeeInvisible();
        gendef = poly_gender();
    } else {
        defperc = perceivesTheInvisible(pmid4mon(mdef));
        gendef = gender(mdef);
    }

    if (agrinvis && !defperc
        && (!SYSOPT_SEDUCE || ((validAttack(mattk)) && mattk.damageType != AD_SSEX)))
        return 0;

    if (monsterClass(pmidagr) != S_NYMPH
        && ((pmidagr != PM_INCUBUS && pmidagr != PM_SUCCUBUS)
            || (SYSOPT_SEDUCE && (validAttack(mattk)) && mattk.damageType != AD_SSEX)))
        return 0;

    if (genagr == 1 - gendef)
        return 1;
    else
        return (monsterClass(pmidagr) == S_NYMPH) ? 2 : 0;
}

/* Returns 1 if monster teleported */
int
doseduce(mon)
register struct monst *mon;
{
    register struct obj *ring, *nring;
    boolean fem = (mon->data == &mons[PM_SUCCUBUS]); /* otherwise incubus */
    int tried_gloves = 0;
    char qbuf[QBUFSZ];

    if (mon->mcan || mon->mspec_used) {
        pline("%s acts as though %s has got a %sheadache.", Monnam(mon),
              mhe(mon), mon->mcan ? "severe " : "");
        return 0;
    }

    if (unconscious()) {
        pline("%s seems dismayed at your lack of response.", Monnam(mon));
        return 0;
    }

    if (youCannotSee())
        pline("It caresses you...");
    else
        You_feel("very attracted to %s.", mon_nam(mon));
    /* if in the process of putting armor on or taking armor off,
       interrupt that activity now */
    (void) stop_donning((struct obj *) 0);
    /* don't try to take off gloves if cursed weapon blocks them */
    if (welded(uwep))
        tried_gloves = 1;

    for (ring = invent; ring; ring = nring) {
        nring = ring->nobj;
        if (ring->otyp != RIN_ADORNMENT)
            continue;
        if (fem) {
            if (ring->owornmask && uarmg) {
                /* don't take off worn ring if gloves are in the way */
                if (!tried_gloves++)
                    mayberem(uarmg, "gloves");
                if (uarmg)
                    continue; /* next ring might not be worn */
            }
            if (rn2(20) < ACURR(A_CHA)) {
                (void) safe_qbuf(qbuf, "\"That ",
                                 " looks pretty.  May I have it?\"", ring,
                                 xname, simpleonames, "ring");
                makeknown(RIN_ADORNMENT);
                if (yn(qbuf) == 'n')
                    continue;
            } else
                pline("%s decides she'd like %s, and takes it.",
                      youCannotSee() ? "She" : Monnam(mon), yname(ring));
            makeknown(RIN_ADORNMENT);
            if (ring == uleft || ring == uright)
                Ring_gone(ring);
            if (ring == uwep)
                setuwep((struct obj *) 0);
            if (ring == uswapwep)
                setuswapwep((struct obj *) 0);
            if (ring == uquiver)
                setuqwep((struct obj *) 0);
            freeinv(ring);
            (void) mpickobj(mon, ring);
        } else {
            if (uleft && uright && uleft->otyp == RIN_ADORNMENT
                && uright->otyp == RIN_ADORNMENT)
                break;
            if (ring == uleft || ring == uright)
                continue;
            if (uarmg) {
                /* don't put on ring if gloves are in the way */
                if (!tried_gloves++)
                    mayberem(uarmg, "gloves");
                if (uarmg)
                    break; /* no point trying further rings */
            }
            if (rn2(20) < ACURR(A_CHA)) {
                (void) safe_qbuf(qbuf, "\"That ",
                                " looks pretty.  Would you wear it for me?\"",
                                 ring, xname, simpleonames, "ring");
                makeknown(RIN_ADORNMENT);
                if (yn(qbuf) == 'n')
                    continue;
            } else {
                pline("%s decides you'd look prettier wearing %s,",
                      youCannotSee() ? "He" : Monnam(mon), yname(ring));
                pline("and puts it on your finger.");
            }
            makeknown(RIN_ADORNMENT);
            if (!uright) {
                pline("%s puts %s on your right %s.",
                      youCannotSee() ? "He" : Monnam(mon), the(xname(ring)),
                      body_part(HAND));
                setworn(ring, RIGHT_RING);
            } else if (!uleft) {
                pline("%s puts %s on your left %s.",
                      youCannotSee() ? "He" : Monnam(mon), the(xname(ring)),
                      body_part(HAND));
                setworn(ring, LEFT_RING);
            } else if (uright && uright->otyp != RIN_ADORNMENT) {
                pline("%s replaces %s with %s.", youCannotSee() ? "He" : Monnam(mon),
                      yname(uright), yname(ring));
                Ring_gone(uright);
                setworn(ring, RIGHT_RING);
            } else if (uleft && uleft->otyp != RIN_ADORNMENT) {
                pline("%s replaces %s with %s.", youCannotSee() ? "He" : Monnam(mon),
                      yname(uleft), yname(ring));
                Ring_gone(uleft);
                setworn(ring, LEFT_RING);
            } else
                impossible("ring replacement");
            Ring_on(ring);
            prinv((char *) 0, ring, 0L);
        }
    }

    if (!uarmc && !uarmf && !uarmg && !uarms && !uarmh && !uarmu)
        pline("%s murmurs sweet nothings into your ear.",
              youCannotSee() ? (fem ? "She" : "He") : Monnam(mon));
    else
        pline("%s murmurs in your ear, while helping you undress.",
              youCannotSee() ? (fem ? "She" : "He") : Monnam(mon));
    mayberem(uarmc, cloak_simple_name(uarmc));
    if (!uarmc)
        mayberem(uarm, "suit");
    mayberem(uarmf, "boots");
    if (!tried_gloves)
        mayberem(uarmg, "gloves");
    mayberem(uarms, "shield");
    mayberem(uarmh, helm_simple_name(uarmh));
    if (!uarmc && !uarm)
        mayberem(uarmu, "shirt");

    if (uarm || uarmc) {
        verbalize("You're such a %s; I wish...",
                  flags.female ? "sweet lady" : "nice guy");
        if (!tele_restrict(mon))
            (void) rloc(mon, TRUE);
        return 1;
    }
    if (currentAlignmentType() == A_CHAOTIC)
        adjalign(1);

    /* by this point you have discovered mon's identity, blind or not... */
    pline("Time stands still while you and %s lie in each other's arms...",
          noit_mon_nam(mon));
    if (rn2(35) > ACURR(A_CHA) + ACURR(A_INT)) {
        /* Don't bother with mspec_used here... it didn't get tired! */
        pline("%s seems to have enjoyed it more than you...",
              noit_Monnam(mon));
        switch (rn2(5)) {
        case 0:
            You_feel("drained of energy.");
            setCurrentMagicalEnergy(0);
            decreaseMaximumMagicalEnergy(rnd(youTakeHalfDamageFromPhysicalAttacks() ? 5 : 10));
            exercise(A_CON, FALSE);
            if (maximumMagicalEnergy() < 0)
                setMaximumMagicalEnergy(0);
            break;
        case 1:
            You("are down in the dumps.");
            (void) adjattrib(A_CON, -1, TRUE);
            exercise(A_CON, FALSE);
            context.botl = 1;
            break;
        case 2:
            Your("senses are dulled.");
            (void) adjattrib(A_WIS, -1, TRUE);
            exercise(A_WIS, FALSE);
            context.botl = 1;
            break;
        case 3:
            if (!resists_drli(&youmonst)) {
                You_feel("out of shape.");
                losexp("overexertion");
            } else {
                You("have a curious feeling...");
            }
            break;
        case 4: {
            int tmp;
            You_feel("exhausted.");
            exercise(A_STR, FALSE);
            tmp = rn1(10, 6);
            losehp(Maybe_Half_Phys(tmp), "exhaustion", KILLED_BY);
            break;
        }
        }
    } else {
        mon->mspec_used = rnd(100); /* monster is worn out */
        You("seem to have enjoyed it more than %s...", noit_mon_nam(mon));
        switch (rn2(5)) {
        case 0:
            You_feel("raised to your full potential.");
            exercise(A_CON, TRUE);
            increaseMaximumMagicalEnergy(rnd(5));
            setCurrentMagicalEnergy(maximumMagicalEnergy());
            break;
        case 1:
            You_feel("good enough to do it again.");
            (void) adjattrib(A_CON, 1, TRUE);
            exercise(A_CON, TRUE);
            context.botl = 1;
            break;
        case 2:
            You("will always remember %s...", noit_mon_nam(mon));
            (void) adjattrib(A_WIS, 1, TRUE);
            exercise(A_WIS, TRUE);
            context.botl = 1;
            break;
        case 3:
            pline("That was a very educational experience.");
            pluslvl(FALSE);
            exercise(A_WIS, TRUE);
            break;
        case 4:
            You_feel("restored to health!");
            setCurrentHitPoints(maximumHitPoints());
            if (areYouPolymorphed())
                setCurrentHitPointsAsMonster(maximumHitPointsAsMonster());
            exercise(A_STR, TRUE);
            context.botl = 1;
            break;
        }
    }

    if (mon->mtame) /* don't charge */
        ;
    else if (rn2(20) < ACURR(A_CHA)) {
        pline("%s demands that you pay %s, but you refuse...",
              noit_Monnam(mon), youCannotSee() ? (fem ? "her" : "him") : mhim(mon));
    } else if (currentMonsterNumber() == PM_LEPRECHAUN)
        pline("%s tries to take your money, but fails...", noit_Monnam(mon));
    else {
        long cost;
        long umoney = money_cnt(invent);

        if (umoney > (long) LARGEST_INT - 10L)
            cost = (long) rnd(LARGEST_INT) + 500L;
        else
            cost = (long) rnd((int) umoney + 10) + 500L;
        if (mon->mpeaceful) {
            cost /= 5L;
            if (!cost)
                cost = 1L;
        }
        if (cost > umoney)
            cost = umoney;
        if (!cost)
            verbalize("It's on the house!");
        else {
            pline("%s takes %ld %s for services rendered!", noit_Monnam(mon),
                  cost, currency(cost));
            money2mon(mon, cost);
            context.botl = 1;
        }
    }
    if (!rn2(25))
        mon->mcan = 1; /* monster is worn out */
    if (!tele_restrict(mon))
        (void) rloc(mon, TRUE);
    return 1;
}

STATIC_OVL void
mayberem(obj, str)
register struct obj *obj;
const char *str;
{
    char qbuf[QBUFSZ];

    if (!obj || !obj->owornmask)
        return;

    if (rn2(20) < ACURR(A_CHA)) {
        Sprintf(qbuf, "\"Shall I remove your %s, %s?\"", str,
                (!rn2(2) ? "lover" : !rn2(2) ? "dear" : "sweetheart"));
        if (yn(qbuf) == 'n')
            return;
    } else {
        char hairbuf[BUFSZ];

        Sprintf(hairbuf, "let me run my fingers through your %s",
                body_part(HAIR));
        verbalize("Take off your %s; %s.", str,
                  (obj == uarm)
                     ? "let's get a little closer"
                     : (obj == uarmc || obj == uarms)
                        ? "it's in the way"
                        : (obj == uarmf)
                           ? "let me rub your feet"
                           : (obj == uarmg)
                              ? "they're too clumsy"
                              : (obj == uarmu)
                                 ? "let me massage you"
                                 /* obj == uarmh */
                                 : hairbuf);
    }
    remove_worn_item(obj, TRUE);
}

STATIC_OVL int
passiveum(oldupmid, mtmp, mattk)
int oldupmid;
register struct monst *mtmp;
register const struct Attack mattk;
{
    int i, tmp;

    int upmid = pmid4you();

    for (i = 0;; i++) {
        if (i >= monsterAttacks(oldupmid)) {
            return 1;
	}
	int olduatype = monsterAttack(oldupmid, i).type;
        if (olduatype == AT_NONE || olduatype == AT_BOOM) {
            break;
	}
    }
    struct Attack oldumattk = monsterAttack(oldupmid, i);
    if (oldumattk.dice) {
        tmp = d(oldumattk.dice, oldumattk.diceSides);
    } else if (oldumattk.diceSides) {
        tmp = d(monsterLevel(oldupmid) + 1, oldumattk.diceSides);
    } else {
        tmp = 0;
    }

    /* These affect the enemy even if you were "killed" (rehumanized) */
    switch (oldumattk.damageType) {
    case AD_ACID:
        if (!rn2(2)) {
            pline("%s is splashed by your acid!", Monnam(mtmp));
            if (resists_acid(mtmp)) {
                pline("%s is not affected.", Monnam(mtmp));
                tmp = 0;
            }
        } else
            tmp = 0;
        if (!rn2(30))
            erode_armor(mtmp, ERODE_CORRODE);
        if (!rn2(6))
            acid_damage(MON_WEP(mtmp));
        goto assess_dmg;
    case AD_STON: /* cockatrice */
    {
        long protector = attk_protection(mattk.type),
             wornitems = mtmp->misc_worn_check;

        /* wielded weapon gives same protection as gloves here */
        if (MON_WEP(mtmp) != 0)
            wornitems |= W_ARMG;

        if (!resists_ston(mtmp)
            && (protector == 0L
                || (protector != ~0L
                    && (wornitems & protector) != protector))) {
            if (monsterPolymorphsWhenStoned(pmid4mon(mtmp))) {
                mon_to_stone(mtmp);
                return 1;
            }
            pline("%s turns to stone!", Monnam(mtmp));
            stoned = 1;
            xkilled(mtmp, 0);
            if (mtmp->mhp > 0)
                return 1;
            return 2;
        }
        return 1;
    }
    case AD_ENCH: /* KMH -- remove enchantment (disenchanter) */
        if (otmp) {
            (void) drain_item(otmp);
            /* No message */
        }
        return 1;
    default:
        break;
    }
    if (!areYouPolymorphed())
        return 1;

    /* These affect the enemy only if you are still a monster */
    struct Attack umattk = monsterAttack(upmid, i);
    if (rn2(3))
        switch (umattk.damageType) {
        case AD_PHYS:
            if (umattk.type == AT_BOOM) {
                You("explode!");
                /* KMH, balance patch -- this is okay with unchanging */
                rehumanize();
                goto assess_dmg;
            }
            break;
        case AD_PLYS: /* Floating eye */
            if (tmp > 127)
                tmp = 127;
            if (currentMonsterNumber() == PM_FLOATING_EYE) {
                if (!rn2(4))
                    tmp = 127;
                if (mtmp->mcansee && hasEyes(pmid4mon(mtmp)) && rn2(3)
                    && (perceivesTheInvisible(pmid4mon(mtmp)) || !youAreInvisibleToOthers())) {
                    if (youCannotSee()) {
			javaString youMonsterName = monsterTypeName(upmid);
                        pline("As a blind %s, you cannot defend yourself.",
                              youMonsterName.c_str);
			releaseJavaString(youMonsterName);
		    } else {
                        if (mon_reflects(mtmp,
                                         "Your gaze is reflected by %s %s."))
                            return 1;
                        pline("%s is frozen by your gaze!", Monnam(mtmp));
                        paralyze_monst(mtmp, tmp);
                        return 3;
                    }
                }
            } else { /* gelatinous cube */
                pline("%s is frozen by you.", Monnam(mtmp));
                paralyze_monst(mtmp, tmp);
                return 3;
            }
            return 1;
        case AD_COLD: /* Brown mold or blue jelly */
            if (resists_cold(mtmp)) {
                shieldeff(mtmp->mx, mtmp->my);
                pline("%s is mildly chilly.", Monnam(mtmp));
                golemeffects(mtmp, AD_COLD, tmp);
                tmp = 0;
                break;
            }
            pline("%s is suddenly very cold!", Monnam(mtmp));
            increaseCurrentHitPointsAsMonster(tmp / 2);
            if (maximumHitPointsAsMonster() < currentHitPointsAsMonster()) {
                setMaximumHitPointsAsMonster(currentHitPointsAsMonster());
            }
            if (maximumHitPointsAsMonster() > ((monsterLevel(upmid) + 1) * 8))
                (void) split_mon(&youmonst, mtmp);
            break;
        case AD_STUN: /* Yellow mold */
            if (!mtmp->mstun) {
                mtmp->mstun = 1;
                pline("%s %s.", Monnam(mtmp),
                      makeplural(staggerVerb(pmid4mon(mtmp), "stagger")));
            }
            tmp = 0;
            break;
        case AD_FIRE: /* Red mold */
            if (resists_fire(mtmp)) {
                shieldeff(mtmp->mx, mtmp->my);
                pline("%s is mildly warm.", Monnam(mtmp));
                golemeffects(mtmp, AD_FIRE, tmp);
                tmp = 0;
                break;
            }
            pline("%s is suddenly very hot!", Monnam(mtmp));
            break;
        case AD_ELEC:
            if (resists_elec(mtmp)) {
                shieldeff(mtmp->mx, mtmp->my);
                pline("%s is slightly tingled.", Monnam(mtmp));
                golemeffects(mtmp, AD_ELEC, tmp);
                tmp = 0;
                break;
            }
            pline("%s is jolted with your electricity!", Monnam(mtmp));
            break;
        default:
            tmp = 0;
            break;
        }
    else
        tmp = 0;

assess_dmg:
    if ((mtmp->mhp -= tmp) <= 0) {
        pline("%s dies!", Monnam(mtmp));
        xkilled(mtmp, 0);
        if (mtmp->mhp > 0)
            return 1;
        return 2;
    }
    return 1;
}

struct monst *
cloneu()
{
    register struct monst *mon;
    int mndx = pmid4you();

    if (currentHitPointsAsMonster() <= 1)
        return (struct monst *) 0;
    if (mvitals[mndx].mvflags & G_EXTINCT)
        return (struct monst *) 0;
    mon = makeMonsterOfType(pmid4you(), currentX(), currentY(), NO_MINVENT | MM_EDOG);
    if (!mon)
        return NULL;
    mon->mcloned = 1;
    mon = christen_monst(mon, plname);
    initedog(mon);
    mon->m_lev = monsterLevel(pmid4you());
    mon->mhpmax = maximumHitPointsAsMonster();
    mon->mhp = currentHitPointsAsMonster() / 2;
    decreaseCurrentHitPointsAsMonster(mon->mhp);
    context.botl = 1;
    return mon;
}

/*mhitu.c*/
