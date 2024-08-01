#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <ctime>

using namespace std;


const int FILE_SIZE = 256 * 1024*1024 ; // 256 MB
const int NUM_RANGE = 1000000;
const int NUM_COUNT = FILE_SIZE / sizeof(int); // Number of integers to write

void generateRandomNumbers(const string &filename);
void readNumbers(const string &filename, vector<int> &numbers);


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

            if (right < n && heap[right].first < heap[smallest].first) {
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

    IntervalHeap ih;

    string inputFile = "random_numbers.bin";

    generateRandomNumbers(inputFile);

    vector<int> num2;

    readNumbers(inputFile,num2);

    cout<<"Interval heap is initializing...."<<endl;

    for(int i=0;i<num2.size();i++){
        ih.insert(num2[i]);
    }

    cout<<"Interval heap is initialized"<<endl;
    cout<<"Interval_heap_size: "<<ih.heap_size()<<endl;

    cout<<"Remove min and max elements until interval heap is empty."<<endl;
    cout<<"Elements removing...."<<endl;

    int b =INT_MIN,g,k;
    bool f =true;

    while(!ih.is_empty()){

        g=ih.get_min();
        k=ih.remove_min();
        ih.remove_max();

        if(b>g){
            cout<<"Something went wrong. b= "<<b<<" g= "<<g<<endl;
            f=false;
            break;
        }
        if(k!=g){
            cout<<"Something went wrong. k= "<<b<<" g= "<<g<<endl;
            f=false;
            break;
        }
        b=g;
    }
    if(f){
        cout<<"Completed. Interval_heap_size: "<<ih.heap_size()<<endl;
    }

    return 0;
}



void generateRandomNumbers(const string &filename) {

    cout<<"Random numbers generating...."<<endl;

    ofstream out(filename, ios::binary);
    srand(time(nullptr));
    for (size_t i = 0; i < NUM_COUNT; ++i) {
        int num = rand() % NUM_RANGE + 1;
        out.write(reinterpret_cast<const char *>(&num), sizeof(int));
    }
    out.close();

    cout<<"Random numbers generated"<<endl;
}

void readNumbers(const string &filename, vector<int> &numbers) {
    ifstream inFile(filename, ios::binary); // Open in binary mode
    if (!inFile) {
        cerr << "Error opening file for reading: " << filename << endl;
        return;
    }

    inFile.seekg(0, ios::end);
    size_t fileSize = inFile.tellg();
    inFile.seekg(0, ios::beg);

    size_t numElements = fileSize / sizeof(int);
    numbers.resize(numElements);
    inFile.read(reinterpret_cast<char*>(numbers.data()), fileSize);

    if (!inFile) {
        cerr << "Error occurred while reading from file: " << filename << endl;
    } else {
        cout << "Input file '" << filename << "' is successfully read." << endl;
    }

    inFile.close();
}
