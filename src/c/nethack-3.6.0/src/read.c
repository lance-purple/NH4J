/* NetHack 3.6	read.c	$NHDT-Date: 1448862378 2015/11/30 05:46:18 $  $NHDT-Branch: master $:$NHDT-Revision: 1.125 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

static boolean Your_Own_Role(int pmid)
{
    return (pmid == malePMIDForYourRole())
     || (yourRoleHasFemalePMID() && (pmid == femalePMIDForYourRole()));
}

#define Your_Own_Race(mndx)  \
    ((mndx) == yourSpeciesPMIDAsMale() \
     || (yourSpeciesPMIDHasFemaleVersion() && (mndx) == yourSpeciesPMIDAsFemale()))

boolean known;

static NEARDATA const char readable[] = { ALL_CLASSES, SCROLL_CLASS,
                                          SPBOOK_CLASS, 0 };
static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };

STATIC_DCL boolean FDECL(learnscrolltyp, (SHORT_P));
STATIC_DCL char * FDECL(erode_obj_text, (struct obj *, char *));
STATIC_DCL void NDECL(do_class_genocide);
STATIC_DCL void FDECL(stripspe, (struct obj *));
STATIC_DCL void FDECL(p_glow1, (struct obj *));
STATIC_DCL void FDECL(p_glow2, (struct obj *, const char *));
STATIC_DCL void FDECL(randomize, (int *, int));
STATIC_DCL void FDECL(forget_single_object, (int));
STATIC_DCL void FDECL(forget, (int));
STATIC_DCL int FDECL(maybe_tame, (struct monst *, struct obj *));
STATIC_DCL boolean FDECL(is_valid_stinking_cloud_pos, (int, int, BOOLEAN_P));
STATIC_DCL void FDECL(display_stinking_cloud_positions, (int));
STATIC_PTR void FDECL(set_lit, (int, int, genericptr));

STATIC_OVL boolean
learnscrolltyp(scrolltyp)
short scrolltyp;
{
    if (!objects[scrolltyp].oc_name_known) {
        makeknown(scrolltyp);
        more_experienced(0, 10);
        return TRUE;
    } else
        return FALSE;
}

/* also called from teleport.c for scroll of teleportation */
void
learnscroll(sobj)
struct obj *sobj;
{
    /* it's implied that sobj->dknown is set;
       we couldn't be reading this scroll otherwise */
    if (sobj->oclass != SPBOOK_CLASS)
        (void) learnscrolltyp(sobj->otyp);
}

char *
erode_obj_text(otmp, buf)
struct obj *otmp;
char *buf;
{
    int erosion = greatest_erosion(otmp);
    if (erosion)
        wipeout_text(buf, (int) (strlen(buf) * erosion / (2 * MAX_ERODE)),
                     otmp->o_id ^ (unsigned) ubirthday);
    return buf;
}

char *
tshirt_text(tshirt, buf)
struct obj *tshirt;
char *buf;
{
    static const char *shirt_msgs[] = {
        /* Scott Bigham */
      "I explored the Dungeons of Doom and all I got was this lousy T-shirt!",
        "Is that Mjollnir in your pocket or are you just happy to see me?",
      "It's not the size of your sword, it's how #enhance'd you are with it.",
        "Madame Elvira's House O' Succubi Lifetime Customer",
        "Madame Elvira's House O' Succubi Employee of the Month",
        "Ludios Vault Guards Do It In Small, Dark Rooms",
        "Yendor Military Soldiers Do It In Large Groups",
        "I survived Yendor Military Boot Camp",
        "Ludios Accounting School Intra-Mural Lacrosse Team",
        "Oracle(TM) Fountains 10th Annual Wet T-Shirt Contest",
        "Hey, black dragon!  Disintegrate THIS!",
        "I'm With Stupid -->",
        "Don't blame me, I voted for Izchak!",
        "Don't Panic", /* HHGTTG */
        "Furinkan High School Athletic Dept.",                /* Ranma 1/2 */
        "Hel-LOOO, Nurse!",                                   /* Animaniacs */
        "=^.^=",
        "100% goblin hair - do not wash",
        "Aberzombie and Fitch",
        "cK -- Cockatrice touches the Kop",
        "Don't ask me, I only adventure here",
        "Down with pants!",
        "d, your dog or a killer?",
        "FREE PUG AND NEWT!",
        "Go team ant!",
        "Got newt?",
        "Hello, my darlings!", /* Charlie Drake */
        "Hey! Nymphs! Steal This T-Shirt!",
        "I <3 Dungeon of Doom",
        "I <3 Maud",
        "I am a Valkyrie. If you see me running, try to keep up.",
        "I am not a pack rat - I am a collector",
        "I bounced off a rubber tree",         /* Monkey Island */
        "Plunder Island Brimstone Beach Club", /* Monkey Island */
        "If you can read this, I can hit you with my polearm",
        "I'm confused!",
        "I scored with the princess",
        "I want to live forever or die in the attempt.",
        "Lichen Park",
        "LOST IN THOUGHT - please send search party",
        "Meat is Mordor",
        "Minetown Better Business Bureau",
        "Minetown Watch",
 "Ms. Palm's House of Negotiable Affection -- A Very Reputable House Of Disrepute",
        "Protection Racketeer",
        "Real men love Crom",
        "Somebody stole my Mojo!",
        "The Hellhound Gang",
        "The Werewolves",
        "They Might Be Storm Giants",
        "Weapons don't kill people, I kill people",
        "White Zombie",
        "You're killing me!",
        "Anhur State University - Home of the Fighting Fire Ants!",
        "FREE HUGS",
        "Serial Ascender",
        "Real men are valkyries",
        "Young Men's Cavedigging Association",
        "Occupy Fort Ludios",
        "I couldn't afford this T-shirt so I stole it!",
        "Mind flayers suck",
        "I'm not wearing any pants",
        "Down with the living!",
        "Pudding farmer",
        "Vegetarian",
        "Hello, I'm War!",
    };

    Strcpy(buf, shirt_msgs[tshirt->o_id % SIZE(shirt_msgs)]);
    return erode_obj_text(tshirt, buf);
}

char *
apron_text(apron, buf)
struct obj *apron;
char *buf;
{
    static const char *apron_msgs[] = {
        "Kiss the cook",
        "I'm making SCIENCE!",
        "Don't mess with the chef",
        "Don't make me poison you",
        "Gehennom's Kitchen",
        "Rat: The other white meat",
        "If you can't stand the heat, get out of Gehennom!",
        "If we weren't meant to eat animals, why are they made out of meat?",
        "If you don't like the food, I'll stab you",
    };

    Strcpy(buf, apron_msgs[apron->o_id % SIZE(apron_msgs)]);
    return erode_obj_text(apron, buf);
}

int
doread()
{
    register struct obj *scroll;
    boolean confused, nodisappear;

    known = FALSE;
    if (check_capacity((char *) 0))
        return 0;
    scroll = getobj(readable, "read");
    if (!scroll)
        return 0;

    /* outrumor has its own blindness check */
    if (scroll->otyp == FORTUNE_COOKIE) {
        if (flags.verbose)
            You("break up the cookie and throw away the pieces.");
        outrumor(bcsign(scroll), BY_COOKIE);
        if (youCanSee())
            incrementLiteracyCount(1);
        useup(scroll);
        return 1;
    } else if (scroll->otyp == T_SHIRT || scroll->otyp == ALCHEMY_SMOCK) {
        char buf[BUFSZ];
        if (youCannotSee()) {
            You_cant("feel any Braille writing.");
            return 0;
        }
        /* can't read shirt worn under suit (under cloak is ok though) */
        if (scroll->otyp == T_SHIRT && uarm && scroll == uarmu) {
            pline("%s shirt is obscured by %s%s.",
                  scroll->unpaid ? "That" : "Your", shk_your(buf, uarm),
                  suit_simple_name(uarm));
            return 0;
        }
        incrementLiteracyCount(1);
        if (flags.verbose)
            pline("It reads:");
        pline("\"%s\"", (scroll->otyp == T_SHIRT) ? tshirt_text(scroll, buf)
                                                  : apron_text(scroll, buf));
        return 1;
    } else if (scroll->otyp == CREDIT_CARD) {
        static const char *card_msgs[] = {
            "Leprechaun Gold Tru$t - Shamrock Card",
            "Magic Memory Vault Charge Card", "Larn National Bank", /* Larn */
            "First Bank of Omega",               /* Omega */
            "Bank of Zork - Frobozz Magic Card", /* Zork */
            "Ankh-Morpork Merchant's Guild Barter Card",
            "Ankh-Morpork Thieves' Guild Unlimited Transaction Card",
            "Ransmannsby Moneylenders Association",
            "Bank of Gehennom - 99% Interest Card",
            "Yendorian Express - Copper Card",
            "Yendorian Express - Silver Card",
            "Yendorian Express - Gold Card",
            "Yendorian Express - Mithril Card",
            "Yendorian Express - Platinum Card", /* must be last */
        };

        if (youCannotSee()) {
            You("feel the embossed numbers:");
        } else {
            if (flags.verbose)
                pline("It reads:");
            pline("\"%s\"",
                  scroll->oartifact
                      ? card_msgs[SIZE(card_msgs) - 1]
                      : card_msgs[scroll->o_id % (SIZE(card_msgs) - 1)]);
        }
        /* Make a credit card number */
        pline("\"%d0%d %d%d1 0%d%d0\"", ((scroll->o_id % 89) + 10),
              (scroll->o_id % 4), (((scroll->o_id * 499) % 899999) + 100000),
              (scroll->o_id % 10), (!(scroll->o_id % 3)),
              ((scroll->o_id * 7) % 10));
        incrementLiteracyCount(1);
        return 1;
    } else if (scroll->otyp == CAN_OF_GREASE) {
        pline("This %s has no label.", singular(scroll, xname));
        return 0;
    } else if (scroll->otyp == MAGIC_MARKER) {
        if (youCannotSee()) {
            You_cant("feel any Braille writing.");
            return 0;
        }
        if (flags.verbose)
            pline("It reads:");
        pline("\"Magic Marker(TM) Red Ink Marker Pen. Water Soluble.\"");
        incrementLiteracyCount(1);
        return 1;
    } else if (scroll->oclass == COIN_CLASS) {
        if (youCannotSee())
            You("feel the embossed words:");
        else if (flags.verbose)
            You("read:");
        pline("\"1 Zorkmid. 857 GUE. In Frobs We Trust.\"");
        incrementLiteracyCount(1);
        return 1;
    } else if (scroll->oartifact == ART_ORB_OF_FATE) {
        if (youCannotSee())
            You("feel the engraved signature:");
        else
            pline("It is signed:");
        pline("\"Odin.\"");
        incrementLiteracyCount(1);
        return 1;
    } else if (scroll->otyp == CANDY_BAR) {
        static const char *wrapper_msgs[] = {
            "Apollo",       /* Lost */
            "Moon Crunchy", /* South Park */
            "Snacky Cake",    "Chocolate Nuggie", "The Small Bar",
            "Crispy Yum Yum", "Nilla Crunchie",   "Berry Bar",
            "Choco Nummer",   "Om-nom", /* Cat Macro */
            "Fruity Oaty",              /* Serenity */
            "Wonka Bar" /* Charlie and the Chocolate Factory */
        };

        if (youCannotSee()) {
            You_cant("feel any Braille writing.");
            return 0;
        }
        pline("The wrapper reads: \"%s\"",
              wrapper_msgs[scroll->o_id % SIZE(wrapper_msgs)]);
        incrementLiteracyCount(1);
        return 1;
    } else if (scroll->oclass != SCROLL_CLASS
               && scroll->oclass != SPBOOK_CLASS) {
        pline(silly_thing_to, "read");
        return 0;
    } else if (youCannotSee() && (scroll->otyp != SPE_BOOK_OF_THE_DEAD)) {
        const char *what = 0;
        if (scroll->oclass == SPBOOK_CLASS)
            what = "mystic runes";
        else if (!scroll->dknown)
            what = "formula on the scroll";
        if (what) {
            pline("Being blind, you cannot read the %s.", what);
            return 0;
        }
    }

    confused = youAreConfused();
#ifdef MAIL
    if (scroll->otyp == SCR_MAIL) {
        confused = FALSE; /* override */
        /* reading mail is a convenience for the player and takes
           place outside the game, so shouldn't affect gameplay;
           on the other hand, it starts by explicitly making the
           hero actively read something, which is pretty hard
           to simply ignore; as a compromise, if the player has
           maintained illiterate conduct so far, and this mail
           scroll didn't come from bones, ask for confirmation */
        if (!u.uconduct.literate) {
            if (!scroll->spe && yn(
             "Reading mail will violate \"illiterate\" conduct.  Read anyway?"
                                   ) != 'y')
                return 0;
        }
    }
#endif

    /* Actions required to win the game aren't counted towards conduct */
    /* Novel conduct is handled in read_tribute so exclude it too*/
    if (scroll->otyp != SPE_BOOK_OF_THE_DEAD
        && scroll->otyp != SPE_BLANK_PAPER && scroll->otyp != SCR_BLANK_PAPER
        && scroll->otyp != SPE_NOVEL)
        incrementLiteracyCount(1);

    if (scroll->oclass == SPBOOK_CLASS) {
        return study_book(scroll);
    }
    scroll->in_use = TRUE; /* scroll, not spellbook, now being read */
    if (scroll->otyp != SCR_BLANK_PAPER) {
        /* a few scroll feedback messages describe something happening
           to the scroll itself, so avoid "it disappears" for those */
        nodisappear = (scroll->otyp == SCR_FIRE
                       || (scroll->otyp == SCR_REMOVE_CURSE
                           && scroll->cursed));
        if (youCannotSee())
            pline(nodisappear
                      ? "You %s the formula on the scroll."
                      : "As you %s the formula on it, the scroll disappears.",
                  isSilent(pmid4you()) ? "cogitate" : "pronounce");
        else
            pline(nodisappear ? "You read the scroll."
                              : "As you read the scroll, it disappears.");
        if (confused) {
            if (youAreHallucinating())
                pline("Being so trippy, you screw up...");
            else
                pline("Being confused, you %s the magic words...",
                      isSilent(pmid4you()) ? "misunderstand"
                                               : "mispronounce");
        }
    }
    if (!seffects(scroll)) {
        if (!objects[scroll->otyp].oc_name_known) {
            if (known)
                learnscroll(scroll);
            else if (!objects[scroll->otyp].oc_uname)
                docall(scroll);
        }
        scroll->in_use = FALSE;
        if (scroll->otyp != SCR_BLANK_PAPER)
            useup(scroll);
    }
    return 1;
}

