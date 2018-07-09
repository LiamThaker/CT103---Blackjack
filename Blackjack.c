#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "stdafx.h"
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#pragma warning(disable: 4996)
 
//Structures & Prototypes
struct card
{ 
	int suit; 
	int no; 
}; 
struct user 
{ 
	char username[20];
	int chips; 
	float score; 
	int difficulty;
	struct card deck[105]; 
};
struct hiscores
{
	float score[10];
	char username[20][10];
};
struct savefile
{
	struct user cont;
	struct hiscores top10;
};
int dealeround(int bust, int insurance,struct card dhand[], int dcards, int userTotal, int cardno, int bet, int sidebet, int ddown);
int difficultyselect();
int saveUser(char *address, struct savefile info);
int loadUsers(char *address);
int deleteUser(char *address, struct savefile info);
int menu(int showX, int showY);
int loaddeck();
int handtotal(struct card hand[], int amount);
int hiscores(char *address, int print);
int jokercheck(struct card anycard, int cardNo);
int custgame();
int betting(int ddown, int bet);
int newHiscore(char *address);
FILE *openfile(char *address, char *mode);
void winner(int bust, int userTotal, int dealerTotal, int insurance, int bet, int sidebet, int ddown);
void sorthiscore(struct hiscores sorthiscore,struct savefile data, char *address, int limit);
void savdel(int todo, char *address);
void boxword(char string[]);
void toback(int amount);
void round();
void game(int loaded, char *address);
void printcard(struct card plain);
void newdeck(int decks);
void shuffle(int start, int decks);
void printhand(struct card hand[], int amount);
void delaystring(char string[20], int ms);
void hint(int total);
void calcscore(int bet, int ddown);

//PLAYER DATA IS GLOBAL
struct user player1; 

void main()
{
	int option, showcont, decks, num =0, showhiscores;
	//save address
	char *address = "sav.dat";
	do{
		//if they return 1, it shows them in the menu
		showhiscores = hiscores(address, 0);
		showcont = loadUsers(address);
		option = menu(showcont, showhiscores);
		switch(option)
		{
		case 1:
			//QUICK GAME
			newdeck(1);
			shuffle(1,1);
			//sets player data
			strcpy(player1.username,"");
			player1.difficulty = 2;
			player1.chips = 250;
			game(-1, address);
			break;
		case 2:
			//NEW GAME
			player1.score = 0;
			player1.difficulty = difficultyselect();
			//deck amount change with Difficulty
			if(player1.difficulty == 3)
			{
				decks = 2;
			}
			else
			{
				decks = 1;
			}
			newdeck(decks);
			shuffle(1,decks);
			game(0, address);
			break;
		case 3:
			//loads all player info into player1
			loadUsers(address);
			game(1, address);
			break;
		case 4:
			//creates custom playerdata
			decks = custgame();
			newdeck(decks);
			shuffle(1,decks);
			game(-1, address);
			break;
		case 5:
			//prints hiscores
			hiscores(address, 1);
			break;
		default:
			option = 0;
			break;
		}
	}while(option != 0);
}


//game that loops to round
void game(int loaded, char *address)
{
	char choice;
	int loop = 1;
	int save = 0;
	//loaded -1, means they picked quick game
	//loaded 1 means they loaded
	//loaded 0 means they click new game
	while(player1.chips > 0 && loop)
	{
		round();
		if(player1.chips > 0)
		{
			printf("   Would you like to quit? [Y/N] \n");
			do{
				choice = getch();
				if(tolower(choice) == 'y')
				{
					loop = 0;
					if(loaded != -1)
					{
						printf("    Save and Quit? [Y/N] \n");
						do{
							choice = getch();
							if(tolower(choice) == 'y')
							{
							savdel(1, address);
							save = 1;
							}
							if(tolower(choice == 'n'))
							{
								loop = 0;
							}
						}while(loop);
					}
					break;
				}
				if(tolower(choice) == 'n')
				{
					break;
				}
			}while(loop);
			printf("\n");
		}
	}
	if(player1.chips <= 0 && loaded == 1)
	{
		savdel(2, address);
		newHiscore(address);
	}
	if(save == 0 && loaded != 1)
	{
		newHiscore(address);
	}
	printf("\n");
}

//Takes cards used in a round & places them at the back of the deck
void toback(int amount)
{
	//amount is the amount of cards used in a round.
	int i;
	struct card last[52];
	for(i=0;i<amount;i++)
	{
		last[i] = player1.deck[i];
	}
	for(;i<54;i++)
	{
		player1.deck[i-amount] = player1.deck[i];
	}
	for(i=0;i<amount;i++)
	{
		player1.deck[54-amount+i] = last[i];
	}
}

