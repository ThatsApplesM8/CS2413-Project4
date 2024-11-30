#include <vector>
#include <iostream>
#include <exception>
using namespace std;
/***** Write your exception class here ******* SEE main function *****/

class duplicateInsertion : public exception {   // duplicatesInsertion class
    virtual const char* what() const throw() {
        return "Duplicate insertion exception";
    }
};
class NotFoundException : public exception {    // NotFoundException class
    virtual const char* what() const throw() {
        return "Not found exception";
    }
};

template <class DT>
class MTree {
    protected:
        int M;  // Maximum number of children per node (M+1 may split)
        vector<DT> values;  // Values stored in the node (M-1 values)
        vector<MTree*> children;    // Pointers to child MTrees (M+1 children)

    public:
        MTree(int M);
        ~MTree();
        bool is_leaf() const;   // checks if the current node is a leaf
        void insert(const DT& value);   // Inserts a value into the MTree
        void split_node();  // Splits the node if it exceeds capacity (i.e >=M)
        MTree* find_child(const DT& value); // Find the correct child to follow
        bool search(const DT& value);   // Search for a value in the MTree
        void remove(const DT& value);   // Build the tree
        void buildTree(const vector<DT>& input_values); // Build the tree
        vector<DT>& collect_values();   // Collect values from all leaf nodes
        bool find(DT& value);   

};

template <class DT>
MTree<DT>:: MTree(int M) {  // Constructor
    this->M = M;
}

template <class DT>
MTree<DT>:: ~MTree() {
    for(int i = 0; i < children.size(); i++) {  // delete all children
        delete children[i];
    }
}

template <class DT>
bool MTree<DT>:: is_leaf() const {
    return children.empty();    // returns if the the children has no values.
}

template <class DT>
void MTree<DT>:: insert(const DT& value) { 
    if(search(value)) { // Checks if there is already a value in the tree
        throw duplicateInsertion();
    }

    if(is_leaf()) {     // if the current node = leaf, attempt inserting into array

        if(values.size() < M - 1) {  // check if theres any capacity for value to be put.

            values.push_back(value);    
            for(size_t i = 1; i < values.size(); i++) { // insertion sort   
                DT key = values[i]; 
                int j = i - 1;

                
                while(j >= 0 && values[j] > key) {          
                    values[j + 1] = values[j];
                    j = j - 1;
                }
                values[j + 1] = key;
            }

        }
        else {  // node itself has full capacity => split
            split_node();
            find_child(value)->insert(value);
            
        }
    } else {    // node is a non-leaf so find correct child to do insertion
        find_child(value)->insert(value);
    }

}

template <class DT>
void MTree<DT>:: split_node() {
    int mid = values.size() / 2; // getting a mid value to split nodes.
    DT midValue = values[mid];

    MTree* newNode = new MTree(M);  

    newNode->values.assign(values.begin() + mid + 1, values.end()); // copying values from mid to end to new node
    values.resize(mid); // resizing the current node to mid

    if(!is_leaf()) {    // if the current node is not a leaf, copy children to new node
        newNode-> children.assign(children.begin() + mid + 1, children.end());
        children.resize(mid+1);
    }
}
template <class DT>
MTree<DT>* MTree<DT>:: find_child(const DT& value) {
    for(int i = 0; i < values.size(); i++) {    // iterate through the values to find the correct child
        if(value <= values[i]) {
            return children[i];
        }
    }
    return children[values.size()];     // returns the node if the value is greater than all in the current node.
}

template <class DT> 
bool MTree<DT> :: search(const DT& value) {  // search function
    if(is_leaf()) {     // checks if the value is a leaf
        for(int i = 0; i < values.size(); i++) {
            if(values[i] == value) {
                return true;
            }
        }
    } else {
        return find_child(value)->search(value);    // if the value is not a leaf, find the child and search for the value
    }
    return false;
}

template <class DT>
void MTree<DT> :: remove(const DT& value) {   // remove function
    if(!search(value)) {    // if the value is not found, throw exception
        throw NotFoundException();
    }

    if(is_leaf()) { // if the current node is a leaf, remove the value
        for(int i = 0; i < values.size(); i++) {    
            if(values[i] == value) {
                values.erase(values.begin() + i);
                return;
            }
        }
    } else {    // if the current node is not a leaf, find the child and remove the value
        MTree* child = find_child(value);
        child->remove(value);
    }

    vector<DT> all_values = collect_values();   // collect all values from the leaf nodes
    buildTree(all_values);  
    
}