STATIC_OVL void
stripspe(obj)
register struct obj *obj;
{
    if (obj->blessed || obj->spe <= 0) {
        pline1(nothing_happens);
    } else {
        /* order matters: message, shop handling, actual transformation */
        pline("%s briefly.", Yobjnam2(obj, "vibrate"));
        costly_alteration(obj, COST_UNCHRG);
        obj->spe = 0;
        if (obj->otyp == OIL_LAMP || obj->otyp == BRASS_LANTERN)
            obj->age = 0;
    }
}

STATIC_OVL void
p_glow1(otmp)
register struct obj *otmp;
{
    pline("%s briefly.", Yobjnam2(otmp, youCannotSee() ? "vibrate" : "glow"));
}

STATIC_OVL void
p_glow2(otmp, color)
register struct obj *otmp;
register const char *color;
{
    pline("%s%s%s for a moment.", Yobjnam2(otmp, youCannotSee() ? "vibrate" : "glow"),
          youCannotSee() ? "" : " ", youCannotSee() ? "" : hcolor(color));
}

/* Is the object chargeable?  For purposes of inventory display; it is
   possible to be able to charge things for which this returns FALSE. */
boolean
is_chargeable(obj)
struct obj *obj;
{
    if (obj->oclass == WAND_CLASS)
        return TRUE;
    /* known && !oc_name_known is possible after amnesia/mind flayer */
    if (obj->oclass == RING_CLASS)
        return (boolean) (objects[obj->otyp].oc_charged
                          && (obj->known
                              || (obj->dknown
                                  && objects[obj->otyp].oc_name_known)));
    if (is_weptool(obj)) /* specific check before general tools */
        return FALSE;
    if (obj->oclass == TOOL_CLASS)
        return (boolean) objects[obj->otyp].oc_charged;
    return FALSE; /* why are weapons/armor considered charged anyway? */
}

/* recharge an object; curse_bless is -1 if the recharging implement
   was cursed, +1 if blessed, 0 otherwise. */
void
recharge(obj, curse_bless)
struct obj *obj;
int curse_bless;
{
    register int n;
    boolean is_cursed, is_blessed;

    is_cursed = curse_bless < 0;
    is_blessed = curse_bless > 0;

    if (obj->oclass == WAND_CLASS) {
        int lim = (obj->otyp == WAN_WISHING)
                      ? 3
                      : (objects[obj->otyp].oc_dir != NODIR) ? 8 : 15;

        /* undo any prior cancellation, even when is_cursed */
        if (obj->spe == -1)
            obj->spe = 0;

        /*
         * Recharging might cause wands to explode.
         *      v = number of previous recharges
         *            v = percentage chance to explode on this attempt
         *                    v = cumulative odds for exploding
         *      0 :   0       0
         *      1 :   0.29    0.29
         *      2 :   2.33    2.62
         *      3 :   7.87   10.28
         *      4 :  18.66   27.02
         *      5 :  36.44   53.62
         *      6 :  62.97   82.83
         *      7 : 100     100
         */
        n = (int) obj->recharged;
        if (n > 0 && (obj->otyp == WAN_WISHING
                      || (n * n * n > rn2(7 * 7 * 7)))) { /* recharge_limit */
            wand_explode(obj, rnd(lim));
            return;
        }
        /* didn't explode, so increment the recharge count */
        obj->recharged = (unsigned) (n + 1);

        /* now handle the actual recharging */
        if (is_cursed) {
            stripspe(obj);
        } else {
            n = (lim == 3) ? 3 : rn1(5, lim + 1 - 5);
            if (!is_blessed)
                n = rnd(n);

            if (obj->spe < n)
                obj->spe = n;
            else
                obj->spe++;
            if (obj->otyp == WAN_WISHING && obj->spe > 3) {
                wand_explode(obj, 1);
                return;
            }
            if (obj->spe >= lim)
                p_glow2(obj, NH_BLUE);
            else
                p_glow1(obj);
#if 0 /*[shop price doesn't vary by charge count]*/
            /* update shop bill to reflect new higher price */
            if (obj->unpaid)
                alter_cost(obj, 0L);
#endif
        }

    } else if (obj->oclass == RING_CLASS && objects[obj->otyp].oc_charged) {
        /* charging does not affect ring's curse/bless status */
        int s = is_blessed ? rnd(3) : is_cursed ? -rnd(2) : 1;
        boolean is_on = (obj == uleft || obj == uright);

        /* destruction depends on current state, not adjustment */
        if (obj->spe > rn2(7) || obj->spe <= -5) {
            pline("%s momentarily, then %s!", Yobjnam2(obj, "pulsate"),
                  otense(obj, "explode"));
            if (is_on)
                Ring_gone(obj);
            s = rnd(3 * abs(obj->spe)); /* amount of damage */
            useup(obj);
            losehp(Maybe_Half_Phys(s), "exploding ring", KILLED_BY_AN);
        } else {
            long mask = is_on ? (obj == uleft ? LEFT_RING : RIGHT_RING) : 0L;

            pline("%s spins %sclockwise for a moment.", Yname2(obj),
                  s < 0 ? "counter" : "");
            if (s < 0)
                costly_alteration(obj, COST_DECHNT);
            /* cause attributes and/or properties to be updated */
            if (is_on)
                Ring_off(obj);
            obj->spe += s; /* update the ring while it's off */
            if (is_on)
                setworn(obj, mask), Ring_on(obj);
            /* oartifact: if a touch-sensitive artifact ring is
               ever created the above will need to be revised  */
            /* update shop bill to reflect new higher price */
            if (s > 0 && obj->unpaid)
                alter_cost(obj, 0L);
        }

    } else if (obj->oclass == TOOL_CLASS) {
        int rechrg = (int) obj->recharged;

        if (objects[obj->otyp].oc_charged) {
            /* tools don't have a limit, but the counter used does */
            if (rechrg < 7) /* recharge_limit */
                obj->recharged++;
        }
        switch (obj->otyp) {
        case BELL_OF_OPENING:
            if (is_cursed)
                stripspe(obj);
            else if (is_blessed)
                obj->spe += rnd(3);
            else
                obj->spe += 1;
            if (obj->spe > 5)
                obj->spe = 5;
            break;
        case MAGIC_MARKER:
        case TINNING_KIT:
        case EXPENSIVE_CAMERA:
            if (is_cursed)
                stripspe(obj);
            else if (rechrg
                     && obj->otyp
                            == MAGIC_MARKER) { /* previously recharged */
                obj->recharged = 1; /* override increment done above */
                if (obj->spe < 3)
                    Your("marker seems permanently dried out.");
                else
                    pline1(nothing_happens);
            } else if (is_blessed) {
                n = rn1(16, 15); /* 15..30 */
                if (obj->spe + n <= 50)
                    obj->spe = 50;
                else if (obj->spe + n <= 75)
                    obj->spe = 75;
                else {
                    int chrg = (int) obj->spe;
                    if ((chrg + n) > 127)
                        obj->spe = 127;
                    else
                        obj->spe += n;
                }
                p_glow2(obj, NH_BLUE);
            } else {
                n = rn1(11, 10); /* 10..20 */
                if (obj->spe + n <= 50)
                    obj->spe = 50;
                else {
                    int chrg = (int) obj->spe;
                    if ((chrg + n) > 127)
                        obj->spe = 127;
                    else
                        obj->spe += n;
                }
                p_glow2(obj, NH_WHITE);
            }
            break;
        case OIL_LAMP:
        case BRASS_LANTERN:
            if (is_cursed) {
                stripspe(obj);
                if (obj->lamplit) {
                    if (youCanSee())
                        pline("%s out!", Tobjnam(obj, "go"));
                    end_burn(obj, TRUE);
                }
            } else if (is_blessed) {
                obj->spe = 1;
                obj->age = 1500;
                p_glow2(obj, NH_BLUE);
            } else {
                obj->spe = 1;
                obj->age += 750;
                if (obj->age > 1500)
                    obj->age = 1500;
                p_glow1(obj);
            }
            break;
        case CRYSTAL_BALL:
            if (is_cursed) {
                stripspe(obj);
            } else if (is_blessed) {
                obj->spe = 6;
                p_glow2(obj, NH_BLUE);
            } else {
                if (obj->spe < 5) {
                    obj->spe++;
                    p_glow1(obj);
                } else
                    pline1(nothing_happens);
            }
            break;
        case HORN_OF_PLENTY:
        case BAG_OF_TRICKS:
        case CAN_OF_GREASE:
            if (is_cursed) {
                stripspe(obj);
            } else if (is_blessed) {
                if (obj->spe <= 10)
                    obj->spe += rn1(10, 6);
                else
                    obj->spe += rn1(5, 6);
                if (obj->spe > 50)
                    obj->spe = 50;
                p_glow2(obj, NH_BLUE);
            } else {
                obj->spe += rnd(5);
                if (obj->spe > 50)
                    obj->spe = 50;
                p_glow1(obj);
            }
            break;
        case MAGIC_FLUTE:
        case MAGIC_HARP:
        case FROST_HORN:
        case FIRE_HORN:
        case DRUM_OF_EARTHQUAKE:
            if (is_cursed) {
                stripspe(obj);
            } else if (is_blessed) {
                obj->spe += d(2, 4);
                if (obj->spe > 20)
                    obj->spe = 20;
                p_glow2(obj, NH_BLUE);
            } else {
                obj->spe += rnd(4);
                if (obj->spe > 20)
                    obj->spe = 20;
                p_glow1(obj);
            }
            break;
        default:
            goto not_chargable;
            /*NOTREACHED*/
            break;
        } /* switch */

    } else {
    not_chargable:
        You("have a feeling of loss.");
    }
}

