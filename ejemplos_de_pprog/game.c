/**
 * @brief It implements the game interface and all the associated callbacks
 * for each command
 *
 * @file game.c
 * @author Leandro Garcia, Fabian Gutierrez, Junco de las Heras & Marta Vaquerizo
 * @version 3.0
 * @date 22-11-2019
 * @copyright GNU Public License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "game_management.h"
#include "graphic_engine.h"
#include "command.h"
#include "game_rules.h"

#define N_CALLBACK 18
#define N_CALLBACK_L_S 2
#define OBJECT_ID_INPUT 4
#define MOVE_INPUT 15
#define MAX_DIE 6
#define MIN_DIE 1

Id global_next_die_id = 1; /*stores next available die id*/

/**
 * @brief The Game structure
 *
 * It stores the different elements the game manages
 */
struct _Game {
    Player* player; /*!< Pointer to the player*/
    Object* objects[MAX_OBJECTS]; /*!< Pointer array of the multiple game objects*/
    Space* spaces[MAX_SPACES]; /*!< Pointer array of the multiple game spaces*/
    Link* links[MAX_LINKS]; /*!< Pointer array of the multiple game links*/
    Die* die; /*!< Pointer to the die*/
    T_Command last_cmd; /*!< Last command input by the user*/
    char last_str[WORD_SIZE]; /*!< Last string input by the user*/
    char description[WORD_SIZE]; /*!< Last description inspected*/
    char full_desc[WORD_SIZE]; /*!< Last full description inspected*/
    char last_change[WORD_SIZE]; /*!< Last change made by game rules*/

    int n_objects; /*!< Number of objects at any given time*/
    int n_spaces; /*!< Number of spaces at any given time*/
    int n_links; /*!< Number of links at any given time*/
};

/**
Define the function type for the callbacks, movements
 */
typedef STATUS(*callback_fn)(Game* game);

/**
List of callbacks for each command in the game
 */
STATUS game_callback_unknown(Game* game);
STATUS game_callback_exit(Game* game);
STATUS game_callback_next(Game* game);
STATUS game_callback_back(Game* game);
STATUS game_callback_take(Game* game);
STATUS game_callback_drop(Game* game);
STATUS game_callback_roll(Game* game);
STATUS game_callback_left(Game* game);
STATUS game_callback_right(Game* game);
STATUS game_callback_up(Game* game);
STATUS game_callback_down(Game* game);
STATUS game_callback_move(Game* game);
STATUS game_callback_inspect(Game* game);
STATUS game_callback_turnon(Game* game);
STATUS game_callback_turnoff(Game* game);
STATUS game_callback_load(Game* game);
STATUS game_callback_save(Game* game);
STATUS game_callback_open(Game* game);

static callback_fn game_callback_fn_list[N_CALLBACK] = {
    game_callback_unknown,
    game_callback_exit,
    game_callback_next,
    game_callback_back,
    game_callback_take,
    game_callback_drop,
    game_callback_roll,
    game_callback_left,
    game_callback_right,
    game_callback_up,
    game_callback_down,
    game_callback_move,
    game_callback_inspect,
    game_callback_turnon,
    game_callback_turnoff,
    game_callback_load,
    game_callback_save,
    game_callback_open
};

/**
Private functions
 */
int game_get_space_index_at(Game* game, Id space_id);
STATUS game_delete(Game* game);
BOOL game_has_illuminating_object(Game* game, Id* array, int num);
Id game_id_object(Game* game, char* name);
int game_index_object(Game* game, Id id);
int game_index_object_name(Game* game, char* name);

/**
Game interface implementation
 */
Game* game_create() {
    Game *game;
    int i;

    game = (Game*) malloc(sizeof (Game));
    if (game == NULL) {
        return NULL;
    }

    for (i = 0; i < MAX_OBJECTS; i++) {
        game->objects[i] = NULL;
    }

    for (i = 0; i < MAX_SPACES; i++) {
        game->spaces[i] = NULL;
    }

    for (i = 0; i < MAX_LINKS; i++) {
        game->links[i] = NULL;
    }

    game->player = NULL;
    game->die = die_create(global_next_die_id++, MIN_DIE, MAX_DIE);
    if (game->die == NULL) {
        free(game);
        return NULL;
    }
    game->last_cmd = NO_CMD;
    game->n_objects = 0;
    game->n_spaces = 0;
    game->n_links = 0;
    game->last_str[0] = '\0';
    game->description[0] = '\0';
    game->last_change[0] = '\0';

    return game;
}

