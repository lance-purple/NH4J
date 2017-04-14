package rec.games.roguelike.nh4j;

public enum MC {

    ANT(1),
    BLOB(2),
    COCKATRICE(3),
    DOG(4),
    EYE(5),
    FELINE(6),
    GREMLIN(7),
    HUMANOID(8),
    IMP(9),
    JELLY(10),
    KOBOLD(11),
    LEPRECHAUN(12),
    MIMIC(13),
    NYMPH(14),
    ORC(15),
    PIERCER(16),
    QUADRUPED(17),
    RODENT(18),
    SPIDER(19),
    TRAPPER(20),
    UNICORN(21),
    VORTEX(22),
    WORM(23),
    XAN(24),
    LIGHT(25),
    ZRUTY(26),
    ANGEL(27),
    BAT(28),
    CENTAUR(29),
    DRAGON(30),
    ELEMENTAL(31),
    FUNGUS(32),
    GNOME(33),
    GIANT(34),
    invisible(35),      /* non-class */
    JABBERWOCK(36),
    KOP(37),
    LICH(38),
    MUMMY(39),
    NAGA(40),
    OGRE(41),
    PUDDING(42),
    QUANTMECH(43),
    RUSTMONST(44),
    SNAKE(45),
    TROLL(46),
    UMBER(47),
    VAMPIRE(48),
    WRAITH(49),
    XORN(50),
    YETI(51),
    ZOMBIE(52),
    HUMAN(53),
    GHOST(54),
    GOLEM(55),
    DEMON(56),
    EEL(57),
    LIZARD(58),    
    WORM_TAIL(59),
    MIMIC_DEF(60);
	
	private int id;
	
	MC(int i)
	{
		id = i;
	}
	
	public int id()
	{
		return id;
	}
    
}
