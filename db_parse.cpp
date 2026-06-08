#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <climits>
#include <string>

#include "db_parse.h"

static char *next_token(char *start, char delim)
{
  int i;
  static char *s;

  if (start) {
    s = start;
  }

  start = s;

  for (i = 0; s[i] && s[i] != delim; i++)
    ;
  s[i] = '\0';
  s = s + i + 1;

  return start;
}

static const char *i2s(int i)
{
  static int next = 0;
  static char s[20][12];

  if (next == 20) {
    next = 0;
  }

  if (i == INT_MAX) {
    s[next][0] = '\0';
  } else {
    sprintf(s[next], "%d", i);
  }

  return s[next++];
}

pokemon_move_db pokemon_moves[528239];
pokemon_db pokemon[1093];
char *types[19];
move_db moves[845];
pokemon_species_db species[899];
experience_db experience[601];
pokemon_stats_db pokemon_stats[6553];
stats_db stats[9];
pokemon_types_db pokemon_types[1676];
int db_species_count;

static char *directory_prefix(const char *path)
{
  size_t length;
  char *prefix;

  if (!path || !*path) {
    return NULL;
  }

  length = strlen(path);
  prefix = (char *) malloc(length + (path[length - 1] == '/' ? 1 : 2));
  strcpy(prefix, path);
  if (path[length - 1] != '/') {
    strcat(prefix, "/");
  }

  return prefix;
}

static bool database_is_complete(const char *prefix)
{
  static const char *required_files[] = {
    "pokemon.csv",
    "moves.csv",
    "pokemon_moves.csv",
    "pokemon_species.csv",
    "experience.csv",
    "type_names.csv",
    "pokemon_stats.csv",
    "stats.csv",
    "pokemon_types.csv"
  };
  struct stat buf;

  for (const char *file : required_files) {
    std::string path = std::string(prefix) + file;
    if (stat(path.c_str(), &buf)) {
      return false;
    }
  }

  return true;
}

static char *find_database()
{
  const char *environment_path = getenv("POKE327_DATA_DIR");
  const char *home = getenv("HOME");
  const char *candidates[] = {
    environment_path,
    "./data/csv",
    "./pokedex/pokedex/data/csv",
    "/share/cs327/pokedex/pokedex/data/csv"
  };

  for (const char *candidate : candidates) {
    char *prefix = directory_prefix(candidate);
    if (prefix && database_is_complete(prefix)) {
      return prefix;
    }
    free(prefix);
  }

  if (home) {
    std::string home_path =
      std::string(home) + "/.poke327/pokedex/pokedex/data/csv";
    char *prefix = directory_prefix(home_path.c_str());
    if (prefix && database_is_complete(prefix)) {
      return prefix;
    }
    free(prefix);
  }

  return NULL;
}

static void set_move(int index, const char *name, int type, int power,
                     int accuracy, int priority = 0)
{
  moves[index].id = index;
  strncpy(moves[index].identifier, name,
          sizeof (moves[index].identifier) - 1);
  moves[index].identifier[sizeof (moves[index].identifier) - 1] = '\0';
  moves[index].type_id = type;
  moves[index].power = power;
  moves[index].accuracy = accuracy;
  moves[index].priority = priority;
  moves[index].damage_class_id = 2;
}

static void set_species(int index, const char *name, int capture_rate,
                        int hp, int attack, int defense, int special_attack,
                        int special_defense, int speed, int type1, int type2,
                        int move1, int move2, int &type_index)
{
  species[index].id = index;
  strncpy(species[index].identifier, name,
          sizeof (species[index].identifier) - 1);
  species[index].identifier[sizeof (species[index].identifier) - 1] = '\0';
  species[index].capture_rate = capture_rate;
  species[index].base_stat[0] = hp;
  species[index].base_stat[1] = attack;
  species[index].base_stat[2] = defense;
  species[index].base_stat[3] = special_attack;
  species[index].base_stat[4] = special_defense;
  species[index].base_stat[5] = speed;
  species[index].levelup_moves.push_back({ 1, move1 });
  species[index].levelup_moves.push_back({ 1, move2 });

  pokemon_types[type_index++] = { index, type1, 1 };
  if (type2) {
    pokemon_types[type_index++] = { index, type2, 2 };
  }
}

