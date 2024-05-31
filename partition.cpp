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

vector< vector<long double> >data_matrix;
vector< vector<long double> >data_matrix1;
vector<int>rand_subset;

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
    data_matrix.resize(nrow,vector<long double>(ncol));
    data_matrix1.resize(nrow,vector<long double>(ncol));

  //Read the rest of the values from the input binary file
  long double temp;
  for(int i=0;i<=nrow-1;i++)
    {
      cout<<i<<endl;
       for(int j=0;j<=ncol-1;j++)
       {
	 in.read((char*)&temp,sizeof(long double));
	 data_matrix[i][j] = temp;
	 data_matrix1[i][j] = temp;
       }
    }
 
   cout<<"The data matrix is:\n";
   /*for(int i=0;i<=nrow-1;i++)
   {
      for(int j=0;j<=ncol-1;j++)
      {
         cout<<data_matrix[i][j]<<" ";
      }
    
     cout<<endl;
   }*/
 
   in.close();
   cout<<"Rows:"<<nrow<<";Columns:"<<ncol<<endl;
  
  
   
    
    
                                          // ****** Custom Partition *******//
                                          
              int points = nrow;
              int partitions;
              int remaining; int pt;
              cout<<"Total no. of data points: "<<nrow<<endl;
              cout<<"Enter the no. of partitions:";
              cin>>partitions;
                            
              int partition_size = nrow/partitions;
              remaining = points - partitions*partition_size;
              cout<<endl<<"Partition size: "<<partition_size<<endl;
              cout<<endl<<"Remaining points: "<<(points - partitions*partition_size)<<endl;
              
              while(rand_subset.size()!=points)
              {
                cout<<rand_subset.size()<<endl; 	
                 srand (time(NULL));   
                 pt = rand()%points + 0; 
                 if(data_matrix1[pt].size()!=0)
                 {
                    rand_subset.push_back(pt);
                    data_matrix1[pt].erase(data_matrix1[pt].begin(),data_matrix1[pt].end());
                    continue;
                 }
                 
              }                        
    
              for(int i=0;i<=rand_subset.size();i++)
              {
                 cout<<rand_subset[i]<<endl;
              }
              
           
              string dir = "./subsample/";
              string path,fname;
              const int dir_err = mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
              
              ofstream data_write;
              
              
              int i,k;
              int j = 0;
              int part = partition_size;
              for(i=0;i<partitions;i++)
              {
                 stringstream ss;
			ss << i+1;
			fname = ss.str();
                        path = dir+fname; 
                        data_write.open(path.c_str(),ios::out);
                        data_write<<"";
                        data_write.close();
                        data_write.open(path.c_str(),ios::app);       
                        data_write<<partition_size<<" "<<ncol<<endl;
		         
		         for(int l=0;l<partition_size;l++)
		         {
		            
		                for(k=0;k<ncol;k++)
		                 {
		                    data_write<<data_matrix[rand_subset[j]][k]<<" ";
		                 }  
		                  
		                  data_write<<endl;
		                  j++;
		                  cout<<j<<endl;
		         }
		         
		         
		         data_write.close();
              
              }
              
             
                        
              for(j;j<rand_subset.size();j++)
              {              
                 //Fetch the file
                 stringstream ss;
                 ss << i;
                 fname = ss.str();
                 path = dir+fname;
                 fstream data_write;
                 data_write.open(path.c_str(),ios::in);
                 
                 string str, word;
                 char *ptr;
                 vector< vector<long double> >subset;
                 int l=0;
                 
                 
                 
                 
                 //Load the file into the memory
                 while(getline(data_write,str))
                 {
                    l++;
                    subset.resize(l);
                      stringstream ss1(str);
                       while (ss1 >> word)
			{
			   long double temp;
			   temp = strtold(word.c_str(),&ptr);
			    
		            subset[l-1].push_back(temp);

			    if (ss.peek() == ' ')
				ss.ignore();
			}
                 
                 }
                 
                 data_write.close();
                 
                 //Insert new point in-memory
                 l++;
                 subset[0][0]++; 
                 subset.resize(l);
                 for(k=0;k<ncol;k++)
                 {
                    subset[l-1].push_back(data_matrix[rand_subset[j]][k]);
                 }
                 
                 
                                 
                 //Write back to file
                 data_write.open(path.c_str(),ios::out);
                 for(int k1=0;k1<subset.size();k1++)
                 {
                     for(k=0;k<subset[k1].size();k++)
	                 {
	                    data_write<<subset[k1][k]<<" ";
	                 }  
		                  
		         data_write<<endl;
		         
		  }       
		   data_write.close(); 
		   
		   i--;
		         
		                        
              }//end for
              
              
              
  return 0;
  
  
}




















