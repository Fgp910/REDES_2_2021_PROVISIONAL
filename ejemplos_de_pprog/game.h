/**
 * @brief It defines the game interface
 *
 * @file game.h
 * @author Leandro Garcia, Fabian Gutierrez, Junco de las Heras & Marta Vaquerizo
 * @version 3.0
 * @date 15-11-2019
 * @copyright GNU Public License
 */

#ifndef GAME_H
#define GAME_H

#include "command.h"
#include "space.h"
#include "link.h"
#include "player.h"
#include "object.h"
#include "inventory.h"
#include "die.h"

typedef struct _Game Game;

/**
 * @brief Creates new game
 *
 * game_create Allocates memory for a new Game element
 *
 * @author Fabian Gutierrez
 * @date 18-10-2019
 *
 * @return new Game element address, or NULL in case of failure
 */
Game* game_create();

/**
 * @brief Updates game according to a command
 *
 * game_update Executes an input command and updates the game accordingly
 *
 * @author Profesores PPROG
 * @date 13-01-2015
 *
 * @param game the game to update
 * @param cmd the command to execute
 * @return OK in case of success, ERROR otherwise
 */
STATUS game_update(Game* game, T_Command cmd);

/**
 * @brief Frees memory associated with a Game
 *
 * game_destroy Frees the memory occupied by a Game element
 *
 * @author Fabian Gutierrez
 * @date 18-10-2019
 *
 * @param game the game to be destroyed
 * @return OK in case of success, ERROR otherwise
 */
STATUS game_destroy(Game* game);

/**
 * @brief Evaluates whether a game is over or not
 *
 * game_is_over Evaluates whether a game is over or not
 *
 * @author Profesores PPROG
 * @date 13-01-2015
 *
 * @param game the game to be evaluated
 * @return TRUE if game is over, FALSE otherwise
 */
BOOL game_is_over(Game* game);

/**
 * @brief Prints game information
 *
 * game_print_data Prints data of the fields of a given game
 *
 * @author Fabian Gutierrez
 * @date 18-10-2019
 *
 * @param game the game to be printed
 */
void game_print_data(Game* game);

/**
 * @brief Retrieves a space of a game from id
 *
 * game_get_space Looks for a certain space of a game,
 * using its id as key
 *
 * @author Fabian Gutierrez
 * @date 02-10-2019
 *
 * @param game the game to be evaluated
 * @param id the id to look for
 * @return the address of the space found, NULL otherwise
 */
Space* game_get_space(Game* game, Id id);

/**
 * @brief Retrieves a link of a game from id
 *
 * game_get_link Looks for a certain link of a game,
 * using its id as key
 *
 * @author Fabian Gutierrez
 * @date 06-11-2019
 *
 * @param game the game to be evaluated
 * @param id the id to look for
 * @return the address of the link found, NULL otherwise
 */
Link* game_get_link(Game* game, Id id);

/**
 * @brief Returns the game player's address
 *
 * game_get_player Returns the address of the Player associated
 * with game
 *
 * @author Fabian Gutierrez
 * @date 02-10-2019
 *
 * @param game the game to be evaluated
 * @return the address of the Player element, NULL otherwise
 */
Player* game_get_player(Game* game);

/**
 * @brief Retrieves an object of a game from id
 *
 * game_get_object Looks for a certain object of a game,
 * using its id as key
 *
 * @author Fabian Gutierrez
 * @date 18-10-2019
 *
 * @param game the game to be evaluated
 * @param id the id to look for
 * @return the address of the object found, NULL otherwise
 */
Object* game_get_object(Game* game, Id id);

/**
 * @brief Retrieves every object from game
 *
 * game_get_objects Returns an array with all the objects of the game
 *
 * @author Fabian Gutierrez
 * @date 25-10-2019
 *
 * @param game the game to be evaluated
 * @return the address of the object array, NULL otherwise
 */
Object** game_get_objects(Game* game);

/**
 * @brief Retrieves every link from game
 *
 * game_get_objects Returns an array with all the objects of the game
 *
 * @author Marta Vaquerizo
 * @date 06-12-2019
 *
 * @param game the game to be evaluated
 * @return the address of the link array, NULL otherwise
 */
Link** game_get_links(Game* game);

/**
 * @brief Retrieves every space from game
 *
 * game_get_objects Returns an array with all the spaces of the game
 *
 * @author Marta Vaquerizo
 * @date 06-12-2019
 *
 * @param game the game to be evaluated
 * @return the address of the space array, NULL otherwise
 */
Space** game_get_spaces(Game* game);

/**
 * @brief Returns the game die's last roll
 *
 * game_get_object Returns the result of the last roll of the die
 *
 * @author Fabian Gutierrez
 * @date 17-10-2019
 *
 * @param game the game to be evaluated
 * @return the value of the last roll, -1 in case of error
 */
int game_get_last_roll(Game* game);

/**
 * @brief Returns the last string input
 *
 * game_get_last_str Returns the last string input by the user
 *
 * @author Junco de las Heras Valenzuela
 * @date 15-11-2019
 *
 * @param game the game to be evaluated
 * @return the last command string
 */
const char* game_get_last_str(Game* game);

/**
 * @brief Returns the last command input
 *
 * game_get_last_command Returns the last command input by the user
 *
 * @author Profesores PPROG
 * @date 13-01-2015
 *
 * @param game the game to be evaluated
 * @return the last command input
 */
