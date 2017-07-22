package rec.games.roguelike.nh4j;

import java.util.HashMap;

public class MonsterRandomizer {

	static int choiceCount = -1;
	static HashMap<Integer,Integer> choices = new HashMap<>();

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
	
	static int choices(int pmid) {
		if (choices.containsKey(pmid)) {
			return choices.get(pmid);
		} else {
			return 0;
		}
	}

	static void setChoices(int pmid, int c) {
		choices.put(pmid, c);
	}

}
