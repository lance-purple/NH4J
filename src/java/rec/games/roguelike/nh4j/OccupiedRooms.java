package rec.games.roguelike.nh4j;

public class OccupiedRooms {

	private static final int MAXIMUM_COUNT = 5;
	
	private static int previouslyOccupiedRoomIDs[] = new int[MAXIMUM_COUNT];

	public static int maximumCount() {
		return MAXIMUM_COUNT;
	}
	
	public static int previouslyOccupiedRoomIDs(int index) {
		return previouslyOccupiedRoomIDs[index];
	}

	public static void setPreviouslyOccupiedRoomIDs(int index, int roomID) {
		previouslyOccupiedRoomIDs[index] = roomID;
	}

}
