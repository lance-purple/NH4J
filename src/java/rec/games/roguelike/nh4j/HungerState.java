package rec.games.roguelike.nh4j;

public enum HungerState {
	
	SATIATED(0), NOT_HUNGRY(1), HUNGRY(2), WEAK(3), FAINTING(4), FAINTED(5), STARVED(6);

	private int key;

	HungerState(int key) {
		this.key = key;
	}

	int key() {
		return this.key;
	}

	static HungerState byKey(int key) {
		for (HungerState type : values()) {
			if (key == type.key) {
				return type;
			}
		}
		throw new IllegalArgumentException("No HungerState with key=" + key);
	}
}
