#include <stdio.h>
#include "tuning.h"

int load_tuning(char *file) {
	FILE *in;
	int i, n;
	char current_note[NOTE_NAME_LEN];
	
	fprintf(stdout,"Loading tuning... (");
	
	in=fopen(file,"r");
	if(!in) {
		fflush(stdout);
		fprintf(stderr, "Error: Cannot open tuning file \"%s\"!\n",file);
		return 1;
	}
	
	//TODO: change this to strings
	for(i=0;i<NO_STRINGS;i++) {
		n=0;
		do {
			fscanf(in,"%c",&current_note[n]);
			n++;
		} while((current_note[n-1]!='\n')&&(!feof(in)));
		current_note[n-1]=0;
		
		tuning[i]=notes_look(current_note);
		
		
		fprintf(stdout, "%s ",notes[tuning[i]]);
	}
	
	fprintf(stdout,") (done)\n");
	
	fclose(in);
	return 0;
}