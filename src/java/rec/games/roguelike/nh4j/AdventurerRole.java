package rec.games.roguelike.nh4j;

import java.util.HashMap;

public class AdventurerRole {

	private static HashMap<Integer, AdventurerRole> rolesByPMID = new HashMap<>();

	private final PM roleID;
	private GenderedName roleName;
	private HashMap<Integer, GenderedName> rankNamesByIndex = new HashMap<>();

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

	private AdventurerRole withRank(int rank, String genericVersion) {
		GenderedName rankName = GenderedName.of(genericVersion);
		rankNamesByIndex.put(rank, rankName);
		return this;
	}

	private AdventurerRole withRank(int rank, String maleVersion, String femaleVersion) {
		GenderedName rankName = GenderedName.of(maleVersion, femaleVersion);
		rankNamesByIndex.put(rank, rankName);
		return this;
	}

	private void add() {
		rolesByPMID.put(roleID.id(), this);
	}
	
	private GenderedName rankName(int rank)
	{
		return rankNamesByIndex.get(rank);
	}
	

	private static AdventurerRole of(PM pmid) {
		return new AdventurerRole(pmid);
	}

	private static AdventurerRole getRole(int pmid) {
		if (rolesByPMID.isEmpty()) {
			initialize();
		}
		if (rolesByPMID.containsKey(pmid)) {
			return rolesByPMID.get(pmid);
		}
		return null;
	}

	public static int numberOfKnownRoles() {
		if (rolesByPMID.isEmpty()) {
			initialize();
		}
		return rolesByPMID.size();
	}

	public static boolean roleNameHasMaleVersion(int pmid) {
		AdventurerRole role = getRole(pmid);
		if ((null != role) && (null != role.roleName))
		{
			return role.roleName.hasMaleVersion();
		}
		return false;
	}
	
	public static String roleNameAsMale(int pmid) {
		AdventurerRole role = getRole(pmid);
		if ((null != role) && (null != role.roleName))
		{
			return role.roleName.asMale();
		}
		return "???";
	}
	
	public static boolean roleNameHasFemaleVersion(int pmid) {
		AdventurerRole role = getRole(pmid);
		if ((null != role) && (null != role.roleName))
		{
			return role.roleName.hasFemaleVersion();
		}
		return false;
	}

	
	public static String roleNameAsFemale(int pmid) {
		AdventurerRole role = getRole(pmid);
		if ((null != role) && (null != role.roleName))
		{
			return role.roleName.asFemale();
		}
		return "???";
	}

	
	public static boolean rankNameHasMaleVersion(int pmid, int rank) {
		AdventurerRole role = getRole(pmid);
		if (null != role)
		{
			GenderedName rankName = role.rankName(rank);
			return ((null != rankName) && (rankName.hasMaleVersion()));
		}
		return false;
	}

	public static String rankNameAsMale(int pmid, int rank) {
		AdventurerRole role = getRole(pmid);
		if (null != role)
		{
			GenderedName rankName = role.rankName(rank);
			if (null != rankName)
			{
				return rankName.asMale();
			}
		}
		return "???";
	}

	
	public static boolean rankNameHasFemaleVersion(int pmid, int rank) {
		AdventurerRole role = getRole(pmid);
		if (null != role)
		{
			GenderedName rankName = role.rankName(rank);
			return ((null != rankName) && (rankName.hasFemaleVersion()));
		}
		return false;
	}

	public static String rankNameAsFemale(int pmid, int rank) {
		AdventurerRole role = getRole(pmid);
		if (null != role)
		{
			GenderedName rankName = role.rankName(rank);
			if (null != rankName)
			{
				return rankName.asFemale();
			}
		}
		return "???";
	}


