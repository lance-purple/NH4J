package rec.games.roguelike.nh4j;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

public class AdventurerRole {

	public static final int ARCHAEOLOGIST_ID = 0;
	public static final int BARBARIAN_ID = 1;
	public static final int CAVEMAN_ID = 2;
	public static final int HEALER_ID = 3;
	public static final int KNIGHT_ID = 4;
	public static final int MONK_ID = 5;
	public static final int PRIEST_ID = 6;
	public static final int ROGUE_ID = 7;
	public static final int RANGER_ID = 8;
	public static final int SAMURAI_ID = 9;
	public static final int TOURIST_ID = 10;
	public static final int VALKYRIE_ID = 11;
	public static final int WIZARD_ID = 12;

	private static HashMap<Integer, AdventurerRole> rolesByID = new HashMap<>();

	private final int roleID;
	private GenderedName roleName;
	private ArrayList<GenderedName> rankNames = new ArrayList<>();
	private Pantheon pantheon = null;
	private String fileCode;
	private PM malePM;
	private PM femalePM;
	
	private ATTRS attributeBases;
	private ATTRS attributeDistributions;
	
	private long canStartAs;
	
	private int cutoffLevel;
	private HitPointAdvancement hitPointAdvancement;
	private EnergyAdvancement energyAdvancement;
	
	private int initialAlignmentRecord;
	private int baseSpellcastingPenalty;
	
	private PM preferredPet = PM.UNKNOWN;

	private PM questLeader = PM.UNKNOWN;
	private PM questGuardians = PM.UNKNOWN;
	private PM questNemesis = PM.UNKNOWN;
	private PM questEnemy1 = PM.UNKNOWN;
	private PM questEnemy2 = PM.UNKNOWN;
	private MC questEnemyClass1 = MC.UNKNOWN;
	private MC questEnemyClass2 = MC.UNKNOWN;

	private String questHomeBase;
	private String intermediateQuestLevel;
	private ART questArtifact = ART.NONE;

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
	
	private AdventurerRole withQuestGuardians(PM guardians)
	{
		this.questGuardians = guardians;
		return this;
	}
	
	private AdventurerRole withQuestNemesis(PM nemesis)
	{
		this.questNemesis = nemesis;
		return this;
	}

	private AdventurerRole withQuestEnemies(PM enemy1, MC enemyClass1, MC enemyClass2)
	{
		this.questEnemy1 = enemy1;
		this.questEnemyClass1 = enemyClass1;
		this.questEnemyClass2 = enemyClass2;
		return this;
	}
	
	private AdventurerRole withQuestEnemies(PM enemy1, PM enemy2, MC enemyClass1, MC enemyClass2)
	{
		this.questEnemy1 = enemy1;
		this.questEnemy2 = enemy2;
		this.questEnemyClass1 = enemyClass1;
		this.questEnemyClass2 = enemyClass2;
		return this;
	}
	
	private AdventurerRole withQuestArtifact(ART artifact)
	{
		this.questArtifact  = artifact;
		return this;
	}
	
	private AdventurerRole withBase(ATTRS attrs)
	{
		this.attributeBases = attrs;
		return this;
	}
	
	private AdventurerRole withDistribution(ATTRS attrs)
	{
		this.attributeDistributions = attrs;
		return this;
	}
	
	private AdventurerRole withCutoffLevel(int level)
	{
		this.cutoffLevel = level;
		return this;
	}
	
	private AdventurerRole canStartAs(long canStartAs)
	{
		this.canStartAs = canStartAs;
		return this;
	}
	
	private AdventurerRole with(HitPointAdvancement advancement)
	{
		this.hitPointAdvancement = advancement;
		return this;
	}

	private AdventurerRole with(EnergyAdvancement advancement)
	{
		this.energyAdvancement = advancement;
		return this;
	}

	private AdventurerRole withInitialAlignmentRecord(int initialAlignmentRecord)
	{
		this.initialAlignmentRecord = initialAlignmentRecord;
		return this;
	}

	private AdventurerRole withBaseSpellcastingPenalty(int penalty)
	{
		this.baseSpellcastingPenalty = penalty;
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
	
	public static int pmidOfQuestGuardians(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.questGuardians.id();
		}
		return PM.UNKNOWN.id();
	}
	
	public static boolean hasQuestGuardians(int roleID) {
		return PM.UNKNOWN.id() != pmidOfQuestGuardians(roleID);
	}
	
