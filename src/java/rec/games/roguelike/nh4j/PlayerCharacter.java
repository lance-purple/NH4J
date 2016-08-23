package rec.games.roguelike.nh4j;

public class PlayerCharacter {
	
    private static NHAttributeSet currentAttributes = new NHAttributeSet();
    private static NHAttributeSet maximumAttributes = new NHAttributeSet();
    private static NHAttributeSet attributeBonuses = new NHAttributeSet();
    private static NHAttributeSet attributeChangesFromExertion = new NHAttributeSet();

    
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

}
