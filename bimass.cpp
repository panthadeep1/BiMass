/****
  Code: The 'BiMass' algorithm code. 
  Description: It is an extension of the 'iMass' algorithm.
  BiMass supports batch-mode insertion of data points, whereas iMass was built for point-based insertion.
  Author: Dr. Panthadeep Bhattacharjee
****/
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

vector< vector<long double> >data_matrix_inc;
vector< vector<int> >iso_tree_matrix_inc;

//Storing the tree-id for tracking the lowest node/region while mass calculation
      
      typedef struct iTreeNode{
          public:
          int node_id[20];  //no. of iTrees
      }TreeNode;

vector< vector<TreeNode> >Lowest_Node_inc;
TreeNode lowest_node;

int batch_size; 

vector< vector<long double> >mass_matrix_inc;
vector< vector< pair<int,long double> > >range_matrix_inc;
vector<int>tree_index;

vector<int>core_inc;
vector<int>non_core_inc;
vector<int>noise_inc;

double calMass_xy(int,int, int);

typedef struct Clust{

     public:
      int status;
      int rep;
      int core;
}Cluster;

vector<Cluster>cluster_inc;

void link_cluster(int,int);

void writeVal(int,int,int,int,long double);

int main(int argc, char* argv[])
{

                                      //Load the Parameters for iMass from MBSCAN
                                      
          double t1_loadParam=(double)clock()/CLOCKS_PER_SEC;  
          int tree_num,tree_size,points_num,minpts;
          long double mu;
                                      
          fstream param_bin;
          param_bin.open("Parameters.bin",ios::in|ios::binary);
          if(!param_bin)
           {cout<<"Can't read file!!";exit(0);}
           
          param_bin.read((char*)&tree_num,sizeof(tree_num));
          param_bin.read((char*)&tree_size,sizeof(tree_size));
          param_bin.read((char*)&points_num,sizeof(points_num));
          param_bin.read((char*)&minpts,sizeof(minpts));
          param_bin.read((char*)&mu,sizeof(mu)); 
          
          param_bin.close();
          double t2_loadParam=(double)clock()/CLOCKS_PER_SEC; 
          double t_loadParam = t2_loadParam-t1_loadParam;
          cout<<"\nThe Parameter load time is:"<<t_loadParam<<endl;
          
          cout<<"No. of iTrees: "<<tree_num<<endl;
          cout<<"Size of each iTree: "<<tree_size<<endl;
          cout<<"Total no. of data points: "<<points_num<<endl;
          cout<<"Core pt. formation threshold: "<<minpts<<endl;
          cout<<"Mu threshold: "<<mu<<endl;
          
          
          
          
                                                   // Load the original Data Matrix
                                                   
              //Complete Dataset 
              string data_file = argv[1]; 
              string data_file_string = data_file;
              string data_file_bin = data_file+".bin";
              string init_data_file = data_file_bin;
                                           
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
		   
                data_matrix_inc.resize(nrow,vector<long double>(ncol));
                long double temp;
                for(int i=0;i<=nrow-1;i++)
		    {
		       for(int j=0;j<=ncol-1;j++)
		       {
			 data.read((char*)&temp,sizeof(long double));
			 data_matrix_inc[i][j] = temp;
		       }
		       
		    }		
	        
	        data.close();
	        double t2_loadData=(double)clock()/CLOCKS_PER_SEC;	
	        double t_loadData = t2_loadData-t1_loadData;
	        
	        cout<<"\nThe Original dataset matrix load time is:"<<(t_loadData)<<endl;
		  /*for(int i=0;i<=nrow-1;i++)
		   {
		      for(int j=0;j<=ncol-1;j++)
		      {
			 cout<<data_matrix_inc[i][j]<<" ";
		      }
		    
		     cout<<endl;
		   } */  
		   
		   
		   
		   
		                                        //Load the iForest		                                        
		
		double t1_loadiForest=(double)clock()/CLOCKS_PER_SEC;   
		string str, word;
                char *ptr;
                int i = iso_tree_matrix_inc.size();
                   
		ifstream iso_tree;
		iso_tree.open("iForest",ios::in);
		
		while(getline(iso_tree,str))
		{
                     i++;                              
                     iso_tree_matrix_inc.resize(i); 
                     stringstream ss(str);
                     //cout<<"Line read"<<endl;
                      while (ss >> word)
				{
				    //cout<<"Word read:"<<word;				   
				     iso_tree_matrix_inc[i-1].push_back(atoi(word.c_str()));
				     
				     if(iso_tree_matrix_inc[i-1][0] < 0)
				      tree_index.push_back(i-1); // Store the start index of 'i'th tree
				      				    
				}
				                              
                }  
                
                //tree_index.push_back(iso_tree_matrix_inc.size());
                cout<<endl<<"iForest size (#Nodes+#iTrees):"<<iso_tree_matrix_inc.size()<<" Index of last iTree:"<<tree_index[tree_num-1];
                cout<<endl<<"Tree index size:"<<tree_index.size();
                iso_tree.close();
                double t2_loadiForest=(double)clock()/CLOCKS_PER_SEC;	
	        double t_loadiForest = t2_loadiForest-t1_loadiForest;
                
                int total_nodes = iso_tree_matrix_inc.size()-tree_num;
                cout<<"\nTotal no. of nodes is:"<<total_nodes<<endl;
                
                cout<<"\nThe iForest load time is:"<<t_loadiForest<<endl;
                /*for(int i=0;i<iso_tree_matrix_inc.size();i++)
		    {
		      for(int j=0;j<iso_tree_matrix_inc[i].size();j++)
		      {
			 cout<<iso_tree_matrix_inc[i][j]<<" ";
		      }
		      
		      cout<<endl;
		    } */
		    
		    
		    
		    
		                                 //Load the Lowest-node matrix
		  
		   double t1_loadLowestNode=(double)clock()/CLOCKS_PER_SEC;                               
		   ifstream LowestNode_bin;
                   LowestNode_bin.open("lowest_node.bin",ios::in|ios::binary);
                   TreeNode temp1; 
                   Lowest_Node_inc.resize(nrow);
                   for(int i=0;i<nrow;i++)
		   {
		       //cout<<"Display:"<<i<<endl;
		       for(int j=0;j<nrow;j++)
		       {  
			  		  
			  LowestNode_bin.read((char*)&temp1,sizeof(temp1)); 
			  Lowest_Node_inc[i].push_back(temp1);			  
			  
		       }
		       
		       
		       
		   }
		   
		   LowestNode_bin.close();
		   
		   double t2_loadLowestNode=(double)clock()/CLOCKS_PER_SEC;	
	           double t_loadLowestNode = t2_loadLowestNode-t1_loadLowestNode;
	           cout<<"\nThe Lowest Node load time is:"<<t_loadLowestNode<<endl;
	           
	           /*for(int i=0;i<nrow;i++)
		   {
		       
		       for(int j=0;j<nrow;j++)
		       {  
			  cout<<"\n["<<i<<"---"<<j<<"]:\n";  
			  		  
			  for(int k=0;k<tree_num;k++)
			  {
			      cout<<Lowest_Node_inc[i][j].node_id[k]<<" ";
			  }
			  
		       }
		       
		       cout<<endl<<"-1";
		       
		   }*/ 
		   
		   
		          
                
                    
                                                     // Load the  Mass-Matrix 
                                                     
                    
                    double t1_loadMassMatrix=(double)clock()/CLOCKS_PER_SEC;
                    ifstream mass_bin;
                    mass_bin.open("mass_matrix.bin",ios::in|ios::binary);
                    long double temp2;
                    mass_matrix_inc.resize(nrow);
                    for(int i=0;i<nrow;i++)
		    {
		       for(int j=0;j<nrow;j++)
		       {  
			  mass_bin.read((char*)&temp2,sizeof(temp2));        
			  mass_matrix_inc[i].push_back(temp2);
		       }
		       
		       
		    }
		    mass_bin.close();
		    double t2_loadMassMatrix=(double)clock()/CLOCKS_PER_SEC;	
	            double t_loadMassMatrix = t2_loadMassMatrix-t1_loadMassMatrix;
	            
	            cout<<"\nThe Mass matrix load time is:"<<t_loadMassMatrix<<endl;
	            
	            /*for(int i=0;i<mass_matrix_inc.size();i++)
		    {
		       for(int j=0;j<mass_matrix_inc[i].size();j++)
		       {  
			  cout<<i<<" "<<j<<"----"<<mass_matrix_inc[i][j]<<endl;
		       }
		       
		       cout<<endl;
		    }*/
		    
		    
		    
		    double t_loadVal = (t_loadParam+t_loadData+t_loadiForest+t_loadLowestNode+t_loadMassMatrix);
		    
		    
		    
		    
		    
		    
		    
		                                 /* ---------The 'BiMass' Algorithm----------*/	                                 
		                                 
		    
		    cout<<endl<<"....The BiMass Algorithm....."<<endl;
		    
		                                       //Load the new batch of data pts.
		     
		    double t1_loadNewPoint=(double)clock()/CLOCKS_PER_SEC;                            
		    string new_pt = argv[2]; 
                    string new_pt_string = new_pt;
                    string new_pt_bin = new_pt_string+".bin";
                    string new_pt_file = new_pt_bin;
                    string new_pt_path = new_pt_file;
                    
                    int inrow,incol;
                    long double temp3;
                    
                    ifstream npt;
                    npt.open(new_pt_path.c_str(),ios::in|ios::binary); 
                    
                    npt.read((char*)&inrow,sizeof(int));
                    npt.read((char*)&incol,sizeof(int));  
                    
                    batch_size = inrow; //no. of pts. per batch 
                    
                    cout<<"\nIncremental dataset file:"<<new_pt_file<<"\n";
		    cout<<"Rows:"<<inrow;
		    cout<<" Columns:"<<incol;
		    
		    data_matrix_inc.resize(nrow+inrow);
                    cout<<endl<<"Inc. Data Matrix size:"<<data_matrix_inc.size();
                    
                    for(int i=nrow;i<(nrow+inrow);i++)
                    {
                         for(int j=0;j<incol;j++)
                         {
                            npt.read((char*)&temp3,sizeof(temp3)); 
                            data_matrix_inc[i].push_back(temp3);
                         }            
		    } 
		    
		    double t2_loadNewPoint=(double)clock()/CLOCKS_PER_SEC;	
	            double t_loadNewPoint = t2_loadNewPoint-t1_loadNewPoint;
	            
	          cout<<"\nIncremental data matrix load time is:"<<(t_loadNewPoint)<<endl;
		  /*for(int i=nrow;i<(nrow+inrow);i++)
		   {
		      for(int j=0;j<incol;j++)
		      {
			 cout<<data_matrix_inc[i][j]<<" ";
		      }
		    
		     cout<<endl;
		   }*/  
		   
		   
		  
		   
		   
		                                     //Update the iForest incrementally
		                                     
		                                    
		   // Store the new batch pts.
		   vector< vector<long double> >iPoint;
		   iPoint.resize(inrow);
		   for(int i=0;i<inrow;i++)
		   {
		       for(int j=0;j<incol;j++)
		       {
		           iPoint[i].push_back(data_matrix_inc[nrow+i][j]);
		       }
		   
		   }
		   
		   //Display the new batch of points
		   cout<<endl<<"The new batch of pts. are:"<<endl;
		   /*for(int i=0;i<inrow;i++)
		   {
		       for(int j=0;j<incol;j++)
		       {
		           cout<<iPoint[i][j]<<" " ;
		       }
		       
		       cout<<endl;
		   
		   }*/
		   
		      
		   
		   
		   // iTree indices
		   cout<<"Test enter 1";
		   cout<<endl<<"Tree index size:"<<tree_index.size()<<endl; //gdb err
		  
		   
		   /*for(int i=0;i<tree_index.size();i++)
		   {
		      cout<<i<<"---"<<tree_index[i]<<"...."<<iso_tree_matrix_inc[tree_index[i]][0]<<endl;    
		   }*/
		  
		     		   
		   double t1_update_iForest=(double)clock()/CLOCKS_PER_SEC;
		   
		   int treeId,nodeIn=0;
		   int start_index,root_index,last_index; 
		   ifstream itree_pq;		
		   
		  		   
		   for(int i=0;i<tree_num;i++)
		   {
		       treeId = i+1;  // tree Id
		       start_index = tree_index[i]; //tree start index/row in iso-tree-matrix-inc array
		       root_index = start_index + 1;  // tree root index/row in iso-tree-matrix-inc array
		       last_index = tree_index[i+1]-1; // tree last index/row in iso-tree-matrix-inc array
		       
		       //cout<<i<<"---"<<start_index<<"...."<<root_index<<endl;  
		       string tree_pq_path = "./iTrees_pq/itree_pq";
		       string pqId;
		       stringstream ss;
		       ss << treeId; 
		       pqId = ss.str();
		       tree_pq_path += pqId;
		       //cout<<tree_pq_path<<endl; 
		       
		         
			   // Place the new batch of pts. in the 'i' th iTree
			      for(int bpt=0;bpt<inrow;bpt++)
			      {
				       
				       int level_current,node_current;
				       int level_next=0,node_next=1; // initialize to level 0 node 1
				       int split_attr; long double split_val;
				       string str,word;
				       char *ptr;
				       int j;
				          
				          itree_pq.open(tree_pq_path.c_str(),ios::in);//open itree_pq
				       
					       while(getline(itree_pq,str))//Positioning a pt. in appropriate nodes of that iTree
					       {
						    stringstream ss(str);
						    ss >> level_current >> node_current >> split_attr >> word;
						    split_val = strtold(word.c_str(),&ptr);
						    //cout<<endl<<level_current<<" "<<node_current<<" "<<split_attr<<" "<<split_val<<endl;
						    
						    
						    if(level_current == level_next && node_current == node_next)
						    {
							//Push the new point 'iPoint' into that node of 'i'th tree(iso-tree-matrix-inc array)
							//cout<<"tree-index"<<i+1<<": "<<tree_index[i+1]<<endl;
							for(j=root_index;j<tree_index[i+1];j++)
							{
							   //cout<<endl<<"j:"<<j<<endl;
							   		                    
							  if(iso_tree_matrix_inc[j][0] == level_current && iso_tree_matrix_inc[j][1] == node_current)
							  {
							      //cout<<endl<<"Enter/Push j:"<<j<<endl;
							      if(!binary_search(iso_tree_matrix_inc[j].begin()+2,iso_tree_matrix_inc[j].end(),nrow+bpt)) 
								iso_tree_matrix_inc[j].push_back(nrow+bpt); //Push new batch pt.
							      
							      break;
							      
							  }//end if
							    
							}//end for
						
							 //Decide the next node for 'iPoint' in the 'i' th tree
							    if(iPoint[bpt][split_attr] <= split_val)
							    {
								level_next = level_current + 1;
								node_next = 2*node_current;     
							    }  
							    
							    if(iPoint[bpt][split_attr] > split_val)
							    {
								level_next = level_current + 1;
								node_next = 2*node_current + 1;     
							    } 
							    
							    //cout<<endl<<"Level next:"<<level_next<<" Node next:"<<node_next<<endl; 
						
						    }//end if 
						    		    
						   				    
					       }//end while
					       
					       			       
				           itree_pq.close(); //close itree_pq    
				       
				 } //End of placing one pt. of the batch. Repeat 'for loop' for next pt.
				 
			         
				 
				 
				 // **** No. of affected nodes per iTree ****
				 for(int i=root_index;i<=last_index;i++)
				 {
				     
				     for(int j=nrow;j<nrow+inrow;j++)
				     {
				        
				        bool X = binary_search(iso_tree_matrix_inc[i].begin()+2,iso_tree_matrix_inc[i].end(),j);	
						if(X) //newly entered pt(s). present in the node
						{
						    nodeIn++; // no. of affected nodes
						    break;				      						
						}//end if
				        			      
				     }//end for
				     
				            
				 }//end for      
				       
						                 
		   }//end for; Repeat loop for next iTree
		   
		   
		   //Display updated iForest
		   /*for(int i=0;i<iso_tree_matrix_inc.size();i++)
		   {
		         for(int j=0;j<iso_tree_matrix_inc[i].size();j++)
		         {
		             cout<<iso_tree_matrix_inc[i][j]<<" ";		         
		         }
		         
		         cout<<endl;
		   
		   }*/
		   
		   
		   
		   		     
		   double t2_update_iForest=(double)clock()/CLOCKS_PER_SEC;	
	           double t_update_iForest = t2_update_iForest-t1_update_iForest; 
	           
	           
	           cout<<"\nNo. of nodes affected:"<<nodeIn<<"/"<<(nodeIn*100/total_nodes)<<"%"<<endl;	           
	           cout<<"\nIncremental iForest update time is:"<<(t_update_iForest)<<endl;  
		                                                         
		                                                        
		                                                         
		                             // Update the Mass-matrix incrementally
	
	   	                             
		                             
	   long double treeIn,lowestIn;
	   int count_Npt, sum_Npt;  //count and sum of new pts. in the affected lowest nodes
	   int iNewPoint;  //index of the new point
           points_num = nrow+inrow; //total no. of points after 'inrow' batch insertion		   		        
	
	   double t1_update_mass_matrix=(double)clock()/CLOCKS_PER_SEC; 	   		                 
	   for(int i=0;i<mass_matrix_inc.size();i++) //update the old mass-matrix
	   {
	      for(int j=0;j<mass_matrix_inc[i].size();j++)
	      {
	          //cout<<"\n["<<i<<"---"<<j<<"]"; 
	          
	          treeIn = 0;
	          sum_Npt = 0;
	          
	          //Check if the new point is present in lowest node of each iTree
	          for(int k=0;k<tree_num;k++)
	          {
	            lowestIn = Lowest_Node_inc[i][j].node_id[k];
	            //cout<<treeIn<<" ";
	            count_Npt = 0;
	            
			     for(int l=0;l<inrow;l++)
			      {
			          iNewPoint = nrow+l;
			          bool X = binary_search(iso_tree_matrix_inc[lowestIn].begin()+2,iso_tree_matrix_inc[lowestIn].end(),iNewPoint);
			           // if present in lowest node
				    if(X)
				    {
				       count_Npt++; //no. of new pts. in the lowest node containing (x,y)
				    }
				    
			      }//end for 
	            
			    // if new pts. present in lowest node, then node affected
			    if(count_Npt>0)
			    {
			       treeIn++; //no. of affected iTrees
			    }
			    
			    sum_Npt += count_Npt;
	             
	          }// end for 
	          
	          //cout<<treeIn;
	          //updated 'pmm' value using the derived formula (BiMass draft)     
	          mass_matrix_inc[i][j] = (nrow*mass_matrix_inc[i][j] + (sum_Npt/(long double)tree_num))/(long double)points_num; 
	    
	      }// inner end for 
	      
	   }// outer end for     
	   
	   
	   
	   //Update the probability mean-mass values for the newpoint non-incrementally	
	        
	   mass_matrix_inc.resize(points_num);  // increase mass-matrix size	   
	   Lowest_Node_inc.resize(points_num);   // increase lowest-node matrix size
	   
	   long double R = 0;
	   long double mass_xy; 
	   
	   
	   for(int i = 0; i < mass_matrix_inc.size()-inrow; i++)
	   {
	    
	       //cout<<i<<endl;
	            
	       for(int j = 0; j < inrow; j++) //'inrow' is the no. of insertions in a batch 
	       {   
	          iNewPoint = nrow + j;
	          R = calMass_xy(i,iNewPoint,tree_num); //calculate the mass dissimilarity me_(x,y) based on MBSCAN	            
		             
                  mass_xy = (long double)R/(points_num*tree_num); 
                  
                  mass_matrix_inc[i].push_back(mass_xy);                                     
                  mass_matrix_inc[iNewPoint].push_back(mass_xy);
                  
               }   
               
	   }
	   
	   for(int i = 0; i < inrow; i++) //fill the last sq. block
	   {
	        for(int j = i; j < inrow; j++)
	         {
	            iNewPoint = nrow + j;
	             R = calMass_xy(nrow+i,iNewPoint,tree_num);
	             mass_xy = (long double)R/(points_num*tree_num);
	             
	             mass_matrix_inc[nrow+i].push_back(mass_xy);
	             
	             if(iNewPoint == nrow+i) //diagonal or self-dissimilarity check
	               continue;
	             else
	               mass_matrix_inc[iNewPoint].push_back(mass_xy);
	         
	         }
	        	   
	   }
	   
	   double t2_update_mass_matrix=(double)clock()/CLOCKS_PER_SEC;	
	   double t_update_mass_matrix = t2_update_mass_matrix-t1_update_mass_matrix; 
	   
	   cout<<"\nIncremental Mass-matrix update time is:"<<(t_update_mass_matrix)<<endl;
	   
	  //Display updated Mass-matrix
	  
	  /*for(int i=0;i<mass_matrix_inc.size();i++)
	  {
	     for(int j=0;j<mass_matrix_inc[i].size();j++)
	     {
	         cout<<"["<<i<<"---"<<j<<"]"<<mass_matrix_inc[i][j]<<endl;
	     }
	     
	  }*/
	  
	 
	       
	                                           //Update the mu-threshold  
	       
	                                           
	       double t1_mu=(double)clock()/CLOCKS_PER_SEC;                                    
	       long double mu_th = 0;
               for(int i=0;i<mass_matrix_inc.size();i++)
               {
                   for(int j=i;j<mass_matrix_inc[i].size();j++)
                   {
                       //mu_th += mass_matrix_inc[i][j];
                       if(i==j)
                       {
                          if(mass_matrix_inc[i][j]>mu_th)
                           mu_th = mass_matrix_inc[i][j]; 
                       }
                   } 
               } 
               //mu_th /= pow(mass_matrix_inc.size(),2);
               mu = mu_th+0.1; 
               double t2_mu=(double)clock()/CLOCKS_PER_SEC;
               double t_mu=t2_mu-t1_mu;
               
               cout<<endl<<"Threshold mu/Mean mu:"<<mu_th<<"/"<<mu<<"; Time mu:"<<(t_mu)<<endl;
               
               
               
               
               
                                             // Update the core and non-core points                                        
	                                                           	                                                            
	       Cluster clus;      
	       int mass_th = 0;
               pair<int,long double>a;
               range_matrix_inc.resize(points_num);  
               
               double t1_CoreNoncore=(double)clock()/CLOCKS_PER_SEC;
               for(int i=0;i<mass_matrix_inc.size();i++)
               {
                  mass_th = 0;
                   for(int j=0;j<mass_matrix_inc[i].size();j++)
                    {
                        if(mass_matrix_inc[i][j] <= mu){
                         a.first = j;
                         a.second = mass_matrix_inc[i][j];
                         range_matrix_inc[i].push_back(a);
                         mass_th++;
                        }
                    }
               
                 
               
               if(mass_th > minpts){
                     core_inc.push_back(i);
                     clus.status=1;
                     clus.rep=i;
                     clus.core=1;
                    } 
                    
               else{
                     non_core_inc.push_back(i);
                     clus.status=0;
                     clus.rep=-1;
                     clus.core=0;
                    }    
                    
               cluster_inc.push_back(clus);
               
             }
             
             double t2_CoreNoncore=(double)clock()/CLOCKS_PER_SEC;
             double t_CoreNoncore = t2_CoreNoncore-t1_CoreNoncore;
             
             cout<<endl<<"No. of core points: "<<core_inc.size();
             cout<<endl<<"No. of non-core points: "<<non_core_inc.size();
             
             cout<<"\nCore Non-core points update time is:"<<(t_CoreNoncore)<<endl;
             
             
             //Initial Clusters
               /*for(int i=0;i<cluster_inc.size();i++)
	       {
		     cout<<i<<"] ";
		     cout<<cluster_inc[i].status<<"/";
		     cout<<cluster_inc[i].rep<<"/";
		     cout<<cluster_inc[i].core<<endl;

	       } */
	       
	      
	       
	       //******Case When No Clusters are formed******//
	      
	       if(core_inc.size()==0)
		{
		  cout<<"\nNo additional clusters possible\n";
		  cout<<"\nNoise points:\n";
		            
		       
		       for(int i=0;i<points_num;i++)
		       {
			  if(cluster_inc[i].core==0 && cluster_inc[i].rep==-1)
			   {
			     //cout<<i<<" ";
			     noise_inc.push_back(i);    
			   }
		       } 
		               
                 }
             
               //*/
                                   
                                                          
                                            // **** Clusters as per MBSCAN **** //
                                            
                                             
             //Core pt. cluster formation 
             double t1_Cluster =(double)clock()/CLOCKS_PER_SEC;  
             vector<int>temp_cluster;
             int min;                                               
                                                           
                                                           
             for(int i=0;i<range_matrix_inc.size();i++)
             {                                
                                                     
                 if(range_matrix_inc[i].size() > minpts)// 'i' core pt. check
		 {
		        for(int j=0;j<range_matrix_inc[i].size();j++)
		             {
		                if(range_matrix_inc[range_matrix_inc[i][j].first].size() > minpts)// 'j' core pt. check
		                {
		                   temp_cluster.push_back(cluster_inc[range_matrix_inc[i][j].first].rep);
		                }                  
		             }
		             
		             
		         if(temp_cluster.size() == 0)//(single core pt. cluster)no link with other core pts.
                             {
                                 cluster_inc[i].status = 1;
                                 cluster_inc[i].rep = i;
                                 cluster_inc[i].core = 1;
                                 continue;
                             }     
		             	
		         min = *min_element(temp_cluster.begin(),temp_cluster.end()); // find the min. rep. of all pts.
		         
		             if(cluster_inc[i].rep < min)
		             { 
		                cluster_inc[i].rep = cluster_inc[i].rep;
                                cluster_inc[i].status = 1;
		                for(int j=0;j<range_matrix_inc[i].size();j++)
		                {
		                   if(range_matrix_inc[range_matrix_inc[i][j].first].size() > minpts)// core pt. check
				        {
                                           link_cluster(cluster_inc[range_matrix_inc[i][j].first].rep,cluster_inc[i].rep);
				           cluster_inc[range_matrix_inc[i][j].first].rep = cluster_inc[i].rep;
                                           cluster_inc[range_matrix_inc[i][j].first].status = 1; 
				        }

		                }

		             }
		             
		             if(cluster_inc[i].rep >= min)
		             { 
                                link_cluster(cluster_inc[i].rep,min);
		                cluster_inc[i].rep = min;
		                for(int j=0;j<range_matrix_inc[i].size();j++)
		                {
		                   if(range_matrix_inc[range_matrix_inc[i][j].first].size() > minpts)// core pt. check
				        {
                                           link_cluster(cluster_inc[range_matrix_inc[i][j].first].rep,min);
				           cluster_inc[range_matrix_inc[i][j].first].rep = min;
                                           cluster_inc[range_matrix_inc[i][j].first].status = 1;
				        }

		                }

		             }	
		             
		             
		 
		 } //end if   
		 
		 
		                temp_cluster.erase(temp_cluster.begin(),temp_cluster.end());   
		                                    
             
             } //end for
             
             
            
             //Non-Core pt. cluster formation
             priority_queue< pair<long double,int>, vector< pair<long double,int> >, greater< pair<long double,int> > >nearest_core;
             int nearest;
             for(int i=0;i<range_matrix_inc.size();i++)
	     {
	     
	             if(range_matrix_inc[i].size() <= minpts)// 'i' non-core pt. check
			  {
		             if(range_matrix_inc[i].size() == 0)
		               continue;
		             
			     for(int j=0;j<range_matrix_inc[i].size();j++)
			     {
				if(range_matrix_inc[range_matrix_inc[i][j].first].size() > minpts)// 'j' core pt. check
				nearest_core.push(pair<int,int>(range_matrix_inc[i][j].second,range_matrix_inc[i][j].first));		        
			     } 
		            
		             if(nearest_core.size() > 0){
		             nearest = nearest_core.top().second;
		             cluster_inc[i].rep = cluster_inc[nearest].rep;
		             cluster_inc[i].status = 1;
		             } 

			  }//end if
			  
			  
                      while(!nearest_core.empty())
		      nearest_core.pop();
              
             }//end for     
             
                                                         
              //Noise points.
	          
	          for(int i=0;i<points_num;i++)
		       {
			  if(cluster_inc[i].core==0 && cluster_inc[i].rep==-1)
			   {
			      noise_inc.push_back(i);    
			   }
		       } 
		       
	       cout<<"\nNo. of noise points:"<<noise_inc.size()<<endl;	
	       double t2_Cluster =(double)clock()/CLOCKS_PER_SEC;
	       double t_Cluster = (t2_Cluster - t1_Cluster);
	       
	       cout<<"Updated clusters time:"<<(t_Cluster)<<endl;      
	       
	       
	                                
	                                 
	                                         //Total time for 'iMass' algorithm. 
	                                        
	       
	       double t_iMass =(t_loadNewPoint+t_update_iForest+t_update_mass_matrix+t_mu+t_CoreNoncore+t_Cluster); 
	       double total_time = t_loadVal+t_iMass;
	       
	       cout<<"Total time for BiMass:"<<total_time<<endl; 
	       ofstream time_BiMass("Time_BiMass",ios::app);
	       time_BiMass<<total_time<<endl;
	       time_BiMass.close();
	       
	       //Nodes affected
	       ofstream affected_nodes("iNodes",ios::app);
	       affected_nodes<<(nodeIn*100/total_nodes)<<endl;
	       affected_nodes.close();       
	       
	       
	       //Output for'iMass'
	       ofstream output("output_BiMass",ios::app);
	       output<<"File batch:"<<new_pt_file<<endl;
	       output<<"Data load time:"<<t_loadVal<<endl;
	       output<<"New point insertion time:"<<t_loadNewPoint<<endl;
	       output<<"iForest update time:"<<t_update_iForest<<endl;
	       output<<"Mass-matrix update time:"<<t_update_mass_matrix<<endl;
	       output<<"Core Non-core update time:"<<t_CoreNoncore<<endl;
	       output<<"Cluster time:"<<t_Cluster<<endl;
	       output<<"Total iMass time:"<<total_time<<endl;
	       output<<"#Core points:"<<core_inc.size()<<endl;
	       output<<"#Non-Core points:"<<non_core_inc.size()<<endl;
	       output<<"#Noise points:"<<noise_inc.size()<<endl;
	       output<<"--------------------------------------"<<endl;
	       output.close();
	       
	       
	       //Write values to file 
	       writeVal(tree_num,tree_size,points_num,minpts,mu);
	       
	       
	       
	       //Total MEMORY
               ofstream mem("Memory_BiMass",ios::app);
	       void process_mem_usage(double&, double&);
	       double vm, rss;
	       process_mem_usage(vm, rss);
	       mem << vm <<" "<< rss << endl;
	       
	      

   return 0;
}


