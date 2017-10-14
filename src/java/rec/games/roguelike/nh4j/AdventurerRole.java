package rec.games.roguelike.nh4j;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

public class AdventurerRole {

	private static HashMap<Integer, AdventurerRole> rolesByID = new HashMap<>();

	private final int roleID;
	private GenderedName roleName;
	private ArrayList<GenderedName> rankNames = new ArrayList<>();
	private Pantheon pantheon = null;
	private String fileCode;
	
	private String questHomeBase;

	private AdventurerRole(int id) {
		roleID = id;
	}

	private AdventurerRole named(String genericVersion) {
		roleName = GenderedName.of(genericVersion);
		return this;
	}

	private AdventurerRole named(String maleVersion, String femaleVersion) {
		roleName = GenderedName.of(maleVersion, femaleVersion);
		return this;
	}

	private AdventurerRole withRank(String genericVersion) {
		rankNames.add(GenderedName.of(genericVersion));
		return this;
	}

	private AdventurerRole withRank(String maleVersion, String femaleVersion) {
		rankNames.add(GenderedName.of(maleVersion, femaleVersion));
		return this;
	}
	
	private AdventurerRole with(Pantheon pantheon)
	{
		this.pantheon = pantheon;
		return this;
	}
	
	private AdventurerRole withFileCode(String fileCode)
	{
		this.fileCode = fileCode;
		return this;
	}
	
	private AdventurerRole withQuestHomeBase(String base)
	{
		this.questHomeBase = base;
		return this;
	}


	private void add() {
		rolesByID.put(this.roleID, this);
	}
	
	private GenderedName rankName(int rank)
	{
		return rankNames.get(rank);
	}
	

	private static AdventurerRole id(int id) {
		return new AdventurerRole(id);
	}

	private static AdventurerRole getRole(int id) {
		if (rolesByID.isEmpty()) {
			initialize();
		}
		if (rolesByID.containsKey(id)) {
			return rolesByID.get(id);
		}
		return null;
	}

	public static int numberOfKnownRoles() {
		if (rolesByID.isEmpty()) {
			initialize();
		}
		return rolesByID.size();
	}

	public static boolean roleNameHasMaleVersion(int id) {
		AdventurerRole role = getRole(id);
		if ((null != role) && (null != role.roleName))
		{
			return role.roleName.hasMaleVersion();
		}
		return false;
	}
	
	public static String roleNameAsMale(int id) {
		AdventurerRole role = getRole(id);
		if ((null != role) && (null != role.roleName))
		{
			return role.roleName.asMale();
		}
		return "(UNKNOWN ROLE)";
	}
	
	public static boolean roleNameHasFemaleVersion(int id) {
		AdventurerRole role = getRole(id);
		if ((null != role) && (null != role.roleName))
		{
			return role.roleName.hasFemaleVersion();
		}
		return false;
	}

	
	public static String roleNameAsFemale(int id) {
		AdventurerRole role = getRole(id);
		if ((null != role) && (null != role.roleName))
		{
			return role.roleName.asFemale();
		}
		return "(UNKNOWN ROLE)";
	}

	
	public static boolean rankNameHasMaleVersion(int id, int rank) {
		AdventurerRole role = getRole(id);
		if (null != role)
		{
			GenderedName rankName = role.rankName(rank);
			return ((null != rankName) && (rankName.hasMaleVersion()));
		}
		return false;
	}

	public static String rankNameAsMale(int id, int rank) {
		AdventurerRole role = getRole(id);
		if (null != role)
		{
			GenderedName rankName = role.rankName(rank);
			if (null != rankName)
			{
				return rankName.asMale();
			}
		}
		return "(UNKNOWN RANK)";
	}

	
	public static boolean rankNameHasFemaleVersion(int id, int rank) {
		AdventurerRole role = getRole(id);
		if (null != role)
		{
			GenderedName rankName = role.rankName(rank);
			return ((null != rankName) && (rankName.hasFemaleVersion()));
		}
		return false;
	}

