/* NetHack 3.6	priest.c	$NHDT-Date: 1446892452 2015/11/07 10:34:12 $  $NHDT-Branch: master $:$NHDT-Revision: 1.41 $ */
/* Copyright (c) Izchak Miller, Steve Linhart, 1989.              */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "mfndpos.h"

/* these match the categorizations shown by enlightenment */
#define ALGN_SINNED (-4) /* worse than strayed (-1..-3) */
#define ALGN_PIOUS 14    /* better than fervent (9..13) */

STATIC_DCL boolean FDECL(histemple_at, (struct monst *, XCHAR_P, XCHAR_P));
STATIC_DCL boolean FDECL(has_shrine, (struct monst *));

void
newepri(mtmp)
struct monst *mtmp;
{
    if (!mtmp->mextra)
        mtmp->mextra = newmextra();
    if (!EPRI(mtmp)) {
        EPRI(mtmp) = (struct epri *) alloc(sizeof(struct epri));
        (void) memset((genericptr_t) EPRI(mtmp), 0, sizeof(struct epri));
    }
}

void
free_epri(mtmp)
struct monst *mtmp;
{
    if (mtmp->mextra && EPRI(mtmp)) {
        free((genericptr_t) EPRI(mtmp));
        EPRI(mtmp) = (struct epri *) 0;
    }
    mtmp->ispriest = 0;
}

/*
 * Move for priests and shopkeepers.  Called from shk_move() and pri_move().
 * Valid returns are  1: moved  0: didn't  -1: let m_move do it  -2: died.
 */
int
move_special(mtmp, in_his_shop, appr, uondoor, avoid, omx, omy, gx, gy)
register struct monst *mtmp;
boolean in_his_shop;
schar appr;
boolean uondoor, avoid;
register xchar omx, omy, gx, gy;
{
    register xchar nx, ny, nix, niy;
    register schar i;
    schar chcnt, cnt;
    coord poss[9];
    long info[9];
    long allowflags;
    struct obj *ib = (struct obj *) 0;

    if (omx == gx && omy == gy)
        return 0;
    if (mtmp->mconf) {
        avoid = FALSE;
        appr = 0;
    }

    nix = omx;
    niy = omy;
    if (mtmp->isshk)
        allowflags = ALLOW_SSM;
    else
        allowflags = ALLOW_SSM | ALLOW_SANCT;
    if (passesThroughWalls(pmid4mon(mtmp)))
        allowflags |= (ALLOW_ROCK | ALLOW_WALL);
    if (throwsRocks(pmid4mon(mtmp)))
        allowflags |= ALLOW_ROCK;
    if (isTunneler(pmid4mon(mtmp)))
        allowflags |= ALLOW_DIG;
    if (!hasNoHands(pmid4mon(mtmp)) && !isVerySmallMonster(pmid4mon(mtmp))) {
        allowflags |= OPENDOOR;
        if (monhaskey(mtmp, TRUE))
            allowflags |= UNLOCKDOOR;
    }
    if (isGiant(pmid4mon(mtmp)))
        allowflags |= BUSTDOOR;
    cnt = mfndpos(mtmp, poss, info, allowflags);

    if (mtmp->isshk && avoid && uondoor) { /* perhaps we cannot avoid him */
        for (i = 0; i < cnt; i++)
            if (!(info[i] & NOTONL))
                goto pick_move;
        avoid = FALSE;
    }

#define GDIST(x, y) (dist2(x, y, gx, gy))
pick_move:
    chcnt = 0;
    for (i = 0; i < cnt; i++) {
        nx = poss[i].x;
        ny = poss[i].y;
        if (IS_ROOM(levl[nx][ny].typ)
            || (mtmp->isshk && (!in_his_shop || ESHK(mtmp)->following))) {
            if (avoid && (info[i] & NOTONL))
                continue;
            if ((!appr && !rn2(++chcnt))
                || (appr && GDIST(nx, ny) < GDIST(nix, niy))) {
                nix = nx;
                niy = ny;
            }
        }
    }
    if (mtmp->ispriest && avoid && nix == omx && niy == omy
        && linedUpWithYou(omx, omy)) {
        /* might as well move closer as long it's going to stay
         * lined up */
        avoid = FALSE;
        goto pick_move;
    }

    if (nix != omx || niy != omy) {
        remove_monster(omx, omy);
        place_monster(mtmp, nix, niy);
        newsym(nix, niy);
        if (mtmp->isshk && !in_his_shop && inhishop(mtmp))
            check_special_room(FALSE);
        if (ib) {
            if (cansee(mtmp->mx, mtmp->my))
                pline("%s picks up %s.", Monnam(mtmp),
                      distant_name(ib, doname));
            obj_extract_self(ib);
            (void) mpickobj(mtmp, ib);
        }
        return 1;
    }
    return 0;
}

