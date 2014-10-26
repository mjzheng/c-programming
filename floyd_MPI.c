/* File:        p3.c
 * Author:      Musica Zheng
 *
 * Purpose:     Calculate shortest paths with Floyd's method using MPI
 * Description: The program will ask user for n and a matrix. Then, it
 *		calls the function, Floyd, to calculate the shortest 
 *		path from a vertext to another. After it's done, the
 *		program will print the final matrix out.
 *
 * Input:       n = number of vertex
 *		temp_mat = original matrix
 * Output:      Final matrix that contains the shortest paths
 *
 * Algorithm: 
 *    1. Process 0 reads in an int
 *    2. Processes call Bcast
 *    3. Process 0 reads in a matrix
 *    4. Processes call Scatter
 *    5. Processes call Floyed function
 *    6. Processes call Gather
 *    7. Process 0 prints out final matrix
 *
 * Compile:    mpicc -g -Wall -o p3 p3.c
 * Run:        mpiexec -n <number of nodes> ./p3
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

const int INFINITY = 1000000;

void Get_matrix(int temp_mat[], int n);
void Print_matrix(int local_mat[], int n, int p, int my_rank, MPI_Comm comm);
void Floyd(int local_mat[], int n, int p, int my_rank, MPI_Comm comm);

int main(void){
    	int      p, my_rank;
    	MPI_Comm comm;
    	int      n;
    	int*     temp_mat;
    	int*     local_mat;

    	MPI_Init(NULL, NULL);
    	comm = MPI_COMM_WORLD;
    	MPI_Comm_size(comm, &p);
   	MPI_Comm_rank(comm, &my_rank);

	if(my_rank == 0){
		printf("Enter n: \n");
		scanf("%d", &n);
	}
	MPI_Bcast(&n, 1, MPI_INT, 0, comm);

	temp_mat = malloc(n*n*sizeof(int));
	local_mat = malloc(n*n/p*sizeof(int));

	if(my_rank == 0){
		printf("Enter the matrix:\n");
		Get_matrix(temp_mat, n);
	}
	MPI_Scatter(temp_mat, n*n/p, MPI_INT, local_mat, n*n/p, MPI_INT, 0, comm);

	Floyd(local_mat, n, p, my_rank, comm);

	Print_matrix(local_mat, n, p, my_rank, comm);

	MPI_Finalize();
	return 0;
} /*main*/

/*-------------------------------------------------------------------
 * Function:  Get_matrix
 * Purpose:   Read in the adjacency matrix
 * In arg:    temp_mat, n
 */
void Get_matrix(int temp_mat[], int n){
	int i, j;

	for (i=0; i<n; i++){
		for(j=0; j<n; j++){
			scanf("%d", &temp_mat[i*n+j]);
		}
	}
} /*Get_matrix*/

/*-------------------------------------------------------------------
 * Function:  Floyd
 * Purpose:   Calculate the shortest path from a vertex to another
 * In arg:    local_mat, n, p, my_rank, comm
 */
void Floyd(int local_mat[], int n, int p, int my_rank, MPI_Comm comm){
	int local_city1, city2;
	int int_city, root, local_int_city, j, temp;
	int row_int_city[n];

	for(int_city = 0; int_city<n; int_city++){
		root = int_city/(n/p);
		if(my_rank == root){
			local_int_city = int_city % (n/p);
			for(j=0; j<n; j++){
				row_int_city[j] = local_mat[local_int_city*n + j];
			}
		}

		//broadcast row_int_city to all processes
		MPI_Bcast(row_int_city, n, MPI_INT, root, comm);

		for(local_city1=0; local_city1<(n/p); local_city1++){
			for(city2=0; city2<n; city2++){
				temp = local_mat[local_city1*n + int_city] + row_int_city[city2];
				if (temp < local_mat[local_city1*n+city2]){
					local_mat[local_city1*n + city2] = temp;
				}
			}
		}
	}
} /*Floyd*/

/*-------------------------------------------------------------------
 * Function:  Print_matrix
 * Purpose:   Print out the matrix from Process 0
 * In arg:    local_mat, n, p, my_rank, comm
 */
void Print_matrix(int local_mat[], int n, int p, int my_rank, MPI_Comm comm) {
   int i, j;
   int* final_mat = NULL;

   if(my_rank == 0){
   	// allocate space for final_mat
   	final_mat = malloc(n*n*sizeof(int));
   	MPI_Gather(local_mat, n*n/p, MPI_INT, final_mat, n*n/p, MPI_INT, 0, comm);
   	// print out contents of final_mat

   	printf("Final Matrix:\n");
   	for (i = 0; i < n; i++) {
      	for (j = 0; j < n; j++){
         	if (final_mat[i*n+j] == INFINITY){
                     printf("i ");
         	} else{
         	     printf("%d ", final_mat[i*n+j]);
       		}
       	}
       	printf("\n");
       	}	
        free(final_mat);
   }else{
   	MPI_Gather(local_mat, n*n/p, MPI_INT, final_mat, n*n/p, MPI_INT, 0, comm);
   }
} /*Print_matrix*/

