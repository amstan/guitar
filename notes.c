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