boolean wasPreviouslyOccupyingTemple()
{
    register char *ptr;

    for (int i = 0; ('\0' != previouslyOccupiedRoomIDs(i)); i++) {
        if (rooms[previouslyOccupiedRoomIDs(i) - ROOMOFFSET].rtype == TEMPLE)
	{
            return TRUE;
	}
    }
    return FALSE;
}


char mostRecentCurrentlyOccupiedTempleID()
{
    for (int i = 0; ('\0' != currentlyOccupiedRoomIDs(i)); i++)
    {
        char roomID = currentlyOccupiedRoomIDs(i);
        if (rooms[roomID - ROOMOFFSET].rtype == TEMPLE)
	{
            return roomID;
	}
    }
    return '\0';
}

STATIC_OVL boolean
histemple_at(priest, x, y)
register struct monst *priest;
register xchar x, y;
{
    return (boolean) (priest && priest->ispriest
                      && (EPRI(priest)->shroom == templeLocatedAt(x, y))
                      && areYouOnLevel(&(EPRI(priest)->shrlevel)));
}

boolean
inhistemple(priest)
struct monst *priest;
{
    /* make sure we have a priest */
    if (!priest || !priest->ispriest)
        return FALSE;
    /* priest must be on right level and in right room */
    if (!histemple_at(priest, priest->mx, priest->my))
        return FALSE;
    /* temple room must still contain properly aligned altar */
    return has_shrine(priest);
}

/*
 * pri_move: return 1: moved  0: didn't  -1: let m_move do it  -2: died
 */
int
pri_move(priest)
register struct monst *priest;
{
    register xchar gx, gy, omx, omy;
    schar temple;
    boolean avoid = TRUE;

    omx = priest->mx;
    omy = priest->my;

    if (!histemple_at(priest, omx, omy))
        return -1;

    temple = EPRI(priest)->shroom;

    gx = EPRI(priest)->shrpos.x;
    gy = EPRI(priest)->shrpos.y;

    gx += rn1(3, -1); /* mill around the altar */
    gy += rn1(3, -1);

    if (!priest->mpeaceful
        || (youCauseConflict() && !resist(priest, RING_CLASS, 0, 0))) {
        if (monnear(priest, currentX(), currentY())) {
            if (youAppearDisplaced())
                Your("displaced image doesn't fool %s!", mon_nam(priest));
            (void) mattacku(priest);
            return 0;
        } else if (oneOfCurrentlyOccupiedRoomsHasID(temple)) {
            /* chase player if inside temple & can see him */
            if (priest->mcansee && monsterCanSeeYou(priest)) {
                gx = currentX();
                gy = currentY();
            }
            avoid = FALSE;
        }
    } else if (youAreInvisibleToOthers())
        avoid = FALSE;

    return move_special(priest, FALSE, TRUE, FALSE, avoid, omx, omy, gx, gy);
}

/* exclusively for mktemple() */
void
initPriestOnCurrentLevel(sroom, sx, sy, sanctum)
struct mkroom *sroom;
int sx, sy;
boolean sanctum; /* is it the seat of the high priest? */
{
    struct monst *priest;
    struct obj *otmp;
    int cnt;

    if (MON_AT(sx + 1, sy))
        (void) rloc(m_at(sx + 1, sy), FALSE); /* insurance */

    priest = makeMonsterOfType((sanctum ? PM_HIGH_PRIEST : PM_ALIGNED_PRIEST), sx + 1, sy, MM_EPRI);
    if (priest) {
        EPRI(priest)->shroom = (schar) ((sroom - rooms) + ROOMOFFSET);
        EPRI(priest)->shralign = Amask2align(levl[sx][sy].altarmask);
        EPRI(priest)->shrpos.x = sx;
        EPRI(priest)->shrpos.y = sy;
        assignFromCurrentLevel(&(EPRI(priest)->shrlevel));
        priest->mtrapseen = ~0; /* traps are known */
        priest->mpeaceful = 1;
        priest->ispriest = 1;
        priest->isminion = 0;
        priest->msleeping = 0;
        set_malign(priest); /* mpeaceful may have changed */

        /* now his/her goodies... */
        if (sanctum && EPRI(priest)->shralign == A_NONE
            && areYouOnLevel(&sanctum_level)) {
            (void) mongets(priest, AMULET_OF_YENDOR);
        }
        /* 2 to 4 spellbooks */
        for (cnt = rn1(3, 2); cnt > 0; --cnt) {
            (void) mpickobj(priest, mkobj(SPBOOK_CLASS, FALSE));
        }
        /* robe [via makeMonsterOfType()] */
        if (rn2(2) && (otmp = which_armor(priest, W_ARMC)) != 0) {
            if (p_coaligned(priest))
                uncurse(otmp);
            else
                curse(otmp);
        }
    }
}

