/* NetHack 3.6	questpgr.c	$NHDT-Date: 1448541043 2015/11/26 12:30:43 $  $NHDT-Branch: master $:$NHDT-Revision: 1.36 $ */
/*      Copyright 1991, M. Stephenson                             */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "dlb.h"

/*  quest-specific pager routines. */

#include "qtext.h"

#define QTEXT_FILE "quest.dat"

#ifdef TTY_GRAPHICS
#include "wintty.h"
#endif

/* from sp_lev.c, for deliver_splev_message() */
extern char *lev_message;

static void NDECL(dump_qtlist);
static void FDECL(Fread, (genericptr_t, int, int, dlb *));
STATIC_DCL struct qtmsg *FDECL(construct_qtlist, (long));
STATIC_DCL const char *NDECL(neminame);
STATIC_DCL const char *NDECL(guardname);
STATIC_DCL void FDECL(qtext_pronoun, (CHAR_P, CHAR_P));
STATIC_DCL struct qtmsg *FDECL(msg_in, (struct qtmsg *, int));
STATIC_DCL void FDECL(convert_arg, (CHAR_P));
STATIC_DCL void FDECL(convert_line, (char *,char *));
STATIC_DCL void FDECL(deliver_by_pline, (struct qtmsg *));
STATIC_DCL void FDECL(deliver_by_window, (struct qtmsg *, int));
STATIC_DCL boolean FDECL(skip_pager, (BOOLEAN_P));

static char cvt_buf[64];
static struct qtlists qt_list;
static dlb *msg_file;
/* used by ldrname() and neminame(), then copied into cvt_buf */
static char nambuf[sizeof cvt_buf];

/* dump the character msg list to check appearance;
   build with DEBUG enabled and use DEBUGFILES=questpgr.c
   in sysconf file or environment */
static void
dump_qtlist()
{
#ifdef DEBUG
    struct qtmsg *msg;

    if (!explicitdebug(__FILE__))
        return;

    for (msg = qt_list.chrole; msg->msgnum > 0; msg++) {
        (void) dlb_fseek(msg_file, msg->offset, SEEK_SET);
        deliver_by_window(msg, NHW_MAP);
    }
#endif /* DEBUG */
    return;
}

static void
Fread(ptr, size, nitems, stream)
genericptr_t ptr;
int size, nitems;
dlb *stream;
{
    int cnt;

    if ((cnt = dlb_fread(ptr, size, nitems, stream)) != nitems) {
        panic("PREMATURE EOF ON QUEST TEXT FILE! Expected %d bytes, got %d",
              (size * nitems), (size * cnt));
    }
}

STATIC_OVL struct qtmsg *
construct_qtlist(hdr_offset)
long hdr_offset;
{
    struct qtmsg *msg_list;
    int n_msgs;

    (void) dlb_fseek(msg_file, hdr_offset, SEEK_SET);
    Fread(&n_msgs, sizeof(int), 1, msg_file);
    msg_list = (struct qtmsg *) alloc((unsigned) (n_msgs + 1)
                                      * sizeof (struct qtmsg));

    /*
     * Load up the list.
     */
    Fread((genericptr_t) msg_list, n_msgs * sizeof (struct qtmsg), 1,
          msg_file);

    msg_list[n_msgs].msgnum = -1;
    return msg_list;
}

