package rec.games.roguelike.nh4j;

public enum SpecialItem {

	AMULET(0),
	BELL(1),
	BOOK(2),
	CANDELABRUM(3),
	QUEST_ARTIFACT(4);
	
	private int id;
	
	SpecialItem(int i) {
		id = i;
	}
	
	public int id() {
		return id;
	}
	
	public static int AMULET() {
		return AMULET.id();
	}

	public static int BELL() {
		return BELL.id();
	}

	public static int BOOK() {
		return BOOK.id();
	}

	public static int CANDELABRUM() {
		return CANDELABRUM.id();
	}

	public static int QUEST_ARTIFACT() {
		return QUEST_ARTIFACT.id();
	}
	
}
