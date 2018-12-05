package rec.games.roguelike.nh4j;

public class Clock {

	static long elapsedPlayingTimeSeconds = 0L;

	
	static long elapsedPlayingTimeSeconds() {
		return elapsedPlayingTimeSeconds;
	}
	
	static void setElapsedPlayingTimeSeconds(long s) {
		elapsedPlayingTimeSeconds = s;
	}

}
