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
using namespace std;

vector< vector<long double> >Data;
vector< vector<long int> >root_matrix;
vector< vector<int> >iso_tree_matrix;
vector< vector<long double> >mass_matrix;
vector< vector< pair<int,long double> > >range_matrix;
vector<int>tree_index;

vector<int>core;
vector<int>non_core;
vector<int>noise;

void disp_param(int, int, int, int, long double);
void disp_data_matrix();

void load_iTree(int, string);
void disp_iForest();

double calMass_xy(int,int, int);
void dispMass_xy(int);
void dispLowestNode_xy(int);

void link_cluster(int,int);
void disp_cluster();

void writeVal();

int flag = 0;
string itree_path;
string tree_name;

typedef struct Clust{

     public:
      int status;
      int rep;
      int core;
}Cluster;

vector<Cluster>cluster;

//Storing the tree-id for tracking the lowest node/region while mass calculation
      
      typedef struct iTreeNode{
          public:
          int node_id[20];  //no. of iTrees
      }TreeNode;

vector< vector<TreeNode> >Node_id;
TreeNode lowest_node;

int main(int argc, char* argv[])
{
           
      //number of trees
      string tree_num_str = argv[1];        
      int tree_num = atoi(tree_num_str.c_str());
      
      //subsample size
      string tree_size_str = argv[2];
      int tree_size = atoi(tree_size_str.c_str());
      
      //total number of data points
      string points_num_str = argv[3];
      int points_num = atoi(points_num_str.c_str());
      
      //core point threshold
      string minpts_str = argv[4];
      int minpts = atoi(minpts_str.c_str());
      
      //mu threshold 
      char *ptr;
      string mu_str = argv[5];
      long double mu = strtold(mu_str.c_str(),&ptr);
      
      //Complete Dataset 
      string data_file = argv[6]; 
      string data_file_string = data_file;
      string data_file_bin = data_file+".bin";
      string init_data_file = data_file_bin;
      
          
      
      
      
                                                   // **** Load the original dataset **** //
                                                   
              double t1_loadData=(double)clock()/CLOCKS_PER_SEC;                                                                
              int nrow,ncol;                                     
              ifstream data(data_file_bin.c_str());
	      if(!data)
	      {
		  cout<<"Cannot open the dataset file: "<<data_file_bin<<endl;
		  exit(0);
	      } 
	      
	        data.read((char*)&nrow,sizeof(int));
                data.read((char*)&ncol,sizeof(int));
                
                cout<<"\nOriginal dataset file:"<<data_file_bin<<"\n";
		   cout<<"Rows:"<<nrow;
		   cout<<" Columns:"<<ncol;
		   
                Data.resize(nrow,vector<long double>(ncol));
                long double temp;
                for(int i=0;i<=nrow-1;i++)
		    {
		       for(int j=0;j<=ncol-1;j++)
		       {
			 data.read((char*)&temp,sizeof(long double));
			 Data[i][j] = temp;
		       }
		       
		    }		
	        
	        double t2_loadData=(double)clock()/CLOCKS_PER_SEC;	
	        double t_loadData = t2_loadData-t1_loadData;
		cout<<"\nThe Original dataset matrix is:"<<(t2_loadData-t1_loadData)<<endl;
		  /* for(int i=0;i<=nrow1-1;i++)
		   {
		      for(int j=0;j<=ncol1-1;j++)
		      {
			 cout<<Data[i][j]<<" ";
		      }
		    
		     cout<<endl;
		   } */  
		   
		   
		//Write original data_matrix to file     
                disp_data_matrix();
                
      
     
      
                                        //****** Load isolation Forest in the memory for fast access *******//
                                   
                                     
              double t1_iForest=(double)clock()/CLOCKS_PER_SEC;                                 
              tree_index.resize(tree_num+1);                       
              itree_path = "./iTrees/"; //Directory of the subsample data   
              int t;                    
              for(t=1;t<=tree_num;t++){
		             stringstream ss;
		             ss << t;
		             tree_name = itree_path+"itree"+ss.str();
		             iso_tree_matrix.resize(iso_tree_matrix.size()+1);
		             
		             iso_tree_matrix[iso_tree_matrix.size()-1].resize(1);
		             iso_tree_matrix[iso_tree_matrix.size()-1][0] = -t;
		             tree_index[t] = iso_tree_matrix.size()-1;
		             //cout<<"iTree:"<<t<<" "<<iso_tree_matrix.size()<<" "<<tree_index[t]<<endl;
		             
		             load_iTree(t,tree_name);
		             
		             //cout<<"iTree:"<<t<<" "<<iso_tree_matrix.size()<<endl;
		          }                           
		          
             cout<<"iForest size: "<<iso_tree_matrix.size()<<endl;      
             double t2_iForest=(double)clock()/CLOCKS_PER_SEC;   
             double t_iForest = t2_iForest - t1_iForest;
                                               
             cout<<endl<<"iForest loaded...."<<(t2_iForest-t1_iForest)<<endl;
             
             //Write iForest to file
             disp_iForest();
                                                                
                                    
       
     
                                                //***** Mass-matrix creation M_e(x,y) ******//
                                           
                                            
             double t1_massMatrix=(double)clock()/CLOCKS_PER_SEC; 
             long double R = 0;                             
             long double mass_xy;  
             int t_index;                      
             mass_matrix.resize(points_num,vector<long double>(points_num));
             Node_id.resize(points_num,vector<TreeNode>(points_num));  
                                          
             for(int i=0;i<mass_matrix.size();i++)
             {  
               //cout<<i<<endl;                            
                 for(int j=i;j<mass_matrix[i].size();j++)
                 {
                     if(i==j)      //mass based dissimilarity between same element me_(x,x).
                       {   
                          //cout<<i<<j<<endl;                       
		          		         
		          R = calMass_xy(i,j,tree_num); //calculate the mass dissimilarity me_(x,y)	            
		             
                          mass_xy = (long double)R/(points_num*tree_num); 
                          mass_matrix[i][j] = mass_xy;
                          continue;
                       }
                       
                     if(i!=j)      //mass based dissimilarity between different elements me_(x,y).
                       {
                            //cout<<i<<j<<endl;                             
	                 	       			         
			      R = calMass_xy(i,j,tree_num); //calculate the mass dissimilarity me_(x,y)			           
			             
			      mass_xy = (long double)R/(points_num*tree_num);
			      mass_matrix[i][j] = mass_xy;
	                      mass_matrix[j][i] = mass_xy;                
			                          	                  	                 
		                  
                       }//end if
                       
                       
                 }//end for
                 
                  
                 
             }//end for       
             double t2_massMatrix=(double)clock()/CLOCKS_PER_SEC;
             double t_massMatrix = t2_massMatrix-t1_massMatrix;
             cout<<endl<<"Mass matrix created...."<<(t2_massMatrix-t1_massMatrix)<<endl;
             
            //Write Mass-Matrix and Lowest-nodes to file 
            dispMass_xy(tree_num);
            dispLowestNode_xy(tree_num);
            
            
            
                                                  // **** Parameters of Mass based clustering ****//
                                                        
                                                       
               //Selecting the mu-threshold
               double t1_massParam =(double)clock()/CLOCKS_PER_SEC;
               long double mu_th = 0;
               for(int i=0;i<mass_matrix.size();i++)
               {
                   for(int j=0;j<mass_matrix[i].size();j++)
                   {
                       //mu_th += mass_matrix[i][j];
                       if(i==j)
                       {
                          if(mass_matrix[i][j]>mu_th)
                           mu_th = mass_matrix[i][j]; 
                       }
                       
                   } 
               } 
               //mu_th /= pow(mass_matrix.size(),2);
               mu = mu_th+0.1; 
               
               
               cout<<endl<<"Mean mu/Threshold mu:"<<mu_th<<"/"<<mu<<endl;
               
               
               //Core and non-core points + initial cluster creation
               
               Cluster clus;
               int mass_th = 0;
               pair<int,long double>a;
               range_matrix.resize(points_num);               
               for(int i=0;i<mass_matrix.size();i++)
               {
                  mass_th = 0;
                    for(int j=0;j<mass_matrix[i].size();j++)
                    {
                        if(mass_matrix[i][j] <= mu){
                         a.first = j;
                         a.second = mass_matrix[i][j];
                         range_matrix[i].push_back(a);
                         mass_th++;
                        }
                    }
                    
                    if(mass_th > minpts){
                     core.push_back(i);
                     clus.status=1;
                     clus.rep=i;
                     clus.core=1;
                    } 
                    else{
                     non_core.push_back(i);
                     clus.status=0;
                     clus.rep=-1;
                     clus.core=0;
                    }          
                    
                    cluster.push_back(clus); 
                     
               } 
               
               cout<<endl<<"No. of core points: "<<core.size();
               cout<<endl<<"No. of non-core points: "<<non_core.size(); 
               
               cout<<endl<<"Initial Clusters:\n";
	       /*for(int i=0;i<cluster.size();i++)
	       {
		     cout<<i<<"] ";
		     cout<<cluster[i].status<<"/";
		     cout<<cluster[i].rep<<"/";
		     cout<<cluster[i].core<<endl;

	       }*/

               
              //******Case When No Clusters are formed******//
	       if(core.size()==0)
		{
		  cout<<"\nNo additional clusters possible\n";
		  cout<<"\nNoise points:\n";
		            
		       
		       for(int i=0;i<points_num;i++)
		       {
			  if(cluster[i].core==0 && cluster[i].rep==-1)
			   {
			     //cout<<i<<" ";
			     noise.push_back(i);    
			   }
		       } 
		               
                 }
                                   
                double t2_massParam =(double)clock()/CLOCKS_PER_SEC;
                double t_massParam = (t2_massParam - t1_massParam);
                cout<<endl<<"Parameters created:"<<(t2_massParam-t1_massParam)<<endl;
                
               //Write parameters to file
               disp_param(tree_num,tree_size,points_num,minpts,mu);
                                                    
                                    
                                                    // **** Clusters as per DBSCAN **** // 
                                                    
               
             //Core pt. cluster formation 
             double t1_Cluster =(double)clock()/CLOCKS_PER_SEC;  
             vector<int>temp_cluster;
             int min; 
             for(int i=0;i<range_matrix.size();i++)
             {  
                   if(range_matrix[i].size() > minpts)// 'i' core pt. check
		   {
		         for(int j=0;j<range_matrix[i].size();j++)
		             {
		                if(range_matrix[range_matrix[i][j].first].size() > minpts)// 'j' core pt. check
		                {
		                   temp_cluster.push_back(cluster[range_matrix[i][j].first].rep);
		                }                  
		             }	
		             
		         if(temp_cluster.size() == 0)//(single core pt. cluster)no link with other core pts.
                             {
                                 cluster[i].status = 1;
                                 cluster[i].rep = i;
                                 cluster[i].core = 1;
                                 continue;
                             }
                             
                         min = *min_element(temp_cluster.begin(),temp_cluster.end()); // find the min. rep. of all pts.
                         
                          if(cluster[i].rep < min)
		             { 
		                cluster[i].rep = cluster[i].rep;
                                cluster[i].status = 1;
		                for(int j=0;j<range_matrix[i].size();j++)
		                {
		                   if(range_matrix[range_matrix[i][j].first].size() > minpts)// core pt. check
				        {
                                           link_cluster(cluster[range_matrix[i][j].first].rep,cluster[i].rep);
				           cluster[range_matrix[i][j].first].rep = cluster[i].rep;
                                           cluster[range_matrix[i][j].first].status = 1; 
				        }

		                }

		             }
		             
		             
		             if(cluster[i].rep >= min)
		             { 
                                link_cluster(cluster[i].rep,min);
		                cluster[i].rep = min;
		                for(int j=0;j<range_matrix[i].size();j++)
		                {
		                   if(range_matrix[range_matrix[i][j].first].size() > minpts)// core pt. check
				        {
                                           link_cluster(cluster[range_matrix[i][j].first].rep,min);
				           cluster[range_matrix[i][j].first].rep = min;
                                           cluster[range_matrix[i][j].first].status = 1;
				        }

		                }

		             }		             
		             
		             	   
		   }//end if
		   
		   
		   temp_cluster.erase(temp_cluster.begin(),temp_cluster.end());
		   
		   
             }//end for 
              
             //Non-Core pt. cluster formation              	 
             priority_queue< pair<long double,int>, vector< pair<long double,int> >, greater< pair<long double,int> > >nearest_core;
             int nearest;  
             for(int i=0;i<range_matrix.size();i++)
	       {
			  if(range_matrix[i].size() <= minpts)// 'i' non-core pt. check
			  {
		             if(range_matrix[i].size() == 0)
		               continue;
		             
			     for(int j=0;j<range_matrix[i].size();j++)
			     {
				if(range_matrix[range_matrix[i][j].first].size() > minpts)// 'j' core pt. check
				nearest_core.push(pair<long double,int>(range_matrix[i][j].second,range_matrix[i][j].first));		        
			     } 
		            
		             if(nearest_core.size() > 0){
		             nearest = nearest_core.top().second;
		             cluster[i].rep = cluster[nearest].rep;
		             cluster[i].status = 1;
		             } 

			  }//end if

		    while(!nearest_core.empty())
		    nearest_core.pop();

	       }//end for 
	       
	       //Noise points.
	          
	          for(int i=0;i<points_num;i++)
		       {
			  if(cluster[i].core==0 && cluster[i].rep==-1)
			   {
			      noise.push_back(i);    
			   }
		       } 
		       
	      cout<<"No. of noise points:"<<noise.size()<<endl;
	      double t2_Cluster =(double)clock()/CLOCKS_PER_SEC;
	      double t_Cluster = (t2_Cluster - t1_Cluster);
	      
	      cout<<"Final clusters:"<<(t2_Cluster-t1_Cluster)<<endl;
	      
	      //Write Clusters to file
	      disp_cluster();
	      
	      
	      //Total TIME 
              double Time = (t_loadData+t_iForest+t_massMatrix+t_massParam+t_Cluster);             
              cout<<"Total time:"<<Time<<endl;
              
              //Final Output
              ofstream output("output_mbc",ios::out);
              output<<"Data load time:"<<t_loadData<<endl;
              output<<"iForest load time:"<<t_iForest<<endl;
              output<<"Mass-matrix creation time:"<<t_massMatrix<<endl;
              output<<"Parameters+Core+Non-core creation time:"<<t_massParam<<endl;
              output<<"Cluster creation time:"<<t_Cluster<<endl;
              output<<"Total time for MBSCAN:"<<Time<<endl;
              
              output<<"#Core pts.: "<<core.size()<<endl;
              output<<"#Non-Core pts.: "<<non_core.size()<<endl;
              output<<"#Noise pts.: "<<noise.size()<<endl; 
              
              output.close();
              
              //Total MEMORY
               ofstream mem("Memory_mbc",ios::out);
	       void process_mem_usage(double&, double&);
	       double vm, rss;
	       process_mem_usage(vm, rss);
	       mem << vm <<" "<< rss << endl;
              
                                          
       return 0;
       
}// End Main