static void load_builtin_database()
{
  int type_index = 1;

  set_move(1, "tackle", 1, 40, 100);
  set_move(2, "vine-whip", 12, 45, 100);
  set_move(3, "ember", 10, 40, 100);
  set_move(4, "water-gun", 11, 40, 100);
  set_move(5, "thunder-shock", 13, 40, 100);
  set_move(6, "rock-throw", 6, 50, 90);
  set_move(7, "lick", 8, 30, 100);
  set_move(8, "confusion", 14, 50, 100);
  set_move(9, "karate-chop", 2, 50, 100);
  set_move(10, "twister", 16, 40, 100);
  set_move(11, "quick-attack", 1, 40, 100, 1);
  set_move(12, "disarming-voice", 18, 40, 100);
  set_move(13, "bug-bite", 7, 60, 100);
  set_move(14, "gust", 3, 40, 100);
  set_move(15, "powder-snow", 15, 40, 100);
  set_move(16, "metal-claw", 9, 50, 95);
  set_move(17, "bite", 17, 60, 100);
  set_move(18, "poison-sting", 4, 15, 100);
  set_move(19, "mud-slap", 5, 20, 100);
  set_move(20, "bubble-beam", 11, 65, 100);

  set_species(1, "bulbasaur", 45, 45, 49, 49, 65, 65, 45,
              12, 4, 1, 2, type_index);
  set_species(2, "charmander", 45, 39, 52, 43, 60, 50, 65,
              10, 0, 1, 3, type_index);
  set_species(3, "squirtle", 45, 44, 48, 65, 50, 64, 43,
              11, 0, 1, 4, type_index);
  set_species(4, "pikachu", 190, 35, 55, 40, 50, 50, 90,
              13, 0, 5, 11, type_index);
  set_species(5, "geodude", 255, 40, 80, 100, 30, 30, 20,
              6, 5, 6, 19, type_index);
  set_species(6, "gastly", 190, 30, 35, 30, 100, 35, 80,
              8, 4, 7, 18, type_index);
  set_species(7, "abra", 200, 25, 20, 15, 105, 55, 90,
              14, 0, 8, 11, type_index);
  set_species(8, "machop", 180, 70, 80, 50, 35, 35, 35,
              2, 0, 9, 1, type_index);
  set_species(9, "dratini", 45, 41, 64, 45, 50, 50, 50,
              16, 0, 10, 1, type_index);
  set_species(10, "eevee", 45, 55, 55, 50, 45, 65, 55,
              1, 0, 1, 11, type_index);
  set_species(11, "jigglypuff", 170, 115, 45, 20, 45, 25, 20,
              1, 18, 1, 12, type_index);
  set_species(12, "caterpie", 255, 45, 30, 35, 20, 20, 45,
              7, 0, 13, 1, type_index);
  set_species(13, "pidgey", 255, 40, 45, 40, 35, 35, 56,
              1, 3, 14, 11, type_index);
  set_species(14, "snorunt", 190, 50, 50, 50, 50, 50, 50,
              15, 0, 15, 1, type_index);
  set_species(15, "magnemite", 190, 25, 35, 70, 95, 55, 45,
              13, 9, 5, 16, type_index);
  set_species(16, "sandile", 180, 50, 72, 35, 35, 35, 65,
              5, 17, 19, 17, type_index);
  set_species(17, "trubbish", 190, 50, 50, 62, 40, 62, 65,
              4, 0, 18, 1, type_index);
  set_species(18, "lapras", 45, 130, 85, 80, 85, 95, 60,
              11, 15, 20, 15, type_index);

  db_species_count = 18;
}

