##########################################
#quitar el -g
CC = gcc -g
CFLAGS = -Wall -ansi -pedantic -Iinclude

.PHONY: clean

#AÑADIR EL documentation EN EL ALL
all: I4 graphic_engine_test game_rules_test game_management_test game_test space_test player_test inventory_test object_test die_test set_test link_test dialogue_test game_rules_test #clean documentation

I4: obj/game_loop.o obj/screen.o obj/command.o obj/graphic_engine.o obj/game_management.o obj/game.o obj/space.o obj/player.o obj/object.o obj/set.o obj/die.o obj/inventory.o obj/link.o obj/dialogue.o obj/game_rules.o
	@echo "Para ejecutar las pruebas, usa make I4_NO_RULE"
	$(CC) -o ejecutables/$@.exe $^

I4_NO_RULE: clean
	@echo "Compilando con NO_RULE"
	make I4 CFLAGS="-Wall -ansi -pedantic -Iinclude -DNO_RULE"

#-----Test programs-----#
graphic_engine_test: obj/graphic_engine_test.o obj/space.o obj/player.o obj/object.o obj/game.o obj/game_rules.o obj/game_management.o obj/graphic_engine.o obj/dialogue.o obj/set.o obj/die.o obj/link.o obj/inventory.o obj/screen.o obj/command.o
	$(CC) -o ejecutables/$@.exe $^

dialogue_test: obj/dialogue_test.o obj/space.o obj/player.o obj/object.o obj/game.o obj/game_management.o obj/dialogue.o obj/set.o obj/die.o obj/link.o obj/inventory.o obj/command.o
	$(CC) -o ejecutables/$@.exe $^

game_management_test: obj/game_management_test.o obj/space.o obj/player.o obj/object.o obj/game.o obj/game_management.o obj/set.o obj/die.o obj/link.o obj/inventory.o
	$(CC) -o ejecutables/$@.exe $^

game_test: obj/game_test.o obj/space.o obj/player.o obj/object.o obj/game.o obj/set.o obj/die.o obj/link.o obj/inventory.o obj/game_management.o obj/game_rules.o
	$(CC) -o ejecutables/$@.exe $^

space_test: obj/space_test.o obj/space.o obj/set.o
	$(CC) -o ejecutables/$@.exe $^

player_test: obj/player_test.o obj/space.o obj/player.o obj/object.o obj/set.o obj/inventory.o
	$(CC) -o ejecutables/$@.exe $^

inventory_test: obj/inventory_test.o obj/inventory.o obj/set.o
	$(CC) -o ejecutables/$@.exe $^

object_test: obj/object_test.o obj/object.o
	$(CC) -o ejecutables/$@.exe $^

die_test: obj/die_test.o obj/die.o
	$(CC) -o ejecutables/$@.exe $^

set_test: obj/set_test.o obj/set.o
	$(CC) -o ejecutables/$@.exe $^

link_test: obj/link_test.o obj/link.o
	$(CC) -o ejecutables/$@.exe $^

game_rules_test: obj/game_rules_test.o obj/game_rules.o obj/game.o obj/space.o obj/player.o obj/object.o obj/set.o obj/die.o obj/link.o obj/inventory.o obj/game_management.o
	$(CC) -o ejecutables/$@.exe $^