void
load_qtlist()
{
    int n_classes, i;
    char qt_classes[N_HDR][LEN_HDR];
    long qt_offsets[N_HDR];

    msg_file = dlb_fopen(QTEXT_FILE, RDBMODE);
    if (!msg_file)
        panic("CANNOT OPEN QUEST TEXT FILE %s.", QTEXT_FILE);

    /*
     * Read in the number of classes, then the ID's & offsets for
     * each header.
     */

    Fread(&n_classes, sizeof (int), 1, msg_file);
    Fread(&qt_classes[0][0], sizeof (char) * LEN_HDR, n_classes, msg_file);
    Fread(qt_offsets, sizeof (long), n_classes, msg_file);

    /*
     * Now construct the message lists for quick reference later
     * on when we are actually paging the messages out.
     */

    qt_list.common = qt_list.chrole = (struct qtmsg *) 0;

    for (i = 0; i < n_classes; i++) {
	javaString filecode = yourRoleFileCode();

        if (!strncmp(COMMON_ID, qt_classes[i], LEN_HDR))
            qt_list.common = construct_qtlist(qt_offsets[i]);
        else if (!strncmp(filecode.c_str, qt_classes[i], LEN_HDR))
            qt_list.chrole = construct_qtlist(qt_offsets[i]);

	releaseJavaString(filecode);
    }

    if (!qt_list.common || !qt_list.chrole)
        impossible("load_qtlist: cannot load quest text.");
    dump_qtlist();
    return; /* no ***DON'T*** close the msg_file */
}

/* called at program exit */
void
unload_qtlist()
{
    if (msg_file)
        (void) dlb_fclose(msg_file), msg_file = 0;
    if (qt_list.common)
        free((genericptr_t) qt_list.common), qt_list.common = 0;
    if (qt_list.chrole)
        free((genericptr_t) qt_list.chrole), qt_list.chrole = 0;
    return;
}

short
quest_info(typ)
int typ;
{
    switch (typ) {
    case 0:
        return idOfQuestArtifactForYourRole();
    case MS_LEADER:
        return pmidOfQuestLeaderForYourRole();
    case MS_NEMESIS:
        return pmidOfQuestNemesisForYourRole();
    case MS_GUARDIAN:
        return pmidOfQuestGuardiansForYourRole();
    default:
        impossible("quest_info(%d)", typ);
    }
    return 0;
}

/* return your role leader's name */
const char *
ldrname()
{
    int i = pmidOfQuestLeaderForYourRole();

    javaString leaderName = monsterTypeName(i);
    Sprintf(nambuf, "%s%s", typeIsProperName(i) ? "" : "the ",
            leaderName.c_str);
    releaseJavaString(leaderName);
    return nambuf;
}

boolean
is_quest_artifact(otmp)
struct obj *otmp;
{
    return (boolean) (otmp->oartifact == idOfQuestArtifactForYourRole());
}

/* return your role nemesis' name */
STATIC_OVL const char *
neminame()
{
    int i = pmidOfQuestNemesisForYourRole();

    javaString nemesisName = monsterTypeName(i);
    Sprintf(nambuf, "%s%s", typeIsProperName(i) ? "" : "the ",
            nemesisName.c_str);
    releaseJavaString(nemesisName);
    return nambuf;
}

STATIC_OVL const char *
guardname() /* return your role leader's guard monster name */
{
    javaString guardName = monsterTypeName(pmidOfQuestGuardiansForYourRole());
    Sprintf(nambuf, "%s", guardName.c_str);
    releaseJavaString(guardName);
    return nambuf;
}


/* replace deity, leader, nemesis, or artifact name with pronoun;
   overwrites cvt_buf[] */
STATIC_OVL void
qtext_pronoun(who, which)
char who,  /* 'd' => deity, 'l' => leader, 'n' => nemesis, 'o' => artifact */
    which; /* 'h'|'H'|'i'|'I'|'j'|'J' */
{
    javaString pnoun = NO_JAVA_STRING;
    int g;
    char lwhich = lowc(which); /* H,I,J -> h,i,j */

    /*
     * Invalid subject (not d,l,n,o) yields neuter, singular result.
     *
     * For %o, treat all artifacts as neuter; some have plural names,
     * which we don't handle; cvt_buf[] already contains name.
     */
    if (who == 'o'
        && (strstri(cvt_buf, "Eyes ")
            || strcmpi(cvt_buf, makesingular(cvt_buf)))) {
        pnoun = (lwhich == 'h') ? javaStringFromC("they")
                : (lwhich == 'i') ? javaStringFromC("them")
                : (lwhich == 'j') ? javaStringFromC("their")
		: javaStringFromC("?");
    } else {
        g = (who == 'd') ? quest_status.godgend
            : (who == 'l') ? quest_status.ldrgend
            : (who == 'n') ? quest_status.nemgend
            : 2; /* default to neuter */
        pnoun = (lwhich == 'h') ? subjectivePronoun(g)
                : (lwhich == 'i') ? objectivePronoun(g)
                : (lwhich == 'j') ? possessivePronoun(g)
		: javaStringFromC("?");
    }
    Strcpy(cvt_buf, pnoun.c_str);
    releaseJavaString(pnoun);

    /* capitalize for H,I,J */
    if (lwhich != which)
        cvt_buf[0] = highc(cvt_buf[0]);
    return;
}