//Users drawing
void round()
{
	//two of most variables because of a spilt
	//A split creates two hands for the user
	int cardno, insurance = 0, insurance2 = 0, quit =0 ,dbust = 0, i;
	int userTotal2 = 0, userTotal=0, dealerTotal=0, ddown =0, ddown2 = 0, pcards=0, dcards=0;
	int bet=0, bet2=0 ,sidebet = 0, sidebet2 = 0, split =0, bust = 0, bust2 = 1;
	int showoption = 1;
	char choice;
	struct card hand[10], dhand[10];

	bet = betting(0,0);//Gets starting bet
	player1.chips += -bet;//bet takes away from chips
	//Always checks if cardno 0, is a joker.
	cardno = 0;
	cardno = jokercheck(player1.deck[cardno], cardno);
	hand[0] = player1.deck[cardno];
	pcards++;//nextcard in playerhand
	cardno++;//nextcard in deck

	printf("\n      ");
	delaystring("Drawing . . .\n\n",100);

	cardno = jokercheck(player1.deck[cardno], cardno);
	dhand[0] = player1.deck[cardno];
	dcards++;//next card in dealerhand
	cardno++;
	cardno = jokercheck(player1.deck[cardno], cardno);
	hand[1] = player1.deck[cardno];

	pcards++;
	cardno++;
	dhand[1].no = 0; //printcard then makes it look like a blank card.
	dcards++;

	boxword("DEALER");
	printhand(dhand, dcards);
	handtotal(dhand, dcards);

	cardno = jokercheck(player1.deck[cardno], cardno);
	dhand[1] = player1.deck[cardno];
	cardno++;
	printf("\n");
	boxword(player1.username);
	printhand(hand, pcards);
	userTotal = handtotal(hand, pcards);//finds & prints usertotal
	if(userTotal >= 9 && userTotal <= 11)
	{
		//doubling down issues with difficulty
		ddown = 1;
		if(player1.difficulty != 1 && userTotal == 9)
		{
			ddown = 0;
		}
	}
	
	//keeps looping here until user breaks or goes bust
	while(!bust)
	{
			if(userTotal == 21)
			{
				//blackjack check
						delaystring(" \n", 200);
						boxword("BLACKJACK!");
						Beep(1500, 50); 
						Beep(1750, 60);
						Beep(2000, 80);
						delaystring(" \n", 200);
						if(split == 1)
						{
							//checks user has split
							split++;
							userTotal2 = userTotal;
							ddown2 = ddown;
							insurance2 = insurance;
							sidebet2 = sidebet;
							bet2 = bet;
							bust2 = bust;
							//resets variables
							bust = 0;
							userTotal = 0;
							sidebet = 0;
							ddown = 0;
							insurance = 0;
							pcards = 1;
							//hits
							printf("\n");
							boxword("HAND #1");
							delaystring("\tHitting . . .\n",200);
							hand[pcards] = player1.deck[cardno];
							pcards++;
							cardno++;
							boxword(player1.username);//type username More often & SCORE!!
							printhand(hand, pcards);
							userTotal = handtotal(hand, pcards);
						}
						else
						{
							break;
						}
			}
			cardno = jokercheck(player1.deck[cardno], cardno);
			//shows options
			if(showoption)
			{
						printf("\n %c", 201);
						for(i=0;i<23;i++){printf("%c",205);}
						printf("%c\n",187);

						printf(" %c   %s\t %c\n",186, "[SPACE]  Hit", 186);
						printf(" %c   %s\t %c\n",186, " [X]     Stand", 186);

						if(player1.chips >= bet && ddown && cardno == 4)
						{
							printf(" %c   %s\t %c\n",186, " [D]  Double down", 186);
						}

						if(dhand[0].no == 1 && cardno == 4 && player1.chips > 0 && insurance == 0)
						{
							printf(" %c   %s\t %c\n",186, " [I]   Insurance", 186);
						}
						if(hand[0].no == hand[1].no &&  cardno == 4 && player1.chips >= bet)
						{
							printf(" %c   %s\t %c\n",186, " [2]    Split  ", 186);
						}
						printf(" %c   %s\t %c\n",186, "[ESC] Surrender", 186);

						printf(" %c", 200);
						for(i=0;i<23;i++){printf("%c",205);}
						printf("%c",188);
						showoption = 0;
			}
			choice = getch();
			choice = tolower(choice);
			if(choice == 'd' && player1.chips >= bet && ddown )
			{
				//Adds double down to bet
				printf("\n");
				bet2 = betting(1, bet);
				player1.chips += -bet2;
				bet += bet2;
				printhand(hand, pcards);
				handtotal(hand, pcards);
				showoption = 1;
			}
			if(choice == '2'  && player1.chips >= bet && split == 0 && hand[0].no == hand[1].no)
			{
				// SPLITS
				pcards--;
				split = 1;
				player1.chips += -bet;
				bet2 = bet;
				//hits
				printf("\n");
				boxword("HAND #1");
				delaystring("\tHitting . . .\n",200);
				hand[pcards] = player1.deck[cardno];
				pcards++;
				cardno++;
				boxword(player1.username);//type username More often & SCORE!!
				printhand(hand, pcards);
				userTotal = handtotal(hand, pcards);
				showoption = 1;
			}
			if(choice == 'i' && dhand[0].no == 1 && cardno == 4 && player1.chips > 0 && insurance == 0)
			{
				//adds sidebet
				printf("\n");
				sidebet = betting(0,bet);
				player1.chips += -sidebet;
				insurance = 1;
				showoption = 1;
				boxword(player1.username);
				printhand(hand, pcards);
				handtotal(hand, pcards);
			}
			if(choice == 27)
			{
				//surrender
				bust = 1;
				printf("\n   You have surrendered.\n");
				printf("  You forfeit half your chips.\n");
				player1.chips += bet/2;
				Sleep(500);
				showoption = 1;
				if(split == 1)
						{
							//checks user has split
							split++;
							userTotal2 = userTotal;
							ddown2 = ddown;
							insurance2 = insurance;
							sidebet2 = sidebet;
							bet2 = bet;
							bust2 = bust;
							//resets variables
							bust = 0;
							userTotal = 0;
							sidebet = 0;
							ddown = 0;
							insurance = 0;
							pcards = 1;
							//hits
							printf("\n");
							boxword("HAND #2");
							delaystring("\tHitting . . .\n",200);
							hand[pcards] = player1.deck[cardno];
							pcards++;
							cardno++;
							boxword(player1.username);//type username More often & SCORE!!
							printhand(hand, pcards);
							userTotal = handtotal(hand, pcards);
							continue;
						}
			}		
			if(choice == ' ')
			{
					//hits
					printf("\n\n");
					hand[pcards] = player1.deck[cardno];
					pcards++;
					cardno++;
					boxword(player1.username);//type username More often & SCORE!!
					printhand(hand, pcards);
					userTotal = handtotal(hand, pcards);
					showoption = 1;
			}
			if(choice == 'x')
			{
					//stands
					printf("\n\n");
					showoption = 1;
					if(split == 1)
						{
							//checks user has split
							split++;
							userTotal2 = userTotal;
							ddown2 = ddown;
							insurance2 = insurance;
							sidebet2 = sidebet;
							bet2 = bet;
							bust2 = bust;
							//resets variables
							bust = 0;
							userTotal = 0;
							sidebet = 0;
							ddown = 0;
							insurance = 0;
							pcards = 1;
							//hits
							printf("\n");
							boxword("HAND #2");
							delaystring("\tHitting . . .\n",200);
							hand[pcards] = player1.deck[cardno];
							pcards++;
							cardno++;
							boxword(player1.username);//type username More often & SCORE!!
							printhand(hand, pcards);
							userTotal = handtotal(hand, pcards);
							continue;
						}
					break;
			}
			if(userTotal > 21)
			{
				//gets bust
				printf("\n");
				boxword("YOU BUST");
				Beep(250, 300);
				Beep(200, 800);
				bust = 1;
				showoption = 1;
				if(split == 1)
				{
							//checks user has split
							split++;
							userTotal2 = userTotal;
							ddown2 = ddown;
							insurance2 = insurance;
							sidebet2 = sidebet;
							bet2 = bet;
							bust2 = bust;
							//resets variables
							bust = 0;
							userTotal = 0;
							sidebet = 0;
							ddown = 0;
							insurance = 0;
							pcards = 1;
							//hits
							printf("\n");
							boxword("HAND #2");
							delaystring("\tHitting . . .\n",200);
							hand[pcards] = player1.deck[cardno];
							pcards++;
							cardno++;
							boxword(player1.username);//type username More often & SCORE!!
							printhand(hand, pcards);
							userTotal = handtotal(hand, pcards);
							continue;
				}
			}
	}
	//After user has finished drawing
	if(!bust || !bust2 || insurance == 1)
	{
		 //sends informtaion to delaler
		 dealerTotal = dealeround(bust, insurance,dhand, dcards, userTotal, cardno, bet, sidebet, ddown);
	}
		 if(split>1)
		 {
				 boxword("First hand");
		 }
		 winner(bust, userTotal, dealerTotal, insurance, bet, sidebet, ddown);
		 if(split>1)
		 {
				 boxword("Second hand");
			  winner(bust2, userTotal2, dealerTotal, insurance2, bet2, sidebet2, ddown2);
		 }
	if(player1.chips <= 0)
	{
		boxword("Out of chips.");
		Beep(200, 1000);

	}
	else
	{
		toback(cardno-1);
	}
}