/* get a monster's alignment type without caller needing EPRI & EMIN */
aligntyp
mon_aligntyp(mon)
struct monst *mon;
{
    aligntyp algn = mon->ispriest ? EPRI(mon)->shralign
                                  : mon->isminion ? EMIN(mon)->min_align
                                                  : monsterAlignment(pmid4mon(mon));

    if (algn == A_NONE)
        return A_NONE; /* negative but differs from chaotic */
    return (algn > 0) ? A_LAWFUL : (algn < 0) ? A_CHAOTIC : A_NEUTRAL;
}

/*
 * Specially aligned monsters are named specially.
 *      - aligned priests with ispriest and high priests have shrines
 *              they retain ispriest and epri when polymorphed
 *      - aligned priests without ispriest are roamers
 *              they have isminion set and use emin rather than epri
 *      - minions do not have ispriest but have isminion and emin
 *      - caller needs to inhibit hallucination if it wants to force
 *              the true name even when under that influence
 */
char *
priestname(mon, pname)
register struct monst *mon;
char *pname; /* caller-supplied output buffer */
{
    boolean do_hallu = youAreHallucinating();
    int pmid = pmid4mon(mon);
    boolean aligned_priest = (pmid == PM_ALIGNED_PRIEST);
    boolean high_priest = (pmid == PM_HIGH_PRIEST);

    char whatcode = '\0';
    javaString what = NO_JAVA_STRING;

    if (do_hallu) {
        what.c_str = rndmonnam(&whatcode);
    } else {
        what = monsterTypeName(pmid4mon(mon));
    }

    if (!mon->ispriest && !mon->isminion) {       /* should never happen...  */
        char* result = strcpy(pname, what.c_str); /* caller must be confused */
	releaseJavaString(what);
	return result;
    }

    *pname = '\0';
    if (!do_hallu || !bogon_is_pname(whatcode))
        Strcat(pname, "the ");
    if (mon->minvis)
        Strcat(pname, "invisible ");
    if (mon->isminion && EMIN(mon)->renegade)
        Strcat(pname, "renegade ");

    if (mon->ispriest || aligned_priest) { /* high_priest implies ispriest */
        if (!aligned_priest && !high_priest) {
            ; /* polymorphed priest; use ``what'' as is */
        } else {
            if (high_priest)
                Strcat(pname, "high ");

	    releaseJavaString(what);
            what.j_str = NULL;
            if (youAreHallucinating())
                what.c_str = "poohbah";
            else if (mon->female)
                what.c_str = "priestess";
            else
                what.c_str = "priest";
        }
    } else {
        if (mon->mtame && !strcmpi(what.c_str, "Angel"))
            Strcat(pname, "guardian ");
    }

    Strcat(pname, what.c_str);
    releaseJavaString(what);

    /* same as distant_monnam(), more or less... */
    if (do_hallu || !high_priest || !areYouOnAstralLevel()
        || distanceSquaredToYou(mon->mx, mon->my) <= 2 || program_state.gameover) {
        Strcat(pname, " of ");

	javaString deityName;
	if (do_hallu) {
	    deityName = randomHallucinatoryDeityName();
	} else {
	    deityName = nameOfAlignedDeityFromYourPantheon(mon_aligntyp(mon));
	}
        Strcat(pname, deityName.c_str);
	releaseJavaString(deityName);
    }
    return pname;
}

boolean
p_coaligned(priest)
struct monst *priest;
{
    return (boolean) (currentAlignmentType() == mon_aligntyp(priest));
}

