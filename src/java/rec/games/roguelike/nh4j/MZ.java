package rec.games.roguelike.nh4j;

public enum MZ {

    TINY(0),     /* < 2' */
    SMALL(1),    /* 2-4' */
    MEDIUM(2),   /* 4-7' */
    HUMAN(2),    /* human-sized */
    LARGE(3),    /* 7-12' */
    HUGE(4),     /* 12-25' */
    GIGANTIC(7); /* off the scale */

    int id;

    MZ(int id) {
        this.id = id;
    }

    public int id() {
        return id;
    }

}
