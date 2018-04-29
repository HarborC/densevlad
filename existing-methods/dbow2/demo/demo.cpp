/**
 * File: demo.cpp
 * Date: April 2018
 * Author: Abdul Mohsi Jawaid
 * Description: Evaluation of DBoW2 for place recognition with our datasets
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <fstream>

// DBoW2
#include "DBoW2.h" // defines OrbVocabulary and OrbDatabase

#include <DUtils/DUtils.h>
#include <DVision/DVision.h>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>


using namespace DBoW2;
using namespace DUtils;
using namespace std;

typedef vector<string> stringvec;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void loadFeatures(string name, vector<vector<cv::Mat > > &features, vector<string> &labels);
void testNewDatabase(const vector<vector<cv::Mat > > &trainFeatures, const vector<vector<cv::Mat > > &testFeatures, vector<string> &trainLabels, vector<string> &testLabels);
void testTrainedDatabase(const vector<vector<cv::Mat > > &testFeatures, vector<string> &trainLabels, vector<string> &testLabels);
void saveLabels(vector<string> &trainLabels);
void loadLabels(vector<string> &trainLabels);

void changeStructure(const cv::Mat &plain, vector<cv::Mat> &out);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

// number of training images
const int NTRAIN = 1400;

// number of testing images
const int NTEST = 350;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void wait()
{
  cout << endl << "Press enter to continue" << endl;
  getchar();
}

// ----------------------------------------------------------------------------

/* files are named as <LABEL>_<NUMBER> */
string getLabel(string filename)
{
  string label = "";
  for(int i = 0; i < int(filename.length()); i++)
  {
    if(filename[i] != '_')
    {
      label += filename[i];
    }
    else
    {
      break;
    }
  }
  return label;
}

// ----------------------------------------------------------------------------

void saveLabels(vector<string> &trainLabels){
  cout << "Saving labels..." << endl;
  fstream f;
  f.open("labels",fstream::out);
  for(size_t i = 0; i < trainLabels.size(); i++)
  {
    f << trainLabels[i] << endl;
  }
  f.close();
}

// ----------------------------------------------------------------------------

void loadLabels(vector<string> &trainLabels)
{
  trainLabels.clear();
  trainLabels.reserve(NTRAIN);
  fstream f;
  f.open("labels",fstream::in);
  string label = "";
  for(int i = 0; i < 1400; i++)
  {
    getline(f,label);
    trainLabels.push_back(label);
  }
}

// ----------------------------------------------------------------------------

void printVector(vector<double> &v)
{
  cout << '[';
  for(size_t i = 0; i < v.size(); i++)
  {
    if(i!=v.size()-1)
    {
      cout << v[i] << ',';
    }
    else
    {
      cout << v[i];
    }
  }
  cout << ']' << endl;
}

// ----------------------------------------------------------------------------

// expects "db.yml.gz" and "labels" files in build directory
void evaluateExistingTrainingSet(string &dataset)
{
  vector<vector<cv::Mat > > featurestest; //testing images
  vector<string> labelstrain; // training image labels
  vector<string> labelstest; // testing image labels

  // load training labels from file
  loadLabels(labelstrain);

  // load testing features and labels
  loadFeatures("images/test/" + dataset + "/resized", featurestest, labelstest);

  testTrainedDatabase(featurestest, labelstrain, labelstest);
}

// ----------------------------------------------------------------------------

void evaluateNewTrainingSet(string &dataset)
{
  vector<vector<cv::Mat > > featurestrain; //training images
  vector<vector<cv::Mat > > featurestest; //testing images
  vector<string> labelstrain; // training image labels
  vector<string> labelstest; // testing image labels

  // load training features and labels
  loadFeatures("images/train",featurestrain,labelstrain);

  // load testing features and labels
  loadFeatures("images/test/" + dataset + "/resized", featurestest, labelstest);

  testNewDatabase(featurestrain, featurestest, labelstrain, labelstest);
}

// ----------------------------------------------------------------------------

int main(int argc, char** argv)
{
  string dataset = argv[1];
  evaluateExistingTrainingSet(dataset);
  wait();

  return 0;
}

// ----------------------------------------------------------------------------

