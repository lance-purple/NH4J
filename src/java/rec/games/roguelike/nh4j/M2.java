package rec.games.roguelike.nh4j;

public class M2 {

    public static final long NOPOLY = 0x00000001L;       /* players mayn't poly into one */
    public static final long UNDEAD = 0x00000002L;       /* is walking dead */
    public static final long WERE = 0x00000004L;         /* is a lycanthrope */
    public static final long HUMAN = 0x00000008L;        /* is a human */
    public static final long ELF = 0x00000010L;          /* is an elf */
    public static final long DWARF = 0x00000020L;        /* is a dwarf */
    public static final long GNOME = 0x00000040L;        /* is a gnome */
    public static final long ORC = 0x00000080L;          /* is an orc */
    public static final long DEMON = 0x00000100L;        /* is a demon */
    public static final long MERC = 0x00000200L;         /* is a guard or soldier */
    public static final long LORD = 0x00000400L;         /* is a lord to its kind */
    public static final long PRINCE = 0x00000800L;       /* is an overlord to its kind */
    public static final long MINION = 0x00001000L;       /* is a minion of a deity */
    public static final long GIANT = 0x00002000L;        /* is a giant */
    public static final long SHAPESHIFTER = 0x00004000L; /* is a shapeshifting species */
    public static final long MALE = 0x00010000L;         /* always male */
    public static final long FEMALE = 0x00020000L;       /* always female */
    public static final long NEUTER = 0x00040000L;       /* neither male nor female */
    public static final long PNAME = 0x00080000L;        /* monster name is a proper name */
    public static final long HOSTILE = 0x00100000L;      /* always starts hostile */
    public static final long PEACEFUL = 0x00200000L;     /* always starts peaceful */
    public static final long DOMESTIC = 0x00400000L;     /* can be tamed by feeding */
    public static final long WANDER = 0x00800000L;       /* wanders randomly */
    public static final long STALK = 0x01000000L;        /* follows you to other levels */
    public static final long NASTY = 0x02000000L;        /* extra-nasty monster (more xp) */
    public static final long STRONG = 0x04000000L;       /* strong (or big) monster */
    public static final long ROCKTHROW = 0x08000000L;    /* throws boulders */
    public static final long GREEDY = 0x10000000L;       /* likes gold */
    public static final long JEWELS = 0x20000000L;       /* likes gems */
    public static final long COLLECT = 0x40000000L;      /* picks up weapons and food */
    public static final long MAGIC = 0x80000000L;        /* picks up magic items */

}
