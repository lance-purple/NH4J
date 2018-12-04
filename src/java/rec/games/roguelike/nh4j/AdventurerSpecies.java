package rec.games.roguelike.nh4j;

import java.util.HashMap;

public class AdventurerSpecies {

	private static final int HUMAN_ID = 0;
	private static final int ELF_ID = 1;
	private static final int DWARF_ID = 2;
	private static final int GNOME_ID = 3;
	private static final int ORC_ID = 4;

	private static HashMap<Integer, AdventurerSpecies> speciesByID = new HashMap<>();

	private final int speciesID;
	private String speciesNoun;
	private String speciesAdjective;
	private String speciesCollectiveNoun;
	private String fileCode;
	GenderedName individualName;
	
	private PM malePM;
	private PM femalePM;
	private PM mummyPM;
	private PM zombiePM;
	
	private long canStartAs;
	private long selfMask;
	private long friendshipMask = 0L;
	private long hostilityMask = 0L;
	
	private ATTRS minimumAttributes;
	private ATTRS maximumAttributes;
	
	private HitPointAdvancement hitPointAdvancement;
	private EnergyAdvancement energyAdvancement;

	private AdventurerSpecies(int speciesID) {
		this.speciesID = speciesID;
	}

	private AdventurerSpecies withSelf(long selfMask)
	{
		this.selfMask = selfMask;
		return this;
	}
	
	private AdventurerSpecies withNoun(String speciesNoun)
	{
		this.speciesNoun = speciesNoun;
		return this;
	}
	
	private AdventurerSpecies withAdjective(String speciesAdjective)
	{
		this.speciesAdjective = speciesAdjective;
		return this;
	}
	
	private AdventurerSpecies withCollectiveNoun(String speciesCollectiveNoun)
	{
		this.speciesCollectiveNoun = speciesCollectiveNoun;
		return this;
	}
	
	private AdventurerSpecies withFileCode(String fileCode)
	{
		this.fileCode = fileCode;
		return this;
	}
	
	private AdventurerSpecies withIndividualName(String maleVersion, String femaleVersion)
	{
		this.individualName = GenderedName.of(maleVersion, femaleVersion);
		return this;
	}
	
	private AdventurerSpecies withIndividualName(String genericVersion)
	{
		this.individualName = GenderedName.of(genericVersion);
		return this;
	}
	
	private AdventurerSpecies withPM(PM genericPM)
	{
		this.malePM = genericPM;
		this.femalePM = genericPM;
		return this;
	}
	
	private AdventurerSpecies withMummyPM(PM mummyPM)
	{
		this.mummyPM = mummyPM;
		return this;
	}
	
	private AdventurerSpecies withZombiePM(PM zombiePM)
	{
		this.zombiePM = zombiePM;
		return this;
	}
	
	private AdventurerSpecies canStartAs(long canStartAs)
	{
		this.canStartAs = canStartAs;
		return this;
	}
	
	private AdventurerSpecies friendship(long friendshipMask)
	{
		this.friendshipMask = friendshipMask;
		return this;
	}
	
	private AdventurerSpecies hostility(long hostilityMask)
	{
		this.hostilityMask = hostilityMask;
		return this;
	}
	
	private AdventurerSpecies minimums(ATTRS minimums)
	{
		this.minimumAttributes = minimums;
		return this;
	}
	
	private AdventurerSpecies maximums(ATTRS maximums)
	{
		this.maximumAttributes = maximums;
		return this;
	}

	private AdventurerSpecies with(HitPointAdvancement advancement)
	{
		this.hitPointAdvancement = advancement;
		return this;
	}

	private AdventurerSpecies with(EnergyAdvancement advancement)
	{
		this.energyAdvancement = advancement;
		return this;
	}

	private void add() {
		AdventurerSpecies.speciesByID.put(this.speciesID, this);
	}
	
	private static AdventurerSpecies withID(int roleID) {
		return new AdventurerSpecies(roleID);
	}

	private static AdventurerSpecies getSpecies(int speciesID) {
		if (speciesByID.isEmpty()) {
			initialize();
		}
		if (speciesByID.containsKey(speciesID)) {
			return speciesByID.get(speciesID);
		}
		return null;
	}

	public static int numberOfPlayableSpecies() {
		if (speciesByID.isEmpty()) {
			initialize();
		}
		return speciesByID.size();
	}

