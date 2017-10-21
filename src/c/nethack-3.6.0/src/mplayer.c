/* NetHack 3.6	mplayer.c	$NHDT-Date: 1432512774 2015/05/25 00:12:54 $  $NHDT-Branch: master $:$NHDT-Revision: 1.19 $ */
/*	Copyright (c) Izchak Miller, 1992.			  */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_DCL const char *NDECL(dev_name);
STATIC_DCL void FDECL(get_mplname, (struct monst *, char *));
STATIC_DCL void FDECL(mk_mplayer_armor, (struct monst *, SHORT_P));

/* These are the names of those who
 * contributed to the development of NetHack 3.2/3.3/3.4/3.6.
 *
 * Keep in alphabetical order within teams.
 * Same first name is entered once within each team.
 */
static const char *developers[] = {
    /* devteam */
    "Dave",    "Dean",   "Eric",    "Izchak", "Janet",    "Jessie",
    "Ken",     "Kevin",  "Michael", "Mike",   "Pat",      "Paul",
    "Steve",   "Timo",   "Warwick",
    /* PC team */
    "Bill",    "Eric",   "Keizo",   "Ken",    "Kevin",    "Michael",
    "Mike",    "Paul",   "Stephen", "Steve",  "Timo",     "Yitzhak",
    /* Amiga team */
    "Andy",    "Gregg",  "Janne",   "Keni",   "Mike",     "Olaf",
    "Richard",
    /* Mac team */
    "Andy",    "Chris",  "Dean",    "Jon",    "Jonathan", "Kevin",
    "Wang",
    /* Atari team */
    "Eric",    "Marvin", "Warwick",
    /* NT team */
    "Alex",    "Dion",   "Michael",
    /* OS/2 team */
    "Helge",   "Ron",    "Timo",
    /* VMS team */
    "Joshua",  "Pat",    ""
};

/* return a randomly chosen developer name */
STATIC_OVL const char *
dev_name()
{
    register int i, m = 0, n = SIZE(developers);
    register struct monst *mtmp;
    register boolean match;

    do {
        match = FALSE;
        i = rn2(n);
        for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
            if (!isMonsterPlayer(pmid4mon(mtmp)))
                continue;
            if (!strncmp(developers[i], (has_mname(mtmp)) ? MNAME(mtmp) : "",
                         strlen(developers[i]))) {
                match = TRUE;
                break;
            }
        }
        m++;
    } while (match && m < 100); /* m for insurance */

    if (match)
        return (const char *) 0;
    return (developers[i]);
}

STATIC_OVL void
get_mplname(mtmp, nam)
register struct monst *mtmp;
char *nam;
{
    boolean fmlkind = isFemale(pmid4mon(mtmp));
    const char *devnam;

    devnam = dev_name();
    if (!devnam)
        Strcpy(nam, fmlkind ? "Eve" : "Adam");
    else if (fmlkind && !!strcmp(devnam, "Janet"))
        Strcpy(nam, rn2(2) ? "Maud" : "Eve");
    else
        Strcpy(nam, devnam);

    if (fmlkind || !strcmp(nam, "Janet"))
        mtmp->female = 1;
    else
        mtmp->female = 0;
    Strcat(nam, " the ");
    javaString rankName = rankOf((int) mtmp->m_lev, pmid4mon(mtmp),
                        (boolean) mtmp->female);
    Strcat(nam, rankName.c_str);
    releaseJavaString(rankName);
}

STATIC_OVL void
mk_mplayer_armor(mon, typ)
struct monst *mon;
short typ;
{
    struct obj *obj;

    if (typ == STRANGE_OBJECT)
        return;
    obj = mksobj(typ, FALSE, FALSE);
    if (!rn2(3))
        obj->oerodeproof = 1;
    if (!rn2(3))
        curse(obj);
    if (!rn2(3))
        bless(obj);
    /* Most players who get to the endgame who have cursed equipment
     * have it because the wizard or other monsters cursed it, so its
     * chances of having plusses is the same as usual....
     */
    obj->spe = rn2(10) ? (rn2(3) ? rn2(5) : rn1(4, 4)) : -rnd(3);
    (void) mpickobj(mon, obj);
}

