package rec.games.roguelike.nh4j;

public enum AD {

    ANY(-1),   /* fake damage; attacktype_fordmg wildcard */
    PHYS(0),   /* ordinary physical */
    MAGM(1),   /* magic missiles */
    FIRE(2),   /* fire damage */
    COLD(3),   /* frost damage */
    SLEE(4),   /* sleep ray */
    DISN(5),   /* disintegration (death ray) */
    ELEC(6),   /* shock damage */
    DRST(7),   /* drains str (poison) */
    ACID(8),   /* acid damage */
    SPC1(9),   /* for extension of buzz() */
    SPC2(10),  /* for extension of buzz() */
    BLND(11),  /* blinds (yellow light) */
    STUN(12),  /* stuns */
    SLOW(13),  /* slows */
    PLYS(14),  /* paralyses */
    DRLI(15),  /* drains life levels (Vampire) */
    DREN(16),  /* drains magic energy */
    LEGS(17),  /* damages legs (xan) */
    STON(18),  /* petrifies (Medusa, cockatrice) */
    STCK(19),  /* sticks to you (mimic) */
    SGLD(20),  /* steals gold (leppie) */
    SITM(21),  /* steals item (nymphs) */
    SEDU(22),  /* seduces & steals multiple items */
    TLPT(23),  /* teleports you (Quantum Mech.) */
    RUST(24),  /* rusts armour (Rust Monster)*/
    CONF(25),  /* confuses (Umber Hulk) */
    DGST(26),  /* digests opponent (trapper, etc.) */
    HEAL(27),  /* heals opponent's wounds (nurse) */
    WRAP(28),  /* special "stick" for eels */
    WERE(29),  /* confers lycanthropy */
    DRDX(30),  /* drains dexterity (quasit) */
    DRCO(31),  /* drains constitution */
    DRIN(32),  /* drains intelligence (mind flayer) */
    DISE(33),  /* confers diseases */
    DCAY(34),  /* decays organics (brown Pudding) */
    SSEX(35),  /* Succubus seduction (extended) */
    HALU(36),  /* causes hallucination */
    DETH(37),  /* for Death only */
    PEST(38),  /* for Pestilence only */
    FAMN(39),  /* for Famine only */
    SLIM(40),  /* turns you into green slime */
    ENCH(41),  /* remove enchantment (disenchanter) */
    CORR(42),  /* corrode armor (black pudding) */
    
    CLRC(240), /* random clerical spell */
    SPEL(241), /* random magic spell */
    RBRE(242), /* random breath weapon */
    
    SAMU(252), /* hits, may steal Amulet (Wizard) */
    CURS(253); /* random curse (ex. gremlin) */

	int id;
	
	AD(int id)
	{
		this.id = id;
	}
	
	public int id()
	{
		return id;
	}
}