STATUS game_update(Game* game, T_Command cmd) {
    if (game == NULL) {
        return ERROR;
    }
    game->last_str[0] = '\0'; /*Removes previous last_str. It will be overwritten if needed by cmd*/
    game->last_cmd = cmd;
    return (*game_callback_fn_list[cmd])(game);
}

STATUS game_destroy(Game* game) {
    int i = 0;

    if (game == NULL) {
        return ERROR;
    }

    player_destroy(game->player);
    game->player = NULL;
    die_destroy(game->die);
    game->die = NULL;

    for (i = 0; (i < MAX_SPACES) && (game->spaces[i] != NULL); i++) {
        space_destroy(game->spaces[i]);
        game->spaces[i] = NULL;
    }

    for (i = 0; (i < MAX_LINKS) && (game->links[i] != NULL); i++) {
        link_destroy(game->links[i]);
        game->links[i] = NULL;
    }

    for (i = 0; (i < MAX_OBJECTS) && (game->objects[i] != NULL); i++) {
        object_destroy(game->objects[i]);
        game->objects[i] = NULL;
    }

    free(game);
    game = NULL;
    return OK;
}

int game_get_num_spaces(Game* game) {
    if (game == NULL)
        return -1;

    return game->n_spaces;
}

int game_get_num_links(Game* game) {
    if (game == NULL)
        return -1;

    return game->n_links;
}

int game_get_num_objects(Game* game) {
    if (game == NULL)
        return -1;

    return game->n_objects;
}

Space* game_get_space(Game* game, Id id) {
    int i = 0;

    if (id != NO_ID) {
        for (i = 0; i < game->n_spaces; i++) {
            if (id == space_get_id(game->spaces[i])) {
                return game->spaces[i];
            }
        }
    }
    return NULL;
}

Link* game_get_link(Game* game, Id id) {
    int i = 0;

    if (id != NO_ID) {
        for (i = 0; i < game->n_links; i++) {
            if (id == link_get_id(game->links[i])) {
                return game->links[i];
            }
        }
    }
    return NULL;
}

Player* game_get_player(Game* game) {
    if (game == NULL) {
        return NULL;
    }
    return game->player;
}

Object* game_get_object(Game* game, Id id) {
    int i = 0;

    if (game == NULL || id == NO_ID) {
        return NULL;
    }

    for (i = 0; i < game->n_objects; i++) {
        if (id == object_get_id(game->objects[i])) {
            return game->objects[i];
        }
    }

    return NULL;
}

Object** game_get_objects(Game* game) {
    if (game == NULL) {
        return NULL;
    }

    return game->objects;
}

Link** game_get_links(Game* game) {
    if (game == NULL) {
        return NULL;
    }

    return game->links;
}

Space** game_get_spaces(Game* game) {
    if (game == NULL) {
        return NULL;
    }

    return game->spaces;
}

int game_get_last_roll(Game* game) {
    if (game == NULL) {
        return -1;
    }

    return die_get_last_roll(game->die);
}

const char* game_get_last_str(Game* game) {
    if (game == NULL) {
        return NULL;
    }
    return game->last_str;
}

T_Command game_get_last_command(Game* game) {
    if (game == NULL) {
        return NO_CMD;
    }
    return game->last_cmd;
}

const char* game_get_last_description(Game* game) {
    if (game == NULL) {
        return NULL;
    }
    return game->description;
}

const char* game_get_last_full_desc(Game* game) {
    if (game == NULL) {
        return NULL;
    }
    return game->full_desc;
}

STATUS game_set_last_change(Game* game, char* ch) {
    if (game == NULL || ch == NULL) {
        return ERROR;
    }

    if (strncpy(game->last_change, ch, WORD_SIZE) == NULL) {
        return ERROR;
    }
    return OK;
}