struct monst *
makeMonsterPlayerOfType(pmid, x, y, special)
int pmid;
xchar x, y;
register boolean special;
{
    register struct monst *mtmp;
    char nam[PL_NSIZ];

    if (!isMonsterPlayer(pmid)) {
        return ((struct monst *) 0);
    }

    if (MON_AT(x, y))
        (void) rloc(m_at(x, y), FALSE); /* insurance */

    if (!areYouInEndgame())
        special = FALSE;

    if ((mtmp = makeMonsterOfType(pmid, x, y, NO_MM_FLAGS)) != 0) {
        short weapon = rn2(2) ? LONG_SWORD : rnd_class(SPEAR, BULLWHIP);
        short armor =
            rnd_class(GRAY_DRAGON_SCALE_MAIL, YELLOW_DRAGON_SCALE_MAIL);
        short cloak = !rn2(8)
                          ? STRANGE_OBJECT
                          : rnd_class(OILSKIN_CLOAK, CLOAK_OF_DISPLACEMENT);
        short helm = !rn2(8) ? STRANGE_OBJECT : rnd_class(ELVEN_LEATHER_HELM,
                                                          HELM_OF_TELEPATHY);
        short shield = !rn2(8)
                           ? STRANGE_OBJECT
                           : rnd_class(ELVEN_SHIELD, SHIELD_OF_REFLECTION);
        int quan;
        struct obj *otmp;

        mtmp->m_lev = (special ? rn1(16, 15) : rnd(16));
        mtmp->mhp = mtmp->mhpmax =
            d((int) mtmp->m_lev, 10) + (special ? (30 + rnd(30)) : 30);
        if (special) {
            get_mplname(mtmp, nam);
            mtmp = christen_monst(mtmp, nam);
            /* that's why they are "stuck" in the endgame :-) */
            (void) mongets(mtmp, FAKE_AMULET_OF_YENDOR);
        }
        mtmp->mpeaceful = 0;
        set_malign(mtmp); /* peaceful may have changed again */

        switch (pmid) {
        case PM_ARCHEOLOGIST:
            if (rn2(2))
                weapon = BULLWHIP;
            break;
        case PM_BARBARIAN:
            if (rn2(2)) {
                weapon = rn2(2) ? TWO_HANDED_SWORD : BATTLE_AXE;
                shield = STRANGE_OBJECT;
            }
            if (rn2(2))
                armor = rnd_class(PLATE_MAIL, CHAIN_MAIL);
            if (helm == HELM_OF_BRILLIANCE)
                helm = STRANGE_OBJECT;
            break;
        case PM_CAVEMAN:
        case PM_CAVEWOMAN:
            if (rn2(4))
                weapon = MACE;
            else if (rn2(2))
                weapon = CLUB;
            if (helm == HELM_OF_BRILLIANCE)
                helm = STRANGE_OBJECT;
            break;
        case PM_HEALER:
            if (rn2(4))
                weapon = QUARTERSTAFF;
            else if (rn2(2))
                weapon = rn2(2) ? UNICORN_HORN : SCALPEL;
            if (rn2(4))
                helm = rn2(2) ? HELM_OF_BRILLIANCE : HELM_OF_TELEPATHY;
            if (rn2(2))
                shield = STRANGE_OBJECT;
            break;
        case PM_KNIGHT:
            if (rn2(4))
                weapon = LONG_SWORD;
            if (rn2(2))
                armor = rnd_class(PLATE_MAIL, CHAIN_MAIL);
            break;
        case PM_MONK:
            weapon = !rn2(3) ? SHURIKEN : STRANGE_OBJECT;
            armor = STRANGE_OBJECT;
            cloak = ROBE;
            if (rn2(2))
                shield = STRANGE_OBJECT;
            break;
        case PM_PRIEST:
        case PM_PRIESTESS:
            if (rn2(2))
                weapon = MACE;
            if (rn2(2))
                armor = rnd_class(PLATE_MAIL, CHAIN_MAIL);
            if (rn2(4))
                cloak = ROBE;
            if (rn2(4))
                helm = rn2(2) ? HELM_OF_BRILLIANCE : HELM_OF_TELEPATHY;
            if (rn2(2))
                shield = STRANGE_OBJECT;
            break;
        case PM_RANGER:
            if (rn2(2))
                weapon = ELVEN_DAGGER;
            break;
        case PM_ROGUE:
            if (rn2(2))
                weapon = rn2(2) ? SHORT_SWORD : ORCISH_DAGGER;
            break;
        case PM_SAMURAI:
            if (rn2(2))
                weapon = KATANA;
            break;
        case PM_TOURIST:
            /* Defaults are just fine */
            break;
        case PM_VALKYRIE:
            if (rn2(2))
                weapon = WAR_HAMMER;
            if (rn2(2))
                armor = rnd_class(PLATE_MAIL, CHAIN_MAIL);
            break;
        case PM_WIZARD:
            if (rn2(4))
                weapon = rn2(2) ? QUARTERSTAFF : ATHAME;
            if (rn2(2)) {
                armor = rn2(2) ? BLACK_DRAGON_SCALE_MAIL
                               : SILVER_DRAGON_SCALE_MAIL;
                cloak = CLOAK_OF_MAGIC_RESISTANCE;
            }
            if (rn2(4))
                helm = HELM_OF_BRILLIANCE;
            shield = STRANGE_OBJECT;
            break;
        default:
            impossible("bad mplayer monster");
            weapon = 0;
            break;
        }

        if (weapon != STRANGE_OBJECT) {
            otmp = mksobj(weapon, TRUE, FALSE);
            otmp->spe = (special ? rn1(5, 4) : rn2(4));
            if (!rn2(3))
                otmp->oerodeproof = 1;
            else if (!rn2(2))
                otmp->greased = 1;
            if (special && rn2(2))
                otmp = mk_artifact(otmp, A_NONE);
            /* usually increase stack size if stackable weapon */
            if (objects[otmp->otyp].oc_merge && !otmp->oartifact)
                otmp->quan += (long) rn2(is_spear(otmp) ? 4 : 8);
            /* mplayers knew better than to overenchant Magicbane */
            if (otmp->oartifact == ART_MAGICBANE)
                otmp->spe = rnd(4);
            (void) mpickobj(mtmp, otmp);
        }

        if (special) {
            if (!rn2(10))
                (void) mongets(mtmp, rn2(3) ? LUCKSTONE : LOADSTONE);
            mk_mplayer_armor(mtmp, armor);
            mk_mplayer_armor(mtmp, cloak);
            mk_mplayer_armor(mtmp, helm);
            mk_mplayer_armor(mtmp, shield);
            if (rn2(8))
                mk_mplayer_armor(
                    mtmp, rnd_class(LEATHER_GLOVES, GAUNTLETS_OF_DEXTERITY));
            if (rn2(8))
                mk_mplayer_armor(mtmp,
                                 rnd_class(LOW_BOOTS, LEVITATION_BOOTS));
            m_dowear(mtmp, TRUE);

            quan = rn2(3) ? rn2(3) : rn2(16);
            while (quan--)
                (void) mongets(mtmp, rnd_class(DILITHIUM_CRYSTAL, JADE));
            /* To get the gold "right" would mean a player can double his */
            /* gold supply by killing one mplayer.  Not good. */
            mkmonmoney(mtmp, rn2(1000));
            quan = rn2(10);
            while (quan--)
                (void) mpickobj(mtmp, mkobj(RANDOM_CLASS, FALSE));
        }
        quan = rnd(3);
        while (quan--)
            (void) mongets(mtmp, rnd_offensive_item(mtmp));
        quan = rnd(3);
        while (quan--)
            (void) mongets(mtmp, rnd_defensive_item(mtmp));
        quan = rnd(3);
        while (quan--)
            (void) mongets(mtmp, rnd_misc_item(mtmp));
    }

    return (mtmp);
}