STATIC_OVL struct qtmsg *
msg_in(qtm_list, msgnum)
struct qtmsg *qtm_list;
int msgnum;
{
    struct qtmsg *qt_msg;

    for (qt_msg = qtm_list; qt_msg->msgnum > 0; qt_msg++)
        if (qt_msg->msgnum == msgnum)
            return qt_msg;

    return (struct qtmsg *) 0;
}

STATIC_OVL void
convert_arg(c)
char c;
{
    javaString jstr;

    switch (c) {
    case 'p':
        jstr = javaStringFromC(plname);
        break;
    case 'c':
        jstr = (flags.female && roleNameHasFemaleVersion(yourCurrentRoleID()))
		? roleNameAsFemale(yourCurrentRoleID())
		: roleNameAsMale(yourCurrentRoleID());
        break;
    case 'r':
        jstr = rankOf(currentExperienceLevel(), yourRolePMID(), flags.female);
        break;
    case 'R':
        jstr = rankOf(MIN_QUEST_LEVEL, yourRolePMID(), flags.female);
        break;
    case 's':
        jstr = (flags.female) ? javaStringFromC("sister") : javaStringFromC("brother");
        break;
    case 'S':
        jstr = (flags.female) ? javaStringFromC("daughter") : javaStringFromC("son");
        break;
    case 'l':
        jstr = javaStringFromC(ldrname());
        break;
    case 'i':
        jstr = yourIntermediateQuestLevel();
        break;
    case 'O':
    case 'o':
        jstr = javaStringFromC(the(artiname(idOfQuestArtifactForYourRole())));
        if (c == 'O') {
            /* shorten "the Foo of Bar" to "the Foo"
               (buffer returned by the() is modifiable) */
            char *p = strstri(jstr.c_str, " of ");

            if (p)
                *p = '\0';
        }
        break;
    case 'n':
        jstr = javaStringFromC(neminame());
        break;
    case 'g':
        jstr = javaStringFromC(guardname());
        break;
    case 'G':
        jstr = titleOfAlignedDeityFromYourPantheon(originalAlignmentBase());
        break;
    case 'H':
        jstr = yourQuestHomeBase();
        break;
    case 'a':
        jstr = javaStringFromC(align_str(originalAlignmentBase()));
        break;
    case 'A':
        jstr = javaStringFromC(align_str(currentAlignmentType()));
        break;
    case 'd':
        jstr = nameOfAlignedDeityFromYourPantheon(originalAlignmentBase());
        break;
    case 'D':
        jstr = nameOfAlignedDeityFromYourPantheon(A_LAWFUL);
        break;
    case 'C':
        jstr = javaStringFromC("chaotic");
        break;
    case 'N':
        jstr = javaStringFromC("neutral");
        break;
    case 'L':
        jstr = javaStringFromC("lawful");
        break;
    case 'x':
        jstr = youCannotSee() ? javaStringFromC("sense") : javaStringFromC("see");
        break;
    case 'Z':
        jstr = javaStringFromC(dungeons[0].dname);
        break;
    case '%':
        jstr = javaStringFromC("%");
        break;
    default:
        jstr = javaStringFromC("");
        break;
    }
    Strcpy(cvt_buf, jstr.c_str);
    releaseJavaString(jstr);
}

