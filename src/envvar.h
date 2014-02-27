/* Get the value of an integer environment variable and store it in v.
 * Returns 1 on success (the variable was set and successfully parsed),
 * 0 if the variable was unset, and -1 on error. 
 * 
 * FIXME: zero is considered an invalid value because strtol doesn't set
 * errno when it can't find any digits; should check for this condition
 * explicitly. */
char get_envvar_int(const char *name, int *v);
/* Return the value of LIBMARQUISE_COLLATOR_MAX_MESSAGES if it's set,
 * default to the COLLATOR_MAX_MESSAGES #define if it's not. */
int get_collator_max_messages();