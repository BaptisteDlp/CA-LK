/**
 * \file main.cpp
 * \brief Runs Preys_Predators.
 * \author Baptiste D.
 * \version 1.0
 * \date January 29th 2022
 *
 * Program that runs the Game
 *
 */




#include<iostream>
#include<fstream>
#include<ctime>
#include<cassert>
#include<vector>
#include<string>
#include <algorithm>
#include<stdio.h>
#include<stdlib.h>
#include<SDL/SDL.h>
#include<SDL/SDL_image.h>
#include"Map.hpp"
#include"Input.hpp"
#include"const.hpp"
#include"Animal.hpp"
#include"Text.hpp"


#define SPEED 100

using namespace std;



void init_map(Map &map, vector<Animal> &preys, vector<Animal> &predators,int nb_preys, int nb_predators){


  for(int k = 0; k<nb_preys; k++){
    int x(-1), y(-1);

    do{
      x = rand() % map.getgridW();
      y = rand() % map.getgridH();
    }while(map.getgrid(x,y) != EMPTY);

    Animal::count_prey_tot++;
    Animal prey(x,y,PREY,k);
    preys.push_back(prey);
    map.setgrid_who_there(x,y,prey.get_id(),k);
  }

  for(int k = 0; k<nb_predators; k++){
    int x(-1), y(-1);

    do{
      x = rand() % map.getgridW();
      y = rand() % map.getgridH();
    }while(map.getgrid(x,y) != EMPTY);

    Animal::count_predator_tot++;
    Animal predator(x,y,PREDATOR,k);
    predators.push_back(predator);
    map.setgrid_who_there(x,y,predator.get_id(),k);
  }

}


void debug(vector<Animal>  pr){

  vector<Animal>::iterator p;
  int i(0);
  for(p = pr.begin(); p != pr.end(); p++){
    cout<<"P"<<i++<<" pos "<<(*p).get_position_x()<<" "<<(*p).get_position_y()<<" id "<<(*p).get_id()<<" index "<<(*p).get_index()<<endl;
  }
  cout<<endl;
}


void rule_pred(Map &map, vector<Animal> &predators, vector<Animal> &preys){

  int index_prey_dead(-1);


  vector<Animal>  new_born_predator;
  vector<Animal>::iterator p = predators.begin();
   while(p != predators.end()){
    index_prey_dead = (*p).rules(map);
    (*p).birth(map,new_born_predator);

    if(index_prey_dead != -1){
      vector<Animal>::iterator  it = preys.begin();
      while((*it).get_index() != index_prey_dead) it++;
      preys.erase(it);
      p++;
    }else{
      if((*p).get_yo() == MAX_YO_PREDATOR){
        vector<Animal>::iterator it = p;

        map.setgrid_who_there((*it).get_position_x(),(*it).get_position_y(),
        EMPTY,-1);

        predators.erase(it);
      }else{
        p++;
      }
    }
  }

  if(new_born_predator.size()!=0){
    predators.insert(predators.end(),new_born_predator.begin(),new_born_predator.end());
  }

  vector<Animal>  new_born_preys;
  p = preys.begin();
  while(p != preys.end()){
   (*p).rules(map);
   (*p).birth(map,new_born_preys);
    if((*p).get_yo() == MAX_YO_PREY){
       vector<Animal>::iterator it = p;
       map.setgrid_who_there((*it).get_position_x(),(*it).get_position_y(),
       EMPTY,-1);
       preys.erase(it);
     }else{
       p++;
     }
   }

   if(new_born_preys.size()!=0){
     preys.insert(preys.end(),new_born_preys.begin(),new_born_preys.end());
   }



}




/*************Processing data*/

void compute_density(vector<int> &dat){

  dat.push_back(Animal::count_prey);
  dat.push_back(Animal::count_predator);
}


