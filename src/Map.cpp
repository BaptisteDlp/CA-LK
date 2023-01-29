/*!
 * \file Map.cpp
 * \brief Definition of Map class
 * \author Baptiste D.
 * \version 1.1
 */

#include<cstdlib>
#include<iostream>
#include<fstream>
#include<string>
#include<cassert>
#include<SDL/SDL.h>
#include"Map.hpp"
#include"Animal.hpp"




using namespace std;




/*!
 *  \brief Constuctor
 *
 *  Constructor of class Map
 *
 *  \param fileName : name of the file that contains of the informations to load
    the map
 *  \param bgx : to load the beginning of the grid along the x-direction within
    the screen
 *  \param bgy : to load the beginning of the grid along the y-direction within
    the screen
 */
Map::Map(char* const fileName,int bgx, int bgy){


  ifstream stream(fileName);

  if(!stream){
    cout<<"ERROR OPENING "<<fileName<<endl; exit(1);
  }
  string line;
  int nbTile,num;

  for(int i = 0;i<3;i++){
    getline(stream,line);
  }
  //===========================A remanier pour charger le bon ficher============
  //line = "/home/baptiste/Bureau/GameOfLife/" + line;
  //char cstr[line.length()+1];
  //strcpy(cstr, line.c_str());
  //cout<<cstr<<endl;
  //tileMap = SDL_LoadBMP(cstr);
  tileMap = SDL_LoadBMP("data/pp4.bmp");
  if(tileMap == NULL){
      cout<<"ERROR OPENING "<<line<<endl; exit(1);
  }

  //============================================================================


  stream>>nbTileW;
  stream>>nbTileH;
  stream>>widthTile;
  stream>>heigthTile;

  beginGridx = widthTile * bgx;
  beginGridy = heigthTile * bgy;


  nbTile = nbTileW * nbTileH;
  tileProp = new Propreties[nbTile];

  for(int i = 0; i<nbTile; i++){
    stream>>num;

    stream>>num;

    if(num==1){
      tileProp[i].wall = true;
    }else{
      tileProp[i].wall = false;
    }
  }

  int n(0);
  for(int i = 0; i<nbTileW; i++){
    int width = i * widthTile;
    for(int j = 0; j<nbTileH; j++){
      tileProp[n].rectTile.w = widthTile;
      tileProp[n].rectTile.h = heigthTile;
      tileProp[n].rectTile.x = width;
      tileProp[n].rectTile.y = j*heigthTile;
      n++;
    }
  }

  getline(stream,line);
  getline(stream,line);
  stream>>gridW;
  stream>>gridH;

  grid = new Compo[gridW*gridH];


  for(int k = 0; k<gridW*gridH; k++){
    grid[k].tile = 0; grid[k].who_there = -1;
  }

  stream.close();


}












Map::Map(Map const& map){

  tileMap = map.gettileMap();
  widthTile = map.getwidthTile();
  heigthTile = map.getheigthTile();
  nbTileW = map.getnbTileW();
  nbTileH = map.getnbTileH();
  gridW = map.getgridW();
  gridH = map.getgridH();
  beginGridx = map.getbeginX();
  beginGridy = map.getbeginY();

  int nbTile = nbTileW * nbTileH;
  tileProp = new Propreties[nbTile];

  for(int i = 0;i<nbTile;i++){
    tileProp[i].wall = map.getwall(i);
  }

  int n(0);
  for(int i = 0; i<nbTileW; i++){
    int width = i * widthTile;
    for(int j = 0; j<nbTileH; j++){
      tileProp[n].rectTile.w = widthTile;
      tileProp[n].rectTile.h = heigthTile;
      tileProp[n].rectTile.x = width;
      tileProp[n].rectTile.y = j*heigthTile;
      n++;
    }
  }

  grid = new Compo[gridW*gridH];

  for(int i = 0; i<gridW; i++){
    for(int j = 0; j<gridH; j++){
      grid[j*gridW+i].tile = map.getgrid(i,j);
      grid[j*gridW+i].who_there = map.getgrid_who_there(i,j);
    }
  }


}




Map::~Map(){

  delete[] grid;
  delete[] tileProp;

  SDL_FreeSurface(tileMap);



}




void Map::clear_TileSetColorsKey(int red, int green, int blue){

  SDL_SetColorKey(tileMap, SDL_SRCCOLORKEY,
     SDL_MapRGB(tileMap->format, red, green, blue));

}






int Map::getwidthTile() const{ return widthTile;}
int Map::getheigthTile() const{ return heigthTile;}
int Map::getbeginX() const{ return beginGridx;};
int Map::getbeginY() const{ return beginGridy;};
int Map::getnbTileW() const{ return nbTileW;}
int Map::getnbTileH() const{ return nbTileH;}
int Map::getgridW() const{ return gridW;}
int Map::getgridH() const{ return gridH;}
bool Map::getwall(int i) const{ return tileProp[i].wall;}
int Map::getgrid(int i, int j) const{return grid[j*gridW+i].tile;}
int Map::getgrid_who_there(int i, int j) const{return grid[j*gridW+i].who_there;}
SDL_Surface* Map::gettileMap() const{return tileMap;}



void Map::setgrid_who_there(int i, int j, int id, int index){
  assert(i > -1 && j > -1 && i < gridW && j < gridH);
  assert(id >= 0);
  grid[j*gridW+i].tile = id;
  grid[j*gridW+i].who_there = index;
}

void Map::setgrid(int i, int j, int numTile){

  assert(i > -1 && j > -1 && i < gridW && j < gridH);
  assert(numTile >= 0 && numTile <= nbTileH*nbTileW-1);
  grid[j*gridW+i].tile = numTile;

}


void Map::set_all_grid(int *t){

  for(int i = 0; i < (*this).getgridW(); i++){
    for(int j = 0; j < (*this).getgridH(); j++){
      (*this).setgrid(i,j,t[j*(*this).getgridW()+i]);
      if(t[j*(*this).getgridW()+i]==EMPTY){
        (*this).setgrid_who_there(i,j,EMPTY,-1);
      }
    }
  }
}



void Map::setTileMap(char *const fileName, int colR, int colG, int colB){

  assert(colR >= 0 && colR <= 256);
  assert(colG >= 0 && colG <= 256);
  assert(colB >= 0 && colB <= 256);
  SDL_FreeSurface(tileMap);
  tileMap = SDL_LoadBMP(fileName);
  (*this).clear_TileSetColorsKey(colR,colG,colB);


}





/*!
 *  \brief Display of the map on the SDL screen
 *
 *  \param screen: main SDL surface where the map has to be printed on
 *  \return 1 to check if this operation have been successfully achevied
 */
int Map::print_Map(SDL_Surface *screen) const{

  SDL_Rect position;
  int numTile(0);


  for(int j = 0; j<gridH; j++){
    for(int i = 0; i<gridW; i++){
      position.x = i * widthTile + beginGridx;
      position.y = j * heigthTile + beginGridy;
      numTile = grid[j*gridW+i].tile;
      SDL_BlitSurface(tileMap, &tileProp[numTile].rectTile, screen, &position);
    }
  }
  return 1;

}



Map& Map::operator=(Map const& map){

  Map m(map);

  return m;

}
