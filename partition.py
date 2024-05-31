#########################################
#Description: Subsample the Given Data
#Author: Sandeep Vidyapu
#Date: 03.01.2022
#Project: BiMASS
#Co-Author(s): Panthadeep Bhattacharjee
########################################
import pandas as pd
import random, os, shutil

datafile = "g2-16-20"
nsubsamples = 20
baseDatasetSize = 1350 #0: process whole data, else uses the given number as the data points to be subsampled
subsamplesDir = "subsamples" #Directory to store the subsamples


def prependDataDimensions(subsamplesDir=subsamplesDir):
    if(subsamplesDir is None):
        print("You must enter the name of the directory that contains all the subsampled data files")
        return

    os.chdir(subsamplesDir)
    for datafile in os.listdir('.'):
        print("Prepending data dimensions to the file", datafile, "...")
        data = pd.read_csv(datafile, delim_whitespace=True, header=None)
        line = str(data.shape[0]) + " " + str(data.shape[1])
        with open(datafile, "r+") as ssfile:
            storedData = ssfile.read()
            ssfile.seek(0, 0)
            ssfile.write(line + '\n' + storedData)


def subsampling(data, nsubsamples=None):
    print("Given Data:\n", data)
    print("Data Dimensions: ", data.shape)
    nrows = data.shape[0]
    ncols = data.shape[1]

    if(baseDatasetSize!=0):
        data = data[0:baseDatasetSize]
        print("\nSelecting the base data of size", baseDatasetSize, "from given dataset")
        print("Base Data:\n", data)
        print("Base Data Dimensions: ", data.shape)
        nrows = data.shape[0]
        ncols = data.shape[1]


    if(nsubsamples is None):
        print("You must enter an Integer number of subsamples")
        return
    print("Desired number of subsamples: ", nsubsamples)

    dataPerSubsample = nrows//nsubsamples
    rowsToRandomize = dataPerSubsample*nsubsamples
    if(nrows > rowsToRandomize):
        print(str(nrows-rowsToRandomize), "subsamples contain", dataPerSubsample+1, "data points")
    print(str(nsubsamples - (nrows-rowsToRandomize)), "subsamples contain", dataPerSubsample, "data points")

    print("Creating", subsamplesDir, "directory to store the subsamples")
    if(os.path.isdir(subsamplesDir)):
        shutil.rmtree(subsamplesDir) #removes the existing directory and inside files
    os.mkdir(subsamplesDir)

    rand_indices = random.sample(range(rowsToRandomize), rowsToRandomize)

    firstEffectedFile = nsubsamples-(nrows-rowsToRandomize) #Starting from this file (index), all the above files contain one data point extra
    print("\nFrom", firstEffectedFile+1, "all the files will have one extra data point")
    for i in range(nsubsamples):
        subsampleFile = subsamplesDir+"/"+str(i+1)
        print("\n\n\nSubsampling data for file", subsampleFile, "...")
        subsampledData = data.iloc[rand_indices[i*dataPerSubsample:(i+1)*dataPerSubsample]]
        if(i >= firstEffectedFile):
            subsampledData = subsampledData.append(data.iloc[[nrows+firstEffectedFile-i-1]])
            print("Appending the following data point to the subsample file:", subsampleFile)
            print(data.iloc[[nrows+firstEffectedFile-i-1]])

        print("The Subsampled Data:\n", subsampledData)
        print("Writing subsamples to file", subsampleFile, "...")
        subsampledData.to_csv(subsampleFile, index=False, header=False, sep=" ")


    print("========== SUBSAMPLING COMPLETED ===========")
    prependDataDimensions()




data = pd.read_csv(datafile, delim_whitespace=True, header=None)
subsampling(data, nsubsamples)
