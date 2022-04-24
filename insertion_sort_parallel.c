// Parallel sort-simple.c
// By Hrithik Rai Saxena
#include <time.h> // for CPU time
#include <sys/time.h> //for gettimeofday
#include <stdio.h>
#include <omp.h>
#define max_len 100000
#define LENGTH 40

double find_min(double *arr,double size){
    double a = arr[0];
    for(int i = 1; i<size; i++){
        if(arr[i]<a){
            a = arr[i];
        }
    }
    return a;
} 

double find_max(double *arr,double size){
    double b = arr[0];
    for(int i = 1; i<size; i++){
        if(arr[i]>b){
            b = arr[i]; 
        }
    }
    return b;
} 

int find_max_loc(double *arr,double size){
    double b = arr[0];
    int max_loc;
    for(int i = 1; i<size; i++){
        if(arr[i]>b){
             max_loc =i;
        }
    }
    return max_loc;
} 

main(){
int i=1,len,j,k,N,me,num_threads,max_loc;
double b[max_len+1],c[max_len+1],bnew,cnew,time,c_pr[max_len+1],b_pr[max_len+1],low, high,range;
char name[LENGTH]="100k.txt",line[LENGTH];
FILE *fp;
clock_t cpu0,cpu1,cpu2,cpu3; // clock_t defined in <time.h> and <sys/types.h> as int
struct timeval time0, time1,time2,time3; // for wall clock in s and us
double  dtime12,dtime03; // for wall clock in s (real number)

cpu0 = clock();    // assign initial CPU time (IN CPU CLOCKS)
gettimeofday(&time0, NULL); // returns structure with time in s and us (microseconds)
//printf("Input filename\n"); scanf("%s",name);
fp=fopen(name,"r");
while(1){ //1 serves as true, i.e. condition which is always true
  if(fgets(line, LENGTH,fp)==NULL)break; // finish reading when empty line is read
  if(sscanf(line, "%lf %lf",&b[i],&c[i])==-1) break; // finish reading after error
  i++;
}
len=i-1;
printf("Number of items to sort: %i\n",len);
fclose(fp);

    double max = find_max(b,len);
    double min = find_min(b,len);
    max_loc = find_max_loc(b,len);
    
    printf("Maximum element of the array is %lf\n", max);
    printf("Minimum element of the array is %lf\n", min);

cpu1 = clock();    // assign initial CPU time (IN CPU CLOCKS)
gettimeofday(&time1, NULL); // returns structure with time in s and us (microseconds)

// Parallel Region Begins //
#pragma omp parallel num_threads(12) private(i,b_pr,c_pr,range,low,high,N)

{ 

int me = omp_get_thread_num();  
int num_threads = omp_get_num_threads();
printf("Number of threads = %d\n", num_threads);

range = (max - min) / num_threads; //Making ranges
N=0;
low =  ( me* range) + min ;
high=  ( (me + 1)* range) + min;

// Here we are distributing the elements from the shared array to the private ones according to their respective ranges.
for (i=1;i<=len;i++) {
  if((b[i]>=low) & (b[i]<high)) {
    b_pr[N]=b[i]; 
    c_pr[N]=c[i]; 
    N++;
    } 
}

// For files other than 100k.txt please add the below code
// The search condition will not be reading the maximum value for the last thread therefore we assign it.

//if (me == (num_threads-1) ) {
//  b_pr[N]=b[max_loc];
//  c_pr[N]=c[max_loc];
//  N++;
//  } 

// Start sorting 
for(j=1;j<N;j++){ // start sorting with the second item
  bnew=b_pr[j];cnew=c_pr[j];
  for(i=0;i<j;i++){
    // if(bnew==b[i]) printf("Equal numbers %lf\n",bnew);
    if((bnew<b_pr[i]) | ((bnew==b_pr[i])&(cnew<c_pr[i])))
	{
	  for(k=j;k>i;k--) { b_pr[k]=b_pr[k-1]; c_pr[k]=c_pr[k-1]; } //shift larger numbers
	  b_pr[i]=bnew; c_pr[i]=cnew; //insert number j on ith position
	  break; //no need to check larger items which are already sorted !
	} // else do nothing - j is the largest number
  }   // continue checking other numbers if larger or not
}

cpu2 = clock();    // assign CPU time (IN CPU CLOCKS)
gettimeofday(&time2, NULL);

// Calculating the sorting time
dtime12 = ((time2.tv_sec  - time1.tv_sec)+(time2.tv_usec - time1.tv_usec)/1e6);

//Sequential Write
#pragma omp for ordered
for (int m=0 ; m < num_threads;m++) { 
#pragma omp ordered 
{ 
if(me==0) fp=fopen("finally_sorted.txt","w"); 
else {fp=fopen("finally_sorted.txt","a");}

   for (i=0;i<N;i++) {
          fprintf(fp,"%lf %lf\n",b_pr[i],c_pr[i]);
    }
          
fclose(fp);
}
}
} 

cpu3 = clock();    // assign initial CPU time (IN CPU CLOCKS)
gettimeofday(&time3, NULL); // returns structure with time in s and us (microseconds)

// Total time for reading and calculating
dtime03 = ((time3.tv_sec  - time0.tv_sec)+(time3.tv_usec - time0.tv_usec)/1e6);
printf("Elapsed wall time sorting---------CPU time\n");
printf("Time Taken -> %f               %f\n", dtime12,(float) (cpu2-cpu1)/CLOCKS_PER_SEC);
printf("Elapsed wall time complete--------CPU time\n");
printf("Time Taken -> %f                %f\n", dtime03,(float) (cpu3-cpu0)/CLOCKS_PER_SEC);

}