const char* game_get_last_change(Game* game) {
    if (game == NULL) {
        return NULL;
    }

    return game->last_change;
}

void game_print_data(Game* game) {
    int i = 0;

    if (game == NULL) {
        return;
    }

    printf("\n\n-------------\n\n");

    printf("=> Spaces: \n");
    for (i = 0; i < game->n_spaces; i++)
        space_print(game->spaces[i]);

    printf("=> Links: \n");
    for (i = 0; i < game->n_links; i++)
        link_print(game->links[i]);

    printf("=> Objects: \n");
    for (i = 0; i < game->n_objects; i++)
        object_print(game->objects[i]);

    printf("=> Player: \n");
    player_print(game->player);
    printf("=> Die: \n");
    die_print(game->die);
    printf("prompt:> ");
}

BOOL game_is_over(Game* game) {
    return FALSE;
}

STATUS game_add_space(Game* game, Space* space) {
    if (game == NULL || space == NULL || game->n_spaces == MAX_SPACES) {
        return ERROR;
    }

    game->spaces[game->n_spaces++] = space;


    return OK;
}

STATUS game_add_link(Game* game, Link* link) {
    if (game == NULL || link == NULL || game->n_links == MAX_LINKS) {
        return ERROR;
    }

    game->links[game->n_links++] = link;

    return OK;
}

STATUS game_add_object(Game* game, Object* object) {
    if (game == NULL || object == NULL || game->n_objects == MAX_OBJECTS) {
        return ERROR;
    }

    game->objects[game->n_objects++] = object;

    return OK;
}

STATUS game_set_player(Game* game, Player* player) {
    int space_index = -1;

    if (game == NULL || player == NULL) {
        return ERROR;
    }
    player_destroy(game->player); /*Frees potential previous player*/
    game->player = player;

    if ((space_index = game_get_space_index_at(game, player_get_space(player))) >= 0) { /*Sets, if possible, new game description*/
        strncpy(game->description, space_get_description(game->spaces[space_index]), WORD_SIZE - 1);
    }

    return OK;
}

STATUS game_set_die(Game* game, Die* die) {
    if (game == NULL || die == NULL) {
        return ERROR;
    }
    die_destroy(game->die); /*Frees potential previous die*/
    game->die = die;
    return OK;
}

Id game_find_object(Game* game, Id id) {
    int i;

    if (game != NULL) {
        for (i = 0; i < game->n_spaces; i++) {
            if (space_has_object(game->spaces[i], id) == TRUE) {
                return space_get_id(game->spaces[i]);
            }
        }
    }
    return NO_ID;
}

BOOL game_space_is_lit(Game* game, Space* space) {
    Id *array;

    if (game == NULL || space == NULL) {
        return FALSE;
    }

    if (space_is_lit(space)) {
        return TRUE;
    }

    /*Checking if the player illuminates the space*/
    if (player_get_space(game->player) == space_get_id(space)) {
        array = player_get_objects(game->player);
        if (array == NULL) {
            return FALSE;
        }

        if (game_has_illuminating_object(game, array, player_get_num_max_objects(game->player))) {
            return TRUE;
        }
    }

    /*Checking if there is an object illuminating the space*/
    array = space_get_objects(space);
    if (array == NULL) {
        return FALSE;
    }

    if (game_has_illuminating_object(game, array, space_get_nelems(space))) {
        return TRUE;
    }

    return FALSE;
}

BOOL game_object_is_hidden(Game* game, Object* obj) {
    if (game == NULL || obj == NULL) {
        return FALSE;
    }

    if (object_get_hidden(obj) == FALSE) {
        return FALSE;
    }

    if (player_has_object(game->player, SHOW_OBJECT)) {
        return FALSE;
    }

    return TRUE;
}

/**
Callbacks implementation for each action
 */
STATUS game_callback_unknown(Game* game) {
    return ERROR;
}

