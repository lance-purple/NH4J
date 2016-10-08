package rec.games.roguelike.nh4j;

public enum TrapType {
	
	BEAR_TRAP(0), PIT(1), WEB(2), LAVA(3), IN_FLOOR(4), BURIED_BALL(5);

	private int key;

	TrapType(int key) {
		this.key = key;
	}

	int key() {
		return this.key;
	}

	static TrapType byKey(int key) {
		for (TrapType type : values()) {
			if (key == type.key) {
				return type;
			}
		}
		throw new IllegalArgumentException("No TrapType with key=" + key);
	}
}