/* Forget known information about this object type. */
STATIC_OVL void
forget_single_object(obj_id)
int obj_id;
{
    objects[obj_id].oc_name_known = 0;
    objects[obj_id].oc_pre_discovered = 0; /* a discovery when relearned */
    if (objects[obj_id].oc_uname) {
        free((genericptr_t) objects[obj_id].oc_uname);
        objects[obj_id].oc_uname = 0;
    }
    undiscover_object(obj_id); /* after clearing oc_name_known */

    /* clear & free object names from matching inventory items too? */
}

#if 0 /* here if anyone wants it.... */
/* Forget everything known about a particular object class. */
STATIC_OVL void
forget_objclass(oclass)
int oclass;
{
    int i;

    for (i = bases[oclass];
         i < NUM_OBJECTS && objects[i].oc_class == oclass; i++)
        forget_single_object(i);
}
#endif

/* randomize the given list of numbers  0 <= i < count */
STATIC_OVL void
randomize(indices, count)
int *indices;
int count;
{
    int i, iswap, temp;

    for (i = count - 1; i > 0; i--) {
        if ((iswap = rn2(i + 1)) == i)
            continue;
        temp = indices[i];
        indices[i] = indices[iswap];
        indices[iswap] = temp;
    }
}

/* Forget % of known objects. */
void
forget_objects(percent)
int percent;
{
    int i, count;
    int indices[NUM_OBJECTS];

    if (percent == 0)
        return;
    if (percent <= 0 || percent > 100) {
        impossible("forget_objects: bad percent %d", percent);
        return;
    }

    indices[0] = 0; /* lint suppression */
    for (count = 0, i = 1; i < NUM_OBJECTS; i++)
        if (OBJ_DESCR(objects[i])
            && (objects[i].oc_name_known || objects[i].oc_uname))
            indices[count++] = i;

    if (count > 0) {
        randomize(indices, count);

        /* forget first % of randomized indices */
        count = ((count * percent) + rn2(100)) / 100;
        for (i = 0; i < count; i++)
            forget_single_object(indices[i]);
    }
}

/* Forget some or all of map (depends on parameters). */
void
forget_map(howmuch)
int howmuch;
{
    register int zx, zy;

    if (Sokoban)
        return;

    known = TRUE;
    for (zx = 0; zx < COLNO; zx++)
        for (zy = 0; zy < ROWNO; zy++)
            if (howmuch & ALL_MAP || rn2(7)) {
                /* Zonk all memory of this location. */
                levl[zx][zy].seenv = 0;
                levl[zx][zy].waslit = 0;
                levl[zx][zy].glyph = cmap_to_glyph(S_stone);
                lastseentyp[zx][zy] = STONE;
            }
    /* forget overview data for this level */
    forget_mapseen(currentLevelLedgerNum());
}

/* Forget all traps on the level. */
void
forget_traps()
{
    register struct trap *trap;

    /* forget all traps (except the one the hero is in :-) */
    for (trap = ftrap; trap; trap = trap->ntrap)
        if ((trap->tx != currentX() || trap->ty != currentY()) && (trap->ttyp != HOLE))
            trap->tseen = 0;
}

/*
 * Forget given % of all levels that the hero has visited and not forgotten,
 * except this one.
 */
void
forget_levels(percent)
int percent;
{
    int i, count;
    xchar maxl, this_lev;
    int indices[MAXLINFO];

    if (percent == 0)
        return;

    if (percent <= 0 || percent > 100) {
        impossible("forget_levels: bad percent %d", percent);
        return;
    }

    this_lev = currentLevelLedgerNum();
    maxl = maxledgerno();

    /* count & save indices of non-forgotten visited levels */
    /* Sokoban levels are pre-mapped for the player, and should stay
     * so, or they become nearly impossible to solve.  But try to
     * shift the forgetting elsewhere by fiddling with percent
     * instead of forgetting fewer levels.
     */
    indices[0] = 0; /* lint suppression */
    for (count = 0, i = 0; i <= maxl; i++)
        if ((level_info[i].flags & VISITED)
            && !(level_info[i].flags & FORGOTTEN) && i != this_lev) {
            if (ledger_to_dnum(i) == sokoban_dnum)
                percent += 2;
            else
                indices[count++] = i;
        }

    if (percent > 100)
        percent = 100;

    if (count > 0) {
        randomize(indices, count);

        /* forget first % of randomized indices */
        count = ((count * percent) + 50) / 100;
        for (i = 0; i < count; i++) {
            level_info[indices[i]].flags |= FORGOTTEN;
            forget_mapseen(indices[i]);
        }
    }
}

/*
 * Forget some things (e.g. after reading a scroll of amnesia).  When called,
 * the following are always forgotten:
 *      - felt ball & chain
 *      - traps
 *      - part (6 out of 7) of the map
 *
 * Other things are subject to flags:
 *      howmuch & ALL_MAP       = forget whole map
 *      howmuch & ALL_SPELLS    = forget all spells
 */
STATIC_OVL void
forget(howmuch)
int howmuch;
{
    if (youAreBeingPunished()) {
        setFeltBall(FALSE);
        setFeltChain(FALSE);
    }

    forget_map(howmuch);
    forget_traps();

    /* 1 in 3 chance of forgetting some levels */
    if (!rn2(3))
        forget_levels(rn2(25));

    /* 1 in 3 chance of forgetting some objects */
    if (!rn2(3))
        forget_objects(rn2(25));

    if (howmuch & ALL_SPELLS)
        losespells();
    /*
     * Make sure that what was seen is restored correctly.  To do this,
     * we need to go blind for an instant --- turn off the display,
     * then restart it.  All this work is needed to correctly handle
     * walls which are stone on one side and wall on the other.  Turning
     * off the seen bits above will make the wall revert to stone,  but
     * there are cases where we don't want this to happen.  The easiest
     * thing to do is to run it through the vision system again, which
     * is always correct.
     */
    docrt(); /* this correctly will reset vision */
}

/* monster is hit by scroll of taming's effect */
STATIC_OVL int
maybe_tame(mtmp, sobj)
struct monst *mtmp;
struct obj *sobj;
{
    int was_tame = mtmp->mtame;
    unsigned was_peaceful = mtmp->mpeaceful;

    if (sobj->cursed) {
        setmangry(mtmp);
        if (was_peaceful && !mtmp->mpeaceful)
            return -1;
    } else {
        if (mtmp->isshk)
            make_happy_shk(mtmp, FALSE);
        else if (!resist(mtmp, sobj->oclass, 0, NOTELL))
            (void) tamedog(mtmp, (struct obj *) 0);
        if ((!was_peaceful && mtmp->mpeaceful) || (!was_tame && mtmp->mtame))
            return 1;
    }
    return 0;
}

boolean
is_valid_stinking_cloud_pos(x, y, showmsg)
int x, y;
boolean showmsg;
{
    if (!cansee(x, y) || !ACCESSIBLE(levl[x][y].typ) || distanceSquaredToYou(x, y) >= 32) {
        if (showmsg)
            You("smell rotten eggs.");
        return FALSE;
    }
    return TRUE;
}

void
display_stinking_cloud_positions(state)
int state;
{
    if (state == 0) {
        tmp_at(DISP_BEAM, cmap_to_glyph(S_goodpos));
    } else if (state == 1) {
        int x, y, dx, dy;
        int dist = 6;

        for (dx = -dist; dx <= dist; dx++)
            for (dy = -dist; dy <= dist; dy++) {
                x = currentX() + dx;
                y = currentY() + dy;
                if (isok(x, y) && is_valid_stinking_cloud_pos(x, y, FALSE))
                    tmp_at(x, y);
            }
    } else {
        tmp_at(DISP_END, 0);
    }
}

/* scroll effects; return 1 if we use up the scroll and possibly make it
   become discovered, 0 if caller should take care of those side-effects */
int
seffects(sobj)
struct obj *sobj; /* scroll, or fake spellbook object for scroll-like spell */
{
    int cval, otyp = sobj->otyp;
    boolean confused = youAreConfused(), sblessed = sobj->blessed,
            scursed = sobj->cursed, already_known, old_erodeproof,
            new_erodeproof;
    struct obj *otmp;

    if (objects[otyp].oc_magic)
        exercise(A_WIS, TRUE);                       /* just for trying */
    already_known = (sobj->oclass == SPBOOK_CLASS /* spell */
                     || objects[otyp].oc_name_known);