//Dealer Drawing
int dealeround(int bust, int insurance,struct card dhand[], int dcards, int userTotal, int cardno, int bet, int sidebet, int ddown)
{
		boxword("DEALER");
		printhand(dhand, dcards);
		int dealerTotal = handtotal(dhand, dcards);
		dhand[dcards] = player1.deck[cardno];
		dcards++;
		cardno++;
		printf("\n");
		while(dealerTotal < 17 && player1.difficulty != 3 || (dealerTotal < 22 && player1.difficulty == 3 && userTotal > dealerTotal && !bust) )
		{
				Sleep(100);
				printf("        Drawing");
				delaystring(" . . .\n\n", 200);
				printhand(dhand, dcards);
				dealerTotal = handtotal(dhand, dcards);
				dhand[dcards] = player1.deck[cardno];
				dcards++;
				cardno++;
				printf("\n");
		}
		Sleep(700);
		if(dealerTotal > 21)
		{
			boxword("DEALER BUST");
			printf("\n");
			Sleep(800);
		}
		return dealerTotal;
}

//Calculates chips won & lost
void winner(int bust, int userTotal, int dealerTotal, int insurance, int bet, int sidebet, int ddown)
{
	if(!bust && (userTotal > dealerTotal || dealerTotal > 22))
		{
			if(userTotal == 21)
			{
				bet =  bet*(3.00/2.00);
			}
			else
			{
				bet += bet;
			}
			printf("    You just won %d Chips!\n",bet);
			player1.chips += bet;
			delaystring(" \n", 500);
			Beep(1000, 50); 
			Beep(1500, 150);
			calcscore(bet, ddown);
		}
		if((userTotal < dealerTotal && dealerTotal < 22) || bust)
		{
			Sleep(600);
			printf("    You just lost %d Chips!\n",bet);
			Beep(100, 1000);
		}
		if(userTotal == dealerTotal && dealerTotal < 22)
		{
			boxword("Equal Hands");
			Beep(500, 800);
			Sleep(600);
			if(player1.difficulty != 3)
			{
			printf(" Your chips return to your side.\n");
			player1.chips += bet;
			}
			else
			{
				printf("     Dealer wins Ties.\n");
			}
		}
		if(insurance && dealerTotal == 21)
		{
			sidebet = sidebet*2;
			printf("[Insurance]\n You just won %d Chips!\n",sidebet);
			calcscore(sidebet, ddown);
			player1.chips += sidebet;
			calcscore(sidebet, ddown);
		}		
		if(insurance && dealerTotal != 21)
		{
			
			printf("[Insurance]\n You just lost %d Chips!\n",sidebet);
		}
		Sleep(400);
		return;
}

