/* NetHack 3.6	hacklib.c	$NHDT-Date: 1446336792 2015/11/01 00:13:12 $  $NHDT-Branch: master $:$NHDT-Revision: 1.44 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* Copyright (c) Robert Patrick Rankin, 1991                      */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h" /* for config.h+extern.h */

/*=
    Assorted 'small' utility routines.  They're virtually independent of
    NetHack, except that rounddiv may call panic().  setrandom calls one
    of srandom(), srand48(), or srand() depending upon configuration.

      return type     routine name    argument type(s)
        boolean         digit           (char)
        boolean         letter          (char)
        char            highc           (char)
        char            lowc            (char)
        char *          lcase           (char *)
        char *          ucase           (char *)
        char *          upstart         (char *)
        char *          mungspaces      (char *)
        char *          eos             (char *)
        boolean         str_end_is      (const char *, const char *)
        char *          strkitten       (char *,char)
        void            copynchars      (char *,const char *,int)
        char            chrcasecpy      (int,int)
        char *          strcasecpy      (char *,const char *)
        char *          s_suffix        (const char *)
        char *          ing_suffix      (const char *)
        char *          xcrypt          (const char *, char *)
        boolean         onlyspace       (const char *)
        char *          tabexpand       (char *)
        char *          visctrl         (char)
        char *          strsubst        (char *, const char *, const char *)
        const char *    ordin           (int)
        char *          sitoa           (int)
        int             sgn             (int)
        int             rounddiv        (long, int)
        int             distmin         (int, int, int, int)
        int             dist2           (int, int, int, int)
        boolean         online2         (int, int)
        boolean         pmatch          (const char *, const char *)
        boolean         pmatchi         (const char *, const char *)
        boolean         pmatchz         (const char *, const char *)
        int             strncmpi        (const char *, const char *, int)
        char *          strstri         (const char *, const char *)
        boolean         fuzzymatch      (const char *,const char *,
                                         const char *, boolean)
        void            setrandom       (void)
        time_t          getnow          (void)
        int             getyear         (void)
        char *          yymmdd          (time_t)
        long            yyyymmdd        (time_t)
        long            hhmmss          (time_t)
        char *          yyyymmddhhmmss  (time_t)
        time_t          time_from_yyyymmddhhmmss (char *)
        int             phase_of_the_moon (void)
        boolean         friday_13th     (void)
        int             night           (void)
        int             midnight        (void)
=*/
#ifdef LINT
#define Static /* pacify lint */
#else
#define Static static
#endif

#define PLAYER_CHARACTER_CLASS "rec/games/roguelike/nh4j/PlayerCharacter"

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

int javaGetInt(const char* classname, const char* methodname) {
    jclass you_class = getJavaClass(classname);
    jmethodID method = getStaticMethod(you_class, methodname, "()I");
    return (*jni_env)->CallStaticIntMethod(jni_env, you_class, method);
}

int javaGetIntFromInt(const char* classname, const char* methodname, int i) {
    jclass you_class = getJavaClass(classname);
    jmethodID method = getStaticMethod(you_class, methodname, "(I)I");
    return (*jni_env)->CallStaticIntMethod(jni_env, you_class, method, i);
}

long javaGetLong(const char* classname, const char* methodname) {
    jclass you_class = getJavaClass(classname);
    jmethodID method = getStaticMethod(you_class, methodname, "()J");
    return (*jni_env)->CallStaticLongMethod(jni_env, you_class, method);
}

boolean javaGetBoolean(const char* classname, const char* methodname) {
    jclass you_class = getJavaClass(classname);
    jmethodID method = getStaticMethod(you_class, methodname, "()Z");
    return (*jni_env)->CallStaticBooleanMethod(jni_env, you_class, method);
}

