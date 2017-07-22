package rec.games.roguelike.nh4j;

public class MonsterRandomizer {

	static int choiceCount = -1;

	static int choiceCount() {
		return choiceCount;
	}

	static void setChoiceCount(int c) {
		choiceCount = c;
	}
	
	static void increaseChoiceCount(int c) {
		choiceCount += c;
	}

	static void decreaseChoiceCount(int c) {
		choiceCount += c;
	}

}
