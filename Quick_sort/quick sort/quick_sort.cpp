#include <iostream>

using namespace std;

void swap_two_elements(int* t, int* r);
int array_partition(int A[], int l, int h);
void quick_sort(int A[], int l, int h);



int main(){
    int A[] = {2, 1, 4, 3, 6, 3,10,9};
    int n = sizeof(A) / sizeof(int);

    quick_sort(A, 0, n - 1);

    for(int i = 0; i < n; i++){
        cout << A[i] << " ";
    }
    cout << endl;
    return 0;
}

void quick_sort(int A[], int l, int h){
    if (l < h) {
        int b = array_partition(A, l, h);
        quick_sort(A, l, b - 1);
        quick_sort(A, b + 1, h);
    }
}

void swap_two_elements(int* t, int* r){
    int temp = *r;
    *r = *t;
    *t = temp;
}

int array_partition(int A[], int l, int h){
    int pivot = A[l];
    int i = l, j = h;

    while (i < j){
        while (i <= h && A[i] <= pivot) {
            i++;
        }
        while (A[j] > pivot) {
            j--;
        }
        if (i < j){
            swap_two_elements(&A[i], &A[j]);
        }
    }
    swap_two_elements(&A[l], &A[j]);

    return j;
}