/* create the indicated number (num) of monster-players,
 * randomly chosen, and in randomly chosen (free) locations
 * on the level.  If "special", the size of num should not
 * be bigger than the number of _non-repeated_ names in the
 * developers array, otherwise a bunch of Adams and Eves will
 * fill up the overflow.
 */
void
create_mplayers(num, special)
register int num;
boolean special;
{
    int pm, x, y;

    while (num) {
        int tryct = 0;

        /* roll for character class */
        pm = PM_ARCHEOLOGIST + rn2(PM_WIZARD - PM_ARCHEOLOGIST + 1);

        /* roll for an available location */
        do {
            x = rn1(COLNO - 4, 2);
            y = rnd(ROWNO - 2);
        } while (!goodPositionForMonsterOfType(x, y, pm, 0) && tryct++ <= 50);

        /* if pos not found in 50 tries, don't bother to continue */
        if (tryct > 50)
            return;

        (void) makeMonsterPlayerOfType(pm, (xchar) x, (xchar) y, special);
        num--;
    }
}

void
mplayer_talk(mtmp)
register struct monst *mtmp;
{
    static const char *same_class_msg[3] =
        {
          "I can't win, and neither will you!", "You don't deserve to win!",
          "Mine should be the honor, not yours!",
        },
                      *other_class_msg[3] = {
                          "The low-life wants to talk, eh?", "Fight, scum!",
                          "Here is what I have to say!",
                      };

    if (mtmp->mpeaceful)
        return; /* will drop to humanoid talk */

    int pmid = pmid4mon(mtmp);

    pline("Talk? -- %s", (yourRoleHasMalePMID(pmid) || yourRoleHasFemalePMID(pmid))
                             ? same_class_msg[rn2(3)]
                             : other_class_msg[rn2(3)]);
}

/*mplayer.c*/
