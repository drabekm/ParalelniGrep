// xdrabe08 Matěj Drábek

#include <stdio.h>
#include<unistd.h>
#include<thread>
#include<queue>
#include<mutex>
#include<vector>
#include<iostream>
#include<string.h>
#include<regex>



std::vector<std::mutex *> zamky; /* pole zamku promenne velikosti */

int numberOfRegex; //Pocet zpracovavanych regexu
int lineScore; // Skore soucasneho radku
int finishedThreads = 0; //Pocet threadu co dokoncili praci
bool quit = false; //pouzito pro ukonceni threadu
std::mutex lineScoreLock; //Mutex pro povoleni zvysovani skore radku 
std::mutex finishedLock; //Mutex pro povoleni zvysovani poctu dokoncenych threadu

char *line; // Analyzovany radek

char *read_line(int *res)
	{
	std::string line;
	char *str;
	if (std::getline(std::cin, line)) 
	{
		str=(char *) malloc(sizeof(char)*(line.length()+1));
		strcpy(str,line.c_str());
		*res=1;
		return str;
	} else 
	{
		*res=0;
		return NULL;
	}
}


void f(int ID, int scoreToAdd, char* regex)
{
	printf("Thread %i started\n",ID);
	while(true)
	{
		(*(zamky[ID])).lock(); //Cekani na odemknuti
		if(quit) 
		{
			break;
		}
		std::regex re(regex);
		if(std::regex_match(line, re)) // Spusteni regexu
		{
			lineScoreLock.lock(); //Inkrementace skore
			lineScore += scoreToAdd;
			lineScoreLock.unlock();
		}
		
		finishedLock.lock(); //Inkrementace poctu dokoncenych threadu
		finishedThreads++;
		finishedLock.unlock();
	}
}

int main(int argc, char **argv) {

	

	int minScore;
	std::vector<int> regexScore; 
	std::vector<char *> regexes; 
	int numberOfPairs; // Pocet paru regex-regexScore

	//Zpracovani argumentu
	if(argc < 4 || argc % 2 == 1) //Zachyceni spatne zadanych argumentu
	{
		printf("Spatne zadane argumenty");
		return -1;
	}
	else // Zpracovani dobre zadanych argumentu
	{
		char *bordel;
		minScore = strtod(argv[1], &bordel); 
		numberOfPairs = (argc -1) / 2;

		regexScore.resize(numberOfPairs);
		regexes.resize(numberOfPairs);
		int helpIndex = 0;
		for(int i = 2; i < argc; i++)
		{
			if(i % 2 == 0) //Nacita se regex vyraz
			{
				regexes[helpIndex] = argv[i];
			}
			else // Nacita se skore pro regex
			{
				regexScore[helpIndex] = strtod(argv[i], &bordel);
				helpIndex++; //Po nacteni skore se bude ukladat novy regex. Tak si posunu pointer
			}
		}
	} //Zpracovavani argumentu je hotovo


	/*******************************
	 * Inicializace threadu a zamku
	 * *****************************/
	int num = numberOfPairs; 
	int num_zamky = numberOfPairs; 

	std::vector <std::thread *> threads; /* pole threadu promenne velikosti */

	/* vytvorime zamky */
	zamky.resize(num_zamky); /* nastavime si velikost pole zamky */
	for(int i=0;i<num_zamky;i++){	
		std::mutex *new_zamek = new std::mutex();
		zamky[i]=new_zamek;
		/* Pokud je treba, tak vytvoreny zamek muzeme rovnou zamknout */
		(*(zamky[i])).lock();
	}

	/* vytvorime thready */
	threads.resize(num); /* nastavime si velikost pole threads */
	for(int i=0;i<num;i++){	
		std::thread *new_thread = new std::thread (f,i, regexScore[i], regexes[i]);
		threads[i]=new_thread;
	}
	/**********************************
	 * Vlastni vypocet pgrep
	 * ********************************/
	int res;
	line=read_line(&res);
	while (res) {

		for(int i = 0; i < numberOfPairs; i++)
		{
			(*(zamky[i])).unlock();
		}

		while(finishedThreads != numberOfPairs) //Cekani nez dokonci thready svoji praci
		{
			//Nuffin
		}

		if(lineScore >= minScore)
		{
			printf("%s\n", line);
		}
		finishedThreads = 0;
		lineScore = 0;
		free(line); /* uvolnim pamet */
		line=read_line(&res);
	}
	
	quit = true; //Promena pro ukonceni cinosti threadu
	for(int i = 0; i < numberOfPairs; i++) //Posledni spusteni threadu kdy se ukonci jejich cinost
	{
		(*(zamky[i])).unlock();
	}
	
	/**********************************
	 * Uvolneni pameti
	 * ********************************/

	/* provedeme join a uvolnime pamet threads */
	for(int i=0;i<num;i++){
		(*(threads[i])).join();
		delete threads[i];
	}
	/* uvolnime pamet zamku */
	for(int i=0;i<num_zamky;i++){
		delete zamky[i];
	}

	printf("everything finished\n");
	
}