STATUS game_callback_exit(Game* game) {
    if (game == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS game_callback_next(Game* game) {
    int i = 0;
    Id current_id = NO_ID;
    Id next_id = NO_ID;
    Id link_id = NO_ID;
    Id player_space_id = NO_ID;
    Link* link = NULL;

    if (game == NULL) {
        return ERROR;
    }

    player_space_id = player_get_space(game->player);
    if (player_space_id == NO_ID)
        return ERROR;

    for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
        current_id = space_get_id(game->spaces[i]);
        if (current_id == NO_ID) {
            return ERROR;
        }
        if (current_id == player_space_id) {
            link_id = space_get_south(game->spaces[i]);
            if (link_id == NO_ID || (link = game_get_link(game, link_id)) == NULL || !link_is_open(link)) {
                return ERROR;
            }
            /*Gets the other side of the link*/
            if ((next_id = link_get_space1(link)) == current_id) {
                next_id = link_get_space2(link);
            }
            if (next_id == NO_ID) {
                return ERROR;
            }

            if (player_set_space(game->player, next_id) == ERROR) {
                return ERROR;
            }

            if (game_space_is_lit(game, game_get_space(game, next_id)) == TRUE) {
                strncpy(game->description, space_get_description(game_get_space(game, next_id)), WORD_SIZE - 1);
            } else {
                game->description[0] = '\0';
            }
            return OK;
        }
    }
    return ERROR;
}

STATUS game_callback_back(Game* game) {
    int i = 0;
    Id current_id = NO_ID;
    Id next_id = NO_ID;
    Id link_id = NO_ID;
    Id player_space_id = NO_ID;
    Link* link = NULL;

    if (game == NULL) {
        return ERROR;
    }

    player_space_id = player_get_space(game->player);
    if (player_space_id == NO_ID)
        return ERROR;

    for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
        current_id = space_get_id(game->spaces[i]);
        if (current_id == NO_ID) {
            return ERROR;
        }
        if (current_id == player_space_id) {
            link_id = space_get_north(game->spaces[i]);
            if (link_id == NO_ID || (link = game_get_link(game, link_id)) == NULL || !link_is_open(link)) {
                return ERROR;
            }
            /*Gets the other side of the link*/
            if ((next_id = link_get_space1(link)) == current_id) {
                next_id = link_get_space2(link);
            }
            if (next_id == NO_ID) {
                return ERROR;
            }

            if (player_set_space(game->player, next_id) == ERROR) {
                return ERROR;
            }
            if (game_space_is_lit(game, game_get_space(game, next_id)) == TRUE) {
                strncpy(game->description, space_get_description(game_get_space(game, next_id)), WORD_SIZE - 1);
            } else {
                game->description[0] = '\0';
            }
            return OK;
        }
    }
    return ERROR;
}

STATUS game_callback_take(Game* game) {
    int space_index, obj_index;
    char s[200];
    Id player_id, id_take = NO_ID;

    if (game == NULL)
        return ERROR;

    printf("Object to take: ");
    if (scanf("%s", s) < 0) {
        return ERROR;
    }
    strcpy(game->last_str, s);
    if (!game->last_str)
        return ERROR;

    id_take = game_id_object(game, s);

    if (id_take == NO_ID) {
        return ERROR;
    }


    /*get the position of the player*/
    player_id = player_get_space(game->player);
    if (player_id == NO_ID) {
        return ERROR;
    }

    /*get the index of the space in which the player is*/
    space_index = game_get_space_index_at(game, player_id);
    if (space_index == -1) {
        return ERROR;
    }

    /*get the index of the object to take*/
    obj_index = game_index_object_name(game, s);
    if (obj_index == -1) {
        return ERROR;
    }

    if (object_is_movable(game->objects[obj_index]) == FALSE) {
        return ERROR;
    }
    /*if the object not exist there will return ERROR*/
    if (space_del_object(game->spaces[space_index], id_take) == ERROR) {
        return ERROR;
    }

    if (player_insert_object(game->player, id_take) == ERROR) {
        space_add_object(game->spaces[space_index], id_take);
        return ERROR;
    }

    if (object_set_moved(game->objects[obj_index], TRUE) == ERROR) {
        player_delete_object(game->player, id_take);
        space_add_object(game->spaces[space_index], id_take);
        return ERROR;
    }

    if (object_set_hidden(game->objects[obj_index], FALSE) == ERROR) {
        player_delete_object(game->player, id_take);
        space_add_object(game->spaces[space_index], id_take);
        return ERROR;
    }

    return OK;
}

