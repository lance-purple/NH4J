package rec.games.roguelike.nh4j;

public class Attack {

	private AT type;
	private AD damage;
	private int numberOfDice;
	private Dice dice;

	public Attack(AT type, AD damage, int numberOfDice, Dice dice) {
		this.type = type;
		this.damage = damage;
		this.numberOfDice = numberOfDice;
		this.dice = dice;
	}

}
