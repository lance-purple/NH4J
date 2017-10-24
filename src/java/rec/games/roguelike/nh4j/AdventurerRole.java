package rec.games.roguelike.nh4j;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

public class AdventurerRole {

	private static final int ARCHAEOLOGIST_ID = 0;
	private static final int BARBARIAN_ID = 1;
	private static final int CAVEMAN_ID = 2;
	private static final int HEALER_ID = 3;
	private static final int KNIGHT_ID = 4;
	private static final int MONK_ID = 5;
	private static final int PRIEST_ID = 6;
	private static final int ROGUE_ID = 7;
	private static final int RANGER_ID = 8;
	private static final int SAMURAI_ID = 9;
	private static final int TOURIST_ID = 10;
	private static final int VALKYRIE_ID = 11;
	private static final int WIZARD_ID = 12;

	private static HashMap<Integer, AdventurerRole> rolesByID = new HashMap<>();

	private final int roleID;
	private GenderedName roleName;
	private ArrayList<GenderedName> rankNames = new ArrayList<>();
	private Pantheon pantheon = null;
	private String fileCode;
	private PM malePM;
	private PM femalePM;
	private PM preferredPet = PM.UNKNOWN;
	private PM questLeader = PM.UNKNOWN;
	
	private String questHomeBase;
	private String intermediateQuestLevel;

	private AdventurerRole(int roleID) {
		this.roleID = roleID;
	}

	private AdventurerRole named(String genericVersion) {
		this.roleName = GenderedName.of(genericVersion);
		return this;
	}

	private AdventurerRole named(String maleVersion, String femaleVersion) {
		this.roleName = GenderedName.of(maleVersion, femaleVersion);
		return this;
	}

	private AdventurerRole withRank(String genericVersion) {
		this.rankNames.add(GenderedName.of(genericVersion));
		return this;
	}