STATIC_OVL boolean
has_shrine(pri)
struct monst *pri;
{
    struct rm *lev;
    struct epri *epri_p;

    if (!pri || !pri->ispriest)
        return FALSE;
    epri_p = EPRI(pri);
    lev = &levl[epri_p->shrpos.x][epri_p->shrpos.y];
    if (!IS_ALTAR(lev->typ) || !(lev->altarmask & AM_SHRINE))
        return FALSE;
    return (boolean) (epri_p->shralign
                      == (Amask2align(lev->altarmask & ~AM_SHRINE)));
}

struct monst *
findpriest(roomno)
char roomno;
{
    register struct monst *mtmp;

    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
        if (DEADMONSTER(mtmp))
            continue;
        if (mtmp->ispriest && (EPRI(mtmp)->shroom == roomno)
            && histemple_at(mtmp, mtmp->mx, mtmp->my))
            return mtmp;
    }
    return (struct monst *) 0;
}

/* called from check_special_room() when the player enters the temple room */
void
intemple(roomno)
int roomno;
{
    struct monst *priest, *mtmp;
    struct epri *epri_p;
    boolean shrined, sanctum, can_speak;
    long *this_time, *other_time;
    const char *msg1, *msg2;
    char buf[BUFSZ];

    /* don't do anything if hero is already in the room */
    if (wasPreviouslyOccupyingTemple()) {
        return;
    }

    if ((priest = findpriest((char) roomno)) != 0) {
        /* tended */

        epri_p = EPRI(priest);
        shrined = has_shrine(priest);
        sanctum = ((pmid4mon(priest) == PM_HIGH_PRIEST)
                   && (areYouOnSanctumLevel() || areYouInEndgame()));
        can_speak = (priest->mcanmove && !priest->msleeping);
        if (can_speak && !youAreDeaf() && moves >= epri_p->intone_time) {
            unsigned save_priest = priest->ispriest;

            /* don't reveal the altar's owner upon temple entry in
               the endgame; for the Sanctum, the next message names
               Moloch so suppress the "of Moloch" for him here too */
            if (sanctum && !youAreHallucinating())
                priest->ispriest = 0;
            pline("%s intones:",
                  canseemon(priest) ? Monnam(priest) : "A nearby voice");
            priest->ispriest = save_priest;
            epri_p->intone_time = moves + (long) d(10, 500); /* ~2505 */
            /* make sure that we don't suppress entry message when
               we've just given its "priest intones" introduction */
            epri_p->enter_time = 0L;
        }
        msg1 = msg2 = 0;
        if (sanctum && areYouOnSanctumLevel()) {
            if (priest->mpeaceful) {
                /* first time inside */
                msg1 = "Infidel, you have entered Moloch's Sanctum!";
                msg2 = "Be gone!";
                priest->mpeaceful = 0;
                /* became angry voluntarily; no penalty for attacking him */
                set_malign(priest);
            } else {
                /* repeat visit, or attacked priest before entering */
                msg1 = "You desecrate this place by your presence!";
            }
        } else if (moves >= epri_p->enter_time) {
            Sprintf(buf, "Pilgrim, you enter a %s place!",
                    !shrined ? "desecrated" : "sacred");
            msg1 = buf;
        }
        if (msg1 && can_speak && !youAreDeaf()) {
            verbalize1(msg1);
            if (msg2)
                verbalize1(msg2);
            epri_p->enter_time = moves + (long) d(10, 100); /* ~505 */
        }
        if (!sanctum) {
            if (!shrined || !p_coaligned(priest)
                || currentAlignmentRecord() <= ALGN_SINNED) {
                msg1 = "have a%s forbidding feeling...";
                msg2 = (!shrined || !p_coaligned(priest)) ? "" : " strange";
                this_time = &epri_p->hostile_time;
                other_time = &epri_p->peaceful_time;
            } else {
                msg1 = "experience %s sense of peace.";
                msg2 = (currentAlignmentRecord() >= ALGN_PIOUS) ? "a" : "an unusual";
                this_time = &epri_p->peaceful_time;
                other_time = &epri_p->hostile_time;
            }
            /* give message if we haven't seen it recently or
               if alignment update has caused it to switch from
               forbidding to sense-of-peace or vice versa */
            if (moves >= *this_time || *other_time >= *this_time) {
                You(msg1, msg2);
                *this_time = moves + (long) d(10, 20); /* ~55 */
                /* avoid being tricked by the RNG:  switch might have just
                   happened and previous random threshold could be larger */
                if (*this_time <= *other_time)
                    *other_time = *this_time - 1L;
            }
        }
        /* recognize the Valley of the Dead and Moloch's Sanctum
           once hero has encountered the temple priest on those levels */
        mapseen_temple(priest);
    } else {
        /* untended */

        switch (rn2(4)) {
        case 0:
            You("have an eerie feeling...");
            break;
        case 1:
            You_feel("like you are being watched.");
            break;
        case 2:
            pline("A shiver runs down your %s.", body_part(SPINE));
            break;
        default:
            break; /* no message; unfortunately there's no
                      EPRI(priest)->eerie_time available to
                      make sure we give one the first time */
        }
        if (!rn2(5)
            && (mtmp = makeMonsterOfType(PM_GHOST, currentX(), currentY(), NO_MM_FLAGS))
                   != 0) {
            /* [TODO: alter this (at a minimum, by switching from
               an exclamation to a simple declaration) if hero has
               already killed enough ghosts.] */
            if (canspotmon(mtmp))
                pline("An enormous ghost appears next to you!");
            else
                You("sense a presence close by!");
            mtmp->mpeaceful = 0;
            set_malign(mtmp);
            if (flags.verbose)
                You("are frightened to death, and unable to move.");
            nomul(-3);
            multi_reason = "being terrified of a demon";
            nomovemsg = "You regain your composure.";
        }
    }
}

