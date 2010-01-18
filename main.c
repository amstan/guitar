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
#include "config.h"

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


//!the topmost fret on each string
char last_frets[NO_STRINGS];

//!the topmost fret(used for chords)
char last_fret;

//!last note that was playing on the string, used to stop it when the next note plays
char lastnote[NO_STRINGS];

//!fretboard button states, either 1 for pressed or 0 for not pressed
char fretboard[NO_STRINGS][NO_FRETS];

//!This is the number to transpose by.
//!Negative numbers makes stuff go flat, positive sharp and 0 stays the same.
int transpose;

//misc functions
void mute(char string) {
	//mute string
	if(lastnote[string]==-1) return;
	
	//add note off to midi
	jack_midinote(0,lastnote[string],0,CHANNEL);
	
	//turn off string
	lastnote[string]=-1;
}

void pluck(char string, char note, char velocity) {
	note+=transpose;
	
	//mute string
	mute(string);
		
	//add a note on to midi
	jack_midinote(1,note,velocity,0);
	
	//remember which note was on
	lastnote[string]=note;
}

/*! looks on the fretboard and updates the last fret
 * @param string the string to look at
 */
void frets_update(char string) {
	char i;
	
	last_frets[string]=0;
	for(i=0;i<NO_FRETS;i++)
		if(fretboard[string][i])
			last_frets[string]=i;
	
	last_fret=0;
	for(i=0;i<NO_STRINGS;i++)
		if(last_frets[last_fret]<=last_frets[i])
			last_fret=i;
}

int init()
{
	fprintf(stdout,"Welcome to the Guitar Sequencer!\n made by Alex Stan\n\n");
	
	int status=0;
	status += config_init("etc/config.ini");
	status += notes_load(config_look("notes"));
	status += tuning_load(config_look("tuning"));
	status += chords_load(config_look("chords"));
	status += chords_load_mappings(config_look("chord_mappings"));
	
	if(!jack_client) status += jack_init(config_look("jack_client"),config_look("connect_output"));
	
	transpose=0;
	transpose=atoi(config_look("transpose"));
	fprintf(stdout,"Transposing by %+i subtunes.\n",transpose);
	
	if(status!=0)
	{
		fflush(stdout);
		fprintf(stderr, "Something went wrong with the initialization! Please check for errors above this line.\n");
		return 1;
	}
	
	return 0;
}

//*Main Function*//
int main(int narg, char **args) {
	args_init(narg,args);
	
	int i,j;
	
	if(init()) return 1;
	
	//clear some memory
	for(i=0;i<NO_STRINGS;i++) {
		lastnote[i]=-1;
		
		last_frets[i]=0;
		for(j=0;j<NO_FRETS;j++)
			fretboard[i][j]=0;
	}
	last_fret=0;
	
	//main loop
	//look for key events
	char command[20]; //one command coming from the guitar
	char p, //pressed/not
	     s, //string
		 f; //fret
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
				
				//validate string
				if(!((s>=0)&&(s<NO_STRINGS)))
					continue;
				
				if(!((f>=0)&&(f<NO_FRETS)))
					continue;
				
				fretboard[s][f]=p;
				
				if(mode=='n') if(last_frets[s]<=f) mute(s);
				
				frets_update(s);
				
				break;
				
			case 's':
				p=command[0]=='p'?1:0; //released or pressed
				s=command[2]-'0';
				
				//validate string
				if(!((s>=0)&&(s<NO_STRINGS)))
					continue;
				
				switch(mode) {
					case 'n':
						printf("p%d\n",p);
						if(p)
							pluck(s,last_frets[s]+tuning[s],64);
// 						else
// 							mute(s);
						break;
					case 'c':
						if(p) {
							mute(s);
							
							int k;
							
							if(last_frets[last_fret]==-1) break; //don't play when no button is pressed
							k=chord_mappings[last_fret][last_frets[last_fret]-1];
							
							if(k==-1) break; //don't play non assigned chord
							if(chord[k][s]==-1) break; //don't play unplayable note
								
							pluck(s,chord[k][s],64);
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
				if(command[2]=='6')
				{
					init();
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