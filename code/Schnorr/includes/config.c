/*
 * Zero knowledge proof of knowledge C application framework
 *
 *  # Configuration utility module #
 *
 *
 * This modules is responsible to read and give access to options and global
 * inputs read from a configuration file.
 *
 * #######################################################################
 *
 * This file contains all functions needed to read the configuraiton file as
 * well as access the stored resources.
 *
 * #######################################################################
 *
 * This is part of the CACE (www.cace-project.eu) C-back-end of their zero
 * knowledge proof of knowledge compiler framework
 *
 * License: GNU GPLv2 (http://www.gnu.org/licenses/gpl-2.0.html)
 *
 *  Created on: 03.10.2009
 *      Author: Andreas Gruenert
 */


#include "config.h"
#include "types.h"
#include "mexp.h"

// The file where options and initial values are stored
#define CONFIG_FILE "test.config"
// For memory management: The default allocated space for option names and
// variables. If need exceeds this value it is extended with STANDARD_VARSIZE^2
// (ad infinitum)
#define STANDARD_VARSIZE 15
// There are in particular two general options (not module specific) that are
// necessary for the application to run:
// - 'main:verbose' and 'main:maxbitlen'.
// If these values are not given default values are used:
int verbose = DEFAULT_VERBOSE;
int max_bit_length = MAX_BIT_LENGTH;

/**
 * Returns the configured verbose level. If this function is called before
 * the configuration file is read, it will return the default value and not the
 * specified one.
 *
 * @return (int): Level telling how verbose the output shall be.
 */
int get_verbose() {
	return verbose;
}

/**
 * Returns the maximum bit length for variables whose bitlength is not
 * predefined by the compiler. If this function is called before
 * the configuration file is read, it will return the default value and not the
 * specified one.
 *
 * This setting has to reflect the needs of the protocol. Chosen wrong, the
 * protocol might fail to work as intended.
 *
 * @return (int): The maximum bit length of any undefined variable
 */
int get_max_bit_length() {
	return max_bit_length;
}

/**
 * Returns a value from a requested key or a linked list with name -> value
 * mapping. The value will be stored in /target/
 *
 * @return (int): -1 if no value was found, 0 otherwise
 */
int get_var(char **target, struct input_var *origin, char *option) {
	struct input_var *tmp;
	int res = -1, len;
	tmp = origin;
	//printf("get_var: %s\n",option);
	while (tmp != NULL && res < 0) {
		//printf("try..: %d\n",res);
		len = strlen(option);
		// We only need to do expensive string comparison if they have the
		// same length
		if (strlen(tmp->name) == len) {
			if (strncmp(option, tmp->name, len) == 0) {
				*target = tmp->value;
				res = 0;
			}
		}
		tmp = tmp->next;
	}
	return res;
}

/**
 * Frees the given linked list structures used to store options or inputs
 */
int deinit_config(struct input_var *origin) {
	struct input_var *tmp, *tmpx;
	tmp = origin;
	while (tmp != NULL) {
		tmpx = tmp;
		tmp = tmp->next;
		free(tmpx->name);
		free(tmpx->value);
		free(tmpx);
	}
	return 0;
}

/**
 * Attaches a key -> value pair to a linked list
 *
 * @return (int): 0
 */
int attach_input(struct input_var *input, struct input_var **target) {
	struct input_var *tmp;
	//printf("attach: %s",input->name);
	if (*target == NULL) {
		//printf("to begin\n");
		*target = input;
	} else {
		//printf("to end");
		tmp = *target;
		while (tmp->next != NULL) {
			//printf(".");
			tmp = tmp->next;
		}
		//printf("\n");
		tmp->next = input;
		tmp->next->next = NULL;
	}
	return 0;
}

/**
 * Reads a section of the configuration file and attaches all key -> value
 * pairs to the linked list /target/.
 *
 * The function reads values satisfying the grammar:
 * entries := entry*
 * entry   := ' '* chars* ' '* '=' ' '* chars* ' '* '\n'
 * chars   := [^\ \n\[]*
 *
 * The implementation is based on a simple state pattern, defining each position
 * in the entry as a state.
 *
 * @precondition: /pos/ is the correct position of a section content in file
 * 		handler /fd/
 * @returns (int): the position in /fd/ where the next section header begins or
 * 		0 on EOL
 */