//Calculate Mass-matrix                                            
double calMass_xy(int x, int y, int tree_num)
{   
   // Find the lowest node where x,y are present (LCA)
   
      int l=0,level,node_num,size;
      int t, mass = 0;      
      
           for(t = 0; t < tree_num; t++)
           {
              //cout<<t<<"---"<<l; 
              level=-1;            
	      for(l = l+1; l < tree_index[t+1]; l++)
	      {
	                  
		 if(iso_tree_matrix_inc[l][0] > level){
		 
			  bool X = binary_search(iso_tree_matrix_inc[l].begin()+2,iso_tree_matrix_inc[l].end(),x); 
			  bool Y = binary_search(iso_tree_matrix_inc[l].begin()+2,iso_tree_matrix_inc[l].end(),y);
			  if(X && Y)
			  {
			     
				level = iso_tree_matrix_inc[l][0];
				node_num = iso_tree_matrix_inc[l][1];
				size = iso_tree_matrix_inc[l].size()-2; //return the total no. of points in that lowest node containing x,y.
			        lowest_node.node_id[t] = l; //lowest node index in iForest array
			     
			  }
		  
		   }//end if 
		 
	      }//end for 
	      
	      //cout<<"----"<<lowest_node.node_id[t]<<endl;
	      mass += size;
	      
	  }//end for
      
      Lowest_Node_inc[x].push_back(lowest_node); //store the lowest node ids of each itree for new pt.
      Lowest_Node_inc[y].push_back(lowest_node);
      
      return mass;
      
} 


