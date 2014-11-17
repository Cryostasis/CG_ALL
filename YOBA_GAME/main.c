#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "CG_data.h"
#include "CG_vec.h"
#include "CG_mat.h"
#include "CG_error_stream.h"

#define MAXN 10

FILE * f_in;
int n, m;

void read_mat(mat_p m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n;  j++)
        {
            double c;
            fscanf(f_in, "%lf", &c);
            mat_set(m, i, j, c);
        }
}

void print_mat(mat_p m)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n;  j++)
            printf("%lf ", mat_get(m, i, j));
        printf("\n");
    }
}

void read_vec(vec_p v)
{
    for (int i = 0; i < n; i++)
    {
        double c;
        fscanf(f_in, "%lf", &c);
        vec_set(v, i, c);
    }
}

void print_vec(vec_p v)
{
    for (int i = 0; i < n; i++)
        printf("%lf ", vec_get(v, i));
    printf("\n");
}

void print_solve(vec_p v)
{
    for (int i = 0; i < n; i++)
        printf("X%d = %lf\n", i + 1, vec_get(v, i));
    printf("\n");
}

void check(double a, double b, char *str)
{
    if (fabs(a - b) > data_eps())
        printf("%s - error  ", str);
    else
        printf("%s - OK     ", str);
}

void check_pair_m(mat_p a, mat_p b, char *str, int q)
{
    if (q)
        read_mat(b);
    for (int i = 0; i < n;  i++)
    {
        printf("\n");
        for (int j = 0; j < n;  j++)
            check(mat_get(a, i, j), mat_get(b, i, j), str);
    }
    printf("\n");
    mat_destruct(&a);
}

void check_pair_v(vec_p a, vec_p b, char *str, int q)
{
    if (q)
        read_vec(b);
    for (int i = 0; i < n;  i++)
        check(vec_get(a, i), vec_get(b, i), str);
    printf("\n");
    vec_destruct(&a);
}

void solve_gauss(int nn)
{
    n = nn;
    mat_p a = mat_create(n, n);
    vec_p b = vec_create(n);
    read_mat(a);
    read_vec(b);

    printf("Solving SLE:\nA:\n");
    print_mat(a);
    printf("\nB:\n");
    print_vec(b);

    vec_p X;
    int solve = mat_gauss_solve(a, b, &X);
    printf("\nSOLVE:\n\n");
    if (!solve)
        print_solve(X);
    else
        if (solve == SL_NO)
            printf("No solutions\n");
        else
            printf("Infinity solutions\n");
}


int main()
{
    init_vec_err_stream();
    init_mat_err_stream();

    f_in = fopen("example.txt", "rt");

    //==========================VECTORS===============================
    printf(">> VECTORS\n\n");

    fscanf(f_in, "%d%d", &n, &m);
    vec_p v[MAXN], nv[MAXN], buf, buf1;
    double len[MAXN];
    for (int j = 0; j < m;  j++)
    {
        v[j] = vec_create(n);
        nv[j] = vec_create(n);
        read_vec(v[j]);
        fscanf(f_in, "%lf", &len[j]);
        read_vec(nv[j]);
    }

    for (int j = 0; j < m;  j++)
    {
        printf("Vector %d\n", j);
        check(vec_length(v[j]), len[j], "Length");
        buf = vec_normalize(v[j]);
        check_pair_v(buf, nv[j], "Normalize", 0);
        printf("\n");
    }

    double dot;
    fscanf(f_in, "%lf", &dot);
    check(dot, vec_dot(v[0], v[1]), "Dot");
    printf("\n");

    buf1 = vec_create(n);

    buf = vec_cross(v[0], v[1]);
    check_pair_v(buf, buf1, "Cross", 1);

    buf = vec_sum(v[0], v[1]);
    check_pair_v(buf, buf1, "Sum", 1);

    buf = vec_diff(v[0], v[1]);
    check_pair_v(buf, buf1, "Diff", 1);

    printf("\n");
    //==========================MATRIX===============================
    printf("\n>> MATRIX\n\n");

    mat_p M[MAXN], im[MAXN], mt, mt1;
    double det[MAXN];
    for (int j = 0; j < m;  j++)
    {
        M[j] = mat_create(n, n);
        im[j] = mat_create(n, n);
        read_mat(M[j]);
        fscanf(f_in, "%lf", &det[j]);
        read_mat(im[j]);
    }

    for (int j = 0; j < m;  j++)
    {
        printf("Matrix %d\n", j);
        check(mat_determinant(M[j]), det[j], "Determinant");
        mt = mat_invert(M[j]);
        check_pair_m(mt, im[j], "Invert", 0);
        printf("\n");
    }

    mt1 = mat_create(n, n);

    mt = mat_sum(M[0], M[1]);
    check_pair_m(mt, mt1, "Sum", 1);

    mt = mat_diff(M[0], M[1]);
    check_pair_m(mt, mt1, "Diff", 1);

    mt = mat_mul_mat(M[0], M[1]);
    check_pair_m(mt, mt1, "Mul matrix", 1);

    printf("\n");
    buf = mat_mul_vec(M[0], v[0]);
    check_pair_v(buf, buf1, "Matrix mul vector", 1);

    vec_destruct(&buf1);
    mat_destruct(&mt1);


    solve_gauss(3);
    solve_gauss(4);

    n = 4;
    mat_p k = mat_create(n, n);
    read_mat(k);
    mat_determinant(k);
    //buf1 = vec_create(-1);
    //double l = vec_length(buf1);

    printf("%d", rand() % 3);

    print_errors(vec_get_e_stream());
    print_errors(mat_get_e_stream());
	
	system("pause");
	
    return 0;
}