/* reset the move counters used to limit temple entry feedback;
   leaving the level and then returning yields a fresh start */
void
forget_temple_entry(priest)
struct monst *priest;
{
    struct epri *epri_p = priest->ispriest ? EPRI(priest) : 0;

    if (!epri_p) {
        impossible("attempting to manipulate shrine data for non-priest?");
        return;
    }
    epri_p->intone_time = epri_p->enter_time = epri_p->peaceful_time =
        epri_p->hostile_time = 0L;
}

void
priest_talk(priest)
register struct monst *priest;
{
    boolean coaligned = p_coaligned(priest);
    boolean strayed = (currentAlignmentRecord() < 0);

    /* KMH, conduct */
    setAtheistConduct(FALSE);

    if (priest->mflee || (!priest->ispriest && coaligned && strayed)) {
        pline("%s doesn't want anything to do with you!", Monnam(priest));
        priest->mpeaceful = 0;
        return;
    }

    /* priests don't chat unless peaceful and in their own temple */
    if (!inhistemple(priest) || !priest->mpeaceful
        || !priest->mcanmove || priest->msleeping) {
        static const char *cranky_msg[3] = {
            "Thou wouldst have words, eh?  I'll give thee a word or two!",
            "Talk?  Here is what I have to say!",
            "Pilgrim, I would speak no longer with thee."
        };

        if (!priest->mcanmove || priest->msleeping) {
            javaString possessive = possessivePronoun(pronoun_gender(priest));
            pline("%s breaks out of %s reverie!", Monnam(priest),
                  possessive.c_str);
	    releaseJavaString(possessive);
            priest->mfrozen = priest->msleeping = 0;
            priest->mcanmove = 1;
        }
        priest->mpeaceful = 0;
        verbalize1(cranky_msg[rn2(3)]);
        return;
    }

    /* you desecrated the temple and now you want to chat? */
    if (priest->mpeaceful && locationIsInATemple(priest->mx, priest->my)
        && !has_shrine(priest)) {
        verbalize(
              "Begone!  Thou desecratest this holy place with thy presence.");
        priest->mpeaceful = 0;
        return;
    }
    if (!money_cnt(invent)) {
        if (coaligned && !strayed) {
            long pmoney = money_cnt(priest->minvent);
            if (pmoney > 0L) {
                /* Note: two bits is actually 25 cents.  Hmm. */
                pline("%s gives you %s for an ale.", Monnam(priest),
                      (pmoney == 1L) ? "one bit" : "two bits");
                money2u(priest, pmoney > 1L ? 2 : 1);
            } else
                pline("%s preaches the virtues of poverty.", Monnam(priest));
            exercise(A_WIS, TRUE);
        } else
            pline("%s is not interested.", Monnam(priest));
        return;
    } else {
        long offer;

        pline("%s asks you for a contribution for the temple.",
              Monnam(priest));
        if ((offer = bribe(priest)) == 0) {
            verbalize("Thou shalt regret thine action!");
            if (coaligned)
                adjalign(-1);
        } else if (offer < (currentExperienceLevel() * 200)) {
            if (money_cnt(invent) > (offer * 2L)) {
                verbalize("Cheapskate.");
            } else {
                verbalize("I thank thee for thy contribution.");
                /* give player some token */
                exercise(A_WIS, TRUE);
            }
        } else if (offer < (currentExperienceLevel() * 400)) {
            verbalize("Thou art indeed a pious individual.");
            if (money_cnt(invent) < (offer * 2L)) {
                if (coaligned && currentAlignmentRecord() <= ALGN_SINNED)
                    adjalign(1);
                verbalize("I bestow upon thee a blessing.");
                incrementYourIntrinsicTimeout(CLAIRVOYANT, rn1(500, 500));
            }
        } else if (offer < (currentExperienceLevel() * 600)
                   /* blessings is only active when Protection is
                      enabled via something other than worn gear
                      (theft by gremlin clears the intrinsic but not
                      its former magnitude, making it recoverable) */
                   && (!(yourIntrinsicHasMask(PROTECTION, INTRINSIC))
                       || (blessings() < 20
                           && (blessings() < 9 || !rn2(blessings()))))) {
            verbalize("Thy devotion has been rewarded.");
            if (!(yourIntrinsicHasMask(PROTECTION, INTRINSIC))) {
                setYourIntrinsicMask(PROTECTION, FROMOUTSIDE);
                if (!blessings()) {
                    setBlessings(rn1(3, 2));
                }
            } else {
                increaseBlessings(1);
            }
        } else {
            verbalize("Thy selfless generosity is deeply appreciated.");
            if (money_cnt(invent) < (offer * 2L) && coaligned) {
                if (strayed && (moves - cleansedSinceMove()) > 5000L) {
                    setCurrentAlignmentRecord(0); /* cleanse thee */
                    setCleansedSinceMove(moves);
                } else {
                    adjalign(2);
                }
            }
        }
    }
}