void javaSetInt(const char* classname, const char* methodname, int v) {
    jclass you_class = getJavaClass(classname);
    jmethodID method = getStaticMethod(you_class, methodname, "(I)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, v);
}

void javaSetIntFromInt(const char* classname, const char* methodname, int i, int v) {
    jclass you_class = getJavaClass(classname);
    jmethodID method = getStaticMethod(you_class, methodname, "(II)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, i, v);
}

void javaSetLong(const char* classname, const char* methodname, long v) {
    jclass you_class = getJavaClass(classname);
    jmethodID method = getStaticMethod(you_class, methodname, "(J)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, v);
}

void javaSetBoolean(const char* classname, const char* methodname, boolean v) {
    jclass you_class = getJavaClass(classname);
    jmethodID method = getStaticMethod(you_class, methodname, "(Z)V");
    (*jni_env)->CallStaticVoidMethod(jni_env, you_class, method, v);
}

static boolean FDECL(pmatch_internal, (const char *, const char *,
                                       BOOLEAN_P, const char *));

/* is 'c' a digit? */
boolean
digit(c)
char c;
{
    return (boolean) ('0' <= c && c <= '9');
}

/* is 'c' a letter?  note: '@' classed as letter */
boolean
letter(c)
char c;
{
    return (boolean) ('@' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

/* force 'c' into uppercase */
char
highc(c)
char c;
{
    return (char) (('a' <= c && c <= 'z') ? (c & ~040) : c);
}

/* force 'c' into lowercase */
char
lowc(c)
char c;
{
    return (char) (('A' <= c && c <= 'Z') ? (c | 040) : c);
}

/* convert a string into all lowercase */
char *
lcase(s)
char *s;
{
    register char *p;

    for (p = s; *p; p++)
        if ('A' <= *p && *p <= 'Z')
            *p |= 040;
    return s;
}

/* convert a string into all uppercase */
char *
ucase(s)
char *s;
{
    register char *p;

    for (p = s; *p; p++)
        if ('a' <= *p && *p <= 'z')
            *p &= ~040;
    return s;
}

/* convert first character of a string to uppercase */
char *
upstart(s)
char *s;
{
    if (s)
        *s = highc(*s);
    return s;
}

/* remove excess whitespace from a string buffer (in place) */
char *
mungspaces(bp)
char *bp;
{
    register char c, *p, *p2;
    boolean was_space = TRUE;

    for (p = p2 = bp; (c = *p) != '\0'; p++) {
        if (c == '\n')
            break; /* treat newline the same as end-of-string */
        if (c == '\t')
            c = ' ';
        if (c != ' ' || !was_space)
            *p2++ = c;
        was_space = (c == ' ');
    }
    if (was_space && p2 > bp)
        p2--;
    *p2 = '\0';
    return bp;
}

/* return the end of a string (pointing at '\0') */
char *
eos(s)
register char *s;
{
    while (*s)
        s++; /* s += strlen(s); */
    return s;
}

/* determine whether 'str' ends in 'chkstr' */
boolean
str_end_is(str, chkstr)
const char *str, *chkstr;
{
    int clen = (int) strlen(chkstr);

    if ((int) strlen(str) >= clen)
        return (boolean) (!strncmp(eos((char *) str) - clen, chkstr, clen));
    return FALSE;
}

/* append a character to a string (in place): strcat(s, {c,'\0'}); */
char *
strkitten(s, c)
char *s;
char c;
{
    char *p = eos(s);

    *p++ = c;
    *p = '\0';
    return s;
}

/* truncating string copy */
void
copynchars(dst, src, n)
char *dst;
const char *src;
int n;
{
    /* copies at most n characters, stopping sooner if terminator reached;
       treats newline as input terminator; unlike strncpy, always supplies
       '\0' terminator so dst must be able to hold at least n+1 characters */
    while (n > 0 && *src != '\0' && *src != '\n') {
        *dst++ = *src++;
        --n;
    }
    *dst = '\0';
}

/* convert char nc into oc's case; mostly used by strcasecpy */
char
chrcasecpy(oc, nc)
int oc, nc;
{
#if 0 /* this will be necessary if we switch to <ctype.h> */
    oc = (int) (unsigned char) oc;
    nc = (int) (unsigned char) nc;
#endif
    if ('a' <= oc && oc <= 'z') {
        /* old char is lower case; if new char is upper case, downcase it */
        if ('A' <= nc && nc <= 'Z')
            nc += 'a' - 'A'; /* lowc(nc) */
    } else if ('A' <= oc && oc <= 'Z') {
        /* old char is upper case; if new char is lower case, upcase it */
        if ('a' <= nc && nc <= 'z')
            nc += 'A' - 'a'; /* highc(nc) */
    }
    return (char) nc;
}

/* overwrite string, preserving old chars' case;
   for case-insensitive editions of makeplural() and makesingular();
   src might be shorter, same length, or longer than dst */
char *
strcasecpy(dst, src)
char *dst;
const char *src;
{
    char *result = dst;
    int ic, oc, dst_exhausted = 0;

    /* while dst has characters, replace each one with corresponding
       character from src, converting case in the process if they differ;
       once dst runs out, propagate the case of its last character to any
       remaining src; if dst starts empty, it must be a pointer to the
       tail of some other string because we examine the char at dst[-1] */
    while ((ic = (int) *src++) != '\0') {
        if (!dst_exhausted && !*dst)
            dst_exhausted = 1;
        oc = (int) *(dst - dst_exhausted);
        *dst++ = chrcasecpy(oc, ic);
    }
    *dst = '\0';
    return result;
}

/* return a name converted to possessive */
char *
s_suffix(s)
const char *s;
{
    Static char buf[BUFSZ];

    Strcpy(buf, s);
    if (!strcmpi(buf, "it")) /* it -> its */
        Strcat(buf, "s");
    else if (!strcmpi(buf, "you")) /* you -> your */
        Strcat(buf, "r");
    else if (*(eos(buf) - 1) == 's') /* Xs -> Xs' */
        Strcat(buf, "'");
    else /* X -> X's */
        Strcat(buf, "'s");
    return buf;
}

/* construct a gerund (a verb formed by appending "ing" to a noun) */
char *
ing_suffix(s)
const char *s;
{
    const char *vowel = "aeiouy";
    static char buf[BUFSZ];
    char onoff[10];
    char *p;

    Strcpy(buf, s);
    p = eos(buf);
    onoff[0] = *p = *(p + 1) = '\0';
    if ((strlen(buf) > 4)
        && (!strcmpi(p - 3, " on") || !strcmpi(p - 4, " off")
            || !strcmpi(p - 5, " with"))) {
        p = strrchr(buf, ' ');
        Strcpy(onoff, p);
    }
    if (!index(vowel, *(p - 1)) && index(vowel, *(p - 2))
        && !index(vowel, *(p - 3))) {
        /* tip -> tipp + ing */
        *p = *(p - 1);
        *(p + 1) = '\0';
    } else if (!strcmpi(p - 2, "ie")) { /* vie -> vy + ing */
        *(p - 2) = 'y';
        *(p - 1) = '\0';
    } else if (*(p - 1) == 'e') /* grease -> greas + ing */
        *(p - 1) = '\0';
    Strcat(buf, "ing");
    if (onoff[0])
        Strcat(buf, onoff);
    return buf;
}

/* trivial text encryption routine (see makedefs) */
char *
xcrypt(str, buf)
const char *str;
char *buf;
{
    register const char *p;
    register char *q;
    register int bitmask;

    for (bitmask = 1, p = str, q = buf; *p; q++) {
        *q = *p++;
        if (*q & (32 | 64))
            *q ^= bitmask;
        if ((bitmask <<= 1) >= 32)
            bitmask = 1;
    }
    *q = '\0';
    return buf;
}

/* is a string entirely whitespace? */
boolean
onlyspace(s)
const char *s;
{
    for (; *s; s++)
        if (*s != ' ' && *s != '\t')
            return FALSE;
    return TRUE;
}

/* expand tabs into proper number of spaces */
char *
tabexpand(sbuf)
char *sbuf;
{
    char buf[BUFSZ];
    register char *bp, *s = sbuf;
    register int idx;

    if (!*s)
        return sbuf;
    /* warning: no bounds checking performed */
    for (bp = buf, idx = 0; *s; s++)
        if (*s == '\t') {
            do
                *bp++ = ' ';
            while (++idx % 8);
        } else {
            *bp++ = *s;
            idx++;
        }
    *bp = 0;
    return strcpy(sbuf, buf);
}

/* make a displayable string from a character */
char *
visctrl(c)
char c;
{
    Static char ccc[3];

    c &= 0177;
    ccc[2] = '\0';
    if (c < 040) {
        ccc[0] = '^';
        ccc[1] = c | 0100; /* letter */
    } else if (c == 0177) {
        ccc[0] = '^';
        ccc[1] = c & ~0100; /* '?' */
    } else {
        ccc[0] = c; /* printable character */
        ccc[1] = '\0';
    }
    return ccc;
}

/* substitute a word or phrase in a string (in place) */
/* caller is responsible for ensuring that bp points to big enough buffer */
char *
strsubst(bp, orig, replacement)
char *bp;
const char *orig, *replacement;
{
    char *found, buf[BUFSZ];

    if (bp) {
        found = strstr(bp, orig);
        if (found) {
            Strcpy(buf, found + strlen(orig));
            Strcpy(found, replacement);
            Strcat(bp, buf);
        }
    }
    return bp;
}

/* return the ordinal suffix of a number */
const char *
ordin(n)
int n;               /* note: should be non-negative */
{
    register int dd = n % 10;

    return (dd == 0 || dd > 3 || (n % 100) / 10 == 1) ? "th"
               : (dd == 1) ? "st" : (dd == 2) ? "nd" : "rd";
}

/* make a signed digit string from a number */
char *
sitoa(n)
int n;
{
    Static char buf[13];

    Sprintf(buf, (n < 0) ? "%d" : "+%d", n);
    return buf;
}

/* return the sign of a number: -1, 0, or 1 */
int
sgn(n)
int n;
{
    return (n < 0) ? -1 : (n != 0);
}

/* calculate x/y, rounding as appropriate */
int
rounddiv(x, y)
long x;
int y;
{
    int r, m;
    int divsgn = 1;

    if (y == 0)
        panic("division by zero in rounddiv");
    else if (y < 0) {
        divsgn = -divsgn;
        y = -y;
    }
    if (x < 0) {
        divsgn = -divsgn;
        x = -x;
    }
    r = x / y;
    m = x % y;
    if (2 * m >= y)
        r++;

    return divsgn * r;
}

/* distance between two points, in moves */
int
distmin(x0, y0, x1, y1)
int x0, y0, x1, y1;
{
    register int dx = x0 - x1, dy = y0 - y1;

    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;
    /*  The minimum number of moves to get from (x0,y0) to (x1,y1) is the
     *  larger of the [absolute value of the] two deltas.
     */
    return (dx < dy) ? dy : dx;
}

/* square of euclidean distance between pair of pts */
int
dist2(x0, y0, x1, y1)
int x0, y0, x1, y1;
{
    register int dx = x0 - x1, dy = y0 - y1;

    return dx * dx + dy * dy;
}

/* your current dungeon level and number */

int currentDungeonLevel() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentDungeonLevel");
}

int currentDungeonNumber() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentDungeonNumber");
}

void setCurrentDungeonLevel(int level) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentDungeonLevel", level);
}

void setCurrentDungeonNumber(int number) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentDungeonNumber", number);
}

/* the previous turn current dungeon level and number */

int previousDungeonNumber() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "previousDungeonNumber");
}