	public static String rankNameAsFemale(int id, int rank) {
		AdventurerRole role = getRole(id);
		if (null != role)
		{
			GenderedName rankName = role.rankName(rank);
			if (null != rankName)
			{
				return rankName.asFemale();
			}
		}
		return "(UNKNOWN RANK)";
	}
	

	public static boolean hasDefaultPantheon(int id) {
		return (-1 != defaultPantheon(id));
	}
		
	
	public static int defaultPantheon(int id) {
		AdventurerRole role = getRole(id);
		if (null != role)
		{
			if (null != role.pantheon)
			{
				return role.pantheon.ordinal();
			}
		}
		return -1;
	}
	
	public static String fileCode(int id) {
		AdventurerRole role = getRole(id);
		if (null != role)
		{
			return role.fileCode;
		}
		return "UNK";
	}
	
	public static String questHomeBase(int id) {
		AdventurerRole role = getRole(id);
		if (null != role)
		{
			return role.questHomeBase;
		}
		return "(UNKNOWN LOCATION)";
	}	
	
	
	
	public static AdventurerRole random() {
		if (rolesByID.isEmpty()) {
			initialize();
		}
		ArrayList<AdventurerRole> roles = new ArrayList<>(rolesByID.values());
		Collections.shuffle(roles);
		return roles.get(0);
	}


