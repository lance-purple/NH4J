package rec.games.roguelike.nh4j;

public class WeaponSkill {

	private static int slots = 0;
	
	public static int slots() {
		return slots;
	}

	public static void setSlots(int s) {
		slots = s;
	}

	public static void increaseSlots(int n) {
		slots += n;
	}

	public static void decreaseSlots(int n) {
		slots -= n;
	}

}