    switch (otyp) {
#ifdef MAIL
    case SCR_MAIL:
        known = TRUE;
        if (sobj->spe)
            pline(
    "This seems to be junk mail addressed to the finder of the Eye of Larn.");
        /* note to the puzzled: the game Larn actually sends you junk
         * mail if you win!
         */
        else
            readmail(sobj);
        break;
#endif
    case SCR_ENCHANT_ARMOR: {
        register schar s;
        boolean special_armor;
        boolean same_color;

        otmp = some_armor(&youmonst);
        if (!otmp) {
            strange_feeling(sobj, youCanSee()
                                      ? "Your skin glows then fades."
                                      : "Your skin feels warm for a moment.");
            sobj = 0; /* useup() in strange_feeling() */
            exercise(A_CON, !scursed);
            exercise(A_STR, !scursed);
            break;
        }
        if (confused) {
            old_erodeproof = (otmp->oerodeproof != 0);
            new_erodeproof = !scursed;
            otmp->oerodeproof = 0; /* for messages */
            if (youCannotSee()) {
                otmp->rknown = FALSE;
                pline("%s warm for a moment.", Yobjnam2(otmp, "feel"));
            } else {
                otmp->rknown = TRUE;
                pline("%s covered by a %s %s %s!", Yobjnam2(otmp, "are"),
                      scursed ? "mottled" : "shimmering",
                      hcolor(scursed ? NH_BLACK : NH_GOLDEN),
                      scursed ? "glow"
                              : (is_shield(otmp) ? "layer" : "shield"));
            }
            if (new_erodeproof && (otmp->oeroded || otmp->oeroded2)) {
                otmp->oeroded = otmp->oeroded2 = 0;
                pline("%s as good as new!",
                      Yobjnam2(otmp, youCannotSee() ? "feel" : "look"));
            }
            if (old_erodeproof && !new_erodeproof) {
                /* restore old_erodeproof before shop charges */
                otmp->oerodeproof = 1;
                costly_alteration(otmp, COST_DEGRD);
            }
            otmp->oerodeproof = new_erodeproof ? 1 : 0;
            break;
        }
        /* elven armor vibrates warningly when enchanted beyond a limit */
        special_armor = is_elven_armor(otmp)
                        || (yourRoleHasPMID(PM_WIZARD) && otmp->otyp == CORNUTHAUM);
        if (scursed)
            same_color = (otmp->otyp == BLACK_DRAGON_SCALE_MAIL
                          || otmp->otyp == BLACK_DRAGON_SCALES);
        else
            same_color = (otmp->otyp == SILVER_DRAGON_SCALE_MAIL
                          || otmp->otyp == SILVER_DRAGON_SCALES
                          || otmp->otyp == SHIELD_OF_REFLECTION);
        if (youCannotSee())
            same_color = FALSE;

        /* KMH -- catch underflow */
        s = scursed ? -otmp->spe : otmp->spe;
        if (s > (special_armor ? 5 : 3) && rn2(s)) {
            otmp->in_use = TRUE;
            pline("%s violently %s%s%s for a while, then %s.", Yname2(otmp),
                  otense(otmp, youCannotSee() ? "vibrate" : "glow"),
                  (youCanSee() && !same_color) ? " " : "",
                  (youCannotSee() || same_color) ? "" : hcolor(scursed ? NH_BLACK
                                                              : NH_SILVER),
                  otense(otmp, "evaporate"));
            remove_worn_item(otmp, FALSE);
            useup(otmp);
            break;
        }
        s = scursed ? -1 : otmp->spe >= 9
                               ? (rn2(otmp->spe) == 0)
                               : sblessed ? rnd(3 - otmp->spe / 3) : 1;
        if (s >= 0 && Is_dragon_scales(otmp)) {
            /* dragon scales get turned into dragon scale mail */
            pline("%s merges and hardens!", Yname2(otmp));
            setworn((struct obj *) 0, W_ARM);
            /* assumes same order */
            otmp->otyp += GRAY_DRAGON_SCALE_MAIL - GRAY_DRAGON_SCALES;
            if (sblessed) {
                otmp->spe++;
                if (!otmp->blessed)
                    bless(otmp);
            } else if (otmp->cursed)
                uncurse(otmp);
            otmp->known = 1;
            setworn(otmp, W_ARM);
            if (otmp->unpaid)
                alter_cost(otmp, 0L); /* shop bill */
            break;
        }
        pline("%s %s%s%s%s for a %s.", Yname2(otmp),
              s == 0 ? "violently " : "",
              otense(otmp, youCannotSee() ? "vibrate" : "glow"),
              (youCanSee() && !same_color) ? " " : "",
              (youCannotSee() || same_color) ? ""
                                    : hcolor(scursed ? NH_BLACK : NH_SILVER),
              (s * s > 1) ? "while" : "moment");
        /* [this cost handling will need updating if shop pricing is
           ever changed to care about curse/bless status of armor] */
        if (s < 0)
            costly_alteration(otmp, COST_DECHNT);
        if (scursed && !otmp->cursed)
            curse(otmp);
        else if (sblessed && !otmp->blessed)
            bless(otmp);
        if (s) {
            otmp->spe += s;
            adj_abon(otmp, s);
            known = otmp->known;
            /* update shop bill to reflect new higher price */
            if (s > 0 && otmp->unpaid)
                alter_cost(otmp, 0L);
        }

        if ((otmp->spe > (special_armor ? 5 : 3))
            && (special_armor || !rn2(7)))
            pline("%s %s.", Yobjnam2(otmp, "suddenly vibrate"),
                  youCannotSee() ? "again" : "unexpectedly");
        break;
    }
    case SCR_DESTROY_ARMOR: {
        otmp = some_armor(&youmonst);
        if (confused) {
            if (!otmp) {
                strange_feeling(sobj, "Your bones itch.");
                sobj = 0; /* useup() in strange_feeling() */
                exercise(A_STR, FALSE);
                exercise(A_CON, FALSE);
                break;
            }
            old_erodeproof = (otmp->oerodeproof != 0);
            new_erodeproof = scursed;
            otmp->oerodeproof = 0; /* for messages */
            p_glow2(otmp, NH_PURPLE);
            if (old_erodeproof && !new_erodeproof) {
                /* restore old_erodeproof before shop charges */
                otmp->oerodeproof = 1;
                costly_alteration(otmp, COST_DEGRD);
            }
            otmp->oerodeproof = new_erodeproof ? 1 : 0;
            break;
        }
        if (!scursed || !otmp || !otmp->cursed) {
            if (!destroy_arm(otmp)) {
                strange_feeling(sobj, "Your skin itches.");
                sobj = 0; /* useup() in strange_feeling() */
                exercise(A_STR, FALSE);
                exercise(A_CON, FALSE);
                break;
            } else
                known = TRUE;
        } else { /* armor and scroll both cursed */
            pline("%s.", Yobjnam2(otmp, "vibrate"));
            if (otmp->spe >= -6) {
                otmp->spe += -1;
                adj_abon(otmp, -1);
            }
            make_stunned(yourIntrinsicTimeout(STUNNED) + (long) rn1(10, 10), TRUE);
        }
    } break;
    case SCR_CONFUSE_MONSTER:
    case SPE_CONFUSE_MONSTER:
        if (monsterClass(pmid4you()) != S_HUMAN || scursed) {
            if (!yourIntrinsic(CONFUSION))
                You_feel("confused.");
            make_confused(yourIntrinsic(CONFUSION) + rnd(100), FALSE);
        } else if (confused) {
            if (!sblessed) {
                Your("%s begin to %s%s.", makeplural(body_part(HAND)),
                     youCannotSee() ? "tingle" : "glow ",
                     youCannotSee() ? "" : hcolor(NH_PURPLE));
                make_confused(yourIntrinsic(CONFUSION) + rnd(100), FALSE);
            } else {
                pline("A %s%s surrounds your %s.",
                      youCannotSee() ? "" : hcolor(NH_RED),
                      youCannotSee() ? "faint buzz" : " glow", body_part(HEAD));
                make_confused(0L, TRUE);
            }
        } else {
            if (!sblessed) {
                Your("%s%s %s%s.", makeplural(body_part(HAND)),
                     youCannotSee() ? "" : " begin to glow",
                     youCannotSee() ? (const char *) "tingle" : hcolor(NH_RED),
                     abilityToConfuseMonsters() ? " even more" : "");
                increaseAbilityToConfuseMonsters(1);
            } else {
                if (youCannotSee())
                    Your("%s tingle %s sharply.", makeplural(body_part(HAND)),
                         abilityToConfuseMonsters() ? "even more" : "very");
                else
                    Your("%s glow a%s brilliant %s.",
                         makeplural(body_part(HAND)),
                         abilityToConfuseMonsters() ? "n even more" : "", hcolor(NH_RED));
                /* after a while, repeated uses become less effective */
                if (abilityToConfuseMonsters() >= 40) {
                    increaseAbilityToConfuseMonsters(1);
                }
                else {
                    increaseAbilityToConfuseMonsters(rn1(8, 2));
                }
            }
        }
        break;
    case SCR_SCARE_MONSTER:
    case SPE_CAUSE_FEAR: {
        register int ct = 0;
        register struct monst *mtmp;

        for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
            if (DEADMONSTER(mtmp))
                continue;
            if (cansee(mtmp->mx, mtmp->my)) {
                if (confused || scursed) {
                    mtmp->mflee = mtmp->mfrozen = mtmp->msleeping = 0;
                    mtmp->mcanmove = 1;
                } else if (!resist(mtmp, sobj->oclass, 0, NOTELL))
                    monflee(mtmp, 0, FALSE, FALSE);
                if (!mtmp->mtame)
                    ct++; /* pets don't laugh at you */
            }
        }
        if (otyp == SCR_SCARE_MONSTER || !ct)
            You_hear("%s %s.", (confused || scursed) ? "sad wailing"
                                                     : "maniacal laughter",
                     !ct ? "in the distance" : "close by");
        break;
    }
    case SCR_BLANK_PAPER:
        if (youCannotSee())
            You("don't remember there being any magic words on this scroll.");
        else
            pline("This scroll seems to be blank.");
        known = TRUE;
        break;
    case SCR_REMOVE_CURSE:
    case SPE_REMOVE_CURSE: {
        register struct obj *obj;

        You_feel(!youAreHallucinating()
                     ? (!confused ? "like someone is helping you."
                                  : "like you need some help.")
                     : (!confused ? "in touch with the Universal Oneness."
                                  : "the power of the Force against you!"));

        if (scursed) {
            pline_The("scroll disintegrates.");
        } else {
            for (obj = invent; obj; obj = obj->nobj) {
                long wornmask;
                /* gold isn't subject to cursing and blessing */
                if (obj->oclass == COIN_CLASS)
                    continue;
                wornmask = (obj->owornmask & ~(W_BALL | W_ART | W_ARTI));
                if (wornmask && !sblessed) {
                    /* handle a couple of special cases; we don't
                       allow auxiliary weapon slots to be used to
                       artificially increase number of worn items */
                    if (obj == uswapwep) {
                        if (!usingTwoWeapons())
                            wornmask = 0L;
                    } else if (obj == uquiver) {
                        if (obj->oclass == WEAPON_CLASS) {
                            /* mergeable weapon test covers ammo,
                               missiles, spears, daggers & knives */
                            if (!objects[obj->otyp].oc_merge)
                                wornmask = 0L;
                        } else if (obj->oclass == GEM_CLASS) {
                            /* possibly ought to check whether
                               alternate weapon is a sling... */
                            if (!uslinging())
                                wornmask = 0L;
                        } else {
                            /* weptools don't merge and aren't
                               reasonable quivered weapons */
                            wornmask = 0L;
                        }
                    }
                }
                if (sblessed || wornmask || obj->otyp == LOADSTONE
                    || (obj->otyp == LEASH && obj->leashmon)) {
                    /* water price varies by curse/bless status */
                    boolean shop_h2o =
                        (obj->unpaid && obj->otyp == POT_WATER);

                    if (confused) {
                        blessorcurse(obj, 2);
                        /* lose knowledge of this object's curse/bless
                           state (even if it didn't actually change) */
                        obj->bknown = 0;
                        /* blessorcurse() only affects uncursed items
                           so no need to worry about price of water
                           going down (hence no costly_alteration) */
                        if (shop_h2o && (obj->cursed || obj->blessed))
                            alter_cost(obj, 0L); /* price goes up */
                    } else if (obj->cursed) {
                        if (shop_h2o)
                            costly_alteration(obj, COST_UNCURS);
                        uncurse(obj);
                    }
                }
            }
        }
        if (youAreBeingPunished() && !confused)
            unpunish();
        if (currentlyTrapped() && currentTrapType() == TT_BURIEDBALL) {
            buried_ball_to_freedom();
            pline_The("clasp on your %s vanishes.", body_part(LEG));
        }
        update_inventory();
        break;
    }
    case SCR_CREATE_MONSTER:
    case SPE_CREATE_MONSTER:
        if (createCrittersOfType(1 + ((confused || scursed) ? 12 : 0)
                                + ((sblessed || rn2(73)) ? 0 : rnd(4)),
                            (confused ? PM_ACID_BLOB : NON_PM),
                            FALSE))
            known = TRUE;
        /* no need to flush monsters; we ask for identification only if the
         * monsters are not visible
         */
        break;
    case SCR_ENCHANT_WEAPON:
        if (confused && uwep
            && (uwep->oclass == WEAPON_CLASS || is_weptool(uwep))) {
            old_erodeproof = (uwep->oerodeproof != 0);
            new_erodeproof = !scursed;
            uwep->oerodeproof = 0; /* for messages */
            if (youCannotSee()) {
                uwep->rknown = FALSE;
                Your("weapon feels warm for a moment.");
            } else {
                uwep->rknown = TRUE;
                pline("%s covered by a %s %s %s!", Yobjnam2(uwep, "are"),
                      scursed ? "mottled" : "shimmering",
                      hcolor(scursed ? NH_PURPLE : NH_GOLDEN),
                      scursed ? "glow" : "shield");
            }
            if (new_erodeproof && (uwep->oeroded || uwep->oeroded2)) {
                uwep->oeroded = uwep->oeroded2 = 0;
                pline("%s as good as new!",
                      Yobjnam2(uwep, youCannotSee() ? "feel" : "look"));
            }
            if (old_erodeproof && !new_erodeproof) {
                /* restore old_erodeproof before shop charges */
                uwep->oerodeproof = 1;
                costly_alteration(uwep, COST_DEGRD);
            }
            uwep->oerodeproof = new_erodeproof ? 1 : 0;
            break;
        }
        if (!chwepon(sobj,
                     scursed
                         ? -1
                         : !uwep ? 1 : (uwep->spe >= 9)
                                           ? !rn2(uwep->spe)
                                           : sblessed ? rnd(3 - uwep->spe / 3)
                                                      : 1))
            sobj = 0; /* nothing enchanted: strange_feeling -> useup */
        break;
    case SCR_TAMING:
    case SPE_CHARM_MONSTER: {
        int candidates, res, results, vis_results;

        if (swallowed()) {
            candidates = 1;
            results = vis_results = maybe_tame(u.ustuck, sobj);
        } else {
            int i, j, bd = confused ? 5 : 1;
            struct monst *mtmp;

            /* note: maybe_tame() can return either positive or
               negative values, but not both for the same scroll */
            candidates = results = vis_results = 0;
            for (i = -bd; i <= bd; i++)
                for (j = -bd; j <= bd; j++) {
                    if (!isok(currentX() + i, currentY() + j))
                        continue;
                    if ((mtmp = m_at(currentX() + i, currentY() + j)) != 0
                        || (!i && !j && (mtmp = u.usteed) != 0)) {
                        ++candidates;
                        res = maybe_tame(mtmp, sobj);
                        results += res;
                        if (canspotmon(mtmp))
                            vis_results += res;
                    }
                }
        }
        if (!results) {
            pline("Nothing interesting %s.",
                  !candidates ? "happens" : "seems to happen");
        } else {
            pline_The("neighborhood %s %sfriendlier.",
                      vis_results ? "is" : "seems",
                      (results < 0) ? "un" : "");
            if (vis_results > 0)
                known = TRUE;
        }
    } break;
    case SCR_GENOCIDE:
        if (!already_known)
            You("have found a scroll of genocide!");
        known = TRUE;
        if (sblessed)
            do_class_genocide();
        else
            do_genocide(!scursed | (2 * !!yourIntrinsic(CONFUSION)));
        break;
    case SCR_LIGHT:
        if (!confused || rn2(5)) {
            if (youCanSee())
                known = TRUE;
            litroom(!confused && !scursed, sobj);
            if (!confused && !scursed) {
                if (lightdamage(sobj, TRUE, 5))
                    known = TRUE;
            }
        } else {
            /* could be scroll of create monster, don't set known ...*/
            (void) createCrittersOfType(1, (!scursed ? PM_YELLOW_LIGHT : PM_BLACK_LIGHT), TRUE);
            if (!objects[sobj->otyp].oc_uname)
                docall(sobj);
        }
        break;
    case SCR_TELEPORTATION:
        if (confused || scursed) {
            level_tele();
        } else {
            known = scrolltele(sobj);
        }
        break;
    case SCR_GOLD_DETECTION:
        if ((confused || scursed) ? trap_detect(sobj) : gold_detect(sobj))
            sobj = 0; /* failure: strange_feeling() -> useup() */
        break;
    case SCR_FOOD_DETECTION:
    case SPE_DETECT_FOOD:
        if (food_detect(sobj))
            sobj = 0; /* nothing detected: strange_feeling -> useup */
        break;
    case SCR_IDENTIFY:
        /* known = TRUE; -- handled inline here */
        /* use up the scroll first, before makeknown() performs a
           perm_invent update; also simplifies empty invent check */
        useup(sobj);
        sobj = 0; /* it's gone */
        if (confused)
            You("identify this as an identify scroll.");
        else if (!already_known || !invent)
            /* force feedback now if invent became
               empty after using up this scroll */
            pline("This is an identify scroll.");
        if (!already_known)
            (void) learnscrolltyp(SCR_IDENTIFY);
        /*FALLTHRU*/
    case SPE_IDENTIFY:
        cval = 1;
        if (sblessed || (!scursed && !rn2(5))) {
            cval = rn2(5);
            /* note: if cval==0, identify all items */
            if (cval == 1 && sblessed && currentLuckWithBonus() > 0)
                ++cval;
        }
        if (invent && !confused) {
            identify_pack(cval, !already_known);
        } else if (otyp == SPE_IDENTIFY) {
            /* when casting a spell we know we're not confused,
               so inventory must be empty (another message has
               already been given above if reading a scroll) */
            pline("You're not carrying anything to be identified.");
        }
        break;
    case SCR_CHARGING:
        if (confused) {
            if (scursed) {
                You_feel("discharged.");
                setCurrentMagicalEnergy(0);
            } else {
                You_feel("charged up!");
                increaseCurrentMagicalEnergy(d(sblessed ? 6 : 4, 4));
                if (currentMagicalEnergy() > maximumMagicalEnergy()) /* if current energy is already at   */
                    setMaximumMagicalEnergy(currentMagicalEnergy()); /* or near maximum, increase maximum */
                else
                    setCurrentMagicalEnergy(maximumMagicalEnergy()); /* otherwise restore current to max  */
            }
            context.botl = 1;
            break;
        }
        /* known = TRUE; -- handled inline here */
        if (!already_known) {
            pline("This is a charging scroll.");
            learnscroll(sobj);
        }
        /* use it up now to prevent it from showing in the
           getobj picklist because the "disappears" message
           was already delivered */
        useup(sobj);
        sobj = 0; /* it's gone */
        otmp = getobj(all_count, "charge");
        if (otmp)
            recharge(otmp, scursed ? -1 : sblessed ? 1 : 0);
        break;
    case SCR_MAGIC_MAPPING:
        if (level.flags.nommap) {
            Your("mind is filled with crazy lines!");
            if (youAreHallucinating())
                pline("Wow!  Modern art.");
            else
                Your("%s spins in bewilderment.", body_part(HEAD));
            make_confused(yourIntrinsic(CONFUSION) + rnd(30), FALSE);
            break;
        }
        if (sblessed) {
            register int x, y;

            for (x = 1; x < COLNO; x++)
                for (y = 0; y < ROWNO; y++)
                    if (levl[x][y].typ == SDOOR)
                        cvt_sdoor_to_door(&levl[x][y]);
            /* do_mapping() already reveals secret passages */
        }
        known = TRUE;
    case SPE_MAGIC_MAPPING:
        if (level.flags.nommap) {
            Your("%s spins as %s blocks the spell!", body_part(HEAD),
                 something);
            make_confused(yourIntrinsic(CONFUSION) + rnd(30), FALSE);
            break;
        }
        pline("A map coalesces in your mind!");
        cval = (scursed && !confused);
        if (cval)
            setYourIntrinsic(CONFUSION, 1L); /* to screw up map */
        do_mapping();
        if (cval) {
            setYourIntrinsic(CONFUSION, 0L); /* restore */
            pline("Unfortunately, you can't grasp the details.");
        }
        break;
    case SCR_AMNESIA:
        known = TRUE;
        forget((!sblessed ? ALL_SPELLS : 0)
               | (!confused || scursed ? ALL_MAP : 0));
        if (youAreHallucinating()) /* Ommmmmm! */
            Your("mind releases itself from mundane concerns.");
        else if (!strncmpi(plname, "Maud", 4))
            pline(
          "As your mind turns inward on itself, you forget everything else.");
        else if (rn2(2))
            pline("Who was that Maud person anyway?");
        else
            pline("Thinking of Maud you forget everything else.");
        exercise(A_WIS, FALSE);
        break;
    case SCR_FIRE:
        cval = bcsign(sobj);
        useup(sobj);
        sobj = 0; /* it's gone */
        if (!already_known)
            (void) learnscrolltyp(SCR_FIRE);
        if (confused) {
            if (youResistFire()) {
                shieldeff(currentX(), currentY());
                if (youCanSee())
                    pline("Oh, look, what a pretty fire in your %s.",
                          makeplural(body_part(HAND)));
                else
                    You_feel("a pleasant warmth in your %s.",
                             makeplural(body_part(HAND)));
            } else {
                pline_The("scroll catches fire and you burn your %s.",
                          makeplural(body_part(HAND)));
                losehp(1, "scroll of fire", KILLED_BY_AN);
            }
            break;
        }
        if (underwater()) {
            pline_The("water around you vaporizes violently!");
        } else {
            pline_The("scroll erupts in a tower of flame!");
            iflags.last_msg = PLNMSG_TOWER_OF_FLAME; /* for explode() */
            burn_away_slime();
        }
        explode(currentX(), currentY(), 11, (2 * (rn1(3, 3) + 2 * cval) + 1) / 3,
                SCROLL_CLASS, EXPL_FIERY);
        break;
    case SCR_EARTH:
        /* TODO: handle steeds */
        if (!areYouOnRogueLevel() && currentLevelHasCeiling()
            && (!areYouInEndgame() || areYouOnEarthLevel())) {
            register int x, y;
            int nboulders = 0;

            /* Identify the scroll */
            if (swallowed())
                You_hear("rumbling.");
            else
                pline_The("%s rumbles %s you!", ceiling(currentX(), currentY()),
                          sblessed ? "around" : "above");
            known = 1;
            sokoban_guilt();

            /* Loop through the surrounding squares */
            if (!scursed)
                for (x = currentX() - 1; x <= currentX() + 1; x++) {
                    for (y = currentY() - 1; y <= currentY() + 1; y++) {
                        /* Is this a suitable spot? */
                        if (isok(x, y) && !closed_door(x, y)
                            && !IS_ROCK(levl[x][y].typ)
                            && !IS_AIR(levl[x][y].typ)
                            && (x != currentX() || y != currentY())) {
                            nboulders +=
                                drop_boulder_on_monster(x, y, confused, TRUE);
                        }
                    }
                }
            /* Attack the player */
            if (!sblessed) {
                drop_boulder_on_player(confused, !scursed, TRUE, FALSE);
            } else if (!nboulders)
                pline("But nothing else happens.");
        }
        break;
    case SCR_PUNISHMENT:
        known = TRUE;
        if (confused || sblessed) {
            You_feel("guilty.");
            break;
        }
        punish(sobj);
        break;
    case SCR_STINKING_CLOUD: {
        coord cc;

        if (!already_known)
            You("have found a scroll of stinking cloud!");
        known = TRUE;
        pline("Where do you want to center the %scloud?",
              already_known ? "stinking " : "");
        cc.x = currentX();
        cc.y = currentY();
        getpos_sethilite(display_stinking_cloud_positions);
        if (getpos(&cc, TRUE, "the desired position") < 0) {
            pline1(Never_mind);
            break;
        }
        if (!is_valid_stinking_cloud_pos(cc.x, cc.y, TRUE))
            break;
        (void) create_gas_cloud(cc.x, cc.y, 3 + bcsign(sobj),
                                8 + 4 * bcsign(sobj));
        break;
    }
    default:
        impossible("What weird effect is this? (%u)", otyp);
    }
    return sobj ? 0 : 1;
}