int previousDungeonLevel() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "previousDungeonLevel");
}

void setPreviousDungeonNumber(int number) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setPreviousDungeonNumber", number);
}

void setPreviousDungeonLevel(int level) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setPreviousDungeonLevel", level);
}

/* dungeon level you are being sent to, or current */

int sentToDungeonNumber() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "sentToDungeonNumber");
}

int sentToDungeonLevel() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "sentToDungeonLevel");
}

void setSentToDungeonNumber(int number) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setSentToDungeonNumber", number);
}

void setSentToDungeonLevel(int level) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setSentToDungeonLevel", level);
}

int typeOfLevelYouWereSentTo() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "typeOfLevelYouWereSentTo");
}

void setTypeOfLevelYouWereSentTo(int mask) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setTypeOfLevelYouWereSentTo", mask);
}

/* your current position */

int currentX() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentX");
}

int currentY() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentY");
}

void setCurrentX(int x) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentX", x);
}

void setCurrentY(int y) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentY", y);
}

/* your direction of movement (or spellcasting etc.) */

int directionX() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "directionX");
}

int directionY() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "directionY");
}

int directionZ() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "directionZ");
}

void setDirectionX(int x) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setDirectionX", x);
}

void setDirectionY(int y) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setDirectionY", y);
}

void setDirectionZ(int z) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setDirectionZ", z);
}

/* your original position */

int originalX() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "originalX");
}

int originalY() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "originalY");
}

void setOriginalX(int x) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setOriginalX", x);
}

void setOriginalY(int y) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setOriginalY", y);
}

/* your travel destination */

int destinationX() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "destinationX");
}

int destinationY() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "destinationY");
}

void setDestinationX(int x) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setDestinationX", x);
}

void setDestinationY(int y) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setDestinationY", y);
}

