#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <utility>
#include <stack>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

const int MEMORY_SIZE = 16 * 1024*1024 ; // 16 MB
const int FILE_SIZE = 256 * 1024*1024 ; // 256 MB
const int INPUT_BUFFER_SIZE = 4 * 1024*1024 ; // Input buffer
const int OUTPUT_BUFFER_SIZE = 1 * 1024*1024 ; // two output buffers
const int NUM_RANGE = 1000000;
const int NUM_COUNT = FILE_SIZE / sizeof(int); // Number of integers to write
const int NUM_LIMIT_INPUT_BUFFER = INPUT_BUFFER_SIZE / sizeof(int);
const int NUM_LIMIT_OUTPUT_BUFFER = OUTPUT_BUFFER_SIZE / sizeof(int);

bool fileExists(const string& filename);

struct TreeNode {
    string sorted_file;
    string small_numbers_file;
    string large_numbers_file;
    TreeNode* left;
    TreeNode* right;

    // Constructor
    TreeNode(const string &sorted, const string &small, const string &large)
        : sorted_file(sorted), small_numbers_file(small), large_numbers_file(large), left(nullptr), right(nullptr) {}

    // Destructor to recursively delete the tree
    ~TreeNode() {

        if(fileExists(sorted_file)) remove(sorted_file.c_str());
        if(fileExists(small_numbers_file)) remove(small_numbers_file.c_str());
        if(fileExists(large_numbers_file)) remove(large_numbers_file.c_str());

        delete left;  // Recursively delete the left subtree
        delete right; // Recursively delete the right subtree
    }
};


bool readBinaryFile(const string& filename, vector<int>& data);
void writeBufferToFile(ofstream &file, vector<int> &buffer);
bool middleGroupSort(const string &inputFile, const string &outputFile, const string &smallNumbersFile, const string &largeNumbersFile);
void generateRandomNumbers(const string &filename);
void externalSort(const string &inputFile, const string &outputFile);
TreeNode* createSortedFilesTree(const string &inputFile, int &t);
void sortedFilesMerge(TreeNode* root,  ofstream &out);

class IntervalHeap {
private:
    vector<pair<int, int>> heap;

    void min_swap_elements(int i, int j) {
        int temp = heap[i].first;
        heap[i].first = heap[j].first;
        heap[j].first = temp;
    };

    void max_swap_elements(int i, int j) {
        int temp = heap[i].second;
        heap[i].second = heap[j].second;
        heap[j].second = temp;
    };

    void swap_elements_in_pair(int i) {
        int temp = heap[i].first;
        heap[i].first = heap[i].second;
        heap[i].second = temp;
    }

    void swap_parent_right_and_child_left(int i){
        int parent = i/2-1;
        int temp = heap[parent].second;
        heap[parent].second = heap[i-1].first;
        heap[i-1].first = temp;

    };

    void min_heapify(int index) {
        int n = heap.size();
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        while (true) {

            if (right < n  && heap[right].first < heap[smallest].first) {
                smallest = right;
            }

            if (left < n && heap[left].first < heap[smallest].first) {
                smallest = left;
            }


            if (smallest != index) {
                min_swap_elements(index, smallest);

                if(heap[smallest].first > heap[smallest].second){
                    swap_elements_in_pair(smallest);
                }
                index = smallest;
                left = 2 * index + 1;
                right = 2 * index + 2;
            } else {
                return;
            }
        }
    }

    void max_heapify(int index) {

        int n = heap.size();
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        while (true) {

            if (right < n && heap[right].second != INT_MAX && heap[right].second > heap[largest].second) {
                largest = right;
            }

            if (left < n && heap[left].second != INT_MAX && heap[left].second > heap[largest].second) {
                largest = left;
            }

            if (largest != index) {
                max_swap_elements(index, largest);

                if(heap[largest].first > heap[largest].second){
                    swap_elements_in_pair(largest);
                }
                index = largest;
                left = 2 * index + 1;
                right = 2 * index + 2;
            } else {
                return;
            }
        }
    }

public:
    IntervalHeap() {};

