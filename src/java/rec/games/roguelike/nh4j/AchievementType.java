package rec.games.roguelike.nh4j;

public enum AchievementType {

	GOT_AMULET(0),
	GOT_BELL(1),
	GOT_BOOK(2),
	GOT_CANDELABRUM(3),
	ENTERED_GEHENNOM(4),
	ASCENDED(5),
	FOUND_LUCKSTONE_IN_MINES(6),
	FINISHED_SOKOBAN_LEVEL(7),
	KILLED_MEDUSA(8);
	
	private int id;
	
	AchievementType(int i) {
		id = i;
	}
	
	public int id() {
		return id;
	}
}