STATUS game_callback_drop(Game *game) {
    Id player_id, id_drop = NO_ID;
    int space_index;
    char s[WORD_SIZE];

    if (game == NULL) {
        return ERROR;
    }

    printf("Object to drop: ");
    if (scanf("%s", s) < 0) {
        return ERROR;
    }
    strcpy(game->last_str, s);
    if (!game->last_str)
        return ERROR;

    id_drop = game_id_object(game, s);
    if (id_drop == NO_ID) {
        return ERROR;
    }

    /*Check if the player has the object*/
    if (player_delete_object(game->player, id_drop) == ERROR) {
        return ERROR;
    }


    /*get where is the player*/
    player_id = player_get_space(game->player);
    if (player_id == NO_ID) {
        return ERROR;
    }

    /*get the index in the array*/
    space_index = game_get_space_index_at(game, player_id);
    if (space_index == -1) {
        return ERROR;
    }

    if (space_add_object(game->spaces[space_index], id_drop) == ERROR) {
        return ERROR;
    }

    return OK;
}

STATUS game_callback_turnon(Game* game) {
    int i;
    char s[WORD_SIZE];

    if (!game)
        return ERROR;


    printf("Object to turnon: ");
    if (scanf("%s", s) < 0) {
        return ERROR;
    }
    strcpy(game->last_str, s);
    if (!game->last_str)
        return ERROR;

    i = game_index_object_name(game, s);
    if (i < 0)
        return ERROR;

    return object_set_turnedon(game->objects[i], TRUE);
}

STATUS game_callback_turnoff(Game* game) {
    int i;
    char s[WORD_SIZE];

    if (!game)
        return ERROR;


    printf("Object to turnoff: ");
    if (scanf("%s", s) < 0) {
        return ERROR;
    }
    strcpy(game->last_str, s);
    if (!game->last_str)
        return ERROR;

    i = game_index_object_name(game, s);
    if (i < 0)
        return ERROR;

    return object_set_turnedon(game->objects[i], TRUE);
}

STATUS game_callback_roll(Game* game) {
    if (game == NULL) {
        return ERROR;
    }

    return die_roll(game->die);
}

STATUS game_callback_left(Game* game) {
    int i = 0;
    Id current_id = NO_ID;
    Id next_id = NO_ID;
    Id link_id = NO_ID;
    Id player_space_id = NO_ID;
    Link* link = NULL;

    if (game == NULL) {
        return ERROR;
    }

    player_space_id = player_get_space(game->player);
    if (player_space_id == NO_ID)
        return ERROR;

    for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
        current_id = space_get_id(game->spaces[i]);
        if (current_id == NO_ID) {
            return ERROR;
        }
        if (current_id == player_space_id) {
            link_id = space_get_west(game->spaces[i]);
            if (link_id == NO_ID || (link = game_get_link(game, link_id)) == NULL || !link_is_open(link)) {
                return ERROR;
            }
            /*Gets the other side of the link*/
            if ((next_id = link_get_space1(link)) == current_id) {
                next_id = link_get_space2(link);
            }
            if (next_id == NO_ID) {
                return ERROR;
            }

            if (player_set_space(game->player, next_id) == ERROR) {
                return ERROR;
            }
            if (game_space_is_lit(game, game_get_space(game, next_id)) == TRUE) {
                strncpy(game->description, space_get_description(game_get_space(game, next_id)), WORD_SIZE - 1);
            } else {
                game->description[0] = '\0';
            }
            return OK;
        }
    }
    return ERROR;
}

