#include "pch.h"
#include <mpi.h>
#include <iostream>

using namespace std;
const int tag = 0;
const int root = 0;

int calculate_term(int k)
{
	return k; 
}

int main(int argc, char** argv)
{
	double sum, temp_sum=0, endTime, startTime = 0.0;
	int rank, size, startTag = 0, endTag = 1, exchangeTag = 3;
	unsigned int n=0, workPerProc, startPoint, endPoint, nproc;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	nproc = size;
	if (rank == root)
	{
		cout << "Process count = " << size << endl;
		cout << "Enter n: ";
		cin >> n;
		workPerProc = n / size;
		int rest = n % size;
		int start = 0;
		int end = workPerProc;
		startPoint = start;
		endPoint = end;
		for (int i = 1; i < size; i++)
		{
			start = end + 1;
			end = start + workPerProc - 1;
			if (rest > 0)
			{
				end++;
				rest--;
			}
			MPI_Send(&start, 1, MPI_INT, i, startTag, MPI_COMM_WORLD);
			MPI_Send(&end, 1, MPI_INT, i, endTag, MPI_COMM_WORLD);
		}
	}

	if (rank != root)
	{
		MPI_Recv(&startPoint, 1, MPI_INT, root, startTag, MPI_COMM_WORLD, &status);
		MPI_Recv(&endPoint, 1, MPI_INT, root, endTag, MPI_COMM_WORLD, &status);
	}

	for (int number = startPoint; number <= endPoint; number++)
		temp_sum+=calculate_term(number);
	cout << " while ";

	while (nproc > 1) 
	{
		cout <<" while ";
		if (rank < nproc / 2) 
		{
			MPI_Recv(&sum, 1, MPI_DOUBLE, nproc - rank - 1, exchangeTag, MPI_COMM_WORLD, &status);
			temp_sum += sum;
		}
		else if (rank < nproc)
			MPI_Send(&temp_sum, 1, MPI_DOUBLE, nproc - rank - 1, exchangeTag, MPI_COMM_WORLD);
		nproc = nproc / 2;
	}

	if (rank== root)
	{
		cout << sum;
	}

	MPI_Finalize();
}