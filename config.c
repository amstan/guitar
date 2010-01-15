#include <stdio.h>
#include <string.h>
#include "config.h"

int config_init(char *file) {
	FILE *in;
	
	fprintf(stdout,"Loading configuration (\"%s\")... ",file);
	
	in=fopen(file,"r");
	if(!in) {
		fflush(stdout);
		fprintf(stderr, "Error: Cannot open config file \"%s\"!\n",file);
		return 1;
	}
	
	while(fscanf(in,"%s",config_name[config_n])!=EOF) {
		fscanf(in,"%*[ =:]s"); //skip the assignment
		fscanf(in,"%[^\n]s",config_value[config_n]); //value
		config_n++;
		if(config_name[config_n-1][0]=='#') config_n--;
	}
	
	fprintf(stdout,"(done)\n");
	fclose(in);
	return 0;
}

char *config_look(char *name) {
	char i;
	for(i=config_n-1;i>=0;i--)
		if(strcmp(name,config_name[i])==0)
			return config_value[i];
	return "";
}