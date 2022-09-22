
#define BILLION 1000000000.;

// player counts for each sport : baseball football and soccer respectively
// this order is used throughout the file
// playerCounts is the number of players and playerMaxCounts is the maximum number of players that can be on the field
int playerCounts[] = {36,44,44},playerMaxCounts[] = {18,22,22};
// ()waitTimes stores the times each player has been waiting to go on the field
// if the player is on the field their wait times should be close to zero
// ()InitTimes stores the time the player was last on the field
double baseballWaitTimes[playerCounts[0]],footballWaitTimes[playerCounts[1]],soccerWaitTimes[playerCounts[2]];
struct timespec baseballInitTimes[playerCounts[0]],footballInitTimes[playerCounts[1]],soccerInitTimes[playerCounts[2]];

// threads representing each player
pthread_t baseballPlayers[playerCounts[0]];
pthread_t footballPlayers[playerCounts[1]];
pthread_t soccerPlayers[playerCounts[2]];
// conditional threads representing each player
pthread_cond_t baseballPlayersCond[playerCounts[0]];
pthread_cond_t footballPlayersCondplayerCounts[1]];
pthread_cond_t soccerPlayersCond[playerCounts[2]];

// lock used for field assignments
// any player has the possiblility of using it, so it is not centralized
bool midGame = false;
pthread_mutex_t midGameMutex;

int main(void) {
	
	//reads seed
	int seed = readSeed();
	
	// game time for baseball and football selected as a random value between 2 and 7, for soccer 5 and 9
	int i = 0,j = 0,playerCount,number = 0,gameTime = 2 + (rand() % 6),soccerTime = 5 + (rand() % 5);
	
	resetAllWaitTimes();
	pthread_mutex_init(&midGameMutex,NULL);
	 
	// creates player threads
	for(i = 0;i < 3;i++) {
		playerCount = playerCounts[i];
		for(j = 0;j < playerCount;j++) {
			pthread.create(&baseballPlayers[j],NULL,Player(i,j + 1,(i < 2) ? gameTime : soccerTime),NULL); 
		}
	}
	
	return 0;
}