void loadFeatures(string name, vector<vector<cv::Mat > > &features, vector<string> &labels)
{
  features.clear();
  features.reserve(NTRAIN);
  labels.clear();
  labels.reserve(NTRAIN);

  // ORB-SLAM extracts 1000 features for mono kitti example that we used for GTA V
  cv::Ptr<cv::ORB> orb = cv::ORB::create(1000);

  cout << "Extracting ORB features..." << endl;

  // get all images in directory and get their features
  DIR* dirp = opendir(name.c_str());
  struct dirent * dp;
  while((dp = readdir(dirp)) != NULL) 
  {
    if(strcmp(dp->d_name,".")!=0 && strcmp(dp->d_name,"..")!=0)
    {
      stringstream ss;
      ss<<name<<"/"<<dp->d_name;
      cout<<"file is:"<<ss.str()<<endl;
      cv::Mat image = cv::imread(ss.str(), 0);
      cv::Mat mask;
      vector<cv::KeyPoint> keypoints;
      cv::Mat descriptors;

      orb->detectAndCompute(image, mask, keypoints, descriptors);

      labels.push_back(getLabel(dp->d_name));
      features.push_back(vector<cv::Mat >());
      changeStructure(descriptors, features.back());
    }
  }
  closedir(dirp);
}

// ----------------------------------------------------------------------------

void queryDatabase(OrbDatabase &db, const vector<vector<cv::Mat > > &testFeatures, vector<string> &trainLabels, vector<string> &testLabels, int K)
{
  int relevantImages = 0; // number of images in results that have the expected label of query
  int currRecall = 0; // recall per query

  vector<double> recalls(K);
  vector<double> precisions(K);

  QueryResults ret;

  // initialize vectors
  for(int i = 0; i < K; i++)
  {
    recalls[i] = 0;
    precisions[i] = 0;
  }

  // collect precision/recall data
  for(int i = 0; i < NTEST; i++)
  {
    db.query(testFeatures[i],ret,K);
    
    currRecall = 0;
    relevantImages = 0;
    for(int j = 0; j < K; j++)
    {
      if(testLabels[i]==trainLabels[ret[j].Id])
      {
        relevantImages++;
        if(currRecall==0)
        {
          currRecall++;
        }
      }
      recalls[j] += currRecall;
      precisions[j] += (double(relevantImages))/double(j+1);
    }
  }
  for(int i = 0; i < K; i++)
  {
    recalls[i] /= NTEST;
    precisions[i] /= NTEST;
  }

  cout << endl;

  cout << "recalls=";
  printVector(recalls);
  cout << "precisions=";
  printVector(precisions);

  cout << endl;
}

// ----------------------------------------------------------------------------

void testNewDatabase(const vector<vector<cv::Mat > > &trainFeatures, const vector<vector<cv::Mat > > &testFeatures, vector<string> &trainLabels, vector<string> &testLabels)
{
  
  cout << "Creating trained database..." << endl;

  // load the vocabulary from disk
  OrbVocabulary  voc;

  voc.loadFromTextFile("ORBvoc.txt");
  
  OrbDatabase db(voc, false, 0); // false = do not use direct index
  // (so ignore the last param)
  // The direct index is useful if we want to retrieve the features that 
  // belong to some vocabulary node.
  // db creates a copy of the vocabulary, we may get rid of "voc" now

  // add images to the database
  for(int i = 0; i < NTRAIN; i++)
  {
    db.add(trainFeatures[i]);
  }

  cout << "... done!" << endl;

  cout << "Database information: " << endl << db << endl;

  cout << "Querying the database: " << endl;

  // query and print precision/recall with top 50 results
  queryDatabase(db, testFeatures, trainLabels, testLabels, 50); 

  cout << "Saving database..." << endl;
  db.save("db.yml.gz");
  cout << "... done!" << endl;

  saveLabels(trainLabels);
}

// ----------------------------------------------------------------------------

void testTrainedDatabase(const vector<vector<cv::Mat > > &testFeatures, vector<string> &trainLabels, vector<string> &testLabels)
{

  // load the database from disk

  cout << "Loading database..." << endl;
  
  OrbDatabase db("db.yml.gz");

  cout << "... done!" << endl;

  cout << "Database information: " << endl << db << endl;

  cout << "Querying the database: " << endl;

  // query and print precision/recall with top 50 results
  queryDatabase(db, testFeatures, trainLabels, testLabels, 50); 

}

// ----------------------------------------------------------------------------

void changeStructure(const cv::Mat &plain, vector<cv::Mat> &out)
{
  out.resize(plain.rows);

  for(int i = 0; i < plain.rows; ++i)
  {
    out[i] = plain.row(i);
  }
}

