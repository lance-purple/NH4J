package rec.games.roguelike.nh4j;

public class M1 {

    public static final long FLY = 0x00000001L;         /* can fly or float */
    public static final long SWIM = 0x00000002L;        /* can traverse water */
    public static final long AMORPHOUS = 0x00000004L;   /* can flow under doors */
    public static final long WALLWALK = 0x00000008L;    /* can phase thru rock */
    public static final long CLING = 0x00000010L;       /* can cling to ceiling */
    public static final long TUNNEL = 0x00000020L;      /* can tunnel thru rock */
    public static final long NEEDPICK = 0x00000040L;    /* needs pick to tunnel */
    public static final long CONCEAL = 0x00000080L;     /* hides under objects */
    public static final long HIDE = 0x00000100L;        /* mimics, blends in with ceiling */
    public static final long AMPHIBIOUS = 0x00000200L;  /* can survive underwater */
    public static final long BREATHLESS = 0x00000400L;  /* doesn't need to breathe */
    public static final long NOTAKE = 0x00000800L;      /* cannot pick up objects */
    public static final long NOEYES = 0x00001000L;      /* no eyes to gaze into or blind */
    public static final long NOHANDS = 0x00002000L;     /* no hands to handle things */
    public static final long NOLIMBS = 0x00006000L;     /* no arms/legs to kick/wear on */
    public static final long NOHEAD = 0x00008000L;      /* no head to behead */
    public static final long MINDLESS = 0x00010000L;    /* has no mind--golem, zombie, mold */
    public static final long HUMANOID = 0x00020000L;    /* has humanoid head/arms/torso */
    public static final long ANIMAL = 0x00040000L;      /* has animal body */
    public static final long SLITHY = 0x00080000L;      /* has serpent body */
    public static final long UNSOLID = 0x00100000L;     /* has no solid or liquid body */
    public static final long THICK_HIDE = 0x00200000L;  /* has thick hide or scales */
    public static final long OVIPAROUS = 0x00400000L;   /* can lay eggs */
    public static final long REGEN = 0x00800000L;       /* regenerates hit points */
    public static final long SEE_INVIS = 0x01000000L;   /* can see invisible creatures */
    public static final long TPORT = 0x02000000L;       /* can teleport */
    public static final long TPORT_CNTRL = 0x04000000L; /* controls where it teleports to */
    public static final long ACID = 0x08000000L;        /* acidic to eat */
    public static final long POIS = 0x10000000L;        /* poisonous to eat */
    public static final long CARNIVORE = 0x20000000L;   /* eats corpses */
    public static final long HERBIVORE = 0x40000000L;   /* eats fruits */
    public static final long OMNIVORE = 0x60000000L;    /* eats both */
    public static final long METALLIVORE = 0x80000000L; /* eats metal */

}
