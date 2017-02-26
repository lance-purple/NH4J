package rec.games.roguelike.nh4j;

public class Context {

	static int identifier = 1; /* unique serial for monsters etc. */

	static void initIdentifier(int i) {
		identifier = i;
	}

	static int nextIdentifier() {
		int next = identifier++;
		if (0 == next) {
			/* don't return 0 if overflow */
			next = identifier++;
		}
		return next;
	}
	
	static int currentIdentifier() {
		return identifier;
	}

}