void
drop_boulder_on_player(confused, helmet_protects, byu, skip_uswallow)
boolean confused, helmet_protects, byu, skip_uswallow;
{
    int dmg;
    struct obj *otmp2;

    /* hit monster if swallowed */
    if (swallowed() && !skip_uswallow) {
        drop_boulder_on_monster(currentX(), currentY(), confused, byu);
        return;
    }

    otmp2 = mksobj(confused ? ROCK : BOULDER, FALSE, FALSE);
    if (!otmp2)
        return;
    otmp2->quan = confused ? rn1(5, 2) : 1;
    otmp2->owt = weight(otmp2);
    if (!isAmorphous(pmid4you()) && !youCanPassThroughWalls()
        && !isNoncorporeal(pmid4you()) && !isUnsolid(pmid4you())) {
        You("are hit by %s!", doname(otmp2));
        dmg = dmgval(otmp2, &youmonst) * otmp2->quan;
        if (uarmh && helmet_protects) {
            if (is_metallic(uarmh)) {
                pline("Fortunately, you are wearing a hard helmet.");
                if (dmg > 2)
                    dmg = 2;
            } else if (flags.verbose) {
                pline("%s does not protect you.", Yname2(uarmh));
            }
        }
    } else
        dmg = 0;
    /* Must be before the losehp(), for bones files */
    if (!flooreffects(otmp2, currentX(), currentY(), "fall")) {
        place_object(otmp2, currentX(), currentY());
        stackobj(otmp2);
        newsym(currentX(), currentY());
    }
    if (dmg)
        losehp(Maybe_Half_Phys(dmg), "scroll of earth", KILLED_BY_AN);
}

