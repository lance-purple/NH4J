
package rec.games.roguelike.nh4j;

import java.util.HashMap;

public class PlayerCharacter {
	
	private static final int MAXULEVEL = 30;

	private static final int SICK_VOMITABLE = 0x01;
	private static final int SICK_NON_VOMITABLE = 0x02;
	private static final int SICK_ALL = 0x03;

	private static int currentRoleID;
	private static int speciesID;
	
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
	
	private static long sleepingSinceMove;

	private static boolean immersed;
	private static boolean swallowed;
	private static boolean buried;
	
	private static int timeInVault;
	private static int timeSinceBeingSwallowed;
	private static int timeSinceBecomingADemigod;
	
	private static int currentExperienceLevel;
	private static int highestExperienceLevelSoFar;
	private static long currentXP;
	private static long currentScore;
	
	private static Pantheon currentPantheon;
	private static int currentAlignmentType;
	private static int currentAlignmentRecord;
	
	private static int originalAlignmentBase;
	private static int currentAlignmentBase;
	private static int currentLuck;
	private static int luckBonus;
	private static int blessings;
	private static int timeToNextBlessing;
	private static int divineWrath;
	private static long cleansedSinceMove;
	private static boolean invulnerableWhilePraying;
		
	private static int toHitModifier;
	private static int damageBonus;
	private static boolean usingTwoWeapons;
	
	private static int armorClass;
	private static int armorBonusFromProtectionSpell;
	private static int protectionSpellPointDuration;
	private static int protectionSpellPointCountdown;
	
	private static ATTRS currentAttributes = new ATTRS();
	private static ATTRS maximumAttributes = new ATTRS();
	private static ATTRS attributeBonuses = new ATTRS();
	private static ATTRS attributeChangesFromExertion = new ATTRS();
	private static ATTRS temporaryAttributeChanges = new ATTRS();
	private static ATTRS attributeChangeTimeouts = new ATTRS();

	private static ATTRS attributesAsMonster = new ATTRS();
	private static ATTRS maximumAttributesAsMonster = new ATTRS();
	
	private static int currentHitPoints;
	private static int maximumHitPoints;
	private static int currentHitPointsAsMonster;
	private static int maximumHitPointsAsMonster;

	private static int hitPointIncreasePerLevel[] = new int[MAXULEVEL];

	private static int originalMonsterNumber;
	private static int currentMonsterNumber;
	private static int timeRemainingAsMonster;
	
	private static boolean inherentlyFemale; /* since you might be polymorphed into a male being */
	
	private static boolean lurking; /* you are an undetected piercer or other hidden monster */
	
	private static int deathCount;
	private static int ariseFromGraveAsMonster;
	
	private static int lycanthropeType;
	
	private static int currentMagicalEnergy;
	private static int maximumMagicalEnergy;

	private static int magicalEnergyIncreasePerLevel[] = new int[MAXULEVEL];

	private static long remainingGallopTime;
	private static int turnsRiddenSinceLastSkillCheck;
	
	private static HungerState currentHungerState = HungerState.NOT_HUNGRY;
	private static int currentNutrition;
	
	private static boolean canSenseUnsafeFood;
	
	private static int abilityToConfuseMonsters;
	
	private static int creamed;
	
	private static int nightVisionRange;
	private static int xRayVisionRange;
	
	private static boolean feltBall;
	private static boolean feltChain;
	
	private static int glyphUnderBall;
	private static int glyphUnderChain;
	private static int ballAndChainOrder;
	
	private static boolean sickWithFoodPoisoning;
	private static boolean sickWithIllness;
	
	private static int giftsBestowed;

	private static HashMap<Integer, Boolean> achievements = new HashMap<>();
	private static HashMap<Integer, Boolean> specialItems = new HashMap<>();
	
	private static int startingMoney;
	
	private static int numBonesFilesLoaded;
	private static boolean permanentlyBlind = false;
	private static boolean nudist = false;

	private static boolean vegetarianConduct = true;
	private static boolean veganConduct = true;
	private static boolean foodlessConduct = true;
	private static boolean atheistConduct = true;

	private static long literacyCount = 0;
	private static long weaponHitCount = 0;
	private static long polypileCount = 0;
	private static long polyselfCount = 0;
	private static long wishCount = 0;
	private static long artifactWishCount = 0;

	private static boolean pacifistConduct = true;
	
	private static boolean gotMinorOracle = false;
	private static boolean gotMajorOracle = false;
	private static boolean hasReadTribute = false;
	private static boolean calledByQuestLeader = false;
	private static boolean expelledFromQuestDungeon = false;
	private static boolean completedQuest = false;
	private static boolean haveOpenedDrawbridge = false;
	private static boolean haveInvokedGateToSanctum = false;
	private static boolean enteredGehennomViaValley = false;
	