struct monst *
makeRoamingMonsterOfType(pmid, alignment, x, y, peaceful)
int pmid;
aligntyp alignment;
xchar x, y;
boolean peaceful;
{
    register struct monst *roamer;
    register boolean coaligned = (currentAlignmentType() == alignment);

    if (MON_AT(x, y))
        (void) rloc(m_at(x, y), FALSE); /* insurance */

    if (!(roamer = makeMonsterOfType(pmid, x, y, MM_ADJACENTOK | MM_EMIN)))
        return (struct monst *) 0;

    EMIN(roamer)->min_align = alignment;
    EMIN(roamer)->renegade = (coaligned && !peaceful);
    roamer->ispriest = 0;
    roamer->isminion = 1;
    roamer->mtrapseen = ~0; /* traps are known */
    roamer->mpeaceful = peaceful;
    roamer->msleeping = 0;
    set_malign(roamer); /* peaceful may have changed */

    /* MORE TO COME */
    return roamer;
}

void
reset_hostility(roamer)
register struct monst *roamer;
{
    if (!roamer->isminion)
        return;

    int pmid = pmid4mon(roamer);
    if ((pmid != PM_ALIGNED_PRIEST) && (pmid != PM_ANGEL))
    {
        return;
    }

    if (EMIN(roamer)->min_align != currentAlignmentType()) {
        roamer->mpeaceful = roamer->mtame = 0;
        set_malign(roamer);
    }
    newsym(roamer->mx, roamer->my);
}

boolean
in_your_sanctuary(mon, x, y)
struct monst *mon; /* if non-null, <mx,my> overrides <x,y> */
xchar x, y;
{
    register char roomno;
    register struct monst *priest;

    if (mon) {
        if (isMinion(pmid4mon(mon)) || isRiderOfTheApocalypse(pmid4mon(mon)))
            return FALSE;
        x = mon->mx, y = mon->my;
    }
    if (currentAlignmentRecord() <= ALGN_SINNED) /* sinned or worse */
        return FALSE;
    if ((roomno = mostRecentCurrentlyOccupiedTempleID()) == 0
        || roomno != templeLocatedAt(x, y))
        return FALSE;
    if ((priest = findpriest(roomno)) == 0)
        return FALSE;
    return (boolean) (has_shrine(priest) && p_coaligned(priest)
                      && priest->mpeaceful);
}

