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
 * This file contains all public function signatures and the structures used
 * to store configuration information.
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


#ifndef CONFIG_H_
#define CONFIG_H_

#include <gmp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/*
 * TODO (next release): Use hashtables (if possible (future))
 */
struct input_var {
	char *name;
	char *value;
	struct input_var *next;
};

#define MAX_BIT_LENGTH 20000
#define DEFAULT_VERBOSE 1


extern int get_verbose();
extern int get_max_bit_length();
extern int read_config_file();
extern int get_var(char **target, struct input_var *origin, char *option);
extern int deinit_config(struct input_var *origin);
#endif /* CONFIG_H_ */
