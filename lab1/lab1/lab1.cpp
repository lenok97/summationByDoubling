﻿#include "pch.h"
#include <mpi.h>
#include <iostream>

using namespace std;
const int startTag = 0, endTag = 1, exchangeTag = 3;
const int root = 0;

int calculate_term(int k)
{
	return k; 
}

int main(int argc, char** argv)
{
	int rank, size, startTag = 0, endTag = 1, exchangeTag = 3;
	double endTime, startTime = 0.0;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	while (true)
	{
		long n = 0, workPerProc, startPoint, endPoint;
		long long sum = 0, temp_sum = 0;
		int nproc = size;

		if (rank == root)
		{
			cout << "Process count = " << size << endl;
			cout << "Enter n: ";
			cin >> n;
			startTime = MPI_Wtime();
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
			sum += calculate_term(number);
		
		MPI_Barrier(MPI_COMM_WORLD);

		while (nproc >1)
		{
			if (rank < nproc / 2 )
			{
				MPI_Recv(&temp_sum, 1, MPI_LONG_LONG_INT,nproc - rank - 1, exchangeTag, MPI_COMM_WORLD, &status);
				sum += temp_sum;
			}
			else 
				if (rank < nproc)  
					if  (nproc % 2 == 0 || ((nproc % 2 == 1) && (rank != nproc / 2)))
						MPI_Send(&sum, 1, MPI_LONG_LONG_INT, nproc - rank - 1, exchangeTag, MPI_COMM_WORLD);
			
			if (!(nproc % 2 == 0))
			{
				if (rank == nproc / 2)
				{
					MPI_Send(&sum, 1, MPI_LONG_LONG_INT, root, exchangeTag, MPI_COMM_WORLD);
				}

				if (rank == root)
				{
					MPI_Recv(&temp_sum, 1, MPI_LONG_LONG_INT, nproc / 2, exchangeTag, MPI_COMM_WORLD, &status);
					sum+= temp_sum;
				}

			}
			nproc /= 2;
		}

		if (rank == root)
		{
			endTime = MPI_Wtime();
			cout <<"Sum: "<< sum<< endl;
			cout.precision(3);
			cout << "Time elapsed: " << (endTime - startTime) * 1000 << "ms\n" << endl << endl;
		}
	}
	MPI_Finalize();
}
