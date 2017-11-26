package rec.games.roguelike.nh4j;

import java.util.ArrayList;

public class RoleFilter {

	private static int mask = 0;
	private static ArrayList<Boolean> roles = initRoles();

	private static ArrayList<Boolean> initRoles() {
		int size = AdventurerRole.numberOfKnownRoles();
		ArrayList<Boolean> roles = new ArrayList<>();
		for (int i = 0; i < size; i++) {
			roles.add(i, false);
		}
		return roles;
	}

	public static int mask() {
		return mask;
	}

	public static void addMask(int additionalMask) {
		mask |= additionalMask;
	}

	public static void resetMask() {
		mask = 0;
	}

	public static boolean roleFilter(int roleID) {
		System.err.println("roles " + roles.size());
		return roles.get(roleID);
	}

	public static void setRoleFilter(int roleID, boolean set) {
		roles.set(roleID, set);
	}

	public static boolean anyRoleFilters() {
		for (int i = 0; i < roles.size(); i++) {
			if (true == roles.get(i)) {
				return true;
			}
		}
		return false;
	}

	public static void resetRoleFilters() {
		for (int i = 0; i < roles.size(); i++) {
			roles.set(i, false);
		}
	}

}
