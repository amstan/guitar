#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "tuning.h"
#include "chords.h"

int chords_look(char *name) {
	int i;
	if(name[0]=='-') return -1;
	for(i=0;i<chord_n;i++)
		if(strcmp(name,chord_name[i])==0) return i;
		return -1;
}

int chords_load(char *file) {
	FILE *in;
	int k,i,n;
	char note[NOTE_NAME_LEN];
	
	fprintf(stdout,"Loading chords... ");
	
	in=fopen(file,"r");
	if(!in) {
		fflush(stdout);
		fprintf(stderr, "Error: Cannot open chords file \"%s\"!\n",file);
		return 1;
	}
	
	chord_n=0;
	while(!feof(in)) {
		fscanf(in,"%[^\t]",chord_name[chord_n]);
		
		for(i=0;i<NO_STRINGS;i++) {
			fscanf(in,"%s",note);
			if((note[0]=='-')||(note[0]=='x')||(note[0]=='X'))
				chord[chord_n][i]=-1;
			else if(isdigit(note[0]))
				chord[chord_n][i]=atoi(note)+tuning[i];
			else
				chord[chord_n][i]=notes_look(note);
		}
		
		fscanf(in,"\n");
		chord_n++;
	}
	
	//output status
	fprintf(stdout,"(%d chords)",chord_n);
	fprintf(stdout," (%s:",chord_name[0]);
	for(i=0;i<NO_STRINGS;i++)
	{
		if(chord[0][i]!=-1)
			fprintf(stdout,"%s",notes[chord[0][i]]);
		else
			fprintf(stdout,"-");
		if(i<NO_STRINGS-1) fprintf(stdout," ");
	}
	
	fprintf(stdout,") (done)\n");
	
	for(i=0;i<chord_n;i++)
	{
		printf("%s",chord_name[i]);
		for(k=0;k<6;k++)
			printf(" %d",chord[i][k]);
		printf("\n");
	}
	
	fclose(in);
	return 0;
}

int chords_load_mappings(char *file) {
	FILE *in;
	int i,j;
	char name[CHORD_NAME_LEN], space[CHORD_NAME_LEN];
	
	fprintf(stdout,"Loading chord mappings... ");
	
	in=fopen(file,"r");
	if(!in) {
		fflush(stdout);
		fprintf(stderr, "Error: Cannot open chord mappings file \"%s\"!\n",file);
		return 1;
	}
	
	//clean what was in there before
	for(i=0;i<NO_STRINGS;i++)
		for(j=0;j<NO_FRETS;j++)
			chord_mappings[i][j]=-1;
	
	fscanf(in,"%*[^\n]s"); //skip the first line with the fret #s
	
	for(i=NO_STRINGS-1;i>0;i--) {
		j=0;
		do {
		//for(j=0;j<NO_FRETS;j++) {
			fscanf(in,"%s%*[\t ]s",name);
			chord_mappings[i][j]=chords_look(name);
			j++;
		} while(fscanf(in,"%[\n\r\t ]s",space)==0);
	}
	
	//output status
	fprintf(stdout," (string %d fret %d: %s)", NO_STRINGS, 3 ,chord_name[chord_mappings[NO_STRINGS-1][2]]);
	
	fprintf(stdout," (done)\n");
	
	fclose(in);
	return 0;
}