//Write Parameters to file
void disp_param(int tree_num, int tree_size, int points_num, int minpts, long double mu)
{
     ofstream param, param_bin;
     param.open("Parameters",ios::out);
     param_bin.open("Parameters.bin",ios::out|ios::binary);
     
     param<<"No. of iTrees: "<<tree_num<<endl;
     param<<"Size of each iTree: "<<tree_size<<endl;
     param<<"Total no. of data points: "<<points_num<<endl;
     param<<"Core pt. formation threshold: "<<minpts<<endl;
     param<<"Mu threshold: "<<mu<<endl;
     
     
     param_bin.write((char*)&tree_num,sizeof(tree_num));
     param_bin.write((char*)&tree_size,sizeof(tree_size));
     param_bin.write((char*)&points_num,sizeof(points_num));
     param_bin.write((char*)&minpts,sizeof(minpts));
     param_bin.write((char*)&mu,sizeof(mu));
     
     param.close();
     
}


//Write original data_matrix to file
void disp_data_matrix()
{

     ofstream data_matrix,data_matrix_bin;
     data_matrix.open("data_matrix",ios::out);
     data_matrix_bin.open("data_matrix.bin",ios::out|ios::binary);
     
     data_matrix<<Data.size()<<" "<<Data[0].size()<<endl;
     for(int i=0;i<Data.size();i++)
	   {
	      for(int j=0;j<Data[i].size();j++)
	      {
		 data_matrix<<Data[i][j]<<" ";
	      }
	    
	     data_matrix<<endl;
	   } 
	   
     for(int i=0;i<Data.size();i++)
	   {
	      for(int j=0;j<Data[i].size();j++)
	      {
		 data_matrix_bin.write((char*)&Data[i][j],sizeof(Data[i][j]));
	      }	    
	     
	   }     
}