void db_parse(bool print)
{
  FILE *f;
  char line[800];
  int i;
  char *tmp;
  char *prefix;
  int prefix_len;
  int j;
  int count;

  prefix = find_database();

  if (!prefix) {
    load_builtin_database();
    return;
  }

  db_species_count = 898;
  prefix_len = strlen(prefix);

  prefix = (char *) realloc(prefix, prefix_len + strlen("pokemon.csv") + 1);
  strcpy(prefix + prefix_len, "pokemon.csv");
  f = fopen(prefix, "r");
  prefix = (char *) realloc(prefix, prefix_len + 1);

  fgets(line, 80, f);
  for (i = 1; i < 1093; i++) {
    fgets(line, 80, f);
    pokemon[i].id = atoi(next_token(line, ','));
    strncpy(pokemon[i].identifier, next_token(NULL, ','), 29);
    pokemon[i].identifier[29] = '\0';
    pokemon[i].species_id = atoi(next_token(NULL, ','));
    pokemon[i].height = atoi(next_token(NULL, ','));
    pokemon[i].weight = atoi(next_token(NULL, ','));
    pokemon[i].base_experience = atoi(next_token(NULL, ','));
    pokemon[i].order = atoi(next_token(NULL, ','));
    pokemon[i].is_default = atoi(next_token(NULL, ','));
  }
  fclose(f);

  if (print) {
    f = fopen("pokemon.csv", "w");
    for (i = 1; i < 1093; i++) {
      fprintf(f, "%s,%s,%s,%s,%s,%s,%s,%s\n",
              i2s(pokemon[i].id),
              pokemon[i].identifier,
              i2s(pokemon[i].species_id),
              i2s(pokemon[i].height),
              i2s(pokemon[i].weight),
              i2s(pokemon[i].base_experience),
              i2s(pokemon[i].order),
              i2s(pokemon[i].is_default));
    }
    fclose(f);
  }

  prefix = (char *) realloc(prefix, prefix_len + strlen("moves.csv") + 1);
  strcpy(prefix + prefix_len, "moves.csv");
  f = fopen(prefix, "r");
  prefix = (char *) realloc(prefix, prefix_len + 1);

  fgets(line, 800, f);
  for (i = 1; i < 845; i++) {
    fgets(line, 800, f);
    moves[i].id = atoi((tmp = next_token(line, ',')));
    strncpy(moves[i].identifier, (tmp = next_token(NULL, ',')), 49);
    moves[i].identifier[49] = '\0';
    tmp = next_token(NULL, ',');
    moves[i].generation_id = *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    moves[i].type_id =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    moves[i].power =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    moves[i].pp =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    moves[i].accuracy =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    moves[i].priority =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    moves[i].target_id =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    moves[i].damage_class_id =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    moves[i].effect_id =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    moves[i].effect_chance =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    moves[i].contest_type_id =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    moves[i].contest_effect_id =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    moves[i].super_contest_effect_id = (*tmp != '\n') ? atoi(tmp) : INT_MAX;
  }
  fclose(f);

  if (print) {
    f = fopen("moves.csv", "w");
    for (i = 1; i < 845; i++) {
      fprintf(f, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
              i2s(moves[i].id),
              moves[i].identifier,
              i2s(moves[i].generation_id),
              i2s(moves[i].type_id),
              i2s(moves[i].power),
              i2s(moves[i].pp),
              i2s(moves[i].accuracy),
              i2s(moves[i].priority),
              i2s(moves[i].target_id),
              i2s(moves[i].damage_class_id),
              i2s(moves[i].effect_id),
              i2s(moves[i].effect_chance),
              i2s(moves[i].contest_type_id),
              i2s(moves[i].contest_effect_id),
              i2s(moves[i].super_contest_effect_id));
    }
    fclose(f);
  }

  prefix = (char *) realloc(prefix,
                            prefix_len + strlen("pokemon_moves.csv") + 1);
  strcpy(prefix + prefix_len, "pokemon_moves.csv");
  f = fopen(prefix, "r");
  prefix = (char *) realloc(prefix, prefix_len + 1);

  fgets(line, 800, f);
  for (i = 1; i < 528239; i++) {
    fgets(line, 800, f);
    tmp = next_token(line, ',');
    pokemon_moves[i].pokemon_id = *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    pokemon_moves[i].version_group_id = *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    pokemon_moves[i].move_id = *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    pokemon_moves[i].pokemon_move_method_id = *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    pokemon_moves[i].level = *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    pokemon_moves[i].order = (*tmp != '\n') ? atoi(tmp) : INT_MAX;
  }
  fclose(f);

  if (print) {
    f = fopen("pokemon_moves.csv", "w");
    for (i = 1; i < 528239; i++) {
      fprintf(f, "%s,%s,%s,%s,%s,%s\n",
              i2s(pokemon_moves[i].pokemon_id),
              i2s(pokemon_moves[i].version_group_id),
              i2s(pokemon_moves[i].move_id),
              i2s(pokemon_moves[i].pokemon_move_method_id),
              i2s(pokemon_moves[i].level),
              i2s(pokemon_moves[i].order));
    }
    fclose(f);
  }

  prefix = (char *) realloc(prefix,
                            prefix_len + strlen("pokemon_species.csv") + 1);
  strcpy(prefix + prefix_len, "pokemon_species.csv");
  f = fopen(prefix, "r");
  prefix = (char *) realloc(prefix, prefix_len + 1);

  fgets(line, 800, f);
  for (i = 1; i < 899; i++) {
    fgets(line, 800, f);
    species[i].id = atoi((tmp = next_token(line, ',')));
    strncpy(species[i].identifier, (tmp = next_token(NULL, ',')), 29);
    species[i].identifier[29] = '\0';
    tmp = next_token(NULL, ',');
    species[i].generation_id = *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].evolves_from_species_id =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].evolution_chain_id =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].color_id =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].shape_id =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].habitat_id =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].gender_rate =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].capture_rate =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].base_happiness =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].is_baby =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].hatch_counter =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].has_gender_differences =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].growth_rate_id =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].forms_switchable =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].is_legendary =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].is_mythical =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].order =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    species[i].conquest_order = (*tmp != '\n') ? atoi(tmp) : INT_MAX;
  }
  fclose(f);

  if (print) {
    f = fopen("pokemon_species.csv", "w");
    for (i = 1; i < 899; i++) {
      fprintf(f,
              "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
              i2s(species[i].id),
              species[i].identifier,
              i2s(species[i].generation_id),
              i2s(species[i].evolves_from_species_id),
              i2s(species[i].evolution_chain_id),
              i2s(species[i].color_id),
              i2s(species[i].shape_id),
              i2s(species[i].habitat_id),
              i2s(species[i].gender_rate),
              i2s(species[i].capture_rate),
              i2s(species[i].base_happiness),
              i2s(species[i].is_baby),
              i2s(species[i].hatch_counter),
              i2s(species[i].has_gender_differences),
              i2s(species[i].growth_rate_id),
              i2s(species[i].forms_switchable),
              i2s(species[i].is_legendary),
              i2s(species[i].is_mythical),
              i2s(species[i].order),
              i2s(species[i].conquest_order));
    }
    fclose(f);
  }

  prefix = (char *) realloc(prefix, prefix_len + strlen("experience.csv") + 1);
  strcpy(prefix + prefix_len, "experience.csv");
  f = fopen(prefix, "r");
  prefix = (char *) realloc(prefix, prefix_len + 1);

  fgets(line, 800, f);
  for (i = 1; i < 601; i++) {
    fgets(line, 800, f);
    experience[i].growth_rate_id = atoi((tmp = next_token(line, ',')));
    tmp = next_token(NULL, ',');
    experience[i].level = *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    experience[i].experience =  (*tmp != '\n') ? atoi(tmp) : INT_MAX;
  }
  fclose(f);

  if (print) {
    f = fopen("experience.csv", "w");
    for (i = 1; i < 601; i++) {
      fprintf(f, "%s,%s,%s\n",
              i2s(experience[i].growth_rate_id),
              i2s(experience[i].level),
              i2s(experience[i].experience));
    }
    fclose(f);
  }

  prefix = (char *) realloc(prefix, prefix_len + strlen("type_names.csv") + 1);
  strcpy(prefix + prefix_len, "type_names.csv");
  f = fopen(prefix, "r");
  prefix = (char *) realloc(prefix, prefix_len + 1);

  fgets(line, 800, f);
  for (i = 1; i < 19; i++) {
    fgets(line, 800, f);
    fgets(line, 800, f);
    fgets(line, 800, f);
    fgets(line, 800, f);
    fgets(line, 800, f);
    fgets(line, 800, f);
    fgets(line, 800, f);
    fgets(line, 800, f);
    for (j = count = 0; count < 2; j++) {
      if (line[j] == ',') {
        count++;
      }
    }
    line[strlen(line) - 1] = '\0';
    types[i] = strdup(line + j);
    fgets(line, 800, f);
    fgets(line, 800, f);
  }
  fclose(f);

  if (print) {
    f = fopen("type_names.csv", "w");
    for (i = 1; i < 19; i++) {
      fprintf(f, "%s\n", types[i]);
    }
    fclose(f);
  }

  prefix = (char *) realloc(prefix,
                            prefix_len + strlen("pokemon_stats.csv") + 1);
  strcpy(prefix + prefix_len, "pokemon_stats.csv");
  f = fopen(prefix, "r");
  prefix = (char *) realloc(prefix, prefix_len + 1);

  fgets(line, 800, f);
  for (i = 1; i < 6553; i++) {
    fgets(line, 800, f);
    pokemon_stats[i].pokemon_id = atoi((tmp = next_token(line, ',')));
    tmp = next_token(NULL, ',');
    pokemon_stats[i].stat_id = *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    pokemon_stats[i].base_stat =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    pokemon_stats[i].effort =  (*tmp != '\n') ? atoi(tmp) : INT_MAX;
  }
  fclose(f);

  if (print) {
    f = fopen("pokemon_stats.csv", "w");
    for (i = 1; i < 6553; i++) {
      fprintf(f, "%s,%s,%s,%s\n",
              i2s(pokemon_stats[i].pokemon_id),
              i2s(pokemon_stats[i].stat_id),
              i2s(pokemon_stats[i].base_stat),
              i2s(pokemon_stats[i].effort));
    }
    fclose(f);
  }

  prefix = (char *) realloc(prefix, prefix_len + strlen("stats.csv") + 1);
  strcpy(prefix + prefix_len, "stats.csv");
  f = fopen(prefix, "r");
  prefix = (char *) realloc(prefix, prefix_len + 1);

  fgets(line, 800, f);
  for (i = 1; i < 9; i++) {
    fgets(line, 800, f);
    stats[i].id = atoi((tmp = next_token(line, ',')));
    tmp = next_token(NULL, ',');
    stats[i].damage_class_id = *tmp ? atoi(tmp) : INT_MAX;
    strncpy(stats[i].identifier, (tmp = next_token(NULL, ',')), 29);
    stats[i].identifier[29] = '\0';
    tmp = next_token(NULL, ',');
    stats[i].is_battle_only =  *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    stats[i].game_index =  (*tmp != '\n') ? atoi(tmp) : INT_MAX;
  }
  fclose(f);

  if (print) {
    f = fopen("stats.csv", "w");
    for (i = 1; i < 9; i++) {
      fprintf(f, "%s,%s,%s,%s,%s\n",
              i2s(stats[i].id),
              i2s(stats[i].damage_class_id),
              stats[i].identifier,
              i2s(stats[i].is_battle_only),
              i2s(stats[i].game_index));
    }
    fclose(f);
  }

  prefix = (char *) realloc(prefix,
                            prefix_len + strlen("pokemon_types.csv") + 1);
  strcpy(prefix + prefix_len, "pokemon_types.csv");
  f = fopen(prefix, "r");
  prefix = (char *) realloc(prefix, prefix_len + 1);

  fgets(line, 800, f);
  for (i = 1; i < 1676; i++) {
    fgets(line, 800, f);
    pokemon_types[i].pokemon_id = atoi((tmp = next_token(line, ',')));
    tmp = next_token(NULL, ',');
    pokemon_types[i].type_id = *tmp ? atoi(tmp) : INT_MAX;
    tmp = next_token(NULL, ',');
    pokemon_types[i].slot = (*tmp != '\n') ? atoi(tmp) : INT_MAX;
  }
  fclose(f);

  if (print) {
    f = fopen("pokemon_types.csv", "w");
    for (i = 1; i < 1676; i++) {
      fprintf(f, "%s,%s,%s\n",
              i2s(pokemon_types[i].pokemon_id),
              i2s(pokemon_types[i].type_id),
              i2s(pokemon_types[i].slot));
    }
    fclose(f);
  }

  free(prefix);
}