boolean
drop_boulder_on_monster(x, y, confused, byu)
int x, y;
boolean confused, byu;
{
    register struct obj *otmp2;
    register struct monst *mtmp;

    /* Make the object(s) */
    otmp2 = mksobj(confused ? ROCK : BOULDER, FALSE, FALSE);
    if (!otmp2)
        return FALSE; /* Shouldn't happen */
    otmp2->quan = confused ? rn1(5, 2) : 1;
    otmp2->owt = weight(otmp2);

    /* Find the monster here (won't be player) */
    mtmp = m_at(x, y);
    int mpmid = pmid4mon(mtmp);
    if (mtmp && !isAmorphous(mpmid) && !passesThroughWalls(mpmid)
        && !isNoncorporeal(mpmid) && !isUnsolid(mpmid)) {
        struct obj *helmet = which_armor(mtmp, W_ARMH);
        int mdmg;

        if (cansee(mtmp->mx, mtmp->my)) {
            pline("%s is hit by %s!", Monnam(mtmp), doname(otmp2));
            if (mtmp->minvis && !canspotmon(mtmp))
                map_invisible(mtmp->mx, mtmp->my);
        } else if (swallowed() && mtmp == u.ustuck)
            You_hear("something hit %s %s over your %s!",
                     s_suffix(mon_nam(mtmp)), mbodypart(mtmp, STOMACH),
                     body_part(HEAD));

        mdmg = dmgval(otmp2, mtmp) * otmp2->quan;
        if (helmet) {
            if (is_metallic(helmet)) {
                if (canspotmon(mtmp))
                    pline("Fortunately, %s is wearing a hard helmet.",
                          mon_nam(mtmp));
                else if (!youAreDeaf())
                    You_hear("a clanging sound.");
                if (mdmg > 2)
                    mdmg = 2;
            } else {
                if (canspotmon(mtmp)) {
		    javaString objective = objectivePronoun(pronoun_gender(mtmp));
                    pline("%s's %s does not protect %s.", Monnam(mtmp),
                          xname(helmet), objective.c_str);
		    releaseJavaString(objective);
		}
            }
        }
        mtmp->mhp -= mdmg;
        if (mtmp->mhp <= 0) {
            if (byu)
                xkilled(mtmp, 1);
            else {
                pline("%s is killed.", Monnam(mtmp));
                mondied(mtmp);
            }
        }
    } else if (swallowed() && mtmp == u.ustuck) {
        obfree(otmp2, (struct obj *) 0);
        /* fall through to player */
        drop_boulder_on_player(confused, TRUE, FALSE, TRUE);
        return 1;
    }
    /* Drop the rock/boulder to the floor */
    if (!flooreffects(otmp2, x, y, "fall")) {
        place_object(otmp2, x, y);
        stackobj(otmp2);
        newsym(x, y); /* map the rock */
    }
    return TRUE;
}

/* overcharging any wand or zapping/engraving cursed wand */
void
wand_explode(obj, chg)
struct obj *obj;
int chg; /* recharging */
{
    const char *expl = !chg ? "suddenly" : "vibrates violently and";
    int dmg, n, k;

    /* number of damage dice */
    if (!chg)
        chg = 2; /* zap/engrave adjustment */
    n = obj->spe + chg;
    if (n < 2)
        n = 2; /* arbitrary minimum */
    /* size of damage dice */
    switch (obj->otyp) {
    case WAN_WISHING:
        k = 12;
        break;
    case WAN_CANCELLATION:
    case WAN_DEATH:
    case WAN_POLYMORPH:
    case WAN_UNDEAD_TURNING:
        k = 10;
        break;
    case WAN_COLD:
    case WAN_FIRE:
    case WAN_LIGHTNING:
    case WAN_MAGIC_MISSILE:
        k = 8;
        break;
    case WAN_NOTHING:
        k = 4;
        break;
    default:
        k = 6;
        break;
    }
    /* inflict damage and destroy the wand */
    dmg = d(n, k);
    obj->in_use = TRUE; /* in case losehp() is fatal (or --More--^C) */
    pline("%s %s explodes!", Yname2(obj), expl);
    losehp(Maybe_Half_Phys(dmg), "exploding wand", KILLED_BY_AN);
    useup(obj);
    /* obscure side-effect */
    exercise(A_STR, FALSE);
}

/* used to collect gremlins being hit by light so that they can be processed
   after vision for the entire lit area has been brought up to date */
struct litmon {
    struct monst *mon;
    struct litmon *nxt;
};
STATIC_VAR struct litmon *gremlins = 0;

/*
 * Low-level lit-field update routine.
 */
STATIC_PTR void
set_lit(x, y, val)
int x, y;
genericptr_t val;
{
    struct monst *mtmp;
    struct litmon *gremlin;

    if (val) {
        levl[x][y].lit = 1;
        if ((mtmp = m_at(x, y)) != 0 && (pmid4mon(mtmp) == PM_GREMLIN)) {
            gremlin = (struct litmon *) alloc(sizeof *gremlin);
            gremlin->mon = mtmp;
            gremlin->nxt = gremlins;
            gremlins = gremlin;
        }
    } else {
        levl[x][y].lit = 0;
        snuff_light_source(x, y);
    }
}

void
litroom(on, obj)
register boolean on;
struct obj *obj;
{
    char is_lit; /* value is irrelevant; we use its address
                    as a `not null' flag for set_lit() */

    /* first produce the text (provided you're not blind) */
    if (!on) {
        register struct obj *otmp;

        if (youCanSee()) {
            if (swallowed()) {
                pline("It seems even darker in here than before.");
            } else {
                if (uwep && artifact_light(uwep) && uwep->lamplit)
                    pline("Suddenly, the only light left comes from %s!",
                          the(xname(uwep)));
                else
                    You("are surrounded by darkness!");
            }
        }

        /* the magic douses lamps, et al, too */
        for (otmp = invent; otmp; otmp = otmp->nobj)
            if (otmp->lamplit)
                (void) snuff_lit(otmp);
    } else { /* on */
        if (swallowed()) {
            if (youCannotSee())
                ; /* no feedback */
            else if (isAnimal(pmid4mon(u.ustuck)))
                pline("%s %s is lit.", s_suffix(Monnam(u.ustuck)),
                      mbodypart(u.ustuck, STOMACH));
            else if (isWhirly(pmid4mon(u.ustuck)))
                pline("%s shines briefly.", Monnam(u.ustuck));
            else
                pline("%s glistens.", Monnam(u.ustuck));
        } else if (youCanSee())
            pline("A lit field surrounds you!");
    }