//Load iForest
void load_iTree(int t, string tree_name)
{
    
      string str, word;
      char *ptr;
      int i = iso_tree_matrix.size();
      
      ifstream iso_tree;
      iso_tree.open(tree_name.c_str());
          while(getline(iso_tree,str))
		  {
		     i++;
		     iso_tree_matrix.resize(i);   
		     stringstream ss(str);
		     //cout<<"Line read"<<endl; 
		          while (ss >> word)
				{
				    //cout<<"Word read:"<<word;
				    if(word == "-1")
				    {				        
				       break;
				    }
				    else 
				     iso_tree_matrix[i-1].push_back(atoi(word.c_str()));
				    
				}
				
		       	
		  }
          
      iso_tree.close(); 
      
 }
 
 
 //Write iForest to file
 void disp_iForest()
 {
    ofstream iforest,iforest_bin;
    iforest.open("iForest",ios::out);
    iforest_bin.open("iforest.bin",ios::out|ios::binary);
    
    for(int i=0;i<iso_tree_matrix.size();i++)
    {
      for(int j=0;j<iso_tree_matrix[i].size();j++)
      {
         iforest<<iso_tree_matrix[i][j]<<" ";
      }
      
      iforest<<endl;
    }
    
    iforest.close();
    
    for(int i=0;i<iso_tree_matrix.size();i++)
    {
      for(int j=0;j<iso_tree_matrix[i].size();j++)
      {
         iforest_bin.write((char*)&iso_tree_matrix[i][j],sizeof(iso_tree_matrix[i][j]));
      }
      
    }
 
    iforest_bin.close();
 }

                                         
