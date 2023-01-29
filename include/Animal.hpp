#ifndef DEF_ANIMAL
#define DEF_ANIMAL

/*!
 * \file Animal.hpp
 * \brief Definition of Animal class
 * \author Baptiste D.
 * \version 1.0
 */

#include"Map.hpp"
#include<vector>


enum{EMPTY, PREY, PREDATOR};
enum{UP, RIGHT, DOWN, LEFT};

#define MAX_YO_PREY 100
#define MAX_YO_PREDATOR 200

#define FECONDITY_PREY 10
#define FECONDITY_PREDATOR 50




 typedef struct{
   int x;
   int y;
 }Position;


class Animal{

public:
  static int count_prey;
  static int count_predator;
  static int count_prey_tot;
  static int count_predator_tot;
  Animal();
  Animal(int x, int y, int identity, int index);
  Animal(Animal const& a);
  ~Animal();

  int rules(Map &map);
  void birth(Map &map, std::vector<Animal> &group);
  int update_position(Map &map) const;

  void clear_position(Map &map) const;

  int get_count_prey() const;
  int get_count_predator() const;
  int get_position_x() const;
  int get_position_y() const;
  int get_yo() const;
  int get_max_yo() const;
  int get_index() const;
  int get_id() const;


private:
  int id;
  int index;
  Position position;
  int health; //from 0 (dead) to 20 (super good health)

  int age;
  int max_yo;

  int fecondity;
  int repro_yo;
  int bth;


};


int not_boundaries(int width_max, int height_max, int x, int y);

#endif // DEF_ANIMAL
