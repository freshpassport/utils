#include <stdio.h>

int main()
{
	  int bytes_read;
	    int nbytes = 100;
	      char *my_string;

	        puts ("Please enter a line of text.");

		  /* These 2 lines are the heart of the program. */
		  my_string = (char *) malloc (nbytes + 1);
		    //bytes_read = getline (&my_string, &nbytes, stdin);
		    bytes_read = getdelim(&my_string, &nbytes, '\n', stdin);

		      if (bytes_read == -1)
			          {
					        puts ("ERROR!");
						    }
		        else
				    {
					          puts ("You typed:");
						        puts (my_string);
							    }

			  return 0;
}


