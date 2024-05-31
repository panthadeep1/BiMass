#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <map>
#include <cmath>
#include <cstring>
#include <cassert>
#include <string>
#include <queue>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <ios>
#include <sstream>
#include <queue>
#include <sys/time.h>
#include <malloc.h>
#include <sys/stat.h>
using namespace std;

void convert_binary(string,string,string,string);
int nrow,ncol,val;
string prt,dir,path,fname;

int main(int argc, char* argv[])
{
  
  string input_file = argv[1];
  string input_file_bin = input_file+".bin";
  string input_file_string = argv[1];

  ifstream in(input_file_bin.c_str(),ios::in|ios::binary);
  if(!in)
  {cout<<input_file_bin<<" dataset file is not found check again!!"<<endl; exit(0);}
  cout<<"Processed Input file:"<<input_file_bin<<endl;

  //Read the nrow,ncol values from input binary file

    in.read((char*)&nrow,sizeof(int));
    in.read((char*)&ncol,sizeof(int));

    cout<<"Rows:"<<nrow<<";Columns:"<<ncol<<endl;
    
    int urow,base,updates;
    cout<<"Enter the no. of updates:";
    cin>>urow;
    long double data_matrix[urow][ncol];
    base = nrow-urow;
    updates = urow;
    
    cout<<"Base dataset size:"<<base<<endl;
    cout<<"Updates size:"<<updates<<endl;

  //Read the rest of the values from the input binary file
  long double temp;
  for(int i=0;i<=nrow-1;i++)
    {
      for(int j=0;j<=ncol-1;j++)
       {
	 in.read((char*)&temp,sizeof(long double));
	 if(i>=base)
	 data_matrix[i-base][j] = temp;
       }
    }
 
   cout<<"The update data matrix is:\n";
   for(int i=0;i<=urow-1;i++)
   {
      for(int j=0;j<=ncol-1;j++)
      {
         cout<<data_matrix[i][j]<<" ";
      }
    
     cout<<endl;
   }
 
   in.close();
   
   
   
   int part;   
   for(part=1;part<=updates;part++)
   {
	   if(urow%part==0)//equal partitions
	   {
             cout<<part<<endl;
	     int p_size=(urow/part);
              stringstream ss;
              ss << part;
              prt = ss.str();
              dir = "./file_bat"+prt+"/";
              const int dir_err = mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


		     for(int i=0;i<part;i++)
		     {
		       int lower=i*p_size;
		       int upper=(i+1)*p_size-1;
		       //cout<<lower<<" "<<upper<<endl;

			stringstream ss;
			ss << i+1;
			fname = ss.str();
                        path = dir+fname; 
                          ofstream data_write(path.c_str());
                          data_write<<p_size<<" "<<ncol;

                       for(int k=lower;k<=upper;k++)
                       {
                         int j;
                         data_write<<endl;
                         for(j=0;j<ncol-1;j++)
                         {
                            data_write<<data_matrix[k][j]<<" ";
                         }
 
                         data_write<<data_matrix[k][j];                                                     

                       } 

                         data_write.close();    
                      		 
		     }//end for 

                               

	   }//end if

    }//end for 
    
    

  return 0;
}















