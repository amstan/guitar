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

#include "jack.h"
#include "args.h"


//global
#define NO_STRINGS 6
#define NO_FRETS 19


#define CLIENT_NAME args_get('C',"guitar-seq")
#define MAX_EVENT_SIZE 10
#define RING_BUFFER_SIZE 10
#define OUTPUT_EVENTS args_exists('o')

#define NOTE_FILE "etc/notes.cfg"
#define CHORDS_FILE "etc/chords.cfg"
#define CHORD_MAPPINGS_FILE "etc/chord_mappings.cfg"
#define TUNING_FILE args_get('t',"etc/tuning_eadgbe.cfg")

#define CHANNEL args_exists('c')?string:0


//Note definitions
#define NO_MIDI_NOTES 128
#define NOTE_NAME_LEN 4
char notes[NO_MIDI_NOTES][NOTE_NAME_LEN];

int look_note(char *target_note) {
	int i=0;
	while((strcmp(target_note,notes[i++])!=0)&&(i<=NO_MIDI_NOTES)); //TODO: test if last note works
	return i-1;
}

void load_notes(char *file)
{
	FILE *in;
	int i, n;
	char current_note[4];
	
	fprintf(stdout,"Loading notes... ");
	
	in=fopen(file,"r");
	
	while(fscanf(in,"%d ",&i)!=-1) {
		n=0;
		do {
			fscanf(in,"%c",&current_note[n]);
			n++;
		} while((current_note[n-1]!='\n')&&(!feof(in)));
		current_note[n-1]=0;
		
		strcpy(notes[i],current_note);
		
		//printf(stdout,"%d \"%s\"\n",i,notes[i]);
	}
	
	//show 1 note
	fprintf(stdout,"(\"%s\" - %d) ","G2",look_note("G2"));
	
	fprintf(stdout,"(done)\n");
	
	fclose(in);
}



//Tuning
char tuning[NO_STRINGS];

void load_tuning(void)
{
	FILE *in;
	int i, n;
	char current_note[4];
	
	fprintf(stdout,"Loading tuning... (");
	
	in=fopen(TUNING_FILE,"r");
	
	for(i=0;i<6;i++) {
		n=0;
		do {
			fscanf(in,"%c",&current_note[n]);
			n++;
		} while((current_note[n-1]!='\n')&&(!feof(in)));
		current_note[n-1]=0;
		
		tuning[i]=look_note(current_note);
		
		
		fprintf(stdout, "%s ",notes[tuning[i]]);
	}
	
	fprintf(stdout,") (done)\n");
	
	fclose(in);
}


//Chords
char chord_name[100][10];
char chord[100][6];
char chord_mappings[6][19];
int chord_n;

int look_chord(char *name) {
	int i;
	if(name[0]=='-') return -1;
	for(i=0;i<chord_n;i++)
		if(strcmp(name,chord_name[i])==0) return i;
	return -1;
}

void load_chords(void) {
	FILE *in;
	int k,i,n;
	char note[4];
	
	fprintf(stdout,"Loading chords... ");
	
	in=fopen(CHORDS_FILE,"r");
	
	chord_n=0;
	while(!feof(in)) {
		fscanf(in,"%[^\t]",chord_name[chord_n]);
		
		for(i=0;i<6;i++) {
			fscanf(in,"%s",note);
			if((note[0]=='-')||(note[0]=='x')||(note[0]=='X'))
				chord[chord_n][i]=-1;
			else if(isdigit(note[0]))
				chord[chord_n][i]=atoi(note)+tuning[i];
			else
				chord[chord_n][i]=look_note(note);
		}
		
		fscanf(in,"\n");
		chord_n++;
	}
	
	//output status
	fprintf(stdout,"(%d chords)",chord_n);
	fprintf(stdout," (%s:",chord_name[0]);
	for(i=0;i<6;i++)
	{
		if(chord[0][i]!=-1)
			fprintf(stdout,"%s",notes[chord[0][i]]);
		else
			fprintf(stdout,"-");
		if(i<5) fprintf(stdout," ");
	}
	
	fprintf(stdout,") (done)\n");
	
	fclose(in);
}

void load_chord_mappings(void) {
	FILE *in;
	int i,j;
	char name[10];
	
	fprintf(stdout,"Loading chord mappings... ");
	in=fopen(CHORD_MAPPINGS_FILE,"r");
	
	
	for(i=5;i>=0;i--) {
		for(j=0;j<19;j++) {
			fscanf(in,"%s",name);
			chord_mappings[i][j]=look_chord(name);
		}
	}
	
	//output status
	fprintf(stdout," (string 6 fret 6: %s)",chord_name[chord_mappings[5][5]]);
	
	fprintf(stdout," (done)\n");
	
	fclose(in);
}


//global variables
char frets[NO_STRINGS];
char lastnote[NO_STRINGS];
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
	
	load_notes(NOTE_FILE);
	load_tuning();
	load_chords();
	load_chord_mappings();
	
	jack_init();
	
	//clear some memory
	for(i=0;i<6;i++) {
		lastnote[i]=-1;
		
		frets[i]=0;
		for(j=0;j<19;j++)
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