//startup menu
int menu(int showX, int showY)
{
	char choice;
	int i=1;
	printf(" %c", 201);
	for(i=0;i<23;i++){printf("%c",205);}
	printf("%c\n",187);

	printf(" %c\t%s\t %c\n",186, "BLACKJACK", 186);
	printf(" %c", 204);
	for(i=0;i<23;i++){printf("%c",205);}
	printf("%c\n",185);

	printf(" %c    [1]  %s\t %c\n",186, "Quick Game", 186);//1
	printf(" %c    [2]   %s\t %c\n",186, "New Game", 186);;//1
	printf(" %c    [3]  %s\t %c\n",186, "Custom Game", 186);
	if(showY == 1){printf(" %c    [H]   %s\t %c\n",186, "Hiscores", 186);};//3
	if(showX == 1){printf(" %c    [C]  %s\t %c\n",186, "Continue", 186);};//2
	printf(" %c   [ESC]   %s\t %c\n",186, "Quit", 186);//0

	printf(" %c", 200);
	for(i=0;i<23;i++){printf("%c",205);}
	printf("%c\n",188);
	do{
		choice = getch();
		tolower(choice);
		if(choice == '1')
		{
			boxword("QUICK GAME");
			return 1;
		}
		if(choice == '2')
		{
			
			boxword("New Game");
			do{
			printf(" Username:  ");	
				gets(player1.username);
				fflush(stdin);
				if(strlen(player1.username)<2)
				{
					printf("   ERROR: Too short.\n");
				}
				else
				{
					i=0;
				}
			}while(i);
			return 2;
		}

		if(tolower(choice) == 'c' && showX == 1)
		{
			boxword("Continue");
			return 3;
		}
		
		if(choice == '3')
		{
			boxword("CUSTOM GAME");
			return 4;
		}

		if(tolower(choice) == 'h'  && showY == 1)
		{
			boxword("Hiscores");
			return 5;
		}

		if(choice == 27)
		{
			return 0;
		}

	}while(i);
	return 0;
}

//prints suit and no
void printcard(struct card plain)
{
	if(plain.no == 0)
	{
		printf("  ");
		return;
	}
	switch(plain.no)
	{
	case 1:
		printf("A");
		break;
	case 2:
		printf("2");
		break;
	case 3:
		printf("3");
		break;
	case 4:
		printf("4");
		break;
	case 5:
		printf("5");
		break;
	case 6:
		printf("6");
		break;
	case 7:
		printf("7");
		break;
	case 8:
		printf("8");
		break;
	case 9:
		printf("9");
		break;
	case 10:
		printf("X");
		break;
	case 11:
		printf("J");
		break;
	case 12:
		printf("Q");
		break;
	case 13:
		printf("K");
		break;
	default:
		printf("  ");
		break;
	}
	switch(plain.suit)
	{
	case 0:
		printf("%c", 0x03);//"%c", 0x04
		break;
	case 1:
		printf("%c", 0x05);
		break;
	case 2:
		printf("%c", 0x04);
		break;
	case 3:
		printf("%c", 0x06);
		break;
	default:
	break;
	}
}

