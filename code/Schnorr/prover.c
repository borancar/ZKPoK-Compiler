/*
 * Zero knowledge proof of knowledge C application framework
 *
 *  # Application flow module #
 *
 *
 * This modules contains all necessary instructions to run the application, 
 * making use of the communication and math utility module. This file
 * is automatically generated.
 *
 * #######################################################################
 *
 * This is part of the CACE (www.cace-project.eu) C-back-end of their zero
 * knowledge proof of knowledge compiler framework.
 *
 * License: GNU GPLv2 (http://www.gnu.org/licenses/gpl-2.0.html)
 *
 *
 *  Created on: Tuesday, December 06, 2011 23:41:22
 *      Author: The compiler: prover.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "includes/comm.h"
#include "includes/types.h"
#include "includes/common.h"
#include "includes/config.h"

// Include header files of external libraries (if any)

// End include

#define ENDSTATE 4
#define STARTSTATE 0


struct cace_int *t1,*t2, *t3, *t4, *sel;

int init_prover(struct cace_type *common_types[], struct cace_int *common_var[]) {
    int res = 0;
    MSG_INFO(fprintf(stderr, "\t//--------------\n\t//START init_prover\n\t//---------------\n"););
    IFNOERROR(cace_type_init(common_types[1],PRIME,160,0);); //init PRIME with bitlen 160
    IFNOERROR(cace_type_init(common_types[2],Z,get_max_bit_length(),0);); //init Z with bitlen get_max_bit_length()
    IFNOERROR(cace_type_init(common_types[0],PRIME,1024,0);); //init PRIME with bitlen 1024
    IFNOERROR(cace_type_init(common_types[5],ZPLUS,160,1);); //init ZPLUS with bitlen 160
    IFNOERROR(cace_type_init(common_types[3],ZMULT,1024,1);); //init ZMULT with bitlen 1024
    IFNOERROR(cace_int_init(common_var[12], common_types[5]);); //var=_s_1-1/_s_1 group=ZPLUS_q
    IFNOERROR(cace_int_init(common_var[13], common_types[5]);); //var=_r_1-1/_r_1 group=ZPLUS_q
    IFNOERROR(cace_int_init(common_var[0], common_types[0]);); //var=p/p group=PRIME_1024
    IFNOERROR(cace_int_init(common_var[1], common_types[1]);); //var=q/q group=PRIME_160
    IFNOERROR(cace_int_init(common_var[2], common_types[2]);); //var=SZKParameter/SZKParameter group=Z
    IFNOERROR(cace_int_assign(common_var[2], "80",1););
    IFNOERROR(cace_int_init(common_var[3], common_types[3]);); //var=y/y group=ZMULT_p
    IFNOERROR(cace_int_init(common_var[4], common_types[3]);); //var=g/g group=ZMULT_p
    IFNOERROR(cace_int_init(common_var[11], common_types[5]);); //var=x-1/x group=ZPLUS_q
    struct input_request input[] = {
        {"p", common_var[0] }, // p
        {"q", common_var[1] }, // q
        {"y", common_var[3] }, // y
        {"g", common_var[4] }, // g
        {"x", common_var[11] }, // x
    };
    IFNOERROR(request_comm(input, ARRAY_SIZE(input),0););
    MSG_INFO(fprintf(stderr,"in: p = ");cace_int_print_err(common_var[0]);fprintf(stderr,"\n"););
    MSG_INFO(fprintf(stderr,"in: q = ");cace_int_print_err(common_var[1]);fprintf(stderr,"\n"););
    MSG_INFO(fprintf(stderr,"in: y = ");cace_int_print_err(common_var[3]);fprintf(stderr,"\n"););
    MSG_INFO(fprintf(stderr,"in: g = ");cace_int_print_err(common_var[4]);fprintf(stderr,"\n"););
    MSG_INFO(fprintf(stderr,"in: x = ");cace_int_print_err(common_var[11]);fprintf(stderr,"\n"););
    //ZPLUS_q (q) = q
    IFNOERROR(cace_type_assign_param(common_types[5],common_var[1]););
    //ZMULT_p (p) = p
    IFNOERROR(cace_type_assign_param(common_types[3],common_var[0]););
    IFNOERROR(cace_calc_element(NULL,common_var[0],CHECK_MEMB, NULL););
    IFNOERROR(cace_calc_element(NULL,common_var[1],CHECK_MEMB, NULL););
    IFNOERROR(cace_calc_element(NULL,common_var[3],CHECK_MEMB, NULL););
    IFNOERROR(cace_calc_element(NULL,common_var[4],CHECK_MEMB, NULL););
    IFNOERROR(cace_calc_element(NULL,common_var[11],CHECK_MEMB, NULL););
    return res;
}
int Round0(struct cace_type *common_types[], struct cace_int *common_var[]) {
    int res = 0;
    MSG_INFO(fprintf(stderr, "\t//--------------\n\t//START Round0\n\t//---------------\n"););
    return res;
}
int Round1(struct cace_type *common_types[], struct cace_int *common_var[]) {
    int res = 0;
    MSG_INFO(fprintf(stderr, "\t//--------------\n\t//START Round1\n\t//---------------\n"););
    int length;
    char *send;
    IFNOERROR(cace_int_init(common_var[14], common_types[3]);); //var=_t_1-1-Round1/_t_1 group=ZMULT_p
    //_r_1 = RANDOM (ZPLUS, NULL)
    IFNOERROR(cace_calc(common_var[13],common_types[5],RANDOM,NULL););
    MSG_INFO(fprintf(stderr,"assign: _r_1 = ");cace_int_print_err(common_var[13]);fprintf(stderr,"\n"););
    //_t_1 = g POW _r_1
    IFNOERROR(cace_calc_element(common_var[14],common_var[4],POW,common_var[13]););
    //NULL = _t_1 CHECK_MEMB NULL
    IFNOERROR(cace_calc_element(NULL,common_var[14],CHECK_MEMB,NULL););
    MSG_INFO(fprintf(stderr,"assign: _t_1 = ");cace_int_print_err(common_var[14]);fprintf(stderr,"\n"););
    //send result
    length = cace_int_max_size(common_var, 19);
    //printf("size: %d\n",res);
    send = malloc(length);
    if(send == NULL) res = -23;
    //send _t_1-1-Round1 _t_1 1
    MSG_INFO(fprintf(stderr,"out: _t_1 = ");cace_int_print_err(common_var[14]);fprintf(stderr,"\n"););
    IFNOERROR(cace_int_serialize(send,common_var[14]););
    IFNOERROR(send_comm(send,cace_int_size(common_var[14])););
    if (send != NULL) free(send);
    return res;
}
int Round2(struct cace_type *common_types[], struct cace_int *common_var[]) {
    int res = 0;
    MSG_INFO(fprintf(stderr, "\t//--------------\n\t//START Round2\n\t//---------------\n"););
    int length;
    char *send;
    IFNOERROR(cace_type_init(common_types[4],Z,80,0);); //init Z with bitlen 80
    IFNOERROR(cace_int_init(common_var[15], common_types[4]);); //var=_c-1-Round2/_c group=_C
    IFNOERROR(cace_int_init(common_var[12], common_types[5]);); //var=_s_1-1/_s_1 group=ZPLUS_q
    struct input_request input[] = {
        {"_c", common_var[15] }, // _c
    };
    IFNOERROR(request_comm(input, ARRAY_SIZE(input),1););
    MSG_INFO(fprintf(stderr,"in: _c = ");cace_int_print_err(common_var[15]);fprintf(stderr,"\n"););
    IFNOERROR(cace_calc_element(NULL,common_var[15],CHECK_MEMB, NULL););
    //t2 = x MULT _c
    IFNOERROR(cace_calc_element(t2,common_var[11],MULT,common_var[15]););
    IFNOERROR(cace_int_push(t2););
    //_s_1 = _r_1 ADD t2
    IFNOERROR(cace_int_pop(t2););
    IFNOERROR(cace_calc_element(common_var[12],common_var[13],ADD,t2););
    //NULL = _s_1 CHECK_MEMB NULL
    IFNOERROR(cace_calc_element(NULL,common_var[12],CHECK_MEMB,NULL););
    MSG_INFO(fprintf(stderr,"assign: _s_1 = ");cace_int_print_err(common_var[12]);fprintf(stderr,"\n"););
    //send result
    length = cace_int_max_size(common_var, 19);
    //printf("size: %d\n",res);
    send = malloc(length);
    if(send == NULL) res = -23;
    //send _s_1-1 _s_1 0
    MSG_INFO(fprintf(stderr,"out: _s_1 = ");cace_int_print_err(common_var[12]);fprintf(stderr,"\n"););
    IFNOERROR(cace_int_serialize(send,common_var[12]););
    IFNOERROR(send_comm(send,cace_int_size(common_var[12])););
    if (send != NULL) free(send);
    return res;
}



/*
 * Table for state dependend functions
 * (moore automata, only one input accepted (of any type))
 */
