#include <stdio.h>
#include <string.h>
#include <syscall.h>

char* get_env(char *key, char **env){
	int key_len = strlen(key);
	/* Modify the prefix to search in env array*/
	char *mod_key = malloc(key_len + 2);
	memcpy(mod_key, key, strlen(key));
	mod_key[key_len] = '=';
	mod_key[key_len+1] = '\0';
	
	/* Search in env array */
	for(int i=0; env[i] != NULL; i++){
		if(strncmp(mod_key, env[i], strlen(mod_key) ) == 1){
			return (env[i] + strlen(mod_key));
		}
	}
	return NULL;

}