STATIC_OVL void
convert_line(in_line, out_line)
char *in_line, *out_line;
{
    char *c, *cc;
    char xbuf[BUFSZ];

    cc = out_line;
    for (c = xcrypt(in_line, xbuf); *c; c++) {
        *cc = 0;
        switch (*c) {
        case '\r':
        case '\n':
            *(++cc) = 0;
            return;

        case '%':
            if (*(c + 1)) {
                convert_arg(*(++c));
                switch (*(++c)) {
                /* insert "a"/"an" prefix */
                case 'A':
                    Strcat(cc, An(cvt_buf));
                    cc += strlen(cc);
                    continue; /* for */
                case 'a':
                    Strcat(cc, an(cvt_buf));
                    cc += strlen(cc);
                    continue; /* for */

                /* capitalize */
                case 'C':
                    cvt_buf[0] = highc(cvt_buf[0]);
                    break;

                /* replace name with pronoun;
                   valid for %d, %l, %n, and %o */
                case 'h': /* he/she */
                case 'H': /* He/She */
                case 'i': /* him/her */
                case 'I':
                case 'j': /* his/her */
                case 'J':
                    if (index("dlno", lowc(*(c - 1))))
                        qtext_pronoun(*(c - 1), *c);
                    else
                        --c; /* default action */
                    break;

                /* pluralize */
                case 'P':
                    cvt_buf[0] = highc(cvt_buf[0]);
                case 'p':
                    Strcpy(cvt_buf, makeplural(cvt_buf));
                    break;

                /* append possessive suffix */
                case 'S':
                    cvt_buf[0] = highc(cvt_buf[0]);
                case 's':
                    Strcpy(cvt_buf, s_suffix(cvt_buf));
                    break;

                /* strip any "the" prefix */
                case 't':
                    if (!strncmpi(cvt_buf, "the ", 4)) {
                        Strcat(cc, &cvt_buf[4]);
                        cc += strlen(cc);
                        continue; /* for */
                    }
                    break;

                default:
                    --c; /* undo switch increment */
                    break;
                }
                Strcat(cc, cvt_buf);
                cc += strlen(cvt_buf);
                break;
            } /* else fall through */

        default:
            *cc++ = *c;
            break;
        }
    }
    if (cc > &out_line[BUFSZ-1])
        panic("convert_line: overflow");
    *cc = 0;
    return;
}

STATIC_OVL void
deliver_by_pline(qt_msg)
struct qtmsg *qt_msg;
{
    long size;
    char in_line[BUFSZ], out_line[BUFSZ];

    *in_line = '\0';
    for (size = 0; size < qt_msg->size; size += (long) strlen(in_line)) {
        (void) dlb_fgets(in_line, sizeof in_line, msg_file);
        convert_line(in_line, out_line);
        pline("%s", out_line);
    }
}

STATIC_OVL void
deliver_by_window(qt_msg, how)
struct qtmsg *qt_msg;
int how;
{
    long size;
    char in_line[BUFSZ], out_line[BUFSZ];
    boolean qtdump = (how == NHW_MAP);
    winid datawin = create_nhwindow(qtdump ? NHW_TEXT : how);

#ifdef DEBUG
    if (qtdump) {
        char buf[BUFSZ];

        /* when dumping quest messages at startup, all of them are passed to
         * deliver_by_window(), even if normally given to deliver_by_pline()
         */
        Sprintf(buf, "msgnum: %d, delivery: %c",
                qt_msg->msgnum, qt_msg->delivery);
        putstr(datawin, 0, buf);
        putstr(datawin, 0, "");
    }
#endif
    for (size = 0; size < qt_msg->size; size += (long) strlen(in_line)) {
        (void) dlb_fgets(in_line, sizeof in_line, msg_file);
        convert_line(in_line, out_line);
        putstr(datawin, 0, out_line);
    }
    display_nhwindow(datawin, TRUE);
    destroy_nhwindow(datawin);

    /* block messages delivered by window aren't kept in message history
       but have a one-line summary which is put there for ^P recall */
    *out_line = '\0';
    if (qt_msg->summary_size) {
        (void) dlb_fgets(in_line, sizeof in_line, msg_file);
        convert_line(in_line, out_line);
#ifdef BETA
    } else if (qt_msg->delivery == 'c') { /* skip for 'qtdump' of 'p' */
        /* delivery 'c' and !summary_size, summary expected but not present;
           this doesn't prefix the number with role code vs 'general'
           but should be good enough for summary verification purposes */
        Sprintf(out_line, "[missing block message summary for #%05d]",
                qt_msg->msgnum);
#endif
    }
    if (*out_line)
        putmsghistory(out_line, FALSE);
}