    void insert(int val) {
        if (heap.empty()){
            heap.push_back({val, INT_MAX});
        }else if(heap.back().second != INT_MAX){
            heap.push_back({val, INT_MAX});

            if(heap[heap.size()-1].first > heap[heap.size()/2-1].second){
                swap_parent_right_and_child_left(heap.size());

                for(int i=heap.size()/ 2 -1;i>=0;i=(i-1)/2){

                    if(((i-1)/2)>=0 && heap[i].second > heap[(i-1)/2].second){
                        max_swap_elements(i,(i-1)/2);
                    }else{
                        break;
                    }

                };
            }else{
                for(int i=heap.size()-1;i>=0;i=(i-1)/2){

                    if(((i-1)/2)>=0 && heap[i].first < heap[(i-1)/2].first){
                        min_swap_elements(i,(i-1)/2);
                    }else{
                        break;
                    }

                };
            }


        }else {
            if (val < heap.back().first) {
                heap.back().second = heap.back().first;
                heap.back().first = val;

                for(int i=heap.size()-1;i>=0;i=(i-1)/2){

                    if(((i-1)/2)>=0 && heap[i].first < heap[(i-1)/2].first){
                        min_swap_elements(i,(i-1)/2);
                    }else{
                        break;
                    }
                };


            } else {
                heap.back().second = val;

                for(int i=heap.size()-1;i>=0;i=(i-1)/2){

                    if(((i-1)/2)>=0 && heap[i].second > heap[(i-1)/2].second){
                        max_swap_elements(i,(i-1)/2);
                    }else{
                        break;
                    }
                };
            }
        }
    };

    int get_min() {
        if (heap.empty()) {
            throw out_of_range("Heap is empty");
        };

        int last = heap.size() - 1;

        if(last==0){
            if(heap[0].first != INT_MIN){
                return heap[0].first;
            }
            return heap[0].second;


        }
        return heap.front().first;
    }

    int get_max() {
        if (heap.empty()) {
            throw out_of_range("Heap is empty");
        };

        int last = heap.size() - 1;

        if(last==0){
            if(heap[last].second != INT_MAX){
                return heap[last].second;

            };
            return heap[last].first;
        };
        return heap.front().second;
    }

    int remove_min() {
        if (heap.empty()) {
            throw out_of_range("Heap is empty");
        }

        int last = heap.size() - 1;
        if(last==0){
            if(heap[0].first != INT_MIN){
                int a = heap[0].first;
                if (heap[0].second == INT_MAX){
                    heap.pop_back();
                    return a;
                }
                heap[0].first = INT_MIN;
                return a;
            }

            int b = heap[0].second;
            heap.pop_back();
            return b;


        }

        int min_num = heap[0].first;

        min_swap_elements(0, last);

        if(heap[last].second == INT_MAX){
            heap.pop_back();
        }else{
            heap[last].first = heap[last].second;
            heap[last].second = INT_MAX;
        }

        min_heapify(0);

        return min_num;
    }

    int remove_max() {
        if (heap.empty()) {
            throw out_of_range("Heap is empty");
        }

        int last = heap.size() - 1;

        if(last==0){
            if(heap[last].second != INT_MAX){
                int a = heap[last].second;
                if(heap[last].first == INT_MIN){
                    heap.pop_back();
                    return a;
                }
                heap[last].second = INT_MAX;
                return a;

            };

            int b = heap[last].first;
            heap.pop_back();
            return b;

        };

        int max_num = heap[0].second;

        if(heap[last].second != INT_MAX){
            max_swap_elements(0, last);
            heap[last].second = INT_MAX;

        }else{
            if(last%2==1){
                max_swap_elements(0, (last-1)/2);
                heap[(last-1)/2].second = heap[last].first;
                heap.pop_back();
            }else{
                max_swap_elements(0, last-1);
                heap[last-1].second = heap[last].first;
                heap.pop_back();

                if(heap[last-1].first>heap[last-1].second){
                    swap_elements_in_pair(last-1);
                }
            }

        }

        max_heapify(0);


        return max_num;
    }

    bool is_empty() {
        return heap.empty();
    };

    int heap_size(){
        return heap.size();
    };

    void print_heap() {
        for (const auto& pair : heap) {
            cout << pair.first << " "<<pair.second<<" ";
        }
        cout << endl;
    }
};


