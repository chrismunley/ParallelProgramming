# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>
# include <omp.h>

int main ( void );
void timestamp ( void );

/******************************************************************************/

int main ( void )

/******************************************************************************/
/*
  Purpose:

    MAIN is the main program for MXM_OPENMP.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    13 October 2011

  Author:

    John Burkardt
    
    
  John Burkardt wrote this mxm code with OpenMP directives, and I took the code and converted it to OpenACC to test
  the efficiency and run this code on a GPU
*/
{
  double a[500][500];
  double angle;
  double b[500][500];
  double c[500][500];
  int i;
  int j;
  int k;
  int n = 500;
  double pi = 3.141592653589793;
  double s;
  int thread_num;
  double wtime;

  timestamp ( );

  printf ( "\n" );
  printf ( "MXM_OPENMP:\n" );
  printf ( "  C/OpenMP version\n" );
  printf ( "  Compute matrix product C = A * B.\n" );

  thread_num = omp_get_max_threads ( );

  printf ( "\n" );
  printf ( "  The number of processors available = %d\n", omp_get_num_procs ( ) );
  printf ( "  The number of threads available    = %d\n", thread_num );

  printf ( "  The matrix order N                 = %d\n", n );
/*
  Loop 1: Evaluate A.
*/
  s = 1.0 / sqrt ( ( double ) ( n ) );

  wtime = omp_get_wtime ( );

#pragma acc data enter create(a[:500][:500], b[:500][:500], c[:500][:500], angle) copy(pi, n, s)
#pragma acc kernels loop collapse(2)
  for ( i = 0; i < n; i++ )
  {
    for ( j = 0; j < n; j++ )
    {
      angle = 2.0 * pi * i * j / ( double ) n;
      a[i][j] = s * ( sin ( angle ) + cos ( angle ) );
    }
  }
/*
  Loop 2: Copy A into B.
*/
#pragma acc parallel loop collapse(2)
  for ( i = 0; i < n; i++ )
  {
    for ( j = 0; j < n; j++ )
    {
      b[i][j] = a[i][j];
    }
  }
/*
  Loop 3: Compute C = A * B.
*/
#pragma acc kernels loop collapse(3)
for ( i = 0; i < n; i++ )
{
  for ( j = 0; j < n; j++ )
  {
    for ( k = 0; k < n; k++ )
    {
      c[i][j] = a[i][k] * b[k][j];
    }
  }
}
#pragma acc data exit delete(a[:500][:500], b[:500][:500], pi, i, n, s, angle)

  wtime = omp_get_wtime ( ) - wtime;
  printf ( "  Elapsed seconds = %g\n", wtime );
  printf ( "  C(100,100)  = %g\n", c[99][99] );
/*
  Terminate.
*/
#pragma acc data exit delete(c[:500][:500])
  printf ( "\n" );
  printf ( "MXM_OPENMP:\n" );
  printf ( "  Normal end of execution.\n" );

  printf ( "\n" );
  timestamp ( );

  return 0;
}
/******************************************************************************/

void timestamp ( void )

/******************************************************************************/
/*
  Purpose:

    TIMESTAMP prints the current YMDHMS date as a time stamp.

  Example:

    31 May 2001 09:45:54 AM

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    24 September 2003

  Author:

    John Burkardt

  Parameters:

    None
*/
{
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct tm *tm;
  time_t now;

  now = time ( NULL );
  tm = localtime ( &now );

  strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

  printf ( "%s\n", time_buffer );

  return;
# undef TIME_SIZE
}