//Calculate Mass-matrix                                            
double calMass_xy(int x, int y, int tree_num)
{   
   // Find the lowest node where x,y are present (LCA)
   
      int l=0,level,node_num,size;
      int t, mass = 0;      
      
           for(t = 1; t <= tree_num; t++)
           {
              //cout<<t<<"---"<<l; 
              level=-1;            
	      for(l = l+1; l < tree_index[t+1]; l++)
	      {
	                  
		 if(iso_tree_matrix[l][0] > level){
		 
			  bool X = binary_search(iso_tree_matrix[l].begin()+2,iso_tree_matrix[l].end(),x); 
			  bool Y = binary_search(iso_tree_matrix[l].begin()+2,iso_tree_matrix[l].end(),y);
			  if(X && Y)
			  {
			     
				level = iso_tree_matrix[l][0];
				node_num = iso_tree_matrix[l][1];
				size = iso_tree_matrix[l].size()-2; //return the total no. of points in that lowest node containing x,y.
			        lowest_node.node_id[t-1] = l; //lowest node index in iForest array
			     
			  }
		  
		   }//end if 
		 
	      }//end for 
	      
	      //cout<<"----"<<lowest_node.node_id[t-1]<<endl;
	      mass += size;
	      
	  }//end for
      
      Node_id[x][y] = lowest_node; //store the lowest node ids of each itree
      Node_id[y][x] = lowest_node;
      
      return mass;
      
}  