//Placing bets/doubling down
int betting(int ddown, int bet)
{
			char betstr[20];
			int chipsbetted;
			char sure, letterS[] = "s";
			int okay=1;
			do
			{
				if(player1.chips == 1)
				{
					strcpy(letterS, "");
				}
				else
				{
					strcpy(letterS, "s");
				}if(ddown)
				{
					if(bet == 1)
					{
						strcpy(letterS, "");
					}
					printf("\n You can bet up to %d Chip%s.\n", bet, letterS);
				}
				else
				{
				printf("\n     You have %d Chip%s.\n", player1.chips,letterS);
				}
				printf("      Place your bet.\n\tAmount: ");
				fflush(stdin);
				gets(betstr);
				chipsbetted = atoi(betstr);
				if(chipsbetted > 0 && chipsbetted <= player1.chips)
				{
					if(ddown && bet > chipsbetted)
					{
						printf(" Sorry, you cannot doubledown more than your inital bet.\n");
						okay = 0;
					}
					else
						{

						if(chipsbetted == 1)
						{
							strcpy(letterS, "");
						}
						else
						{
							strcpy(letterS, "s");
						}
						printf(" %d Chip%s are you sure? [Y/N]", chipsbetted, letterS);
						do{
						sure = getch();
						if(tolower(sure) == 'y')
						{
							okay = 1;
							printf("\n");
							break;
						}
						if(tolower(sure) == 'n' )
						{
							okay = 0;
						}
						}while(okay);
					}
				}
				else
				{
					printf(" ERROR: Invalid Input.\n\n");
					okay = 0;
				}
			}while(!okay);
			return chipsbetted;
}

//opens file
FILE *openfile(char *address, char *mode)
{
	FILE *fptr = fopen(address, mode);
	if(fptr == NULL)
	{
		printf("Cannot access file: %s\n", address);
		delaystring("Creating new file . . . \n\n", 100);
		FILE *fptr = fopen(address, "wb+");
		fclose(fptr);
		return openfile(address,mode);
	}
	return fptr;
}

//delays string
void delaystring(char string[20], int ms)
{
	int i;
	for(i=0;i<strlen(string);i++)
	{
		printf("%c",string[i]);
		Sleep(ms);
	}
}

//counts hand total
int handtotal(struct card hand[], int amount)
{
	int best;
	int i;
	int ace = 0;
	int total[4] = {0};

	for(i=0;i<amount;i++)
	{
	switch(hand[i].no)
	{
	case 1:
		ace++;
		if(ace == 1)
		{
		total[0] += 1;
		total[1] += 1;
		total[2] += 1;
		total[3] += 11;
		}
		if(ace == 2)
		{
		total[0] += 1;
		total[1] += 1;
		total[2] += 1;
		total[3] += 11;
		}
		if(ace == 3)
		{
		total[0] += 1;
		total[1] += 1;
		total[2] += 11;
		total[3] += 11;
		}
		if(ace == 4)
		{
		total[0] += 1;
		total[1] += 11;
		total[2] += 11;
		total[3] += 11;
		}
		break;
	case 2:
		total[0] += 2;
		total[1] += 2;
		total[2] += 2;
		total[3] += 2;
		break;
	case 3:
		total[0] += 3;
		total[1] += 3;
		total[2] += 3;
		total[3] += 3;
		break;
	case 4:
		total[0] += 4;
		total[1] += 4;
		total[2] += 4;
		total[3] += 4;
		break;
	case 5:
		total[0] += 5;
		total[1] += 5;
		total[2] += 5;
		total[3] += 5;
		break;
	case 6:
		total[0] += 6;
		total[1] += 6;
		total[2] += 6;
		total[3] += 6;
		break;
	case 7:
		total[0] += 7;
		total[1] += 7;
		total[2] += 7;
		total[3] += 7;
		break;
	case 8:
		total[0] += 8;
		total[1] += 8;
		total[2] += 8;
		total[3] += 8;
		break;
	case 9:
		total[0] += 9;
		total[1] += 9;
		total[2] += 9;
		total[3] += 9;
		break;
	case 10:
		total[0] += 10;
		total[1] += 10;
		total[2] += 10;
		total[3] += 10;
		break;
	case 11:
		total[0] += 10;
		total[1] += 10;
		total[2] += 10;
		total[3] += 10;
		break;
	case 12:
		total[0] += 10;
		total[1] += 10;
		total[2] += 10;
		total[3] += 10;
		break;
	case 13:
		total[0] += 10;
		total[1] += 10;
		total[2] += 10;
		total[3] += 10;
		break;
	default:
		break;
	}
	}
	delaystring("\tHandTotal: ", 100);
	printf("%d", total[0]);
	Sleep(200);
	best = total[0];
	if(total[0] != total[1] && total[1] < 22)
	{
		printf(" / %d", total[1]);
		Sleep(200);
		if(total[1] > best)
		{
			best = total[1];
		}
	}
	if(total[1] != total[2] && total[0] != total[2] && total[2] < 22)
	{
		printf(" / %d", total[2]);
		Sleep(200);
		if(total[2] > best)
		{
			best = total[2];
		}
	}
	if(total[2] != total[3] && total[0] != total[3] && total[1] != total[3] && total[3] < 22)
	{
		printf(" / %d", total[3]);
		Sleep(200);
		if(total[3] > best)
		{
			best = total[3];
		}
	}
	printf("\n");
	if(player1.difficulty ==  1)
	{
		hint(total[0]);
	}
	return best;
}

