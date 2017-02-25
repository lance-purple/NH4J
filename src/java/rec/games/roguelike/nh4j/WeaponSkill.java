package rec.games.roguelike.nh4j;

import java.util.HashMap;
import java.util.Stack;

public class WeaponSkill {

	private static HashMap<Integer,Integer> skillsByType = new HashMap<>();
	private static HashMap<Integer,Integer> maximumSkillsByType = new HashMap<>();
	private static HashMap<Integer,Integer> skillAdvancesByType = new HashMap<>();
	
	private static Stack<Integer> advances = new Stack<>();
	
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
	
	public static int weaponSkill(int type) {
		if (! skillsByType.containsKey(type)) {
			return 0;
		}
		return skillsByType.get(type);
	}

	public static void setWeaponSkill(int type, int value) {
		if ((value != 0) || skillsByType.containsKey(type)) {
			skillsByType.put(type, value);
		}
	}

	public static int maximumWeaponSkill(int type) {
		if (! maximumSkillsByType.containsKey(type)) {
			return 0;
		}
		return maximumSkillsByType.get(type);
	}

	public static void setMaximumWeaponSkill(int type, int value) {
		if ((value != 0) || maximumSkillsByType.containsKey(type)) {
			maximumSkillsByType.put(type, value);
		}
	}

	public static int weaponSkillAdvance(int type) {
		if (! skillAdvancesByType.containsKey(type)) {
			return 0;
		}
		return skillAdvancesByType.get(type);
	}

	public static void setWeaponSkillAdvance(int type, int value) {
		if ((value != 0) || skillAdvancesByType.containsKey(type)) {
			skillAdvancesByType.put(type, value);
		}
	}
	
	public static int advancesCount() {
		return advances.size();
	}
	
	public static void addAdvance(int value) {
		advances.push(value);
	}

	public static int removeAdvance() {
		return advances.pop();
	}

}