template <class DT>
void MTree<DT>::buildTree(const vector<DT>& input_values) {

    // we must clear the tree before rebuilding it; ensures no memory allocation errors.
    values.clear();
    for(int i = 0; i < children.size(); i++) {
        delete children[i];
    }
    children.clear();

    // code that TA gave us.
    if (input_values.size() <= M - 1) {
        values = input_values;
    } else {
        int D = input_values.size() / M;
        for (int i = 0; i < M; i++) {
            int start = D * i;
            //cout << "start: " << start << " - ";
            int end;
            if (i == M - 1) {
                end = input_values.size() - 1;
                //cout << "end: " << end << endl;
            } else {
                end = start + D - 1;
                //cout << "end: " << end << endl;
                values.push_back(input_values[end]);
            }
            vector<DT> child_values(input_values.begin() + start, input_values.begin() + end + 1);
            MTree<DT>* child = new MTree<DT>(M);
            child->buildTree(child_values);
            children.push_back(child);
        }
    }
}

template <class DT> 
bool MTree<DT> :: find(DT& value) { 
    return search(value);   // returns the search function

}

template <class DT>
vector<DT>& MTree<DT>::collect_values() {   // Collects values from all leaf nodes
    vector<DT>* myValues = new vector<DT>();
    if(is_leaf()) {
        for( int i = 0; i < values.size(); i++) {
            myValues->push_back(values[i]);
        }
    }
    else {
        for(auto child : children) {    // iterate through the children to collect values
            vector<DT> child_values = child->collect_values(); 
            myValues->insert(myValues->end(), child_values.begin(), child_values.end());
        }
    }
    return *myValues;
}


int main() {
    int n; // number of numbers in the inital sorted array
    int MValue;
    int numCommands;
    char command;
    int value;
    
    // read n numbers from the input and add them to the vector mySortedValues
    cin >> n;
    vector<int> mySortedValues(n);

    // store each input into vector
    for( int i = 0; i < n; i++) {
        cin >> mySortedValues[i];
        // cout << mySortedValues[i] << " ";
    }

    // Get the M value
    cin >> MValue;
    MTree<int>* myTree = new MTree<int>(MValue);

    // Build the tree
    myTree->buildTree(mySortedValues);


    cin >> numCommands; // Read the number of commands

    //************** Read each command Process ***************//

    for( int i = 0; i < numCommands; i++) {
        cin >> command; // Read the command type

        switch(command) {
            case 'I': { // Insert
                cin >> value;
                try {
                    myTree-> insert(value);
                    cout << "The value = " << value << " has been inserted." << endl;

                } catch (duplicateInsertion& e) {
                    cout << "The value = " << value << " already in the tree." << endl;
                }
                break;
            }
            
            case 'R': { // Remove 
                cin >> value;
                try {
                    myTree->remove(value);
                    cout << "The value = " << value << " has been removed." << endl;
                }
                catch (NotFoundException& e) {
                    cout << "The value = " << value << " not found." << endl;
                }
                break;
            }

            case 'F': { // Find
                cin >> value;
                if ((myTree->find(value))) {
                    cout << "The element with value = " << value << " was found." << endl;

                } else {
                    cout << "The element with value = " << value << " not found." << endl;
                }
                break;
            }

            case 'B': { // rebuild tree
                vector<int> myValues = myTree->collect_values();
                myTree->buildTree(myValues);
                cout << "The tree has been rebuilt." << endl;
                break;
            }
            default: 
                cout << "Invalid command" << endl;
        }
    }

    cout << "Final list: ";
    vector<int> allValuesObt = myTree->collect_values();
    for(int i = 0; i < allValuesObt.size(); i++) {
        cout << allValuesObt[i] << " ";
        if(i % 20 == 19 && i != 0) {
            cout << endl;
        }
    }
    cout << endl;
    delete myTree;
    return 0;
}


