package rec.games.roguelike.nh4j;

public class PlayerCharacter {

	private static int currentDungeonNumber;
	private static int currentDungeonLevel;

	private static int previousDungeonNumber;
	private static int previousDungeonLevel;

	private static int sentToDungeonNumber;
	private static int sentToDungeonLevel;
	private static int typeOfLevelYouWereSentTo;
	
	private static int currentX;
	private static int currentY;

	private static int directionX;
	private static int directionY;
	private static int directionZ;
	
	private static int originalX;
	private static int originalY;

	private static int destinationX;
	private static int destinationY;

	private static boolean youMoved;
	private static int lastTurningDirection;
	
	private static TrapType currentTrapType;
	private static int currentTrapTimeout;
	
	private static int currentExperienceLevel;
	private static int highestExperienceLevelSoFar;
	
	private static NHAttributeSet currentAttributes = new NHAttributeSet();
	private static NHAttributeSet maximumAttributes = new NHAttributeSet();
	private static NHAttributeSet attributeBonuses = new NHAttributeSet();
	private static NHAttributeSet attributeChangesFromExertion = new NHAttributeSet();
	private static NHAttributeSet temporaryAttributeChanges = new NHAttributeSet();
	private static NHAttributeSet attributeChangeTimeouts = new NHAttributeSet();

	private static NHAttributeSet attributesAsMonster = new NHAttributeSet();
	private static NHAttributeSet maximumAttributesAsMonster = new NHAttributeSet();

	private static HungerState currentHungerState = HungerState.NOT_HUNGRY;
	private static int currentNutrition;
	
	private static int abilityToConfuseMonsters;

	private static boolean sickWithFoodPoisoning;
	private static boolean sickWithIllness;
	
	public static int currentDungeonNumber() {
		return currentDungeonNumber;
	}
	
	public static int currentDungeonLevel() {
		return currentDungeonLevel;
	}

	public static void setCurrentDungeonNumber(int x) {
		currentDungeonNumber = x;
	}

	public static void setCurrentDungeonLevel(int x) {
		currentDungeonLevel = x;
	}

	public static int previousDungeonNumber() {
		return previousDungeonNumber;
	}
	
	public static int previousDungeonLevel() {
		return previousDungeonLevel;
	}

	public static void setPreviousDungeonNumber(int x) {
		previousDungeonNumber = x;
	}

	public static void setPreviousDungeonLevel(int x) {
		previousDungeonLevel = x;
	}

	public static int sentToDungeonNumber() {
		return sentToDungeonNumber;
	}
	
	public static int sentToDungeonLevel() {
		return sentToDungeonLevel;
	}

	public static void setSentToDungeonNumber(int x) {
		sentToDungeonNumber = x;
	}

	public static void setSentToDungeonLevel(int x) {
		sentToDungeonLevel = x;
	}

	public static int typeOfLevelYouWereSentTo() {
		return typeOfLevelYouWereSentTo;
	}	

	public static void setTypeOfLevelYouWereSentTo(int x) {
		typeOfLevelYouWereSentTo = x;
	}

	public static int currentX() {
		return currentX;
	}

	public static int currentY() {
		return currentY;
	}

	public static void setCurrentX(int x) {
		currentX = x;
	}

	public static void setCurrentY(int y) {
		currentY = y;
	}

	public static int directionX() {
		return directionX;
	}

	public static int directionY() {
		return directionY;
	}

	public static int directionZ() {
		return directionZ;
	}

	public static void setDirectionX(int x) {
		directionX = x;
	}

	public static void setDirectionY(int y) {
		directionY = y;
	}
	
	public static void setDirectionZ(int z) {
		directionZ = z;
	}

	public static int originalX() {
		return originalX;
	}

	public static int originalY() {
		return originalY;
	}

	public static void setOriginalX(int x) {
		originalX = x;
	}

	public static void setOriginalY(int y) {
		originalY = y;
	}

	public static int destinationX() {
		return destinationX;
	}

	public static int destinationY() {
		return destinationY;
	}

	public static void setDestinationX(int x) {
		destinationX = x;
	}

	public static void setDestinationY(int y) {
		destinationY = y;
	}
	
	public static boolean youMoved() {
		return youMoved;
	}
	
	public static void setYouMoved(boolean moved) {
		youMoved = moved;
	}
	
	public static int lastTurningDirection() {
		return lastTurningDirection;
	}

	public static void setLastTurningDirection(int d) {
		lastTurningDirection = d;
	}

	public static int currentTrapType() {
	    return (null != currentTrapType) ? currentTrapType.key() : 0;	
	}
	
	public static void setCurrentTrapType(int key) {
	    currentTrapType = TrapType.byKey(key);	
	}
	
	public static int currentTrapTimeout() {
		return currentTrapTimeout;
	}

	public static void setCurrentTrapTimeout(int timeout) {
		currentTrapTimeout = timeout;
	}

	public static int currentExperienceLevel() {
		return currentExperienceLevel;
	}

	public static void setCurrentExperienceLevel(int level) {
		currentExperienceLevel = level;
	}

	public static int highestExperienceLevelSoFar() {
		return highestExperienceLevelSoFar;
	}

	public static void setHighestExperienceLevelSoFar(int level) {
		highestExperienceLevelSoFar = level;
	}

