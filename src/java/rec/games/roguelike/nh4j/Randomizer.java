package rec.games.roguelike.nh4j;

import java.util.Random;

public class Randomizer {
	
	public static final Random randomizer = new Random();

	public static int nextInt(int i) {
		if (i == 0) {
			return i;
		}
		return randomizer.nextInt(i);
	}

}
