// blinker_devmem_test.c

#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <getopt.h>
#include <error.h>
#include <unistd.h>
#include <dirent.h>
#include <gpio_test_devmem.h>
#include <string.h>
#include <hps.h>
#include <hps_0.h>

// command line parameters identification
void *command_write_leds    = NULL;
void *command_read_switches = NULL;
void *command_help			= NULL;

// parameter value when applicable
unsigned char param_value;

// funciton prototypes
void validate_soc_system(void);
void parse_cmdline(int argc, char ** argv);
void do_write_leds(void *blinker_driver_map);
void do_read_switches(void *blinker_driver_map);
void do_help(void);

//main
int main (int argc, char **argv)
{
	int devmem_filedesc;
	void *blinker_map;
	int result;
	
	//validate the features of the actual hardware
	validate_soc_system();
	
	//parse the command line arguments
	parse_cmdline(argc, argv);
	
	// open the /dev/mem device
	devmem_filedesc = open ("/dev/mem", O_RDWR | O_SYNC);
	
	if(devmem_filedesc < 0)
	{
		printf("devmem open");
		exit(EXIT_FAILURE);
	}
	
	//map the base of the blinker hardware
	blinker_map = mmap(	NULL, 
					   	sysconf(_SC_PAGE_SIZE), 
					   	PROT_READ | PROT_WRITE, 
					   	MAP_SHARED, devmem_filedesc, 
					   	ALT_LWFPGASLVS_OFST + GPIO_TEST_0_BASE);
	
	if(blinker_map == MAP_FAILED)
	{
		printf("devmem mmap");
		close(devmem_filedesc);
		exit(EXIT_FAILURE);
	}
	
	//perform the operation selected by the command line arguments
	if(command_write_leds       != NULL) { do_write_leds(blinker_map); }
	if(command_read_switches    != NULL) { do_read_switches(blinker_map); }
	if(command_help 		    != NULL) { do_help(); }
	
	//unmap the blinker and close the /dev/mem file descriptor
	result = munmap(blinker_map, sysconf(_SC_PAGE_SIZE));
	
	if(result < 0) 
	{
		printf("devmem munmamp");
		close(devmem_filedesc);
		exit(EXIT_FAILURE);
	}
	
	close(devmem_filedesc);
	exit(EXIT_SUCCESS);
}


/*** functions ***/

/* validate_soc_system */
void 
	validate_soc_system
		(void)
{
	const char *dirname;
	DIR *ds;
	
	//verify that the blinker device entry exists in the sysfs
	dirname = GPIO_TEST_SYSFS_ENTRY;
	ds = opendir(dirname);
	
	if(ds == NULL)
	{
		printf("ERROR: blinker may not be present in the system hardware");
		exit(EXIT_FAILURE);
	}
	
	if(closedir(ds))
	{
		printf("ERROR: blinker may not be present in the system hardware");
		exit(EXIT_FAILURE);
	}
	
	// verify the the blinker base address is page aligned
	if((ALT_LWFPGASLVS_OFST + GPIO_TEST_0_BASE) & (sysconf(_SC_PAGE_SIZE) - 1))
	{
		printf("ERROR: blinker base 0x%08X is not page aligned", 
			   ALT_LWFPGASLVS_OFST + GPIO_TEST_0_BASE);
		printf("Page size = 0x%08lX", (sysconf(_SC_PAGE_SIZE) - 1));
	}
}

/* do_write_speed */
void do_write_leds (void *blinker_driver_map)
{
    volatile unsigned char *blinker_led_reg = blinker_driver_map + GPIO_TEST_REG_LEDS_OFFSET;

    *blinker_led_reg = param_value;
}

/* do_read_led */
void do_read_switches (void *blinker_driver_map)
{
    volatile unsigned char *blinker_switches_reg = blinker_driver_map + GPIO_TEST_REG_SWITCHES_OFFSET;

    unsigned char dato = *blinker_switches_reg;
    printf("switches = %u\n", dato & 0xff);
}

/* do_help */
void do_help(void)
{
	puts(HELP_STR);
	puts(USAGE_STR);
}

/* parse_cmdline */
void parse_cmdline(int argc, char **argv)
{
	int command;
	int opt_index = 0;
	int action_count = 0;

	static struct option long_options[] =
	{
	    { "write_led", required_argument, NULL, 'l' },
		{ "read_switches", no_argument, NULL, 's' },
		{ "help", no_argument, NULL, 'h' },
	};
	
	//parse the command line arguments
	while(1)
	{
		command = getopt_long( argc, argv, "l:sh", long_options, &opt_index);
		
		if(command == -1) { break; }
		
		switch (command)
		{
			case 0:
			{
				puts(USAGE_STR);
				error(1, 0, "ERROR: wrong command line options.");
				break;
			}

			case 'l':
			{
			    command_write_leds = &command_write_leds;
			    param_value = atoi(optarg);
			    break;
			}
				
			case 's':
			{
				command_read_switches = &command_read_switches;
				break;
			}
				
			case 'h':
			{
				command_help = &command_help;
				break;
			}
				
			default:
			{
				puts(USAGE_STR);
				error(1, 0, "ERROR: wrong command line options.");
				break;
			}
		}
	}
	
	// There was any extra characters on the comman line. the program exit here
	if(optind < argc)
	{
		printf("extra characters on command line: ");
		
		while(optind < argc) printf("%s\n", argv[optind++]);
		puts(USAGE_STR);
		error(1, 0, "ERROR: wrong command line options");
	}
	
	//verify that the user only request one action to perform
	if(command_write_leds       != NULL) { action_count++; }
	if(command_read_switches    != NULL) { action_count++; }
	if(command_help             != NULL) { action_count++; }
	
	if(action_count == 0)
	{
		puts(USAGE_STR);
		error(1, 0, "ERROR: no options parsed");
	}
	
	if(action_count > 1)
	{
		puts(USAGE_STR);
		error(1, 0, "ERROR: too many options parsed");
	}
}
