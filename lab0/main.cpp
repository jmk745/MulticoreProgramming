#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include "set.h"

using namespace std;

//predefined method for testing the class and its methods
bool testRun(int a, bool b=false);


int main(){

  //input random seed and define number of test runs
  int numOfTests = 10;
  srand(time(NULL));


  //get user input for whether they want full result output or condensed..
  int userInput;
  printf("Select full or condensered results print-out..\n'0' for condensed, any other key for full report...\n");
  scanf("%d", &userInput);


  //keep count of all successful tests of the DS
  int successCount = 0;
  for(int i=0; i<numOfTests; i++){
    //Run test and if god run, then increment
    if(testRun(200, userInput)){
      successCount++;
    };
  }
  printf("%d out of %d test runs were succesful...\n\n", successCount, numOfTests);
  return 0;
}



/* The following method takes in an integer 'numOfElements' that instructs the
* number of numbers to use for the following. Upon completion, the method returns an output
* of type boolean (TRUE for no missing elements in Set DS and FALSE otherwise);
*/
bool testRun(int numOfElements, bool toPrint){

  // responsible for the formatted column output of the results
  int columnGap = 13;
  //create instance of Set that will undergo testing.
  Set<int> workSet;
  // instantiate a std::list to record all elements that were entered into the Set DS.
  list<int> controlGroup;

  //Generate random numbers and insert them into the Set DS and our control list;
  int generatedNumber;
  for(int i=0; i<numOfElements; i++){
    generatedNumber = rand() % 200 + 1;
    workSet.insert(generatedNumber);
    controlGroup.push_front(generatedNumber);
  }


  //Make the list unique!
  //Since .unique() only works on sorted lists, controlGroup ust undergo sorting.
  controlGroup.sort();
  controlGroup.unique();

  if(toPrint){
    printf(" %*s %*s %*s \n", columnGap, "Element", columnGap, "Found Status", columnGap, "Delete Status");
  }

  //iterate through the control list (with the help of the std:list iterator)
  // and verify whether all unique generated elements have been entered
  // only once

  // boolean variable records whether there are any missing elements in testSet
  bool elemIsMissing = false;
  for( list<int>::iterator it=controlGroup.begin(); it != controlGroup.end(); it++){

      //finding element and then deleting it
      bool isFound = workSet.find(*it);
      bool deleteStatus = workSet.remove(*it);
      if( isFound == false ){
        elemIsMissing = true; //mark that there is at least 1 missing element in the Set DS
      }

      if(toPrint){
        printf(" %*d %*d %*d \n", columnGap, *it, columnGap, isFound, columnGap, deleteStatus);
      }
  }


  //Print the overall results of the test Run and return the boolean of the result
  printf("------------------------------------------------\n");
  printf("Results\n");
  printf("------------------------------------------------\n");
  printf("1=true and 0=false\n\n");
  printf("Set is Empty: %d\n", workSet.isEmpty());
  printf("Were there any missing elements: %d\n", elemIsMissing);
  printf("Test Run was Succesful: %d\n\n\n", workSet.isEmpty() && !elemIsMissing);
  return !elemIsMissing;
}
