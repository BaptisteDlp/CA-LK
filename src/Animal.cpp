/*!
 * \file Animal.cpp
 * \brief Definition of Animal class
 * \author Baptiste D.
 * \version 1.0
 */



#include<cstdlib>
#include<iostream>
#include<fstream>
#include<SDL/SDL.h>
#include<string>
#include<cassert>
#include<stdio.h>
#include<vector>
#include<algorithm>
#include"Animal.hpp"


using namespace std;



Animal::Animal(){

  id = PREY;
  index = 0;
  position.x = 0;
  position.y = 0;
  health = 10;
  age = 0;
  bth = -1;
  fecondity = 10;
  repro_yo = 0;
  count_prey++;
  max_yo = MAX_YO_PREY;

}


Animal::Animal(int x, int y, int identity, int i){
  id = identity;
  index = i;
  position.x = x;
  position.y = y;
  health = 20;
  age = 0;
  bth = -1;
  repro_yo = 0;
  switch(id){
    case PREY:
    count_prey++;
    max_yo = MAX_YO_PREY;
    fecondity = FECONDITY_PREY;
    break;
    case PREDATOR:
    count_predator++;
    max_yo = MAX_YO_PREDATOR;
    fecondity = FECONDITY_PREDATOR;
    break;
  }
}


Animal::Animal(Animal const& a){

  id = a.id;
  index = a.index;
  position.x = a.position.x;
  position.y = a.position.y;
  health = a.health;
  age = a.age;
  bth = a.bth;
  fecondity = a.fecondity;
  repro_yo = a.repro_yo;
  max_yo = a.max_yo;
  switch(id){
    case PREY:
    count_prey++;
    break;
    case PREDATOR:
    count_predator++;
    break;
  }
}

Animal::~Animal(){
  switch(id){
    case PREY:
    count_prey--;
    break;
    case PREDATOR:
    count_predator--;
    break;
  }
}

int Animal::get_count_prey() const{return count_prey;}
int Animal::get_count_predator() const{return count_predator;}
int Animal::get_position_x() const{return position.x;}
int Animal::get_position_y() const{return position.y;}
int Animal::get_yo() const{return age;}
int Animal::get_max_yo() const{return max_yo;}
int Animal::get_index() const{return index;}
int Animal::get_id() const{return id;}





int Animal::rules(Map &map){

    vector<int> cases;
    int pos[4] = {1,1,1,1};


    age++;

    if(!not_boundaries(map.getgridW(),map.getgridH(),position.x,position.y)){
      if(position.y == 0) pos[UP] = -1;
      if(position.y == map.getgridH()-1) pos[DOWN] = -1;
      if(position.x == 0) pos[LEFT] = -1;
      if(position.x == map.getgridW()-1) pos[RIGHT] = -1;
    }


    if(map.getgrid(position.x,position.y-1) == id) pos[UP] = -1;
    if(map.getgrid(position.x+1,position.y) == id) pos[RIGHT] = -1;
    if(map.getgrid(position.x,position.y+1) == id) pos[DOWN] = -1;
    if(map.getgrid(position.x-1,position.y) == id) pos[LEFT] = -1;

    if(id==PREY){
      if(map.getgrid(position.x,position.y-1) == PREDATOR) pos[UP] = -1;
      if(map.getgrid(position.x+1,position.y) == PREDATOR) pos[RIGHT] = -1;
      if(map.getgrid(position.x,position.y+1) == PREDATOR) pos[DOWN] = -1;
      if(map.getgrid(position.x-1,position.y) == PREDATOR) pos[LEFT] = -1;
    }


    //new position on boundary and surrounding animals are under control

    for(int i = 0; i<4; i++){
      if(pos[i] == 1){
        cases.push_back(i);
      }
    }




    if(cases.size()==0){
      //STATIC
      age = max_yo;
      return -1;
    }else{
      map.setgrid_who_there(position.x,position.y,EMPTY,-1);
      int direction(-1);
      int prey_dead(-1);
      switch(id){
        case PREY:
        direction = rand() % cases.size();
        direction = cases[direction];
        break;

        case PREDATOR:
        vector<int> prey_cases;
        for(int i=0; i<cases.size(); i++){
          switch(cases[i]){
            case UP:
            if(map.getgrid(position.x,position.y-1) == PREY) prey_cases.push_back(UP);
            break;

            case RIGHT:
            if(map.getgrid(position.x+1,position.y) == PREY) prey_cases.push_back(RIGHT);
            break;

            case DOWN:
            if(map.getgrid(position.x,position.y+1) == PREY) prey_cases.push_back(DOWN);
            break;

            case LEFT:
            if(map.getgrid(position.x-1,position.y) == PREY) prey_cases.push_back(LEFT);
            break;
          }
        }
        //position of potential surrounding preys are known

        if(prey_cases.size() == 0){
          direction = rand() % cases.size();
          direction = cases[direction];
          health--;
          if(health==0){
            age = max_yo;
          }
        }else{
          prey_dead = 1;
          direction = rand() % prey_cases.size();
          direction = prey_cases[direction];
          if(health<20) health = 20;
        }
        break;
      }

      switch(direction){
        case UP:
        position.y--;
        break;
        case RIGHT:
        position.x++;
        break;
        case DOWN:
        position.y++;
        break;
        case LEFT:
        position.x--;
        break;
      }

      if(prey_dead == 1 && age != max_yo){
        prey_dead = map.getgrid_who_there(position.x,position.y);
        map.setgrid_who_there(position.x,position.y,id,index);
        return prey_dead;
      }else{
        if(age == max_yo){
          map.setgrid_who_there(position.x,position.y,EMPTY,-1);
        }else{
          map.setgrid_who_there(position.x,position.y,id,index);
        }

        return -1;
      }
    }
}