//Write Mass-matrix to file
void dispMass_xy(int tree_num)
{
    ofstream me_xy, mass_bin;
    me_xy.open("mass_matrix",ios::out);
    mass_bin.open("mass_matrix.bin",ios::out|ios::binary);
    long double temp;
    for(int i=0;i<mass_matrix.size();i++)
    {
      //cout<<"Display:"<<i<<endl;
       for(int j=0;j<mass_matrix[i].size();j++)
       {  
          temp = mass_matrix[i][j];
          mass_bin.write((char*)&temp,sizeof(temp));        
          me_xy<<i<<" "<<j<<" "<<mass_matrix[i][j]<<endl;
       }
       
       
    }
    
    mass_bin.close();
    me_xy.close();

}


//Write Lowest-node of iTrees to file 
void dispLowestNode_xy(int tree_num)
{
   ofstream LowestNode_xy, LowestNode_bin;
   LowestNode_xy.open("lowest_node",ios::out);
   LowestNode_bin.open("lowest_node.bin",ios::out|ios::binary);
   TreeNode temp;
   
   for(int i=0;i<Node_id.size();i++)
   {
       //cout<<"Display:"<<i<<endl;
       for(int j=0;j<Node_id[i].size();j++)
       {  
          temp = Node_id[i][j];
          
          //Write to binary file
          LowestNode_bin.write((char*)&temp,sizeof(temp)); 
                 
          //Write to file
          LowestNode_xy<<i<<" "<<j<<" ";          
          //Display the array index of the lowest node from the in-memory stored iForest
          for(int k=0;k<tree_num;k++)
          {
              LowestNode_xy<<Node_id[i][j].node_id[k]<<" ";
          }
          
          LowestNode_xy<<"-1"<<endl;
          
          
          
       }
       
       
       
   }
   
    LowestNode_bin.close();
    LowestNode_xy.close();

}