/*  LLM and GitHub CoPilot Documentation

    Prompt: "Generate an insertion sort to insert values into the MTree."
    Rationale: Insertion sort was documented aiming to sort the values in the MTree. It was implemented in the insert function to manually sort the values of the node to be inserted. I had difficulties using 
    some sorting method to be implemented to insert a value into the supposed node. Having the prompt to generate an insertion sort, I was able to implement the insertion sort to sort the values in the node.
    Incremental Development: The insertion sort implemented in the insert ensured me that the values are checked and neatly dorted with the corresponding order everytime the insert function was called. 
    The prompt helped me set up the insertion sort function to correlate and readjust the M-Tree after insertion to ensure the values are sorted in the node properly before continuing with additional processes.

    Prompt: "Describe the reason behind the infinite terminal output when the program is run."
    Rationale: The prompt was documented to explain the reason behind the infinite terminal output when the program is run. I believed that the infinite terminal output was caused by inefficient memory allocations from how my code 
    was organizing the values of the M-Tree. I had a feeling that maybe it was potentially caused by the given buildTree function given to us by the TA. The prompt lead to another prompt that I've asked to dementrate the output after 
    the buildTree function was called with the new segemented code that CoPilot has suggested me.
    Incremental Development: The prompt helped me understand the reason behind the infinite terminal output when the program is run. I was able to understand that the infinite terminal output was caused by the buildTree function, and
    tested the fixed code that Copilot had suggested me to see if the infinite terminal output was fixed. The prompt helped me understand the reason behind the infinite terminal output and how to fix it.

    Prompt: "Generate a body of code that will allow the MTree to be cleared then rebuilt."
    Rationale: The prompt was documented to generate a body of code that will allow the MTree to be cleared then rebuilt. I had a feeling that the MTree was not being cleared properly before being rebuilt, causing the infinite terminal output.
    The code that our TA gave us may had been one of the problems with how each input was stored. I decided that to clear the MTree before rebuilding it would be the best solution to fix the infinite terminal output.
    Incremental Development: The prompt helped me generate a body of code that will allow the MTree to be cleared then rebuilt. I was able to clear the MTree before rebuilding it to ensure that the MTree was properly 
    cleared before being rebuilt to ensure that all nodes are properly stored, continuing with the additional processes of the program.

    Prompt: "Provide a suggestion for the MTree collect_values function to collect values from all leaf nodes."
    Rationale: The prompt documented above was to provide a suggestion that I can use to properly collect the values from all the leaf nodes. I noticed that the collect_values() function isn't collecting the values properly, 
    as I seen from the terminal output that multiple values are being repeated, given me the assumption that the values are not being collected correctly and being stroed into the wrong nodes.
    CoPilot suggested me to initate a vector pointer to store the values of the leaf nodes, and iterate through the children to collect the values from the leaf nodes, which in turn, successfully collected the values from the leaf nodes.
    Incremental Development: The prompt helped me provide a suggestion for the MTree collect_values function to collect values from all leaf nodes. I was
    able to properly collect the values from all the leaf nodes by initiating a vector pointer to store the values of the leaf nodes, and iterate through the children to collect the values from the leaf nodes. 
    The successful collection of th evalues from the leaf nodes ensured that the values were properly stored in the correct nodes, enabling me to continue onwards with other changes to the program.


*/

/* Specific Tests/Debugging
    The project programming has been running smoothly with the given test cases. The program has been able to insert, remove, find, and rebuild the tree with the given values, through changes into the MTree class formatting and additional function paragraphs.
    The program has been able to insert values into the MTree, though significant changes into the later function had caused major issues into the insert itself. Some test cases runned showed additional values (repeated another time) showed into the terminal output for the insert inputs.
    I was able to fix the issure through the insertion sort, and properly checked the the runned case program to ensure that the values were properly sorted and inserted into the MTree.

    The program has been able to remove values from the MTree, though the remove function had caused some issues with the values not being removed properly. Through through some debugging, I was able to fix the issue 
    through the iterations of the values and nodes. With the fix, I ran the test cases to ensure that the values were properly removed from the MTree, and the terminal output showed that the values were properly removed from the MTree.

    The find function was properly implemented into the MTree class, and was able to find the values in the MTree through the search function.

    The rebuild function was properly implemented into the MTree class, and was able to rebuild the MTree with the collected values from the leaf nodes through major changes into the code by the infinite temrinal output. 
    Through the debugging stages after fixing the issue, I retested the cases, and showed that the MTree was properly rebuilt with the collected values from the leaf nodes.

    The collect_values function was properly implemented into the MTree class, and was able to collect the values from the leaf nodes through the changes into the code. With the changes, 
    I retested and was shown successfuly to collect the values from the leaf nodes, and ensured that the values were properly stored in the correct nodes.

*/