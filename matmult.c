#include<stdlib.h>
#include<pthread.h>

#define Item(X, I, J, NCOL)      X[((J) + (I) * (NCOL))]

struct elements          // struct for 6 arguments
{
   	int* A;
	int* B;
	int* C;
	int l; 
	int m; 
	int n;
};
struct elements *args;  // instance to access elements
struct ele              // struct to access each element
{
	int row;
	int col;
};

/*
 * matrix multiplication
 *     C = A*B
 *     A has l rows and m cols
 *     B has m rows and n cols
 *     C has l rows and n cols
 */
void matmult(int* A, int* B, int* C, int l, int m, int n) // general multiplication function
{
    for(int i=0; i<l; i++)
        for(int j=0; j<n; j++)
        {
            int sum = 0;
            for(int k=0; k<m; k++)
                sum += Item(A, i, k, m) * Item(B, k, j, n);
            Item(C, i, j, n) = sum;
        }
}


void *elemul(void *el)          // function for multiplication to be called on function of making thread for each element
	{
	
	int sum =0;
	for (int i = 0; i<args->m;i++)
	{	
		sum += Item(args->A,((struct ele *)el)->row,i,args->m)*Item(args->B,i,((struct ele *)el)->col,args->n);
	}

	Item(args->C,((struct ele *)el)->row,((struct ele *)el)->col,args->n) = sum;
	
	
	}



/*
 * matrix multiplication
 *     C = A*B
 *     A has l rows and m cols
 *     B has m rows and n cols
 *     C has l rows and n cols
 */
void *rowmul (void *inst)       // function for multiplication to be called on function of making thread for each row
	{
		
	int row = *(((int*)inst));
		
	 for(int j=0; j<args->n; j++)
        {
            int sum = 0;
            for(int k=0; k<args->m; k++)
                sum += Item(args->A, *((int*)inst), k, args->m) * Item(args->B, k, j, args->n);
            	Item(args->C, row, j, args->n) = sum;
        }

    }
		
		



/*
 * matrix multiplication
 *     C = A*B
 *     A has l rows and m cols
 *     B has m rows and n cols
 *     C has l rows and n cols
 */
void matmult_v1(int* A, int* B, int* C, int l, int m, int n)    // function for making thread for each element (v1) 
{
    args = (struct elements *)malloc(sizeof (struct elements));
  	l=3, m=4, n=5;
	args->A =  (int*)malloc(l*m*sizeof(int));
	args->B =  (int*)malloc(m*n*sizeof(int));
	args->C =  (int*)malloc(l*n*sizeof(int));
	args->l= l;
	args->m = m;
	args->n= n;
	int Nothreads = l*n;
	int count1 = 0;
	int count2 =0;
	pthread_t t[Nothreads];
	for (int i =0; i<l; i++){
		for (int j =0; j<n; j++){
			struct ele *e =(struct ele *)malloc(sizeof(struct ele));
			e->row = i;
			e->col = j;
			pthread_create(&t[count1++],NULL, elemul, (void*)e);	
		}
	}
		for (int i =0; i<l; i++){
			for (int j =0; j<n; j++){
				pthread_join(t[count2++],NULL);
		}
	}
		




}



/*
 * matrix multiplication
 *     C = A*B
 *     A has l rows and m cols
 *     B has m rows and n cols
 *     C has l rows and n cols
 */
void matmult_v2(int* A, int* B, int* C, int l, int m, int n)    // function for making thread for each row(v2) 
{
    args = (struct elements *)malloc(sizeof (struct elements));
  	l=3, m=4, n=5;
	args->A =  (int*)malloc(l*m*sizeof(int));
	args->B =  (int*)malloc(m*n*sizeof(int));
	args->C =  (int*)malloc(l*n*sizeof(int));
	args->l= l;
	args->m = m;
	args->n= n;
    pthread_t t[l];
	for (int i = 0; i<l; i++){
		int *row = malloc(sizeof(*row));
		*row = i;
		pthread_create(&t[i],NULL, &rowmul, row);	
	}
	for (int j = 0; j<l; j++){
		pthread_join(t[j],NULL);

	}
	

}




