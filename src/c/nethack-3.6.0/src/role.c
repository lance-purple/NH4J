/* NetHack 3.6	role.c	$NHDT-Date: 1446861770 2015/11/07 02:02:50 $  $NHDT-Branch: master $:$NHDT-Revision: 1.34 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985-1999. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/*** Table of all roles ***/
/* According to AD&D, HD for some classes (ex. Wizard) should be smaller
 * (4-sided for wizards).  But this is not AD&D, and using the AD&D
 * rule here produces an unplayable character.  Thus I have used a minimum
 * of an 10-sided hit die for everything.  Another AD&D change: wizards get
 * a minimum strength of 4 since without one you can't teleport or cast
 * spells. --KAA
 *
 * As the wizard has been updated (wizard patch 5 jun '96) their HD can be
 * brought closer into line with AD&D. This forces wizards to use magic more
 * and distance themselves from their attackers. --LSZ
 *
 * With the introduction of species, some hit points and energy
 * has been reallocated for each race.  The values assigned
 * to the roles has been reduced by the amount allocated to
 * humans.  --KMH
 *
 * God names use a leading underscore to flag goddesses.
 */
const int roleIDs[] = {
    0, // "Archeologist"
    1, // "Barbarian"
    2, // "Caveman", "Cavewoman"
    3, // "Healer"
    4, // "Knight"
    5, // "Monk"
    6, // "Priest", "Priestess"
    /* Note:  Rogue precedes Ranger so that use of `-R' on the command line
       retains its traditional meaning. */
    7, // "Rogue"
    8, // "Ranger"
    9, // "Samurai"
    10, // "Tourist"
    11, // "Valkyrie"
    12, // "Wizard"
    /* Array terminator */
    -1
};

/* Table of all alignments */
const char* aligns[] = {
    "lawful",
    "neutral",
    "chaotic",
    "unaligned" 
};

/* Filters */
static struct {
    boolean roles[SIZE(roleIDs)];
    short mask;
} XXfilter;

STATIC_DCL int NDECL(randrole_filtered);
STATIC_DCL char *FDECL(promptsep, (char *, int));
STATIC_DCL int FDECL(role_gendercount, (int));
STATIC_DCL int FDECL(race_alignmentcount, (int));

/* used by str2XXX() */
static char NEARDATA randomstr[] = "random";

boolean
validrole(rolenum)
int rolenum;
{
    return (boolean) (rolenum >= 0 && rolenum < numberOfKnownRoles());
}

int
randrole()
{
    return rn2(numberOfKnownRoles());
}

STATIC_OVL int
randrole_filtered()
{
    int i, n = 0, set[SIZE(roleIDs)];

    /* this doesn't rule out impossible combinations but attempts to
       honor all the filter masks */
    for (i = 0; i < SIZE(roleIDs); ++i)
        if (ok_role(i, roleNone(), roleNone(), roleNone())
            && ok_race(i, roleRandom(), roleNone(), roleNone())
            && ok_gend(i, roleNone(), roleRandom(), roleNone())
            && ok_align(i, roleNone(), roleNone(), roleRandom()))
            set[n++] = i;
    return n ? set[rn2(n)] : randrole();
}

int
str2role(str)
const char *str;
{
    /* Is str valid? */
    if (!str || !str[0])
        return roleNone();

    /* Match as much of str as is provided */
    int len = strlen(str);
    for (int i = 0, n = numberOfKnownRoles(); i < n; i++) {
	javaString rnam;
	boolean matches;

        /* Does it match the male name? */
	rnam = roleNameAsMale(i);
	matches = (!strncmpi(str, rnam.c_str, len));
	releaseJavaString(rnam);

        if (matches)
	{
            return i;
	}

        /* Or the female name? */
	if (roleNameHasFemaleVersion(i)) {
	    rnam = roleNameAsFemale(i);
	    matches = (!strncmpi(str, rnam.c_str, len));
	    releaseJavaString(rnam);
            if (matches)
	    {
                return i;
	    }
        }

        /* Or the filecode? */
	javaString filecode = yourRoleFileCode();
        matches = (!strcmpi(str, filecode.c_str));
        releaseJavaString(filecode);
        if (matches)
	{
            return i;
	}
    }

    if ((len == 1 && (*str == '*' || *str == '@'))
        || !strncmpi(str, randomstr, len))
        return roleRandom();

    /* Couldn't find anything appropriate */
    return roleNone();
}

boolean
validrace(rolenum, racenum)
int rolenum, racenum;
{
    /* Assumes validrole */
    return (boolean) (validSpeciesID(racenum)
                      && (startingMaskForRole(rolenum) & startingMaskForSpecies(racenum)));
}

int
randrace(rolenum)
int rolenum;
{
    int i, n = 0;

    /* Count the number of valid species */
    int playableSpecies = numberOfPlayableSpecies();

    for (i = 0; i < playableSpecies; i++)
        if (startingSpeciesMaskForRole(rolenum) & selfMaskForSpecies(i))
            n++;

    /* Pick a random race */
    /* Use a factor of 100 in case of bad random number generators */
    if (n)
        n = rn2(n * 100) / 100;
    for (i = 0; i < playableSpecies; i++)
        if (startingSpeciesMaskForRole(rolenum) & selfMaskForSpecies(i)) {
            if (n)
                n--;
            else
                return i;
        }

    /* This role has no permitted species? */
    return rn2(numberOfPlayableSpecies());
}

int
str2race(str)
const char *str;
{
    int i, len;

    /* Is str valid? */
    if (!str || !str[0])
        return roleNone();

    /* Match as much of str as is provided */
    len = strlen(str);
    int playableSpecies = numberOfPlayableSpecies();
    for (i = 0; i < playableSpecies; i++) {
        /* Does it match the noun? */
	javaString noun = nounForSpecies(i);
        boolean matches = (!strncmpi(str, noun.c_str, len));
	releaseJavaString(noun);

	if (matches) {
            return i;
	}

        /* Does it match the filecode? */
	javaString filecode = fileCodeForSpecies(i);
        matches = (!strcmpi(str, filecode.c_str));
	releaseJavaString(filecode);

	if (matches) {
            return i;
	}
    }

    if ((len == 1 && (*str == '*' || *str == '@'))
        || !strncmpi(str, randomstr, len))
        return roleRandom();

    /* Couldn't find anything appropriate */
    return roleNone();
}

