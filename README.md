Steps to run MBSCAN: (MBSCAN folder)

1. Create Subsample <partition.cpp> file is there. Re run it using a bash script for converting the subsamples into binary.

2. Create iTree from each subsample, say 't' iTrees from 't' subsamples. Code: <itree.cpp>, run iteratively using a bash script <iso-tree.sh>.

3. Run <mbc.cpp> to load iTrees into memory, compute the mass matrix and proceed towards clustering.

4. Copy the contents of 'mbscan' folder into 'BiMass' folder.

BiMass:(BiMass folder)
1. Code: <BiMass.cpp> When new batch of points enters, fetch each iTree file and see what are its appropriate positioning nodes within that iTree. Update mass matrix according to the new formula derived.

2. For creating the batches, run the code <updates.cpp>. All the folders of different batch sizes will appear.

3. Compute clusters incrementally. Use a bash script to insert all the batch of points.