boolean youMoved() {
    return javaGetBoolean(PLAYER_CHARACTER_CLASS, "youMoved");
} 

void setYouMoved(boolean m) {
    javaSetBoolean(PLAYER_CHARACTER_CLASS, "setYouMoved", m);
}

int lastTurningDirection() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "lastTurningDirection");
} 

void setLastTurningDirection(int d) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setLastTurningDirection", d);
}

int currentTrapType() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentTrapType");
}

int currentTrapTimeout() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentTrapTimeout");
}

boolean currentlyTrapped() {
    return (currentTrapTimeout() > 0);
}

void setCurrentTrapType(int type) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentTrapType", type);
}

void setCurrentTrapTimeout(int timeout) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentTrapTimeout", timeout);
}

long sleepingSinceMove() {
    return javaGetLong(PLAYER_CHARACTER_CLASS, "sleepingSinceMove");
}

void setSleepingSinceMove(long m) {
    javaSetLong(PLAYER_CHARACTER_CLASS, "setSleepingSinceMove", m);
}

int timeInVault() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "timeInVault");
}

void setTimeInVault(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setTimeInVault", t);
}

void increaseTimeInVault(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseTimeInVault", t);
}

void decreaseTimeInVault(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseTimeInVault", t);
}

int timeSinceBeingSwallowed() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "timeSinceBeingSwallowed");
}

void setTimeSinceBeingSwallowed(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setTimeSinceBeingSwallowed", t);
}

void increaseTimeSinceBeingSwallowed(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseTimeSinceBeingSwallowed", t);
}

void decreaseTimeSinceBeingSwallowed(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseTimeSinceBeingSwallowed", t);
}

int timeSinceBecomingADemigod() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "timeSinceBecomingADemigod");
}

void setTimeSinceBecomingADemigod(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setTimeSinceBecomingADemigod", t);
}

void increaseTimeSinceBecomingADemigod(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseTimeSinceBecomingADemigod", t);
}

void decreaseTimeSinceBecomingADemigod(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseTimeSinceBecomingADemigod", t);
}

int currentExperienceLevel() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentExperienceLevel");
} 

int highestExperienceLevelSoFar() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "highestExperienceLevelSoFar");
} 

void setCurrentExperienceLevel(int e) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentExperienceLevel", e);
}

void setHighestExperienceLevelSoFar(int e) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setHighestExperienceLevelSoFar", e);
}

long currentXP() {
    return javaGetLong(PLAYER_CHARACTER_CLASS, "currentXP");
} 

void setCurrentXP(long xp) {
    javaSetLong(PLAYER_CHARACTER_CLASS, "setCurrentXP", xp);
}

long currentScore() {
    return javaGetLong(PLAYER_CHARACTER_CLASS, "currentScore");
}

void setCurrentScore(long score) {
    javaSetLong(PLAYER_CHARACTER_CLASS, "setCurrentScore", score);
}

void increaseCurrentScore(long score) {
    javaSetLong(PLAYER_CHARACTER_CLASS, "increaseCurrentScore", score);
}

void decreaseCurrentScore(long score) {
    javaSetLong(PLAYER_CHARACTER_CLASS, "decreaseCurrentScore", score);
}

int currentLuck() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentLuck");
} 

void setCurrentLuck(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentLuck", c);
}

void increaseCurrentLuck(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseCurrentLuck", c);
}

void decreaseCurrentLuck(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseCurrentLuck", c);
}

int luckBonus() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "luckBonus");
} 

void setLuckBonus(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setLuckBonus", c);
}

int currentLuckWithBonus() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentLuckWithBonus");
} 

int minimumPossibleLuck() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "minimumPossibleLuck");
} 

int maximumPossibleLuck() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "maximumPossibleLuck");
} 

int blessings() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "blessings");
} 

void setBlessings(int b) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setBlessings", b);
}

void increaseBlessings(int b) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseBlessings", b);
}

void decreaseBlessings(int b) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseBlessings", b);
}

int timeToNextBlessing() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "timeToNextBlessing");
} 

void setTimeToNextBlessing(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setTimeToNextBlessing", t);
}

void increaseTimeToNextBlessing(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseTimeToNextBlessing", t);
}

void decreaseTimeToNextBlessing(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseTimeToNextBlessing", t);
}

long cleansedSinceMove() {
    return javaGetLong(PLAYER_CHARACTER_CLASS, "cleansedSinceMove");
} 

void setCleansedSinceMove(long t) {
    javaSetLong(PLAYER_CHARACTER_CLASS, "setCleansedSinceMove", t);
}

int divineWrath() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "divineWrath");
} 

void setDivineWrath(int w) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setDivineWrath", w);
}

void increaseDivineWrath(int w) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseDivineWrath", w);
}

void decreaseDivineWrath(int w) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseDivineWrath", w);
}

int toHitModifier() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "toHitModifier");
} 

void setToHitModifier(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setToHitModifier", c);
}

void increaseToHitModifier(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseToHitModifier", c);
}

void decreaseToHitModifier(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseToHitModifier", c);
}

int damageBonus() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "damageBonus");
} 

void setDamageBonus(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setDamageBonus", c);
}

void increaseDamageBonus(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseDamageBonus", c);
}

void decreaseDamageBonus(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseDamageBonus", c);
}

int armorClass() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "armorClass");
} 

void setArmorClass(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setArmorClass", c);
}

int armorBonusFromProtectionSpell() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "armorBonusFromProtectionSpell");
} 

void setArmorBonusFromProtectionSpell(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setArmorBonusFromProtectionSpell", c);
}

void increaseArmorBonusFromProtectionSpell(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseArmorBonusFromProtectionSpell", c);
}

void decreaseArmorBonusFromProtectionSpell(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseArmorBonusFromProtectionSpell", c);
}

