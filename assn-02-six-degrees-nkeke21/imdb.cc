using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"
#include <stdlib.h>

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}


/*
* cmpfunc compare two string to each other and returns int.
* Returned int helps binary search where to go on the next step.
*/
int imdb::cmpfunc(const void* info, const void* ptr){
	
	//Creating new actor struct, which is saving information about actorFile and name of actor
	actors curr = *(actors*)info; 
	int len = curr.actor.size();
	
	//Pointer where we start reading string
	char* start = (char*)curr.actorFile + *(int*)ptr; 
	for(int i = 0; i < len; i ++){
		if(*start == '\0'){ //Our actor's name is longer than the name of current actor in the array
			return 1;
		} 
		if(*start > curr.actor[i]){ //Curr character of the name of our actor is less.
			return -1;
		} else if(*start < curr.actor[i]){ //Curr character of the name of our actor is more.
			return 1;
		}
		start ++;
	}
	if(*start == '\0'){ //Found the our actor's name in the array
		return 0;
	}
	return -1;
}

/*
* comfun compare two films to each other and returns int.
* Returned int helps binary search where to go on the next step.
*/
int imdb::cmpfun(const void* infoM, const void* ptrM){

	//Creating struct(movies) to save information about movieFile and and the film itself.
	movies curr = *(movies*)infoM; 
	char* start  = (char*)curr.movieFile + *(int*)ptrM; //Moving to the pointer where we start reading.
	string st = "";
	
	while(*start != '\0'){
		st += *start;
		start ++;
	}
	
	//Move to the next byte to get information about the date of the film
	start ++; 
	int year = (int)1900 + *start;
	
	//Creating film struct to save information and compare to films' structs to each other.
	film newM; 
	newM.title = st;
	newM.year = year;
	
	//Returning result
	if(newM == curr.films){ 
		return 0;
	} else if(curr.films < newM){
		return -1;
	} 
	return 1;
}


bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

/*
* getCredits return the information if the player is in the list of actors' 
* names. If we find player in the list  we fill the information
* in the vector with the films title and will return true, else false.
*/
bool imdb::getCredits(const string& player, vector<film>& films) const { 
	
	//Creating new struct to save information about the player
	actors curr; 
	curr.actorFile = actorFile;
	curr.actor = player;
	
	//Item pointer return NULL if there is not player in the list, else adress.
	void* item = bsearch(&curr, (int*)actorFile + 1, *(int*)actorFile, sizeof(int), cmpfunc);
	
	//If we found player in the list
	if(item != NULL){
	
		char* ptr = (char*)actorFile + *(int*)item; //adress to the actor named player
		int len = player.size()+1;  //I create int variable because i need to count bytes from the ptr;
		ptr+= player.size();        //We already know player's name so move on to the next byte.
		
		if(player.size() % 2 == 0){ //If length of player's name is even
			ptr++;
			len ++;
		}
		
		
		ptr ++; 
		short amount = *(short*)ptr; //We are standing on the adress which saves information about in how many films the player participated
		
		if((len + 2)%4 == 0){
			ptr = (char*)ptr;
			ptr += 2;
		}
		
		for(int i = 0; i < amount; i ++){ //We know in amount of films player participated, now we want to save their titles and dates.
			int num = *(int*)ptr;     //offset between adresses are 4 bytes(sizeof(int))
			
			string movie = "";
			char* starter = (char*)movieFile + num;
			while(*starter != '\0'){
				movie +=  *starter; //Filling the string
				starter++;
			}
			
			starter += 1;   //Moving to the next byte to save information about the date year of the film
			film newFilm;   //Film struct, saving all information about film
			newFilm.title = movie;
			newFilm.year = (int)1900 + *starter;
			
			films.push_back(newFilm); //Adding new information to the vector
			ptr += sizeof(int);       //Moving forward by 4bytes
		}
		return true;
	}
	return false;
}

/*
* function: getCast returns the true if there is film with the title of movie.title.
* If we find this film, we should save information about the cast of this film int the
* vector of players.
*/
bool imdb::getCast(const film& movie, vector<string>& players) const { 
	
	//Creating struct of type movies to save information about the movieFile and about the film(movie)
	movies curr;
	curr.movieFile = movieFile;
	curr.films.title = movie.title;
	curr.films.year = movie.year;
	
	//Item pointer return NULL if there is not film(movie) in the list, else adress.
	void* item = bsearch(&curr, (int*)movieFile + 1, *(int*)movieFile, sizeof(int), cmpfun);
	
	if(item != NULL){ //Found the film in the list
		char* ptr = (char*)movieFile + *(int*)item; //Adress to the film title movie.title
		ptr += movie.title.size(); //We need not to read the whole name of film because we already know it's name
		int len = movie.title.size()+1; //I create int variable because i need to count bytes from the ptr;
		ptr += 1; //We are standing on '\0'
		len ++;
		ptr += 1; //We are standing on the byte pointing to the date of the film
		if((movie.title.size()+2)%2 == 1){
			ptr+=1;
			len++;
		}
		short amount = *(short*)ptr; //Amount of people casting in the film
		if((len + 2)% 4 == 0){
			ptr = (char*)ptr;
			ptr += 2;
		}
		
		//We already know how many people are participating in the film so we need to save their names in the vector
		for(int i = 0; i < amount; i ++){
			int num = *(int*)ptr;//offset between adresses are 4 bytes(sizeof(int))
			string actor = "";
			char* starter = (char*)actorFile + num;
			
			while(*starter != '\0'){
				actor += *starter; 
				starter++;
			}
			players.push_back(actor); //Adding the actor in the vector
			ptr += sizeof(int);
		}
		return true;
	}
	
	return false; 
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