STATUS game_callback_right(Game* game) {
    int i = 0;
    Id current_id = NO_ID;
    Id next_id = NO_ID;
    Id link_id = NO_ID;
    Id player_space_id = NO_ID;
    Link* link = NULL;

    if (game == NULL) {
        return ERROR;
    }

    player_space_id = player_get_space(game->player);
    if (player_space_id == NO_ID)
        return ERROR;

    for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
        current_id = space_get_id(game->spaces[i]);
        if (current_id == NO_ID) {
            return ERROR;
        }
        if (current_id == player_space_id) {
            link_id = space_get_east(game->spaces[i]);
            if (link_id == NO_ID || (link = game_get_link(game, link_id)) == NULL || !link_is_open(link)) {
                return ERROR;
            }
            /*Gets the other side of the link*/
            if ((next_id = link_get_space1(link)) == current_id) {
                next_id = link_get_space2(link);
            }
            if (next_id == NO_ID) {
                return ERROR;
            }

            if (player_set_space(game->player, next_id) == ERROR) {
                return ERROR;
            }
            if (game_space_is_lit(game, game_get_space(game, next_id)) == TRUE) {
                strncpy(game->description, space_get_description(game_get_space(game, next_id)), WORD_SIZE - 1);
            } else {
                game->description[0] = '\0';
            }
            return OK;
        }
    }
    return ERROR;
}

STATUS game_callback_up(Game* game) {
    int i = 0;
    Id current_id = NO_ID;
    Id next_id = NO_ID;
    Id link_id = NO_ID;
    Id player_space_id = NO_ID;
    Link* link = NULL;

    if (game == NULL) {
        return ERROR;
    }

    player_space_id = player_get_space(game->player);
    if (player_space_id == NO_ID)
        return ERROR;

    for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
        current_id = space_get_id(game->spaces[i]);
        if (current_id == NO_ID) {
            return ERROR;
        }
        if (current_id == player_space_id) {
            link_id = space_get_up(game->spaces[i]);
            if (link_id == NO_ID || (link = game_get_link(game, link_id)) == NULL || !link_is_open(link)) {
                return ERROR;
            }
            /*Gets the other side of the link*/
            if ((next_id = link_get_space1(link)) == current_id) {
                next_id = link_get_space2(link);
            }
            if (next_id == NO_ID) {
                return ERROR;
            }

            if (player_set_space(game->player, next_id) == ERROR) {
                return ERROR;
            }
            if (game_space_is_lit(game, game_get_space(game, next_id)) == TRUE) {
                strncpy(game->description, space_get_description(game_get_space(game, next_id)), WORD_SIZE - 1);
            } else {
                game->description[0] = '\0';
            }
            return OK;
        }
    }
    return ERROR;
}

STATUS game_callback_down(Game* game) {
    int i = 0;
    Id current_id = NO_ID;
    Id next_id = NO_ID;
    Id link_id = NO_ID;
    Id player_space_id = NO_ID;
    Link* link = NULL;

    if (game == NULL) {
        return ERROR;
    }

    player_space_id = player_get_space(game->player);
    if (player_space_id == NO_ID)
        return ERROR;

    for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
        current_id = space_get_id(game->spaces[i]);
        if (current_id == NO_ID) {
            return ERROR;
        }
        if (current_id == player_space_id) {
            link_id = space_get_down(game->spaces[i]);
            if (link_id == NO_ID || (link = game_get_link(game, link_id)) == NULL || !link_is_open(link)) {
                return ERROR;
            }
            /*Gets the other side of the link*/
            if ((next_id = link_get_space1(link)) == current_id) {
                next_id = link_get_space2(link);
            }
            if (next_id == NO_ID) {
                return ERROR;
            }

            if (player_set_space(game->player, next_id) == ERROR) {
                return ERROR;
            }
            if (game_space_is_lit(game, game_get_space(game, next_id)) == TRUE) {
                strncpy(game->description, space_get_description(game_get_space(game, next_id)), WORD_SIZE - 1);
            } else {
                game->description[0] = '\0';
            }
            return OK;
        }
    }
    return ERROR;
}

