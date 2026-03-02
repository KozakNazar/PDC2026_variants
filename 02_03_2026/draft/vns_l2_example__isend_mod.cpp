#define _CRT_SECURE_NO_WARNINGS 
#include "mpi.h" 
#include <stdio.h> 
#include <stdlib.h> 
#include <malloc.h> 
#include <time.h> 
#include <windows.h> 

#define USE_ISEND

double** CreateMatrix(int m, int n)
{
    double** a = (double**)malloc(m * sizeof(double*));
    for (int i = 0; i < m; i++)
        a[i] = (double*)malloc(n * sizeof(double));
    return a;
}

void DeleteMatrix(double** a, int m, int n)
{
    for (int i = 0; i < m; i++)
        free(a[i]);
    free(a);
}

void FillMatrix(double** a, int m, int n)
{
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
        {
            //a[i][j] = rand() % 10 + 1;
            a[i][j] = 1; 
        }

}

void InputMatrix(double** a, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("Enter [%d][%d] element = ", i, j);
            fflush(stdout);
            scanf_s("%lf", &(a[i][j]));
        }
        printf("\n");
    }
}

void PrintMatrix(double** a, int m, int n)
{
    int i, j;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
            printf("%8.2f\t", a[i][j]);
        printf("\n");
    }
    printf("\n");
}

void PrintMatrixToFile(FILE* f, char* title, double** a, int m, int n)
{
    fprintf(f, title);
    fprintf(f, "\n");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            fprintf(f, "%.2f\t", a[i][j]);
        fprintf(f, "\n");
    }
    fprintf(f, "\n");
}

// c = a * b, a - mxn, b - nxk, c - mxk 
double** MultMatrix(double** c, double** a, double** b, int m, int n, int k)
{
    for (int i = 0; i < m; i++)
        for (int j = 0; j < k; j++)
        {
            c[i][j] = 0;
            for (int l = 0; l < n; l++)
                c[i][j] = c[i][j] + a[i][l] * b[l][j];
        }
    return c;
}

// b = k * a 
double** MultMatrixNum(double** b, double k, double** a, int m, int n)
{
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            b[i][j] = a[i][j] * k;
    return b;
}

// c = a + b 
double** SumMatrix(double** c, double** a, double** b, int m, int n)
{
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
        {
            c[i][j] = a[i][j] + b[i][j];
        }
    return c;
}

// vector transposition (column-to-row conversion) 
void TVector(double** b, double** a, int n)
{
    for (int i = 0; i < n; i++)
        b[0][i] = a[i][0];
}

