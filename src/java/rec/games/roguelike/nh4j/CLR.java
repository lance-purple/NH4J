package rec.games.roguelike.nh4j;

public enum CLR {
    BLACK(0),
    RED(1),
    GREEN(2),
    BROWN(3),
    BLUE(4),
    MAGENTA(5), ROYAL_PURPLE(5),
    CYAN(6),
    GRAY(7),
    NO_COLOR(8),
    ORANGE(9),
    BRIGHT_GREEN(10),
    YELLOW(11), GOLD(11),
    BRIGHT_BLUE(12), BLUE_ZAP(12),
    BRIGHT_MAGENTA(13),
    BRIGHT_CYAN(14),
    WHITE(15), DOMESTIC_WHITE(15);

    int id;

    CLR(int id) {
        this.id = id;
    }

    public int id() {
        return id;
    }
}