	public static int pmidOfQuestNemesis(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.questNemesis.id();
		}
		return PM.UNKNOWN.id();
	}
	
	public static boolean hasQuestNemesis(int roleID) {
		return PM.UNKNOWN.id() != pmidOfQuestNemesis(roleID);
	}

	public static int pmidOfQuestEnemies(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.questEnemy1.id();
		}
		return PM.UNKNOWN.id();
	}

	public static int pmidOfOtherQuestEnemies(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.questEnemy2.id();
		}
		return PM.UNKNOWN.id();
	}

	public static int classOfQuestEnemies(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.questEnemyClass1.id();
		}
		return MC.UNKNOWN.id();
	}

	public static int classOfOtherQuestEnemies(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.questEnemyClass2.id();
		}
		return MC.UNKNOWN.id();
	}
	
	public static int idOfQuestArtifact(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.questArtifact.id();
		}
		return ART.NONE.id();		
	}
	
	public static boolean canStartAsMaleOrFemale(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return (role.canStartAs & Gender.MASK) == (Gender.MALE.mask() | Gender.FEMALE.mask());
		}
		return false;		
	}

	public static long startingMask(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.canStartAs;
		}
		return 0L;				
	}

	public static long startingAlignmentMask(int roleID) {
		return startingMask(roleID) & AM.MASK;
	}

	public static long startingGenderMask(int roleID) {
		return startingMask(roleID) & Gender.MASK;
	}

	public static long startingSpeciesMask(int roleID) {
		return startingMask(roleID) & AdventurerSpecies.MASK;
	}

	public static int attributeBase(int roleID, int whichAttr) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.attributeBases.get(whichAttr);
		}
		return 0;		
	}
	
	public static int attributeDistribution(int roleID, int whichAttr) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.attributeDistributions.get(whichAttr);
		}
		return 0;		
	}
	
	public static int cutoffLevel(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role)
		{
			return role.cutoffLevel;
		}
		return 0;		
	}

	public static int hitPointAdvancement(int roleID, int experienceLevel) {
		AdventurerRole role = getRole(roleID);
		if (null != role) {
			return role.hitPointAdvancement.forLevel(experienceLevel, role.cutoffLevel);
		}
		return 0;
	}

	public static EnergyAdvancement energyAdvancement(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role) {
			return role.energyAdvancement;
		}
		return EnergyAdvancement.NONE;
	}

	public static boolean hasQuestArtifact(int roleID) {
		return (0 != idOfQuestArtifact(roleID));
	}

	public static int initialAlignmentRecord(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role) {
			return role.initialAlignmentRecord;
		}
		return 0;
	}

	public static int baseSpellcastingPenalty(int roleID) {
		AdventurerRole role = getRole(roleID);
		if (null != role) {
			return role.baseSpellcastingPenalty;
		}
		return 0;
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
			.withQuestGuardians(PM.STUDENT)
			.withQuestNemesis(PM.MINION_OF_HUHETOTL)
			.withQuestEnemies(PM.HUMAN_MUMMY, MC.SNAKE, MC.MUMMY)
			.withQuestHomeBase("the College of Archeology")
			.withIntermediateQuestLevel( "the Tomb of the Toltec Kings")
			.withQuestArtifact(ART.ORB_OF_DETECTION)
			.canStartAs(M2.HUMAN | M2.DWARF | M2.GNOME |
					    Gender.MALE.mask() | Gender.FEMALE.mask() |
					    AM.LAWFUL | AM.NEUTRAL)
			.withBase(new ATTRS().STR(7).INT(10).WIS(10).DEX(7).CON(7).CHA(7))
			.withDistribution(new ATTRS().STR(20).INT(20).WIS(20).DEX(10).CON(20).CHA(10))
			.withCutoffLevel(14)
			.with(HitPointAdvancement.of().init(11, 0).low(0, 8).high(1, 0))
			.with(EnergyAdvancement.of().init(1, 0).low(0, 1).high(0, 1))
			.withInitialAlignmentRecord(10)
			.withBaseSpellcastingPenalty(5)
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
			.withQuestGuardians(PM.CHIEFTAIN)
			.withQuestNemesis(PM.THOTH_AMON)
			.withQuestEnemies(PM.OGRE, PM.TROLL, MC.OGRE, MC.TROLL)
			.withQuestHomeBase("the Camp of the Duali Tribe")
			.withIntermediateQuestLevel("the Duali Oasis")
			.withQuestArtifact(ART.HEART_OF_AHRIMAN)
			.canStartAs(M2.HUMAN | M2.ORC |
				    Gender.MALE.mask() | Gender.FEMALE.mask() |
				    AM.NEUTRAL | AM.CHAOTIC)
			.withBase(new ATTRS().STR(16).INT(7).WIS(7).DEX(15).CON(16).CHA(6))
			.withDistribution(new ATTRS().STR(30).INT(6).WIS(7).DEX(20).CON(30).CHA(7))
			.withCutoffLevel(10)
			.with(HitPointAdvancement.of().init(14, 0).low(0, 10).high(2, 0))
			.with(EnergyAdvancement.of().init(1, 0).low(0, 1).high(0, 1))
			.withInitialAlignmentRecord(10)
			.withBaseSpellcastingPenalty(14)
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
			.withQuestGuardians(PM.NEANDERTHAL)
			.withQuestNemesis(PM.CHROMATIC_DRAGON)
			.withQuestEnemies(PM.BUGBEAR, PM.HILL_GIANT, MC.HUMANOID, MC.GIANT)
			.withQuestHomeBase("the Caves of the Ancestors")
			.withIntermediateQuestLevel("the Dragon's Lair")
			.withQuestArtifact(ART.SCEPTRE_OF_MIGHT)
			.canStartAs(M2.HUMAN | M2.DWARF | M2.GNOME |
				    Gender.MALE.mask() | Gender.FEMALE.mask() |
				    AM.LAWFUL | AM.NEUTRAL)
			.withBase(new ATTRS().STR(10).INT(7).WIS(7).DEX(7).CON(8).CHA(6))
			.withDistribution(new ATTRS().STR(30).INT(6).WIS(7).DEX(20).CON(30).CHA(7))
			.withCutoffLevel(10)
			.with(HitPointAdvancement.of().init(14, 0).low(0, 8).high(2, 0))
			.with(EnergyAdvancement.of().init(1, 0).low(0, 1).high(0, 1))
			.withInitialAlignmentRecord(0)
			.withBaseSpellcastingPenalty(12)
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
			.withQuestGuardians(PM.ATTENDANT)
			.withQuestNemesis(PM.CYCLOPS)
			.withQuestEnemies(PM.GIANT_RAT, PM.SNAKE, MC.RODENT, MC.YETI)
			.withQuestHomeBase("the Temple of Epidaurus")
			.withIntermediateQuestLevel("the Temple of Coeus")
			.withQuestArtifact(ART.STAFF_OF_AESCULAPIUS)
			.canStartAs(M2.HUMAN | M2.GNOME |
				    Gender.MALE.mask() | Gender.FEMALE.mask() |
				    AM.NEUTRAL)
			.withBase(new ATTRS().STR(7).INT(7).WIS(13).DEX(7).CON(11).CHA(16))
			.withDistribution(new ATTRS().STR(15).INT(20).WIS(20).DEX(15).CON(25).CHA(5))
			.withCutoffLevel(20)
			.with(HitPointAdvancement.of().init(11, 0).low(0, 8).high(1, 0))
			.with(EnergyAdvancement.of().init(1, 4).low(0, 1).high(0, 2))
			.withInitialAlignmentRecord(10)
			.withBaseSpellcastingPenalty(3)
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
			.withQuestGuardians(PM.PAGE)
			.withQuestNemesis(PM.IXOTH)
			.withQuestEnemies(PM.QUASIT, PM.OCHRE_JELLY, MC.IMP, MC.JELLY)
			.withQuestHomeBase("Camelot Castle")
			.withIntermediateQuestLevel("the Isle of Glass")
			.withQuestArtifact(ART.MAGIC_MIRROR_OF_MERLIN)
			.canStartAs(M2.HUMAN |
				    Gender.MALE.mask() | Gender.FEMALE.mask() |
				    AM.LAWFUL)
			.withBase(new ATTRS().STR(13).INT(7).WIS(14).DEX(8).CON(10).CHA(17))
			.withDistribution(new ATTRS().STR(30).INT(15).WIS(15).DEX(10).CON(20).CHA(10))
			.withCutoffLevel(10)
			.with(HitPointAdvancement.of().init(14, 0).low(0, 8).high(2, 0))
			.with(EnergyAdvancement.of().init(1, 4).low(0, 1).high(0, 2))
			.withInitialAlignmentRecord(10)
			.withBaseSpellcastingPenalty(8)
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
			.withQuestGuardians(PM.ABBOT)
			.withQuestNemesis(PM.MASTER_KAEN)
			.withQuestEnemies(PM.EARTH_ELEMENTAL, PM.XORN, MC.ELEMENTAL, MC.XORN)
			.withQuestHomeBase( "the Monastery of Chan-Sune")
			.withIntermediateQuestLevel("the Monastery of the Earth-Lord")
			.withQuestArtifact(ART.EYES_OF_THE_OVERWORLD)
			.canStartAs(M2.HUMAN |
				    Gender.MALE.mask() | Gender.FEMALE.mask() |
				    AM.LAWFUL | AM.NEUTRAL | AM.CHAOTIC)
			.withBase(new ATTRS().STR(10).INT(7).WIS(8).DEX(8).CON(7).CHA(7))
			.withDistribution(new ATTRS().STR(25).INT(10).WIS(20).DEX(20).CON(15).CHA(10))
			.withCutoffLevel(10)
			.with(HitPointAdvancement.of().init(12, 0).low(0, 8).high(1, 0))
			.with(EnergyAdvancement.of().init(2, 2).low(0, 2).high(0, 2))
			.withInitialAlignmentRecord(10)
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
			.withQuestGuardians(PM.ACOLYTE)
			.withQuestNemesis(PM.NALZOK)
			.withQuestEnemies(PM.HUMAN_ZOMBIE, PM.WRAITH, MC.ZOMBIE, MC.WRAITH)
			.withQuestHomeBase("the Great Temple")
			.withIntermediateQuestLevel("the Temple of Nalzok")
			.withQuestArtifact(ART.MITRE_OF_HOLINESS)
			.canStartAs(M2.HUMAN | M2.ELF |
				    Gender.MALE.mask() | Gender.FEMALE.mask() |
				    AM.LAWFUL | AM.NEUTRAL | AM.CHAOTIC)
			.withBase(new ATTRS().STR(7).INT(7).WIS(10).DEX(7).CON(7).CHA(7))
			.withDistribution(new ATTRS().STR(15).INT(10).WIS(30).DEX(15).CON(20).CHA(10))
			.withCutoffLevel(10)
			.with(HitPointAdvancement.of().init(12, 0).low(0, 8).high(1, 0))
			.with(EnergyAdvancement.of().init(4, 3).low(0, 2).high(0, 2))
			.withInitialAlignmentRecord(0)
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
			.withQuestGuardians(PM.THUG)
			.withQuestNemesis(PM.MASTER_ASSASSIN)
			.withQuestEnemies(PM.LEPRECHAUN, PM.GUARDIAN_NAGA, MC.NYMPH, MC.NAGA)
			.withQuestHomeBase("the Thieves' Guild Hall")
			.withIntermediateQuestLevel("the Assassins' Guild Hall")
			.withQuestArtifact(ART.MASTER_KEY_OF_THIEVERY)
			.canStartAs(M2.HUMAN | M2.ORC |
				    Gender.MALE.mask() | Gender.FEMALE.mask() |
				    AM.CHAOTIC)
			.withBase(new ATTRS().STR(7).INT(7).WIS(7).DEX(10).CON(7).CHA(6))
			.withDistribution(new ATTRS().STR(20).INT(10).WIS(10).DEX(30).CON(20).CHA(10))
			.withCutoffLevel(11)
			.with(HitPointAdvancement.of().init(10, 0).low(0, 8).high(1, 0))
			.with(EnergyAdvancement.of().init(1, 0).low(0, 1).high(0, 1))
			.withInitialAlignmentRecord(10)
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
			.withQuestGuardians(PM.HUNTER)
			.withQuestNemesis(PM.SCORPIUS)
			.withQuestEnemies(PM.FOREST_CENTAUR, PM.SCORPION, MC.CENTAUR, MC.SPIDER)
			.withQuestHomeBase("Orion's camp")
			.withIntermediateQuestLevel("the cave of the wumpus")
			.withQuestArtifact(ART.LONGBOW_OF_DIANA)
			.canStartAs(M2.HUMAN | M2.ELF | M2.ORC | M2.GNOME |
				    Gender.MALE.mask() | Gender.FEMALE.mask() |
				    AM.NEUTRAL | AM.CHAOTIC)
			.withBase(new ATTRS().STR(13).INT(13).WIS(13).DEX(9).CON(13).CHA(7))
			.withDistribution(new ATTRS().STR(30).INT(10).WIS(10).DEX(20).CON(20).CHA(10))
			.withCutoffLevel(12)
			.with(HitPointAdvancement.of().init(13, 0).low(0, 6).high(1, 0))
			.with(EnergyAdvancement.of().init(1, 0).low(0, 1).high(0, 1))
			.withInitialAlignmentRecord(10)
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
			.withQuestGuardians(PM.ROSHI)
			.withQuestNemesis(PM.ASHIKAGA_TAKAUJI)
			.withQuestEnemies(PM.WOLF, PM.STALKER, MC.DOG, MC.ELEMENTAL)
			.withQuestHomeBase("the Castle of the Taro Clan")
			.withIntermediateQuestLevel("the Shogun's Castle")
			.withQuestArtifact(ART.TSURUGI_OF_MURAMASA)
			.canStartAs(M2.HUMAN |
				    Gender.MALE.mask() | Gender.FEMALE.mask() |
				    AM.LAWFUL)
			.withBase(new ATTRS().STR(10).INT(8).WIS(7).DEX(10).CON(17).CHA(6))
			.withDistribution(new ATTRS().STR(30).INT(10).WIS(8).DEX(30).CON(14).CHA(8))
			.withCutoffLevel(11)
			.with(HitPointAdvancement.of().init(13, 0).low(0, 8).high(1, 0))
			.with(EnergyAdvancement.of().init(1, 0).low(0, 1).high(0, 1))
			.withInitialAlignmentRecord(10)
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
			.withQuestGuardians(PM.GUIDE)
			.withQuestNemesis(PM.MASTER_OF_THIEVES)
			.withQuestEnemies(PM.GIANT_SPIDER, PM.FOREST_CENTAUR, MC.SPIDER, MC.CENTAUR)
			.withQuestHomeBase("Ankh-Morpork")
			.withIntermediateQuestLevel("the Thieves' Guild Hall")
			.withQuestArtifact(ART.YENDORIAN_EXPRESS_CARD)
			.canStartAs(M2.HUMAN |
				    Gender.MALE.mask() | Gender.FEMALE.mask() |
				    AM.NEUTRAL)
			.withBase(new ATTRS().STR(7).INT(10).WIS(6).DEX(7).CON(7).CHA(10))
			.withDistribution(new ATTRS().STR(15).INT(10).WIS(10).DEX(15).CON(30).CHA(20))
			.withCutoffLevel(14)
			.with(HitPointAdvancement.of().init(8, 0).low(0, 8).high(0, 0))
			.with(EnergyAdvancement.of().init(1, 0).low(0, 1).high(0, 1))
			.withInitialAlignmentRecord(0)
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
			.withQuestGuardians(PM.WARRIOR)
			.withQuestNemesis(PM.LORD_SURTUR)
			.withQuestEnemies(PM.FIRE_ANT, PM.FIRE_GIANT, MC.ANT, MC.GIANT)
			.withQuestHomeBase("the Shrine of Destiny")
			.withIntermediateQuestLevel("the cave of Surtur")
			.withQuestArtifact(ART.ORB_OF_FATE)
			.canStartAs(M2.HUMAN | M2.DWARF |
				    Gender.FEMALE.mask() |
				    AM.LAWFUL | AM.NEUTRAL)
			.withBase(new ATTRS().STR(10).INT(7).WIS(7).DEX(7).CON(10).CHA(7))
			.withDistribution(new ATTRS().STR(30).INT(6).WIS(7).DEX(20).CON(30).CHA(7))
			.withCutoffLevel(10)
			.with(HitPointAdvancement.of().init(14, 0).low(0, 8).high(2, 0))
			.with(EnergyAdvancement.of().init(1, 0).low(0, 1).high(0, 1))
			.withInitialAlignmentRecord(0)
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
			.withQuestGuardians(PM.APPRENTICE)
			.withQuestNemesis(PM.DARK_ONE)
			.withQuestEnemies(PM.VAMPIRE_BAT, PM.XORN, MC.BAT, MC.WRAITH)
			.withQuestHomeBase("the Lonely Tower")
			.withIntermediateQuestLevel("the Tower of Darkness")
			.withQuestArtifact(ART.EYE_OF_THE_AETHIOPICA)
			.canStartAs(M2.HUMAN | M2.ELF | M2.ORC | M2.GNOME |
				    Gender.MALE.mask() | Gender.FEMALE.mask() |
				    AM.NEUTRAL | AM.CHAOTIC)
			.withBase(new ATTRS().STR(7).INT(10).WIS(7).DEX(7).CON(7).CHA(7))
			.withDistribution(new ATTRS().STR(10).INT(30).WIS(10).DEX(20).CON(20).CHA(10))
			.withCutoffLevel(12)
			.with(HitPointAdvancement.of().init(10, 8).low(0, 8).high(1, 0))
			.with(EnergyAdvancement.of().init(4, 3).low(0, 2).high(0, 3))
			.withInitialAlignmentRecord(0)
			.add();

	}
}
