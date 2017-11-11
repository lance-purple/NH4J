package rec.games.roguelike.nh4j;

import java.util.ArrayList;
import java.util.Collections;
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
	

	private AdventurerSpecies(int speciesID) {
		this.speciesID = speciesID;
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

	private AdventurerSpecies withPM(PM malePM, PM femalePM)
	{
		this.malePM = malePM;
		this.femalePM = femalePM;
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
	
	public static String speciesNoun(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.speciesNoun;
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
	
	private static void initialize() {

		AdventurerSpecies.withID(HUMAN_ID)
		    .withNoun("human")
		    .withAdjective("human")
		    .withCollectiveNoun("humanity")
			.withFileCode("Hum")
			.withIndividualName("man", "woman")
			.withPM(PM.HUMAN)
			.withMummyPM(PM.HUMAN_MUMMY)
			.withZombiePM(PM.HUMAN_ZOMBIE)
			.add();

		AdventurerSpecies.withID(ELF_ID)
	    .withNoun("elf")
	    .withAdjective("elven")
	    .withCollectiveNoun("elvenkind")
		.withFileCode("Elf")
		.withIndividualName("elf")
		.withPM(PM.ELF)
		.withMummyPM(PM.ELF_MUMMY)
		.withZombiePM(PM.ELF_ZOMBIE)
		.add();

		AdventurerSpecies.withID(DWARF_ID)
	    .withNoun("dwarf")
	    .withAdjective("dwarven")
	    .withCollectiveNoun("dwarvenkind")
		.withFileCode("Dwa")
		.withIndividualName("dwarf")
		.withPM(PM.DWARF)
		.withMummyPM(PM.DWARF_MUMMY)
		.withZombiePM(PM.DWARF_ZOMBIE)
		.add();
		
		AdventurerSpecies.withID(GNOME_ID)
	    .withNoun("gnome")
	    .withAdjective("gnomish")
	    .withCollectiveNoun("gnomehood")
		.withFileCode("Gno")
		.withIndividualName("gnome")
		.withPM(PM.GNOME)
		.withMummyPM(PM.GNOME_MUMMY)
		.withZombiePM(PM.GNOME_ZOMBIE)
		.add();

		AdventurerSpecies.withID(ORC_ID)
	    .withNoun("orc")
	    .withAdjective("orcish")
	    .withCollectiveNoun("orcdom")
		.withFileCode("Orc")
		.withIndividualName("orc")
		.withPM(PM.ORC)
		.withMummyPM(PM.ORC_MUMMY)
		.withZombiePM(PM.ORC_ZOMBIE)
		.add();
	}
}