int protectionSpellPointDuration() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "protectionSpellPointDuration");
} 

void setProtectionSpellPointDuration(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setProtectionSpellPointDuration", c);
}

int protectionSpellPointCountdown() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "protectionSpellPointCountdown");
} 

void setProtectionSpellPointCountdown(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setProtectionSpellPointCountdown", c);
}

void increaseProtectionSpellPointCountdown(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseProtectionSpellPointCountdown", c);
}

void decreaseProtectionSpellPointCountdown(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseProtectionSpellPointCountdown", c);
}

int originalMonsterNumber() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "originalMonsterNumber");
} 

int currentMonsterNumber() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentMonsterNumber");
} 

void setOriginalMonsterNumber(int n) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setOriginalMonsterNumber", n);
}

void setCurrentMonsterNumber(int n) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentMonsterNumber", n);
}

int lycanthropeType() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "lycanthropeType");
} 

void setLycanthropeType(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setLycanthropeType", t);
}

int timeRemainingAsMonster() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "timeRemainingAsMonster");
} 

void setTimeRemainingAsMonster(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setTimeRemainingAsMonster", t);
}

void increaseTimeRemainingAsMonster(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseTimeRemainingAsMonster", t);
}

void decreaseTimeRemainingAsMonster(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseTimeRemainingAsMonster", t);
}

void multiplyTimeRemainingAsMonster(int t) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "multiplyTimeRemainingAsMonster", t);
}

int currentHitPoints() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentHitPoints");
} 

void setCurrentHitPoints(int hp) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentHitPoints", hp);
}

void increaseCurrentHitPoints(int hp) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseCurrentHitPoints", hp);
}

void decreaseCurrentHitPoints(int hp) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseCurrentHitPoints", hp);
}

int hitPointIncreasePerLevel(int level) {
    return javaGetIntFromInt(PLAYER_CHARACTER_CLASS, "hitPointIncreasePerLevel", level);
}

void setHitPointIncreasePerLevel(int level, int hp) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "setHitPointIncreasePerLevel", level, hp);
}

int maximumHitPoints() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "maximumHitPoints");
} 

void setMaximumHitPoints(int hp) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setMaximumHitPoints", hp);
}

void increaseMaximumHitPoints(int hp) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseMaximumHitPoints", hp);
}

void decreaseMaximumHitPoints(int hp) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseMaximumHitPoints", hp);
}

int currentHitPointsAsMonster() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentHitPointsAsMonster");
} 

void setCurrentHitPointsAsMonster(int m) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentHitPointsAsMonster", m);
}

void increaseCurrentHitPointsAsMonster(int m) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseCurrentHitPointsAsMonster", m);
}

void decreaseCurrentHitPointsAsMonster(int m) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseCurrentHitPointsAsMonster", m);
}


int maximumHitPointsAsMonster() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "maximumHitPointsAsMonster");
} 

void setMaximumHitPointsAsMonster(int m) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setMaximumHitPointsAsMonster", m);
}

void increaseMaximumHitPointsAsMonster(int m) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseMaximumHitPointsAsMonster", m);
}

void decreaseMaximumHitPointsAsMonster(int m) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseMaximumHitPointsAsMonster", m);
}

void multiplyMaximumHitPointsAsMonster(int m) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "multiplyMaximumHitPointsAsMonster", m);
}

boolean areYouPolymorphed() {
    return (currentMonsterNumber() != originalMonsterNumber());
}

int currentMagicalEnergy() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentMagicalEnergy");
} 

void setCurrentMagicalEnergy(int n) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentMagicalEnergy", n);
}

void increaseCurrentMagicalEnergy(int n) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseCurrentMagicalEnergy", n);
}

void decreaseCurrentMagicalEnergy(int n) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseCurrentMagicalEnergy", n);
}

int maximumMagicalEnergy() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "maximumMagicalEnergy");
} 

void setMaximumMagicalEnergy(int n) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setMaximumMagicalEnergy", n);
}

void increaseMaximumMagicalEnergy(int n) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseMaximumMagicalEnergy", n);
}

void decreaseMaximumMagicalEnergy(int n) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseMaximumMagicalEnergy", n);
}

int magicalEnergyIncreasePerLevel(int level) {
    return javaGetIntFromInt(PLAYER_CHARACTER_CLASS, "magicalEnergyIncreasePerLevel", level);
}

void setMagicalEnergyIncreasePerLevel(int level, int en) {
    javaSetIntFromInt(PLAYER_CHARACTER_CLASS, "setMagicalEnergyIncreasePerLevel", level, en);
}

int currentNutrition() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentNutrition");
} 

int currentHungerState() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "currentHungerState");
} 

void setCurrentNutrition(int n) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentNutrition", n);
}

void increaseCurrentNutrition(int n) {
    setCurrentNutrition(currentNutrition() + n);
}

void decreaseCurrentNutrition(int n) {
    setCurrentNutrition(currentNutrition() - n);
}

void setCurrentHungerState(int s) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCurrentHungerState", s);
}

int abilityToConfuseMonsters() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "abilityToConfuseMonsters");
} 

void setAbilityToConfuseMonsters(int a) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setAbilityToConfuseMonsters", a);
}

void increaseAbilityToConfuseMonsters(a) int a; {
    setAbilityToConfuseMonsters(abilityToConfuseMonsters() + a);
}

void decreaseAbilityToConfuseMonsters(a) int a; {
    setAbilityToConfuseMonsters(abilityToConfuseMonsters() - a);
}

int creamed() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "creamed");
} 

void setCreamed(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setCreamed", c);
}

void increaseCreamed(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseCreamed", c);
}

void decreaseCreamed(int c) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseCreamed", c);
}

int nightVisionRange() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "nightVisionRange");
} 