void Animal::birth(Map &map, vector<Animal> &group){

  if(health > 9){
    if(age%fecondity==0){
      bth = rand() % 10;
      repro_yo = age+bth;
    }
    if(bth != -1 && age == repro_yo){
      vector<int> cases;
      int pos[4] = {1,1,1,1};
      if(!not_boundaries(map.getgridW(),map.getgridH(),position.x,position.y)){
        if(position.y == 0) pos[UP] = -1;
        if(position.y == map.getgridH()-1) pos[DOWN] = -1;
        if(position.x == 0) pos[LEFT] = -1;
        if(position.x == map.getgridW()-1) pos[RIGHT] = -1;
      }


      if(map.getgrid(position.x,position.y-1) != EMPTY) pos[UP] = -1;
      if(map.getgrid(position.x+1,position.y) != EMPTY) pos[RIGHT] = -1;
      if(map.getgrid(position.x,position.y+1) != EMPTY) pos[DOWN] = -1;
      if(map.getgrid(position.x-1,position.y) != EMPTY) pos[LEFT] = -1;

      //new position on boundary and surrounding animals are under control

      for(int i = 0; i<4; i++){
        if(pos[i] == 1){
          cases.push_back(i);
        }
      }

      if(cases.size()!=0){
        int direction = rand() % cases.size();
        direction = cases[direction];
        Position pos;
        pos.x = position.x;  pos.y = position.y;
        switch(direction){
          case UP:
          pos.y--;
          break;
          case RIGHT:
          pos.x++;
          break;
          case DOWN:
          pos.y++;
          break;
          case LEFT:
          pos.x--;
          break;
        }

        int ind(-1);
        switch(id){
          case PREY:
          ind = count_prey_tot++;
          break;
          case PREDATOR:
          ind = count_predator_tot++;
          break;
        }
        Animal a(pos.x,pos.y,id,ind);
        group.push_back(a);
        map.setgrid_who_there(a.get_position_x(),a.get_position_y(),
        a.get_id(),a.get_index());
      }
      bth = -1;
    }
  }
}







int not_boundaries(int width_max, int height_max, int x, int y){

  if(x > 0  && x < (width_max-1) && y > 0 && y < (height_max-1)){
    return 1;
  }else{
    return 0;
  }

}




int Animal::update_position(Map &map) const{
    map.setgrid_who_there(position.x,position.y,id,index);

}


void Animal::clear_position(Map &map) const{
  map.setgrid_who_there(position.x,position.y,EMPTY,-1);

}