//prints whole hand
void printhand(struct card hand[], int amount)
{
	int i;
	char tab = '\t';
	if(amount > 3)
	{
		tab = ' ';
	}
	printf("%c", tab);
	for(i=0;i<amount;i++)
	{
	printf("%c%c%c%c ", 218,196,196,191);
	}
	printf("\n%c", tab);
	for(i=0;i<amount;i++)
	{
	printf("%c  %c ", 179, 179);
	}
	printf("\n%c", tab);
	for(i=0;i<amount;i++)
	{
	printf("%c",179);
	printcard(hand[i]);
	printf("%c ",179);
	}
	printf("\n%c", tab);
	for(i=0;i<amount;i++)
	{
	printf("%c  %c ", 179, 179);
	}
	printf("\n%c", tab);
	for(i=0;i<amount;i++)
	{
	printf("%c%c%c%c ", 192,196, 196,217);
	}
	printf("\n");
}

//prints words in box
void boxword(char string[])
{
	//strlen is only = 0, in quick game
	int i;
	char boxstring[20];
	strcpy(boxstring, string);

	if(strlen(boxstring) == 0)
	{
		strcpy(boxstring, " YOU ");
	}

	int len = strlen(boxstring);
	printf("\t%c", 201);
	for(i=0;i<len+2;i++){printf("%c",205);}
	printf("%c\n",187);

	printf("\t%c %s %c\n",186, boxstring, 186);

	printf("\t%c", 200);
	for(i=0;i<len+2;i++){printf("%c",205);}
	printf("%c\n",188);
}

//checks card if joker
int jokercheck(struct card anycard, int cardNo)
{
	int decks = 1;
	if(player1.difficulty == 3)
	{
		decks = 2;
	}
		if (anycard.no == 0)
		{
			boxword("Joker drawn");
			printf("\t");
			shuffle(0,decks);
			return 0;
		}
		return cardNo;
}

//custom game
int custgame()
{
	//setup for custom game
	int deckamount, sure, i;
	char convert[20];
	do{
		sure = 1;
		do{
		printf(" [Player Name]:  ");
		gets(player1.username);
		fflush(stdin);
		if(strlen(player1.username)<2)
		{
			printf("   ERROR: Too short.\n");
			sure =1;
		}
		else
		{
			sure =0;
		}
		}while(sure);
		sure = 1;
		do{
			printf(" [Staring Chips]: ");
			gets(convert);
			fflush(stdin);
			player1.chips = atoi(convert);
			if(player1.chips > 99999999)
			{
				printf(" Limit is 99999999.\n");
				sure = 0;
				continue;
			}
			if(player1.chips > 0 )
			{
			sure = 1;
			}
			else
			{
				printf(" Invalid Amount.\n");
				sure = 0;
			}
		}while(!sure);

		do{
			printf(" [1 or 2 Decks]: ");
			gets(convert);
			fflush(stdin);
			deckamount = atoi(convert);
			if(deckamount != 1 && deckamount != 2)
			{
				printf(" Invalid Amount.\n");
				sure = 0;
			}
			else
			{
				sure = 1;
			}
			
		}while(!sure);
		printf("%c", 201);
		for(i=0;i<23;i++){printf("%c",205);}
		printf("%c\n",187);
		printf("%c\t%s\t%c\n",186, "PlayerInfo", 186);
		printf("%c", 204);
		for(i=0;i<23;i++){printf("%c",205);}
		printf("%c\n",185);
		printf("%c   %s %s\t%c\n",186, "Playername:", player1.username, 186);
		printf("%c   %s %d    \t%c\n",186, "Chips:",player1.chips, 186);
		printf("%c   %s %d\t%c\n",186, "Decks used:",deckamount, 186);
		printf("%c", 200);
		for(i=0;i<23;i++){printf("%c",205);}
		printf("%c\n",188);
		printf("  Is this data Correct? [Y/N]");
		do{
			convert[0] = getch();
			if(tolower(convert[0]) == 'y')
			{
				sure = 1;
			}
			if(tolower(convert[0]) == 'n')
			{
				sure = 0;
		}
		}while(!sure);
		}while(!sure);
		player1.difficulty = 2;
		printf("\n");
		return deckamount;
}