	public static int currentAttribute(int attrIndex) {
		return currentAttributes.get(attrIndex);
	}

	public static void setCurrentAttribute(int attrIndex, int value) {
		currentAttributes.set(attrIndex, value);
	}

	public static void increaseCurrentAttribute(int attrIndex, int delta) {
		currentAttributes.increase(attrIndex, delta);
	}

	public static void decreaseCurrentAttribute(int attrIndex, int delta) {
		currentAttributes.decrease(attrIndex, delta);
	}

	public static int maximumAttribute(int attrIndex) {
		return maximumAttributes.get(attrIndex);
	}

	public static void setMaximumAttribute(int attrIndex, int value) {
		maximumAttributes.set(attrIndex, value);
	}

	public static void increaseMaximumAttribute(int attrIndex, int delta) {
		maximumAttributes.increase(attrIndex, delta);
	}

	public static void decreaseMaximumAttribute(int attrIndex, int delta) {
		maximumAttributes.decrease(attrIndex, delta);
	}

	public static int attributeBonus(int attrIndex) {
		return attributeBonuses.get(attrIndex);
	}

	public static void setAttributeBonus(int attrIndex, int value) {
		attributeBonuses.set(attrIndex, value);
	}

	public static void increaseAttributeBonus(int attrIndex, int delta) {
		attributeBonuses.increase(attrIndex, delta);
	}

	public static void decreaseAttributeBonus(int attrIndex, int delta) {
		attributeBonuses.decrease(attrIndex, delta);
	}

	public static int attributeChangeFromExertion(int attrIndex) {
		return attributeChangesFromExertion.get(attrIndex);
	}

	public static void setAttributeChangeFromExertion(int attrIndex, int value) {
		attributeChangesFromExertion.set(attrIndex, value);
	}

	public static void increaseAttributeChangeFromExertion(int attrIndex, int delta) {
		attributeChangesFromExertion.increase(attrIndex, delta);
	}

	public static void decreaseAttributeChangeFromExertion(int attrIndex, int delta) {
		attributeChangesFromExertion.decrease(attrIndex, delta);
	}

	public static int temporaryAttributeChange(int attrIndex) {
		return temporaryAttributeChanges.get(attrIndex);
	}

	public static void setTemporaryAttributeChange(int attrIndex, int value) {
		temporaryAttributeChanges.set(attrIndex, value);
	}

	public static void increaseTemporaryAttributeChange(int attrIndex, int delta) {
		temporaryAttributeChanges.increase(attrIndex, delta);
	}

	public static void decreaseTemporaryAttributeChange(int attrIndex, int delta) {
		temporaryAttributeChanges.decrease(attrIndex, delta);
	}

	public static int attributeChangeTimeout(int attrIndex) {
		return attributeChangeTimeouts.get(attrIndex);
	}

	public static void setAttributeChangeTimeout(int attrIndex, int value) {
		attributeChangeTimeouts.set(attrIndex, value);
	}

	public static void increaseAttributeChangeTimeout(int attrIndex, int delta) {
		attributeChangeTimeouts.increase(attrIndex, delta);
	}

	public static void decreaseAttributeChangeTimeout(int attrIndex, int delta) {
		attributeChangeTimeouts.decrease(attrIndex, delta);
	}

	public static int attributeAsMonster(int attrIndex) {
		return attributesAsMonster.get(attrIndex);
	}

	public static void setAttributeAsMonster(int attrIndex, int value) {
		attributesAsMonster.set(attrIndex, value);
	}

	public static void increaseAttributeAsMonster(int attrIndex, int delta) {
		attributesAsMonster.increase(attrIndex, delta);
	}

	public static void decreaseAttributeAsMonster(int attrIndex, int delta) {
		attributesAsMonster.decrease(attrIndex, delta);
	}

	public static int maximumAttributeAsMonster(int attrIndex) {
		return maximumAttributesAsMonster.get(attrIndex);
	}

	public static void setMaximumAttributeAsMonster(int attrIndex, int value) {
		maximumAttributesAsMonster.set(attrIndex, value);
	}

	public static void increaseMaximumAttributeAsMonster(int attrIndex, int delta) {
		maximumAttributesAsMonster.increase(attrIndex, delta);
	}

	public static void decreaseMaximumAttributeAsMonster(int attrIndex, int delta) {
		maximumAttributesAsMonster.decrease(attrIndex, delta);
	}

	public static final int currentNutrition() {
		return currentNutrition;
	}

	public static final void setCurrentNutrition(int n) {
		currentNutrition = n;
	}

	public static final int currentHungerState() {
	    return (null != currentHungerState) ? currentHungerState.key() : 0;	
	}

	public static final void setCurrentHungerState(int key) {
		currentHungerState = HungerState.byKey(key);
	}
	
	public static final int abilityToConfuseMonsters() {
		return abilityToConfuseMonsters;
	}

	public static final void setAbilityToConfuseMonsters(int a) {
		abilityToConfuseMonsters = a;
	}

	public static final boolean sickWithFoodPoisoning() {
		return sickWithFoodPoisoning;
	}

	public static final boolean sickWithIllness() {
		return sickWithIllness;
	}

	public static final void setSickWithFoodPoisoning(boolean p) {
		sickWithFoodPoisoning = p;
	}

	public static final void setSickWithIllness(boolean i) {
		sickWithIllness = i;
	}

}
