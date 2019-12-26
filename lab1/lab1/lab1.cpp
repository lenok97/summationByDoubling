#include "pch.h"
#include <mpi.h>
#include <iostream>

using namespace std;
const int startTag = 0, endTag = 1, exchangeTag = 3;
const int root = 0;

int calculateTerm(int k)
{
	return 1; 
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
		long long sum = 0, tempSum = 0;
		int groupSize = size;

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
			sum += calculateTerm(number);
		
		MPI_Barrier(MPI_COMM_WORLD);

		while (groupSize >1)
		{
			int pairNum = groupSize - rank - 1;
			if (rank < groupSize / 2) // процессы, из первой половины группы, принимают элемент от своих пар из второй половины
			{
				MPI_Recv(&tempSum, 1, MPI_LONG_LONG_INT, pairNum, exchangeTag, MPI_COMM_WORLD, &status);
				sum += tempSum;
			}
			else // вторая часть группы отправляет свой элемент
			{
				if (groupSize % 2 == 1 && rank == groupSize / 2) // непарный элемент отправляет мастеру
					pairNum = root;
				MPI_Send(&sum, 1, MPI_LONG_LONG_INT, pairNum, exchangeTag, MPI_COMM_WORLD);
			}
			if (groupSize % 2 == 1 && rank == root) // мастер принимает элемент от непарных процессов из подгруппы 
			{
				pairNum = groupSize / 2;
				MPI_Recv(&tempSum, 1, MPI_LONG_LONG_INT, pairNum, exchangeTag, MPI_COMM_WORLD, &status);
				sum += tempSum;
			}
			groupSize /= 2; // уменьшаем размер подгруппы в два раза
			if (rank > groupSize)
				break;
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
