package rec.games.roguelike.nh4j;

public class Context {

	static int identifier = 1; /* unique serial for monsters etc. */
	static int startingPetID = 0;
	
    static int numberOfWizards = 0; /* 0, 1, or 2 for Wizard of Yendor and his shadow */
    static int runningPace = 0;
    
    static int currentFruit;
	
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

	static int runningPace() {
		return runningPace;
	}
	
	static void setRunningPace(int p) {
		runningPace = p;
	}
	
	static int startingPetID() {
		return startingPetID;
	}
	
	static void setStartingPetID(int id) {
		startingPetID = id;
	}

	
	static int currentFruit() {
		return currentFruit;
	}
	
	static void setCurrentFruit(int id) {
		currentFruit = id;
	}

}
