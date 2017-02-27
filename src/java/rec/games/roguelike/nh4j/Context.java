package rec.games.roguelike.nh4j;

public class Context {

	static int identifier = 1; /* unique serial for monsters etc. */
    static int numberOfWizards = 0; /* 0, 1, or 2 for Wizard of Yendor and his shadow */
	
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

	static int numberOfWizards() {
		return numberOfWizards;
	}

	static void setNumberOfWizards(int n) {
		numberOfWizards = n;
	}

	static void increaseNumberOfWizards(int n) {
		numberOfWizards += n;
	}

	static void decreaseNumberOfWizards(int n) {
		numberOfWizards -= n;
	}

}
