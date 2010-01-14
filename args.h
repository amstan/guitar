static char arg_name[100];
static char arg_value[100][256];
static int arg_n;

/*! Parse all the terminal args given
    @param narg number of arguments
    @param args list of arguments as strings
  */
void args_init(int narg, char **args);

/*! Get one argument by name
  @param name name of the argument, currently limited to 1 char
  @param default_value value to return if the given argument does not exist
  @returns the value if the argument if it exists, otherwise default_value
*/
char *args_get(char name, char *default_value);

/*! Check if an argument exists
    @param  name argument to check
	@returns 1 if name exists 0 if not
  */
int args_exists(char name);