//Link cluster
void link_cluster(int rp,int rq)
{
   if(rp<rq)
   {
      for(int i=0;i<range_matrix_inc.size();i++)  
      {
        if(cluster_inc[i].rep==rq)
         cluster_inc[i].rep=rp;
      }
   }

   if(rq<rp)
   {
      for(int i=0;i<range_matrix_inc.size();i++)  
      {
        if(cluster_inc[i].rep==rp)
         cluster_inc[i].rep=rq;
      }
   }
}//end func



//Write values to file
void writeVal(int tree_num,int tree_size,int points_num,int minpts,long double mu)
{
          //Write parameters
          fstream param_bin;
          param_bin.open("Parameters.bin",ios::out|ios::binary);
          if(!param_bin)
           {cout<<"Can't read file!!";exit(0);}
          
          tree_size += batch_size; 
          param_bin.write((char*)&tree_num,sizeof(tree_num));
          param_bin.write((char*)&tree_size,sizeof(tree_size));
          param_bin.write((char*)&points_num,sizeof(points_num));
          param_bin.write((char*)&minpts,sizeof(minpts));
          param_bin.write((char*)&mu,sizeof(mu)); 
          
          param_bin.close();
          
          fstream param;
          param.open("Parameters",ios::out);
          if(!param)
           {cout<<"Can't read file!!";exit(0);}
           
          param<<"No. of iTrees: "<<tree_num<<endl;
          param<<"Size of each iTree: "<<tree_size<<endl;
          param<<"Total no. of data points: "<<points_num<<endl;
          param<<"Core pt. formation threshold: "<<minpts<<endl;
          param<<"Mu threshold: "<<mu<<endl;  
          
          param.close();          
          cout<<"\nParameters written\n";
          
          //Write updated data matrix
          ofstream data_matrix,data_matrix_bin;          
          data_matrix_bin.open("data_matrix.bin",ios::out|ios::binary);
          if(!data_matrix_bin)
           {cout<<"Can't read file!!";exit(0);}
          
          int nrow = data_matrix_inc.size();
          int ncol = data_matrix_inc[0].size(); 
          data_matrix_bin.write((char*)&nrow,sizeof(nrow));
          data_matrix_bin.write((char*)&ncol,sizeof(ncol)); 
           
          for(int i=0;i<data_matrix_inc.size();i++)
	   {
	      for(int j=0;j<data_matrix_inc[i].size();j++)
	      {
		 data_matrix_bin.write((char*)&data_matrix_inc[i][j],sizeof(data_matrix_inc[i][j]));
	      }	    
	     
	   }     
	   
	   data_matrix_bin.close();
	   
	   data_matrix.open("data_matrix",ios::out);
           if(!data_matrix)
           {cout<<"Can't read file!!";exit(0);}
           
           data_matrix<<nrow<<" "<<ncol<<endl;           
           for(int i=0;i<data_matrix_inc.size();i++)
	   {
	      for(int j=0;j<data_matrix_inc[i].size();j++)
	      {
		 data_matrix<<data_matrix_inc[i][j]<<" ";
	      }
	    
	     data_matrix<<endl;
	   } 	   
	   data_matrix.close();
	   
	   cout<<"Updated data matrix written\n";
	   
	   
	   //Write updated iForest
	   ofstream iforest,iforest_bin;
           iforest_bin.open("iforest.bin",ios::out|ios::binary);
           if(!iforest_bin)
           {cout<<"Can't read file!!";exit(0);}
           for(int i=0;i<iso_tree_matrix_inc.size();i++)
	    {
	      for(int j=0;j<iso_tree_matrix_inc[i].size();j++)
	      {
		 iforest_bin.write((char*)&iso_tree_matrix_inc[i][j],sizeof(iso_tree_matrix_inc[i][j]));
	      }
	      
	    }	 
	    iforest_bin.close();    
	    
	    iforest.open("iForest",ios::out); 
	    if(!iforest)
            {cout<<"Can't read file!!";exit(0);}
             
	    for(int i=0;i<iso_tree_matrix_inc.size();i++)
	    {
	      for(int j=0;j<iso_tree_matrix_inc[i].size();j++)
	      {
		 iforest<<iso_tree_matrix_inc[i][j]<<" ";
	      }
	      
	      iforest<<endl;
	    }
	    
	    iforest.close();	    
	    cout<<"Updated iForest written\n";
	    
	    
	    //Write updates Lowest node matrix   
	    ofstream LowestNode_xy, LowestNode_bin;
            LowestNode_xy.open("lowest_node",ios::out);
            LowestNode_bin.open("lowest_node.bin",ios::out|ios::binary);
            TreeNode temp;
            for(int i=0;i<Lowest_Node_inc.size();i++)
		   {
		       //cout<<"Display:"<<i<<endl;
		       for(int j=0;j<Lowest_Node_inc[i].size();j++)
		       {  
			  temp = Lowest_Node_inc[i][j];
			  
			  //Write to binary file
			  LowestNode_bin.write((char*)&temp,sizeof(temp)); 
				 
			  //Write to file
			  LowestNode_xy<<i<<" "<<j<<" ";          
			  //Display the array index of the lowest node from the in-memory stored iForest
			  for(int k=0;k<tree_num;k++)
			  {
			      LowestNode_xy<<Lowest_Node_inc[i][j].node_id[k]<<" ";
			  }
			  
			  LowestNode_xy<<"-1"<<endl;
			  
			  
			  
		       }
		       
		    }
   
		    LowestNode_bin.close();
		    LowestNode_xy.close();
		    
		    cout<<"Updated Lowest nodes written\n";
		    
		    
		    
		    
                    //Write updated Mass-matrix	
                    ofstream me_xy, mass_bin;
                    me_xy.open("mass_matrix",ios::out);
                    mass_bin.open("mass_matrix.bin",ios::out|ios::binary);	    
                    long double temp1;
                    
                    for(int i=0;i<mass_matrix_inc.size();i++)
		    {
		      //cout<<"Display:"<<i<<endl;
		       for(int j=0;j<mass_matrix_inc[i].size();j++)
		       {  
			  temp1 = mass_matrix_inc[i][j];
			  mass_bin.write((char*)&temp1,sizeof(temp1));        
			  me_xy<<i<<" "<<j<<" "<<mass_matrix_inc[i][j]<<endl;
		       }
		       
		       
		    }
		    
		     mass_bin.close();
                     me_xy.close();
                     cout<<"Updated Mass-matrix written\n";
                     
                     
                     
                 //Write Clusters to file
                 ofstream out_cluster_global("Cluster",ios::out);
                 if(!out_cluster_global)
		     {
		      cout<<"Cannot open the out_cluster file\n"<<endl;
		      exit(0);
		     }    
		     
		 int p; 
		 
		 for(p=0;p<range_matrix_inc.size();p++)
		 {
                   out_cluster_global<<p<<"]";
		   out_cluster_global<<cluster_inc[p].status<<"/";
		   out_cluster_global<<cluster_inc[p].rep<<"/";
		   out_cluster_global<<cluster_inc[p].core;
		   out_cluster_global<<endl;

		 }                      
                  
                 out_cluster_global.close();    
                 
                 
                 ofstream out_cluster_bin("Cluster.bin",ios::out|ios::binary);
                 for(p=0;p<range_matrix_inc.size();p++)
		 {
		   Cluster clust;
		   clust = cluster_inc[p];
		   
		   out_cluster_bin.write((char*)&clust,sizeof(clust));		   
                
		 }
		 
		 out_cluster_bin.close();
		 
               cout<<"Updated Clusters written\n";
          
              //Write Core + Non-core + Noise pts. to file
              ofstream core_pt("core",ios::out);
              ofstream core_pt_bin("core.bin",ios::out|ios::binary);
              
              ofstream non_core_pt("non-core",ios::out);
              ofstream non_core_pt_bin("non-core.bin",ios::out|ios::binary);
		 
	      ofstream noise_pt("noise",ios::out);
              ofstream noise_pt_bin("noise.bin",ios::out|ios::binary);	 
              
               for(int i=0;i<core_inc.size();i++)
                core_pt<<core_inc[i]<<endl;             
               for(int i=0;i<core_inc.size();i++)   
                core_pt_bin.write((char*)&core_inc[i],sizeof(core_inc[i])); 
                
                
              for(int i=0;i<non_core_inc.size();i++)
                non_core_pt<<non_core_inc[i]<<endl;
              for(int i=0;i<non_core_inc.size();i++)  
                 non_core_pt_bin.write((char*)&non_core_inc[i],sizeof(non_core_inc[i]));
                 
                 
              for(int i=0;i<noise_inc.size();i++)             
                noise_pt<<noise_inc[i]<<endl;
              for(int i=0;i<noise_inc.size();i++)  
                noise_pt_bin.write((char*)&noise_inc[i],sizeof(noise_inc[i]));
                
             core_pt.close();core_pt_bin.close();
             non_core_pt.close();non_core_pt_bin.close();
             noise_pt.close();noise_pt_bin.close();  
             
             cout<<"Updated Core,Non-Core,Noise points written\n";
             	
		 
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

 