    /* No-op when swallowed or in water */
    if (swallowed() || underwater() || areYouOnWaterLevel())
        return;
    /*
     *  If we are darkening the room and the hero is punished but not
     *  blind, then we have to pick up and replace the ball and chain so
     *  that we don't remember them if they are out of sight.
     */
    if (youAreBeingPunished() && !on && youCanSee())
        move_bc(1, 0, uball->ox, uball->oy, uchain->ox, uchain->oy);

    if (areYouOnRogueLevel()) {
        /* Can't use do_clear_area because MAX_RADIUS is too small */
        /* rogue lighting must light the entire room */
        int rnum = levl[currentX()][currentY()].roomno - ROOMOFFSET;
        int rx, ry;

        if (rnum >= 0) {
            for (rx = rooms[rnum].lx - 1; rx <= rooms[rnum].hx + 1; rx++)
                for (ry = rooms[rnum].ly - 1; ry <= rooms[rnum].hy + 1; ry++)
                    set_lit(rx, ry,
                            (genericptr_t) (on ? &is_lit : (char *) 0));
            rooms[rnum].rlit = on;
        }
        /* hallways remain dark on the rogue level */
    } else
        do_clear_area(currentX(), currentY(),
                      (obj && obj->oclass == SCROLL_CLASS && obj->blessed)
                         ? 9 : 5,
                      set_lit, (genericptr_t) (on ? &is_lit : (char *) 0));

    /*
     *  If we are not blind, then force a redraw on all positions in sight
     *  by temporarily blinding the hero.  The vision recalculation will
     *  correctly update all previously seen positions *and* correctly
     *  set the waslit bit [could be messed up from above].
     */
    if (youCanSee()) {
        vision_recalc(2);

        /* replace ball&chain */
        if (youAreBeingPunished() && !on)
            move_bc(0, 0, uball->ox, uball->oy, uchain->ox, uchain->oy);
    }

    vision_full_recalc = 1; /* delayed vision recalculation */
    if (gremlins) {
        struct litmon *gremlin;

        /* can't delay vision recalc after all */
        vision_recalc(0);
        /* after vision has been updated, monsters who are affected
           when hit by light can now be hit by it */
        do {
            gremlin = gremlins;
            gremlins = gremlin->nxt;
            light_hits_gremlin(gremlin->mon, rnd(5));
            free((genericptr_t) gremlin);
        } while (gremlins);
    }
}

STATIC_OVL void
do_class_genocide()
{
    int i, j, immunecnt, gonecnt, goodcnt, class, feel_dead = 0;
    char buf[BUFSZ];
    boolean gameover = FALSE; /* true iff killed self */

    for (j = 0;; j++) {
        if (j >= 5) {
            pline1(thats_enough_tries);
            return;
        }
        do {
            getlin("What class of monsters do you wish to genocide?", buf);
            (void) mungspaces(buf);
        } while (!*buf);
        /* choosing "none" preserves genocideless conduct */
        if (*buf == '\033' || !strcmpi(buf, "none")
            || !strcmpi(buf, "nothing"))
            return;

        class = name_to_monclass(buf, (int *) 0);
        if (class == 0 && (i = name_to_mon(buf)) != NON_PM)
            class = monsterClass(i);
        immunecnt = gonecnt = goodcnt = 0;
        for (i = LOW_PM; i < NUMMONS; i++) {
            if (monsterClass(i) == class) {
                if (!(monsterGenerationMask(i) & G_GENO))
                    immunecnt++;
                else if (mvitals[i].mvflags & G_GENOD)
                    gonecnt++;
                else
                    goodcnt++;
            }
        }
        if (!goodcnt && class != monsterClass(yourRolePMID())
            && class != monsterClass(yourSpeciesPMIDAsMale())) {
            if (gonecnt)
                pline("All such monsters are already nonexistent.");
            else if (immunecnt || class == S_invisible)
                You("aren't permitted to genocide such monsters.");
            else if (wizard && buf[0] == '*') {
                register struct monst *mtmp, *mtmp2;

                gonecnt = 0;
                for (mtmp = fmon; mtmp; mtmp = mtmp2) {
                    mtmp2 = mtmp->nmon;
                    if (DEADMONSTER(mtmp))
                        continue;
                    mongone(mtmp);
                    gonecnt++;
                }
                pline("Eliminated %d monster%s.", gonecnt, plur(gonecnt));
                return;
            } else
                pline("That %s does not represent any monster.",
                      strlen(buf) == 1 ? "symbol" : "response");
            continue;
        }

        for (i = LOW_PM; i < NUMMONS; i++) {
            if (monsterClass(i) == class) {
                char nam[BUFSZ];

		javaString monsterName = monsterTypeName(i);
                Strcpy(nam, makeplural(monsterName.c_str));
		releaseJavaString(monsterName);

                /* Although "genus" is Latin for race, the hero benefits
                 * from both race and role; thus genocide affects either.
                 */
                if (Your_Own_Role(i) || Your_Own_Race(i)
                    || ((monsterGenerationMask(i) & G_GENO)
                        && !(mvitals[i].mvflags & G_GENOD))) {
                    /* This check must be first since player monsters might
                     * have G_GENOD or !G_GENO.
                     */
                    mvitals[i].mvflags |= (G_GENOD | G_NOCORPSE);
                    resetMonsterRandomizer(i);
                    kill_genocided_monsters();
                    update_inventory(); /* eggs & tins */
                    pline("Wiped out all %s.", nam);
                    if (areYouPolymorphed() && i == currentMonsterNumber()) {
                        decreaseCurrentHitPointsAsMonster(1);
                        if (youAreUnchanging()) {
                            if (!feel_dead++)
                                You("die.");
                            /* finish genociding this class of
                               monsters before ultimately dying */
                            gameover = TRUE;
                        } else
                            rehumanize();
                    }
                    /* Self-genocide if it matches either your race
                       or role.  Assumption:  male and female forms
                       share same monster class. */
                    if (i == malePMIDForYourRole() || i == yourSpeciesPMIDAsMale()) {
                        setCurrentHitPoints(-1);
                        if (areYouPolymorphed()) {
                            if (!feel_dead++)
                                You_feel("dead inside.");
                        } else {
                            if (!feel_dead++)
                                You("die.");
                            gameover = TRUE;
                        }
                    }
                } else if (mvitals[i].mvflags & G_GENOD) {
                    if (!gameover)
                        pline("All %s are already nonexistent.", nam);
                } else if (!gameover) {
                    /* suppress feedback about quest beings except
                       for those applicable to our own role */
		    int msound = monsterSound(i);
                    if ((msound != MS_LEADER
                         || quest_info(MS_LEADER) == i)
                        && (msound != MS_NEMESIS
                            || quest_info(MS_NEMESIS) == i)
                        && (msound != MS_GUARDIAN
                            || quest_info(MS_GUARDIAN) == i)
                        /* non-leader/nemesis/guardian role-specific monster
                           */
                        && (i != PM_NINJA /* nuisance */
                            || yourRoleHasPMID(PM_SAMURAI))) {
                        boolean named, uniq;

                        named = typeIsProperName(i) ? TRUE : FALSE;
                        uniq = (monsterGenerationMask(i) & G_UNIQ) ? TRUE : FALSE;
                        /* one special case */
                        if (i == PM_HIGH_PRIEST)
                            uniq = FALSE;

		        javaString monsterName = monsterTypeName(i);
                        You("aren't permitted to genocide %s%s.",
                            (uniq && !named) ? "the " : "",
                            (uniq || named) ? monsterName.c_str : nam);
		        releaseJavaString(monsterName);
                    }
                }
            }
        }
        if (gameover || currentHitPoints() == -1) {
            killer.format = KILLED_BY_AN;
            Strcpy(killer.name, "scroll of genocide");
            if (gameover)
                done(GENOCIDED);
        }
        return;
    }
}