int xRayVisionRange() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "xRayVisionRange");
} 

void setNightVisionRange(int r) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setNightVisionRange", r);
}

void setXRayVisionRange(int r) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setXRayVisionRange", r);
}

boolean feltBall() {
    return javaGetBoolean(PLAYER_CHARACTER_CLASS, "feltBall");
}

void setFeltBall(boolean f) {
    javaSetBoolean(PLAYER_CHARACTER_CLASS, "setFeltBall", f);
}

boolean feltChain() {
    return javaGetBoolean(PLAYER_CHARACTER_CLASS, "feltChain");
}

void setFeltChain(boolean f) {
    javaSetBoolean(PLAYER_CHARACTER_CLASS, "setFeltChain", f);
}

int glyphUnderBall() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "glyphUnderBall");
} 

int glyphUnderChain() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "glyphUnderChain");
} 

void setGlyphUnderBall(int g) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setGlyphUnderBall", g);
}

void setGlyphUnderChain(int g) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setGlyphUnderChain", g);
}

int ballAndChainOrder() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "ballAndChainOrder");
} 

void setBallAndChainOrder(int bc) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setBallAndChainOrder", bc);
}

boolean sickWithFoodPoisoning() {
    return javaGetBoolean(PLAYER_CHARACTER_CLASS, "sickWithFoodPoisoning");
} 

void setSickWithFoodPoisoning(boolean p) {
    javaSetBoolean(PLAYER_CHARACTER_CLASS, "setSickWithFoodPoisoning", p);
}

boolean sickWithIllness() {
    return javaGetBoolean(PLAYER_CHARACTER_CLASS, "sickWithIllness");
} 

void setSickWithIllness(boolean i) {
    javaSetBoolean(PLAYER_CHARACTER_CLASS, "setSickWithIllness", i);
}

int giftsBestowed() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "giftsBestowed");
}

void setGiftsBestowed(int g) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setGiftsBestowed", g);
}

void increaseGiftsBestowed(int g) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseGiftsBestowed", g);
}

void decreaseGiftsBestowed(int g) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseGiftsBestowed", g);
}

int startingMoney() {
    return javaGetInt(PLAYER_CHARACTER_CLASS, "startingMoney");
}

void setStartingMoney(int g) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "setStartingMoney", g);
}

void increaseStartingMoney(int g) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "increaseStartingMoney", g);
}

void decreaseStartingMoney(int g) {
    javaSetInt(PLAYER_CHARACTER_CLASS, "decreaseStartingMoney", g);
}

/* square of euclidean distance from pt to your current position */
int
distanceSquaredToYou(x, y)
int x, y;
{
    register int dx = currentX() - x;
    register int dy = currentY() - y;

    return (dx * dx) + (dy * dy);
}

/* integer square root function without using floating point */
int
isqrt(val)
int val;
{
    int rt = 0;
    int odd = 1;
    /*
     * This could be replaced by a faster algorithm, but has not been because:
     * + the simple algorithm is easy to read;
     * + this algorithm does not require 64-bit support;
     * + in current usage, the values passed to isqrt() are not really that
     *   large, so the performance difference is negligible;
     * + isqrt() is used in only few places, which are not bottle-necks.
     */
    while (val >= odd) {
        val = val - odd;
        odd = odd + 2;
        rt = rt + 1;
    }
    return rt;
}

/* is a point lined up (on a straight line) with you? */
boolean
linedUpWithYou(x, y)
int x, y;
{
    int dx = x - currentX(), dy = y - currentY();
    /*  If either delta is zero then they're on an orthogonal line,
     *  else if the deltas are equal (signs ignored) they're on a diagonal.
     */
    return (boolean) (!dy || !dx || dy == dx || dy == -dx);
}

/* guts of pmatch(), pmatchi(), and pmatchz();
   match a string against a pattern */
static boolean
pmatch_internal(patrn, strng, ci, sk)
const char *patrn, *strng;
boolean ci;     /* True => case-insensitive, False => case-sensitive */
const char *sk; /* set of characters to skip */
{
    char s, p;
    /*
     *  Simple pattern matcher:  '*' matches 0 or more characters, '?' matches
     *  any single character.  Returns TRUE if 'strng' matches 'patrn'.
     */
pmatch_top:
    if (!sk) {
        s = *strng++;
        p = *patrn++; /* get next chars and pre-advance */
    } else {
        /* fuzzy match variant of pmatch; particular characters are ignored */
        do {
            s = *strng++;
        } while (index(sk, s));
        do {
            p = *patrn++;
        } while (index(sk, p));
    }
    if (!p)                           /* end of pattern */
        return (boolean) (s == '\0'); /* matches iff end of string too */
    else if (p == '*')                /* wildcard reached */
        return (boolean) ((!*patrn
                           || pmatch_internal(patrn, strng - 1, ci, sk))
                          ? TRUE
                          : s ? pmatch_internal(patrn - 1, strng, ci, sk)
                              : FALSE);
    else if ((ci ? lowc(p) != lowc(s) : p != s) /* check single character */
             && (p != '?' || !s))               /* & single-char wildcard */
        return FALSE;                           /* doesn't match */
    else                 /* return pmatch_internal(patrn, strng, ci, sk); */
        goto pmatch_top; /* optimize tail recursion */
}

/* case-sensitive wildcard match */
boolean
pmatch(patrn, strng)
const char *patrn, *strng;
{
    return pmatch_internal(patrn, strng, FALSE, (const char *) 0);
}

/* case-insensitive wildcard match */
boolean
pmatchi(patrn, strng)
const char *patrn, *strng;
{
    return pmatch_internal(patrn, strng, TRUE, (const char *) 0);
}

