/*
Guitar Sequencer
Copyright HyperTriangle
written by Alex Stan
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "main.h"
#include "jack.h"
#include "args.h"

#include "notes.h"
#include "tuning.h"
#include "chords.h"

//#define CLIENT_NAME args_get('C',"guitar-seq")
//#define MAX_EVENT_SIZE 10
//#define RING_BUFFER_SIZE 10
#define OUTPUT_EVENTS args_exists('o')

//TODO: make this nicer
#define CHORDS_FILE "etc/chords.cfg"
#define CHORD_MAPPINGS_FILE "etc/chord_mappings.cfg"
#define TUNING_FILE args_get('t',"etc/tuning_eadgbe.cfg")

#define CHANNEL args_exists('c')?string:0


//TODO: make this better
char frets[NO_STRINGS];

//!last note that was playing on the string, used to stop it when the next note plays
char lastnote[NO_STRINGS];

//!fretboard button states, either 1 for pressed or 0 for not pressed
char fretboard[NO_STRINGS][NO_FRETS];

//misc functions
void mute(char string) {
	//mute string
	if(lastnote[string]==-1) return;
	
	//add note off to midi
	jack_midinote(0,lastnote[string],0,CHANNEL);
	
	//turn off string
	lastnote[string]=-1;
}

void pluck(char string, char velocity) {
	//mute string
	mute(string);
	
	if(frets[string]==-1) return; //don't play nonplayable string
		
	//add a note on to midi
	jack_midinote(1,tuning[string]+frets[string],velocity,CHANNEL);
	
	//remember which note was on
	lastnote[string]=tuning[string]+frets[string];
}

void update_fret(char string) {
	//looks on the fretboard and updates the fret
	int i;
	frets[string]=0;
	for(i=0;i<19;i++)
		if(fretboard[string][i])
			frets[string]=i;
}

//*Main Function*//
int main(int narg, char **args) {
	args_init(narg,args);
	
	int i,j;
	
	fprintf(stdout,"Welcome to the Guitar Sequencer!\n made by Alex Stan\n\n");
	
	int status=0;
	status += notes_load(NOTE_FILE);
	status += tuning_load(TUNING_FILE);
	status += chords_load(CHORDS_FILE);
	status += chords_load_mappings(CHORD_MAPPINGS_FILE);
	
	status += jack_init();
	
	if(status!=0)
	{
		fflush(stdout);
		fprintf(stderr, "Something went wrong with the initialization! Please check for errors above this line.\n");
		return 1;
	}
	
	
	//clear some memory
	for(i=0;i<NO_STRINGS;i++) {
		lastnote[i]=-1;
		
		frets[i]=0;
		for(j=0;j<NO_FRETS;j++)
			fretboard[i][j]=0;
	}
	
	//main loop
	//look for key events
	char command[20];
	char p, s, f;
	char mode='n';
	while(command[0]!='e') {
		if(fscanf(stdin,"%s",command)!=1)
			break;
		
		if(OUTPUT_EVENTS) puts(command);
		
		switch(command[1]) {
			case 'f':
				p=command[0]=='p'?1:0; //released or pressed
				s=command[2]-'0';
				f=command[3]-'0';
				if(command[4]!=0) {
					f*=10;
					f+=command[4]-'0';
				}
				
				if((s<0)||(s>5)||(f<0)||(f>19)||(p<0)||(p>1))
					continue;
				
				fretboard[s][f]=p;
				
				if(frets[s]<=f) mute(s);
				
				update_fret(s);
				
				break;
				
			case 's':
				p=command[0]=='p'?1:0; //released or pressed
				s=command[2]-'0';
				
				if((s<0)||(s>5)||(p<0)||(p>1))
					continue;
				
				switch(mode) {
					case 'n':
						if(p)
							pluck(s,64);
// 						else
// 							mute(s);
						break;
					case 'c':
						if(p) {
							mute(s);
							
							char k;
							
							k=frets[5]-1;
							k=chord_mappings[5][k];
							printf("%s\n",chord_name[k]);
							
							//TODO find k
							if(k==-1) break; //don't play non assigned chord
							if(chord[k][s]==-1) break; //don't play unplayable note
							
							
							//add a note on to midi
							jack_midinote(1,chord[k][s],65,0);
							
							//remember which note was on
							lastnote[s]=chord[k][s];
						} else {
							//mute(s);
						}
						break;
						
				}
				
				break;
				
			case 'b':
				if(command[0]=='r')
				{
					mode=command[2]=='0'?'n':'c'; //released or pressed
					printf("mode %c\n",mode);
				}
				break;
				
			case 'o':
				//TODO
				break;
				
			default:
				if(command[0]!='e') fprintf(stderr,"No such guitar event \"%s\"!\n",command);
				break;
		}
	}
	
	//cleanup
	fprintf(stdout,"Exiting! cleanup...\n");
	
	return 0;
}