void post_processing(vector<int> const& dat, int nbcells){

  ofstream stream("data/data1.txt");
  ofstream stream2("data/data2.txt");

  for(int k=0; k<int(dat.size()/2); k++){
    stream<<double(SPEED*k)/1000<<" "<<double(dat[2*k])/nbcells<<" "
    <<double(dat[2*k+1])/nbcells<<endl;
    stream2<<double(SPEED*k)/1000<<" "<<double(dat[2*k])<<" "
    <<double(dat[2*k+1])<<endl;
  }

  stream.close();
  stream2.close();

  system("gnuplot data/script.gnu -persist");
  system("gnuplot data/script2.gnu -persist");
}

void processing(vector<int> const& dat, int nbcells, int k){

  ofstream stream("data/data1.txt",ios::app);
  ofstream stream2("data/data2.txt",ios::app);

  stream<<double(SPEED*k)/1000<<" "<<double(dat[2*k])/nbcells<<" "
  <<double(dat[2*k+1])/nbcells<<endl;

  stream2<<double(SPEED*k)/1000<<" "<<double(dat[2*k])<<" "
  <<double(dat[2*k+1])<<endl;


  stream.close();
  stream2.close();

}




//==============================================================================
//================================MAIN==========================================
//==============================================================================
int Animal::count_prey = 0;
int Animal::count_predator = 0;
int Animal::count_prey_tot = 0;
int Animal::count_predator_tot = 0;


int main(int argc, char *argv[]){

    assert(argc == 4);
    srand(time(0));
    TTF_Init();


    SDL_Surface *screen = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    if(SDL_Init(SDL_INIT_VIDEO) == -1){
        fprintf(stderr,"ERROR INITIALISATION SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    Map map(argv[1]); //Map loading

    screen = SDL_SetVideoMode(map.getgridW() * map.getwidthTile(),
     map.getgridH() * map.getheigthTile(),32, SDL_HWSURFACE| SDL_DOUBLEBUF);
    if(screen == NULL){
        fprintf(stderr,"CANNOT LOAD SCREEN: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_WM_SetCaption("Preys_Predators", NULL);


    int time(0), previousTime(0);
    Input in;



    vector<Animal> preys;
    vector<Animal> predators;

    init_map(map,preys,predators,atoi(argv[2]),atoi(argv[3]));
    map.print_Map(screen);


    TTF_Font *police = TTF_OpenFont("data/GOTHIC.TTF",30);
    char tc1[15]; sprintf(tc1,"preys: %d",Animal::count_prey);
    char tc2[15];  sprintf(tc2,"predator: %d",Animal::count_predator);
    Text t_count_prey(tc1,police,0,0,0,true,0,screen->h-80);
    Text t_count_predator(tc2,police,0,0,0,true,0,screen->h-40);

    t_count_prey.blit_Text(screen,false);
    t_count_predator.blit_Text(screen,false);

    vector<int> dat;

    compute_density(dat);
    int k = 0;
    processing(dat,map.getgridW()*map.getgridH(),k);
    system("gnuplot data/script1.gnu &");
    system("gnuplot data/script2.gnu &");

    SDL_Flip(screen);


    while(!(in.getKey(SDLK_ESCAPE)) && !(in.getQuit()) && Animal::count_prey > 0 && Animal::count_predator > 0){
        in.update_event();
        time = SDL_GetTicks();
        if(time - previousTime > SPEED){
          k++;
          rule_pred(map,predators,preys);

          sprintf(tc1,"preys: %d",Animal::count_prey);
          sprintf(tc2,"predators: %d",Animal::count_predator);
          t_count_prey.update_Message(tc1);t_count_predator.update_Message(tc2);
          SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format, 0, 0, 0));
          map.print_Map(screen);
          t_count_prey.blit_Text(screen,false);
          t_count_predator.blit_Text(screen,false);
          SDL_Flip(screen);

          compute_density(dat);
          processing(dat,map.getgridW()*map.getgridH(),k);
          previousTime = time;
        }
    }

    TTF_CloseFont(police);
    TTF_Quit();
    SDL_Quit();

    system("rm data/data1.txt data/data2.txt");


    return EXIT_SUCCESS;

}
