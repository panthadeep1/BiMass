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
#include <sys/types.h>  // For stat().
#include <sys/stat.h> 
using namespace std;

vector< vector<long double> >data_matrix;
vector< vector<long double> >Data;

int nrow,ncol;
int nrow1,ncol1;
int psi;
int attr;
int h_max;
int e;
int k;

string tree_name,tree_name1;
string path_name,path_name1;
ofstream iso_tree;
ofstream itree;


void iTree(vector<int>X, int e, int h_max, int k, vector<int>D);

int main(int argc, char* argv[])
{

      string input_file = argv[1]; //itree name 
      string input_file_string = input_file;
      string input_file_bin = input_file+".bin";
      string init_data = input_file_bin;
            
      string tree_num_str = argv[2]; //itree number
      int tree_num = atoi(tree_num_str.c_str());
      
      string data_file = argv[3]; //Complete Dataset 
      string data_file_string = data_file;
      string data_file_bin = data_file+".bin";
      string init_data_file = data_file_bin;
      
      string subsample_path = "./subsample/"+input_file_bin; //Directory of the subsample data  
      ifstream in(subsample_path.c_str()); 
	  if(!in)
	  {
	    cout<<"Cannot open the input file: "<<input_file_bin<<endl;
	    exit(0);
	  } 
	  
      
        
      in.read((char*)&nrow,sizeof(int));
      in.read((char*)&ncol,sizeof(int));
      
      psi = nrow; //subsample size of isolation tree
      attr = ncol; //no. of attributes
      h_max = log2(psi)+1; // height of each isolation tree
      e = 0;
      
      cout<<"\nOriginal subsample file:"<<input_file_bin<<"\n";
      cout<<"Rows:"<<nrow;
      cout<<" Columns:"<<ncol;
      cout<<" Size of isolation tree: "<<psi;
      cout<<" No. of attributes: "<<attr;
      cout<<" Height: "<< h_max;
      
      
      data_matrix.resize(nrow);
      for(int i=0;i<=nrow-1;i++)
        data_matrix[i].resize(ncol); 
        
      //Read the rest of the values from the input binary file
	  long double temp;
	  for(int i=0;i<=nrow-1;i++)
	    {
	       for(int j=0;j<=ncol-1;j++)
	       {
		 in.read((char*)&temp,sizeof(long double));
		 data_matrix[i][j] = temp;
	       }
	    } 
	    
	    
      //Display the original subsample data matrix
       
          cout<<"\nThe Original subsample data matrix is:\n";
	  /* for(int i=0;i<=nrow-1;i++)
	   {
	      for(int j=0;j<=ncol-1;j++)
	      {
		 cout<<data_matrix[i][j]<<" ";
	      }
	    
	     cout<<endl;
	   } */ 
	   
	                                         //Read the original data set//
	                                         
	   
	   ifstream data(data_file_bin.c_str());
	   if(!data)
	   {
	      cout<<"Cannot open the dataset file: "<<data_file_bin<<endl;
	      exit(0);
	   } 
	   
	   data.read((char*)&nrow1,sizeof(int));
           data.read((char*)&ncol1,sizeof(int));
           
           cout<<"\nOriginal dataset file:"<<data_file_bin<<"\n";
	   cout<<"Rows:"<<nrow1;
	   cout<<" Columns:"<<ncol1;
	   cout<<" No. of attributes: "<<attr;
	   
	  //Read the rest of the values from the input binary file
	  Data.resize(nrow1);
          for(int i=0;i<=nrow1-1;i++)
            Data[i].resize(ncol1); 
        
        	  
	  for(int i=0;i<=nrow1-1;i++)
	    {
	       for(int j=0;j<=ncol1-1;j++)
	       {
		 data.read((char*)&temp,sizeof(long double));
		 Data[i][j] = temp;
	       }
	    } 
	    
	    
	  cout<<"\nThe Original dataset matrix is:\n";
	  /* for(int i=0;i<=nrow1-1;i++)
	   {
	      for(int j=0;j<=ncol1-1;j++)
	      {
		 cout<<Data[i][j]<<" ";
	      }
	    
	     cout<<endl;
	   } */ 
	   
	   
	   
	                                         
                                                 //Construct the isolation tree//
                                                 
         
        //Output isolation split attribute-split point tree
        path_name = "./iTrees_pq/"; 
        const int dir_err = mkdir(path_name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);                                                       
        tree_name = path_name + "itree_pq" + tree_num_str;                                       
        iso_tree.open(tree_name.c_str(),ios::out);
        iso_tree<<"";
        iso_tree.close();   
        
        //Output isolation tree
        path_name1 = "./iTrees/"; 
        const int dir_err1 = mkdir(path_name1.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);       
        tree_name1 = path_name1 + "itree" + tree_num_str;                                       
        itree.open(tree_name1.c_str(),ios::out);
        itree<<"";
        itree.close();
        
        //Write root node to the file
        itree.open(tree_name1.c_str(),ios::app);
        itree<<e<<" "<<"1"<<" "; //level,node no.,node indices.......
        for(int i=0;i<Data.size();i++)
	      {
		  itree<<i<<" ";
	      }
   
        itree<<"-1";   
        itree<<endl;
        itree.close(); 
        
        vector<int>D; // create index of the whole dataset
        for(int i=0;i<Data.size();i++)
         D.push_back(i);                                            
                                                                                                  
        //Create root X as per iTree() algorithm
        vector<int>X;
        for(int i=0;i<psi;i++)
         X.push_back(i);
       
       
        //Function for itree creation p,q w.r.t iTree() algorithm
        
        k = 1;               
        iTree(X,e,h_max,k,D); //k,D are additional parameters added to create the iTree at one go.  
         
	   
	   
	  
      return 0;
      
}