STATUS game_callback_move(Game* game) {
    char input[MOVE_INPUT];

    if (game == NULL) {
        return ERROR;
    }

    if (scanf("%s", input) <= 0) {
        return ERROR;
    }
    strncpy(game->last_str, input, WORD_SIZE);

    if (!strcmp(input, "north") || !strcmp(input, "n")) {
        return game_callback_back(game);
    }

    if (!strcmp(input, "south") || !strcmp(input, "s")) {
        return game_callback_next(game);
    }

    if (!strcmp(input, "east") || !strcmp(input, "e")) {
        return game_callback_right(game);
    }

    if (!strcmp(input, "west") || !strcmp(input, "w")) {
        return game_callback_left(game);
    }

    if (!strcmp(input, "up") || !strcmp(input, "u")) {
        return game_callback_up(game);
    }

    if (!strcmp(input, "down") || !strcmp(input, "d")) {
        return game_callback_down(game);
    }

    return ERROR;
}

STATUS game_callback_inspect(Game* game) {
    char input[WORD_SIZE];
    int i, j, object_index;
    Id current_id = NO_ID, space_id, object_id;

    if (game == NULL) {
        return ERROR;
    }

    printf("Object or space to inspect: ");
    if (scanf("%s", input) <= 0) {
        return ERROR;
    }
    strncpy(game->last_str, input, WORD_SIZE);
    if (game->last_str == NULL) {
        return ERROR;
    }
    /*Inspect space*/
    if (!strcmp(input, "space") || !strcmp(input, "s")) {
        space_id = player_get_space(game->player);
        if (space_id == NO_ID) {
            return ERROR;
        }

        for (i = 0; i < game->n_spaces; i++) {
            current_id = space_get_id(game->spaces[i]);
            if (current_id == NO_ID) {
                return ERROR;
            }

            if (current_id == space_id) {
                if (game_space_is_lit(game, game->spaces[i]) == FALSE) {
                    return ERROR;
                }
                if (strncpy(game->full_desc, space_get_full_desc(game->spaces[i]), WORD_SIZE) == NULL) {
                    return ERROR;
                }
                return OK;
            }
        }
        return ERROR;
    }
    /*Inspect object*/
    object_id = game_id_object(game, input);
    if (!object_id)
        return ERROR;
    object_index = game_index_object(game, object_id);
    if (object_index < 0)
        return ERROR;

    space_id = player_get_space(game->player);
    if (space_id == NO_ID) {
        return ERROR;
    }

    for (j = 0; j < game->n_spaces && space_id != current_id; j++) {
        current_id = space_get_id(game->spaces[j]);
        if (current_id == NO_ID) {
            return ERROR;
        }
    }

    if (player_has_object(game->player, object_id) == FALSE && space_has_object(game->spaces[j - 1], object_id) == FALSE) {
        return ERROR;
    }

    if (game_space_is_lit(game, game->spaces[j - 1]) == FALSE) {
        return ERROR;
    }

    if (object_get_moved(game->objects[object_index]) == FALSE) {
        if (strncpy(game->full_desc, object_get_description(game->objects[object_index]), WORD_SIZE) == NULL) {
            return ERROR;
        }
    } else {
        if (strncpy(game->full_desc, object_get_full_desc(game->objects[object_index]), WORD_SIZE) == NULL) {
            return ERROR;
        }
    }
    return OK;
}

STATUS game_callback_load(Game *game) {
    char input[WORD_SIZE];

    if (game == NULL) {
        return ERROR;
    }

    printf("Load file: ");
    if (scanf("%s", input) <= 0) {
        return ERROR;
    }

    strncpy(game->last_str, input, WORD_SIZE);
    if (game->last_str == NULL) {
        return ERROR;
    }

    if (game_delete(game) == ERROR) {
        return ERROR;
    }

    if (game_management_load(game, input) == ERROR) {
        return ERROR;
    }

    return OK;
}

STATUS game_callback_save(Game *game) {
    char input[WORD_SIZE];
    if (game == NULL) {
        return ERROR;
    }

    printf("Store file: ");
    if (scanf("%s", input) <= 0) {
        return ERROR;
    }

    strncpy(game->last_str, input, WORD_SIZE);
    if (game->last_str == NULL) {
        return ERROR;
    }

    if (game_management_save(game, input) == ERROR) {
        return ERROR;
    }

    return OK;
}