boolean
skip_pager(common)
boolean common;
{
    /* WIZKIT: suppress plot feedback if starting with quest artifact */
    if (program_state.wizkit_wishing)
        return TRUE;
    if (!(common ? qt_list.common : qt_list.chrole)) {
        panic("%s: no %s quest text data available",
              common ? "com_pager" : "qt_pager",
              common ? "common" : "role-specific");
        /*NOTREACHED*/
        return TRUE;
    }
    return FALSE;
}

void
com_pager(msgnum)
int msgnum;
{
    struct qtmsg *qt_msg;

    if (skip_pager(TRUE))
        return;

    if (!(qt_msg = msg_in(qt_list.common, msgnum))) {
        impossible("com_pager: message %d not found.", msgnum);
        return;
    }

    (void) dlb_fseek(msg_file, qt_msg->offset, SEEK_SET);
    if (qt_msg->delivery == 'p')
        deliver_by_pline(qt_msg);
    else if (msgnum == 1)
        deliver_by_window(qt_msg, NHW_MENU);
    else
        deliver_by_window(qt_msg, NHW_TEXT);
    return;
}

void
qt_pager(msgnum)
int msgnum;
{
    struct qtmsg *qt_msg;

    if (skip_pager(FALSE))
        return;

    if (!(qt_msg = msg_in(qt_list.chrole, msgnum))) {
        impossible("qt_pager: message %d not found.", msgnum);
        return;
    }

    (void) dlb_fseek(msg_file, qt_msg->offset, SEEK_SET);
    if (qt_msg->delivery == 'p' && strcmp(windowprocs.name, "X11"))
        deliver_by_pline(qt_msg);
    else
        deliver_by_window(qt_msg, NHW_TEXT);
    return;
}

int questMonsterType()
{
    if (rn2(5)) {
        int qpm = pmidOfQuestEnemiesForYourRole();
        if (qpm != NON_PM && rn2(5) && !(mvitals[qpm].mvflags & G_GENOD)) {
            return qpm;
	}
        return pickMonsterTypeOfClass(classOfQuestEnemiesForYourRole(), 0);
    } else {
        int qpm = pmidOfOtherQuestEnemiesForYourRole();
        if (qpm != NON_PM && rn2(5) && !(mvitals[qpm].mvflags & G_GENOD)) {
            return qpm;
        }
        return pickMonsterTypeOfClass(classOfOtherQuestEnemiesForYourRole(), 0);
    }
}

/* special levels can include a custom arrival message; display it */
void
deliver_splev_message()
{
    char *str, *nl, in_line[BUFSZ], out_line[BUFSZ];

    /* there's no provision for delivering via window instead of pline */
    if (lev_message) {
        /* lev_message can span multiple lines using embedded newline chars;
           any segments too long to fit within in_line[] will be truncated */
        for (str = lev_message; *str; str = nl + 1) {
            /* copying will stop at newline if one is present */
            copynchars(in_line, str, (int) (sizeof in_line) - 1);

            /* convert_line() expects encrypted input */
            (void) xcrypt(in_line, in_line);
            convert_line(in_line, out_line);
            pline("%s", out_line);

            if ((nl = index(str, '\n')) == 0)
                break; /* done if no newline */
        }

        free((genericptr_t) lev_message);
        lev_message = 0;
    }
}

/*questpgr.c*/