#define REALLY 1
#define PLAYER 2
#define ONTHRONE 4
void
do_genocide(how)
int how;
/* 0 = no genocide; create monsters (cursed scroll) */
/* 1 = normal genocide */
/* 3 = forced genocide of player */
/* 5 (4 | 1) = normal genocide from throne */
{
    char buf[BUFSZ];
    register int i, killplayer = 0;
    register int pmid;
    const char *which;

    if (how & PLAYER) {
        pmid = originalMonsterNumber(); /* non-polymorphed mon num */
        javaString monsterName = monsterTypeName(pmid);
        Strcpy(buf, monsterName.c_str);
	releaseJavaString(monsterName);
        killplayer++;
    } else {
        for (i = 0;; i++) {
            if (i >= 5) {
                pline1(thats_enough_tries);
                return;
            }
            getlin("What monster do you want to genocide? [type the name]",
                   buf);
            (void) mungspaces(buf);
            /* choosing "none" preserves genocideless conduct */
            if (!strcmpi(buf, "none") || !strcmpi(buf, "nothing")) {
                /* ... but no free pass if cursed */
                if (!(how & REALLY)) {
                    pmid = randomMonster();
                    if (NON_PM == pmid)
		    {
                        return; /* no message, like normal case */
		    }
                    break; /* remaining checks don't apply */
                } else {
                    return;
		}
            }

            pmid = name_to_mon(buf);
            if ((pmid == NON_PM) || (mvitals[pmid].mvflags & G_GENOD)) {
                pline("Such creatures %s exist in this world.",
                      (pmid == NON_PM) ? "do not" : "no longer");
                continue;
            }
            /* Although "genus" is Latin for race, the hero benefits
             * from both race and role; thus genocide affects either.
             */
            if (Your_Own_Role(pmid) || Your_Own_Race(pmid)) {
                killplayer++;
                break;
            }
            if (isHuman(pmid))
                adjalign(-sgn(currentAlignmentType()));
            if (isDemon(pmid))
                adjalign(sgn(currentAlignmentType()));

            if (!(monsterGenerationMask(pmid) & G_GENO)) {
                if (!youAreDeaf()) {
                    /* fixme: unconditional "caverns" will be silly in some
                     * circumstances */
                    if (flags.verbose)
                        pline(
                            "A thunderous voice booms through the caverns:");
                    verbalize("No, mortal!  That will not be done.");
                }
                continue;
            }
            /* KMH -- Unchanging prevents rehumanization */
            if (youAreUnchanging() && (pmid == pmid4you()))
	    {
                killplayer++;
	    }
            break;
        }
    }

    which = "all ";
    if (youAreHallucinating()) {
        if (areYouPolymorphed()) {
	    javaString youMonsterName = monsterTypeName(pmid4you());
            Strcpy(buf, youMonsterName.c_str);
	    releaseJavaString(youMonsterName);
	} else {
	    javaString yourRoleName = (flags.female && roleNameHasFemaleVersion(yourCurrentRoleID()))
		    ? roleNameAsFemale(yourCurrentRoleID()) : roleNameAsMale(yourCurrentRoleID());
            Strcpy(buf, yourRoleName.c_str);
            buf[0] = lowc(buf[0]);
	    releaseJavaString(yourRoleName);
        }
    } else {
	javaString monsterName = monsterTypeName(pmid);
        Strcpy(buf, monsterName.c_str); /* make sure we have standard singular */
	releaseJavaString(monsterName);

        if ((monsterGenerationMask(pmid) & G_UNIQ) && (pmid != PM_HIGH_PRIEST))
	{
            which = !typeIsProperName(pmid) ? "the " : "";
	}
    }
    if (how & REALLY) {
        /* setting no-corpse affects wishing and random tin generation */
        mvitals[pmid].mvflags |= (G_GENOD | G_NOCORPSE);
        pline("Wiped out %s%s.", which,
              (*which != 'a') ? buf : makeplural(buf));

        if (killplayer) {
            /* might need to wipe out dual role */
            if (yourRoleHasFemalePMID() && pmid == malePMIDForYourRole())
                mvitals[femalePMIDForYourRole()].mvflags |= (G_GENOD | G_NOCORPSE);
            if (yourRoleHasFemalePMID() && pmid == femalePMIDForYourRole())
                mvitals[malePMIDForYourRole()].mvflags |= (G_GENOD | G_NOCORPSE);
            if (yourSpeciesPMIDHasFemaleVersion() && pmid == yourSpeciesPMIDAsMale())
                mvitals[yourSpeciesPMIDAsFemale()].mvflags |= (G_GENOD | G_NOCORPSE);
            if (yourSpeciesPMIDHasFemaleVersion() && pmid == yourSpeciesPMIDAsFemale())
                mvitals[yourSpeciesPMIDAsMale()].mvflags |= (G_GENOD | G_NOCORPSE);

            setCurrentHitPoints(-1);
            if (how & PLAYER) {
                killer.format = KILLED_BY;
                Strcpy(killer.name, "genocidal confusion");
            } else if (how & ONTHRONE) {
                /* player selected while on a throne */
                killer.format = KILLED_BY_AN;
                Strcpy(killer.name, "imperious order");
            } else { /* selected player deliberately, not confused */
                killer.format = KILLED_BY_AN;
                Strcpy(killer.name, "scroll of genocide");
            }

            /* Polymorphed characters will die as soon as they're rehumanized.
             */
            /* KMH -- Unchanging prevents rehumanization */
            if (areYouPolymorphed() && (pmid != pmid4you())) {
                delayed_killer(POLYMORPH, killer.format, killer.name);
                You_feel("dead inside.");
            } else
                done(GENOCIDED);
        } else if (pmid == pmid4you()) {
            rehumanize();
        }
        resetMonsterRandomizer(pmid);
        kill_genocided_monsters();
        update_inventory(); /* in case identified eggs were affected */
    } else {
        int cnt = 0, census = monster_census(FALSE);

        if (!(monsterGenerationMask(pmid) & G_UNIQ)
            && !(mvitals[pmid].mvflags & (G_GENOD | G_EXTINCT)))
            for (i = rn1(3, 4); i > 0; i--) {
                if (!makeMonsterOfType(pmid, currentX(), currentY(), NO_MINVENT))
                    break; /* couldn't make one */
                ++cnt;
                if (mvitals[pmid].mvflags & G_EXTINCT)
                    break; /* just made last one */
            }
        if (cnt) {
            /* accumulated 'cnt' doesn't take groups into account;
               assume bringing in new mon(s) didn't remove any old ones */
            cnt = monster_census(FALSE) - census;
            pline("Sent in %s%s.", (cnt > 1) ? "some " : "",
                  (cnt > 1) ? makeplural(buf) : an(buf));
        } else
            pline1(nothing_happens);
    }
}

void
punish(sobj)
struct obj *sobj;
{
    struct obj *reuse_ball = (sobj && sobj->otyp == HEAVY_IRON_BALL)
                                ? sobj : (struct obj *) 0;

    /* KMH -- Punishment is still okay when you are riding */
    if (!reuse_ball)
        You("are being punished for your misbehavior!");
    if (youAreBeingPunished()) {
        Your("iron ball gets heavier.");
        uball->owt += 160 * (1 + sobj->cursed);
        return;
    }
    int upmid = pmid4you();
    if (isAmorphous(upmid) || isWhirly(upmid) || isUnsolid(upmid)) {
        if (!reuse_ball) {
            pline("A ball and chain appears, then falls away.");
            dropy(mkobj(BALL_CLASS, TRUE));
        } else {
            dropy(reuse_ball);
        }
        return;
    }
    setworn(mkobj(CHAIN_CLASS, TRUE), W_CHAIN);
    if (!reuse_ball)
        setworn(mkobj(BALL_CLASS, TRUE), W_BALL);
    else
        setworn(reuse_ball, W_BALL);
    uball->spe = 1; /* special ball (see save) */

    /*
     *  Place ball & chain if not swallowed.  If swallowed, the ball &
     *  chain variables will be set at the next call to placebc().
     */
    if (!swallowed()) {
        placebc();
        if (youCannotSee())
            set_bc(1);      /* set up ball and chain variables */
        newsym(currentX(), currentY()); /* see ball&chain if can't see self */
    }
}

/* remove the ball and chain */
void
unpunish()
{
    struct obj *savechain = uchain;

    obj_extract_self(uchain);
    newsym(uchain->ox, uchain->oy);
    setworn((struct obj *) 0, W_CHAIN);
    dealloc_obj(savechain);
    uball->spe = 0;
    setworn((struct obj *) 0, W_BALL);
}

/* some creatures have special data structures that only make sense in their
 * normal locations -- if the player tries to create one elsewhere, or to
 * revive one, the disoriented creature becomes a zombie
 */
boolean
cant_revive(mtype, revival, from_obj)
int *mtype;
boolean revival;
struct obj *from_obj;
{
    /* SHOPKEEPERS can be revived now */
    if (*mtype == PM_GUARD || (*mtype == PM_SHOPKEEPER && !revival)
        || *mtype == PM_HIGH_PRIEST || *mtype == PM_ALIGNED_PRIEST
        || *mtype == PM_ANGEL) {
        *mtype = PM_HUMAN_ZOMBIE;
        return TRUE;
    } else if (*mtype == PM_LONG_WORM_TAIL) { /* for create_particular() */
        *mtype = PM_LONG_WORM;
        return TRUE;
    } else if (corpseOrStatueIsUnique(*mtype)
               && (!from_obj || !has_omonst(from_obj))) {
        /* unique corpses (from bones or wizard mode wish) or
           statues (bones or any wish) end up as shapechangers */
        *mtype = PM_DOPPELGANGER;
        return TRUE;
    }
    return FALSE;
}

/*
 * Make a new monster with the type controlled by the user.
 *
 * Note:  when creating a monster by class letter, specifying the
 * "strange object" (']') symbol produces a random monster rather
 * than a mimic.  This behavior quirk is useful so don't "fix" it
 * (use 'm'--or "mimic"--to create a random mimic).
 *
 * Used in wizard mode only (for ^G command and for scroll or spell
 * of create monster).  Once upon a time, an earlier incarnation of
 * this code was also used for the scroll/spell in explore mode.
 */
boolean
create_particular()
{
    char buf[BUFSZ], *bufp, monclass;
    int which, tryct, i, firstchoice = NON_PM;
    int whichPmid = NON_PM;
    struct monst *mtmp;
    boolean madeany = FALSE;
    boolean maketame, makepeaceful, makehostile;
    boolean randmonst = FALSE;

    tryct = 5;
    do {
        monclass = MAXMCLASSES;
        which = yourRolePMID(); /* an arbitrary ID */
        maketame = makepeaceful = makehostile = FALSE;
        getlin("Create what kind of monster? [type the name or symbol]", buf);
        bufp = mungspaces(buf);
        if (*bufp == '\033')
            return FALSE;
        /* allow the initial disposition to be specified */
        if (!strncmpi(bufp, "tame ", 5)) {
            bufp += 5;
            maketame = TRUE;
        } else if (!strncmpi(bufp, "peaceful ", 9)) {
            bufp += 9;
            makepeaceful = TRUE;
        } else if (!strncmpi(bufp, "hostile ", 8)) {
            bufp += 8;
            makehostile = TRUE;
        }
        /* decide whether a valid monster was chosen */
        if (wizard && (!strcmp(bufp, "*") || !strcmp(bufp, "random"))) {
            randmonst = TRUE;
            break;
        }
        which = name_to_mon(bufp);
        if (which >= LOW_PM)
            break; /* got one */
        monclass = name_to_monclass(bufp, &which);
        if (which >= LOW_PM) {
            monclass = MAXMCLASSES; /* matters below */
            break;
        } else if (monclass > 0) {
            which = yourRolePMID(); /* reset from NON_PM */
            break;
        }
        /* no good; try again... */
        pline("I've never heard of such monsters.");
    } while (--tryct > 0);

    if (!tryct) {
        pline1(thats_enough_tries);
    } else {
        if (!randmonst) {
            firstchoice = which;
            if (cant_revive(&which, FALSE, (struct obj *) 0)) {
                /* wizard mode can override handling of special monsters */
		javaString whichMonsterName = monsterTypeName(which);
		javaString firstChoiceName  = monsterTypeName(firstchoice);
                Sprintf(buf, "Creating %s instead; force %s?",
                        whichMonsterName.c_str, firstChoiceName.c_str);
                if (yn(buf) == 'y') {
                    which = firstchoice;
		}
            }
            whichPmid = which;
        }
        for (i = 0; i <= multi; i++) {
            if (monclass != MAXMCLASSES)
                whichPmid = pickMonsterTypeOfClass(monclass, 0);
            else if (randmonst)
                whichPmid = randomMonster();
            mtmp = makeMonsterOfType(whichPmid, currentX(), currentY(), NO_MM_FLAGS);
            if (!mtmp) {
                /* quit trying if creation failed and is going to repeat */
                if (monclass == MAXMCLASSES && !randmonst)
                    break;
                /* otherwise try again */
                continue;
            }
            if (maketame) {
                (void) tamedog(mtmp, (struct obj *) 0);
            } else if (makepeaceful || makehostile) {
                mtmp->mtame = 0; /* sanity precaution */
                mtmp->mpeaceful = makepeaceful ? 1 : 0;
                set_malign(mtmp);
            }
            madeany = TRUE;
            /* in case we got a doppelganger instead of what was asked
               for, make it start out looking like what was asked for */
            if (mtmp->cham != NON_PM && firstchoice != NON_PM
                && mtmp->cham != firstchoice)
                (void) changeChameleonToType(mtmp, firstchoice, FALSE, FALSE);
        }
    }
    return madeany;
}

/*read.c*/