boolean
validgend(rolenum, racenum, gendnum)
int rolenum, racenum, gendnum;
{
    /* Assumes validrole and validrace */
    return (boolean) (gendnum >= 0 && gendnum < adventurerGenders()
                      && (startingGenderMaskForRole(rolenum) & startingGenderMaskForSpecies(racenum)
                          & genderMask(gendnum)));
}

int
randgend(rolenum, racenum)
int rolenum, racenum;
{
    int i, n = 0;

    /* Count the number of valid genders */
    for (i = 0; i < adventurerGenders(); i++)
        if (startingGenderMaskForRole(rolenum) & startingGenderMaskForSpecies(racenum) & genderMask(i))
            n++;

    /* Pick a random gender */
    if (n)
        n = rn2(n);
    for (i = 0; i < adventurerGenders(); i++)
        if (startingGenderMaskForRole(rolenum) & startingGenderMaskForSpecies(racenum) & genderMask(i)) {
            if (n)
                n--;
            else
                return i;
        }

    /* This role/race has no permitted genders? */
    return rn2(adventurerGenders());
}

int
str2gend(str)
const char *str;
{
    int i, len;

    /* Is str valid? */
    if (!str || !str[0])
        return roleNone();

    /* Match as much of str as is provided */
    len = strlen(str);
    boolean matched;
    for (i = 0; i < adventurerGenders(); i++) {

        /* Does it match the adjective? */
	javaString adjective = genderAdjective(i);
	matched = (!strncmpi(str, adjective.c_str, len));
	releaseJavaString(adjective);

	if (matched) {
            return i;
	}

        /* Or the filecode? */
	javaString filecode = genderAbbreviation(i);
        matched = (!strcmpi(str, filecode.c_str));
	releaseJavaString(filecode);

	if (matched) {
            return i;
	}
    }
    if ((len == 1 && (*str == '*' || *str == '@'))
        || !strncmpi(str, randomstr, len))
        return roleRandom();

    /* Couldn't find anything appropriate */
    return roleNone();
}

boolean
validalign(rolenum, racenum, alignnum)
int rolenum, racenum, alignnum;
{
    /* Assumes validrole and validrace */
    return (boolean) (alignnum >= 0 && alignnum < numberOfRoleAlignments()
                      && (startingAlignmentMaskForRole(rolenum) & startingAlignmentMaskForSpecies(racenum)
                          & roleAlignmentMask(alignnum) & ROLE_ALIGNMASK));
}

int
randalign(rolenum, racenum)
int rolenum, racenum;
{
    int i, n = 0;

    /* Count the number of valid alignments */
    for (i = 0; i < numberOfRoleAlignments(); i++)
        if (startingAlignmentMaskForRole(rolenum) & startingAlignmentMaskForSpecies(racenum)
            & roleAlignmentMask(i) & ROLE_ALIGNMASK)
            n++;

    /* Pick a random alignment */
    if (n)
        n = rn2(n);
    for (i = 0; i < numberOfRoleAlignments(); i++)
        if (startingAlignmentMaskForRole(rolenum) & startingAlignmentMaskForSpecies(racenum)
            & roleAlignmentMask(i) & ROLE_ALIGNMASK) {
            if (n)
                n--;
            else
                return i;
        }

    /* This role/race has no permitted alignments? */
    return rn2(numberOfRoleAlignments());
}

int
str2align(str)
const char *str;
{
    int i, len;

    /* Is str valid? */
    if (!str || !str[0])
        return roleNone();

    /* Match as much of str as is provided */
    len = strlen(str);
    for (i = 0; i < numberOfRoleAlignments(); i++) {
        /* Does it match the adjective? */
        if (!strncmpi(str, aligns[i], len))
            return i;
        /* Or the filecode? */

	javaString fileCode = roleAlignmentFileCode(i);
	int caseSensitiveMatch = strcmpi(str, fileCode.c_str);
	releaseJavaString(fileCode);

        if (!caseSensitiveMatch) {
            return i;
	}
    }
    if ((len == 1 && (*str == '*' || *str == '@'))
        || !strncmpi(str, randomstr, len))
        return roleRandom();

    /* Couldn't find anything appropriate */
    return roleNone();
}

/* is rolenum compatible with any racenum/gendnum/alignnum constraints? */
boolean
ok_role(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (validrole(rolenum)) {
        if (roleFilter(rolenum))
            return FALSE;
        allow = startingMaskForRole(rolenum);
        if (validSpeciesID(racenum)
            && !(allow & selfMaskForSpecies(racenum)))
            return FALSE;
        if (gendnum >= 0 && gendnum < adventurerGenders()
            && !(allow & genderMask(gendnum) & ROLE_GENDMASK))
            return FALSE;
        if (alignnum >= 0 && alignnum < numberOfRoleAlignments()
            && !(allow & roleAlignmentMask(alignnum) & ROLE_ALIGNMASK))
            return FALSE;
        return TRUE;
    } else {
        /* random; check whether any selection is possible */
        for (i = 0; i < numberOfKnownRoles(); i++) {
            if (roleFilter(i))
                continue;
            allow = startingMaskForRole(i);
            if (validSpeciesID(racenum)
                && !(allow & selfMaskForSpecies(racenum)))
                continue;
            if (gendnum >= 0 && gendnum < adventurerGenders()
                && !(allow & genderMask(gendnum) & ROLE_GENDMASK))
                continue;
            if (alignnum >= 0 && alignnum < numberOfRoleAlignments()
                && !(allow & roleAlignmentMask(alignnum) & ROLE_ALIGNMASK))
                continue;
            return TRUE;
        }
        return FALSE;
    }
}

/* pick a random role subject to any racenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a role is returned only if there is  */
/* a single possibility */
int
pick_role(racenum, gendnum, alignnum, pickhow)
int racenum, gendnum, alignnum, pickhow;
{
    int i;
    int roles_ok = 0, set[SIZE(roleIDs)];

    for (i = 0; i < numberOfKnownRoles(); i++) {
        if (ok_role(i, racenum, gendnum, alignnum)
            && ok_race(i, (racenum >= 0) ? racenum : roleRandom(),
                       gendnum, alignnum)
            && ok_gend(i, racenum,
                       (gendnum >= 0) ? gendnum : roleRandom(), alignnum)
            && ok_race(i, racenum,
                       gendnum, (alignnum >= 0) ? alignnum : roleRandom()))
            set[roles_ok++] = i;
    }
    if (roles_ok == 0 || (roles_ok > 1 && pickhow == PICK_RIGID))
        return roleNone();
    return set[rn2(roles_ok)];
}