int main() {

    string inputFile = "random_numbers.bin";
    string outputFile = "sorted_numbers.bin";

    generateRandomNumbers(inputFile);

    externalSort(inputFile, outputFile);

    vector<int> num1;
    vector<int> num2;

    readBinaryFile(inputFile,num1);
    readBinaryFile(outputFile,num2);
    cout<<"generated: "<<num1.size()<<endl;
    num1.clear();

    cout<<"sorted: "<<num2.size()<<endl;

    int b = INT_MIN;
    bool f = true;
    cout<< num2[0]<<" "<< num2[num2.size()-1]<<endl;
    for(int i=0;i<num2.size();i++){
        if(num2[i]<b){
            cout<<"something went wrong"<<endl;
            cout<< "index: "<<i<<"  "<<num2[i]<< " < "<<b<<endl;
            f=false;
            break;
        }
        b=num2[i];

    };
    if(f){
        cout<<"successYo"<<endl;
    }
    num2.clear();

    cout<<"No errors bro"<<endl;

    return 0;
};


TreeNode* createSortedFilesTree(const string &inputFile, int &t) {

    string sortedFile = "s" + to_string(t++) + ".bin";
    string smallFile = "0" + sortedFile;
    string largeFile = "1" + sortedFile;


    bool status = middleGroupSort(inputFile, sortedFile, smallFile, largeFile);

    if (!status) return nullptr;

    // Create a new tree node
    TreeNode* node = new TreeNode(sortedFile, smallFile, largeFile);

    // Recursively create left and right subtrees
    node->left = createSortedFilesTree(node->small_numbers_file, t);
    node->right = createSortedFilesTree(node->large_numbers_file, t);

    return node;
}

void sortedFilesMerge(TreeNode* root,  ofstream &out) {
    if (root == nullptr) {
        return;
    }

    // Traverse the left subtree
    sortedFilesMerge(root->left,out);

    vector<int> temp_buffer;
    bool file_read_status= false;


    file_read_status = readBinaryFile(root->sorted_file,temp_buffer);
    if(file_read_status){
        writeBufferToFile(out, temp_buffer);
    }else{
        cout<<"something went wrong bro"<<endl;
    };

    // Traverse the right subtree
    sortedFilesMerge(root->right,out);
}


void externalSort(const string &inputFile, const string &outputFile){

    cout << "Sorting..." << endl;

    // Start time measurement
    auto start = high_resolution_clock::now();

    int t = 0;

    // Create Sorted Files Tree
    TreeNode* root = createSortedFilesTree(inputFile, t);

    ofstream out(outputFile, ios::binary);

    //Inorder traversal on Sorted Files Tree
    sortedFilesMerge(root,out);

    // End time measurement
    auto end = high_resolution_clock::now();

    // Calculate duration
    auto duration = duration_cast<microseconds>(end - start);
    auto seconds = duration_cast<std::chrono::seconds>(duration).count();
    auto minutes = duration_cast<std::chrono::minutes>(duration).count();
    auto hours = duration_cast<std::chrono::hours>(duration).count();

    // Print the duration
    cout << "Sorting completed in "
         << hours << " hours "
         << (minutes % 60) << " minutes "
         << (seconds % 60) << " seconds "
         << (duration.count() % 1000000) << " microseconds." << endl;

    out.close();

    // Clean up memory (the destructor will be called automatically)
    delete root;  // This will recursively delete the entire tree

    cout<<"External sorting completed"<<endl;
}


