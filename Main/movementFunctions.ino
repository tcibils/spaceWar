/*
 *  This files includes functions necessary for player movement
 * 
 * 
 */


// Moves the passed played in the passed direction, and make checks before doing it (existance of another spaceship or of world map so far)
void movePlayer(const byte playerToMoveIndex, const byte directionToMove) {

  bool blockedByAnotherPlayer = false;
  bool blockedByMapBorder = false;

  // Case 1 : player is facing up
  if (directionToMove == directionUp) {
    blockedByAnotherPlayer = checkIfPlayerAbove(playerToMoveIndex);
  
    // We also check if we're near the map
    if(playersArray[playerToMoveIndex].lineCoordinate == 0) {
      blockedByMapBorder = true;
    }

    // If we're not blocked
    if (!blockedByMapBorder && !blockedByAnotherPlayer) {
      // We move the player by one up
      playersArray[playerToMoveIndex].lineCoordinate--;
    }
  }


  // Case 2 : going right
  if (directionToMove == directionRight) {
    blockedByAnotherPlayer = checkIfPlayerRightTo(playerToMoveIndex);
    
    if (playersArray[playerToMoveIndex].columnCoordinate + shipSizes[playersArray[playerToMoveIndex].level] == displayNumberOfColumns) {
      blockedByMapBorder = true;
    }

    if(!blockedByMapBorder && !blockedByAnotherPlayer) {
      playersArray[playerToMoveIndex].columnCoordinate++;
    }
  }

  if (directionToMove == directionDown) {
    blockedByAnotherPlayer = checkIfPlayerBelow(playerToMoveIndex);
    if (playersArray[playerToMoveIndex].lineCoordinate + shipSizes[playersArray[playerToMoveIndex].level] == displayNumberOfRows) {
      blockedByMapBorder = true;
    }
    
    if(!blockedByMapBorder && !blockedByAnotherPlayer) {
      playersArray[playerToMoveIndex].lineCoordinate++;
    }
  }

  if (directionToMove == directionLeft) {
    blockedByAnotherPlayer = checkIfPlayerLeftTo(playerToMoveIndex);
    
    if (playersArray[playerToMoveIndex].columnCoordinate == 0) {
      blockedByMapBorder = true;
    }
    
    if(!blockedByMapBorder && !blockedByAnotherPlayer) {
      playersArray[playerToMoveIndex].columnCoordinate--;
    }
  }
}


bool checkIfPlayerAbove(const byte playerToMoveIndex) {
  bool result = false;
  
  // We need to check if another player is in the way. We check for all players
  for (byte playersIterationIndex = 0; playersIterationIndex < numberOfPlayers; playersIterationIndex++) {
    // We do not check for the player we're moving, of course
    if (playersIterationIndex != playerToMoveIndex) {
      // If the player's "just above" is the bottom of another player
      if (playersArray[playerToMoveIndex].lineCoordinate == playersArray[playersIterationIndex].lineCoordinate + shipSizes[playersArray[playersIterationIndex].level]) {
        // And if our most left point is at the left of the other player's most right point
        if(playersArray[playerToMoveIndex].columnCoordinate < playersArray[playersIterationIndex].columnCoordinate + shipSizes[playersArray[playersIterationIndex].level]) {
          // And if our most right point is at the right of the other player's most left point
          if(playersArray[playerToMoveIndex].columnCoordinate + shipSizes[playersArray[playerToMoveIndex].level] > playersArray[playersIterationIndex].columnCoordinate) {
            // THEN YES WE'RE FUCKING BLOCKED
            result = true;
          }
        }
      }
    }
  }

  return result;
}

bool checkIfPlayerBelow(const byte playerToMoveIndex) {
  bool result = false;
  
  // We need to check if another player is in the way. We check for all players
  for (byte playersIterationIndex = 0; playersIterationIndex < numberOfPlayers; playersIterationIndex++) {
    // We do not check for the player we're moving, of course
    if (playersIterationIndex != playerToMoveIndex) {
      // If the player's "just below" is the top of another player
      if (playersArray[playerToMoveIndex].lineCoordinate + shipSizes[playersArray[playerToMoveIndex].level] == playersArray[playersIterationIndex].lineCoordinate) {
        // And if our most left point is at the left of the other player's most right point
        if(playersArray[playerToMoveIndex].columnCoordinate < playersArray[playersIterationIndex].columnCoordinate + shipSizes[playersArray[playersIterationIndex].level]) {
          // And if our most right point is at the right of the other player's most left point
          if(playersArray[playerToMoveIndex].columnCoordinate + shipSizes[playersArray[playerToMoveIndex].level] > playersArray[playersIterationIndex].columnCoordinate) {
            // THEN YES WE'RE FUCKING BLOCKED
            result = true;
          }
        }        
      }
    }
  }

  return result;
}


bool checkIfPlayerLeftTo(const byte playerToMoveIndex) {
  bool result = false;
  
  // We need to check if another player is in the way. We check for all players
  for (byte playersIterationIndex = 0; playersIterationIndex < numberOfPlayers; playersIterationIndex++) {
    // We do not check for the player we're moving, of course
    if (playersIterationIndex != playerToMoveIndex) {
      // If the player's "just left" is the right of another player
      if (playersArray[playerToMoveIndex].columnCoordinate == playersArray[playersIterationIndex].columnCoordinate + shipSizes[playersArray[playersIterationIndex].level]) {
        // And if our most up point is upper of the other player's most low point
        if(playersArray[playerToMoveIndex].lineCoordinate < playersArray[playersIterationIndex].lineCoordinate + shipSizes[playersArray[playersIterationIndex].level]) {
          // And if our most down point is downer of the other player's most upper point
          if(playersArray[playerToMoveIndex].lineCoordinate + shipSizes[playersArray[playerToMoveIndex].level] > playersArray[playersIterationIndex].lineCoordinate) {
            // THEN YES WE'RE FUCKING BLOCKED
            result = true;
          }
        }
      }
    }
  }
  
  return result;
}


bool checkIfPlayerRightTo(const byte playerToMoveIndex) {
  bool result = false;
  
  // We need to check if another player is in the way. We check for all players
  for (byte playersIterationIndex = 0; playersIterationIndex < numberOfPlayers; playersIterationIndex++) {
    // We do not check for the player we're moving, of course
    if (playersIterationIndex != playerToMoveIndex) {
      // If the player's "just right" is the right of another player
      if (playersArray[playerToMoveIndex].columnCoordinate + shipSizes[playersArray[playerToMoveIndex].level] == playersArray[playersIterationIndex].columnCoordinate) {
        // And if our most up point is upper of the other player's most low point
        if(playersArray[playerToMoveIndex].lineCoordinate < playersArray[playersIterationIndex].lineCoordinate + shipSizes[playersArray[playersIterationIndex].level]) {
          // And if our most down point is downer of the other player's most upper point
          if(playersArray[playerToMoveIndex].lineCoordinate + shipSizes[playersArray[playerToMoveIndex].level] > playersArray[playersIterationIndex].lineCoordinate) {
            // THEN YES WE'RE FUCKING BLOCKED
            result = true;
          }
        }
      }      
    }
  }
  
  return result;
}