/* case-insensitive wildcard fuzzymatch */
boolean
pmatchz(patrn, strng)
const char *patrn, *strng;
{
    /* ignore spaces, tabs (just in case), dashes, and underscores */
    static const char fuzzychars[] = " \t-_";

    return pmatch_internal(patrn, strng, TRUE, fuzzychars);
}

#ifndef STRNCMPI
/* case insensitive counted string comparison */
int
strncmpi(s1, s2, n) /*{ aka strncasecmp }*/
register const char *s1, *s2;
register int n; /*(should probably be size_t, which is unsigned)*/
{
    register char t1, t2;

    while (n--) {
        if (!*s2)
            return (*s1 != 0); /* s1 >= s2 */
        else if (!*s1)
            return -1; /* s1  < s2 */
        t1 = lowc(*s1++);
        t2 = lowc(*s2++);
        if (t1 != t2)
            return (t1 > t2) ? 1 : -1;
    }
    return 0; /* s1 == s2 */
}
#endif /* STRNCMPI */

#ifndef STRSTRI
/* case insensitive substring search */
char *
strstri(str, sub)
const char *str;
const char *sub;
{
    register const char *s1, *s2;
    register int i, k;
#define TABSIZ 0x20                  /* 0x40 would be case-sensitive */
    char tstr[TABSIZ], tsub[TABSIZ]; /* nibble count tables */
#if 0
    assert( (TABSIZ & ~(TABSIZ-1)) == TABSIZ ); /* must be exact power of 2 */
    assert( &lowc != 0 );                       /* can't be unsafe macro */
#endif

    /* special case: empty substring */
    if (!*sub)
        return (char *) str;

    /* do some useful work while determining relative lengths */
    for (i = 0; i < TABSIZ; i++)
        tstr[i] = tsub[i] = 0; /* init */
    for (k = 0, s1 = str; *s1; k++)
        tstr[*s1++ & (TABSIZ - 1)]++;
    for (s2 = sub; *s2; --k)
        tsub[*s2++ & (TABSIZ - 1)]++;

    /* evaluate the info we've collected */
    if (k < 0)
        return (char *) 0;       /* sub longer than str, so can't match */
    for (i = 0; i < TABSIZ; i++) /* does sub have more 'x's than str? */
        if (tsub[i] > tstr[i])
            return (char *) 0; /* match not possible */

    /* now actually compare the substring repeatedly to parts of the string */
    for (i = 0; i <= k; i++) {
        s1 = &str[i];
        s2 = sub;
        while (lowc(*s1++) == lowc(*s2++))
            if (!*s2)
                return (char *) &str[i]; /* full match */
    }
    return (char *) 0; /* not found */
}
#endif /* STRSTRI */

/* compare two strings for equality, ignoring the presence of specified
   characters (typically whitespace) and possibly ignoring case */
boolean
fuzzymatch(s1, s2, ignore_chars, caseblind)
const char *s1, *s2;
const char *ignore_chars;
boolean caseblind;
{
    register char c1, c2;

    do {
        while ((c1 = *s1++) != '\0' && index(ignore_chars, c1) != 0)
            continue;
        while ((c2 = *s2++) != '\0' && index(ignore_chars, c2) != 0)
            continue;
        if (!c1 || !c2)
            break; /* stop when end of either string is reached */

        if (caseblind) {
            c1 = lowc(c1);
            c2 = lowc(c2);
        }
    } while (c1 == c2);

    /* match occurs only when the end of both strings has been reached */
    return (boolean) (!c1 && !c2);
}

/*
 * Time routines
 *
 * The time is used for:
 *  - seed for rand()
 *  - year on tombstone and yyyymmdd in record file
 *  - phase of the moon (various monsters react to NEW_MOON or FULL_MOON)
 *  - night and midnight (the undead are dangerous at midnight)
 *  - determination of what files are "very old"
 */

/* TIME_type: type of the argument to time(); we actually use &(time_t) */
#if defined(BSD) && !defined(POSIX_TYPES)
#define TIME_type long *
#else
#define TIME_type time_t *
#endif
/* LOCALTIME_type: type of the argument to localtime() */
#if (defined(ULTRIX) && !(defined(ULTRIX_PROTO) || defined(NHSTDC))) \
    || (defined(BSD) && !defined(POSIX_TYPES))
#define LOCALTIME_type long *
#else
#define LOCALTIME_type time_t *
#endif

#if defined(AMIGA) && !defined(AZTEC_C) && !defined(__SASC_60) \
    && !defined(_DCC) && !defined(__GNUC__)
extern struct tm *FDECL(localtime, (time_t *));
#endif
STATIC_DCL struct tm *NDECL(getlt);

void
setrandom()
{
    unsigned long seed = (unsigned long) getnow(); /* time((TIME_type) 0) */

#if defined(UNIX) || defined(VMS)
    {
        unsigned long pid = (unsigned long) getpid();

        /* Quick dirty band-aid to prevent PRNG prediction */
        if (pid) {
            if (!(pid & 3L))
                pid -= 1L;
            seed *= pid;
        }
    }
#endif

    /* the types are different enough here that sweeping the different
     * routine names into one via #defines is even more confusing
     */
#ifdef RANDOM /* srandom() from sys/share/random.c */
    srandom((unsigned int) seed);
#else
#if defined(__APPLE__) || defined(BSD) || defined(LINUX) || defined(ULTRIX) \
    || defined(CYGWIN32) /* system srandom() */
#if defined(BSD) && !defined(POSIX_TYPES) && defined(SUNOS4)
    (void)
#endif
        srandom((int) seed);
#else
#ifdef UNIX /* system srand48() */
    srand48((long) seed);
#else       /* poor quality system routine */
    srand((int) seed);
#endif
#endif
#endif
}

time_t
getnow()
{
    time_t datetime = 0;

    (void) time((TIME_type) &datetime);
    return datetime;
}