int read_inputs(int fd, int pos, struct input_var **target) {
	int res = 0, retval = 0;
	int var_count, val_count, val_start, val_recount, var_size;
	char *val;
	char c = '\0';
	struct input_var *current_input;
	enum states {BEFORE_VAR, VAR, BEFORE_EQUAL, EQUAL, BEFORE_VAL, VAL_COUNT,
		VAL, AFTER_VAL, END} state;

	current_input = malloc(sizeof(struct input_var));
	if (current_input == NULL) {
		cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in config:read_inputs()");
		return -1;
	}
	current_input->name = malloc(STANDARD_VARSIZE);
	if (current_input->name == NULL) {
		cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in config:read_inputs()");
		return -1;
	}


	// Go to correct location
	IFNOERROR(lseek(fd, 0L, SEEK_SET););
	IFNOERROR(lseek(fd, pos, SEEK_CUR););


	var_count = 0;
	val_count = 0;
	val_start = 0;
	val_recount = 0;
	var_size = STANDARD_VARSIZE;
	if (res < 0) {
		state = END;
	} else {
		state = BEFORE_VAR;
	}

	do {
		if (state == BEFORE_VAR) {
			res = read(fd,&c,1);
			if (c != ' ') {
				state = VAR;
				current_input->name[var_count++] = c;
			}
		}
		if (state == VAR) {
			if (var_count >= var_size) {
				var_size *= 2;
				current_input->name = realloc(current_input->name, var_size);
			}
			res = read(fd,&c,1);
			if (c == '=') {
				state = BEFORE_VAL;
				current_input->name[var_count] = '\0';
			} else if (c == ' ') {
				state = BEFORE_EQUAL;
				current_input->name[var_count] = '\0';
			} else {
				current_input->name[var_count++] = c;
			}
		}
		if (state == BEFORE_EQUAL) {
			res = read(fd,&c,1);
			if (c == '=') {
				state = BEFORE_VAL;
			}
		}
		if (state == BEFORE_VAL) {
			res = read(fd,&c,1);
			if (c != ' ') {
				state = VAL_COUNT;
				val_start = lseek(fd, 0L, SEEK_CUR) -1;
			}
		}
		if (state == VAL_COUNT) {
			res = read(fd,&c,1);
			if (c == ' ' || c == '\n' || res == 0) {
				state = VAL;
				val = malloc(val_count + 2);
				if (val == NULL) {
					res = -1;
					state = END;
				} else {
					IFNOERROR(lseek(fd, 0L, SEEK_SET););
					IFNOERROR(lseek(fd, val_start, SEEK_CUR););
					if (res < 0) state = END;
				}
				val_recount = 0;
			} else {
				val_count++;
			}
		}
		if (state == VAL) {
			res = read(fd,&c,1);
			if (c == ' ' || c == '\n' || res == 0) {
				//TODO: how to raise error..
				//error
			}
			if (val_recount <= val_count) {
				val[val_recount] = c;
			} else {
				if (c == '\n') {
					state = END;
				} else {
					state = AFTER_VAL;
				}
				val[val_recount] = '\0';
				current_input->value = val;
				attach_input(current_input, target);
			}
			val_recount++;
			res = 1;
		}
		if (state == AFTER_VAL) {
			res = read(fd,&c,1);
			if (c == '\n') {
				retval = lseek(fd, 0L, SEEK_CUR);
			}
			if (res == 0) {
				retval = res;
			}
		}
		if (state == END) {
			retval = lseek(fd, 0L, SEEK_CUR);
			//printf("res: %d, retval: %d\n",res,retval);
		}
	} while (res > 0 && retval == 0);

	if (retval > 0) {
		return retval;
	} else {
		return res;
	}
}

/**
 * Reading the head of the section and deciding if the section content has to be
 * read and where the key -> value pairs given in the section are to be stored.
 *
 * @precondition: /pos/ points to the beginning of a section head
 * @return (int): 1 for section options, 2 for inputs and -1 if the section is
 * 		is unknown
 */