/* is racenum compatible with any rolenum/gendnum/alignnum constraints? */
boolean
ok_race(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (racenum >= 0 && (racenum < numberOfPlayableSpecies())) {
        if (roleFilterMask() & selfMaskForSpecies(racenum)) {
            return FALSE;
	}
        allow = startingMaskForSpecies(racenum);
        if (rolenum >= 0 && (rolenum < numberOfKnownRoles())
            && !(allow & startingSpeciesMaskForRole(rolenum))) {
            return FALSE;
	}
        if (gendnum >= 0 && (gendnum < adventurerGenders())
            && !(allow & genderMask(gendnum) & ROLE_GENDMASK)) {
            return FALSE;
	}
        if (alignnum >= 0 && alignnum < numberOfRoleAlignments()
            && !(allow & roleAlignmentMask(alignnum) & ROLE_ALIGNMASK)) {
            return FALSE;
	}
        return TRUE;
    } else {
        /* random; check whether any selection is possible */
        for (i = 0; i < numberOfPlayableSpecies(); i++) {
            if (roleFilterMask() & selfMaskForSpecies(i)) {
                continue;
	    }
            allow = startingMaskForSpecies(i);
            if (validrole(rolenum)
                && !(allow & startingSpeciesMaskForRole(rolenum))) {
                continue;
	    }
            if (gendnum >= 0 && gendnum < adventurerGenders()
                && !(allow & genderMask(gendnum) & ROLE_GENDMASK)) {
                continue;
	    }
            if (alignnum >= 0 && alignnum < numberOfRoleAlignments()
                && !(allow & roleAlignmentMask(alignnum) & ROLE_ALIGNMASK)) {
                continue;
	    }
            return TRUE;
        }
        return FALSE;
    }
}

/* pick a random race subject to any rolenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a race is returned only if there is  */
/* a single possibility */
int
pick_race(rolenum, gendnum, alignnum, pickhow)
int rolenum, gendnum, alignnum, pickhow;
{
    int i;
    int species_ok = 0;

    for (i = 0; i < numberOfPlayableSpecies(); i++) {
        if (ok_race(rolenum, i, gendnum, alignnum))
            species_ok++;
    }
    if (species_ok == 0 || (species_ok > 1 && pickhow == PICK_RIGID))
        return roleNone();
    species_ok = rn2(species_ok);
    for (i = 0; i < numberOfPlayableSpecies(); i++) {
        if (ok_race(rolenum, i, gendnum, alignnum)) {
            if (species_ok == 0)
                return i;
            else
                species_ok--;
        }
    }
    return roleNone();
}