T_Command game_get_last_command(Game* game);

/**
 * @brief Returns the last description requested
 *
 * game_get_last_description Returns the last description requested by the user
 *
 * @author Fabian Gutierrez
 * @date 15-11-2019
 *
 * @param game the game to be evaluated
 * @return the last description requested
 */
const char* game_get_last_description(Game* game);

/**
 * @brief Returns the last full description requested
 *
 * game_get_last_full_desc Returns the last full description requested by the user
 *
 * @author Leandro Garcia
 * @date 15-11-2019
 *
 * @param game the game to be evaluated
 * @return the last full description requested
 */
const char* game_get_last_full_desc(Game* game);

/**
 * @brief Returns the current number of spaces
 *
 * game_get_num_spaces Returns the the current number of spaces
 * the game manages
 *
 * @author Marta Vaquerizo
 * @date 15-11-2019
 *
 * @param game the game to be evaluated
 * @return the number of spaces
 */
int game_get_num_spaces(Game* game);

/**
 * @brief Returns the current number of links
 *
 * game_get_num_links Returns the the current number of links
 * the game manages
 *
 * @author Marta Vaquerizo
 * @date 15-11-2019
 *
 * @param game the game to be evaluated
 * @return the number of links
 */
int game_get_num_links(Game* game);

/**
 * @brief Returns the current number of objects
 *
 * game_get_num_objects Returns the the current number of objects
 * the game manages
 *
 * @author Marta Vaquerizo
 * @date 15-11-2019
 *
 * @param game the game to be evaluated
 * @return the number of objects
 */
int game_get_num_objects(Game* game);

/**
 * @brief Adds space to the game
 *
 * game_add_space Appends a space to the game structure
 *
 * @author Fabian Gutierrez
 * @date 24-09-2019
 *
 * @param game the game that will receive the space
 * @param space the space to be added
 * @return OK if successful, ERROR otherwise
 */
STATUS game_add_space(Game* game, Space* space);

/**
 * @brief Adds link to the game
 *
 * game_add_link Appends a link to the game structure
 *
 * @author Fabian Gutierrez
 * @date 06-11-2019
 *
 * @param game the game that will receive the link
 * @param link the link to be added
 * @return OK if successful, ERROR otherwise
 */
STATUS game_add_link(Game* game, Link* link);

/**
 * @brief Adds object to the game
 *
 * game_add_object Appends an object to the game structure
 *
 * @author Fabian Gutierrez
 * @date 18-10-2019
 *
 * @param game the game that will receive the object
 * @param object the object to be added
 * @return OK if successful, ERROR otherwise
 */
STATUS game_add_object(Game* game, Object* object);

/**
 * @brief Sets the player of the game
 *
 * game_set_player Assigns a new Player element to the game
 *
 * @author Fabian Gutierrez
 * @date 02-10-2019
 *
 * @param game the game to have its player set
 * @param player the player to be set
 * @return OK if successful, ERROR otherwise
 */
STATUS game_set_player(Game* game, Player* player);

/**
 * @brief Sets the die of the game
 *
 * game_set_die Assigns a new Die element to the game
 *
 * @author Fabian Gutierrez
 * @date 17-10-2019
 *
 * @param game the game to have its die set
 * @param die the die to be set
 * @return OK if successful, ERROR otherwise
 */
STATUS game_set_die(Game* game, Die* die);

/**
* @brief Returns the object of a given id managed by a game
*
* game_find_object Looks for the object by its id in the game
* spaces and retreives the id of its location
*
* @author Fabian Gutierrez
* @date 24-10-2019
*
* @param game the game to be evaluated
* @param id the object's id
* @return the id of the object's location
*/
Id game_find_object (Game* game, Id id);

/**
* @brief Returns if the space is lit
*
* game_space_is_lit Returns a boolean that indicates if there is something illuminating the space
*
* @author Leandro Garcia 
* @date 12-12-2019
*
* @param game the game to be evaluated
* @param space the space to be evaluated
* @return TRUE if the space is lit, FALSE if it isn't or in case of failure
*/
BOOL game_space_is_lit(Game* game, Space* space);

/**
 * @brief Returns if the object is hidden
 *
 * game_object_is_hidden Returns a boolean that indicates if an object is hidden
 *
 * @author Leandro Garcia
 * @date 12-12-2019
 *
 * @param game the game to be evaluated
 * @param obj the object to be evaluated
 * @return TRUE if the object is hidden, FALSE if it isn't or in case of failure
 */
BOOL game_object_is_hidden(Game* game, Object* obj);

/**
 * @brief Changes the game's last change
 *
 * game_set_last_change Changes the last_change field from a given game
 *
 * @author Leandro Garcia
 * @date 14-12-2019
 *
 * @param game the game to be updated
 * @param ch the new last change
 * @return OK in case of success, ERROR otherwise
 */
STATUS game_set_last_change(Game* game, char* ch);

/**
 * @brief Returns the game's last change
 *
 * game_get_last_change Returns the last_change field from a given game
 *
 * @author Leandro Garcia
 * @date 14-12-2019
 *
 * @param game the game to be evaluated
 * @return the game's last change, NULL in case of failure
 */
const char* game_get_last_change(Game* game);
#endif
