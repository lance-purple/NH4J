package rec.games.roguelike.nh4j;

public class SpellcastingSpecialty {
	
	private Spell spell;
	private int penalty;
	
	private SpellcastingSpecialty(Spell spell, int penalty)
	{
		this.spell = spell;
		this.penalty = penalty;
	}

	public static SpellcastingSpecialty of(Spell spell, int penalty)
	{
		return new SpellcastingSpecialty(spell, penalty);
	}
	
	public Spell spell()
	{
		return this.spell;
	}
	
	public int penalty()
	{
		return this.penalty;
	}
}
