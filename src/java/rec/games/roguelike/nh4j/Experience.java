package rec.games.roguelike.nh4j;

public class Experience {

	public static final int MAXIMUM_PLAYER_CHARACTER_LEVEL = 30;

	private static int applyEnergyMultiplierForRole(int energy)
	{
	    switch (PlayerCharacter.currentRoleID()) {
	    case AdventurerRole.PRIEST_ID:
	    case AdventurerRole.WIZARD_ID:
	        return (2 * energy);
	        
	    case AdventurerRole.HEALER_ID:
	    case AdventurerRole.KNIGHT_ID:
	        return ((3 * energy) / 2);

	    case AdventurerRole.BARBARIAN_ID:
	    case AdventurerRole.VALKYRIE_ID:
	        return ((3 * energy) / 4);

	    default:
	        return energy;
	    }
	}
	
	/* calculate spell power/energy points for new level */
	public static int energyAdvancementForRoleAndSpecies(int wisdomModifier)
	{
		EnergyAdvancement roleAdvancement = AdventurerRole.energyAdvancement(PlayerCharacter.currentRoleID());
		EnergyAdvancement speciesAdvancement = AdventurerSpecies.energyAdvancement(PlayerCharacter.speciesID());
		
	    int en = 0;
	    int enrnd = 0;
	    int enfix = 0;

	    if (PlayerCharacter.currentExperienceLevel() == 0) {
	        en = roleAdvancement.initialConstant + speciesAdvancement.initialConstant;
	        en += Randomizer.rndIfNonzero(roleAdvancement.initialVariable);
	        en += Randomizer.rndIfNonzero(speciesAdvancement.initialVariable);
	    } else {
	        enrnd = wisdomModifier;
	        if (PlayerCharacter.currentExperienceLevel() < PlayerCharacter.cutoffLevel()) {
	            enfix += roleAdvancement.lowLevelConstant;
	            enfix += speciesAdvancement.lowLevelConstant;

	            enrnd += roleAdvancement.lowLevelVariable;
	            enrnd += speciesAdvancement.lowLevelVariable;
	        } else {
	            enfix += roleAdvancement.highLevelConstant;
	            enfix += speciesAdvancement.highLevelConstant;

	            enrnd += roleAdvancement.highLevelVariable;
	            enrnd += speciesAdvancement.highLevelVariable;
	        }
	        en = applyEnergyMultiplierForRole(Randomizer.rn1(enrnd, enfix));
	    }
	    if (en <= 0){
	        en = 1;
	    }
	    if (PlayerCharacter.currentExperienceLevel() < MAXIMUM_PLAYER_CHARACTER_LEVEL) {
	        PlayerCharacter.setMagicalEnergyIncreasePerLevel(PlayerCharacter.currentExperienceLevel(), en);
	    }
	    return en;
	}


}
