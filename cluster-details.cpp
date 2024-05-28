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
using namespace std;

bool myfunction (int i, int j) {
  return (i==j);
}

typedef struct Clust{

     public: 
      int status;
      int rep;
      int core;
}Cluster;

vector< vector<long double> >data_matrix;
vector<int>cluster_num;
vector< pair<int,int> >cluster_unique;

vector< vector<pair<int,int> > >cluster_matrix;
vector< vector<pair<int,int> > >class_matrix;
vector< pair<int,long double> >NMI_class;
vector<long double>NMI_cluster;

int main(int argc, char* argv[])
{
		  
		  
		                                       /***** Load data file *****/
		  
		  
		  string input_file = argv[1];
		  string input_file_bin = input_file+".bin";
		  string input_file_string = argv[1];  
		  ifstream in(input_file_bin.c_str(),ios::in|ios::binary);
		  if(!in)
		  {cout<<input_file_bin<<"  Data matrix file not found check again!!"<<endl; exit(0);}
		  cout<<"Processed Input file:"<<input_file_bin<<endl;
		  
		  
		      int nrow,ncol; //Fetch the row,col
		      long double temp;
		      in.read((char*)&nrow,sizeof(nrow));
		      in.read((char*)&ncol,sizeof(ncol));
		      cout<<"#Rows:"<<nrow<<" #Columns:"<<ncol<<endl;
		      
		       data_matrix.resize(nrow);            
		       for(int i=0;i<nrow;i++)
		       {
			   for(int j=0;j<ncol;j++)
			   {
				in.read((char*)&temp,sizeof(temp));    
				data_matrix[i].push_back(temp);       
			   }
		       
		       }
		      
		       in.close();

                  
                                               
                                                     //****** Load Clusters *******//    
                                                     
                                                   
                  input_file = argv[2];
		  input_file_bin = input_file+".bin";
		  input_file_string = argv[2];  
		  in.open(input_file_bin.c_str(),ios::in|ios::binary);
		  if(!in)
		  {cout<<input_file_bin<<"Cluster file not found check again!!"<<endl; exit(0);}
		  cout<<"Processed Cluster file:"<<input_file_bin<<endl;
  
  
  //cluster_num.resize(nrow);
  Cluster cluster_temp;
  vector<Cluster>tempCluster;
  
  int core=0,non_core=0,noise=0;
   for(int i=0;i<=nrow-1;i++)
          { 
                   in.read((char*)&cluster_temp,sizeof(cluster_temp));
                   tempCluster.push_back(cluster_temp);
                   cluster_num.push_back(cluster_temp.rep);
                   
                   if(cluster_num[i] == -1) // is a noise pt.
		   {
		   	   noise++;
		   }	
                   
                   else //not a noise pt.
                   {
                   	   if(cluster_temp.core == 1)
                   	   {
                   	   	    core++;
					   
			   }
			   if(cluster_temp.core == 0)
                   	   {
                   	   	    non_core++;
			   }
					   
		    }
				   			   
				   
                                      
           }//end for
           
      in.close();
  
  cout<<"Dataset size:"<<cluster_num.size()<<endl;
  sort(cluster_num.begin(),cluster_num.end());

  for(int i=0;i<cluster_num.size();i++)
  {
      cout<<cluster_num[i]<<endl;     
  }


  vector<int>::iterator it;
  int a=0;
  int count=1;
  pair<int,int>cid_count;
  cid_count.first = cluster_num[a];
  cid_count.second = count;
  cluster_unique.push_back(cid_count);  
  for(int i=1;i<cluster_num.size();i++)
  {
      if(cluster_num[i] == cluster_unique[a].first){
	       count++;
           continue;
       }
      else{
       cluster_unique[a].second = count;
       cid_count.first = cluster_num[i];
       count = 1;
       cid_count.second = count;
       cluster_unique.push_back(cid_count);
       a++;
      }     
  }
  
  if(cluster_unique[0].first != -1)//No noise pts. insert a box at beginning
   {
      cluster_unique.insert(cluster_unique.begin(),pair<int,int>(-1,-1));
   }
  
   
                                               //********* Creating cluster matrix *********//
                                                                                                                                       
                         int clasLabel,clustNum,clustPoints;
                         clustNum = cluster_unique.size();//No. of clusters+outliers
                                                  
                         clustPoints = core+non_core;//No. of clustered pts.
                         
                         input_file = argv[3];//No. of classes
                         clasLabel = atoi(input_file.c_str());
                                          
                         cluster_matrix.resize(clustNum); //Creating cluster matrix  
                         for(int i=0;i<clustNum;i++)  
                         {
                             
                                int rep = cluster_unique[i].first;                             
		                     for(int j=0;j<tempCluster.size();j++) //All pts. cluster status
		                     {
		                         if(tempCluster[j].rep == rep)
		                           {
		                               pair<int,int>temp;
		                               int clas;
		                               temp.first = j; //pt.index
		                               if(clasLabel != 0){
		                               clas = data_matrix[j].size()-1;
		                               temp.second = data_matrix[j][clas];
		                               }
		                               
		                               cluster_matrix[i].push_back(temp);//Push pt. index, pt. class
		                               
		                           }
		                     }//end for
                             
                         } //end for 
                         
                         
                         
			  cout<<"No. of clusters:"<<cluster_matrix.size()-1<<endl;			  
			  cout<<"No. of core points:"<<core<<endl;
			  cout<<"No. of non-core points:"<<non_core<<endl;
			  cout<<"No. of clustered points:"<<core+non_core<<endl;
			  cout<<"No. of noise points:"<<noise<<endl;
                         
                         cout<<"\nCluster matrix is:"<<endl;
                         //Display Cluster matrix      
                         for(int i=0;i<cluster_matrix.size();i++)
                         {
                               cout<<"Cluster"<<i<<"]";
	                       cout<<" Lead point:"<<cluster_unique[i].first;
	                       cout<<" Cluster size:"<<cluster_matrix[i].size()<<endl;
	                          for(int j=0;j<cluster_matrix[i].size();j++)
	                          {
	                                cout<<cluster_matrix[i][j].first<<"/";
	                                cout<<cluster_matrix[i][j].second<<" ";
	                          }
	                          
	                       cout<<"\n--------------------\n";  
                                                      
                         }
                                        
                                        
                                        
                                                  //******** Create Class matrix *********//
                                        
                                        
                            if(clasLabel == 0)
                             {
                                cout<<"\nUnclassified data";
                                exit(0);
                             }
                                        
                             class_matrix.resize(clasLabel+1);
                             for(int i=0;i<data_matrix.size();i++)
                             {
                                  int clas;
                                  int flag=0;
                                  clas = data_matrix[i][data_matrix[i].size()-1];                                  
                                  pair<int,int>temp;
                                  temp.first = i;
		                          for(int j=0;j<cluster_matrix.size();j++)//Search for pt. 'i' in all the 'j' clusters
		                          {
		                            flag=0;
		                               for(int k=0;k<cluster_matrix[j].size();k++)
		                               {
		                                   if(cluster_matrix[j][k].first == i)//Check the presence of 'i' in cluster 'j'
		                                   {
		                                       //pt.found
		                                       temp.second = j; //Assign the cluster no.
		                                       flag=1;
		                                       break;
		                                   } 
		                                   
		                               }
		                               
		                               if(flag == 1)
		                                 break;
		                               
		                               
		                          }//end for
                                   
                                  class_matrix[clas].push_back(temp);
                                    
                             }//end for       
                             
                             cout<<"\nClass matrix is:"<<endl;
		                 //Display Class matrix      
		                 for(int i=0;i<class_matrix.size();i++)
		                 {
		                       cout<<"Class"<<i<<"]";			               
			               cout<<" Class size:"<<class_matrix[i].size()<<endl;
			                  for(int j=0;j<class_matrix[i].size();j++)
			                  {
			                        cout<<class_matrix[i][j].first<<"/";
			                        cout<<class_matrix[i][j].second<<" ";
			                  }
			                  
			               cout<<"\n--------------------\n";  
		                                              
		                 }
                             
                             
                             
                             
                                                                            //***** Normalized Mutual Information *****//
                                                  
                                            
                                 long double H_Y,H_C,H_YC,I_YC,NMI_YC;           
                                 NMI_class.resize(clasLabel+1);
                                  
                                 //P(Y) calculation of NMI
                                 for(int i=0;i<NMI_class.size();i++)
                                 {
                                    NMI_class[i].first = 0;
                                    NMI_class[i].second = 0;
                                 }           
                                 for(int i=1;i<cluster_matrix.size();i++)
                                 {                                 
                                     for(int j=0;j<cluster_matrix[i].size();j++)
                                     {
                                        int clas = cluster_matrix[i][j].second;
                                        NMI_class[clas].first++;
                                     }
                                 }
                                 
                                 for(int i=1;i<NMI_class.size();i++)//P(Y) values
                                    NMI_class[i].second = (long double)NMI_class[i].first/clustPoints;
                                 
                                 //Display Clustered classed pts.
                                 cout<<"\nP(Y) values:"<<endl;
                                 for(int i=1;i<NMI_class.size();i++)
                                 {
                                      cout<<"Class"<<i<<":";
                                      cout<<NMI_class[i].first<<"/";
                                      cout<<NMI_class[i].second<<" ";                                 
                                 } 
                                 
                                 
                                 //H(Y) calculation 
                                 H_Y=0.0;
                                 for(int i=1;i<NMI_class.size();i++)
                                 {
                                     long double P_Y;
                                     P_Y = NMI_class[i].second;
                                     if(P_Y == 0)//unclustered pts. of class 
                                       continue;
                                       
                                     H_Y += P_Y*log2 (P_Y);
                                 }   
                                 
                                 H_Y = H_Y*(-1);//Entropy class
                                 cout<<"\nH(Y) is:"<<H_Y<<endl;     
                                 
                                 
                                 //P(C) calculation
                                 NMI_cluster.resize(cluster_matrix.size());
                                 for(int i=0;i<NMI_cluster.size();i++)
                                 {
                                    NMI_cluster[i]=0.0;
                                 }                                 
                                 for(int i=1;i<NMI_cluster.size();i++)
                                 {
                                     int size = cluster_matrix[i].size(); 
                                     NMI_cluster[i]=(long double)size/clustPoints;                                 
                                 }
                                 
                                 //H(C) calculation 
                                 H_C=0.0;
                                 for(int i=1;i<NMI_cluster.size();i++)
                                 {
                                     long double P_C;
                                     P_C = NMI_cluster[i];
                                     if(P_C == 0)//unclustered pts. of class 
                                       continue;
                                       
                                     H_C += P_C*log2 (P_C);
                                 }   
                                 
                                  H_C = H_C*(-1);//Entropy cluster
                                  cout<<"\nH(C) is:"<<H_C<<endl;     
                                 
                                 long double H_YCi;
                                 H_YC=0.0;
                                 for(int i=1;i<cluster_matrix.size();i++)
                                 {
                                      //H_YC calculation  
                                      for(int l=0;l<NMI_class.size();l++)//Clear NMI class
                                      {
                                          NMI_class[l].first = 0;
                                          NMI_class[l].second = 0;
                                      }  
                                 
                                      for(int j=0;j<cluster_matrix[i].size();j++)
                                      {
		                         int clas = cluster_matrix[i][j].second;
                                         NMI_class[clas].first++;
                                         NMI_class[clas].second = (long double)NMI_class[clas].first/cluster_matrix[i].size();
                                      }  
                                      
                                                                           
                                      H_YCi=0.0;
                                      for(int k=1;k<NMI_class.size();k++)//H(YC)calculation 
                                      {                          
                                        long double P_C;
                                        P_C = NMI_class[k].second;
                                        if(P_C == 0)//unclustered pts. of class 
                                          continue;
                                          
                                        H_YCi += P_C*log2 (P_C);	  
                                      }
                                     
                                     H_YCi = H_YCi*(-NMI_cluster[i]);
                                     cout<<H_YCi<<" ";
                                     H_YC += H_YCi;
                                      
                                 }//end for 
                                 
                                 
                                 cout<<"\nH(Y|C) is:"<<H_YC<<endl;  
                                 
                                 //I(Y|C),NMI_YC compute
                                 I_YC = H_Y-H_YC; 
                                 NMI_YC = (2*I_YC)/(H_Y+H_C); 
                                 
                                 cout<<"\nNMI(Y|C):"<<NMI_YC<<endl;
                                 
                                 
                                 
                                 
                                                //****** Concurrency Matrix calculation *******//
                                                
                               
                              int n = class_matrix.size();
                              int m = cluster_matrix.size();  
                              cout<<"Class matrix size: "<<n<<endl;
                              cout<<"Cluster matrix size: "<<m<<endl;
                              
                               
                              vector< vector<int> >concurrency_matrix;
                              concurrency_matrix.resize(n);
                              
                              //Inititalize concurrency-matrix              
                              for(int i=0;i<concurrency_matrix.size();i++)
                              {
                              
                                     for(int j=0;j<m;j++)
                                     {
                                          concurrency_matrix[i].push_back(0);
                                     }
                              
                              }
                              
                              
                              /*for(int i=0;i<concurrency_matrix.size();i++)
                              {
                                   for(int j=0;j<concurrency_matrix[i].size();j++)
                                   {
                                        cout<<concurrency_matrix[i][j];
                                   }
                                   
                                   cout<<endl;                        
                              }*/
                               
	                         
	                        
	                      for(int i = 0; i < cluster_matrix.size(); i++ ) //starting from 'Cluster 1' as 'Cluster 0' consists noise pts.
	                      {
	                          if(i==0) //Cluster '0' contains all noise pts.
	                            continue;
	                             
	                            for(int j = 0; j < cluster_matrix[i].size(); j++)
	                            {
	                                int clas = cluster_matrix[i][j].second; // inserting the class value of that pt. within cluster 'i'
	                                if(clas == 0) //class 0 invalid
	                                  continue;
	                                
	                                concurrency_matrix[clas][i] += 1; //increasing the class count by 1 within cluster 'i'	                            
	                            }
	                      
	                      }   
	                      
	                      
	                      //Display concurrency matrix
	                      cout<<endl<<"The Concurrency-Matrix is:\n";
	                      for(int i = 1; i < concurrency_matrix.size(); i++ )
	                      {
	                          
	                            for(int j = 1; j < concurrency_matrix[i].size(); j++)
	                            {
	                                cout<<concurrency_matrix[i][j]<<" ";                            
	                            }
	                            
	                          cout<<endl;                        
	                      }  
	                      
	                      
	                      //Write Concurrency-Matrix to a file
	                      ofstream rand("Concurrency-Matrix",ios::out);//Clear Concurrency-Matrix file
	                         rand<<"";
	                         rand.close();  
	                         
	                           
	                      rand.open("Concurrency-Matrix",ios::app);   
	                      for(int i = 1; i < concurrency_matrix.size(); i++ )
	                      {
	                          
	                            for(int j = 1; j < concurrency_matrix[i].size(); j++)
	                            {
	                                rand<<concurrency_matrix[i][j]<<", ";                            
	                            }
	                            
	                          rand<<endl;                        
	                      }  
	                         
	                         
	                     rand.close(); 
                              
                          
  
       return 0;

}