/* is gendnum compatible with any rolenum/racenum/alignnum constraints? */
/* gender and alignment are not comparable (and also not constrainable) */
boolean
ok_gend(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum;
int alignnum UNUSED;
{
    int i;
    short allow;

    if (gendnum >= 0 && gendnum < adventurerGenders()) {
        if (roleFilterMask() & genderMask(gendnum))
            return FALSE;
        allow = genderMask(gendnum);
        if (validrole(rolenum)
            && !(allow & startingGenderMaskForRole(rolenum)))
            return FALSE;
        if (validSpeciesID(racenum)
            && !(allow & startingGenderMaskForSpecies(racenum)))
            return FALSE;
        return TRUE;
    } else {
        /* random; check whether any selection is possible */
        for (i = 0; i < adventurerGenders(); i++) {
            if (roleFilterMask() & genderMask(i))
                continue;
            allow = genderMask(i);
            if (validrole(rolenum)
                && !(allow & startingGenderMaskForRole(rolenum)))
                continue;
            if (validSpeciesID(racenum)
                && !(allow & startingGenderMaskForSpecies(racenum)))
                continue;
            return TRUE;
        }
        return FALSE;
    }
}

/* pick a random gender subject to any rolenum/racenum/alignnum constraints */
/* gender and alignment are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID a gender is returned only if there is  */
/* a single possibility */
int
pick_gend(rolenum, racenum, alignnum, pickhow)
int rolenum, racenum, alignnum, pickhow;
{
    int i;
    int gends_ok = 0;

    for (i = 0; i < adventurerGenders(); i++) {
        if (ok_gend(rolenum, racenum, i, alignnum))
            gends_ok++;
    }
    if (gends_ok == 0 || (gends_ok > 1 && pickhow == PICK_RIGID))
        return roleNone();
    gends_ok = rn2(gends_ok);
    for (i = 0; i < adventurerGenders(); i++) {
        if (ok_gend(rolenum, racenum, i, alignnum)) {
            if (gends_ok == 0)
                return i;
            else
                gends_ok--;
        }
    }
    return roleNone();
}

/* is alignnum compatible with any rolenum/racenum/gendnum constraints? */
/* alignment and gender are not comparable (and also not constrainable) */
boolean
ok_align(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum;
int gendnum UNUSED;
int alignnum;
{
    int i;
    short allow;

    if (alignnum >= 0 && alignnum < numberOfRoleAlignments()) {
        if (roleFilterMask() & roleAlignmentMask(alignnum))
            return FALSE;
        allow = roleAlignmentMask(alignnum);
        if (validrole(rolenum)
            && !(allow & startingAlignmentMaskForRole(rolenum)))
            return FALSE;
        if (validSpeciesID(racenum)
            && !(allow & startingAlignmentMaskForSpecies(racenum)))
            return FALSE;
        return TRUE;
    } else {
        /* random; check whether any selection is possible */
        for (i = 0; i < numberOfRoleAlignments(); i++) {
            if (roleFilterMask() & roleAlignmentMask(i))
                return FALSE;
            allow = roleAlignmentMask(i);
            if (validrole(rolenum)
                && !(allow & startingAlignmentMaskForRole(rolenum)))
                continue;
            if (validSpeciesID(racenum)
                && !(allow & startingAlignmentMaskForSpecies(racenum)))
                continue;
            return TRUE;
        }
        return FALSE;
    }
}

/* pick a random alignment subject to any rolenum/racenum/gendnum constraints
 */
/* alignment and gender are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID an alignment is returned only if there is  */
/* a single possibility */
int
pick_align(rolenum, racenum, gendnum, pickhow)
int rolenum, racenum, gendnum, pickhow;
{
    int i;
    int aligns_ok = 0;

    for (i = 0; i < numberOfRoleAlignments(); i++) {
        if (ok_align(rolenum, racenum, gendnum, i))
            aligns_ok++;
    }
    if (aligns_ok == 0 || (aligns_ok > 1 && pickhow == PICK_RIGID))
        return roleNone();
    aligns_ok = rn2(aligns_ok);
    for (i = 0; i < numberOfRoleAlignments(); i++) {
        if (ok_align(rolenum, racenum, gendnum, i)) {
            if (aligns_ok == 0)
                return i;
            else
                aligns_ok--;
        }
    }
    return roleNone();
}

void
rigid_role_checks()
{
    /* Some roles are limited to a single race, alignment, or gender and
     * calling this routine prior to XXX_player_selection() will help
     * prevent an extraneous prompt that actually doesn't allow
     * you to choose anything further. Note the use of PICK_RIGID which
     * causes the pick_XX() routine to return a value only if there is one
     * single possible selection, otherwise it returns roleNone().
     *
     */
    if (flags.initrole == roleRandom()) {
        /* If the role was explicitly specified as roleRandom()
         * via -uXXXX-@ then choose the role in here to narrow down
         * later choices. Pick a random role in this case.
         */
        flags.initrole = pick_role(flags.initrace, flags.initgend,
                                   flags.initalign, PICK_RANDOM);
        if (flags.initrole < 0)
            flags.initrole = randrole_filtered();
    }
    if (flags.initrole != roleNone()) {
        if (flags.initrace == roleNone())
            flags.initrace = pick_race(flags.initrole, flags.initgend,
                                       flags.initalign, PICK_RIGID);
        if (flags.initalign == roleNone())
            flags.initalign = pick_align(flags.initrole, flags.initrace,
                                         flags.initgend, PICK_RIGID);
        if (flags.initgend == roleNone())
            flags.initgend = pick_gend(flags.initrole, flags.initrace,
                                       flags.initalign, PICK_RIGID);
    }
}

boolean
setrolefilter(bufp)
const char *bufp;
{
    int i;
    boolean reslt = TRUE;

    if ((i = str2role(bufp)) != roleNone() && i != roleRandom()) {
        setRoleFilter(i, TRUE);
    }
    else if ((i = str2race(bufp)) != roleNone() && i != roleRandom()) {
        addRoleFilterMask(selfMaskForSpecies(i));
    }
    else if ((i = str2gend(bufp)) != roleNone() && i != roleRandom()) {
        addRoleFilterMask(genderMask(i));
    }
    else if ((i = str2align(bufp)) != roleNone() && i != roleRandom()) {
        addRoleFilterMask(roleAlignmentMask(i));
    }
    else {
        reslt = FALSE;
    }

    return reslt;
}

boolean
gotrolefilter()
{
    int i;

    if (roleFilterMask() > 0) {
        return TRUE;
    } else {
        return anyRoleFilters();
    }
}

void
clearrolefilter()
{
    resetRoleFilters();
    resetRoleFilterMask();
}

#define BP_ALIGN 0
#define BP_GEND 1
#define BP_RACE 2
#define BP_ROLE 3
#define NUM_BP 4

STATIC_VAR char pa[NUM_BP], post_attribs;

STATIC_OVL char *
promptsep(buf, num_post_attribs)
char *buf;
int num_post_attribs;
{
    const char *conjuct = "and ";

    if (num_post_attribs > 1 && post_attribs < num_post_attribs
        && post_attribs > 1)
        Strcat(buf, ",");
    Strcat(buf, " ");
    --post_attribs;
    if (!post_attribs && num_post_attribs > 1)
        Strcat(buf, conjuct);
    return buf;
}

STATIC_OVL int
role_gendercount(rolenum)
int rolenum;
{
    int gendcount = 0;

    if (validrole(rolenum)) {
        long mask = startingMaskForRole(rolenum);	     
        if (mask & ROLE_MALE)
            ++gendcount;
        if (mask & ROLE_FEMALE)
            ++gendcount;
        if (mask & ROLE_NEUTER)
            ++gendcount;
    }
    return gendcount;
}

STATIC_OVL int
race_alignmentcount(racenum)
int racenum;
{
    int aligncount = 0;

    if (racenum != roleNone() && racenum != roleRandom()) {
        if (speciesCanStartAsChaotic(racenum))
            ++aligncount;
        if (speciesCanStartAsLawful(racenum))
            ++aligncount;
        if (speciesCanStartAsNeutral(racenum))
            ++aligncount;
    }
    return aligncount;
}

char *
root_plselection_prompt(suppliedbuf, buflen, rolenum, racenum, gendnum,
                        alignnum)
char *suppliedbuf;
int buflen, rolenum, racenum, gendnum, alignnum;
{
    int k, gendercount = 0, aligncount = 0;
    char buf[BUFSZ];
    static char err_ret[] = " character's";
    boolean donefirst = FALSE;

    if (!suppliedbuf || buflen < 1)
        return err_ret;

    /* initialize these static variables each time this is called */
    post_attribs = 0;
    for (k = 0; k < NUM_BP; ++k)
        pa[k] = 0;
    buf[0] = '\0';
    *suppliedbuf = '\0';

    /* How many alignments are allowed for the desired race? */
    if (racenum != roleNone() && racenum != roleRandom())
        aligncount = race_alignmentcount(racenum);

    if (alignnum != roleNone() && alignnum != roleRandom()
        && ok_align(rolenum, racenum, gendnum, alignnum)) {
        /* if race specified, and multiple choice of alignments for it */
        if ((racenum >= 0) && (aligncount > 1)) {
            if (donefirst)
                Strcat(buf, " ");
            Strcat(buf, aligns[alignnum]);
            donefirst = TRUE;
        } else {
            if (donefirst)
                Strcat(buf, " ");
            Strcat(buf, aligns[alignnum]);
            donefirst = TRUE;
        }
    } else {
        /* in case we got here by failing the ok_align() test */
        if (alignnum != roleRandom())
            alignnum = roleNone();
        /* if alignment not specified, but race is specified
           and only one choice of alignment for that race then
           don't include it in the later list */
        if ((((racenum != roleNone() && racenum != roleRandom())
              && ok_race(rolenum, racenum, gendnum, alignnum))
             && (aligncount > 1))
            || (racenum == roleNone() || racenum == roleRandom())) {
            pa[BP_ALIGN] = 1;
            post_attribs++;
        }
    }
    /* <your lawful> */

    /* How many genders are allowed for the desired role? */
    if (validrole(rolenum))
        gendercount = role_gendercount(rolenum);

    if (gendnum != roleNone() && gendnum != roleRandom()) {
        if (validrole(rolenum)) {
            /* if role specified, and multiple choice of genders for it,
               and name of role itself does not distinguish gender */
            if ((rolenum != roleNone()) && (gendercount > 1)
                && !roleNameHasFemaleVersion(rolenum)) {
                if (donefirst)
		{
                    Strcat(buf, " ");
		}
		javaString adjective = genderAdjective(gendnum);
                Strcat(buf, adjective.c_str);
		releaseJavaString(adjective);
                donefirst = TRUE;
            }
        } else {
            if (donefirst)
                Strcat(buf, " ");
	    javaString adjective = genderAdjective(gendnum);
            Strcat(buf, adjective.c_str);
	    releaseJavaString(adjective);
            donefirst = TRUE;
        }
    } else {
        /* if gender not specified, but role is specified
                and only one choice of gender then
                don't include it in the later list */
        if ((validrole(rolenum) && (gendercount > 1))
            || !validrole(rolenum)) {
            pa[BP_GEND] = 1;
            post_attribs++;
        }
    }
    /* <your lawful female> */

    if (racenum != roleNone() && racenum != roleRandom()) {
        if (validrole(rolenum)
            && ok_race(rolenum, racenum, gendnum, alignnum)) {
            if (donefirst)
                Strcat(buf, " ");
	    javaString noun = nounForSpecies(racenum);
	    javaString adjective = adjectiveForSpecies(racenum);

            Strcat(buf, (rolenum == roleNone()) ? noun.c_str
                                               : adjective.c_str);
	    releaseJavaString(noun);
	    releaseJavaString(adjective);

            donefirst = TRUE;
        } else if (!validrole(rolenum)) {
            if (donefirst)
                Strcat(buf, " ");
	    javaString noun = nounForSpecies(racenum);
            Strcat(buf, noun.c_str);
	    releaseJavaString(noun);
            donefirst = TRUE;
        } else {
            pa[BP_RACE] = 1;
            post_attribs++;
        }
    } else {
        pa[BP_RACE] = 1;
        post_attribs++;
    }
    /* <your lawful female gnomish> || <your lawful female gnome> */

    if (validrole(rolenum)) {
        if (donefirst)
            Strcat(buf, " ");

        javaString femaleRoleName = roleNameAsFemale(rolenum);
        javaString maleRoleName   = roleNameAsMale(rolenum);

        if (gendnum != roleNone()) {
            if (gendnum == 1 && roleNameHasFemaleVersion(rolenum))
	    {
                Strcat(buf, femaleRoleName.c_str);
	    }
            else
	    {
                Strcat(buf, maleRoleName.c_str);
	    }
        } else {
            if (roleNameHasFemaleVersion(rolenum)) {
                Strcat(buf, maleRoleName.c_str);
                Strcat(buf, "/");
                Strcat(buf, femaleRoleName.c_str);
            } else
                Strcat(buf, maleRoleName.c_str);
        }
        donefirst = TRUE;
    } else if (rolenum == roleNone()) {
        pa[BP_ROLE] = 1;
        post_attribs++;
    }

    if ((racenum == roleNone() || racenum == roleRandom())
        && !validrole(rolenum)) {
        if (donefirst)
            Strcat(buf, " ");
        Strcat(buf, "character");
        donefirst = TRUE;
    }
    /* <your lawful female gnomish cavewoman> || <your lawful female gnome>
     *    || <your lawful female character>
     */
    if (buflen > (int) (strlen(buf) + 1)) {
        Strcpy(suppliedbuf, buf);
        return suppliedbuf;
    } else
        return err_ret;
}

char *
build_plselection_prompt(buf, buflen, rolenum, racenum, gendnum, alignnum)
char *buf;
int buflen, rolenum, racenum, gendnum, alignnum;
{
    const char *defprompt = "Shall I pick a character for you? [ynaq] ";
    int num_post_attribs = 0;
    char tmpbuf[BUFSZ], *p;

    if (buflen < QBUFSZ)
        return (char *) defprompt;

    Strcpy(tmpbuf, "Shall I pick ");
    if (racenum != roleNone() || validrole(rolenum))
        Strcat(tmpbuf, "your ");
    else {
        Strcat(tmpbuf, "a ");
    }
    /* <your> */

    (void) root_plselection_prompt(eos(tmpbuf), buflen - strlen(tmpbuf),
                                   rolenum, racenum, gendnum, alignnum);
    Sprintf(buf, "%s", s_suffix(tmpbuf));
    /* don't bother splitting caveman/cavewoman or priest/priestess
       in order to apply possessive suffix to both halves, but do
       change "priest/priestess'" to "priest/priestess's" */
    if ((p = strstri(buf, "priest/priestess'")) != 0
        && p[sizeof "priest/priestess'" - sizeof ""] == '\0')
        strkitten(buf, 's');

    /* buf should now be:
     *    <your lawful female gnomish cavewoman's>
     * || <your lawful female gnome's>
     * || <your lawful female character's>
     *
     * Now append the post attributes to it
     */
    num_post_attribs = post_attribs;
    if (post_attribs) {
        if (pa[BP_RACE]) {
            (void) promptsep(eos(buf), num_post_attribs);
            Strcat(buf, "race");
        }
        if (pa[BP_ROLE]) {
            (void) promptsep(eos(buf), num_post_attribs);
            Strcat(buf, "role");
        }
        if (pa[BP_GEND]) {
            (void) promptsep(eos(buf), num_post_attribs);
            Strcat(buf, "gender");
        }
        if (pa[BP_ALIGN]) {
            (void) promptsep(eos(buf), num_post_attribs);
            Strcat(buf, "alignment");
        }
    }
    Strcat(buf, " for you? [ynaq] ");
    return buf;
}

#undef BP_ALIGN
#undef BP_GEND
#undef BP_RACE
#undef BP_ROLE
#undef NUM_BP

void
plnamesuffix()
{
    char *sptr, *eptr;
    int i;

#ifdef GENERIC_USERNAMES
    {
        /* some generic user names will be ignored in favor of prompting */
        const char *uptr = GENERIC_USERNAMES;

        i = (int) strlen(plname);
        if ((sptr = strstri(uptr, plname)) != 0
            && (sptr == uptr || sptr[-1] == ' ')
            && (sptr[i] == ' ' || sptr[i] == '\0'))
            *plname = '\0'; /* call askname() */
    }
#endif

    do {
        if (!*plname)
            askname(); /* fill plname[] if necessary */

        /* Look for tokens delimited by '-' */
        if ((eptr = index(plname, '-')) != (char *) 0)
            *eptr++ = '\0';
        while (eptr) {
            /* Isolate the next token */
            sptr = eptr;
            if ((eptr = index(sptr, '-')) != (char *) 0)
                *eptr++ = '\0';

            /* Try to match it to something */
            if ((i = str2role(sptr)) != roleNone())
                flags.initrole = i;
            else if ((i = str2race(sptr)) != roleNone())
                flags.initrace = i;
            else if ((i = str2gend(sptr)) != roleNone())
                flags.initgend = i;
            else if ((i = str2align(sptr)) != roleNone())
                flags.initalign = i;
        }
    } while (!*plname);

    /* commas in the plname confuse the record file, convert to spaces */
    for (sptr = plname; *sptr; sptr++) {
        if (*sptr == ',')
            *sptr = ' ';
    }
}

/* show current settings for name, role, race, gender, and alignment
   in the specified window */
void
role_selection_prolog(which, where)
int which;
winid where;
{
    static const char NEARDATA choosing[] = " choosing now",
                               not_yet[] = " not yet specified",
                               rand_choice[] = " random";
    char buf[BUFSZ];
    int r, c, g, a, allowmask;

    r = flags.initrole;
    c = flags.initrace;
    g = flags.initgend;
    a = flags.initalign;
    if (r >= 0) {
        allowmask = startingMaskForRole(r);
        if ((allowmask & ROLE_RACEMASK) == MH_HUMAN)
            c = 0; /* HUMAN_ID */
        else if (c >= 0 && !(allowmask & selfMaskForSpecies(c)))
            c = roleRandom();
        if ((allowmask & ROLE_GENDMASK) == ROLE_MALE)
            g = 0; /* role forces male (hypothetical) */
        else if ((allowmask & ROLE_GENDMASK) == ROLE_FEMALE)
            g = 1; /* role forces female (valkyrie) */
        if ((allowmask & ROLE_ALIGNMASK) == AM_LAWFUL)
            a = 0; /* aligns[lawful] */
        else if ((allowmask & ROLE_ALIGNMASK) == AM_NEUTRAL)
            a = 1; /* aligns[neutral] */
        else if ((allowmask & ROLE_ALIGNMASK) == AM_CHAOTIC)
            a = 2; /* alings[chaotic] */
    }
    if (c >= 0) {
        allowmask = startingMaskForSpecies(c);
        if ((allowmask & ROLE_ALIGNMASK) == AM_LAWFUL)
            a = 0; /* aligns[lawful] */
        else if ((allowmask & ROLE_ALIGNMASK) == AM_NEUTRAL)
            a = 1; /* aligns[neutral] */
        else if ((allowmask & ROLE_ALIGNMASK) == AM_CHAOTIC)
            a = 2; /* alings[chaotic] */
        /* [c never forces gender] */
    }
    /* [g and a don't constrain anything sufficiently
       to narrow something done to a single choice] */

    Sprintf(buf, "%12s ", "name:");
    Strcat(buf, (which == RS_NAME) ? choosing : !*plname ? not_yet : plname);
    putstr(where, 0, buf);
    Sprintf(buf, "%12s ", "role:");

    if (which == RS_ROLE) {
        Strcat(buf, choosing);
    }
    else if (r == roleNone())
    {
        Strcat(buf, not_yet);
    }
    else if (r == roleRandom())
    {
        Strcat(buf, rand_choice);
    }
    else
    {
        javaString roleName = roleNameAsMale(r);
        Strcat(buf, roleName.c_str);
	releaseJavaString(roleName);
    }

    if (r >= 0 && roleNameHasFemaleVersion(r)) {
        /* distinct female name [caveman/cavewoman, priest/priestess] */
        if (g == 1)
	{
            /* female specified; replace male role name with female one */
	    javaString femaleRoleName = roleNameAsFemale(r);
            Sprintf(index(buf, ':'), ": %s", femaleRoleName.c_str);
	    releaseJavaString(femaleRoleName);
	}
        else if (g < 0)
	{
            /* gender unspecified; append slash and female role name */
	    javaString femaleRoleName = roleNameAsFemale(r);
            Sprintf(eos(buf), "/%s", femaleRoleName.c_str);
	    releaseJavaString(femaleRoleName);
	}
    }

    putstr(where, 0, buf);
    Sprintf(buf, "%12s ", "race:");
    javaString speciesNoun = nounForSpecies(c);
    Strcat(buf, (which == RS_RACE) ? choosing : (c == roleNone())
                                                    ? not_yet
                                                    : (c == roleRandom())
                                                          ? rand_choice
                                                          : speciesNoun.c_str);
    releaseJavaString(speciesNoun);
    putstr(where, 0, buf);
    Sprintf(buf, "%12s ", "gender:");

    if (which == RS_GENDER) {
        Strcat(buf, choosing);
    } else if (g == roleNone()) {
        Strcat(buf, not_yet);
    } else if (g == roleRandom()) {
        Strcat(buf, rand_choice);
    } else {
	javaString adjective = genderAdjective(g);
        Strcat(buf, adjective.c_str);
	releaseJavaString(adjective);
    }

    putstr(where, 0, buf);
    Sprintf(buf, "%12s ", "alignment:");
    Strcat(buf, (which == RS_ALGNMNT) ? choosing : (a == roleNone())
                                                       ? not_yet
                                                       : (a == roleRandom())
                                                             ? rand_choice
                                                             : aligns[a]);
    putstr(where, 0, buf);
}

/* add a "pick alignment first"-type entry to the specified menu */
void
role_menu_extra(which, where)
int which;
winid where;
{
    static NEARDATA const char RS_menu_let[] = {
        '=',  /* name */
        '?',  /* role */
        '/',  /* race */
        '\"', /* gender */
        '[',  /* alignment */
    };
    anything any;
    char buf[BUFSZ];
    const char *what = 0, *constrainer = 0;
    javaString forcedValue = NO_JAVA_STRING;
    int f = 0, r, c, g, a, i, allowmask;

    r = flags.initrole;
    c = flags.initrace;
    switch (which) {
    case RS_NAME:
        what = "name";
        break;
    case RS_ROLE:
        what = "role";
        f = r;
        for (i = 0; i < SIZE(roleIDs); ++i)
            if (i != f && !roleFilter(i))
                break;
        if (i == SIZE(roleIDs)) {
            constrainer = "filter";
            forcedValue = javaStringFromC("role");
        }
        break;
    case RS_RACE:
        what = "race";
        f = flags.initrace;
        c = roleNone(); /* override player's setting */
        if (r >= 0) {
            allowmask = startingSpeciesMaskForRole(r);
            if (allowmask == MH_HUMAN)
                c = 0; /* HUMAN_ID */
            if (c >= 0) {
                constrainer = "role";
                forcedValue = nounForSpecies(c);
            } else if (f >= 0
                       && (allowmask & ~roleFilterMask()) == selfMaskForSpecies(f)) {
                /* if there is only one race choice available due to user
                   options disallowing others, race menu entry is disabled */
                constrainer = "filter";
                forcedValue = javaStringFromC("race");
            }
        }
        break;
    case RS_GENDER:
        what = "gender";
        f = flags.initgend;
        g = roleNone();
        if (r >= 0) {
            allowmask = startingGenderMaskForRole(r);
            if (allowmask == ROLE_MALE)
                g = 0; /* male */
            else if (allowmask == ROLE_FEMALE)
                g = 1; /* female */
            if (g >= 0) {
                constrainer = "role";
                forcedValue = genderAdjective(g);
            } else if (f >= 0
                       && (allowmask & ~roleFilterMask()) == genderMask(f)) {
                /* if there is only one gender choice available due to user
                   options disallowing other, gender menu entry is disabled */
                constrainer = "filter";
                forcedValue = javaStringFromC("gender");
            }
        }
        break;
    case RS_ALGNMNT:
        what = "alignment";
        f = flags.initalign;
        a = roleNone();
        if (r >= 0) {
            allowmask = startingAlignmentMaskForRole(r);
            if (allowmask == AM_LAWFUL)
                a = 0; /* aligns[lawful] */
            else if (allowmask == AM_NEUTRAL)
                a = 1; /* aligns[neutral] */
            else if (allowmask == AM_CHAOTIC)
                a = 2; /* aligns[chaotic] */
            if (a >= 0)
                constrainer = "role";
        }
        if (c >= 0 && !constrainer) {
            allowmask = startingAlignmentMaskForSpecies(c);
            if (allowmask == AM_LAWFUL)
                a = 0; /* aligns[lawful] */
            else if (allowmask == AM_NEUTRAL)
                a = 1; /* aligns[neutral] */
            else if (allowmask == AM_CHAOTIC)
                a = 2; /* aligns[chaotic] */
            if (a >= 0)
                constrainer = "race";
        }
        if (f >= 0 && !constrainer
            && (ROLE_ALIGNMASK & ~roleFilterMask()) == roleAlignmentMask(f)) {
            /* if there is only one alignment choice available due to user
               options disallowing others, algn menu entry is disabled */
            constrainer = "filter";
            forcedValue = javaStringFromC("alignment");
        }
        if (a >= 0)
            forcedValue = javaStringFromC(aligns[a]);
        break;
    }

    any = zeroany; /* zero out all bits */
    if (constrainer) {
        any.a_int = 0;
        /* use four spaces of padding to fake a grayed out menu choice */
        Sprintf(buf, "%4s%s forces %s", "", constrainer, forcedValue.c_str);
        add_menu(where, NO_GLYPH, &any, ' ', 0, ATR_NONE, buf,
                 MENU_UNSELECTED);
    } else if (what) {
        any.a_int = RS_menu_arg(which);
        Sprintf(buf, "Pick%s %s first", (f >= 0) ? " another" : "", what);
        add_menu(where, NO_GLYPH, &any, RS_menu_let[which], 0, ATR_NONE, buf,
                 MENU_UNSELECTED);
    } else if (which == RS_filter) {
        any.a_int = RS_menu_arg(RS_filter);
        add_menu(where, NO_GLYPH, &any, '~', 0, ATR_NONE,
                 "Reset role/race/&c filtering", MENU_UNSELECTED);
    } else if (which == roleRandom()) {
        any.a_int = roleRandom();
        add_menu(where, NO_GLYPH, &any, '*', 0, ATR_NONE, "Random",
                 MENU_UNSELECTED);
    } else if (which == roleNone()) {
        any.a_int = roleNone();
        add_menu(where, NO_GLYPH, &any, 'q', 0, ATR_NONE, "Quit",
                 MENU_UNSELECTED);
    } else {
        impossible("role_menu_extra: bad arg (%d)", which);
    }

    releaseJavaString(forcedValue);
}

/*
 *      Special setup modifications here:
 *
 *      Unfortunately, this is going to have to be done
 *      on each newgame or restore, because you lose the MonsterType mods
 *      across a save/restore.  :-)
 *
 *      1 - The Rogue Leader is the Tourist Nemesis.
 *      2 - Priests start with a random alignment - convert the leader and
 *          guardians here.
 *      3 - Priests also get their of deities from a randomly chosen role.
 *      4 - [obsolete] Elves can have one of two different leaders,
 *          but can't work it out here because it requires hacking the
 *          level file data (see sp_lev.c).
 *
 * This code also replaces quest_init().
 */
void
role_init()
{
    int alignmnt;
    int pmid;

    /* Strip the role letter out of the player name.
     * This is included for backwards compatibility.
     */
    plnamesuffix();

    /* Check for a valid role.  Try flags.initrole first. */
    if (!validrole(flags.initrole)) {
        /* Try the player letter second */
        if ((flags.initrole = str2role(pl_character)) < 0)
            /* None specified; pick a random role */
            flags.initrole = randrole_filtered();
    }

    /* We now have a valid role index.  Copy the role name back. */
    /* This should become OBSOLETE */
    javaString maleRoleName = roleNameAsMale(flags.initrole);
    Strcpy(pl_character, maleRoleName.c_str);
    releaseJavaString(maleRoleName);

    pl_character[PL_CSIZ - 1] = '\0';

    /* Check for a valid race */
    if (!validrace(flags.initrole, flags.initrace))
        flags.initrace = randrace(flags.initrole);

    /* Check for a valid gender.  If new game, check both initgend
     * and female.  On restore, assume flags.female is correct. */
    if (flags.pantheon == -1) { /* new game */
        if (!validgend(flags.initrole, flags.initrace, flags.female))
            flags.female = !flags.female;
    }
    if (!validgend(flags.initrole, flags.initrace, flags.initgend))
        /* Note that there is no way to check for an unspecified gender. */
        flags.initgend = flags.female;

    /* Check for a valid alignment */
    if (!validalign(flags.initrole, flags.initrace, flags.initalign))
        /* Pick a random alignment */
        flags.initalign = randalign(flags.initrole, flags.initrace);
    alignmnt = roleAlignmentType(flags.initalign);

    /* Initialize role and species */
    setYourCurrentRoleID(flags.initrole);
    setYourSpeciesID(flags.initrace);

    /* Fix up the quest leader */
    if (yourRoleHasQuestLeader()) {
        pmid = pmidOfQuestLeaderForYourRole();
        setMonsterSound(pmid, MS_LEADER);
        setMonsterFlag2(pmid, M2_PEACEFUL);
        setMonsterFlag3(pmid, M3_CLOSE);
        setMonsterAlignment(pmid, (alignmnt * 3));
        /* if gender is random, we choose it now instead of waiting
           until the leader monster is created */
        quest_status.ldrgend =
            isNeuter(pmid) ? 2 : isFemale(pmid) ? 1 : isMale(pmid)
                                                        ? 0
                                                        : (rn2(100) < 50);
    }

    /* Fix up the quest guardians */
    if (yourRoleHasQuestGuardians()) {
        pmid = pmidOfQuestGuardiansForYourRole();
        setMonsterFlag2(pmid, M2_PEACEFUL);
        setMonsterAlignment(pmid, (alignmnt * 3));
    }

    /* Fix up the quest nemesis */
    if (yourRoleHasQuestNemesis()) {
        pmid = pmidOfQuestNemesisForYourRole();
        setMonsterSound(pmid, MS_NEMESIS);
        unsetMonsterFlag2(pmid, M2_PEACEFUL);
        setMonsterFlag2(pmid, M2_NASTY | M2_STALK | M2_HOSTILE);
        unsetMonsterFlag3(pmid, M3_CLOSE);
        setMonsterFlag3(pmid, M3_WANTSARTI | M3_WAITFORU);
        /* if gender is random, we choose it now instead of waiting
           until the nemesis monster is created */
        quest_status.nemgend = isNeuter(pmid) ? 2 : isFemale(pmid) ? 1
                                   : isMale(pmid) ? 0 : (rn2(100) < 50);
    }

    /* Fix up the god names */
    if (flags.pantheon == -1) {             /* new game */
        flags.pantheon = flags.initrole;    /* use own gods */
    }

    while (!roleHasDefaultPantheon(flags.pantheon)) /* make sure they're present */
    {
            flags.pantheon = randrole();
    }

    if (-1 == yourCurrentPantheon()) {
        setYourCurrentPantheon(defaultPantheonForRole(flags.pantheon));
    }

    /* 0 or 1; no gods are neuter, nor is gender randomized */
    javaString deityTitle = titleOfAlignedDeityFromYourPantheon(alignmnt);
    quest_status.godgend = !strcmpi(deityTitle.c_str, "goddess");
    releaseJavaString(deityTitle);

    /* Fix up infravision */
    if (hasInfravision(yourSpeciesPMIDAsMale())) {
        /* although an infravision intrinsic is possible, infravision
         * is purely a property of the physical race.  This means that we
         * must put the infravision flag in the player's current race
         * (either that or have separate MonsterType entries for
         * elven/non-elven members of each class).  The side effect is that
         * all NPCs of that class will have (probably bogus) infravision,
         * but since infravision has no effect for NPCs anyway we can
         * ignore this.
         */
        setMonsterFlag3(malePMIDForYourRole(), M3_INFRAVISION);
        if (yourRoleHasFemalePMID()) {
            setMonsterFlag3(femalePMIDForYourRole(), M3_INFRAVISION);
	}
    }

    /* Artifacts are fixed in hack_artifacts() */

    /* Success! */
    return;
}

const char *
Hello(mtmp)
struct monst *mtmp;
{
    switch (yourRolePMID()) {
    case PM_KNIGHT:
        return "Salutations"; /* Olde English */
    case PM_SAMURAI:
        return (mtmp && (pmid4mon(mtmp) == PM_SHOPKEEPER))
                    ? "Irasshaimase"
                    : "Konnichi wa"; /* Japanese */
    case PM_TOURIST:
        return "Aloha"; /* Hawaiian */
    case PM_VALKYRIE:
        return "Velkommen"; /* Norse */
    default:
        return "Hello";
    }
}

const char *
Goodbye()
{
    switch (yourRolePMID()) {
    case PM_KNIGHT:
        return "Fare thee well"; /* Olde English */
    case PM_SAMURAI:
        return "Sayonara"; /* Japanese */
    case PM_TOURIST:
        return "Aloha"; /* Hawaiian */
    case PM_VALKYRIE:
        return "Farvel"; /* Norse */
    default:
        return "Goodbye";
    }
}

/* role.c */