#-----Compilation-----#
obj/game_loop.o: src/game_loop.c include/graphic_engine.h include/game.h include/command.h include/space.h include/set.h include/types.h include/link.h include/player.h include/object.h include/die.h include/game_management.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/screen.o: src/screen.c include/screen.h include/space.h include/set.h include/types.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/command.o: src/command.c include/command.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/graphic_engine.o: src/graphic_engine.c include/screen.h include/graphic_engine.h include/game.h include/command.h include/space.h include/set.h include/types.h include/link.h include/player.h include/object.h include/die.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/graphic_engine_test.o: src/graphic_engine_test.c include/types.h include/graphic_engine.h include/game.h include/command.h include/space.h include/set.h include/link.h include/player.h include/object.h include/die.h include/graphic_engine_test.h include/test.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/dialogue.o: src/dialogue.c include/dialogue.h include/game.h include/types.h include/command.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/dialogue_test.o: src/dialogue_test.c include/types.h include/dialogue.h include/game.h include/command.h include/space.h include/set.h include/link.h include/player.h include/object.h include/die.h include/dialogue_test.h include/test.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/game_management.o: src/game_management.c include/game_management.h include/game.h include/command.h include/space.h include/set.h include/types.h include/link.h include/player.h include/inventory.h include/object.h include/die.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/game_management_test.o: src/game_management_test.c include/game_management.h include/game.h include/command.h include/space.h include/set.h include/types.h include/link.h include/player.h include/object.h include/die.h include/test.h include/game_management_test.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/game.o: src/game.c include/game.h include/command.h include/space.h include/set.h include/types.h include/link.h include/player.h include/inventory.h include/object.h include/die.h include/game_management.h include/game.h include/graphic_engine.h include/command.h include/game_rules.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/game_test.o: src/game_test.c include/game.h include/command.h include/space.h include/set.h include/types.h include/link.h include/player.h include/object.h include/die.h include/game_test.h include/test.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/space.o: src/space.c include/types.h include/space.h include/set.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/space_test.o: src/space_test.c include/space_test.h include/space.h include/types.h include/set.h include/object.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/player.o: src/player.c include/player.h include/types.h include/inventory.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/player_test.o: src/player_test.c include/player.h include/types.h include/player_test.h include/test.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/object.o: src/object.c include/object.h include/types.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/object_test.o: src/object_test.c include/object.h include/types.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/die.o: src/die.c include/die.h include/types.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/die_test.o: src/die_test.c include/die.h include/types.h include/die_test.h include/test.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/set.o: src/set.c include/set.h include/types.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/set_test.o: src/set_test.c include/set.h include/types.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/link.o: src/link.c include/link.h include/types.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/link_test.o: src/link_test.c include/link.h include/types.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/inventory.o: src/inventory.c include/inventory.h include/types.h include/set.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/inventory_test.o: src/inventory_test.c include/inventory.h include/types.h include/inventory_test.h include/test.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/game_rules.o: src/game_rules.c include/types.h include/game.h include/command.h include/space.h include/set.h include/types.h include/link.h include/player.h include/inventory.h include/object.h include/die.h include/game_rules.h include/game.h include/object.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/game_rules_test.o: src/game_rules_test.c include/game.h include/command.h include/space.h include/set.h include/types.h include/link.h include/player.h include/inventory.h include/object.h include/die.h include/game_rules_test.h include/test.h include/game_rules.h include/game.h
	$(CC) $(CFLAGS) -c $< -o $@


#-----Run commands-----#
run:
	@echo "\n>>>>>>Running I4"
	./ejecutables/I4.exe data.dat -l naughty_and_nice.log

runv:
	@echo "\n>>>>>>Running I4 with valgrind"
	valgrind --leak-check=full ./ejecutables/I4.exe data.dat -l naughty_and_nice.log

run_auto:
	#This command will execute the steps to finish successfully the game
	@echo "\n>>>>>>Running I4 automatic"
	./ejecutables/I4.exe data.dat -l naughty_and_nice.log < Manual/auto.log

run_object_test:
	@echo "Probando las funcionalidades de Object"
	./ejecutables/object_test.exe

runv_object_test:
	@echo "Probando las funcionalidades de Object con Valgrind"
	valgrind --leak-check=full ./ejecutables/object_test.exe

run_set_test:
	@echo "Probando las funcionalidades de Set"
	./ejecutables/set_test.exe

runv_set_test:
	@echo "Probando las funcionalidades de Set con Valgrind"
	valgrind --leak-check=full ./ejecutables/set_test.exe

run_player_test:
	@echo "Probando las funcionalidades de Player"
	./ejecutables/player_test.exe

runv_player_test:
	@echo "Probando las funcionalidades de Player con Valgrind"
	valgrind --leak-check=full ./ejecutables/player_test.exe

run_inventory_test:
	@echo "Probando las funcionalidades de Inventory"
	./ejecutables/inventory_test.exe

