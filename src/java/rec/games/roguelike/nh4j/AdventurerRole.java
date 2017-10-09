package rec.games.roguelike.nh4j;

import java.util.HashMap;

public class AdventurerRole {

	private static HashMap<Integer, AdventurerRole> rolesByPMID = new HashMap<>();

	private static final AdventurerRole UNKNOWN_ROLE = new AdventurerRole(PM.UNKNOWN).named("UNKNOWN");

	private final PM roleID;
	private GenderedName roleName;

	private AdventurerRole(PM pmid) {
		roleID = pmid;
	}

	private AdventurerRole named(String genericVersion) {
		roleName = GenderedName.of(genericVersion);
		return this;
	}

	private AdventurerRole named(String maleVersion, String femaleVersion) {
		roleName = GenderedName.of(maleVersion, femaleVersion);
		return this;
	}
	
	private void add() {
		rolesByPMID.put(roleID.id(), this);
	}

	public static AdventurerRole of(PM pmid) {
		return new AdventurerRole(pmid);
	}
	
	public static AdventurerRole getRole(int pmid) {
		if (rolesByPMID.isEmpty()) {
			initialize();
		}
		if (rolesByPMID.containsKey(pmid))
		{
			return rolesByPMID.get(pmid);
		}
		return UNKNOWN_ROLE;
	}
	
	public static int numberOfKnownRoles() {
		if (rolesByPMID.isEmpty())
		{
			initialize();
		}
		return rolesByPMID.size();
	}
	
	
	public static boolean roleHasName(int pmid, int gender) {
		String name = getRole(pmid).roleName.as(gender);
		return (null != name) && (! name.isEmpty());
	}
	
	public static boolean roleNameHasGender(int pmid) {
		return (! getRole(pmid).roleName.identical());
	}
	
	public static String roleName(int pmid, int gender) {
		return getRole(pmid).roleName.as(gender);
	}


	private static void initialize() {
		AdventurerRole.of(PM.UNKNOWN).named("Unknown").add();
		
		AdventurerRole.of(PM.ARCHEOLOGIST).named("Archaeologist").add();

		AdventurerRole.of(PM.BARBARIAN).named("Barbarian").add();

		AdventurerRole.of(PM.CAVEMAN).named("Caveman", "Cavewoman").add();

		AdventurerRole.of(PM.HEALER).named("Healer").add();

		AdventurerRole.of(PM.KNIGHT).named("Knight").add();

		AdventurerRole.of(PM.MONK).named("Monk").add();

		AdventurerRole.of(PM.PRIEST).named("Priest", "Priestess").add();

		AdventurerRole.of(PM.ROGUE).named("Rogue").add();

		AdventurerRole.of(PM.RANGER).named("Ranger").add();

		AdventurerRole.of(PM.SAMURAI).named("Samurai").add();

		AdventurerRole.of(PM.TOURIST).named("Tourist").add();

		AdventurerRole.of(PM.VALKYRIE).named("Valkyrie").add();

		AdventurerRole.of(PM.WIZARD).named("Wizard").add();
	}
}