	private AdventurerRole withRank(String maleVersion, String femaleVersion) {
		this.rankNames.add(GenderedName.of(maleVersion, femaleVersion));
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

	private AdventurerRole withIntermediateQuestLevel(String level)
	{
		this.intermediateQuestLevel = level;
		return this;
	}
	
	private AdventurerRole withPM(PM genericPM)
	{
		this.malePM = genericPM;
		this.femalePM = genericPM;
		return this;
	}

	private AdventurerRole withPM(PM malePM, PM femalePM)
	{
		this.malePM = malePM;
		this.femalePM = femalePM;
		return this;
	}
	
	private AdventurerRole withPreferredPet(PM pet)
	{
		this.preferredPet = pet;
		return this;
	}
	
	private AdventurerRole withQuestLeader(PM leader)
	{
		this.questLeader = leader;
		return this;
	}

	private void add() {
		AdventurerRole.rolesByID.put(this.roleID, this);
	}
	
	private GenderedName rankName(int rank)
	{
		return this.rankNames.get(rank);
	}
	

	private static AdventurerRole withID(int roleID) {
		return new AdventurerRole(roleID);
	}

	private static AdventurerRole getRole(int roleID) {
		if (rolesByID.isEmpty()) {
			initialize();
		}
		if (rolesByID.containsKey(roleID)) {
			return rolesByID.get(roleID);
		}
		return null;
	}

	public static int numberOfKnownRoles() {
		if (rolesByID.isEmpty()) {
			initialize();
		}
		return rolesByID.size();
	}

	public static boolean roleNameHasMaleVersion(int roleID) {
		AdventurerRole role = getRole(roleID);
		if ((null != role) && (null != role.roleName))
		{
			return role.roleName.hasMaleVersion();
		}
		return false;
	}
	
	public static String roleNameAsMale(int roleID) {
		AdventurerRole role = getRole(roleID);
		if ((null != role) && (null != role.roleName))
		{
			return role.roleName.asMale();
		}
		return "(UNKNOWN ROLE)";
	}
	
	public static boolean roleNameHasFemaleVersion(int roleID) {
		AdventurerRole role = getRole(roleID);
		if ((null != role) && (null != role.roleName))
		{
			return role.roleName.hasFemaleVersion();
		}
		return false;
	}

	
	public static String roleNameAsFemale(int roleID) {
		AdventurerRole role = getRole(roleID);
		if ((null != role) && (null != role.roleName))
		{
			return role.roleName.asFemale();
		}
		return "(UNKNOWN ROLE)";
	}

	
	public static boolean rankNameHasMaleVersion(int roleID, int rank) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			GenderedName rankName = role.rankName(rank);
			return ((null != rankName) && (rankName.hasMaleVersion()));
		}
		return false;
	}

	public static String rankNameAsMale(int roleID, int rank) {
		AdventurerRole role = getRole(roleID);
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

	
	public static boolean rankNameHasFemaleVersion(int roleID, int rank) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			GenderedName rankName = role.rankName(rank);
			return ((null != rankName) && (rankName.hasFemaleVersion()));
		}
		return false;
	}

	public static String rankNameAsFemale(int roleID, int rank) {
		AdventurerRole role = getRole(roleID);
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
	

	public static boolean hasDefaultPantheon(int roleID) {
		return (-1 != defaultPantheon(roleID));
	}
		
	
	public static int defaultPantheon(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			if (null != role.pantheon)
			{
				return role.pantheon.ordinal();
			}
		}
		return -1;
	}
	
	public static String fileCode(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.fileCode;
		}
		return "UNK";
	}
	
	public static String questHomeBase(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.questHomeBase;
		}
		return "(UNKNOWN LOCATION)";
	}	
	
	public static String intermediateQuestLevel(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.intermediateQuestLevel;
		}
		return "(UNKNOWN LOCATION)";
	}	
	
	public static int pmidAsMale(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.malePM.id();
		}
		return PM.UNKNOWN.id();
	}

	public static void setPMIDAsMale(int roleID, int pmid) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			role.malePM = PM.withPMID(pmid);
		}
	}

	public static int pmidAsFemale(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.femalePM.id();
		}
		return PM.UNKNOWN.id();
	}

	public static void setPMIDAsFemale(int roleID, int pmid) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			role.femalePM = PM.withPMID(pmid);
		}
	}

	public static AdventurerRole random() {
		if (rolesByID.isEmpty()) {
			initialize();
		}
		ArrayList<AdventurerRole> roles = new ArrayList<>(rolesByID.values());
		Collections.shuffle(roles);
		return roles.get(0);
	}

	public static int pmidOfPreferredPet(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.preferredPet.id();
		}
		return PM.UNKNOWN.id();
	}
	
	public static boolean hasPreferredPet(int roleID) {
		return PM.UNKNOWN.id() != pmidOfPreferredPet(roleID);
	}

	
	public static int pmidOfQuestLeader(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.questLeader.id();
		}
		return PM.UNKNOWN.id();
	}
	
	public static boolean hasQuestLeader(int roleID) {
		return PM.UNKNOWN.id() != pmidOfQuestLeader(roleID);
	}

	private static void initialize() {

		AdventurerRole.withID(ARCHAEOLOGIST_ID)
		    .named("Archaeologist")
		    .withPM(PM.ARCHEOLOGIST)
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
			.withQuestLeader(PM.LORD_CARNARVON)
			.withQuestHomeBase("the College of Archeology")
			.withIntermediateQuestLevel( "the Tomb of the Toltec Kings")
			.add();

		AdventurerRole.withID(BARBARIAN_ID)
		    .named("Barbarian")
		    .withPM(PM.BARBARIAN)
		    .withRank("Plunderer", "Plunderess")
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
			.withQuestLeader(PM.PELIAS)
			.withQuestHomeBase("the Camp of the Duali Tribe")
			.withIntermediateQuestLevel("the Duali Oasis")
			.add();

		AdventurerRole.withID(CAVEMAN_ID)
		    .named("Caveman", "Cavewoman")
		    .withPM(PM.CAVEMAN, PM.CAVEWOMAN)
		    .withRank("Troglodyte")
		    .withRank("Aborigine")
			.withRank("Wanderer")
			.withRank("Vagrant")
			.withRank("Wayfarer")
			.withRank("Roamer")
			.withRank("Nomad")
			.withRank("Rover")
			.withRank("Pioneer")
			.withPreferredPet(PM.LITTLE_DOG)
			.with(Pantheon.BABYLONIAN)
			.withFileCode("Cav")
			.withQuestLeader(PM.SHAMAN_KARNOV)
			.withQuestHomeBase("the Caves of the Ancestors")
			.withIntermediateQuestLevel("the Dragon's Lair")
			.add();

		AdventurerRole.withID(HEALER_ID)
		    .named("Healer")
		    .withPM(PM.HEALER)
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
			.withQuestLeader(PM.HIPPOCRATES)
			.withQuestHomeBase("the Temple of Epidaurus")
			.withIntermediateQuestLevel("the Temple of Coeus")
			.add();

		AdventurerRole.withID(KNIGHT_ID)
		    .named("Knight")
		    .withPM(PM.KNIGHT)
		    .withRank("Gallant")
		    .withRank("Esquire")
			.withRank("Bachelor")
			.withRank("Sergeant")
			.withRank("Knight")
			.withRank("Banneret")
			.withRank("Chevalier", "Chevaliere")
			.withRank("Seignieur", "Dame")
			.withRank("Paladin")
			.withPreferredPet(PM.PONY)
			.with(Pantheon.CELTIC)
			.withFileCode("Kni")
			.withQuestLeader(PM.KING_ARTHUR)
			.withQuestHomeBase("Camelot Castle")
			.withIntermediateQuestLevel("the Isle of Glass")
			.add();

		AdventurerRole.withID(MONK_ID)
		    .named("Monk")
		    .withPM(PM.MONK)
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
			.withQuestLeader(PM.GRAND_MASTER)
			.withQuestHomeBase( "the Monastery of Chan-Sune")
			.withIntermediateQuestLevel("the Monastery of the Earth-Lord")
			.add();

		AdventurerRole.withID(PRIEST_ID)
		    .named("Priest", "Priestess")
		    .withPM(PM.PRIEST, PM.PRIESTESS)
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
			.withQuestLeader(PM.ARCH_PRIEST)
			.withQuestHomeBase("the Great Temple")
			.withIntermediateQuestLevel("the Temple of Nalzok")
			.add();

		AdventurerRole.withID(ROGUE_ID)
		    .named("Rogue")
		    .withPM(PM.ROGUE)
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
			.withQuestLeader(PM.MASTER_OF_THIEVES)
			.withQuestHomeBase("the Thieves' Guild Hall")
			.withIntermediateQuestLevel("the Assassins' Guild Hall")
			.add();

		AdventurerRole.withID(RANGER_ID)
		    .named("Ranger")
		    .withPM(PM.RANGER)
		    .withRank("Tenderfoot")
		    .withRank("Lookout")
			.withRank("Trailblazer")
			.withRank("Reconnoiterer", "Reconnoiteress")
			.withRank("Scout")
			.withRank("Arbalester") /* One skilled at crossbows */
			.withRank("Archer")
			.withRank("Sharpshooter")
			.withRank("Marksman", "Markswoman")
			.withPreferredPet(PM.LITTLE_DOG)
			.with(Pantheon.ROMAN)
			.withFileCode("Ran")
			.withQuestLeader(PM.ORION)
			.withQuestHomeBase("Orion's camp")
			.withIntermediateQuestLevel("the cave of the wumpus")
			.add();

		AdventurerRole.withID(SAMURAI_ID)
		    .named("Samurai")
		    .withPM(PM.SAMURAI)
			.withRank("Hatamoto") /* Banner Knight */
			.withRank("Ronin") /* no allegiance */
			.withRank("Ninja", "Kunoichi") /* secret society */
			.withRank("Joshu") /* heads a castle */
			.withRank("Ryoshu") /* has a territory */
			.withRank("Kokushu") /* heads a province */
			.withRank("Daimyo") /* a samurai lord */
			.withRank("Kuge") /* Noble of the Court */
			.withRank("Shogun") /* supreme commander, warlord */
			.withPreferredPet(PM.LITTLE_DOG)
			.with(Pantheon.JAPANESE)
			.withFileCode("Sam")
			.withQuestLeader(PM.LORD_SATO)
			.withQuestHomeBase("the Castle of the Taro Clan")
			.withIntermediateQuestLevel("the Shogun's Castle")
			.add();

		AdventurerRole.withID(TOURIST_ID)
		    .named("Tourist")
		    .withPM(PM.TOURIST)
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
			.withQuestLeader(PM.TWOFLOWER)
			.withQuestHomeBase("Ankh-Morpork")
			.withIntermediateQuestLevel("the Thieves' Guild Hall")
			.add();

		AdventurerRole.withID(VALKYRIE_ID)
		    .named("Valkyrie")
		    .withPM(PM.VALKYRIE)
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
			.withQuestLeader(PM.NORN)
			.withQuestHomeBase("the Shrine of Destiny")
			.withIntermediateQuestLevel("the cave of Surtur")
			.add();

		AdventurerRole.withID(WIZARD_ID)
		    .named("Wizard")
		    .withPM(PM.WIZARD)
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
			.withPreferredPet(PM.KITTEN)
			.withFileCode("Wiz")
			.withQuestLeader(PM.NEFERET_THE_GREEN)
			.withQuestHomeBase("the Lonely Tower")
			.withIntermediateQuestLevel("the Tower of Darkness")
			.add();

	}
}