	private static int handOfElberethLevel = 0;
	
	private static boolean becameDemigod = false;
	private static boolean steppedOnVibratingSquare = false;
	private static boolean haveAscended = false;
	
	private static int knowledgeOfPasstune = 0;
	
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

	public static long sleepingSinceMove() {
		return sleepingSinceMove;
	}
	
	public static void setSleepingSinceMove(long m) {
		sleepingSinceMove = m;
	}

	public static boolean underwater() {
		return immersed;
	}
	
	public static void setUnderwater(boolean w) {
		immersed = w;
	}

	/** for states like swimming, wading, etc. */
	public static boolean inWater() {
		return immersed;
	}

	public static void setInWater(boolean w) {
		immersed = w;
	}

	public static boolean buried() {
		return buried;
	}
	
	public static void setBuried(boolean b) {
		buried = b;
	}

	public static int timeInVault() {
		return timeInVault;
	}

	public static void setTimeInVault(int t) {
		timeInVault = t;
	}

	public static void increaseTimeInVault(int t) {
		timeInVault += t;
	}

	public static void decreaseTimeInVault(int t) {
		timeInVault -= t;
	}
	
	public static boolean swallowed() {
		return swallowed;
	}
	
	public static void setSwallowed(boolean s) {
		swallowed = s;
	}

	public static int timeSinceBeingSwallowed() {
		return timeSinceBeingSwallowed;
	}

	public static void setTimeSinceBeingSwallowed(int t) {
		timeSinceBeingSwallowed = t;
	}

	public static void increaseTimeSinceBeingSwallowed(int t) {
		timeSinceBeingSwallowed += t;
	}

	public static void decreaseTimeSinceBeingSwallowed(int t) {
		timeSinceBeingSwallowed -= t;
	}

	public static int timeSinceBecomingADemigod() {
		return timeSinceBecomingADemigod;
	}

	public static void setTimeSinceBecomingADemigod(int t) {
		timeSinceBecomingADemigod = t;
	}

	public static void increaseTimeSinceBecomingADemigod(int t) {
		timeSinceBecomingADemigod += t;
	}