//Populates new deck
void newdeck(int decks)
{
	int i ,j,k, card;
	card = 1;
	//outer loop for multiple decks
	for(k=0;k<decks;k++)
	{
		//middle for loop changes suits
		for(i=0;i<4;i++)
		{
		//inner for loop changes cards
			for(j=1;j<14;j++)
			{
			player1.deck[card].no = j;
			player1.deck[card].suit = i;
			card++;
			}
	}
	}
	//set first card as joker
	player1.deck[0].no = 0;
	player1.deck[0].suit = 0;
}

//shuffles deck
void shuffle(int start, int decks)
{
	//places joker at back
	int y, x;
	struct card temp;
	if(!start)
	{
	delaystring("Shuffling . . . \n", 500);
	}

	srand(time(NULL));

	for(x=52*decks;x>0;x--)
    {
		y = (rand() % 51)+1;
		temp = player1.deck[x];
		player1.deck[x] = player1.deck[y];
		player1.deck[y] = temp;
	}
	temp = player1.deck[52];
	player1.deck[52] = player1.deck[0];
	player1.deck[0] = temp;
}

//hints in easy mode
void hint(int total)
{
	//likelyhood of going bust
	int i = 53, j = 0, bustNo, beforeJoker=0, afterJoker = 0, ctotal=0;
	int cnum[13] = {0};
	char buststring[20];
	if (total >= 21)
	{
		return;
	}
		while (player1.deck[i].no != 0) 
		{
			cnum[player1.deck[i].no-1]++;
			i--;
			afterJoker++;
		}
	if (total <= 11)
	{
		return;
	}
		for (i = 0; i < 13;i++)
		{
			cnum[i] = 4 - cnum[i];
		}
		beforeJoker = 52 - afterJoker;
		bustNo = 22 - total;

		for (i = bustNo; i < 13; i++)
		{
			ctotal = ctotal + cnum[i];
		}
		ctotal = ctotal*100 / beforeJoker;
		sprintf(buststring, " BUST %d%% ", ctotal);
		boxword(buststring);
	return;
}

//explains difficulties
int difficultyselect()
{
	int difficulty;
	int  i, loop= 1;
	char diff;
	fflush(stdin);

	printf("%c", 201);
	for(i=0;i<23;i++){printf("%c",205);}
	printf("%c\n",187);

	printf("%c\t%s\t%c\n",186, "Difficulties", 186);

	printf("%c", 204);
	for(i=0;i<23;i++){printf("%c",205);}
	printf("%c\n",185);
	printf("%c   %s\t%c\n",186, "[1]    Casual    ", 186);
	printf("%c   %s\t%c\n",186, "[2]    Amateur   ", 186);
	printf("%c   %s\t%c\n",186, "[3]  Professional", 186);
	printf("%c   %s\t%c\n",186, "[4]     INFO    ", 186);
	printf("%c", 200);
	for(i=0;i<23;i++){printf("%c",205);}
	printf("%c\n",188);
	
	do{
		diff = getch();
		tolower(diff);
		fflush(stdin);
		
		if(diff == '1')
		{
			difficulty = 1;
			break;
		}
		if(diff == '2')
		{
			difficulty = 2;
			break;
		}
		if(diff == '3')
		{
			difficulty = 3;
			break;
		}
		if(diff == '4')
		{
			boxword("Casual");
			puts(" Player starts off with a bet of 1000 chips.");
			puts(" Given hints and you can always double down.");

			boxword("Amateur");
			puts(" Player starts off with a bet of 500 chips.");
			puts(" The player can only double down on 10 & 11.");

			boxword("Professional");
			puts(" The game is played with two decks.");
			puts(" Player starts off with a bet of 333 chips.");
			puts(" Player can only double down on 10 & 11.");
			puts(" Dealer wins a tied game in professional.");

			puts("\n Playing higher difficulty means a higher bonus.");
		}
	}while(loop);
	player1.chips = 1000/difficulty;
	return difficulty;
}

//calculates score
void calcscore(int bet, int ddown)
{
	//Algorithm for scoring
	player1.score = player1.score + (bet/3.33)*(player1.difficulty)*(ddown+5);
	return;
}