runv_inventory_test:
	@echo "Probando las funcionalidades de Inventory con Valgrind"
	valgrind --leak-check=full ./ejecutables/inventory_test.exe

run_die_test:
	@echo "Probando las funcionalidades de Die"
	./ejecutables/die_test.exe

runv_die_test:
	@echo "Probando las funcionalidades de Die con Valgrind"
	valgrind --leak-check=full ./ejecutables/die_test.exe

run_game_test:
	@echo "Probando las funcionalidades de Game"
	./ejecutables/game_test.exe

runv_game_test:
	@echo "Probando las funcionalidades de Game con Valgrind"
	valgrind --leak-check=full ./ejecutables/game_test.exe

run_link_test:
	@echo "Probando las funcionalidades de Link"
	./ejecutables/link_test.exe

runv_link_test:
	@echo "Probando las funcionalidades de Link con Valgrind"
	valgrind --leak-check=full ./ejecutables/link_test.exe

run_game_management_test:
	@echo "Probando las funcionalidades de Game_management"
	./ejecutables/game_management_test.exe prueba1.dat prueba2.dat

runv_game_management_test:
	@echo "Probando las funcionalidades de Game_management con Valgrind"
	valgrind --leak-check=full ./ejecutables/game_management_test.exe prueba1.dat prueba2.dat

run_dialogue_test:
	@echo "Probando las funcionalidades de dialogue"
	./ejecutables/dialogue_test.exe

runv_dialogue_test:
	@echo "Probando las funcionalidades de dialogue con Valgrind"
	valgrind --leak-check=full ./ejecutables/dialogue_test.exe

run_graphic_engine_test:
	@echo "Probando las funcionalidades de Graphic_engine"
	./ejecutables/graphic_engine_test.exe

runv_graphic_engine_test:
	@echo "Probando las funcionalidades de Graphic_engine con Valgrind"
	valgrind --leak-check=full ./ejecutables/graphic_engine_test.exe

run_space_test:
	@echo "Probando las funcionalidades de Space"
	./ejecutables/space_test.exe

runv_space_test:
	@echo "Probando las funcionalidades de Space con Valgrind"
	valgrind --leak-check=full ./ejecutables/space_test.exe

run_game_rules_test:
	@echo "Probando las funcionalidades de Game_rules"
	./ejecutables/game_rules_test.exe

runv_game_rules_test:
	@echo "Probando las funcionalidades de Game_rules con Valgrind"
	valgrind --leak-check=full ./ejecutables/game_rules_test.exe

test01:
	./ejecutables/I4.exe TestFiles/game01.dat -l game01.sal -t < TestFiles/game01.ent
	diff game01.sal TestFiles/game01.sal

test02:
	./ejecutables/I4.exe TestFiles/game02.dat -l game02.sal -t < TestFiles/game02.ent
	diff game02.sal TestFiles/game02.sal

test03:
	./ejecutables/I4.exe TestFiles/game03.dat -l game03.sal -t < TestFiles/game03.ent
	diff game03.sal TestFiles/game03.sal

test04:
	./ejecutables/I4.exe TestFiles/game04.dat -l game04.sal -t < TestFiles/game04.ent
	diff game04.sal TestFiles/game04.sal

test05:
	./ejecutables/I4.exe TestFiles/game05.dat -l game05.sal -t < TestFiles/game05.ent
	diff game05.sal TestFiles/game05.sal

test06:
	./ejecutables/I4.exe TestFiles/game06.dat -l game06.sal -t < TestFiles/game06.ent
	diff game06.sal TestFiles/game06.sal

test07:
	./ejecutables/I4.exe TestFiles/game07.dat -l game07.sal -t < TestFiles/game07.ent
	diff game07.sal TestFiles/game07.sal


#-----Miscellaneous-----#
documentation:
	@echo Generating documentation:
	@doxygen Doxyfile

help:
	cat Manual/Manual.txt

clean:
	rm -rf obj/*.o *~ *.sal

clean_all:
	rm -rf obj/*.o *~ *.sal ejecutables/*.exe doc/* *.log
##########################################