void iTree(vector<int>X, int e, int h, int k, vector<int>D)
{
   if(e>=h || X.size()<=1)
   {
      ; //do nothing
   } 
   else      
   {
      //Randomly select a split attribute 'q'
      int q;
      srand (time(NULL));   
      q = rand()%(attr-0); 
      cout<<endl<<"Tree level: "<<e;
      cout<<endl<<"Node no.: "<<k;
      cout<<endl<<"Split attribute: "<<q;
      
      //Randomly select a split point 'p' between q_min and q_max
      vector<long double>Q;
      long double p, q_min, q_max; 
      for(int i=0;i<X.size();i++)
      {
         Q.push_back(data_matrix[X[i]][q]);
      }
      
      sort(Q.begin(),Q.end());
      q_min = Q[0];
      q_max = Q[Q.size()-1];
      
      cout<<endl<<"q_min: "<<q_min;
      cout<<endl<<"q_max: "<<q_max;
      
      srand(time(NULL)); //random no. for float point nos.
      p = q_min + static_cast <long double> (rand()) /( static_cast <long double> (RAND_MAX/(q_max-q_min)));
      
      cout<<endl<<"Split point: "<<p;
      
      //Creating left,right filters X_l,X_r wrt iTree() algorithm
      vector<int>X_l;
      vector<int>X_r;
      for(int i=0;i<X.size();i++)
      { 
        if(data_matrix[X[i]][q] < p)
         X_l.push_back(X[i]);
        if(data_matrix[X[i]][q] >= p)
         X_r.push_back(X[i]); 
      }
      
      cout<<endl<<"X_l size: "<<X_l.size();
      cout<<endl<<"X_r size: "<<X_r.size();
      cout<<endl<<"---------"<<endl;
      
      iso_tree.open(tree_name.c_str(),ios::app);
      iso_tree<<e<<" "<<k<<" "<<q<<" "<<p<<endl;
      iso_tree.close();
      
      //Create the inner nodes of the iTree
      vector<int>D_l;
      vector<int>D_r;
      for(int i=0;i<D.size();i++)
      { 
        if(Data[D[i]][q] < p)
         D_l.push_back(D[i]);
        if(Data[D[i]][q] >= p)
         D_r.push_back(D[i]); 
      }
      
       
       itree.open(tree_name1.c_str(),ios::app);
       itree<<e+1<<" "<<2*k<<" "; //level,node no.,node indices.......
       
        for(int i=0;i<D_l.size();i++)
        {
           itree<<D_l[i]<<" ";
        }     

        itree<<"-1";
        itree<<endl;
        
       itree<<e+1<<" "<<2*k+1<<" "; //level,node no.,node indices.......
       for(int i=0;i<D_r.size();i++)
        {
           itree<<D_r[i]<<" ";
        }     

        itree<<"-1";
        itree<<endl;
       
      itree.close();
      
      //Recursively create the isolation tree
      iTree(X_l,e+1,h,2*k,D_l);
      iTree(X_r,e+1,h,2*k+1,D_r);
      
      
   }
   
}















                                                 
            
                    
             
                
                                                 
                                                 
                                                 
                          
