	public static void decreaseTimeSinceBecomingADemigod(int t) {
		timeSinceBecomingADemigod -= t;
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

	public static long currentXP() {
		return currentXP;
	}

	public static void setCurrentXP(long xp) {
		currentXP = xp;
	}

	public static long currentScore() {
		return currentScore;
	}

	public static void setCurrentScore(long score) {
		currentScore = score;
	}

	public static void increaseCurrentScore(long score) {
		currentScore += score;
		 if (currentScore < 0) {
			 currentScore = Long.MAX_VALUE;
		 }
	}

	public static void decreaseCurrentScore(long score) {
		currentScore -= score;
	}

	public static int hitPointIncreasePerLevel(int level) {
		return hitPointIncreasePerLevel[level];
	}

	public static void setHitPointIncreasePerLevel(int level, int hp) {
		hitPointIncreasePerLevel[level] = hp;
	}

	public static int originalAlignmentBase() {
		return originalAlignmentBase;
	}

	public static void setOriginalAlignmentBase(int b) {
		originalAlignmentBase = b;
	}

	public static int currentAlignmentType() {
		return currentAlignmentType;
	}

	public static void setCurrentAlignmentType(int t) {
		currentAlignmentType = t;
	}

	public static int currentAlignmentRecord() {
		return currentAlignmentRecord;
	}

	public static void setCurrentAlignmentRecord(int r) {
		currentAlignmentRecord = r;
	}

	public static int currentAlignmentBase() {
		return currentAlignmentBase;
	}

	public static void setCurrentAlignmentBase(int b) {
		currentAlignmentBase = b;
	}

	public static int currentLuck() {
		return currentLuck;
	}

	public static void setCurrentLuck(int luck) {
		currentLuck = luck;
	}

	public static void increaseCurrentLuck(int luck) {
		currentLuck += luck;
	}

	public static void decreaseCurrentLuck(int luck) {
		currentLuck -= luck;
	}

	public static int luckBonus() {
		return luckBonus;
	}

	public static void setLuckBonus(int bonus) {
		luckBonus = bonus;
	}

	public static int currentLuckWithBonus() {
		return currentLuck() + luckBonus();
	}

	public static int minimumPossibleLuck() {
		return -10;
	}

	public static int maximumPossibleLuck() {
		return 10;
	}

	public static int blessings() {
		return blessings;
	}

	public static void setBlessings(int b) {
		blessings = b;
	}

	public static void increaseBlessings(int b) {
		blessings += b;
	}

	public static void decreaseBlessings(int b) {
		blessings -= b;
	}

	public static int timeToNextBlessing() {
		return timeToNextBlessing;
	}

	public static void setTimeToNextBlessing(int t) {
		timeToNextBlessing = t;
	}

	public static void increaseTimeToNextBlessing(int t) {
		timeToNextBlessing += t;
	}

	public static void decreaseTimeToNextBlessing(int t) {
		timeToNextBlessing -= t;
	}

	public static int divineWrath() {
		return divineWrath;
	}

	public static void setDivineWrath(int wrath) {
		divineWrath = wrath;
	}

	public static void increaseDivineWrath(int wrath) {
		divineWrath += wrath;
	}

	public static void decreaseDivineWrath(int wrath) {
		divineWrath -= wrath;
	}

	public static long cleansedSinceMove() {
		return cleansedSinceMove;
	}

	public static void setCleansedSinceMove(long t) {
		cleansedSinceMove = t;
	}
	
	public static boolean invulnerableWhilePraying() {
		return invulnerableWhilePraying;
	}

	public static void setInvulnerableWhilePraying(boolean i) {
		invulnerableWhilePraying = i;
	}

	public static int toHitModifier() {
		return toHitModifier;
	}

	public static void setToHitModifier(int mod) {
		toHitModifier = mod;
	}

	public static void increaseToHitModifier(int mod) {
		toHitModifier += mod;
	}

	public static void decreaseToHitModifier(int mod) {
		toHitModifier -= mod;
	}

	public static int damageBonus() {
		return damageBonus;
	}

	public static void setDamageBonus(int bonus) {
		damageBonus = bonus;
	}

	public static void increaseDamageBonus(int bonus) {
		damageBonus += bonus;
	}

	public static void decreaseDamageBonus(int bonus) {
		damageBonus -= bonus;
	}
	
	public static boolean usingTwoWeapons() {
		return usingTwoWeapons;
	}

	public static void setUsingTwoWeapons(boolean using) {
		usingTwoWeapons = using;
	}

	public static int armorClass() {
		return armorClass;
	}

	public static void setArmorClass(int ac) {
		armorClass = ac;
	}

	public static int armorBonusFromProtectionSpell() {
		return armorBonusFromProtectionSpell;
	}

	public static void setArmorBonusFromProtectionSpell(int sp) {
		armorBonusFromProtectionSpell = sp;
	}

	public static void increaseArmorBonusFromProtectionSpell(int sp) {
		armorBonusFromProtectionSpell += sp;
	}

	public static void decreaseArmorBonusFromProtectionSpell(int sp) {
		armorBonusFromProtectionSpell -= sp;
	}

	public static int protectionSpellPointDuration() {
		return protectionSpellPointDuration;
	}

	public static void setProtectionSpellPointDuration(int sp) {
		protectionSpellPointDuration = sp;
	}

	public static int protectionSpellPointCountdown() {
		return protectionSpellPointCountdown;
	}

	public static void setProtectionSpellPointCountdown(int sp) {
		protectionSpellPointCountdown = sp;
	}

	public static void increaseProtectionSpellPointCountdown(int sp) {
		protectionSpellPointCountdown += sp;
	}

	public static void decreaseProtectionSpellPointCountdown(int sp) {
		protectionSpellPointCountdown -= sp;
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
	
	public static final int originalMonsterNumber() {
		return originalMonsterNumber;
	}

	public static final int currentMonsterNumber() {
		return currentMonsterNumber;
	}
	
	public static final void setOriginalMonsterNumber(int n) {
		originalMonsterNumber = n;
	}

	public static final void setCurrentMonsterNumber(int n) {
		currentMonsterNumber = n;
	}
	
	public static final int timeRemainingAsMonster() {
		return timeRemainingAsMonster;
	}

	public static final void setTimeRemainingAsMonster(int t) {
		timeRemainingAsMonster = t;
	}

	public static final void increaseTimeRemainingAsMonster(int t) {
		timeRemainingAsMonster += t;
	}

	public static final void decreaseTimeRemainingAsMonster(int t) {
		timeRemainingAsMonster -= t;
	}

	public static final void multiplyTimeRemainingAsMonster(int t) {
		timeRemainingAsMonster *= t;
	}
	
	public static int lycanthropeType() {
		return lycanthropeType;
	}

	public static void setLycanthropeType(int t) {
		lycanthropeType = t;
	}
	
	public static boolean inherentlyFemale() {
		return inherentlyFemale;
	}
	
	public static void setInherentlyFemale(boolean f) {
		inherentlyFemale = f;
	}	
	
	public static boolean lurking() {
		return lurking;
	}
	
	public static void setLurking(boolean b) {
		lurking = b;
	}

	public static final int deathCount() {
		return deathCount;
	}

	public static final void setDeathCount(int c) {
	   deathCount = c;
	}

	public static final void increaseDeathCount(int c) {
		   deathCount += c;
	}

	public static final void decreaseDeathCount(int c) {
		   deathCount -= c;
	}

	public static final int ariseFromGraveAsMonster() {
		return ariseFromGraveAsMonster;
	}

	public static final void setAriseFromGraveAsMonster(int m) {
		ariseFromGraveAsMonster = m;
	}

	public static final int currentHitPoints() {
		return currentHitPoints;
	}
	
	public static final void setCurrentHitPoints(int hp) {
		currentHitPoints = hp;
	}

	public static final void increaseCurrentHitPoints(int hp) {
		currentHitPoints += hp;
	}

	public static final void decreaseCurrentHitPoints(int hp) {
		currentHitPoints -= hp;
	}

	public static final int maximumHitPoints() {
		return maximumHitPoints;
	}
	
	public static final void setMaximumHitPoints(int hp) {
		maximumHitPoints = hp;
	}

	public static final void increaseMaximumHitPoints(int hp) {
		maximumHitPoints += hp;
	}

	public static final void decreaseMaximumHitPoints(int hp) {
		maximumHitPoints -= hp;
	}

	public static final int currentHitPointsAsMonster() {
		return currentHitPointsAsMonster;
	}
	
	public static final void setCurrentHitPointsAsMonster(int m) {
		currentHitPointsAsMonster = m;
	}

	public static final void increaseCurrentHitPointsAsMonster(int m) {
		currentHitPointsAsMonster += m;
	}

	public static final void decreaseCurrentHitPointsAsMonster(int m) {
		currentHitPointsAsMonster -= m;
	}

	public static final int maximumHitPointsAsMonster() {
		return maximumHitPointsAsMonster;
	}

	public static final void setMaximumHitPointsAsMonster(int m) {
		maximumHitPointsAsMonster = m;
	}

	public static final void increaseMaximumHitPointsAsMonster(int m) {
		maximumHitPointsAsMonster += m;
	}

	public static final void decreaseMaximumHitPointsAsMonster(int m) {
		maximumHitPointsAsMonster -= m;
	}

	public static final void multiplyMaximumHitPointsAsMonster(int m) {
		maximumHitPointsAsMonster *= m;
	}

	public static final int currentMagicalEnergy() {
		return currentMagicalEnergy;
	}

	public static final void setCurrentMagicalEnergy(int m) {
		currentMagicalEnergy = m;
	}

	public static final void increaseCurrentMagicalEnergy(int m) {
		currentMagicalEnergy += m;
	}

	public static final void decreaseCurrentMagicalEnergy(int m) {
		currentMagicalEnergy -= m;
	}

	public static final int maximumMagicalEnergy() {
		return maximumMagicalEnergy;
	}

	public static final void setMaximumMagicalEnergy(int m) {
		maximumMagicalEnergy = m;
	}

	public static final void increaseMaximumMagicalEnergy(int m) {
		maximumMagicalEnergy += m;
	}

	public static final void decreaseMaximumMagicalEnergy(int m) {
		maximumMagicalEnergy -= m;
	}

	public static int magicalEnergyIncreasePerLevel(int level) {
		return magicalEnergyIncreasePerLevel[level];
	}

	public static void setMagicalEnergyIncreasePerLevel(int level, int en) {
		magicalEnergyIncreasePerLevel[level] = en;
	}

	public static final long remainingGallopTime() {
		return remainingGallopTime;
	}

	public static final void setRemainingGallopTime(long t) {
		remainingGallopTime = t;
	}

	public static final void increaseRemainingGallopTime(int t) {
		remainingGallopTime += t;
	}

	public static final void decreaseRemainingGallopTime(int t) {
		remainingGallopTime -= t;
	}

	public static final int turnsRiddenSinceLastSkillCheck() {
		return turnsRiddenSinceLastSkillCheck;
	}

	public static final void setTurnsRiddenSinceLastSkillCheck(int t) {
		turnsRiddenSinceLastSkillCheck = t;
	}

	public static final void increaseTurnsRiddenSinceLastSkillCheck(int t) {
		turnsRiddenSinceLastSkillCheck += t;
	}

	public static final void decreaseTurnsRiddenSinceLastSkillCheck(int t) {
		turnsRiddenSinceLastSkillCheck -= t;
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
	
	public static boolean canSenseUnsafeFood() {
		return canSenseUnsafeFood;
	}
	
	public static void setCanSenseUnsafeFood(boolean u) {
		canSenseUnsafeFood = u;
	}
	
	public static final int abilityToConfuseMonsters() {
		return abilityToConfuseMonsters;
	}

	public static final void setAbilityToConfuseMonsters(int a) {
		abilityToConfuseMonsters = a;
	}

	public static final int creamed() {
		return creamed;
	}

	public static final void setCreamed(int c) {
		creamed = c;
	}

	public static final void increaseCreamed(int c) {
		creamed += c;
	}

	public static final void decreaseCreamed(int c) {
		creamed -= c;
	}

	public static final int nightVisionRange() {
		return nightVisionRange;
	}

	public static final void setNightVisionRange(int r) {
		nightVisionRange = r;
	}

	public static final int xRayVisionRange() {
		return xRayVisionRange;
	}

	public static final void setXRayVisionRange(int r) {
		xRayVisionRange = r;
	}
	
	public static final boolean feltBall() {
		return feltBall;
	}

	public static final void setFeltBall(boolean f) {
		feltBall = f;
	}

	public static final boolean feltChain() {
		return feltChain;
	}

	public static final void setFeltChain(boolean f) {
		feltChain = f;
	}

	public static final int glyphUnderBall() {
		return glyphUnderBall;
	}

	public static final void setGlyphUnderBall(int g) {
		glyphUnderBall = g;
	}

	public static final int glyphUnderChain() {
		return glyphUnderChain;
	}

	public static final void setGlyphUnderChain(int g) {
		glyphUnderChain = g;
	}

	public static final int ballAndChainOrder() {
		return ballAndChainOrder;
	}

	public static final void setBallAndChainOrder(int bc) {
		ballAndChainOrder = bc;
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

	public static final int sickNonVomitable() {
		return SICK_NON_VOMITABLE;
	}
	
	public static final int sickVomitable() {
		return SICK_VOMITABLE;
	}
	
	public static final int sickAll() {
		return SICK_ALL;
	}
	
	public static final int giftsBestowed() {
		return giftsBestowed;
	}

	public static final void setGiftsBestowed(int g) {
		giftsBestowed = g;
	}

	public static final void increaseGiftsBestowed(int g) {
		giftsBestowed += g;
	}

	public static final void decreaseGiftsBestowed(int g) {
		giftsBestowed -= g;
	}

	public static final int startingMoney() {
		return startingMoney;
	}

	public static final void setStartingMoney(int m) {
		startingMoney = m;
	}

	public static final void increaseStartingMoney(int m) {
		startingMoney += m;
	}

	public static final void decreaseStartingMoney(int m) {
		startingMoney -= m;
	}
	
	public static final int numBonesFilesLoaded() {
		return numBonesFilesLoaded;
	}

	public static final void setNumBonesFilesLoaded(int b) {
		numBonesFilesLoaded = b;
	}

	public static final void increaseNumBonesFilesLoaded(int b) {
		numBonesFilesLoaded += b;
	}

	public static final void decreaseNumBonesFilesLoaded(int b) {
		numBonesFilesLoaded -= b;
	}
	
	public static final boolean achieved(int type) {
		Boolean result = achievements.get(type);
		return (null == result) ? false : result.booleanValue();
	}

	public static final void setAchieved(int type, boolean state) {
		achievements.put(type, state);
	}

	public static final int knownAchievementTypes() {
		return AchievementType.values().length;
	}

	public static final boolean haveSpecialItem(int type) {
		Boolean result = specialItems.get(type);
		return (null == result) ? false : result.booleanValue();
	}

	public static final void setHaveSpecialItem(int type, boolean state) {
		specialItems.put(type, state);
	}

	public static final int knownSpecialItems() {
		return SpecialItem.values().length;
	}
	
	public static final boolean permanentlyBlind() {
		return permanentlyBlind;
	}
	
	public static final void setPermanentlyBlind(boolean b) {
		permanentlyBlind = b;
	}

	public static final boolean nudist() {
		return nudist;
	}
	
	public static final void setNudist(boolean n) {
		nudist = n;
	}

	public static final boolean veganConduct() {
		return veganConduct;
	}
	
	public static final void setVeganConduct(boolean v) {
		veganConduct = v;
	}

	public static final boolean vegetarianConduct() {
		return vegetarianConduct;
	}
	
	public static final void setVegetarianConduct(boolean v) {
		vegetarianConduct = v;
	}

	public static final boolean foodlessConduct() {
		return foodlessConduct;
	}
	
	public static final void setFoodlessConduct(boolean f) {
		foodlessConduct = f;
	}

	public static final boolean atheistConduct() {
		return atheistConduct;
	}
	
	public static final void setAtheistConduct(boolean a) {
		atheistConduct = a;
	}

	public static final boolean hitlessConduct() {
		return (0L == weaponHitCount);
	}
	
    public static final long weaponHitCount() {
    	return weaponHitCount;
    }

    public static final void setWeaponHitCount(long n) {
    	weaponHitCount = n;
    }

    public static final void incrementWeaponHitCount(long n) {
    	weaponHitCount += n;
    }

	public static final boolean pacifistConduct() {
		return pacifistConduct;
	}
	
	public static final void setPacifistConduct(boolean p) {
		pacifistConduct = p;
	}

	public static final boolean illiterateConduct() {
		return (0L == literacyCount);
	}
	
	public static final long literacyCount() {
		return literacyCount;
	}

	public static final void setLiteracyCount(long n) {
		literacyCount = n;
	}

	public static final void incrementLiteracyCount(long n) {
		literacyCount += n;
	}

	public static final boolean unpolypilingConduct() {
		return (0L == polypileCount);
	}
	
	public static final long polypileCount() {
		return polypileCount;
	}

	public static final void setPolypileCount(long n) {
		polypileCount = n;
	}

	public static final void incrementPolypileCount(long n) {
		polypileCount += n;
	}

	public static final boolean unpolyselfingConduct() {
		return (0L == polyselfCount);
	}
	
	public static final long polyselfCount() {
		return polyselfCount;
	}

	public static final void setPolyselfCount(long n) {
		polyselfCount = n;
	}

	public static final void incrementPolyselfCount(long n) {
		polyselfCount += n;
	}

	public static final boolean wishlessConduct() {
		return (0L == wishCount);
	}
	
	public static final long wishCount() {
		return wishCount;
	}

	public static final void setWishCount(long n) {
		wishCount = n;
	}

	public static final void incrementWishCount(long n) {
		wishCount += n;
	}

	public static final boolean artifactWishlessConduct() {
		return (0L == artifactWishCount);
	}
	
	public static final long artifactWishCount() {
		return artifactWishCount;
	}

	public static final void setArtifactWishCount(long n) {
		artifactWishCount = n;
	}

	public static final void incrementArtifactWishCount(long n) {
		artifactWishCount += n;
	}
	
	public static final boolean gotMinorOracle() {
		return gotMinorOracle;
	}
	
	public static final void setGotMinorOracle(boolean g) {
		gotMinorOracle = g;
	}

	public static final boolean gotMajorOracle() {
		return gotMajorOracle;
	}
	
	public static final void setGotMajorOracle(boolean g) {
		gotMajorOracle = g;
	}

	public static final boolean hasReadTribute() {
		return hasReadTribute;
	}
	
	public static final void setHasReadTribute(boolean t) {
		hasReadTribute = t;
	}

	public static boolean calledByQuestLeader() {
		return calledByQuestLeader;
	}

	public static void setCalledByQuestLeader(boolean c) {
		calledByQuestLeader = c;
	}

	public static boolean expelledFromQuestDungeon() {
		return expelledFromQuestDungeon;
	}

	public static void setExpelledFromQuestDungeon(boolean e) {
		expelledFromQuestDungeon = e;
	}

	public static boolean completedQuest() {
		return completedQuest;
	};

	public static void setCompletedQuest(boolean c) {
		completedQuest = c;
	};
	
	public static int knowledgeOfPasstune() {
		return knowledgeOfPasstune;
	}

	public static void setKnowledgeOfPasstune(int k) {
		knowledgeOfPasstune = k;
	}

	public static void incrementKnowledgeOfPasstune(int k) {
		knowledgeOfPasstune += k;
	}

	public static boolean haveOpenedDrawbridge() {
		return haveOpenedDrawbridge;
	};

	public static void setHaveOpenedDrawbridge(boolean h) {
		haveOpenedDrawbridge = h;
	};
	
	public static boolean haveInvokedGateToSanctum() {
	    return haveInvokedGateToSanctum;	
	}

	public static void setHaveInvokedGateToSanctum(boolean h) {
	    haveInvokedGateToSanctum = h;	
	}

	public static boolean enteredGehennomViaValley() {
		return enteredGehennomViaValley;
	}

	public static void setEnteredGehennomViaValley(boolean e) {
		enteredGehennomViaValley = e;
	}

	public static int handOfElberethLevel() {
		return handOfElberethLevel;
	}

	public static void setHandOfElberethLevel(int h) {
		handOfElberethLevel = h;
	}

	public static boolean becameDemigod() {
		return becameDemigod;
	}

	public static void setBecameDemigod(boolean b) {
		becameDemigod = b;
	}

	public static boolean steppedOnVibratingSquare() {
		return steppedOnVibratingSquare;
	}

	public static void setSteppedOnVibratingSquare(boolean v) {
		steppedOnVibratingSquare = v;
	}

	public static boolean haveAscended() {
		return haveAscended;
	}

	public static void setHaveAscended(boolean h) {
		haveAscended = h;
	}

	public static int currentPantheon()
	{
		if (null == currentPantheon)
		{
			return -1;
		}
		return currentPantheon.ordinal();
	}

	public static void setCurrentPantheon(int id)
	{
		currentPantheon = Pantheon.values()[id];
	}
	
	public static String yourDeityName()
	{
		return nameOfAlignedDeityFromCurrentPantheon(currentAlignmentType);
	}
	
	public static String nameOfAlignedDeityFromCurrentPantheon(int alignment)
	{
		Deity alignedDeity = null;
		alignedDeity = currentPantheon.deityWith(alignment);
		return alignedDeity.deityName();
	}
	
	public static String titleOfAlignedDeityFromCurrentPantheon(int alignment)
	{
		Deity alignedDeity = null;
		if (currentPantheon != null) {
			alignedDeity = currentPantheon.deityWith(alignment);
		}
		return Deity.title(alignedDeity);
	}
	
	public static int currentRoleID()
	{
		return currentRoleID;
	}

	public static void setCurrentRoleID(int id)
	{
		currentRoleID = id;
	}
	
	public static int speciesID()
	{
		return speciesID;
	}

	public static void setSpeciesID(int id)
	{
		speciesID = id;
	}

	public static String speciesNoun()
	{
		return AdventurerSpecies.speciesNoun(speciesID);
	}

	public static String speciesAdjective()
	{
		return AdventurerSpecies.speciesAdjective(speciesID);
	}

	public static String speciesFileCode()
	{
		return AdventurerSpecies.speciesFileCode(speciesID);
	}
	
	public static boolean speciesIndividualNameHasMaleVersion()
	{
		return AdventurerSpecies.individualNameHasMaleVersion(speciesID);
	}
	
	public static boolean speciesIndividualNameHasFemaleVersion()
	{
		return AdventurerSpecies.individualNameHasFemaleVersion(speciesID);
	}
	
	public static String speciesIndividualNameAsMale()
	{
		return AdventurerSpecies.individualNameAsMale(speciesID);
	}
	
	public static String speciesIndividualNameAsFemale()
	{
		return AdventurerSpecies.individualNameAsFemale(speciesID);
	}
	
	public static boolean speciesIs(int pmid)
	{
		return (AdventurerSpecies.pmidAsMale(speciesID) == pmid);
	}

	public static boolean speciesPMIDHasFemaleVersion()
	{
		return AdventurerSpecies.pmidHasFemaleVersion(speciesID);
	}

	public static int speciesPMIDAsFemale()
	{
		return AdventurerSpecies.pmidAsFemale(speciesID);
	}

	public static boolean speciesPMIDHasMaleVersion()
	{
		return AdventurerSpecies.pmidHasMaleVersion(speciesID);
	}

	public static int speciesPMIDAsMummy()
	{
		return AdventurerSpecies.pmidAsMummy(speciesID);
	}

	public static boolean speciesPMIDHasMummyVersion()
	{
		return AdventurerSpecies.pmidHasMummyVersion(speciesID);
	}

	public static int speciesPMIDAsZombie()
	{
		return AdventurerSpecies.pmidAsZombie(speciesID);
	}

	public static boolean speciesPMIDHasZombieVersion()
	{
		return AdventurerSpecies.pmidHasZombieVersion(speciesID);
	}

	public static int speciesPMIDAsMale()
	{
		return AdventurerSpecies.pmidAsMale(speciesID);
	}

	public static String roleFileCode()
	{
		return AdventurerRole.fileCode(currentRoleID);
	}

	public static String questHomeBase()
	{
		return AdventurerRole.questHomeBase(currentRoleID);
	}
	
	public static String intermediateQuestLevel()
	{
		return AdventurerRole.intermediateQuestLevel(currentRoleID);
	}

	public static boolean roleCanStartAsMaleOrFemale()
	{
	    return AdventurerRole.canStartAsMaleOrFemale(currentRoleID);	
	}
	
	public static boolean hasMalePMIDForCurrentRole()
	{
		return (PM.UNKNOWN.id() != AdventurerRole.pmidAsMale(currentRoleID));
	}

	public static int malePMIDForCurrentRole()
	{
		return AdventurerRole.pmidAsMale(currentRoleID);
	}

	public static void setMalePMIDForCurrentRole(int pmid)
	{
		AdventurerRole.setPMIDAsMale(currentRoleID, pmid);
	}

	public static boolean hasFemalePMIDForCurrentRole()
	{
		return (PM.UNKNOWN.id() != AdventurerRole.pmidAsFemale(currentRoleID));
	}

	public static int femalePMIDForCurrentRole()
	{
		return AdventurerRole.pmidAsFemale(currentRoleID);
	}

	public static void setFemalePMIDForCurrentRole(int pmid)
	{
		AdventurerRole.setPMIDAsFemale(currentRoleID, pmid);
	}
	
	public static boolean hasPreferredPet()
	{
		return AdventurerRole.hasPreferredPet(currentRoleID);
	}

	public static int pmidOfPreferredPet()
	{
		return AdventurerRole.pmidOfPreferredPet(currentRoleID);
	}

	public static boolean hasQuestLeader()
	{
		return AdventurerRole.hasQuestLeader(currentRoleID);
	}

	public static int pmidOfQuestLeader()
	{
		return AdventurerRole.pmidOfQuestLeader(currentRoleID);
	}

	public static boolean hasQuestGuardians()
	{
		return AdventurerRole.hasQuestGuardians(currentRoleID);
	}

	public static int pmidOfQuestGuardians()
	{
		return AdventurerRole.pmidOfQuestGuardians(currentRoleID);
	}
	
	public static boolean hasQuestNemesis()
	{
		return AdventurerRole.hasQuestNemesis(currentRoleID);
	}

	public static int pmidOfQuestNemesis()
	{
		return AdventurerRole.pmidOfQuestNemesis(currentRoleID);
	}

	public static int pmidOfQuestEnemies()
	{
		return AdventurerRole.pmidOfQuestEnemies(currentRoleID);
	}

	public static int pmidOfOtherQuestEnemies()
	{
		return AdventurerRole.pmidOfOtherQuestEnemies(currentRoleID);
	}

	public static int classOfQuestEnemies()
	{
		return AdventurerRole.classOfQuestEnemies(currentRoleID);
	}

	public static int classOfOtherQuestEnemies()
	{
		return AdventurerRole.classOfOtherQuestEnemies(currentRoleID);
	}

	public static boolean hasQuestArtifact()
	{
		return AdventurerRole.hasQuestArtifact(currentRoleID);
	}
	
	public static int idOfQuestArtifact()
	{
		return AdventurerRole.idOfQuestArtifact(currentRoleID);
	}
	
	public static int attributeBase(int whichAttr)
	{
		return AdventurerRole.attributeBase(currentRoleID, whichAttr);
	}
	
	public static int attributeDistribution(int whichAttr)
	{
		return AdventurerRole.attributeDistribution(currentRoleID, whichAttr);
	}
	
	public static int cutoffLevel()
	{
		return AdventurerRole.cutoffLevel(currentRoleID);
	}
	
	public static int hitPointAdvancementForRole()
	{
		return AdventurerRole.hitPointAdvancement(currentRoleID, currentExperienceLevel);
	}

	public static int hitPointAdvancementForSpecies()
	{
		int cutoff = AdventurerRole.cutoffLevel(currentRoleID);
		return AdventurerSpecies.hitPointAdvancement(speciesID, currentExperienceLevel, cutoff);
	}

	public static long speciesSelfMask()
	{
		return AdventurerSpecies.selfMask(speciesID);
	}
	
	public static long speciesFriendshipMask()
	{
		return AdventurerSpecies.friendshipMask(speciesID);
	}
	
	public static long speciesHostilityMask()
	{
		return AdventurerSpecies.hostilityMask(speciesID);
	}
	
	public static int speciesAttributeMaximum(int whichAttr)
	{
		return AdventurerSpecies.maximum(speciesID, whichAttr);
	}

	public static int speciesAttributeMinimum(int whichAttr)
	{
		return AdventurerSpecies.minimum(speciesID, whichAttr);
	}

	public static int initialAlignmentRecordForRole()
	{
		return AdventurerRole.initialAlignmentRecord(currentRoleID);
	}

	public static int baseSpellcastingPenaltyForRole()
	{
		return AdventurerRole.baseSpellcastingPenalty(currentRoleID);
	}

	public static int healingSpellPenaltyForRole()
	{
		return AdventurerRole.healingSpellPenalty(currentRoleID);
	}

	public static int spellPenaltyWithShieldForRole()
	{
		return AdventurerRole.spellPenaltyWithShield(currentRoleID);
	}

	public static int spellPenaltyWithArmorForRole()
	{
		return AdventurerRole.spellPenaltyWithArmor(currentRoleID);
	}

	public static int spellcastingAttributeIDForRole()
	{
		return AdventurerRole.spellcastingAttributeID(currentRoleID);
	}

	public static int specialtySpellIDForRole()
	{
		return AdventurerRole.specialtySpellID(currentRoleID);
	}

	public static int specialtySpellPenaltyForRole()
	{
		return AdventurerRole.specialtySpellPenalty(currentRoleID);
	}

}