void *Player(int sport,int number,int gameTime) {

	// prep time to get on field
	int prepTime = 5 + (rand() % 5);
	sleep(prepTime);
	
	// whichever thread reaches "finished" first will do initial field assignments,everything else sent to waiting state
	bool playing = false;
	bool goingToPlay = false;
	bool finished = true;
	bool waiting = false;
	char team[256];
	
	int j = number - 1,playerCount = playerCounts[sport];
	strcpy(team,(j < (playerCount/2)) ? ((j < playerCount/4) ? "Weinstock" : "Engling") : ((j < 3*playerCount/4) ? "Servatius" : "Shue"));
	
	// records time the player started playing
	struct timpespec playerStart;
	
	while(true) {
		if(playing) {
			// constant resetting of wait time while playing
			while(elapsedTime(playerStart) < gameTime) {
				resetWaitTime(0,number);
			}
			playing = false;
			finished = true;
		} else if(goingToPlay) {
			playing = true;
			int status = clock_gettime(CLOCK_REALTIME,&playStart);
			if(status == -1) {
				perror("Clock Gettime");
				exit(EXIT_FAILURE);
			}
			goingToPlay = false;
		} else if(finished) {
			if(midGame) {
				// another thread reached finished first, this one will be sent to waiting
				waiting = true;
				finished = false;
			} else {
				// this thread reached finished first, will do assignments
				midGame = true;
				pthread_mutex_lock(midGameMutex);
				// selects next sport to send on the field
				int nextSport = nextSport();
				// selects players of given sport to go on field next
				int nextPlayers[] = nextPlayers(nextSport),i = 0,fieldMax = playerMaxCounts[1];
				bool selfSelect = false;
				
				// field reassignments
				for(;i < fieldMax;i++) {
					if(i < sizeof(nextPlayers)) {
						int playerNumber = nextPlayers[i];
						// case if this thread selects itself to play next
						bool currSelfSelect = ((sport == nextSport) && (playerNumber == (number - 1)));
						selfSelect |= currSelfSelect;
						if(currSelfSelect) {
							// send no signals if it's to itself
						} else {
							// sends a signal to the appropiate player to enter the field 
							switch(nextSport) {
								case 0:
									pthread_cond_signal(&baseballPlayersCond[playerNumber]); break;
								case 1:
									pthread_cond_signal(&footballPlayersCond[playerNumber]); break;
								case 2:
									// soccer has to send 2 players at a time
									int soccerIndex = 2*i;
									if(soccerIndex < sizeof(nextPlayers)) {
										pthread_cond_signal(&soccerPlayersCond[nextPlayers[soccerIndex]); 
									}
									if((soccerIndex + 1) < sizeof(nextPlayers)) {
										pthread_cond_signal(&soccerPlayersCond[nextPlayers[soccerIndex + 1]); 
									}
									break;
								default:
									printf("Unknown Error"\n);
									return;
							}
						}
					} else {
						// unsure what to do here
					}
				}
				
				finished = false;
				midGame = false;
				// if this current player is not going on a field, enter waiting state
				// otherwise get ready to play
				if(selfSelect) {
					waiting = true;
				} else {
					goingToPlay = true;
				}	
				pthread_mutex_unlock(midGameMutex);
			}
		} else if(waiting) {
			// wait until the thread doing field reassignments sends the signal to enter the field
			switch(sport) {
				case 0:
					pthread_cond_wait(&baseballPlayersCond[number - 1]);
				case 1:
					pthread_cond_wait(&footballPlayersCond[number - 1]);
				case 2:
					pthread_cond_wait(&soccerPlayersCond[number - 1]);
				default:
					printf("Unknown Error"\n);
					return;
			}
			goingToPlay = true;
			waiting = false;
		}
	}
}


// helper methods

// returns elapsed time in seconds
double elapsedTime(struct timespec initTime) {
	struct timespec finalTime;	
	int status = clock_gettime(CLOCK_REALTIME,&finalTime);
	if(status == -1) {
		perror("Clock Gettime");
		exit(EXIT_FAILURE);
	}
	return (finalTime.tv_sec - initTime.tv_sec) + (((finalTime.tv_nsec - initTime.tv_nsec) + 0.0)/ BILLION);
}

/* 0 for Baseball
   1 for Football
   2 for Soccer */
   
// resets waiting time for the specific player with the given sport and number
void resetWaitTime(int sport,int number) {
	struct timespec initTime;
	int status = 0,j = number - 1;
	status = clock_gettime(CLOCK_REALTIME,initTime); break;
	if(status == -1) {
		perror("Clock Gettime");
		exit(EXIT_FAILURE);
	}
	
	switch(sport) {
		case 0:
			baseballInitTimes[j] = initTime; return;
		case 1:
			footballInitTimes[j] = initTime; return;
		default:
			soccerInitTimes[j] = initTime; return;
	}
}

// resets waiting time for all the players in the given sport
void resetWaitTimes(int sport) {
	int playerCount = playerCounts[sport],i = 0;
	for(;i < playerCount;i++) {
		resetWaitTime(sport,i);
	}
}

// reset wait times of all players
void resetAllWaitTimes() {
	int i = 0;
	for(;i < 3;i++) {
		resetWaitTimes(i);
	}
}

// calculates waiting time for the specific player with the given sport and number
void calculateWaitTime(int sport,int number) {
	switch(sport) {
		case 0:
			baseballWaitTimes[number] = elapsedTime(baseballInitTimes[number]); return;
		case 1:
			footballWaitTimes[number] = elapsedTime(footballInitTimes[number]); return;
		default:
			soccerWaitTimes[number] = elapsedTime(soccerInitTimes[number]); return;
	}
}

// calculates waiting time for all the players in the given sport
void calculateWaitTimes(int sport) {
	int playerCount = playerCounts[sport],i = 0;
	for(;i < playerCount;i++) {
		calculateWaitTime(sport,i);
	}
}

// calculates wait times of all players
void calculateAllWaitTimes() {
	int i = 0;
	for(;i < 3;i++) {
		calculateWaitTimes(i);
	}
}

// returns maximum of given array
double[] max(double arr[]) {
	double ans[] = {INT_MIN,-1};
	int i = 0;
	for(;i < sizeof(arr);i++) {
		int get = arr[i];
		if(get > ans[0]) {
			ans[0] = get;
			ans[1] = i;
		}
	}
	return ans;
}

// returns first index of given value in the array
int indexOf(double[] arr,double value) {
	double margin = 0.001;
	for(int i = 0;i < sizeof(arr);i++) {
		if(comp(arr[i],value,margin)) {
			return i;
		}
	}
	return -1;
}

// compares two doubles, returns false if their difference is greater than (the) margin (of error)
bool comp(double d1,double d2,double margin) {
	double diff = d2 - d1;	
	return (((diff > 0) && (diff < margin)) || ((diff < 0) && (diff > (0.0 - margin))));
}

// determines which player has been waiting the longest and sets the sport of that player as the sport to be played next
// this eliminates the problem of starvation
int nextSport() {
	calculateAllWaitTimes();
	double maxWaitTimes[] = {max(baseballWaitTimes)[0],max(footballWaitTimes)[0],max(soccerWaitTimes)[0]};
	return max(maxWaitTimes)[1];
}

// selects the next players of a sport by determining the players who have been waiting the longest
// also helps eliminate starvation
int[] nextPlayers(int sport) {
	
	int selection[playerMaxCounts[sport]],i = 0;
	double[] waitTimeCopy;
	switch(sport) {
		case 0:
			waitTimeCopy = baseballWaitTimes; break;
		case 1:
			waitTimeCopy = footballWaitTimes; break;
		default:
			waitTimeCopy = soccerWaitTimes; break;
	}
	for(;i < sizeof(selection);i++) {
		double[] max = max(waitTimeCopy);
		selection[i] = max[1];
		waitTimeCopy[selection[i]] = -1;
	}
	return selection;
}

// reads seed value
int readSeed() {

	FILE *f;
	char buff[255];

	f = fopen("seed.txt","r");
	fscanf(f,"%s",buff);
	fclose(f);

	printf("Read seed value : %s\n\n",buff);	

	int seed = atoi(buff);
	printf("Read seed value(converted to integer): %d\n",seed);
	
	return seed;
	
}
