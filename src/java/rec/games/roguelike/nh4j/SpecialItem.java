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
}