//chcecks hiscores
int hiscores(char *address, int print)
{
	//if print is true it also prints the hiscores
	//if not it just checks if there are any hiscores
	int i=0;
	struct hiscores printhiscore;
	struct savefile data;
	char tab[2] = " ";
	FILE *fptr = openfile(address,"rb");
	fread(&data,sizeof(struct savefile),1,fptr);
	fclose(fptr);
	printhiscore = data.top10;
	if(printhiscore.score[0] > 0 && print == 0)
	{
		return 1;
	}
	if(print == 0)
	{
		return 0;
	}
	printf(" #  %s\t%s\n","PLAYER","SCORE");
	for(i=0;i<10;i++)
	{
		if(printhiscore.score[i] > 0)
		{
			if(printhiscore.score[i] < 1000)
			{
				strcpy(tab, "\t");
			}
			printf(" %d   %s\t%.0f\n", i+1,printhiscore.username[i],printhiscore.score[i]);
		}
		else
		{
			break;
		}
	}
	return 1;
}


//adds hiscore
int newHiscore(char *address)
{
	int i=0, j=0, amount =0, newhiscore = 0;
	struct hiscores printhiscore;
	struct savefile data;
	int loop = 0;
	FILE *fptr = openfile(address,"rb");
	fread(&data,sizeof(struct savefile),1,fptr);
	
	fclose(fptr);
	printhiscore = data.top10;
	for(i=0;i<10;i++)
	{
		if(printhiscore.score[i] > 0)
		{
			amount++;
		}
		else
		{
			break;
		}
	}
	if(amount < 10)
	{
		if(!strcmp(player1.username, ""))
		{
			do{
				if(loop)
				{
					//IF they selected quickgame they had no username
					printf("   ERROR: Username too short.\n");
				}
				printf("    Please enter a username.\n\tUsername: ");
				gets(player1.username);
				fflush(stdin);
				loop++;
			}while(strlen(player1.username) < 3);
		}
		printhiscore.score[amount] = player1.score;
		strcpy(printhiscore.username[amount],player1.username);
		newhiscore++;
		sorthiscore(printhiscore, data, address, amount);
	}
	else
	{
		if(printhiscore.score[9] < player1.score)
		{
			if(!strcmp(player1.username, ""))
			{
				do{
					if(loop)
					{
						printf("Username too short.\n");
					}
					printf("Please enter a username.\n");
					gets(player1.username);
					fflush(stdin);
					loop++;
				}while(strlen(player1.username) < 3);
			}
			printhiscore.score[9] = player1.score;
			strcpy(printhiscore.username[9],player1.username);
			sorthiscore(printhiscore, data, address, amount);
			newhiscore++;
		}
	}
	return newhiscore;
}

//sorts hiscores
void sorthiscore(struct hiscores sorthiscore,struct savefile data, char *address, int limit)
{
	//sort the sorthiscore
	int i,j;
	float temp;
	char stringtemp[20];

	boxword("NEW HISCORE!");
	for(i=0;i<limit+1;i++)
	{
		for (j=0;j<limit+1;j++)
		{
			if(sorthiscore.score[i]>sorthiscore.score[j])
			{
				temp = sorthiscore.score[i];
				sorthiscore.score[i] = sorthiscore.score[j];
				sorthiscore.score[j] = temp;
				if(sorthiscore.score[j] > 0)
				{
					strcpy(stringtemp,sorthiscore.username[i]);
					strcpy(sorthiscore.username[i],sorthiscore.username[j]);
					strcpy(sorthiscore.username[j],stringtemp);
				}
				else
				{
					strcpy(sorthiscore.username[i],sorthiscore.username[j]);
				}
			}
		}
	}
	
	data.top10 = sorthiscore; 
	FILE *fptr = openfile(address,"wb");
	fwrite(&data, sizeof(struct savefile), 1, fptr);
	fclose(fptr);
}

//loads user
int loadUsers(char *address) 
{
	struct savefile data;
	FILE *fptr = openfile(address, "rb");
	rewind(fptr);
	fread(&data, sizeof(struct savefile), 1, fptr);
	fclose(fptr);
	if (data.cont.score >= 0)
	{
			player1 = data.cont;
			return 1;
	}
	return 0;
}

//deletes user
int deleteUser(char *address, struct savefile info)
{
	info.cont.score = -1;
	FILE *fptr = openfile(address, "wb");
	fwrite(&info, sizeof(struct savefile), 1, fptr);	
	fclose(fptr);
	return 1;
}

//saves user
int saveUser(char *address, struct savefile info)
{		
		info.cont = player1;
		FILE *fptr = openfile(address, "wb");
		rewind(fptr);
		fwrite(&info, sizeof(struct savefile), 1, fptr);	
		fclose(fptr);
		return 1;
}

//links to either delete or save user
void savdel(int todo, char *address)
{
		struct savefile data;
		FILE *fptr = openfile(address, "rb");
		fread(&data, sizeof(struct savefile), 1, fptr);
		fclose(fptr);
		switch(todo)
			{
			case 1:
				 saveUser(address, data);
				break;
			case 2:
				 deleteUser(address, data);
				break;
			default:
				break;
			}
}