bool middleGroupSort(const string &inputFile, const string &outputFile, const string &smallNumbersFile, const string &largeNumbersFile){

    ifstream input(inputFile, ios::binary);

    if (!input) {
        cerr << "Error opening file. "<< endl;
        return false;
    };

    input.seekg(0, ios::end);
    size_t fileSize = input.tellg();
    input.seekg(0, ios::beg);

    if (fileSize == 0) {
        input.close();
        remove(inputFile.c_str());
        return false;
    };

    ofstream outSmall(smallNumbersFile, ios::binary);
    ofstream outLarge(largeNumbersFile, ios::binary);
    ofstream outSorted(outputFile, ios::binary);

    vector<int> inputBuffer;
    vector<int> bufferSmall;
    vector<int> bufferLarge;

    bool remove_max_min_flag = false;

    size_t numElements = (MEMORY_SIZE - 2 * OUTPUT_BUFFER_SIZE - INPUT_BUFFER_SIZE) / sizeof(pair<int, int>);

    IntervalHeap intervalHeap;

    // Heap initialization
    while(!input.eof() && (intervalHeap.heap_size() < numElements)){
        if(inputBuffer.empty()){
            inputBuffer.resize(NUM_LIMIT_INPUT_BUFFER);
            input.read(reinterpret_cast<char*>(inputBuffer.data()), INPUT_BUFFER_SIZE);
            size_t bytesRead = input.gcount();
            size_t elementsRead = bytesRead / sizeof(int);
            inputBuffer.resize(elementsRead);

        };

        while(!inputBuffer.empty() && (intervalHeap.heap_size() < numElements) ){
            intervalHeap.insert(inputBuffer.back());
            inputBuffer.pop_back();

        };
    };

    // Process remaining data

    while (!input.eof() || !inputBuffer.empty()) {

        if (inputBuffer.empty()) {

            inputBuffer.resize(NUM_LIMIT_INPUT_BUFFER);
            input.read(reinterpret_cast<char*>(inputBuffer.data()), INPUT_BUFFER_SIZE);
            size_t bytesRead = input.gcount();
            size_t elementsRead = bytesRead / sizeof(int);
            inputBuffer.resize(elementsRead);

        };

        while(!inputBuffer.empty()){
            int a = inputBuffer.back();
            inputBuffer.pop_back();

            if (intervalHeap.get_max() <= a) {
                if (bufferLarge.size() >= NUM_LIMIT_OUTPUT_BUFFER) {
                    writeBufferToFile(outLarge, bufferLarge);
                };

                bufferLarge.push_back(a);

            } else if (intervalHeap.get_min() >= a) {
                if (bufferSmall.size() >= NUM_LIMIT_OUTPUT_BUFFER) {
                    writeBufferToFile(outSmall, bufferSmall);
                };

                bufferSmall.push_back(a);

            } else {

                if(remove_max_min_flag){
                    if (bufferSmall.size() >= NUM_LIMIT_OUTPUT_BUFFER) {
                        writeBufferToFile(outSmall, bufferSmall);
                    };
                    bufferSmall.push_back(intervalHeap.remove_min());
                    remove_max_min_flag = false;
                }else{
                    if (bufferLarge.size() >= NUM_LIMIT_OUTPUT_BUFFER) {
                        writeBufferToFile(outLarge, bufferLarge);
                    };
                    bufferLarge.push_back(intervalHeap.remove_max());
                    remove_max_min_flag = true;
                }

                intervalHeap.insert(a);
            }

        }

    }
    // small and large buffers write to the output files(flushing)
    if(bufferSmall.size()>0){
        writeBufferToFile(outSmall,bufferSmall);
    }
    if(bufferLarge.size()>0){
        writeBufferToFile(outLarge,bufferLarge);
    }

    // Write remaining data in the heap to the output file
    while (!intervalHeap.is_empty()) {
        int min_val = intervalHeap.remove_min();
        outSorted.write(reinterpret_cast<const char*>(&min_val), sizeof(int));
    }

    outSorted.close();
    outLarge.close();
    outSmall.close();
    input.close();

    return true;
}


bool readBinaryFile(const string& filename, vector<int>& data) {

    ifstream input(filename, ios::binary);
    if (!input) {
        cerr << "Error opening file: " << filename << endl;
        return false;
    }

    input.seekg(0, ios::end);
    size_t fileSize = input.tellg();
    input.seekg(0, ios::beg);

    size_t numElements = fileSize / sizeof(int);
    data.resize(numElements);
    input.read(reinterpret_cast<char*>(data.data()), fileSize);

    if (!input) {
        cerr << "Error occurred while reading file: " << filename << endl;
        return false;
    }

    input.close();
    return true;
}

bool fileExists(const string& filename) {
    ifstream file(filename);
    return file.good(); // true if file can be opened, false otherwise
}

void generateRandomNumbers(const string &filename) {

    cout << "Generating input file..." << endl;

    ofstream out(filename, ios::binary);

    srand(time(nullptr));
    for (size_t i = 0; i < NUM_COUNT; ++i) {
        int num = rand() % NUM_RANGE + 1;
        out.write(reinterpret_cast<const char *>(&num), sizeof(int));
    }

    out.close();

    cout<<"random numbers generated"<<endl;
}

void writeBufferToFile(ofstream &file, vector<int> &buffer) {

    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size() * sizeof(int));
    buffer.clear();

}