int main(int argc, char* argv[])
{
    srand((unsigned int)time(NULL));

    MPI_Init(NULL, NULL);

    LARGE_INTEGER frequency, start, end;

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2)
    {
        printf("This program requires at least two processes !\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int n;
    int in;
    int out;

    if (rank == 0)
    {
        //        printf("Enter the matrix size n : ");
        //        fflush(stdout);
        //        scanf_s("%d", &n);
        n = 534;

        // Процес з рангом 0 відправляє n процесу з рангом 1 
        MPI_Send(&n, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        const char* fileName = "Results_0.txt";
        FILE* file = fopen(fileName, "w");
        if (file == NULL)
        {
            printf("\nError opening file. Check the path and permissions !\n");
            return 1;
        }

        //        printf("\nSelect an input option : 1 - manual input, else - random input :");    
        //        fflush(stdout);
        //        scanf_s("%d", &in);
        in = 2;
        printf("\n");

        double** y1 = CreateMatrix(n, 1);
        double** y2 = CreateMatrix(n, 1);
        double** Y3 = CreateMatrix(n, n);

        if (in == 1)
        {
            // manual input vector y1 
            printf("Enter the elements of vector y1 :\n");
            InputMatrix(y1, n, 1);
            PrintMatrixToFile(file, (char*)"Vector y1 = ", y1, n, 1);

            // manual input vector y2 
            printf("Enter the elements of vector y2 :\n");
            InputMatrix(y2, n, 1);
            PrintMatrixToFile(file, (char*)"Vector y2 = ", y2, n, 1);

            // manual input matrix Y3 
            printf("Enter the elements of matrix Y3 :\n");
            InputMatrix(Y3, n, n);
            PrintMatrixToFile(file, (char*)"Matrix Y3 = ", Y3, n, n);
        }
        else
        {
            printf("Random input... \n"); \

                // random input vector y1 
                FillMatrix(y1, n, 1);
            PrintMatrixToFile(file, (char*)"Vector y1 = ", y1, n, 1);

            // random input vector y2 
            FillMatrix(y2, n, 1);
            PrintMatrixToFile(file, (char*)"Vector y2 = ", y2, n, 1);

            // random input matrix Y3 
            FillMatrix(Y3, n, n);
            PrintMatrixToFile(file, (char*)"Matrix Y3 = ", Y3, n, n);
        }

        MPI_Request * mpiRequests = new MPI_Request[3LL*n];
        for (int i = 0; i < n; ++i)
#ifdef USE_ISEND
            MPI_Isend(y1[i], 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, mpiRequests + i);
#else
            MPI_Send(y1[i], 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
#endif

        for (int i = 0; i < n; ++i)
#ifdef USE_ISEND
            MPI_Isend(y2[i], 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, mpiRequests + n + i);
#else
            MPI_Send(y2[i], 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
#endif

        for (int i = 0; i < n; ++i)
#ifdef USE_ISEND
            MPI_Isend(Y3[i], n, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, mpiRequests + 2LL*n + i);
#else
            MPI_Send(Y3[i], n, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
#endif


        double** t_y2 = CreateMatrix(1, n);

        double** T1 = CreateMatrix(n, n);
        double** T2 = CreateMatrix(n, n);
        double** T3 = CreateMatrix(n, n);
        double** T6 = CreateMatrix(n, n);
        double** T7 = CreateMatrix(n, n);

        double** t1 = CreateMatrix(1, n);

        double** x = CreateMatrix(1, 1);

        //        printf("\nWrite intermediate results to a file? : 1 - yes, else - no : ");
        //        fflush(stdout);
        //        scanf_s("%d", &out);
        out = 2;

        MPI_Send(&out, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);


        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);

        TVector(t_y2, y2, n);

        // calculation matrix T1 = y1*t_y2 
        MultMatrix(T1, y1, t_y2, n, 1, n);

        // calculation matrix T2 = T1*T1 = (y1*t_y2)*(y1*t_y2) 
        MultMatrix(T2, T1, T1, n, n, n);

        // calculation matrix T3 = T2*T1 = T1*T1*T1 = (y1*t_y2)*(y1*t_y2)*(y1*t_y2) 
        MultMatrix(T3, T2, T1, n, n, n);

        for (int i = 0; i < n; ++i)
            MPI_Recv(T6[i], n, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);

        // calculation matrix T7 = T3 + T6 = (y1*t_y2)*(y1*t_y2)*(y1*t_y2) + Y3*Y3 + t_y1* y2* Y3
        SumMatrix(T7, T3, T6, n, n);

        // calculation vector t1 = t_y2 * T7 = t_y2*((y1*t_y2)*(y1*t_y2)*(y1*t_y2) + Y3* Y3 + t_y1 * y2 * Y3)
        MultMatrix(t1, t_y2, T7, 1, n, n);

        // calculation x = t1*y1 = t_y2*((y1*t_y2)*(y1*t_y2)*(y1*t_y2) + Y3*Y3 + t_y1* y2* Y3)* y1
        MultMatrix(x, t1, y1, 1, n, 1);

        QueryPerformanceCounter(&end);
        double elapsedTime = static_cast<double>(end.QuadPart - start.QuadPart) /
            frequency.QuadPart * 1000;
        printf("\nElapsed Time: %f milliseconds on %d process\n", elapsedTime,
            rank);

        if (out == 1)
        {
            PrintMatrixToFile(file, (char*)"Matrix y1*t_y2 = ", T1, n, n);

            PrintMatrixToFile(file, (char*)"Matrix (y1*t_y2)*(y1*t_y2) = ", T2,
                n, n);

            PrintMatrixToFile(file, (char*)"Matrix (y1*t_y2)*(y1*t_y2)*(y1*t_y2) = ", T3, n, n);

            PrintMatrixToFile(file, (char*)"Matrix (y1*t_y2)*(y1*t_y2)*(y1*t_y2) + Y3 * Y3 + t_y1 * y2 * Y3 = ", T7, n, n);

            PrintMatrixToFile(file, (char*)"Vector t_y2 * ((y1 * t_y2) * (y1 * t_y2) * (y1 * t_y2) + Y3 * Y3 + t_y1 * y2 * Y3) = ", t1, 1, n);
        }

        PrintMatrixToFile(file, (char*)"x = t_y2*((y1*t_y2)*(y1*t_y2)*(y1*t_y2) + Y3 * Y3 + t_y1 * y2 * Y3)* y1 = ", x, 1, 1);
        printf("\nx = ");
        PrintMatrix(x, 1, 1);

        MPI_Waitall(3*n, mpiRequests, MPI_STATUS_IGNORE);

        DeleteMatrix(y1, n, 1);
        DeleteMatrix(y2, n, 1);
        DeleteMatrix(Y3, n, n);

        DeleteMatrix(t_y2, 1, n);

        DeleteMatrix(T1, n, n);
        DeleteMatrix(T2, n, n);
        DeleteMatrix(T3, n, n);

        DeleteMatrix(T6, n, n);
        DeleteMatrix(T7, n, n);

        DeleteMatrix(t1, 1, n);

        DeleteMatrix(x, 1, 1);


        fclose(file);
    }
    else if (rank == 1)
    {
        // Процес з рангом 1 отримує n від процесу з рангом 0 
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        const char* fileName = "Results_1.txt";
        FILE* file = fopen(fileName, "w");
        if (file == NULL)
        {
            printf("\nError opening file. Check the path and permissions !\n");
            return 1;
        }

        double** y1 = CreateMatrix(n, 1);
        double** y2 = CreateMatrix(n, 1);
        double** Y3 = CreateMatrix(n, n);

        for (int i = 0; i < n; ++i)
            MPI_Recv(y1[i], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);

        for (int i = 0; i < n; ++i)
            MPI_Recv(y2[i], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);

        for (int i = 0; i < n; ++i)
            MPI_Recv(Y3[i], n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);

        MPI_Recv(&out, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        double** t_y1 = CreateMatrix(1, n);

        double** T4 = CreateMatrix(n, n);
        double** T5 = CreateMatrix(n, n);
        double** T6 = CreateMatrix(n, n);

        double** x1 = CreateMatrix(1, 1);


        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);

        TVector(t_y1, y1, n);

        // calculation matrix T4 = Y3*Y3 
        MultMatrix(T4, Y3, Y3, n, n, n);

        // calculation x1 = t_y1*y2 
        MultMatrix(x1, t_y1, y2, 1, n, 1);

        // calculation matrix T5 = x*Y3 = t_y1*y2*Y3 
        MultMatrixNum(T5, x1[0][0], Y3, n, n);

        // calculation matrix T6 = T4 + T5 = Y3*Y3 + t_y1*y2*Y3 
        SumMatrix(T6, T4, T5, n, n);

        MPI_Request* mpiRequests = new MPI_Request[n];
        for (int i = 0; i < n; ++i)
#ifdef USE_ISEND
        MPI_Isend(T6[i], n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, mpiRequests + i);
#else
        MPI_Send(T6[i], n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
#endif
        

        QueryPerformanceCounter(&end);
        double elapsedTime = static_cast<double>(end.QuadPart - start.QuadPart) /
            frequency.QuadPart * 1000;
        printf("\nElapsed Time: %f milliseconds on %d process\n", elapsedTime,
            rank);

        if (out == 1)
        {
            PrintMatrixToFile(file, (char*)"Matrix Y3*Y3 = ", T4, n, n);

            PrintMatrixToFile(file, (char*)"t_y1*y2 = ", x1, 1, 1);

            PrintMatrixToFile(file, (char*)"Matrix t_y1*y2*Y3 = ", T5, n, n);

            PrintMatrixToFile(file, (char*)"Matrix Y3*Y3 + t_y1*y2*Y3 = ", T6, n,
                n);
        }

        MPI_Waitall(n, mpiRequests, MPI_STATUS_IGNORE);

        DeleteMatrix(y1, n, 1);
        DeleteMatrix(y2, n, 1);
        DeleteMatrix(Y3, n, n);

        DeleteMatrix(t_y1, 1, n);

        DeleteMatrix(T4, n, n);
        DeleteMatrix(T5, n, n);
        DeleteMatrix(T6, n, n);

        DeleteMatrix(x1, 1, 1);

        fclose(file);

    }

    MPI_Finalize();

    return 0;
}