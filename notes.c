#include "notes.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

int notes_look(char *target_note) {
	int i=0;
	while((strcmp(target_note,notes[i++])!=0)&&(i<=NO_MIDI_NOTES)); //TODO: test if last note works
		return i-1;
}

int notes_load(char *file) {
	FILE *in;
	int i, n;
	char current_note[4];
	
	fprintf(stdout,"Loading notes... ");
	
	in=fopen(file,"r");
	if(!in) {
		fflush(stdout);
		fprintf(stderr, "Error: Cannot open notes file \"%s\"!\n",file);
		return 1;
	}
	
	while(fscanf(in,"%d ",&i)!=-1) {
		n=0;
		do {
			fscanf(in,"%c",&current_note[n]);
			n++;
		} while((current_note[n-1]!='\n')&&(!feof(in)));
		current_note[n-1]=0;
		
		strcpy(notes[i],current_note);
	}
	
	//show 1 note
	fprintf(stdout,"(\"%s\" - %d) ","G2",notes_look("G2"));
	
	fprintf(stdout,"(done)\n");
	
	fclose(in);
	
	return 0;
}

char *notes_translate(char *note) {
	int i;
	
	if(!note) return note;
	
	if(note[1]=='b') {
		if((note[0]=='C')||(note[0]=='F')) {
			i=1;
			while(note[i]=note[++i]);
		} else {
			note[1]='#';
		}
		note[0]=(((note[0]-'A'+7)-1)%7)+'A';
	}
	if(((note[0]=='B')||(note[0]=='E'))&(note[1]=='#')) {
		note[0]=(((note[0]-'A'+7)+1)%7)+'A';
		i=1;
		while(note[i]=note[++i]);
	}
	
	char *rest;
	rest=strchr(note,'/');
	if(rest) notes_translate(rest+1);
	
	return note;
}