/* when attacking "priest" in his temple */
void
ghod_hitsu(priest)
struct monst *priest;
{
    int x, y, ax, ay, roomno = (int) mostRecentCurrentlyOccupiedTempleID();
    struct mkroom *troom;

    if (!roomno || !has_shrine(priest))
        return;

    ax = x = EPRI(priest)->shrpos.x;
    ay = y = EPRI(priest)->shrpos.y;
    troom = &rooms[roomno - ROOMOFFSET];

    if ((currentX() == x && currentY() == y) || !linedup(currentX(), currentY(), x, y, 1)) {
        if (IS_DOOR(levl[currentX()][currentY()].typ)) {
            if (currentX() == troom->lx - 1) {
                x = troom->hx;
                y = currentY();
            } else if (currentX() == troom->hx + 1) {
                x = troom->lx;
                y = currentY();
            } else if (currentY() == troom->ly - 1) {
                x = currentX();
                y = troom->hy;
            } else if (currentY() == troom->hy + 1) {
                x = currentX();
                y = troom->ly;
            }
        } else {
            switch (rn2(4)) {
            case 0:
                x = currentX();
                y = troom->ly;
                break;
            case 1:
                x = currentX();
                y = troom->hy;
                break;
            case 2:
                x = troom->lx;
                y = currentY();
                break;
            default:
                x = troom->hx;
                y = currentY();
                break;
            }
        }
        if (! linedup(currentX(), currentY(), x, y, 1))
            return;
    }

    javaString altarDeity = nameOfAlignedDeityFromYourPantheon(a_align(ax, ay));
    switch (rn2(3)) {
    case 0:
        pline("%s roars in anger:  \"Thou shalt suffer!\"",
              altarDeity.c_str);
        break;
    case 1:
        pline("%s voice booms:  \"How darest thou harm my servant!\"",
              s_suffix(altarDeity.c_str));
        break;
    default:
        pline("%s roars:  \"Thou dost profane my shrine!\"",
              altarDeity.c_str);
        break;
    }
    releaseJavaString(altarDeity);

    buzz(-10 - (AD_ELEC - 1), 6, x, y, sgn(tbx),
         sgn(tby)); /* bolt of lightning */
    exercise(A_WIS, FALSE);
}

void
angry_priest()
{
    register struct monst *priest;
    struct rm *lev;

    if ((priest = findpriest(mostRecentCurrentlyOccupiedTempleID())) != 0) {
        struct epri *eprip = EPRI(priest);

        wakeup(priest);
        /*
         * If the altar has been destroyed or converted, let the
         * priest run loose.
         * (When it's just a conversion and there happens to be
         * a fresh corpse nearby, the priest ought to have an
         * opportunity to try converting it back; maybe someday...)
         */
        lev = &levl[eprip->shrpos.x][eprip->shrpos.y];
        if (!IS_ALTAR(lev->typ)
            || ((aligntyp) Amask2align(lev->altarmask & AM_MASK)
                != eprip->shralign)) {
            if (!EMIN(priest))
                newemin(priest);
            priest->ispriest = 0; /* now a roaming minion */
            priest->isminion = 1;
            EMIN(priest)->min_align = eprip->shralign;
            EMIN(priest)->renegade = FALSE;
            /* discard priest's memory of his former shrine;
               if we ever implement the re-conversion mentioned
               above, this will need to be removed */
            free_epri(priest);
        }
    }
}

/*
 * When saving bones, find priests that aren't on their shrine level,
 * and remove them.  This avoids big problems when restoring bones.
 * [Perhaps we should convert them into roamers instead?]
 */
void
clearpriests()
{
    struct monst *mtmp;

    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
        if (DEADMONSTER(mtmp))
            continue;
        if (mtmp->ispriest && !areYouOnLevel(&(EPRI(mtmp)->shrlevel)))
            mongone(mtmp);
    }
}

/* munge priest-specific structure when restoring -dlc */
void
restpriest(mtmp, ghostly)
register struct monst *mtmp;
boolean ghostly;
{
    if (currentDungeonLevel()) {
        if (ghostly)
            assignFromCurrentLevel(&(EPRI(mtmp)->shrlevel));
    }
}

/*priest.c*/