int (*fpnt[])(struct cace_type *common_types[], struct cace_int *common_var[]) = { init_prover, Round0, Round1, Round2 };

/*
 * moore model. next state only dependent on current state (only one input accepted
 * for each state)
 */
unsigned int nextstate[] = {  1, 2, 3, 4 };



int error_handling(int error) {
	printf("error code %d\n",error);
	return 0;
}

int test_memory(void *ptr) {
	int res = 0;
	if (ptr == NULL) {
		res = -1;
		cace_set_error(ERROR_OUT_OF_MEMORY, "Out of memory in main routine"); 
	}
	return res;
}


int main(void) {
	struct cace_type *common_types[6];
	struct cace_int *common_var[19];
	struct cace_type *tmpvar_common_types;
	struct cace_int *tmpvar_common_var;
	struct cace_type_param *tmp, *tmpx;
	struct cace_type *tgroup;
	struct cace_int *tmpvar_initstore;
	struct input_var *file_inputs = NULL;
	struct input_var *file_options = NULL;
	//clock_t start, end, prgstart, prgend;
	struct timespec start, end, prgstart, prgend;
	clockid_t clock_id = CLOCK_PROCESS_CPUTIME_ID;
	int res = 0;
	int i,j;
	char *port_name;

	// Save starting time of application. Used for runtime statistics
	//prgstart = clock();
	clock_gettime(clock_id, &prgstart);
	
	// Signals shall be treated by our own handler, such that the allocated
	// resources can be freed before exiting and a report can be printed.
	if (signal (SIGPIPE, signal_handler) == SIG_IGN) {
	  //signal (SIGPIPE, SIG_IGN);
    } else if (signal (SIGSEGV, signal_handler) == SIG_IGN) {
	  //signal (SIGSEGV, SIG_IGN);
	}
	
	// Read config file
	read_config_file(&file_inputs, &file_options);

	if(get_var(&port_name, file_options, "term:port") == 0) {
	  strcat(port_name, "_prover");
	}

	MSG_INFO(fprintf(stderr, "Adding prover specific: %s!\n", port_name););
	
	// Call initalization functions of external libraries (if any)
	

	// Initialize random function with the current time as seed.
	// For real world deployments this has to be changed to use the
	// operating systems random interface. Using the time as seed has the 
	// advantage that we have no need to code/compile OS specific.
	srand((unsigned)time(NULL));

	// Get memory for all common group structures
	IFNOERRORX(tmpvar_common_types = (struct cace_type*)calloc(ARRAY_SIZE(common_types), sizeof(struct cace_type)););
	IFNOERROR(test_memory(tmpvar_common_types););
	for (i = 0; res == 0 && i < ARRAY_SIZE(common_types); i++) {
		common_types[i] = &tmpvar_common_types[i];
	}
	
	// Get memory for all common variables
	IFNOERRORX(tmpvar_common_var = (struct cace_int*)calloc(ARRAY_SIZE(common_var), sizeof(struct cace_int)););
	IFNOERROR(test_memory(tmpvar_common_var););
	for (i = 0; res == 0 && i < ARRAY_SIZE(common_var); i++) {
		common_var[i] = &tmpvar_common_var[i];
		common_var[i]->initialized = 0;
	}


	// Initialize the temporary variables     
	tgroup = NULL;
	t1 = NULL;
	t2 = NULL;
	t3 = NULL;
	t4 = NULL;
	sel = NULL;
	IFNOERRORX(tgroup = malloc(sizeof(struct cace_type)););
	IFNOERROR(test_memory(tgroup););
	IFNOERRORX(tmpvar_initstore = (struct cace_int*)calloc(5,sizeof(struct cace_int)););
	IFNOERROR(test_memory(tmpvar_initstore););
	if (res >= 0) {
		t1 = &tmpvar_initstore[0];
		cace_int_init(t1,tgroup);
		t2 = &tmpvar_initstore[1];
		cace_int_init(t2,tgroup);
		t3 = &tmpvar_initstore[2];
		cace_int_init(t3,tgroup);
		t4 = &tmpvar_initstore[3];
		cace_int_init(t4,tgroup);
		sel = &tmpvar_initstore[4];
		cace_int_init(sel,tgroup);
		sel->initialized = 0;
		sel->known = 0;
	}



	// Initialize communication module
	IFNOERROR(init_comm(file_inputs, file_options););
	// Initialize reporting module
    IFNOERROR(init_reporting(file_inputs, file_options););

    // Now let the protocol begin:
	int state=STARTSTATE;
	if (res < 0) {
		state = ENDSTATE;
	}
	while(state != ENDSTATE) {
		//start = clock();
		clock_gettime(clock_id, &start);

		// execute current state
		res = (*fpnt[state])(common_types,common_var);
		//end = clock();
		clock_gettime(clock_id, &end);
		
		// Print statistics for round
		MSG_INFO(
			fprintf(stderr,"Statistics for state %d:\n\tcpu time start: %d.%.9ld\n\tcpu time end: %d.%.9ld" 
				"\n\texecution time: %d.%.9ld sec.\n\treturn code: %d\n",
				state,
				(int)start.tv_sec,
				start.tv_nsec,
				(int)end.tv_sec,
				end.tv_nsec,
				(start.tv_nsec < end.tv_nsec) ?
					(int)(end.tv_sec - start.tv_sec):
					(int)(end.tv_sec - start.tv_sec)-1,
				(start.tv_nsec < end.tv_nsec) ? 
					(long)(end.tv_nsec - start.tv_nsec):
					(long)((1000000000 - start.tv_nsec) + end.tv_nsec),  
				res);
			);

		// Get next state
		if (res < 0) {
			//error_handling(res);
			state = ENDSTATE;
		} else {
			state=nextstate[state];
		}

	}
	// The protocol run is finished (or ended in an error)
	
	
	// Debug-print groups
	MSG_DEBUG(4,
		fprintf(stderr, "main: list of groups\n");
		for (i = 0; i < ARRAY_SIZE(common_types); i++) {
			j = 0;
			fprintf(stderr,"main: group: %d, bitlen: %d\n", common_types[i]->type, 
					common_types[i]->bitlen);
			MSG_DEBUG(5,
				tmp = common_types[i]->params;
				while (tmp != NULL) {
					fprintf(stderr, "\tparam %d:",j++);
					cace_int_print_err(tmp->param);
					fprintf(stderr, "\n");
					tmp = tmp->next;
				}); 
		});
	
	MSG_DEBUG(4, fprintf(stderr, "main: finalize start\nmain: free group:"););
	
	// Clear up common & temporary groups
	if (tgroup != NULL) {
		free(tgroup);
	}
	for (i = 0; i < ARRAY_SIZE(common_types); i++) {
		MSG_DEBUG(6, fprintf(stderr, " %d", i););
		//free the params (singly linked list!)
		if (common_types[i]->params != NULL) {
			tmp = common_types[i]->params;
			while (tmp != NULL) {
				tmpx = tmp->next;
				//if (tmp->param->initialized == 1) {
				//	mpz_clear(*tmp->param->value1);
				///	tmp->param->value1 = NULL;
				//} 
				free(tmp);
				tmp = NULL;
				tmp = tmpx;
			}
		}
	}
	if (tmpvar_common_types != NULL) {
		free(tmpvar_common_types);
	} 
	MSG_DEBUG(4, fprintf(stderr, "\nmain: free var:"););
	
	// Clear up temporary variables
	if (t1 != NULL) {
		mpz_clear(*t1->value1);
		free(t1->value1);
		t1->value1 = NULL;
	}
	if (t2 != NULL) {
		mpz_clear(*t2->value1);
		free(t2->value1);
		t2->value1 = NULL;
	}
	if (t3 != NULL) {
		mpz_clear(*t3->value1);
		free(t3->value1);
		t3->value1 = NULL;
	}
	if (t4 != NULL) {
		mpz_clear(*t4->value1);
		free(t4->value1);
		t4->value1 = NULL;
	}
	if (tmpvar_initstore != NULL) {
		free(tmpvar_initstore);
	}

	for (i = 0; i < ARRAY_SIZE(common_var); i++) {
		MSG_DEBUG(6, fprintf(stderr, " %d", i););
		if (common_var[i] != NULL && common_var[i]->initialized == 1) {
			mpz_clear(*common_var[i]->value1);
			
			// Free the values
			free(common_var[i]->value1);
			common_var[i]->value1 = NULL;
		}

	}
	if (tmpvar_common_var != NULL) {
		free(tmpvar_common_var);
	}
	MSG_DEBUG(4, fprintf(stderr, "\n"););

	// Clean up stack
	cace_int_stack_deinit();
	
	// Any other finalize tasks
	deinit_config(file_inputs);
	deinit_config(file_options);
	stop_comm();
	
	// Print statistics for application
	MSG_INFO(
		//prgend = clock();
		clock_gettime(clock_id, &prgend);
		fprintf(stderr,"Running time for application:"
				"\n\tcpu time start: %d.%.9ld\n\tcpu time end: %d.%.9ld" 
				"\n\texecution time: %d.%.9ld sec.\n\treturn code: %d\n"
				"\t(Timing information might be wrong on multiprocessor systems,\n"
				"\tsince it is defered from the per-processor timer value)",
				(int)prgstart.tv_sec,
				prgstart.tv_nsec,
				(int)prgend.tv_sec,
				prgend.tv_nsec,
				(prgstart.tv_nsec < prgend.tv_nsec) ?
					(int)(prgend.tv_sec - prgstart.tv_sec):
					(int)(prgend.tv_sec - prgstart.tv_sec)-1,
				(prgstart.tv_nsec < prgend.tv_nsec) ? 
					(long)(prgend.tv_nsec - prgstart.tv_nsec):
					(long)((1000000000 - prgstart.tv_nsec) + prgend.tv_nsec),  
				res);
			);
		/*
		fprintf(stderr,"Running time for application:\n\tcpu cycles: %d\n"
				"\tcpu time: %.6f sec.\n",(int)(prgend-prgstart), 
				(double) (prgend-prgstart)/CLOCKS_PER_SEC);
		);*/

	// Print information on success failure
	// TODO: improve and allow result to be quite (i.e. state of return
	// given by exit code)
	cace_print_error();
	if (res > 0) {
		printf(">>  Verification successful  <<\n");
	}
	
	
	
	return res;
}