STATIC_OVL struct tm *
getlt()
{
    time_t date = getnow();

    return localtime((LOCALTIME_type) &date);
}

int
getyear()
{
    return (1900 + getlt()->tm_year);
}

#if 0
/* This routine is no longer used since in 20YY it yields "1YYmmdd". */
char *
yymmdd(date)
time_t date;
{
    Static char datestr[10];
    struct tm *lt;

    if (date == 0)
        lt = getlt();
    else
        lt = localtime((LOCALTIME_type) &date);

    Sprintf(datestr, "%02d%02d%02d",
            lt->tm_year, lt->tm_mon + 1, lt->tm_mday);
    return datestr;
}
#endif

long
yyyymmdd(date)
time_t date;
{
    long datenum;
    struct tm *lt;

    if (date == 0)
        lt = getlt();
    else
        lt = localtime((LOCALTIME_type) &date);

    /* just in case somebody's localtime supplies (year % 100)
       rather than the expected (year - 1900) */
    if (lt->tm_year < 70)
        datenum = (long) lt->tm_year + 2000L;
    else
        datenum = (long) lt->tm_year + 1900L;
    /* yyyy --> yyyymm */
    datenum = datenum * 100L + (long) (lt->tm_mon + 1);
    /* yyyymm --> yyyymmdd */
    datenum = datenum * 100L + (long) lt->tm_mday;
    return datenum;
}

long
hhmmss(date)
time_t date;
{
    long timenum;
    struct tm *lt;

    if (date == 0)
        lt = getlt();
    else
        lt = localtime((LOCALTIME_type) &date);

    timenum = lt->tm_hour * 10000L + lt->tm_min * 100L + lt->tm_sec;
    return timenum;
}

char *
yyyymmddhhmmss(date)
time_t date;
{
    long datenum;
    static char datestr[15];
    struct tm *lt;

    if (date == 0)
        lt = getlt();
    else
#if (defined(ULTRIX) && !(defined(ULTRIX_PROTO) || defined(NHSTDC))) \
    || defined(BSD)
        lt = localtime((long *) (&date));
#else
        lt = localtime(&date);
#endif
    /* just in case somebody's localtime supplies (year % 100)
       rather than the expected (year - 1900) */
    if (lt->tm_year < 70)
        datenum = (long) lt->tm_year + 2000L;
    else
        datenum = (long) lt->tm_year + 1900L;
    Sprintf(datestr, "%04ld%02d%02d%02d%02d%02d", datenum, lt->tm_mon + 1,
            lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
    debugpline1("yyyymmddhhmmss() produced date string %s", datestr);
    return datestr;
}

time_t
time_from_yyyymmddhhmmss(buf)
char *buf;
{
    int k;
    time_t timeresult = (time_t) 0;
    struct tm t, *lt;
    char *g, *p, y[5], mo[3], md[3], h[3], mi[3], s[3];

    if (buf && strlen(buf) == 14) {
        g = buf;
        p = y; /* year */
        for (k = 0; k < 4; ++k)
            *p++ = *g++;
        *p = '\0';
        p = mo; /* month */
        for (k = 0; k < 2; ++k)
            *p++ = *g++;
        *p = '\0';
        p = md; /* day */
        for (k = 0; k < 2; ++k)
            *p++ = *g++;
        *p = '\0';
        p = h; /* hour */
        for (k = 0; k < 2; ++k)
            *p++ = *g++;
        *p = '\0';
        p = mi; /* minutes */
        for (k = 0; k < 2; ++k)
            *p++ = *g++;
        *p = '\0';
        p = s; /* seconds */
        for (k = 0; k < 2; ++k)
            *p++ = *g++;
        *p = '\0';
        lt = getlt();
        if (lt) {
            t = *lt;
            t.tm_year = atoi(y) - 1900;
            t.tm_mon = atoi(mo) - 1;
            t.tm_mday = atoi(md);
            t.tm_hour = atoi(h);
            t.tm_min = atoi(mi);
            t.tm_sec = atoi(s);
            timeresult = mktime(&t);
        }
        if ((int) timeresult == -1)
            debugpline1("time_from_yyyymmddhhmmss(%s) would have returned -1",
                        buf ? buf : "");
        else
            return timeresult;
    }
    return (time_t) 0;
}

/*
 * moon period = 29.53058 days ~= 30, year = 365.2422 days
 * days moon phase advances on first day of year compared to preceding year
 *      = 365.2422 - 12*29.53058 ~= 11
 * years in Metonic cycle (time until same phases fall on the same days of
 *      the month) = 18.6 ~= 19
 * moon phase on first day of year (epact) ~= (11*(year%19) + 29) % 30
 *      (29 as initial condition)
 * current phase in days = first day phase + days elapsed in year
 * 6 moons ~= 177 days
 * 177 ~= 8 reported phases * 22
 * + 11/22 for rounding
 */
int
phase_of_the_moon() /* 0-7, with 0: new, 4: full */
{
    register struct tm *lt = getlt();
    register int epact, diy, goldn;

    diy = lt->tm_yday;
    goldn = (lt->tm_year % 19) + 1;
    epact = (11 * goldn + 18) % 30;
    if ((epact == 25 && goldn > 11) || epact == 24)
        epact++;

    return ((((((diy + epact) * 6) + 11) % 177) / 22) & 7);
}

boolean
friday_13th()
{
    register struct tm *lt = getlt();

    /* tm_wday (day of week; 0==Sunday) == 5 => Friday */
    return (boolean) (lt->tm_wday == 5 && lt->tm_mday == 13);
}

int
night()
{
    register int hour = getlt()->tm_hour;

    return (hour < 6 || hour > 21);
}

int
midnight()
{
    return (getlt()->tm_hour == 0);
}

/*hacklib.c*/
