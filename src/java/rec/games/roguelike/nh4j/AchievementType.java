package rec.games.roguelike.nh4j;

public enum AchievementType {

	GOT_AMULET(0), GOT_BELL(1), GOT_BOOK(2), GOT_CANDELABRUM(3), ENTERED_GEHENNOM(4), ASCENDED(
			5), FOUND_LUCKSTONE_IN_MINES(6), FINISHED_SOKOBAN_LEVEL(7), KILLED_MEDUSA(8);

	private int id;

	AchievementType(int i) {
		id = i;
	}

	public int id() {
		return id;
	}

	public static int GOT_AMULET() {
		return GOT_AMULET.id();
	}

	public static int GOT_BELL() {
		return GOT_BELL.id();
	}

	public static int GOT_BOOK() {
		return GOT_BOOK.id();
	}

	public static int GOT_CANDELABRUM() {
		return GOT_CANDELABRUM.id();
	}

	public static int ENTERED_GEHENNOM() {
		return ENTERED_GEHENNOM.id();
	}

	public static int ASCENDED() {
		return ASCENDED.id();
	}

	public static int FOUND_LUCKSTONE_IN_MINES() {
		return FOUND_LUCKSTONE_IN_MINES.id();
	}

	public static int FINISHED_SOKOBAN_LEVEL() {
		return FINISHED_SOKOBAN_LEVEL.id();
	}

	public static int KILLED_MEDUSA() {
		return KILLED_MEDUSA.id();
	}

}
