package rec.games.roguelike.nh4j;

import java.util.Random;

public class Randomizer {

	public static final Random randomizer = new Random();

	public static int rn1(int x, int y) {
		return rn2(x) + y;
	}

	public static int rn2(int i) {
		return randomizer.nextInt(i);
	}

	public static int rndIfNonzero(int i) {
		if (0 == i) {
			return 0;
		}
		return rnd(i);
	}

	public static int rnd(int i) {
		return randomizer.nextInt(i) + 1;
	}

}
