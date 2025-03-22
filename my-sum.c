#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>


int checkIntegers(int, int);
int log2calc(int);
void arriveAndWait(int *, int, int);

int main()
{
	int n, m; // n = number of elements in array, m = number of cores
	char filename[100]; // char array to hold file name
	printf("Enter the number of elements in the array: ");
	scanf("%d", &n);

	printf("Enter the number of cores: ");
	scanf("%d", &m);
	
	// n and m validation
	if(checkIntegers(n, m) != 1)
		return 0;

	printf("Enter filename: ");
	scanf("%s", filename);
	
	// input file validation
	FILE *inputF = fopen(filename, "r");
	if(!inputF)
	{
		printf("ERROR: Cannot open input file\n");
		return 0;
	}
	
	int log2 = log2calc(n); // calculating log2(n)
	size_t size = (n * log2 + 1) * sizeof(int);// calculating memory size for x
	int shm_x = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666); // creating shared memory for x (array A)
	int *x = (int *)shmat(shm_x, NULL, 0); // attaching shared memory to x
	
	for(int i = 0; i < n; i++)
	{
		fscanf(inputF, "%d", &x[i]); // populating x from input file
	}

	int shm_wall = shmget(IPC_PRIVATE, m * sizeof(int), IPC_CREAT | 0666); // creating shared memory for wall
	int *wall = (int *)shmat(shm_wall, NULL, 0);// attaching shared memory to wall
	
	for(int i = 0; i < m; i++)
	{
		wall[i] = 0; // initializing wall with 0
	}

	for(int j = 0; j < m; j++)
	{
		if(fork() == 0) // if process is child process
		{
			int begin = (j * n) / m; // beginning index of process
			int end = ((j + 1) * n) / m; // ending index

			for(int p = 1; p <= log2; p++)
			{
				int pow2 = pow(2, p - 1); // calculates 2^(p - 1) and stores in pow2
				for(int i = begin; i < end; i++)
				{
					int idx = i + p * n;

					if(i < pow2)
					{
						x[idx] = x[idx - n];
					}
					else
					{
						x[idx] = x[idx - n] + x[idx - n - pow2];
					}// end else
				} // end for i
				
				arriveAndWait(wall, m, j); // process sync using wall
			}// end for p

			exit(0);

		} // end if fork == 0
	}// end for j	

	// waiting for processes to finish
	for(int i = 0; i < m; i++)
	{
		wait(NULL);
	}

	// creating output file
	FILE *outputF = fopen("output.txt", "w");
	if(!outputF)
	{
		printf("ERROR: Cannot open output file\n");
		return 0;
	}

	// printing last line of output array
	for(int i = 0; i < n; i++)
	{
		fprintf(outputF, "%d ", x[i + log2 * n]);
	}

	// closing file pointers
	fclose(inputF);
	fclose(outputF);

	// deallocating shared memory
	shmdt(x);
	shmdt(wall);
	shmctl(shm_x, IPC_RMID, NULL);
	shmctl(shm_wall, IPC_RMID, NULL);

	return 0;
}

// method that checks user input integers for n and m
int checkIntegers(int n, int m)
{
	if(n <= 0) // n = number of elements in the array
	{
		printf("ERROR: Invalid number of elements\n");
		return -1;
	}
	
	if(m <= 0) // m = number of cores
	{
		printf("ERROR: Invalid number of cores\n");
		return -2;
	}

	return 1;
}

// method that calculates log2(n) by performing change of base 
int log2calc(int n)
{
	int log2 = log10(n) / log10(2);
	return log2;
}

// barrier method that supports synchronization of process and
// prevents overwriting of shared memory
void arriveAndWait(int *wall, int m, int j)
{
	wall[j]++; // incrementing once a process reaches the barrier
	
	while(1) // waiting until all processes have reached the barrier
	{
		int hold = 1;
		for(int i = 0; i < m; i++)
		{
			if(wall[i] < wall[j])
			{
				hold = 0;
				break;
			}
		}

		if(hold)
			break;
	}
}