	public static long selfMask(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.selfMask;
		}
		return 0L;				
	}

	public static String speciesNoun(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.speciesNoun;
		}
		return "(unknown)";
	}

	public static String speciesCollectiveNoun(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.speciesCollectiveNoun;
		}
		return "(unknown)";
	}

	public static String speciesAdjective(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.speciesAdjective;
		}
		return "(unknown)";
	}

	public static String speciesFileCode(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.fileCode;
		}
		return "(unknown)";
	}

	public static boolean individualNameHasMaleVersion(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.individualName))
		{
			return species.individualName.hasMaleVersion();
		}
		return false;
	}
	
	public static String individualNameAsMale(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.individualName))
		{
			return species.individualName.asMale();
		}
		return "(unknown)";
	}
	
	public static boolean individualNameHasFemaleVersion(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.individualName))
		{
			return species.individualName.hasFemaleVersion();
		}
		return false;
	}

	public static String individualNameAsFemale(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.individualName))
		{
			return species.individualName.asFemale();
		}
		return "(unknown)";
	}
	
	public static boolean pmidHasFemaleVersion(int speciesID) {
		return PM.UNKNOWN.id() != pmidAsFemale(speciesID);
	}
	
	public static int pmidAsFemale(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.femalePM))
		{
			return species.femalePM.id();
		}
		return PM.UNKNOWN.id();
	}

	public static boolean pmidHasMaleVersion(int speciesID) {
		return PM.UNKNOWN.id() != pmidAsMale(speciesID);
	}
	
	public static int pmidAsMale(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.malePM))
		{
			return species.malePM.id();
		}
		return PM.UNKNOWN.id();
	}

	public static boolean pmidHasMummyVersion(int speciesID) {
		return PM.UNKNOWN.id() != pmidAsMummy(speciesID);
	}
	
	public static int pmidAsMummy(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.mummyPM))
		{
			return species.mummyPM.id();
		}
		return PM.UNKNOWN.id();
	}

	public static boolean pmidHasZombieVersion(int speciesID) {
		return PM.UNKNOWN.id() != pmidAsZombie(speciesID);
	}
	
	public static int pmidAsZombie(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.zombiePM))
		{
			return species.zombiePM.id();
		}
		return PM.UNKNOWN.id();
	}

	public static long startingMask(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.canStartAs | species.selfMask;
		}
		return 0L;				
	}

	public static long startingAlignmentMask(int speciesID) {
        return startingMask(speciesID) & AM.MASK;
	}

	public static long startingGenderMask(int speciesID) {
        return startingMask(speciesID) & Gender.MASK;
	}

	public static boolean canStartAsChaotic(int speciesID) {
		return (startingAlignmentMask(speciesID) & AM.CHAOTIC) != 0L;
	}

	public static boolean canStartAsNeutral(int speciesID) {
		return (startingAlignmentMask(speciesID) & AM.NEUTRAL) != 0L;
	}

	public static boolean canStartAsLawful(int speciesID) {
		return (startingAlignmentMask(speciesID) & AM.LAWFUL) != 0L;
	}
	
	public static long friendshipMask(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.friendshipMask;
		}
		return 0L;
	}

	public static long hostilityMask(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.hostilityMask;
		}
		return 0L;
	}
	
	public static int maximum(int speciesID, int whichAttr) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.maximumAttributes.get(whichAttr);
		}
		return 0;
	}
	
	public static int minimum(int speciesID, int whichAttr) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.minimumAttributes.get(whichAttr);
		}
		return 0;
	}
	
	public static int hitPointAdvancement(int speciesID, int experienceLevel, int cutoffLevel) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.hitPointAdvancement.forLevel(experienceLevel, cutoffLevel);
		}
		return 0;
	}
	
	public static EnergyAdvancement energyAdvancement(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species) {
			return species.energyAdvancement;
		}
		return EnergyAdvancement.NONE;
	}
	
	public static boolean validSpeciesID(int speciesID) {
	    return (speciesID >= 0) && (speciesID < numberOfPlayableSpecies());
	}

	public static final int MASK = 0x0ff8;

	public static final int roleMask()
	{
		return MASK;
	}
	
	private static void initialize() {

		AdventurerSpecies.withID(HUMAN_ID)
		.withSelf(M2.HUMAN)
		.withNoun("human")
		.withAdjective("human")
		.withCollectiveNoun("humanity")
		.withFileCode("Hum")
		.withIndividualName("man", "woman")
		.withPM(PM.HUMAN)
		.withMummyPM(PM.HUMAN_MUMMY)
		.withZombiePM(PM.HUMAN_ZOMBIE)
		.canStartAs(Gender.MALE.mask() | Gender.FEMALE.mask() |
				AM.LAWFUL | AM.NEUTRAL | AM.CHAOTIC)
		.hostility(M2.GNOME | M2.ORC)
		.minimums(new ATTRS().STR(3).INT(3).WIS(3).DEX(3).CON(3).CHA(3))
		.maximums(new ATTRS().STR_18(100).INT(18).WIS(18).DEX(18).CON(18).CHA(18))
		.with(HitPointAdvancement.of().init(2, 0).low(0, 2).high(1, 0))
		.with(EnergyAdvancement.of().init(1, 0).low(2, 0).high(2, 0))		
		.add();

		AdventurerSpecies.withID(ELF_ID)
		.withSelf(M2.ELF)
	    .withNoun("elf")
	    .withAdjective("elven")
	    .withCollectiveNoun("elvenkind")
		.withFileCode("Elf")
		.withIndividualName("elf")
		.withPM(PM.ELF)
		.withMummyPM(PM.ELF_MUMMY)
		.withZombiePM(PM.ELF_ZOMBIE)
		.canStartAs(Gender.MALE.mask() | Gender.FEMALE.mask() |
			    AM.CHAOTIC)
		.friendship(M2.ELF)
		.hostility(M2.ORC)
		.minimums(new ATTRS().STR(3).INT(3).WIS(3).DEX(3).CON(3).CHA(3))
		.maximums(new ATTRS().STR(18).INT(20).WIS(20).DEX(18).CON(16).CHA(18))
		.with(HitPointAdvancement.of().init(1, 0).low(0, 1).high(1, 0))
		.with(EnergyAdvancement.of().init(2, 0).low(3, 0).high(3, 0))		
		.add();

		AdventurerSpecies.withID(DWARF_ID)
		.withSelf(M2.DWARF)
	    .withNoun("dwarf")
	    .withAdjective("dwarven")
	    .withCollectiveNoun("dwarvenkind")
		.withFileCode("Dwa")
		.withIndividualName("dwarf")
		.withPM(PM.DWARF)
		.withMummyPM(PM.DWARF_MUMMY)
		.withZombiePM(PM.DWARF_ZOMBIE)
		.canStartAs(Gender.MALE.mask() | Gender.FEMALE.mask() |
			    AM.LAWFUL)
		.friendship(M2.DWARF | M2.GNOME)
		.hostility(M2.ORC)
		.minimums(new ATTRS().STR(3).INT(3).WIS(3).DEX(3).CON(3).CHA(3))
		.maximums(new ATTRS().STR_18(100).INT(16).WIS(16).DEX(20).CON(20).CHA(16))
		.with(HitPointAdvancement.of().init(4, 0).low(0, 3).high(2, 0))
		.with(EnergyAdvancement.of().init(0, 0).low(0, 0).high(0, 0))		
		.add();
		
		AdventurerSpecies.withID(GNOME_ID)
		.withSelf(M2.GNOME)
	    .withNoun("gnome")
	    .withAdjective("gnomish")
	    .withCollectiveNoun("gnomehood")
		.withFileCode("Gno")
		.withIndividualName("gnome")
		.withPM(PM.GNOME)
		.withMummyPM(PM.GNOME_MUMMY)
		.withZombiePM(PM.GNOME_ZOMBIE)
		.canStartAs(Gender.MALE.mask() | Gender.FEMALE.mask() |
			    AM.NEUTRAL)
		.friendship(M2.DWARF | M2.GNOME)
		.hostility(M2.HUMAN)
		.minimums(new ATTRS().STR(3).INT(3).WIS(3).DEX(3).CON(3).CHA(3))
		.maximums(new ATTRS().STR_18(50).INT(19).WIS(18).DEX(18).CON(18).CHA(18))
		.with(HitPointAdvancement.of().init(1, 0).low(0, 1).high(0, 0))
		.with(EnergyAdvancement.of().init(2, 0).low(2, 0).high(2, 0))		
		.add();

		AdventurerSpecies.withID(ORC_ID)
		.withSelf(M2.ORC)
	    .withNoun("orc")
	    .withAdjective("orcish")
	    .withCollectiveNoun("orcdom")
		.withFileCode("Orc")
		.withIndividualName("orc")
		.withPM(PM.ORC)
		.withMummyPM(PM.ORC_MUMMY)
		.withZombiePM(PM.ORC_ZOMBIE)
		.canStartAs(Gender.MALE.mask() | Gender.FEMALE.mask() |
			    AM.CHAOTIC)
		.hostility(M2.HUMAN | M2.ELF | M2.DWARF)
		.minimums(new ATTRS().STR(3).INT(3).WIS(3).DEX(3).CON(3).CHA(3))
		.maximums(new ATTRS().STR_18(20).INT(16).WIS(16).DEX(18).CON(18).CHA(16))
		.with(HitPointAdvancement.of().init(1, 0).low(0, 1).high(0, 0))
		.with(EnergyAdvancement.of().init(1, 0).low(1, 0).high(1, 0))		
		.add();
	}
}
