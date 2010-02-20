#include <string.h>
#include "args.h"

void args_init(int narg, char **args) {
	int i;
	arg_n=0;
	
	for(i=1;i<narg;i++) {
		if(args[i][0]=='-') {
			//it's an option
			arg_name[arg_n]=args[i][1];
			arg_n++;
		}
		
		if((arg_n!=0)&&(args[i][0]!='-')) {
			//it's a string
			//add it to the previous option
			strcpy(arg_value[arg_n-1],args[i]);
		}
	}
}

char *args_get(char name, char *default_value) {
	int i;
	
	for(i=arg_n-1;i>=0;i--) //go downward so you get the most recent first
		if(arg_name[i]==name)
			return arg_value[i];
	
	return default_value;
}

int args_exists(char name) {
	int i;
	
	for(i=0;i<arg_n;i++)
		if(arg_name[i]==name)
			return 1;
	
	return 0;
}

/*
#include <stdio.h>
void args_debug() {
	printf("arguments parsed:\n");
	for(int i=0;i<arg_n;i++) {
		printf(" -%c: \"%s\"\n",arg_name[i],get_arg(arg_name[i],"error"));
	}
} //*/