	private static void initialize() {

		AdventurerRole.id(0).named("Archaeologist")
		    .withRank("Digger")
		    .withRank("Field Worker")
			.withRank("Investigator")
			.withRank("Exhumer")
			.withRank("Excavator")
			.withRank("Spelunker")
			.withRank("Speleologist")
			.withRank("Collector")
			.withRank("Curator")
			.with(Pantheon.CENTRAL_AMERICAN)
			.withFileCode("Arc")
			.withQuestHomeBase("the College of Archeology")
			.add();

		AdventurerRole.id(1).named("Barbarian").withRank("Plunderer", "Plunderess")
			.withRank("Pillager")
			.withRank("Bandit")
			.withRank("Brigand")
			.withRank("Raider")
			.withRank("Reaver")
			.withRank("Slayer")
			.withRank("Chieftain", "Chieftainess")
			.withRank("Conqueror", "Conqueress")
			.with(Pantheon.HYBORIAN)
			.withFileCode("Bar")
			.withQuestHomeBase("the Camp of the Duali Tribe")
			.add();

		AdventurerRole.id(2).named("Caveman", "Cavewoman")
		    .withRank("Troglodyte")
		    .withRank("Aborigine")
			.withRank("Wanderer")
			.withRank("Vagrant")
			.withRank("Wayfarer")
			.withRank("Roamer")
			.withRank("Nomad")
			.withRank("Rover")
			.withRank("Pioneer")
			.with(Pantheon.BABYLONIAN)
			.withFileCode("Cav")
			.withQuestHomeBase("the Caves of the Ancestors")
			.add();

		AdventurerRole.id(3).named("Healer")
		    .withRank("Rhizotomist")
		    .withRank("Empiric")
			.withRank("Embalmer")
			.withRank("Dresser")
			.withRank("Medicus ossium", "Medica ossium")
			.withRank("Herbalist")
			.withRank("Magister", "Magistra")
			.withRank("Physician")
			.withRank("Chirurgeon")
			.with(Pantheon.GREEK)
			.withFileCode("Hea")
			.withQuestHomeBase("the Temple of Epidaurus")
			.add();

		AdventurerRole.id(4).named("Knight")
		    .withRank("Gallant")
		    .withRank("Esquire")
			.withRank("Bachelor")
			.withRank("Sergeant")
			.withRank("Knight")
			.withRank("Banneret")
			.withRank("Chevalier", "Chevaliere")
			.withRank("Seignieur", "Dame")
			.withRank("Paladin")
			.with(Pantheon.CELTIC)
			.withFileCode("Kni")
			.withQuestHomeBase("Camelot Castle")
			.add();

		AdventurerRole.id(5).named("Monk")
		    .withRank("Candidate")
		    .withRank("Novice")
		    .withRank("Initiate")
			.withRank("Student of Stones")
			.withRank("Student of Waters")
			.withRank("Student of Metals")
			.withRank("Student of Winds")
			.withRank("Student of Fire")
			.withRank("Master")
			.with(Pantheon.CHINESE)
			.withFileCode("Mon")
			.withQuestHomeBase( "the Monastery of Chan-Sune")
			.add();

		AdventurerRole.id(6).named("Priest", "Priestess")
		    .withRank("Aspirant")
		    .withRank("Acolyte")
			.withRank("Adept")
			.withRank("Priest", "Priestess")
			.withRank("Curate")
			.withRank("Canon", "Canoness")
			.withRank("Lama")
			.withRank("Patriarch", "Matriarch")
			.withRank("High Priest", "High Priestess")
			.withFileCode("Pri")
			.withQuestHomeBase("the Great Temple")
			.add();

		AdventurerRole.id(7).named("Rogue")
		    .withRank("Footpad")
		    .withRank("Cutpurse")
		    .withRank("Rogue")
			.withRank("Pilferer")
			.withRank("Robber")
			.withRank("Burglar")
			.withRank("Filcher")
			.withRank("Magsman", "Magswoman")
			.withRank("Thief")
			.with(Pantheon.NEHWON)
			.withFileCode("Rog")
			.withQuestHomeBase("the Thieves' Guild Hall")
			.add();

		AdventurerRole.id(8).named("Ranger")
		    .withRank("Tenderfoot")
		    .withRank("Lookout")
			.withRank("Trailblazer")
			.withRank("Reconnoiterer", "Reconnoiteress")
			.withRank("Scout")
			.withRank("Arbalester") /* One skilled at crossbows */
			.withRank("Archer")
			.withRank("Sharpshooter")
			.withRank("Marksman", "Markswoman")
			.with(Pantheon.ROMAN)
			.withFileCode("Ran")
			.withQuestHomeBase("Orion's camp")
			.add();

		AdventurerRole.id(9).named("Samurai")
			.withRank("Hatamoto") /* Banner Knight */
			.withRank("Ronin") /* no allegiance */
			.withRank("Ninja", "Kunoichi") /* secret society */
			.withRank("Joshu") /* heads a castle */
			.withRank("Ryoshu") /* has a territory */
			.withRank("Kokushu") /* heads a province */
			.withRank("Daimyo") /* a samurai lord */
			.withRank("Kuge") /* Noble of the Court */
			.withRank("Shogun") /* supreme commander, warlord */
			.with(Pantheon.JAPANESE)
			.withFileCode("Sam")
			.withQuestHomeBase("the Castle of the Taro Clan")
			.add();

		AdventurerRole.id(10).named("Tourist")
		    .withRank("Rambler")
		    .withRank("Sightseer")
			.withRank("Excursionist")
			.withRank("Peregrinator", "Peregrinatrix")
			.withRank("Traveler")
			.withRank("Journeyer")
			.withRank("Voyager")
			.withRank("Explorer")
			.withRank("Adventurer")
			.with(Pantheon.DISCWORLD)
			.withFileCode("Tou")
			.withQuestHomeBase("Ankh-Morpork")
			.add();

		AdventurerRole.id(11).named("Valkyrie")
		    .withRank("Stripling")
		    .withRank("Skirmisher")
			.withRank("Fighter")
			.withRank("Man-at-arms", "Woman-at-arms")
			.withRank("Warrior")
			.withRank("Swashbuckler")
			.withRank("Hero", "Heroine")
			.withRank("Champion")
			.withRank("Lord", "Lady")
			.with(Pantheon.NORSE)
			.withFileCode("Val")
			.withQuestHomeBase("the Shrine of Destiny")
			.add();

		AdventurerRole.id(12).named("Wizard")
		    .withRank("Evoker")
		    .withRank("Conjurer")
			.withRank("Thaumaturge")
			.withRank("Magician")
			.withRank("Enchanter", "Enchantress")
			.withRank("Sorcerer", "Sorceress")
			.withRank("Necromancer")
			.withRank("Wizard")
			.withRank("Mage")
			.with(Pantheon.EGYPTIAN)
			.withFileCode("Wiz")
			.withQuestHomeBase("the Lonely Tower")
			.add();

	}
}