int read_section_head(int fd, int pos) {
	char c = '\0';
	// The known sections
	const char *sections[] = { "[options]", "[inputs]" };
	int i,j, option_length;
	int res = 0, res_pos = -1;

	i = 0;
	//printf("parsing head... section size:  %d, section i: %d\n",ARRAY_SIZE(sections), strlen(sections[1]));
	while (i < ARRAY_SIZE(sections) && res_pos < 0 && res >= 0) {
		// Move to section start
		IFNOERROR(lseek(fd, 0L, SEEK_SET););
		IFNOERROR(lseek(fd, pos, SEEK_CUR););
		if (res >= 0) {
			option_length = strlen(sections[i]);
			j = -1;
			do {
				j++;
				res = read(fd,&c,1);
			} while (j < option_length && sections[i][j] == c && res > 0);
			if (j != option_length) {
				// pos = not found == -1
				res_pos = -1;
			} else {
				res_pos = i;
			}
			i++;
		}
	}
	//printf("found section %d\n",res_pos);
	if (res >= 0) {
		return res_pos;
	} else {
		return res;
	}
}

/**
 * Reads the config file. The file has to satisfy the grammar:
 * configfile   := (options+ inputs+ unknown+)
 * options      := '[options]' '\n' sectioncontent
 * inputs		:= '[inputs]' '\n' sectioncontent
 * unknown       := '[' .* ']' '\n' .*
 * sectioncontent := entries* (see read_inputs() for grammar of entries).
 *
 * The implementation makes use of a simple state pattern. Each position in the
 * grammar is represented by a state.
 *
 */
int read_config_file(struct input_var **file_inputs, struct input_var **file_options) {
	int fd;
	int current;
	int res = 0;
	int section;
	char *option_value;
	char c = '\0';
	enum states {PRE=1, SECTION_HEAD, INPUTS, OPTIONS, UNKNOWN} state;


	fd = open(CONFIG_FILE,O_RDONLY);

	// Go to the beginning of the file
	IFNOERROR(lseek(fd, 0L, SEEK_SET););
	if (res < 0) {
		state = UNKNOWN;
	} else {
		state = PRE;
	}

	//read inputs
	do {
		// Save current position of file pointer
		current = lseek(fd, 0L, SEEK_CUR);
		if (state == PRE) {
			res = read(fd,&c,1);
			//printf("got charx: %c\n", c);
			if (c == '[') {
				state = SECTION_HEAD;
			} else {
				// Move to next line
				while (res > 0 && c != '\n') {
					res = read(fd,&c,1);
				}
			}
		}
		if (state == UNKNOWN) {
			res = -2;
		}
		if (state == INPUTS || state == OPTIONS) {
			res = read(fd,&c,1);
			//printf("got chary: %c\n", c);
			if (res < 0) {
				res = -1;
				//TODO: cace_set_eror()
			} else if (c == '[') {
				state = SECTION_HEAD;
			}
			else if (c == '/') {
				// Comment, move to next line
				while (res > 0 && c != '\n') {
					res = read(fd,&c,1);
				}
			}
			else if (c == '\n') {
				// Empty line: do nothing
			}
			else {
				res = read(fd,&c,1);
				if (state == INPUTS) current = read_inputs(fd, current, file_inputs);
				else if (state == OPTIONS) current = read_inputs(fd, current, file_options);

				//printf("config:inputs: %X\n",(unsigned long)*file_inputs);

				//printf("config:options: %X\n",(unsigned long)*file_options);

				// In case of EOF while reading input, read_inputs() returns 0
				// handing it to the res(ult) variable will stop the loop
				IFNOERRORX(res = current;);

				IFNOERROR(lseek(fd, 0L, SEEK_SET););
				IFNOERROR(lseek(fd, current, SEEK_CUR););
			}
		}
		if (state == SECTION_HEAD) {
			section = read_section_head(fd,current);

			if (section == 0) state = OPTIONS;
			else if (section == 1) state = INPUTS;
			else state = UNKNOWN;

			// Move to next line
			res = 1;
			while (res > 0 && c != '\n') {
				res = read(fd,&c,1);
			}
		}
	} while (res > 0);

	/**
	 * Read and store global options
	 */
	if ((res = get_var(&option_value, *file_options, "main:verbose")) == 0) {
		//printf("verbose option: %s\n",option_value);
		verbose = strtol(option_value, NULL, 10);
	}
	//printf("verbose: %d\n",verbose);
	if ((res = get_var(&option_value, *file_options, "main:maxbitlen")) == 0) {
		max_bit_length = strtol(option_value, NULL, 10);
	}

	close(fd);
	return res;
}