	private static void initialize() {

		AdventurerRole.of(PM.ARCHEOLOGIST).named("Archaeologist").withRank(1, "Digger").withRank(2, "Field Worker")
				.withRank(3, "Investigator").withRank(4, "Exhumer").withRank(5, "Excavator").withRank(6, "Spelunker")
				.withRank(7, "Speleologist").withRank(8, "Collector").withRank(9, "Curator").add();

		AdventurerRole.of(PM.BARBARIAN).named("Barbarian").withRank(1, "Plunderer", "Plunderess")
				.withRank(1, "Plunderer", "Plunderess").withRank(2, "Pillager").withRank(3, "Bandit")
				.withRank(4, "Brigand").withRank(5, "Raider").withRank(6, "Reaver").withRank(7, "Slayer")
				.withRank(8, "Chieftain", "Chieftainess").withRank(9, "Conqueror", "Conqueress").add();

		AdventurerRole.of(PM.CAVEMAN).named("Caveman", "Cavewoman").withRank(1, "Troglodyte").withRank(2, "Aborigine")
				.withRank(3, "Wanderer").withRank(4, "Vagrant").withRank(5, "Wayfarer").withRank(6, "Roamer")
				.withRank(7, "Nomad").withRank(8, "Rover").withRank(9, "Pioneer").add();

		AdventurerRole.of(PM.HEALER).named("Healer").withRank(1, "Rhizotomist").withRank(2, "Empiric")
				.withRank(3, "Embalmer").withRank(4, "Dresser").withRank(5, "Medicus ossium", "Medica ossium")
				.withRank(6, "Herbalist").withRank(7, "Magister", "Magistra").withRank(8, "Physician")
				.withRank(9, "Chirurgeon").add();

		AdventurerRole.of(PM.KNIGHT).named("Knight").withRank(1, "Gallant").withRank(2, "Esquire")
				.withRank(3, "Bachelor").withRank(4, "Sergeant").withRank(5, "Knight").withRank(6, "Banneret")
				.withRank(7, "Chevalier", "Chevaliere").withRank(8, "Seignieur", "Dame").withRank(9, "Paladin").add();

		AdventurerRole.of(PM.MONK).named("Monk").withRank(1, "Candidate").withRank(2, "Novice").withRank(3, "Initiate")
				.withRank(4, "Student of Stones").withRank(5, "Student of Waters").withRank(6, "Student of Metals")
				.withRank(7, "Student of Winds").withRank(8, "Student of Fire").withRank(9, "Master").add();

		AdventurerRole.of(PM.PRIEST).named("Priest", "Priestess").withRank(1, "Aspirant").withRank(2, "Acolyte")
				.withRank(3, "Adept").withRank(4, "Priest", "Priestess").withRank(5, "Curate")
				.withRank(6, "Canon", "Canoness").withRank(7, "Lama").withRank(8, "Patriarch", "Matriarch")
				.withRank(9, "High Priest", "High Priestess").add();

		AdventurerRole.of(PM.ROGUE).named("Rogue").withRank(1, "Footpad").withRank(2, "Cutpurse").withRank(3, "Rogue")
				.withRank(4, "Pilferer").withRank(5, "Robber").withRank(6, "Burglar").withRank(7, "Filcher")
				.withRank(8, "Magsman", "Magswoman").withRank(9, "Thief").add();

		AdventurerRole.of(PM.RANGER).named("Ranger").withRank(1, "Tenderfoot").withRank(2, "Lookout")
				.withRank(3, "Trailblazer").withRank(4, "Reconnoiterer", "Reconnoiteress").withRank(5, "Scout")
				.withRank(6, "Arbalester") /* One skilled at crossbows */
				.withRank(7, "Archer").withRank(8, "Sharpshooter").withRank(9, "Marksman", "Markswoman").add();

		AdventurerRole.of(PM.SAMURAI).named("Samurai")
				.withRank(1, "Hatamoto") /* Banner Knight */
				.withRank(2, "Ronin") /* no allegiance */
				.withRank(3, "Ninja", "Kunoichi") /* secret society */
				.withRank(4, "Joshu") /* heads a castle */
				.withRank(5, "Ryoshu") /* has a territory */
				.withRank(6, "Kokushu") /* heads a province */
				.withRank(7, "Daimyo") /* a samurai lord */
				.withRank(8, "Kuge") /* Noble of the Court */
				.withRank(9, "Shogun").add(); /* supreme commander, warlord */

		AdventurerRole.of(PM.TOURIST).named("Tourist").withRank(1, "Rambler").withRank(2, "Sightseer")
				.withRank(3, "Excursionist").withRank(4, "Peregrinator", "Peregrinatrix").withRank(5, "Traveler")
				.withRank(6, "Journeyer").withRank(7, "Voyager").withRank(8, "Explorer").withRank(9, "Adventurer")
				.add();

		AdventurerRole.of(PM.VALKYRIE).named("Valkyrie").withRank(1, "Stripling").withRank(2, "Skirmisher")
				.withRank(3, "Fighter").withRank(4, "Man-at-arms", "Woman-at-arms").withRank(5, "Warrior")
				.withRank(6, "Swashbuckler").withRank(7, "Hero", "Heroine").withRank(8, "Champion")
				.withRank(9, "Lord", "Lady").add();

		AdventurerRole.of(PM.WIZARD).named("Wizard").withRank(1, "Evoker").withRank(2, "Conjurer")
				.withRank(3, "Thaumaturge").withRank(4, "Magician").withRank(5, "Enchanter", "Enchantress")
				.withRank(6, "Sorcerer", "Sorceress").withRank(7, "Necromancer").withRank(8, "Wizard")
				.withRank(9, "Mage").add();

	}
}