//Link cluster
void link_cluster(int rp,int rq)
{
   if(rp<rq)
   {
      for(int i=0;i<range_matrix.size();i++)  
      {
        if(cluster[i].rep==rq)
         cluster[i].rep=rp;
      }
   }

   if(rq<rp)
   {
      for(int i=0;i<range_matrix.size();i++)  
      {
        if(cluster[i].rep==rp)
         cluster[i].rep=rq;
      }
   }
}//end func


//Write clusters to file
void disp_cluster()
{
      ofstream out_cluster_global("Cluster",ios::out);
      if(!out_cluster_global)
	     {
	      cout<<"Cannot open the out_cluster file\n"<<endl;
	      exit(0);
	     }
	     int p;
	     
	     for(p=0;p<range_matrix.size();p++)
		 {
                   out_cluster_global<<p<<"]";
		   out_cluster_global<<cluster[p].status<<"/";
		   out_cluster_global<<cluster[p].rep<<"/";
		   out_cluster_global<<cluster[p].core;
		   out_cluster_global<<endl;

		 }
		 
             out_cluster_global.close(); 
             //out_cluster_global<<endl<<"Cluster status/Cluster Rep./Core-NonCore";	
             
             ofstream out_cluster_bin("Cluster.bin",ios::out|ios::binary);
             for(p=0;p<range_matrix.size();p++)
		 {
		   Cluster clust;
		   clust = cluster[p];
		   
		   out_cluster_bin.write((char*)&clust,sizeof(clust));   
		   
                
		 }
		 
             		 
             //Write Core + Non-core + Noise pts. to file
             ofstream core_pt("core",ios::out);
             ofstream core_pt_bin("core.bin",ios::out|ios::binary);
             
             ofstream non_core_pt("non-core",ios::out);
             ofstream non_core_pt_bin("non-core.bin",ios::out|ios::binary);
             
             ofstream noise_pt("noise",ios::out);
             ofstream noise_pt_bin("noise.bin",ios::out|ios::binary);
             
             for(int i=0;i<core.size();i++)
                core_pt<<core[i]<<endl;             
             for(int i=0;i<core.size();i++)   
               core_pt_bin.write((char*)&core[i],sizeof(core[i]));            	
             
       
             
              for(int i=0;i<non_core.size();i++)
                non_core_pt<<non_core[i]<<endl;
              for(int i=0;i<non_core.size();i++)  
                 non_core_pt_bin.write((char*)&non_core[i],sizeof(non_core[i]));
             		 

              for(int i=0;i<noise.size();i++)             
                noise_pt<<noise[i]<<endl;
              for(int i=0;i<noise.size();i++)  
                noise_pt_bin.write((char*)&noise[i],sizeof(noise[i]));
                
             	
             
             core_pt.close();core_pt_bin.close();
             non_core_pt.close();non_core_pt_bin.close();
             noise_pt.close();noise_pt_bin.close();	 


}



//MEMORY FOOTPRINT
     void process_mem_usage(double& vm_usage, double& resident_set)
	{
	   using std::ios_base;
	   using std::ifstream;
	   using std::string;

	   vm_usage     = 0.0;
	   resident_set = 0.0;

	   // 'file' stat seems to give the most reliable results
	   //
	   ifstream stat_stream("/proc/self/stat",ios_base::in);

	   // dummy vars for leading entries in stat that we don't care about
	   //
	   string pid, comm, state, ppid, pgrp, session, tty_nr;
	   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
	   string utime, stime, cutime, cstime, priority, nice;
	   string O, itrealvalue, starttime;

	   // the two fields we want
	   //
	   unsigned long vsize;
	   long rss;

	   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
		       >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
		       >> utime >> stime >> cutime >> cstime >> priority >> nice
		       >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

	   stat_stream.close();

	   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
	   vm_usage     = vsize / 1024.0;
	   resident_set = rss * page_size_kb;
	}