STATUS game_callback_open(Game* game) {
    char link[WORD_SIZE], object[WORD_SIZE], aux[WORD_SIZE];
    int i, j;

    if (game == NULL) {
        return ERROR;
    }

    printf("<link> with <item>:");
    if (scanf("%s %s %s", link, aux, object) < 0) {
        return ERROR;
    }

    if (strcmp(aux, "with")) {
        return ERROR;
    }

    if (sprintf(game->last_str + strlen(game->last_str), "%s %s %s", link, aux, object) < 0) {
        return ERROR;
    }

    for (i = 0; i < game->n_links; i++) {
        if (!strcmp(link, link_get_name(game->links[i]))) {
            if (link_get_space1(game->links[i]) != player_get_space(game->player) && link_get_space2(game->links[i]) != player_get_space(game->player)) {
                return ERROR;
            }

            if (link_is_open(game->links[i]) == TRUE) {
                return ERROR;
            }

            for (j = 0; j < game->n_objects; j++) {
                if (!strcmp(object, object_get_name(game->objects[j]))) {
                    if (object_get_open(game->objects[j]) != link_get_id(game->links[i])) {
                        return ERROR;
                    }

                    if (link_set_open(game->links[i], TRUE) == ERROR) {
                        return ERROR;
                    }
                    return OK;
                }
            }
        }
    }
    return ERROR;
}

/*------------------Private Functions' Implementation------------------*/

Id game_id_object(Game* game, char* name) {
    int i;
    if (!game || !name)
        return NO_ID;

    for (i = 0; i < MAX_OBJECTS; i++) {
        if (game->objects[i] != NULL && strcmp(object_get_name(game->objects[i]), name) == 0) {
            return object_get_id(game->objects[i]);
        }
    }
    return NO_ID;
}

int game_index_object(Game* game, Id id) {
    int i;
    if (!game || id == NO_ID)
        return -1;

    for (i = 0; i < MAX_OBJECTS; i++) {
        if (game->objects[i] != NULL && object_get_id(game->objects[i]) == id) {
            return i;
        }
    }
    return -1;
}

int game_index_object_name(Game* game, char* name) {
    int i;
    if (!game || !name)
        return NO_ID;

    for (i = 0; i < MAX_OBJECTS; i++) {
        if (game->objects[i] != NULL && strcmp(object_get_name(game->objects[i]), name) == 0) {
            return i;
        }
    }
    return NO_ID;
}

int game_get_space_index_at(Game* game, Id space_id) {
    int i, current_id;

    if (game == NULL || space_id == NO_ID) {
        return -1;
    }

    for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
        current_id = space_get_id(game->spaces[i]);
        if (current_id == space_id) break;
    }
    /*ERROR, the space of space_id is not found*/
    if (i == MAX_SPACES || game->spaces[i] == NULL)
        return -1;

    return i;
}

BOOL game_has_illuminating_object(Game* game, Id* array, int num) {
    Object *obj = NULL;
    int i = 0;

    for (i = 0; i < num && array[i] != NO_ID; i++) {
        obj = game_get_object(game, array[i]);
        if (obj == NULL) {
            return FALSE;
        }

        if (object_can_illuminate(obj) && object_is_turnedon(obj)) {
            return TRUE;
        }
    }
    return FALSE;
}

STATUS game_delete(Game* game) {
    int i;
    if (!game) return ERROR;


    for (i = game->n_spaces - 1; i >= 0; i--) {
        space_destroy(game->spaces[i]);
        game->spaces[i] = NULL;
        game->n_spaces--;
    }
    for (i = game->n_objects - 1; i >= 0; i--) {
        object_destroy(game->objects[i]);
        game->objects[i] = NULL;
        game->n_objects--;
    }
    for (i = game->n_links - 1; i >= 0; i--) {
        link_destroy(game->links[i]);
        game->links[i] = NULL;
        game->n_links--;
    }

    player_destroy(game->player);
    game->player = NULL;
    die_destroy(game->die);
    game->die = NULL;
    game->die = die_create(global_next_die_id++, MIN_DIE, MAX_DIE);
    if (!game->die) return ERROR;

    game->last_cmd = NO_CMD;
    game->last_str[0] = '\0';
    game->description[0] = '\0';

    return OK;
}
